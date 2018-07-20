#include "drive_task_temp.h"
#include <stddef.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include "drive_temp.h"


#define TASK_HS_TEMP_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define TASK_HS_TEMP_UPDATE_PERIOD_MS 1000


typedef struct _Hs_Temp_Task {
    // Задача.
    StackType_t task_stack[TASK_HS_TEMP_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
} hs_temp_task_t;

static hs_temp_task_t hs_temp_task;


static void temp_task_proc(void*);

err_t drive_task_temp_init(uint32_t priority)
{
    memset(&hs_temp_task, 0x0, sizeof(hs_temp_task_t));
    
    hs_temp_task.task_handle = xTaskCreateStatic(temp_task_proc, "hs_temp_task",
            TASK_HS_TEMP_STACK_SIZE, NULL, priority, hs_temp_task.task_stack, &hs_temp_task.task_buffer);
    
    if(hs_temp_task.task_handle == NULL) return E_INVALID_VALUE;
    
    return E_NO_ERROR;
}

static void temp_task_proc(void* arg)
{
    for(;;){
        drive_temp_update();
        drive_temp_regulate_fan();
        
        vTaskDelay(pdMS_TO_TICKS(TASK_HS_TEMP_UPDATE_PERIOD_MS));
    }
}
