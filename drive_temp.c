#include "drive_temp.h"
#include "defs/defs.h"
#include "stddef.h"
#include <string.h>
#include "drive_task_i2c_watchdog.h"
#include "settings.h"
#include "stm32f10x.h"
#include "drive.h"
#include "utils/utils.h"


//! Таймаут обмена данными по-умолчанию, мкс.
#define DRIVE_TEMP_IO_TIMEOUT_DEFAULT_US 100000

//! Таймаут попыток обмена данными по-умолчанию, мкс.
#define DRIVE_TEMP_TIMEOUT_DEFAULT_S 1

//! Обороты останова вентилятора.
#define DRIVE_TEMP_FAN_RPM_STOP 0
//! Максимальные обороты вентилятора.
#define DRIVE_TEMP_FAN_RPM_MAX 100


//! Структура температуры привода.
typedef struct _Drive_Temp {
    lm75_t* heatsink_sensor; //!< Датчик температуры радиатора.
    drive_temp_sensor_reset_proc_t heatsink_sensor_reset; //!< Функция сброса датчика температуры радиатора.
    struct timeval heatsink_sensor_io_timeout; //!< Таймаут обмена данными с датчиком температуры радиатора.
    uint32_t heatsink_sensor_i2c_watchdog; //!< Сторож i2c.
    struct timeval heatsink_sensor_timeout; //!< Таймаут попыток обмена данными с датчиком температуры радиатора.
    
    struct timeval update_interval; //!< Интервал обновления температуры.
    struct timeval update_time; //!< Время следующего обновления температуры.
    bool updated; //!< Флаг обновления температуры.
    
    drive_temp_set_fan_rpm_proc_t set_fan_rpm; //!< Функция установки оборотов вентилятора.
    struct timeval last_run_time; //!< Последнее время работы привода.
    bool fan_enable; //!< Разрешение контроля вентиляторов.
    bool fan_eco_mode; //!< Флаг эко-режима вентилятора.
    uint16_t fan_rpm_min; //!< Минимальные обороты вентилятора.
    fixed32_t fan_temp_min; //!< Темперетура включения вентилятора.
    fixed32_t fan_temp_max; //!< Температура максимальных оборотов вентилятора.
    uint16_t fan_eco_time; //!< Время работы вентилятора в эко-режиме.
    uint16_t fan_rpm; //!< Обороты вентилятора, %.
    
    bool heatsink_temp_avail; //!< Доступность температуры радиатора.
    struct timeval heatsink_temp_time; //!< Время последнего чтения температуры радиатора.
    fixed32_t heatsink_temp; //!< Температура радиатора.
    param_t* heatsink_temp_param; //!< Параметр для записи температуры радиатора.
    param_t* heatsink_fan_rpm_param; //!< Параметр для записи оборотов вентилятора.
} drive_temp_t;

//! Температура привода.
static drive_temp_t drive_temp;



ALWAYS_INLINE static fixed32_t fixed16_to_32(fixed16_t f)
{
    return ((fixed32_t)f) << 8;
}


err_t drive_temp_init(drive_temp_init_t* temp_is)
{
    if(temp_is == NULL) return E_NULL_POINTER;
    if(temp_is->heatsink_sensor == NULL) return E_NULL_POINTER;
    
    memset(&drive_temp, 0x0, sizeof(drive_temp_t));
    
    drive_temp.heatsink_sensor = temp_is->heatsink_sensor;
    drive_temp.heatsink_sensor_reset = temp_is->heatsink_sensor_reset_proc;
    
    if(temp_is->update_interval){
        memcpy(&drive_temp.update_interval, temp_is->update_interval, sizeof(struct timeval));
    }
    
    if(temp_is->heatsink_sensor_io_timeout){
        memcpy(&drive_temp.heatsink_sensor_io_timeout, temp_is->heatsink_sensor_io_timeout, sizeof(struct timeval));
    }else{
        drive_temp.heatsink_sensor_io_timeout.tv_sec = 0;
        drive_temp.heatsink_sensor_io_timeout.tv_usec = DRIVE_TEMP_IO_TIMEOUT_DEFAULT_US;
    }
    
    drive_temp.heatsink_sensor_i2c_watchdog = temp_is->heatsink_sensor_i2c_watchdog;
    
    if(temp_is->heatsink_sensor_timeout){
        memcpy(&drive_temp.heatsink_sensor_timeout, temp_is->heatsink_sensor_timeout, sizeof(struct timeval));
    }else{
        drive_temp.heatsink_sensor_timeout.tv_sec = DRIVE_TEMP_TIMEOUT_DEFAULT_S;
        drive_temp.heatsink_sensor_timeout.tv_usec = 0;
    }
    
    drive_temp.set_fan_rpm = temp_is->set_fan_rpm_proc;
    
    drive_temp_update_settings();
    
    drive_temp.heatsink_temp_param = settings_param_by_id(PARAM_ID_HEATSINK_TEMP);
    drive_temp.heatsink_fan_rpm_param = settings_param_by_id(PARAM_ID_HEATSINK_FAN_RPM);
    
    return E_NO_ERROR;
}

void drive_temp_update_settings(void)
{
    drive_temp.fan_enable = settings_valueu(PARAM_ID_FAN_CONTROL_ENABLE);
    drive_temp.fan_temp_min = settings_valuef(PARAM_ID_FAN_TEMP_MIN);
    drive_temp.fan_temp_max = settings_valuef(PARAM_ID_FAN_TEMP_MAX);
    drive_temp.fan_rpm_min = settings_valueu(PARAM_ID_FAN_RPM_MIN);
    drive_temp.fan_eco_mode = settings_valueu(PARAM_ID_FAN_ECO_MODE_ENABLE);
    drive_temp.fan_eco_time = settings_valueu(PARAM_ID_FAN_ECO_COOLING_TIME);
}

void drive_temp_regulate_fan(void)
{
    if(!drive_temp.set_fan_rpm) return;
    
    uint32_t rpm = DRIVE_TEMP_FAN_RPM_STOP;
    
    if(drive_temp.fan_enable){
        
        struct timeval cur_tv = {0};
        gettimeofday(&cur_tv, NULL);

        struct timeval diff_tv = {0};
    
        if(drive_running() || !drive_temp.fan_eco_mode){
            
            timersub(&cur_tv, &drive_temp.heatsink_temp_time, &diff_tv);

            if(!drive_temp.heatsink_temp_avail && timercmp(&diff_tv, &drive_temp.heatsink_sensor_timeout, >)){
                rpm = DRIVE_TEMP_FAN_RPM_MAX;
            }else if(drive_temp.heatsink_temp <= drive_temp.fan_temp_min){
                rpm = drive_temp.fan_rpm_min;
            }else if(drive_temp.heatsink_temp >= drive_temp.fan_temp_max){
                rpm = DRIVE_TEMP_FAN_RPM_MAX;
            }else{
                fixed32_t cur_diff = drive_temp.heatsink_temp - drive_temp.fan_temp_min;
                fixed32_t temp_diff = drive_temp.fan_temp_max - drive_temp.fan_temp_min;
                uint32_t rpm_diff = DRIVE_TEMP_FAN_RPM_MAX - drive_temp.fan_rpm_min;

                fixed32_t rpmf = cur_diff * rpm_diff;
                rpmf = fixed32_div((int64_t)rpmf, temp_diff);
                rpmf = fixed32_round(rpmf);

                rpm = (uint32_t)drive_temp.fan_rpm_min + fixed32_get_int(rpmf);

                rpm = CLAMP(rpm, drive_temp.fan_rpm_min, DRIVE_TEMP_FAN_RPM_MAX);
            }
        }else{// eco mode
            timersub(&cur_tv, &drive_temp.last_run_time, &diff_tv);

            struct timeval eco_tv = {drive_temp.fan_eco_time, 0};

            if(timercmp(&diff_tv, &eco_tv, >=)){
                rpm = DRIVE_TEMP_FAN_RPM_STOP;
            }else{
                rpm = drive_temp.fan_rpm_min;
            }
        }
    
    }
    
    drive_temp.fan_rpm = rpm;
    
    drive_temp.set_fan_rpm(rpm);
}

static void drive_temp_update_params(void)
{
    if(drive_temp.heatsink_temp_param){
        settings_param_set_valuef(drive_temp.heatsink_temp_param, drive_temp.heatsink_temp);
    }
    if(drive_temp.heatsink_fan_rpm_param){
        settings_param_set_valueu(drive_temp.heatsink_fan_rpm_param, drive_temp.fan_rpm);
    }
}

bool drive_temp_update(void)
{
    // Флаг успеха обновления.
    bool update_success = true;
    
    // Текущее время.
    struct timeval cur_time = {0};
        
    // Проверим таймаут.
    gettimeofday(&cur_time, NULL);
    
    if(drive_running()){
        drive_temp.last_run_time.tv_sec = cur_time.tv_sec;
        drive_temp.last_run_time.tv_usec = cur_time.tv_usec;
    }
    
    // Если задан интервал обновления.
    if(drive_temp.updated && timerisset(&drive_temp.update_interval)){
        
        // Если время ещё не пришло.
        if(timercmp(&cur_time, &drive_temp.update_time, <)){
            // Возврат.
            return false;
        }
    }
    
    fixed16_t temp16 = 0;
    
    if(drive_temp.updated || !drive_temp.heatsink_temp_avail){
        
        uint32_t timeout_ms = drive_temp.heatsink_sensor_io_timeout.tv_sec * 1000 +
                drive_temp.heatsink_sensor_io_timeout.tv_usec / 1000;
        
        drive_task_i2c_watchdog_start(drive_temp.heatsink_sensor_i2c_watchdog, timeout_ms);
        
        if(lm75_read_temp(drive_temp.heatsink_sensor, &temp16) == E_NO_ERROR){
            drive_temp.heatsink_temp_avail = true;
            drive_temp.heatsink_temp = fixed16_to_32(temp16);
            memcpy(&drive_temp.heatsink_temp_time, &cur_time, sizeof(struct timeval));
        }else{
            drive_temp.heatsink_temp_avail = false;
            update_success = false;
        }
        
        drive_task_i2c_watchdog_stop(drive_temp.heatsink_sensor_i2c_watchdog);
    }
    
    if(update_success && timerisset(&drive_temp.update_interval)){
        timeradd(&cur_time, &drive_temp.update_interval, &drive_temp.update_time);
    }
    
    drive_temp.updated = update_success;
    
    drive_temp_update_params();
    
    return update_success;
}

bool drive_temp_heatsink_temp_avail(void)
{
    return drive_temp.heatsink_temp_avail;
}

fixed32_t drive_temp_heatsink_temp(void)
{
    return drive_temp.heatsink_temp;
}

uint32_t drive_temp_fan_rpm(void)
{
    return drive_temp.fan_rpm;
}

bool drive_temp_fan_running(void)
{
    return drive_temp.fan_rpm != DRIVE_TEMP_FAN_RPM_STOP;
}
