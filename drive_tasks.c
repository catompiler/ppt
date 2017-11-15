#include "drive_tasks.h"
#include "list/list.h"
#include "utils/utils.h"
#include "utils/critical.h"
#include "settings.h"
#include "drive_temp.h"
#include "drive.h"
#include "drive_events.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>


//! Структура задач привода.
//typedef struct _Drive_Tasks {
//} drive_tasks_t;


//! Задачи привода.
//static drive_tasks_t tasks;



err_t drive_tasks_init(void)
{
    return E_NO_ERROR;
}

err_t drive_tasks_apply_settings(void)
{
    drive_task_settings_apply();
    return E_NO_ERROR;
}

err_t drive_tasks_save_settings(void)
{
    drive_task_settings_save();
    return E_NO_ERROR;
}

err_t drive_tasks_write_error_event(void)
{
    return drive_task_events_write(DRIVE_EVENT_TYPE_ERROR, true);
}

err_t drive_tasks_write_status_event(void)
{
    return drive_task_events_write(DRIVE_EVENT_TYPE_STATUS, true);
}

err_t drive_tasks_write_warning_event(void)
{
    return drive_task_events_write(DRIVE_EVENT_TYPE_WARNING, false);
}

err_t drive_tasks_read_event(future_t* future, drive_event_index_t event_index, drive_event_t* event)
{
    return drive_task_events_read(future, event_index, event);
}

err_t drive_tasks_read_osc_channel(future_t* future, drive_osc_index_t osc_index, size_t osc_channel)
{
    return drive_task_events_read_osc_channel(future, osc_index, osc_channel);
}

err_t drive_tasks_clear_events(void)
{
    return drive_task_events_clear();
}
