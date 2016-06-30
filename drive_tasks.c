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
    return int_to_pvoid(settings_write());
}

err_t drive_tasks_save_settings(void)
{
    return TID_TO_ERR(scheduler_add_task(save_settings_task,
                DRIVE_TASKS_SAVE_SETTINGS_TASK_PRIOR, NULL, TASK_RUN_ONCE, NULL));
}

static void* drive_tasks_write_error_event_task(void* event)
{
    err_t err = drive_events_write_event((drive_event_t*)event);
    if(err != E_NO_ERROR){
        drive_power_oscillograms_resume();
        return int_to_pvoid(err);
    }
    
    while(!drive_power_oscillograms_paused());
    
    err = drive_events_write_current_oscillograms(((drive_event_t*)event)->id);
    if(err != E_NO_ERROR){
        drive_power_oscillograms_resume();
        return int_to_pvoid(err);
    }
    
    drive_power_oscillograms_resume();
    
    err = drive_events_write();
    if(err != E_NO_ERROR) return int_to_pvoid(err);
    
    return int_to_pvoid(err);
}

err_t drive_tasks_write_error_event(void)
{
    static drive_event_t event;
    drive_events_make_event(&event, DRIVE_EVENT_TYPE_ERROR);
    drive_power_oscillograms_half_pause();
    
    return TID_TO_ERR(scheduler_add_task(drive_tasks_write_error_event_task,
                DRIVE_TASKS_WRITE_ERROR_EVENT_TASK_PRIOR, &event, TASK_RUN_ONCE, NULL));
}

