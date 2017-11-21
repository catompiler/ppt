#include "drive_task_modbus.h"
#include <stddef.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>


#define TASK_MODBUS_STACK_SIZE (configMINIMAL_STACK_SIZE)

typedef struct _Modbus_Task {
    // Задача.
    StackType_t task_stack[TASK_MODBUS_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
    // Данные.
    modbus_rtu_t* modbus; //!< Modbus.
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
    
    return E_NO_ERROR;
}

void drive_task_modbus_setup(modbus_rtu_t* modbus, rs485_set_output_proc_t rs485_set_out,
                                                   rs485_set_input_proc_t rs485_set_in)
{
    modbus_task.modbus = modbus;
    modbus_task.rs485_set_out = rs485_set_out;
    modbus_task.rs485_set_in = rs485_set_in;
}

static void modbus_task_proc(void* arg)
{
    for(;;){
        vTaskSuspend(NULL);
        
        //modbus_rs485_set_output();
        if(modbus_task.rs485_set_out) modbus_task.rs485_set_out();
        if(modbus_rtu_dispatch(modbus_task.modbus) != E_NO_ERROR){
            //modbus_rs485_set_input();
            if(modbus_task.rs485_set_in) modbus_task.rs485_set_in();
        }
    }
}

bool drive_task_modbus_process_isr(void)
{
    return xTaskResumeFromISR(modbus_task.task_handle) == pdTRUE;
}
