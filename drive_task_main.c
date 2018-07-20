#include "drive_task_main.h"
#include "drive.h"
#include <stddef.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>


#define TASK_MAIN_STACK_SIZE (configMINIMAL_STACK_SIZE * 3)

typedef struct _Main_Task {
    // Задача.
    StackType_t task_stack[TASK_MAIN_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
} main_task_t;

static main_task_t main_task;


static void main_task_proc(void*);

err_t drive_task_main_init(uint32_t priority)
{
    memset(&main_task, 0x0, sizeof(main_task_t));
    
    main_task.task_handle = xTaskCreateStatic(main_task_proc, "main_task",
            TASK_MAIN_STACK_SIZE, NULL, priority, main_task.task_stack, &main_task.task_buffer);
    
    if(main_task.task_handle == NULL) return E_INVALID_VALUE;
    
    return E_NO_ERROR;
}

static void main_task_proc(void* arg)
{
    for(;;){
        vTaskSuspend(NULL);
        
        drive_process_iter();
    }
}

bool drive_task_main_process_isr(void)
{
    return xTaskResumeFromISR(main_task.task_handle) == pdTRUE;
}
