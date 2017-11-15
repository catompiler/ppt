#include "drive_task_events.h"
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <queue.h>
#include <stddef.h>
#include "drive_events.h"
#include "drive_power.h"
#include "utils/utils.h"


#define TASK_EVENTS_STACK_SIZE configMINIMAL_STACK_SIZE
#define QUEUE_EVENTS_SIZE 4

#define EVETNS_WAIT 0


// Команды задачи.
//! Запись события.
#define TASK_EVENTS_CMD_WRITE 0
//! Запись события с осциллограммой.
#define TASK_EVENTS_CMD_WRITE_OSC 1
//! Чтение события.
#define TASK_EVENTS_CMD_READ 2
//! Чтение осциллограммы.
#define TASK_EVENTS_CMD_READ_OSC 3
//! Очистка событий.
#define TASK_EVENTS_CMD_CLEAR 4
//! .
//#define TASK_EVENTS_CMD_ 0

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
// У события нет данных.
typedef struct _Clear_Events_Cmd {
    uint8_t reserved; //!< Зарезервировано.
} clear_events_cmd_t;

//! Структура элемента очереди записи событий.
typedef struct _Task_Cmd {
    uint8_t type; //!< Тип команды.
    union {
        write_event_cmd_t write_event;
        write_event_osc_cmd_t write_event_osc;
        read_event_cmd_t read_event;
        read_osc_cmd_t read_osc;
        clear_events_cmd_t clear_events;
    };
} task_events_cmd_t;

typedef struct _Task_Events {
    // Задача.
    StackType_t task_stack[TASK_EVENTS_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
    // Очередь.
    task_events_cmd_t queue_storage[QUEUE_EVENTS_SIZE]; //!< Данные очереди.
    StaticQueue_t queue_buffer; //!< Буфер очереди.
    QueueHandle_t queue_handle; //!< Идентификатор очереди.
} task_events_t;


static task_events_t events_task;



static void events_task_proc(void*);

err_t drive_task_events_init(uint32_t priority)
{
    events_task.task_handle = xTaskCreateStatic(events_task_proc, "events_task",
            TASK_EVENTS_STACK_SIZE, NULL, priority, events_task.task_stack, &events_task.task_buffer);
    
    if(events_task.task_handle == NULL) return E_INVALID_VALUE;
    
    
    events_task.queue_handle = xQueueCreateStatic(QUEUE_EVENTS_SIZE, sizeof(task_events_cmd_t),
                                              (uint8_t*)events_task.queue_storage, &events_task.queue_buffer);
    
    if(events_task.queue_handle == NULL) return E_INVALID_VALUE;
    
    return E_NO_ERROR;
}

static void events_task_write_impl(write_event_cmd_t* cmd)
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

static void events_task_write_osc_impl(write_event_osc_cmd_t* cmd)
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

static void events_task_read_impl(read_event_cmd_t* cmd)
{
    err_t err = E_NO_ERROR;
    do {
        err = drive_events_read_event(cmd->event, cmd->index);
    } while(err == E_BUSY);
    
    if(cmd->future) future_finish(cmd->future, int_to_pvoid(err));
}

static void events_task_read_osc_impl(read_osc_cmd_t* cmd)
{
    err_t err = E_NO_ERROR;
    do {
        err = drive_events_read_osc_channel(cmd->index, cmd->channel);
    } while(err == E_BUSY);
    
    if(cmd->future) future_finish(cmd->future, int_to_pvoid(err));
}

static void events_task_clear_impl(clear_events_cmd_t* cmd)
{
    drive_events_reset();
    
    err_t err = E_NO_ERROR;
    do {
        err = drive_events_write();
    } while(err == E_BUSY);
}

static void events_task_proc(void* arg)
{
    task_events_cmd_t cmd;
    for(;;){
        if(xQueueReceive(events_task.queue_handle, &cmd, portMAX_DELAY) == pdTRUE){
            switch(cmd.type){
                default:
                    break;
                case TASK_EVENTS_CMD_WRITE:
                    events_task_write_impl(&cmd.write_event);
                    break;
                case TASK_EVENTS_CMD_WRITE_OSC:
                    events_task_write_osc_impl(&cmd.write_event_osc);
                    break;
                case TASK_EVENTS_CMD_READ:
                    events_task_read_impl(&cmd.read_event);
                    break;
                case TASK_EVENTS_CMD_READ_OSC:
                    events_task_read_osc_impl(&cmd.read_osc);
                    break;
                case TASK_EVENTS_CMD_CLEAR:
                    events_task_clear_impl(&cmd.clear_events);
                    break;
            }
        }
    }
}

err_t drive_task_events_write(drive_event_type_t type, bool need_osc)
{
    task_events_cmd_t cmd;
    
    bool osc_paused = false;
    
    if(need_osc){
        cmd.type = TASK_EVENTS_CMD_WRITE_OSC;
        drive_events_make_event(&cmd.write_event_osc.event, type);
        
        portENTER_CRITICAL();
        
        if(!drive_power_oscillogram_is_paused()){
            drive_power_oscillogram_half_pause();
            osc_paused = true;
        }
        
        portEXIT_CRITICAL();
    }else{
        cmd.type = TASK_EVENTS_CMD_WRITE;
        drive_events_make_event(&cmd.write_event.event, type);
    }
    
    if(xQueueSendToBack(events_task.queue_handle, &cmd, EVETNS_WAIT) != pdTRUE){
        if(osc_paused) drive_power_oscillogram_resume();
        return E_OUT_OF_MEMORY;
    }
    
    return E_NO_ERROR;
}

err_t drive_task_events_write_error(void)
{
    return drive_task_events_write(true, DRIVE_EVENT_TYPE_ERROR);
}

err_t drive_task_events_write_status(void)
{
    return drive_task_events_write(true, DRIVE_EVENT_TYPE_STATUS);
}

err_t drive_task_events_write_warning(void)
{
    return drive_task_events_write(false, DRIVE_EVENT_TYPE_WARNING);
}

err_t drive_task_events_read(future_t* future, drive_event_index_t event_index, drive_event_t* event)
{
    task_events_cmd_t cmd;
    cmd.type = TASK_EVENTS_CMD_READ;
    cmd.read_event.future = future;
    cmd.read_event.index = event_index;
    cmd.read_event.event = event;
    
    if(xQueueSendToBack(events_task.queue_handle, &cmd, EVETNS_WAIT) != pdTRUE){
        return E_OUT_OF_MEMORY;
    }
    
    return E_NO_ERROR;
}

err_t drive_task_events_read_osc_channel(future_t* future, drive_osc_index_t osc_index, size_t osc_channel)
{
    task_events_cmd_t cmd;
    cmd.type = TASK_EVENTS_CMD_READ_OSC;
    cmd.read_osc.future = future;
    cmd.read_osc.index = osc_index;
    cmd.read_osc.channel = (uint8_t)osc_channel;
    
    if(xQueueSendToBack(events_task.queue_handle, &cmd, EVETNS_WAIT) != pdTRUE){
        return E_OUT_OF_MEMORY;
    }
    
    return E_NO_ERROR;
}

err_t drive_task_events_clear(void)
{
    task_events_cmd_t cmd;
    cmd.type = TASK_EVENTS_CMD_CLEAR;
    
    if(xQueueSendToBack(events_task.queue_handle, &cmd, EVETNS_WAIT) != pdTRUE){
        return E_OUT_OF_MEMORY;
    }
    
    return E_NO_ERROR;
}
