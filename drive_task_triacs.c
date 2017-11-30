#include "drive_task_triacs.h"
#include "drive.h"
#include <stddef.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>


#define TASK_TRIACS_STACK_SIZE (configMINIMAL_STACK_SIZE)

typedef struct _Triacs_Task {
    // Задача.
    StackType_t task_stack[TASK_TRIACS_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
} triacs_task_t;

static triacs_task_t triacs_task;


static void triacs_task_proc(void*);

err_t drive_task_triacs_init(uint32_t priority)
{
    memset(&triacs_task, 0x0, sizeof(triacs_task_t));
    
    triacs_task.task_handle = xTaskCreateStatic(triacs_task_proc, "triacs_task",
            TASK_TRIACS_STACK_SIZE, NULL, priority, triacs_task.task_stack, &triacs_task.task_buffer);

    if(triacs_task.task_handle == NULL) return E_INVALID_VALUE;

    return E_NO_ERROR;
}

static void triacs_task_proc(void* arg)
{
    for(;;){
        vTaskSuspend(NULL);

        drive_process_triacs_iter();
    }
}

bool drive_task_triacs_process_isr(void)
{
    return xTaskResumeFromISR(triacs_task.task_handle) == pdTRUE;
}
