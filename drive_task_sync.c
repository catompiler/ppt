#include "drive_task_sync.h"
#include "drive.h"
#include <stddef.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>


#define TASK_SYNC_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

typedef struct _Zero_Task {
    // Задача.
    StackType_t task_stack[TASK_SYNC_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
} sync_task_t;

static sync_task_t sync_task;


static void sync_task_proc(void*);

err_t drive_task_sync_init(uint32_t priority)
{
    memset(&sync_task, 0x0, sizeof(sync_task_t));
    
    sync_task.task_handle = xTaskCreateStatic(sync_task_proc, "sync_task",
            TASK_SYNC_STACK_SIZE, NULL, priority, sync_task.task_stack, &sync_task.task_buffer);

    if(sync_task.task_handle == NULL) return E_INVALID_VALUE;

    return E_NO_ERROR;
}

static void sync_task_proc(void* arg)
{
    for(;;){
        vTaskSuspend(NULL);

        drive_process_sync_iter();
    }
}

bool drive_task_sync_process_isr(void)
{
    return xTaskResumeFromISR(sync_task.task_handle) == pdTRUE;
}
