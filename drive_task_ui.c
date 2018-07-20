#include "drive_task_ui.h"
#include "drive_ui.h"
#undef LIST_H
#include <stddef.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>


#define TASK_UI_STACK_SIZE (configMINIMAL_STACK_SIZE * 4)


typedef struct _Ui_Task {
    // Задача.
    StackType_t task_stack[TASK_UI_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
} ui_task_t;

static ui_task_t ui_task;


static void ui_task_proc(void*);

err_t drive_task_ui_init(uint32_t priority)
{
    memset(&ui_task, 0x0, sizeof(ui_task_t));
    
    ui_task.task_handle = xTaskCreateStatic(ui_task_proc, "ui_task",
            TASK_UI_STACK_SIZE, NULL, priority, ui_task.task_stack, &ui_task.task_buffer);
    
    if(ui_task.task_handle == NULL) return E_INVALID_VALUE;
    
    return E_NO_ERROR;
}

void drive_task_ui_deinit(void)
{
    vTaskDelete(ui_task.task_handle);
}

static void ui_task_proc(void* arg)
{
    while(drive_ui_setup() != E_NO_ERROR);
    for(;;){
        drive_ui_process();
    }
}
