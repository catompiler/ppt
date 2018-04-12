#include "drive_nvdata.h"
#include "nvdata.h"
#include <string.h>
#include "settings.h"
#include "storage.h"
#include "crc/crc16_ccitt.h"


//! Magic - контрольное значение валидности данных.
#define DRIVE_NVDATA_MAGIC 0xaa55
#define DRIVE_NVDATA_MAGIC_ADDRESS (NVDATA_SIZE - 2)

//! Общее время включения.
#define DRIVE_NVDATA_LIFETIME_ADDRESS 0
#define DRIVE_NVDATA_LIFETIME_SIZE 4

//! Время работы.
#define DRIVE_NVDATA_RUNTIME_ADDRESS 4
#define DRIVE_NVDATA_RUNTIME_SIZE 4

//! Время работы вентилятора.
#define DRIVE_NVDATA_FAN_RUNTIME_ADDRESS 8
#define DRIVE_NVDATA_FAN_RUNTIME_SIZE 4

//! 
//#define DRIVE_NVDATA__ADDRESS 
//#define DRIVE_NVDATA__SIZE 

//! 
//#define DRIVE_NVDATA__ADDRESS 
//#define DRIVE_NVDATA__SIZE 

//! 
//#define DRIVE_NVDATA__ADDRESS 
//#define DRIVE_NVDATA__SIZE 

#pragma pack(push, 1)
typedef struct _Drive_Nvdata_Backup {
    uint32_t lifetime;
    uint32_t runtime;
    uint32_t fan_runtime;
    uint16_t crc;
} drive_nvdata_backup_t;
#pragma pack(pop)


//! Структура энергонезависимых данных привода.
typedef struct _Drive_Nvdata {
    uint8_t fan_runtime_fract; //!< Дробная часть времени работы вентилятора.
    time_t last_run_time; //!< Время работы после последнего включения.
    
    // Параметры.
    param_t* param_lifetime; //!< Время включения.
    param_t* param_runtime; //!< Время работы.
    param_t* param_fan_runtime; //!< Время работы вентилятора.
    param_t* param_last_runtime; //!< Время работы после включения.
} drive_nvdata_t;

//! Энергонезависимые данные привода.
static drive_nvdata_t nvdata;



void drive_nvdata_init(void)
{
    memset(&nvdata, 0x0, sizeof(drive_nvdata_t));
    
    nvdata.param_lifetime = settings_param_by_id(PARAM_ID_LIFETIME);
    nvdata.param_runtime = settings_param_by_id(PARAM_ID_RUNTIME);
    nvdata.param_fan_runtime = settings_param_by_id(PARAM_ID_FAN_RUNTIME);
    nvdata.param_last_runtime = settings_param_by_id(PARAM_ID_LAST_RUNTIME);
}

err_t drive_nvdata_read(void)
{
    drive_nvdata_backup_t data;
    
    err_t err = storage_read(STORAGE_RGN_NVDATA_ADDRESS, &data, sizeof(drive_nvdata_backup_t));
    if(err != E_NO_ERROR) return err;
    
    uint16_t crc = crc16_ccitt(&data, sizeof(drive_nvdata_backup_t) - sizeof(uint16_t));
    if(crc != data.crc) return E_CRC;
    
    drive_nvdata_set_lifetime(data.lifetime);
    drive_nvdata_set_runtime(data.runtime);
    drive_nvdata_set_fan_runtime(data.fan_runtime);
    
    return E_NO_ERROR;
}

err_t drive_nvdata_write(void)
{
    drive_nvdata_backup_t data;
    
    data.lifetime = drive_nvdata_lifetime();
    data.runtime = drive_nvdata_runtime();
    data.fan_runtime = drive_nvdata_fan_runtime();
    data.crc = crc16_ccitt(&data, sizeof(drive_nvdata_backup_t) - sizeof(uint16_t));
    
    return storage_write(STORAGE_RGN_NVDATA_ADDRESS, &data, sizeof(drive_nvdata_backup_t));
}

bool drive_nvdata_valid(void)
{
    uint16_t magic = 0;
    if(nvdata_get_half_word(DRIVE_NVDATA_MAGIC_ADDRESS, &magic) != E_NO_ERROR){
        return false;
    }
    
    return magic == DRIVE_NVDATA_MAGIC;
}

void drive_nvdata_clear(void)
{
    nvdata_clear();
    nvdata_put_half_word(DRIVE_NVDATA_MAGIC_ADDRESS, DRIVE_NVDATA_MAGIC);
}

void drive_nvdata_update_time_params(void)
{
    DRIVE_UPDATE_PARAM_UINT(nvdata.param_lifetime, drive_nvdata_lifetime() / 3600);
    DRIVE_UPDATE_PARAM_UINT(nvdata.param_runtime, drive_nvdata_runtime() / 3600);
    DRIVE_UPDATE_PARAM_UINT(nvdata.param_fan_runtime, drive_nvdata_fan_runtime() / 3600);
    DRIVE_UPDATE_PARAM_UINT(nvdata.param_last_runtime, drive_nvdata_last_runtime() / 3600);
}

time_t drive_nvdata_lifetime(void)
{
    uint32_t lifetime = 0;
    
    if(nvdata_get_word(DRIVE_NVDATA_LIFETIME_ADDRESS, &lifetime) != E_NO_ERROR){
        return 0;
    }
    
    return (time_t)lifetime;
}

void drive_nvdata_set_lifetime(time_t lifetime)
{
    nvdata_put_word(DRIVE_NVDATA_LIFETIME_ADDRESS, (uint32_t)lifetime);
}

void drive_nvdata_inc_lifetime(void)
{
    drive_nvdata_set_lifetime(drive_nvdata_lifetime() + 1);
}

time_t drive_nvdata_runtime(void)
{
    uint32_t runtime = 0;
    
    if(nvdata_get_word(DRIVE_NVDATA_RUNTIME_ADDRESS, &runtime) != E_NO_ERROR){
        return 0;
    }
    
    return (time_t)runtime;
}

void drive_nvdata_set_runtime(time_t runtime)
{
    nvdata_put_word(DRIVE_NVDATA_RUNTIME_ADDRESS, (uint32_t)runtime);
}

void drive_nvdata_inc_runtime(void)
{
    drive_nvdata_set_runtime(drive_nvdata_runtime() + 1);
}

time_t drive_nvdata_last_runtime(void)
{
    return nvdata.last_run_time;
}

void drive_nvdata_set_last_runtime(time_t runtime)
{
    nvdata.last_run_time = runtime;
}

void drive_nvdata_inc_last_runtime(void)
{
    nvdata.last_run_time ++;
}

time_t drive_nvdata_fan_runtime(void)
{
    uint32_t runtime = 0;
    
    if(nvdata_get_word(DRIVE_NVDATA_FAN_RUNTIME_ADDRESS, &runtime) != E_NO_ERROR){
        return 0;
    }
    
    return (time_t)runtime;
}

void drive_nvdata_set_fan_runtime(time_t runtime)
{
    nvdata_put_word(DRIVE_NVDATA_FAN_RUNTIME_ADDRESS, (uint32_t)runtime);
}

void drive_nvdata_inc_fan_runtime(uint32_t rpm_percents)
{
    nvdata.fan_runtime_fract += (uint8_t)rpm_percents;
    if(nvdata.fan_runtime_fract >= 100){
        nvdata.fan_runtime_fract -= 100;
        drive_nvdata_set_fan_runtime(drive_nvdata_fan_runtime() + 1);
    }
}

void drive_nvdata_reset_fan_runtime(void)
{
    nvdata.fan_runtime_fract = 0;
    drive_nvdata_set_fan_runtime(0);
}
