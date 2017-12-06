#include "drive_task_adc.h"
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <queue.h>
#include <stddef.h>
#include <string.h>
#include "drive.h"
#include "drive_power.h"


#define TASK_ADC_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define QUEUE_ADC_SIZE 8


// Команды задачи.
//! Сохранение.
#define TASK_ADC_CMD_SAVE 0
//! Применение.
#define TASK_ADC_CMD_APPLY 1

//! Структура элемента очереди.
typedef struct _Task_Adc_Cmd {
    uint16_t data[DRIVE_POWER_ADC_CHANNELS_COUNT];
} task_adc_cmd_t;


typedef struct _Task_Adc {
    // Задача.
    StackType_t task_stack[TASK_ADC_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
    // Очередь.
    task_adc_cmd_t queue_storage[QUEUE_ADC_SIZE]; //!< Данные очереди.
    StaticQueue_t queue_buffer; //!< Буфер очереди.
    QueueHandle_t queue_handle; //!< Идентификатор очереди.
} task_adc_t;


static task_adc_t adc_task;



static void adc_task_proc(void*);

err_t drive_task_adc_init(uint32_t priority)
{
    memset(&adc_task, 0x0, sizeof(task_adc_t));
    
    adc_task.task_handle = xTaskCreateStatic(adc_task_proc, "adc_task",
            TASK_ADC_STACK_SIZE, NULL, priority, adc_task.task_stack, &adc_task.task_buffer);
    
    if(adc_task.task_handle == NULL) return E_INVALID_VALUE;
    
    
    adc_task.queue_handle = xQueueCreateStatic(QUEUE_ADC_SIZE, sizeof(task_adc_cmd_t),
                                               (uint8_t*)adc_task.queue_storage, &adc_task.queue_buffer);
    
    if(adc_task.queue_handle == NULL) return E_INVALID_VALUE;
    
    return E_NO_ERROR;
}

static void adc_task_process_data_impl(task_adc_cmd_t* cmd)
{
    drive_process_power_adc_values(DRIVE_POWER_ADC_CHANNELS, cmd->data);
}

static void adc_task_proc(void* arg)
{
    static task_adc_cmd_t cmd;
    for(;;){
        if(xQueueReceive(adc_task.queue_handle, &cmd, portMAX_DELAY) == pdTRUE){
            adc_task_process_data_impl(&cmd);
        }
    }
}

err_t drive_task_adc_process_data_isr(uint16_t* adc_data, BaseType_t* pxHigherPriorityTaskWoken)
{
    task_adc_cmd_t cmd;
    
    memcpy(cmd.data, adc_data, sizeof(uint16_t) * DRIVE_POWER_ADC_CHANNELS_COUNT);
    
    if(xQueueSendToBackFromISR(adc_task.queue_handle, &cmd, pxHigherPriorityTaskWoken) != pdTRUE){
        return E_OUT_OF_MEMORY;
    }
    
    return E_NO_ERROR;
}
