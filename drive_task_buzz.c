#include "drive_task_buzz.h"
#include <stddef.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include "drive_keypad.h"


#define TASK_BUZZER_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

typedef struct _Buzzer_Task {
    // Задача.
    StackType_t task_stack[TASK_BUZZER_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
} buzzer_task_t;

static buzzer_task_t buzzer_task;


static void buzzer_task_proc(void*);

err_t drive_task_buzz_init(uint32_t priority)
{
    memset(&buzzer_task, 0x0, sizeof(buzzer_task_t));
    
    buzzer_task.task_handle = xTaskCreateStatic(buzzer_task_proc, "buzzer_task",
            TASK_BUZZER_STACK_SIZE, NULL, priority, buzzer_task.task_stack, &buzzer_task.task_buffer);
    
    if(buzzer_task.task_handle == NULL) return E_INVALID_VALUE;
    
    return E_NO_ERROR;
}

static void buzzer_task_proc(void* arg)
{
    for(;;){
        vTaskSuspend(NULL);
        
        while(drive_keypad_pins_on(DRIVE_KPD_PIN_BUZZ) != E_NO_ERROR);
        
        vTaskDelay(pdMS_TO_TICKS(100));
        
        while(drive_keypad_pins_off(DRIVE_KPD_PIN_BUZZ) != E_NO_ERROR);
    }
}

void drive_task_buzz_beep(void)
{
    vTaskResume(buzzer_task.task_handle);
}

