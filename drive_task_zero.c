#include "drive_task_zero.h"
#include "drive.h"
#include <stddef.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>


#define TASK_ZERO_STACK_SIZE (configMINIMAL_STACK_SIZE)

typedef struct _Zero_Task {
    // Задача.
    StackType_t task_stack[TASK_ZERO_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
} zero_task_t;

static zero_task_t zero_task;


static void zero_task_proc(void*);

err_t drive_task_zero_init(uint32_t priority)
{
    memset(&zero_task, 0x0, sizeof(zero_task_t));
    
    zero_task.task_handle = xTaskCreateStatic(zero_task_proc, "zero_task",
            TASK_ZERO_STACK_SIZE, NULL, priority, zero_task.task_stack, &zero_task.task_buffer);

    if(zero_task.task_handle == NULL) return E_INVALID_VALUE;

    return E_NO_ERROR;
}

static void zero_task_proc(void* arg)
{
    for(;;){
        vTaskSuspend(NULL);

        drive_process_zero();
    }
}

bool drive_task_zero_process_isr(void)
{
    return xTaskResumeFromISR(zero_task.task_handle) == pdTRUE;
}
