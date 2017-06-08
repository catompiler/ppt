#include "drive_nvdata.h"
#include "nvdata.h"
#include <string.h>


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


//! Структура энергонезависимых данных привода.
typedef struct _Drive_Nvdata {
    uint8_t fan_runtime_fract; //! Дробная часть времени работы вентилятора.
} drive_nvdata_t;

//! Энергонезависимые данные привода.
static drive_nvdata_t nvdata;



void drive_nvdata_init(void)
{
    memset(&nvdata, 0x0, sizeof(drive_nvdata_t));
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
