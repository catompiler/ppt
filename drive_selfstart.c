#include "drive_selfstart.h"
#include <sys/time.h>
#include <stddef.h>
#include <string.h>
#include "settings.h"
#include "drive.h"


//! Состояние самозапуска.
typedef enum _Drive_Selfstart_State {
    DRIVE_SELFSTART_IDLE = 0,
    DRIVE_SELFSTART_START
} drive_selfstart_state_t;

/**
 * Структура самозапуска.
 */
typedef struct _Drive_Selfstart {
    bool enabled;
    struct timeval start_period;
    struct timeval start_delay;
    bool clear_errors_enabled;
    size_t clear_errors_attempts;
    struct timeval clear_errors_period;
    drive_selfstart_state_t state;
    struct timeval start_timeout;
    struct timeval delay_timeout;
    struct timeval clear_timeout;
    size_t clear_attempts;
} drive_selfstart_t;

//! Самозапуск.
static drive_selfstart_t selfstart;


void drive_selfstart_init(void)
{
    memset(&selfstart, 0x0, sizeof(drive_selfstart_t));
}

void drive_selfstart_update_settings(void)
{
    selfstart.enabled = settings_valueu(PARAM_ID_SELFSTART_ENABLED);
    
    selfstart.start_period.tv_sec = settings_valueu(PARAM_ID_SELFSTART_PERIOD) * 60;
    selfstart.start_period.tv_usec = 0;
    
    selfstart.start_delay.tv_sec = settings_valueu(PARAM_ID_SELFSTART_DELAY);
    selfstart.start_delay.tv_usec = 0;
    
    selfstart.clear_errors_enabled = settings_valueu(PARAM_ID_SELFSTART_CLEAR_ERRORS_ENABLED);
    
    selfstart.clear_errors_attempts = settings_valueu(PARAM_ID_SELFSTART_CLEAR_ERRORS_ATTEMPTS);
    
    selfstart.clear_errors_period.tv_sec = settings_valueu(PARAM_ID_SELFSTART_CLEAR_ERRORS_PERIOD);
    selfstart.clear_errors_period.tv_usec = 0;
    
    if(!selfstart.enabled) selfstart.state = DRIVE_SELFSTART_IDLE;
}

bool drive_selfstart_enabled(void)
{
    return selfstart.enabled;
}

void drive_selfstart_on_error(void)
{
    if(!selfstart.enabled) return;
    
    struct timeval cur_time;
    
    switch(selfstart.state){
        case DRIVE_SELFSTART_IDLE:
            // Получим текущее время.
            gettimeofday(&cur_time, NULL);
            // Если запуск требуется чаще чем раз в заданный период - выход.
            if(timercmp(&cur_time, &selfstart.start_timeout, <)) break;
            // Добавим задержку на самозапуск.
            timeradd(&cur_time, &selfstart.start_delay, &selfstart.delay_timeout);
            // Установим задержку на очистку ошибок.
            memcpy(&selfstart.clear_timeout, &selfstart.delay_timeout, sizeof(struct timeval));
            // Установим число попыток сброса ошибок.
            selfstart.clear_attempts = selfstart.clear_errors_attempts;
            // Установим состояние самозапуска.
            selfstart.state = DRIVE_SELFSTART_START;
            break;
        default:
            break;
    }
}

bool drive_selfstart_starting(void)
{
    if(!selfstart.enabled) return false;
    if(selfstart.state != DRIVE_SELFSTART_START) return false;
    
    return true;
}

bool drive_selfstart_can_clear_errors(void)
{
    if(!selfstart.enabled) return false;
    if(!selfstart.clear_errors_enabled) return false;
    if(selfstart.clear_attempts == 0) return false;
    if(selfstart.state != DRIVE_SELFSTART_START) return false;
    
    return true;
}

bool drive_selfstart_need_clear_errors(void)
{
    if(!selfstart.enabled) return false;
    if(!selfstart.clear_errors_enabled) return false;
    if(selfstart.clear_attempts == 0) return false;
    if(selfstart.state != DRIVE_SELFSTART_START) return false;
    
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);
    
    if(timercmp(&cur_time, &selfstart.clear_timeout, <)) return false;
    
    return true;
}

void drive_selfstart_errors_cleared(void)
{
    if(!selfstart.enabled) return;
    if(!selfstart.clear_errors_enabled) return;
    if(selfstart.state != DRIVE_SELFSTART_START) return;
    if(selfstart.clear_attempts == 0) return;
    
    selfstart.clear_attempts --;
    
    if(selfstart.clear_attempts != 0){
        struct timeval cur_time;
        gettimeofday(&cur_time, NULL);
        
        timeradd(&cur_time, &selfstart.clear_errors_period, &selfstart.clear_timeout);
    }
}

bool drive_selfstart_need_start(void)
{
    if(!selfstart.enabled) return false;
    if(selfstart.state != DRIVE_SELFSTART_START) return false;
    
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);
    
    if(timercmp(&cur_time, &selfstart.delay_timeout, <)) return false;
    
    return true;
}

void drive_selfstart_started(void)
{
    if(!selfstart.enabled) return;
    
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);
    
    timeradd(&cur_time, &selfstart.start_period, &selfstart.start_timeout);
}

void drive_selfstart_done(void)
{
    if(!selfstart.enabled) return;
    if(selfstart.state != DRIVE_SELFSTART_START) return;
    
    selfstart.state = DRIVE_SELFSTART_IDLE;
}
