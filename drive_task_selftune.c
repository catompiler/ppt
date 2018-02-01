#include "drive_task_selftune.h"
#include "drive_selftuning.h"
#include <stddef.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include "future/future.h"
#include "utils/utils.h"


#define TASK_SELFTUNE_STACK_SIZE (configMINIMAL_STACK_SIZE)

#define QUEUE_SETTINGS_SIZE 4

#define QUEUE_SEND_DELAY 0


//! Тип команды задачи самонастройки.
typedef struct _Selftune_Task_Cmd{
    uint8_t type;
    future_t* future;
} selftune_task_cmd_t;

//! Команда посчитать текущие собранные данные.
#define SELFTUNE_TASK_CMD_CALC_DATA 0
//! Команда посчитать R и L.
#define SELFTUNE_TASK_CMD_CALC 1

typedef struct _Zero_Task {
    // Задача.
    StackType_t task_stack[TASK_SELFTUNE_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
    
    // Очередь.
    selftune_task_cmd_t queue_storage[QUEUE_SETTINGS_SIZE]; //!< Данные очереди.
    StaticQueue_t queue_buffer; //!< Буфер очереди.
    QueueHandle_t queue_handle; //!< Идентификатор очереди.
} selftune_task_t;

static selftune_task_t selftune_task;


static void selftune_task_proc(void*);

err_t drive_task_selftune_init(uint32_t priority)
{
    memset(&selftune_task, 0x0, sizeof(selftune_task_t));
    
    selftune_task.task_handle = xTaskCreateStatic(selftune_task_proc, "selftune_task",
            TASK_SELFTUNE_STACK_SIZE, NULL, priority, selftune_task.task_stack, &selftune_task.task_buffer);

    if(selftune_task.task_handle == NULL) return E_INVALID_VALUE;
    
    
    selftune_task.queue_handle = xQueueCreateStatic(QUEUE_SETTINGS_SIZE, sizeof(selftune_task_cmd_t),
                                              (uint8_t*)selftune_task.queue_storage, &selftune_task.queue_buffer);
    
    if(selftune_task.queue_handle == NULL) return E_INVALID_VALUE;

    return E_NO_ERROR;
}

static void selftune_task_calc_data_impl(future_t* future)
{
    drive_selftuning_calculate_adc_data();
    
    bool res = drive_selftuning_calculate_current_data();
    
    if(future) future_finish(future, int_to_pvoid((int)res));
}

static void selftune_task_calc_impl(future_t* future)
{
    drive_selftuning_calculate();
    
    if(future) future_finish(future, int_to_pvoid(1));
}

static void selftune_task_proc(void* arg)
{
    selftune_task_cmd_t cmd;
    for(;;){
        if(xQueueReceive(selftune_task.queue_handle, &cmd, portMAX_DELAY) == pdTRUE){
            switch(cmd.type){
                default:
                    break;
                case SELFTUNE_TASK_CMD_CALC_DATA:
                    selftune_task_calc_data_impl(cmd.future);
                    break;
                case SELFTUNE_TASK_CMD_CALC:
                    selftune_task_calc_impl(cmd.future);
                    break;
            }
        }
    }
}

bool drive_task_selftune_calc_data(future_t* future)
{
    selftune_task_cmd_t cmd;
    
    cmd.type = SELFTUNE_TASK_CMD_CALC_DATA;
    cmd.future = future;
    
    if(future) future_start(future);
    
    if(xQueueSendToBack(selftune_task.queue_handle, &cmd, QUEUE_SEND_DELAY) != pdTRUE){
        if(future) future_finish(future, int_to_pvoid(0));
        return false;
    }
    return true;
}

bool drive_task_selftune_calc(future_t* future)
{
    selftune_task_cmd_t cmd;
    
    cmd.type = SELFTUNE_TASK_CMD_CALC;
    cmd.future = future;
    
    if(future) future_start(future);
    
    if(xQueueSendToBack(selftune_task.queue_handle, &cmd, QUEUE_SEND_DELAY) != pdTRUE){
        if(future) future_finish(future, int_to_pvoid(0));
        return false;
    }
    return true;
}
