#include "drive_task_storage.h"
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <queue.h>
#include <stddef.h>
#include <string.h>
#include "drive_events.h"
#include "drive_power.h"
#include "utils/utils.h"
#include "settings.h"
#include "drive_nvdata.h"


#define TASK_STORAGE_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define QUEUE_STORAGE_SIZE 4

#define STORAGE_WAIT 0


// Команды задачи.
//! Запись события.
#define TASK_STORAGE_CMD_WRITE_EVENT 0
//! Запись события с осциллограммой.
#define TASK_STORAGE_CMD_WRITE_OSC 1
//! Чтение события.
#define TASK_STORAGE_CMD_READ_EVENT 2
//! Чтение осциллограммы.
#define TASK_STORAGE_CMD_READ_OSC 3
//! Очистка событий.
#define TASK_STORAGE_CMD_CLEAR_EVENTS 4
//! Сохранение настроек.
#define TASK_STORAGE_CMD_SAVE_SETTINGS 5
//! Сохранение энергонезависимых данных.
#define TASK_STORAGE_CMD_SAVE_NVDATA 6
//! .
//#define TASK_STORAGE_CMD_ 0

// Структуры команд.
//! Запись события.
typedef struct _Write_Event_Cmd {
    drive_event_t event; //!< Событие.
} write_event_cmd_t;
//! Запись события с осциллограммой.
typedef struct _Write_Event_Ocs_Cmd {
    drive_event_t event; //!< Событие.
} write_event_osc_cmd_t;
//! Чтение события.
typedef struct _Read_Event_Cmd {
    future_t* future; //!< Будущее.
    drive_event_index_t index; //!< Индекс события.
    drive_event_t* event; //!< Буфер для события.
} read_event_cmd_t;
//! Чтение осциллограммы.
typedef struct _Read_Osc_Cmd {
    future_t* future; //!< Будущее.
    drive_osc_index_t index; //!< Индекс осциллограммы.
    uint8_t channel; //!< Канал осциллограммы.
} read_osc_cmd_t;
//! Очистка событий.
typedef struct _Clear_Events_Cmd {
    uint8_t reserved; //!< Зарезервировано.
} clear_events_cmd_t;
//! Сохранение настроек.
typedef struct _Save_Settings_Cmd {
    uint8_t reserved; //!< Зарезервировано.
} save_settings_cmd_t;
//! Сохранение энергонезависимых данных.
typedef struct _Save_Nvdata_Cmd {
    uint8_t reserved; //!< Зарезервировано.
} save_nvdata_cmd_t;

//! Структура элемента очереди записи событий.
typedef struct _Task_Cmd {
    uint8_t type; //!< Тип команды.
    union {
        write_event_cmd_t write_event;
        write_event_osc_cmd_t write_event_osc;
        read_event_cmd_t read_event;
        read_osc_cmd_t read_osc;
        clear_events_cmd_t clear_events;
        save_settings_cmd_t save_settings;
        save_nvdata_cmd_t save_nvdata;
    };
} task_storage_cmd_t;

typedef struct _Task_Storage {
    // Задача.
    StackType_t task_stack[TASK_STORAGE_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
    // Очередь.
    task_storage_cmd_t queue_storage[QUEUE_STORAGE_SIZE]; //!< Данные очереди.
    StaticQueue_t queue_buffer; //!< Буфер очереди.
    QueueHandle_t queue_handle; //!< Идентификатор очереди.
} task_storage_t;


static task_storage_t storage_task;



static void storage_task_proc(void*);

err_t drive_task_storage_init(uint32_t priority)
{
    memset(&storage_task, 0x0, sizeof(task_storage_t));
    
    storage_task.task_handle = xTaskCreateStatic(storage_task_proc, "storage_task",
            TASK_STORAGE_STACK_SIZE, NULL, priority, storage_task.task_stack, &storage_task.task_buffer);
    
    if(storage_task.task_handle == NULL) return E_INVALID_VALUE;
    
    
    storage_task.queue_handle = xQueueCreateStatic(QUEUE_STORAGE_SIZE, sizeof(task_storage_cmd_t),
                                              (uint8_t*)storage_task.queue_storage, &storage_task.queue_buffer);
    
    if(storage_task.queue_handle == NULL) return E_INVALID_VALUE;
    
    return E_NO_ERROR;
}

static void storage_task_write_event_impl(write_event_cmd_t* cmd)
{
    err_t err = E_NO_ERROR;
    
    do {
        err = drive_events_write_event(&cmd->event);
    } while(err == E_BUSY);
    
    if(err != E_NO_ERROR) return;
    
    do {
        err = drive_events_write();
    } while(err == E_BUSY);
}

static void storage_task_write_osc_impl(write_event_osc_cmd_t* cmd)
{
    err_t err = E_NO_ERROR;
    
    do {
        err = drive_events_write_event(&cmd->event);
    } while(err == E_BUSY);
    
    if(err != E_NO_ERROR){
        drive_power_oscillogram_resume();
        return;
    }
    
    do{
        if(!drive_power_oscillogram_is_paused()){
            drive_power_oscillogram_half_pause();
        }
    }while(!drive_power_oscillogram_paused());
    
    do {
        err = drive_events_write_current_oscillogram(cmd->event.id);
    } while(err == E_BUSY);
    
    drive_power_oscillogram_resume();
    
    if(err != E_NO_ERROR){
        return;
    }
    
    do {
        err = drive_events_write();
    } while(err == E_BUSY);
}

static void storage_task_read_event_impl(read_event_cmd_t* cmd)
{
    err_t err = E_NO_ERROR;
    do {
        err = drive_events_read_event(cmd->event, cmd->index);
    } while(err == E_BUSY);
    
    if(cmd->future) future_finish(cmd->future, int_to_pvoid(err));
}

static void storage_task_read_osc_impl(read_osc_cmd_t* cmd)
{
    err_t err = E_NO_ERROR;
    do {
        err = drive_events_read_osc_channel(cmd->index, cmd->channel);
    } while(err == E_BUSY);
    
    if(cmd->future) future_finish(cmd->future, int_to_pvoid(err));
}

static void storage_task_clear_events_impl(clear_events_cmd_t* cmd)
{
    drive_events_reset();
    
    err_t err = E_NO_ERROR;
    do {
        err = drive_events_write();
    } while(err == E_BUSY);
}

static void storage_task_save_settings_impl(save_settings_cmd_t* cmd)
{
    err_t err = E_NO_ERROR;
    do {
        err = settings_write();
    } while(err == E_BUSY);
}

static void storage_task_save_nvdata_impl(save_nvdata_cmd_t* cmd)
{
    err_t err = E_NO_ERROR;
    do {
        err = drive_nvdata_write();
    } while(err == E_BUSY);
}

static void storage_task_proc(void* arg)
{
    static task_storage_cmd_t cmd;
    for(;;){
        if(xQueueReceive(storage_task.queue_handle, &cmd, portMAX_DELAY) == pdTRUE){
            switch(cmd.type){
                default:
                    break;
                case TASK_STORAGE_CMD_WRITE_EVENT:
                    storage_task_write_event_impl(&cmd.write_event);
                    break;
                case TASK_STORAGE_CMD_WRITE_OSC:
                    storage_task_write_osc_impl(&cmd.write_event_osc);
                    break;
                case TASK_STORAGE_CMD_READ_EVENT:
                    storage_task_read_event_impl(&cmd.read_event);
                    break;
                case TASK_STORAGE_CMD_READ_OSC:
                    storage_task_read_osc_impl(&cmd.read_osc);
                    break;
                case TASK_STORAGE_CMD_CLEAR_EVENTS:
                    storage_task_clear_events_impl(&cmd.clear_events);
                    break;
                case TASK_STORAGE_CMD_SAVE_SETTINGS:
                    storage_task_save_settings_impl(&cmd.save_settings);
                    break;
                case TASK_STORAGE_CMD_SAVE_NVDATA:
                    storage_task_save_nvdata_impl(&cmd.save_nvdata);
                    break;
            }
        }
    }
}

err_t drive_task_storage_write_event(drive_event_type_t type, bool need_osc)
{
    task_storage_cmd_t cmd;
    
    bool osc_paused = false;
    
    if(need_osc){
        cmd.type = TASK_STORAGE_CMD_WRITE_OSC;
        drive_events_make_event(&cmd.write_event_osc.event, type);
        
        portENTER_CRITICAL();
        
        if(!drive_power_oscillogram_is_paused()){
            drive_power_oscillogram_half_pause();
            osc_paused = true;
        }
        
        portEXIT_CRITICAL();
    }else{
        cmd.type = TASK_STORAGE_CMD_WRITE_EVENT;
        drive_events_make_event(&cmd.write_event.event, type);
    }
    
    if(xQueueSendToBack(storage_task.queue_handle, &cmd, STORAGE_WAIT) != pdTRUE){
        if(osc_paused) drive_power_oscillogram_resume();
        return E_OUT_OF_MEMORY;
    }
    
    return E_NO_ERROR;
}

err_t drive_task_storage_write_error_event(void)
{
    return drive_task_storage_write_event(true, DRIVE_EVENT_TYPE_ERROR);
}

err_t drive_task_storage_write_status_event(void)
{
    return drive_task_storage_write_event(true, DRIVE_EVENT_TYPE_STATUS);
}

err_t drive_task_storage_write_warning_event(void)
{
    return drive_task_storage_write_event(false, DRIVE_EVENT_TYPE_WARNING);
}

err_t drive_task_storage_read_event(future_t* future, drive_event_index_t event_index, drive_event_t* event)
{
    task_storage_cmd_t cmd;
    cmd.type = TASK_STORAGE_CMD_READ_EVENT;
    cmd.read_event.future = future;
    cmd.read_event.index = event_index;
    cmd.read_event.event = event;
    
    if(xQueueSendToBack(storage_task.queue_handle, &cmd, STORAGE_WAIT) != pdTRUE){
        return E_OUT_OF_MEMORY;
    }
    
    return E_NO_ERROR;
}

err_t drive_task_storage_read_osc_channel(future_t* future, drive_osc_index_t osc_index, size_t osc_channel)
{
    task_storage_cmd_t cmd;
    cmd.type = TASK_STORAGE_CMD_READ_OSC;
    cmd.read_osc.future = future;
    cmd.read_osc.index = osc_index;
    cmd.read_osc.channel = (uint8_t)osc_channel;
    
    if(xQueueSendToBack(storage_task.queue_handle, &cmd, STORAGE_WAIT) != pdTRUE){
        return E_OUT_OF_MEMORY;
    }
    
    return E_NO_ERROR;
}

err_t drive_task_storage_clear_events(void)
{
    task_storage_cmd_t cmd;
    cmd.type = TASK_STORAGE_CMD_CLEAR_EVENTS;
    
    if(xQueueSendToBack(storage_task.queue_handle, &cmd, STORAGE_WAIT) != pdTRUE){
        return E_OUT_OF_MEMORY;
    }
    
    return E_NO_ERROR;
}

err_t drive_task_storage_save_settings(void)
{
    task_storage_cmd_t cmd;
    cmd.type = TASK_STORAGE_CMD_SAVE_SETTINGS;
    
    if(xQueueSendToBack(storage_task.queue_handle, &cmd, STORAGE_WAIT) != pdTRUE){
        return E_OUT_OF_MEMORY;
    }
    
    return E_NO_ERROR;
}

err_t drive_task_storage_save_nvdata(void)
{
    task_storage_cmd_t cmd;
    cmd.type = TASK_STORAGE_CMD_SAVE_NVDATA;
    
    if(xQueueSendToBack(storage_task.queue_handle, &cmd, STORAGE_WAIT) != pdTRUE){
        return E_OUT_OF_MEMORY;
    }
    
    return E_NO_ERROR;
}
