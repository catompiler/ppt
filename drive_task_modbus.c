#include "drive_task_modbus.h"
#include <stddef.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>


#define TASK_MODBUS_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define TASK_MODBUS_QUEUE_SIZE 4

#define TASK_MODBUS_WAIT_TICKS 0


//! Тип команды задачи Modbus.
typedef struct _Modbus_Task_Cmd {
    modbus_rtu_t* modbus; //!< Modbus.
    bool is_rs485; //!< Флаг 485го интерфейса.
} modbus_task_cmd_t;


typedef struct _Modbus_Task {
    // Задача.
    StackType_t task_stack[TASK_MODBUS_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
    // Очередь.
    modbus_task_cmd_t queue_storage[TASK_MODBUS_QUEUE_SIZE]; //!< Данные очереди.
    StaticQueue_t queue_buffer; //!< Буфер очереди.
    QueueHandle_t queue_handle; //!< Идентификатор очереди.
    // Данные.
    rs485_set_output_proc_t rs485_set_out; //!< Установка rs485 на выход.
    rs485_set_input_proc_t rs485_set_in; //!< Установка rs485 на вход.
} modbus_task_t;

static modbus_task_t modbus_task;


static void modbus_task_proc(void*);

err_t drive_task_modbus_init(uint32_t priority)
{
    memset(&modbus_task, 0x0, sizeof(modbus_task_t));
    
    modbus_task.task_handle = xTaskCreateStatic(modbus_task_proc, "modbus_task",
            TASK_MODBUS_STACK_SIZE, NULL, priority, modbus_task.task_stack, &modbus_task.task_buffer);
    
    if(modbus_task.task_handle == NULL) return E_INVALID_VALUE;
    
    modbus_task.queue_handle = xQueueCreateStatic(TASK_MODBUS_QUEUE_SIZE, sizeof(modbus_task_cmd_t),
                                              (uint8_t*)modbus_task.queue_storage, &modbus_task.queue_buffer);

    if(modbus_task.queue_handle == NULL) return E_INVALID_VALUE;

    return E_NO_ERROR;
}

void drive_task_modbus_setup(rs485_set_output_proc_t rs485_set_out,
                             rs485_set_input_proc_t rs485_set_in)
{
    modbus_task.rs485_set_out = rs485_set_out;
    modbus_task.rs485_set_in = rs485_set_in;
}

static void modbus_task_proc(void* arg)
{
    modbus_task_cmd_t cmd;
    for(;;){
        if(xQueueReceive(modbus_task.queue_handle, &cmd, portMAX_DELAY) == pdTRUE){

            if(cmd.modbus){
                if(cmd.is_rs485 && modbus_task.rs485_set_out) modbus_task.rs485_set_out();

                if(modbus_rtu_dispatch(cmd.modbus) != E_NO_ERROR){
                    if(cmd.is_rs485 && modbus_task.rs485_set_in) modbus_task.rs485_set_in();
                }
            }
        }
    }
}

err_t drive_task_modbus_process_isr(modbus_rtu_t* modbus, bool is485, BaseType_t* pxHigherPriorityTaskWoken)
{
    modbus_task_cmd_t cmd;

    cmd.modbus = modbus;
    cmd.is_rs485 = is485;

    if(xQueueSendToBackFromISR(modbus_task.queue_handle, &cmd, pxHigherPriorityTaskWoken) != pdTRUE){
        return E_OUT_OF_MEMORY;
    }

    return E_NO_ERROR;
}
