#include "drive_task_settings.h"
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <queue.h>
#include <stddef.h>
#include <string.h>
#include "settings.h"
#include "drive.h"
#include "drive_temp.h"


#define TASK_SETTINGS_STACK_SIZE configMINIMAL_STACK_SIZE
#define QUEUE_SETTINGS_SIZE 4


// Команды задачи.
//! Сохранение.
#define TASK_SETTINGS_CMD_SAVE 0
//! Применение.
#define TASK_SETTINGS_CMD_APPLY 1


typedef struct _Task_Settings {
    // Задача.
    StackType_t task_stack[TASK_SETTINGS_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
    // Очередь.
    uint8_t queue_storage[QUEUE_SETTINGS_SIZE]; //!< Данные очереди.
    StaticQueue_t queue_buffer; //!< Буфер очереди.
    QueueHandle_t queue_handle; //!< Идентификатор очереди.
} task_settings_t;


static task_settings_t settings_task;



static void settings_task_proc(void*);

err_t drive_task_settings_init(uint32_t priority)
{
    memset(&settings_task, 0x0, sizeof(task_settings_t));
    
    settings_task.task_handle = xTaskCreateStatic(settings_task_proc, "settings_task",
            TASK_SETTINGS_STACK_SIZE, NULL, priority, settings_task.task_stack, &settings_task.task_buffer);
    
    if(settings_task.task_handle == NULL) return E_INVALID_VALUE;
    
    
    settings_task.queue_handle = xQueueCreateStatic(QUEUE_SETTINGS_SIZE, sizeof(uint8_t),
                                              settings_task.queue_storage, &settings_task.queue_buffer);
    
    if(settings_task.queue_handle == NULL) return E_INVALID_VALUE;
    
    return E_NO_ERROR;
}

static void settings_task_apply_impl(void)
{
    drive_update_settings();
    drive_temp_update_settings();
}

static void settings_task_save_impl(void)
{
    err_t err = E_NO_ERROR;
    do {
        err = settings_write();
    } while(err == E_BUSY);
}

static void settings_task_proc(void* arg)
{
    uint8_t cmd = 0;
    for(;;){
        if(xQueueReceive(settings_task.queue_handle, &cmd, portMAX_DELAY) == pdTRUE){
            switch(cmd){
                default:
                    break;
                case TASK_SETTINGS_CMD_SAVE:
                    settings_task_save_impl();
                    break;
                case TASK_SETTINGS_CMD_APPLY:
                    settings_task_apply_impl();
                    break;
            }
        }
    }
}

void drive_task_settings_apply(void)
{
    uint8_t cmd = TASK_SETTINGS_CMD_APPLY;
    xQueueSendToBack(settings_task.queue_handle, &cmd, portMAX_DELAY);
}

void drive_task_settings_save(void)
{
    uint8_t cmd = TASK_SETTINGS_CMD_SAVE;
    xQueueSendToBack(settings_task.queue_handle, &cmd, portMAX_DELAY);
}