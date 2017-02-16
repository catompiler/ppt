#include "drive_tasks.h"
#include "list/list.h"
#include "scheduler/scheduler.h"
#include "utils/utils.h"
#include "utils/critical.h"
#include "settings.h"
#include "drive.h"
#include "drive_events.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>


// Приоритет фоновой задачи.
//#define DRIVE_TASKS_MAIN_TASK_PRIOR 0
//! Приоритет задачи применения настроек.
#define DRIVE_TASKS_APPLY_SETTINGS_TASK_PRIOR 10
//! Приоритет задачи сохранения настроек.
#define DRIVE_TASKS_SAVE_SETTINGS_TASK_PRIOR 10
//! Приоритет задачи записи события ошибки.
#define DRIVE_TASKS_WRITE_ERROR_EVENT_TASK_PRIOR 100
//! Приоритет задачи записи события предупреждения.
#define DRIVE_TASKS_WRITE_WARNING_EVENT_TASK_PRIOR 80
//! Приоритет задачи записи события состояния.
#define DRIVE_TASKS_WRITE_STATUS_EVENT_TASK_PRIOR 10
//! Приоритет задачи чтения события ошибки.
#define DRIVE_TASKS_READ_ERROR_EVENT_TASK_PRIOR 10
//! Приоритет задачи чтения канала осциллограммы.
#define DRIVE_TASKS_READ_OSC_CHANNEL_TASK_PRIOR 10
//! Приоритет задачи очистки событий.
#define DRIVE_TASKS_CLEAR_EVENTS_TASK_PRIOR 10


#define TID_TO_ERR(T) ((T == INVALID_TASK_ID) ? E_INVALID_VALUE : E_NO_ERROR)


//! Максимальное число записываемых за один раз событий.
#define DRIVE_TASKS_EVENTS_MAX 5


//! Элемент списка событий.
typedef struct _Drive_Tasks_Events_Item {
    list_item_t item; //!< Элемент списка.
    drive_event_t event; //!< Событие.
    bool used; //!< Флаг использования.
} drive_tasks_event_item_t;

//! Структура задач привода.
typedef struct _Drive_Tasks {
    drive_tasks_event_item_t events_items[DRIVE_TASKS_EVENTS_MAX]; //!< События для записи.
    list_t events_list; //!< Очередь событий.
} drive_tasks_t;


//! Задачи привода.
static drive_tasks_t tasks;



err_t drive_tasks_init(void)
{
    list_init(&tasks.events_list);
    
    size_t i;
    for(i = 0; i < DRIVE_TASKS_EVENTS_MAX; i ++){
        tasks.events_items[i].used = false;
        memset(&tasks.events_items[i].event, 0x0, sizeof(drive_event_t));
        list_item_init_data(&tasks.events_items[i].item, &tasks.events_items[i]);
        list_append(&tasks.events_list, &tasks.events_items[i].item);
    }
    
    return E_NO_ERROR;
}

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

static drive_tasks_event_item_t* drive_tasks_get_event_item(void)
{
    list_item_t* item = NULL;
    drive_tasks_event_item_t* event_item = NULL;
    
    CRITICAL_ENTER();
    
    item = list_first(&tasks.events_list);
    event_item = (drive_tasks_event_item_t*)list_item_data(item);
    
    if(event_item->used){
        CRITICAL_EXIT();
        return NULL;
    }
    
    list_remove(&tasks.events_list, item);
    list_append(&tasks.events_list, item);
    
    CRITICAL_EXIT();
    
    return event_item;
}

static void drive_tasks_release_event_item(drive_tasks_event_item_t* event_item)
{
    memset(&event_item->event, 0x0, sizeof(drive_event_t));
    
    list_item_t* item = &event_item->item;
    
    CRITICAL_ENTER();
    
    event_item->used = false;
    
    list_remove(&tasks.events_list, item);
    list_prepend(&tasks.events_list, item);
    
    CRITICAL_EXIT();
}

static void* drive_tasks_write_event_osc_task(void* event_item_arg)
{
    drive_tasks_event_item_t* event_item = (drive_tasks_event_item_t*)event_item_arg;
    drive_event_t* event = &event_item->event;
    
    err_t err = E_NO_ERROR;
    
    do {
        err = drive_events_write_event(event);
    } while(err == E_BUSY);
    
    if(err != E_NO_ERROR){
        drive_power_oscillogram_resume();
        drive_tasks_release_event_item(event_item);
        return int_to_pvoid(err);
    }
    
    while(!drive_power_oscillogram_paused());
    
    do {
        err = drive_events_write_current_oscillogram(event->id);
    } while(err == E_BUSY);
    
    drive_power_oscillogram_resume();
    
    if(err != E_NO_ERROR){
        drive_tasks_release_event_item(event_item);
        return int_to_pvoid(err);
    }
    
    do {
        err = drive_events_write();
    } while(err == E_BUSY);
    
    drive_tasks_release_event_item(event_item);
    return int_to_pvoid(err);
}

static void* drive_tasks_write_event_task(void* event_item_arg)
{
    drive_tasks_event_item_t* event_item = (drive_tasks_event_item_t*)event_item_arg;
    drive_event_t* event = &event_item->event;
    
    err_t err = E_NO_ERROR;
    
    do {
        err = drive_events_write_event((drive_event_t*)event);
    } while(err == E_BUSY);
    
    if(err != E_NO_ERROR){
        drive_tasks_release_event_item(event_item);
        return int_to_pvoid(err);
    }
    
    do {
        err = drive_events_write();
    } while(err == E_BUSY);
    
    drive_tasks_release_event_item(event_item);
    return int_to_pvoid(err);
}

static err_t drive_tasks_write_event(bool need_osc, drive_event_type_t type, task_priority_t priority)
{
    drive_tasks_event_item_t* event_item = drive_tasks_get_event_item();
    
    if(event_item == NULL) return E_OUT_OF_MEMORY;
    
    task_proc_t task_proc = drive_tasks_write_event_task;
    
    if(need_osc){
        CRITICAL_ENTER();
        if(!drive_power_oscillogram_is_paused()){
            drive_power_oscillogram_half_pause();
            task_proc = drive_tasks_write_event_osc_task;
        }
        CRITICAL_EXIT();
    }
    
    drive_events_make_event(&event_item->event, type);
    
    return TID_TO_ERR(scheduler_add_task(task_proc, priority, (void*)event_item, TASK_RUN_ONCE, NULL));
}

err_t drive_tasks_write_error_event(void)
{
    return drive_tasks_write_event(true, DRIVE_EVENT_TYPE_ERROR, DRIVE_TASKS_WRITE_ERROR_EVENT_TASK_PRIOR);
}

err_t drive_tasks_write_status_event(void)
{
    return drive_tasks_write_event(true, DRIVE_EVENT_TYPE_STATUS, DRIVE_TASKS_WRITE_STATUS_EVENT_TASK_PRIOR);
}

err_t drive_tasks_write_warning_event(void)
{
    return drive_tasks_write_event(false, DRIVE_EVENT_TYPE_WARNING, DRIVE_TASKS_WRITE_WARNING_EVENT_TASK_PRIOR);
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
