#include "drive_task_utils.h"
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <queue.h>
#include <stddef.h>
#include <string.h>
#include "drive_temp.h"
#include "drive_task_storage.h"



// Стэк.
#define TASK_UTILS_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
// Длина очереди.
#define QUEUE_UTILS_SIZE 4

// Ожидание очереди.
#define TASK_UTILS_WAIT 0

// Число таймеров.
#define TIMERS_COUNT 1
// Таймер записи наработки.
#define TIMER_NVDATA 0
#define TIMER_NVDATA_PERIOD_MS (60*60*1000)

// Команды задачи.
//! Применение настроек.
#define TASK_UTILS_CMD_APPLY_SETTINGS 0
//! Сброс задачи UI.
#define TASK_UTILS_CMD_RESET_UI 1


typedef struct _Task_Utils {
    // Задача.
    StackType_t task_stack[TASK_UTILS_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
    // Очередь.
    uint8_t queue_storage[QUEUE_UTILS_SIZE]; //!< Данные очереди.
    StaticQueue_t queue_buffer; //!< Буфер очереди.
    QueueHandle_t queue_handle; //!< Идентификатор очереди.
    // Таймер.
    StaticTimer_t timer_buffer[TIMERS_COUNT]; //!< Буфер таймера.
    TimerHandle_t timer_handle[TIMERS_COUNT]; //!< Идентификатор таймера.

    // Данные.
    drive_task_utils_callback_t reset_ui; //!< Каллбэк сброса UI.
} task_utils_t;


static task_utils_t utils_task;


static void utils_task_nvdata_proc(void*);
static void utils_task_proc(void*);

err_t drive_task_utils_init(uint32_t priority)
{
    memset(&utils_task, 0x0, sizeof(task_utils_t));
    
    utils_task.task_handle = xTaskCreateStatic(utils_task_proc, "utils_task",
            TASK_UTILS_STACK_SIZE, NULL, priority, utils_task.task_stack, &utils_task.task_buffer);
    
    if(utils_task.task_handle == NULL) return E_INVALID_VALUE;
    
    
    utils_task.queue_handle = xQueueCreateStatic(QUEUE_UTILS_SIZE, sizeof(uint8_t),
                                              utils_task.queue_storage, &utils_task.queue_buffer);
    
    if(utils_task.queue_handle == NULL) return E_INVALID_VALUE;
    
    
    utils_task.timer_handle[TIMER_NVDATA] = xTimerCreateStatic("nvdata_timer", pdMS_TO_TICKS(TIMER_NVDATA_PERIOD_MS),
                                                  pdTRUE, NULL, utils_task_nvdata_proc, &utils_task.timer_buffer[TIMER_NVDATA]);
    
    if(utils_task.timer_handle[TIMER_NVDATA] == NULL) return E_INVALID_VALUE;
    if(xTimerStart(utils_task.timer_handle[TIMER_NVDATA], 0) != pdPASS) return E_STATE;
    
    return E_NO_ERROR;
}

void drive_task_utils_set_reset_ui_callback(drive_task_utils_callback_t callback)
{
    utils_task.reset_ui = callback;
}

static void utils_task_apply_impl(void)
{
    drive_update_settings();
    drive_temp_update_settings();
}

static void utils_task_reset_ui_impl(void)
{
    if(utils_task.reset_ui) utils_task.reset_ui();
}

static void utils_task_proc(void* arg)
{
    (void) arg;
    
    uint8_t cmd = 0;
    for(;;){
        if(xQueueReceive(utils_task.queue_handle, &cmd, portMAX_DELAY) == pdTRUE){
            switch(cmd){
                default:
                    break;
                case TASK_UTILS_CMD_APPLY_SETTINGS:
                    utils_task_apply_impl();
                    break;
                case TASK_UTILS_CMD_RESET_UI:
                    utils_task_reset_ui_impl();
                    break;
            }
        }
    }
}

void utils_task_nvdata_proc(void* arg)
{
    (void) arg;
    
    drive_task_storage_save_nvdata();
}

err_t drive_task_settings_apply(void)
{
    uint8_t cmd = TASK_UTILS_CMD_APPLY_SETTINGS;
    if(xQueueSendToBack(utils_task.queue_handle, &cmd, TASK_UTILS_WAIT) != pdTRUE){
        return E_OUT_OF_MEMORY;
    }
    return E_NO_ERROR;
}

err_t drive_task_utils_reset_ui(void)
{
    uint8_t cmd = TASK_UTILS_CMD_RESET_UI;
    if(xQueueSendToBack(utils_task.queue_handle, &cmd, TASK_UTILS_WAIT) != pdTRUE){
        return E_OUT_OF_MEMORY;
    }
    return E_NO_ERROR;
}
