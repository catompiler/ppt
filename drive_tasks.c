#include "drive_tasks.h"
#include "scheduler/scheduler.h"
#include "utils/utils.h"
#include "settings.h"
#include "drive.h"
#include "drive_events.h"
#include <stddef.h>
#include <stdint.h>

// Приоритет фоновой задачи.
//#define DRIVE_TASKS_MAIN_TASK_PRIOR 0
//! Приоритет задачи применения настроек.
#define DRIVE_TASKS_APPLY_SETTINGS_TASK_PRIOR 10
//! Приоритет задачи сохранения настроек.
#define DRIVE_TASKS_SAVE_SETTINGS_TASK_PRIOR 10
//! Приоритет задачи записи события ошибки.
#define DRIVE_TASKS_WRITE_ERROR_EVENT_TASK_PRIOR 100
//! Приоритет задачи записи события состояния.
#define DRIVE_TASKS_WRITE_STATUS_EVENT_TASK_PRIOR 10
//! Приоритет задачи чтения события ошибки.
#define DRIVE_TASKS_READ_ERROR_EVENT_TASK_PRIOR 10
//! Приоритет задачи чтения канала осциллограммы.
#define DRIVE_TASKS_READ_OSC_CHANNEL_TASK_PRIOR 10
//! Приоритет задачи очистки событий.
#define DRIVE_TASKS_CLEAR_EVENTS_TASK_PRIOR 10


#define TID_TO_ERR(T) ((T == INVALID_TASK_ID) ? E_INVALID_VALUE : E_NO_ERROR)


static void* apply_settings_task()
{
    drive_update_settings();
    return NULL;
}

err_t drive_tasks_apply_settings(void)
{
    return TID_TO_ERR(scheduler_add_task(apply_settings_task,
                DRIVE_TASKS_APPLY_SETTINGS_TASK_PRIOR, NULL, TASK_RUN_ONCE, NULL));
}

static void* save_settings_task()
{
    err_t err = E_NO_ERROR;
    
    do {
        err = settings_write();
    } while(err == E_BUSY);
    
    return int_to_pvoid(err);
}

err_t drive_tasks_save_settings(void)
{
    return TID_TO_ERR(scheduler_add_task(save_settings_task,
                DRIVE_TASKS_SAVE_SETTINGS_TASK_PRIOR, NULL, TASK_RUN_ONCE, NULL));
}

static void* drive_tasks_write_event_task(void* event)
{
    err_t err = E_NO_ERROR;
    
    do {
        err = drive_events_write_event((drive_event_t*)event);
    } while(err == E_BUSY);
    
    if(err != E_NO_ERROR){
        drive_power_oscillogram_resume();
        return int_to_pvoid(err);
    }
    
    while(!drive_power_oscillogram_paused());
    
    do {
        err = drive_events_write_current_oscillogram(((drive_event_t*)event)->id);
    } while(err == E_BUSY);
    
    drive_power_oscillogram_resume();
    
    if(err != E_NO_ERROR){
        return int_to_pvoid(err);
    }
    
    do {
        err = drive_events_write();
    } while(err == E_BUSY);
    
    return int_to_pvoid(err);
}

err_t drive_tasks_write_error_event(void)
{
    static drive_event_t event;
    drive_events_make_event(&event, DRIVE_EVENT_TYPE_ERROR);
    drive_power_oscillogram_half_pause();
    
    return TID_TO_ERR(scheduler_add_task(drive_tasks_write_event_task,
                DRIVE_TASKS_WRITE_ERROR_EVENT_TASK_PRIOR, &event, TASK_RUN_ONCE, NULL));
}

err_t drive_tasks_write_status_event(void)
{
    static drive_event_t event;
    drive_events_make_event(&event, DRIVE_EVENT_TYPE_STATUS);
    drive_power_oscillogram_pause();
    
    return TID_TO_ERR(scheduler_add_task(drive_tasks_write_event_task,
                DRIVE_TASKS_WRITE_STATUS_EVENT_TASK_PRIOR, &event, TASK_RUN_ONCE, NULL));
}

void* drive_tasks_read_event_task(void* event)
{
    err_t err = E_NO_ERROR;
    do {
        err = drive_events_read_event((drive_event_t*)event, ((drive_event_t*)event)->id);
    } while(err == E_BUSY);
    
    return int_to_pvoid(err);
}

err_t drive_tasks_read_event(future_t* future, drive_event_index_t event_index, drive_event_t* event)
{
    event->id = event_index;
    
    return TID_TO_ERR(scheduler_add_task(drive_tasks_read_event_task,
                DRIVE_TASKS_READ_ERROR_EVENT_TASK_PRIOR, event, TASK_RUN_ONCE, future));
}

void* drive_tasks_read_osc_task(void* data)
{
    size_t osc_channel = (size_t)((uint32_t)data & 0xffff);
    drive_osc_index_t osc_index = (drive_osc_index_t)(((uint32_t)data >> 16) & 0xffff);
    err_t err = E_NO_ERROR;
    do {
        err = drive_events_read_osc_channel(osc_index, osc_channel);
    } while(err == E_BUSY);
    
    return int_to_pvoid(err);
}

err_t drive_tasks_read_osc_channel(future_t* future, drive_osc_index_t osc_index, size_t osc_channel)
{
    uint32_t packed_arg = ((uint32_t)osc_index << 16) | ((uint32_t)osc_channel & 0xffff);
    return TID_TO_ERR(scheduler_add_task(drive_tasks_read_osc_task,
                DRIVE_TASKS_READ_OSC_CHANNEL_TASK_PRIOR, (void*)packed_arg, TASK_RUN_ONCE, future));
}

static void* clear_events_task()
{
    drive_events_reset();
    
    err_t err = E_NO_ERROR;
    do {
        err = drive_events_write();
    } while(err == E_BUSY);
    
    return int_to_pvoid(err);
}

err_t drive_tasks_clear_events(void)
{
    return TID_TO_ERR(scheduler_add_task(clear_events_task,
                DRIVE_TASKS_CLEAR_EVENTS_TASK_PRIOR, NULL, TASK_RUN_ONCE, NULL));
}
