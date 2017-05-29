#include "drive_temp.h"
#include "defs/defs.h"
#include "stddef.h"
#include <string.h>
#include "timers/timers.h"
#include "settings.h"


//! Таймаут обмена данными по-умолчанию, мкс.
#define DRIVE_TEMP_TIMEOUT_DEFAULT_US 100000


//! Структура температуры привода.
typedef struct _Drive_Temp {
    lm75_t* heatsink_sensor; //!< Датчик температуры радиатора.
    drive_temp_sensor_reset_proc_t heatsink_sensor_reset; //!< Функция сброса датчика температуры радиатора.
    struct timeval heatsink_sensor_timeout; //!< Таймаут обмена данными с датчиком температуры радиатора.
    struct timeval update_interval; //!< Интервал обновления температуры.
    struct timeval update_time; //!< Время следующего обновления температуры.
    bool updated; //!< Флаг обновления температуры.
    
    bool heatsink_temp_avail; //!< Доступность температуры радиатора.
    fixed32_t heatsink_temp; //!< Температура радиатора.
    param_t* heatsink_temp_param; //!< Параметр для записи температуры радиатора.
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
    drive_temp.heatsink_sensor_reset = temp_is->heatsink_sensor_reset;
    
    if(temp_is->update_interval){
        memcpy(&drive_temp.update_interval, temp_is->update_interval, sizeof(struct timeval));
    }
    
    if(temp_is->heatsink_sensor_timeout){
        memcpy(&drive_temp.heatsink_sensor_timeout, temp_is->heatsink_sensor_timeout, sizeof(struct timeval));
    }else{
        drive_temp.heatsink_sensor_timeout.tv_sec = 0;
        drive_temp.heatsink_sensor_timeout.tv_usec = DRIVE_TEMP_TIMEOUT_DEFAULT_US;
    }
    
    drive_temp.heatsink_temp_param = settings_param_by_id(PARAM_ID_HEATSINK_TEMP);
    
    return E_NO_ERROR;
}

static void drive_temp_update_params(void)
{
    if(drive_temp.heatsink_temp_param){
        settings_param_set_valuef(drive_temp.heatsink_temp_param, drive_temp.heatsink_temp);
    }
}

static void* drive_temp_reset_sensor_task(void* arg_proc)
{
    if(arg_proc) ((drive_temp_sensor_reset_proc_t)arg_proc)();
    
    return NULL;
}

bool drive_temp_update(void)
{
    // Флаг успеха обновления.
    bool update_success = true;
    
    // Текущее время.
    struct timeval cur_time = {0};
        
    // Проверим таймаут.
    gettimeofday(&cur_time, NULL);
    
    // Если задан интервал обновления.
    if(drive_temp.updated && timerisset(&drive_temp.update_interval)){
        
        // Если время ещё не пришло.
        if(timercmp(&cur_time, &drive_temp.update_time, <)){
            // Возврат.
            return false;
        }
    }
    
    timer_id_t tid = INVALID_TIMER_ID;
    struct timeval timeout = {0, 0};
    
    fixed16_t temp16 = 0;
    
    if(drive_temp.updated || !drive_temp.heatsink_temp_avail){
        
        timeradd(&cur_time, &drive_temp.heatsink_sensor_timeout, &timeout);
        
        tid = timers_add_timer(drive_temp_reset_sensor_task, &timeout, NULL, (void*)drive_temp.heatsink_sensor_reset, NULL);
        
        if(lm75_read_temp(drive_temp.heatsink_sensor, &temp16) == E_NO_ERROR){
            drive_temp.heatsink_temp_avail = true;
            drive_temp.heatsink_temp = fixed16_to_32(temp16);
        }else{
            drive_temp.heatsink_temp_avail = false;
            update_success = false;
        }
        
        timers_remove_timer(tid);
    }
    
    if(update_success && timerisset(&drive_temp.update_interval)){
        if(!timerisset(&cur_time)){
            gettimeofday(&cur_time, NULL);
        }
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
