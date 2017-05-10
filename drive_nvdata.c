#include "drive_nvdata.h"
#include "nvdata.h"


//! Magic - контрольное значение валидности данных.
#define DRIVE_NVDATA_MAGIC 0xaa55
#define DRIVE_NVDATA_MAGIC_ADDRESS (NVDATA_SIZE - 2)

//! Общее время включения.
#define DRIVE_NVDATA_LIFETIME_ADDRESS 0
#define DRIVE_NVDATA_LIFETIME_SIZE 4

//! Время работы.
#define DRIVE_NVDATA_RUNTIME_ADDRESS 4
#define DRIVE_NVDATA_RUNTIME_SIZE 4

//! 
//#define DRIVE_NVDATA__ADDRESS 
//#define DRIVE_NVDATA__SIZE 

//! 
//#define DRIVE_NVDATA__ADDRESS 
//#define DRIVE_NVDATA__SIZE 

//! 
//#define DRIVE_NVDATA__ADDRESS 
//#define DRIVE_NVDATA__SIZE 

//! 
//#define DRIVE_NVDATA__ADDRESS 
//#define DRIVE_NVDATA__SIZE 


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
