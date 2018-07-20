#include "drive_task_i2c_watchdog.h"
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <queue.h>
#include <stddef.h>
#include <string.h>


#define TASK_I2C_WATCHDOG_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define TIMER_I2C_WATCHDOG_PERIOD_MS 1000
#define QUEUE_I2C_WATCHDOG_SIZE 4


typedef struct _Task_I2C_Watchdog {
    // Задача.
    StackType_t task_stack[TASK_I2C_WATCHDOG_STACK_SIZE]; //!< Стэк задачи.
    StaticTask_t task_buffer; //!< Буфер задачи.
    TaskHandle_t task_handle; //!< Идентификатор задачи.
    // Очередь.
    uint8_t queue_storage[QUEUE_I2C_WATCHDOG_SIZE]; //!< Данные очереди.
    StaticQueue_t queue_buffer; //!< Буфер очереди.
    QueueHandle_t queue_handle; //!< Идентификатор очереди.
    // Таймер.
    StaticTimer_t timer_buffer[DRIVE_TASK_I2C_WATCHDOG_COUNT]; //!< Буфер таймера.
    TimerHandle_t timer_handle[DRIVE_TASK_I2C_WATCHDOG_COUNT]; //!< Идентификатор таймера.
    // Прочее.
    reset_i2c_proc_t reset_proc[DRIVE_TASK_I2C_WATCHDOG_COUNT]; //!< Функция сброса i2c.
} task_i2c_wdt_t;


static task_i2c_wdt_t i2c_wdt_task;



static void i2c_watchdog_task_proc(void*);
static void i2c_watchdog_timer_proc(TimerHandle_t);

err_t drive_task_i2c_watchdog_init(uint32_t priority)
{
    memset(&i2c_wdt_task, 0x0, sizeof(task_i2c_wdt_t));
    
    i2c_wdt_task.task_handle = xTaskCreateStatic(i2c_watchdog_task_proc, "i2c_wdt_task",
            TASK_I2C_WATCHDOG_STACK_SIZE, NULL, priority, i2c_wdt_task.task_stack, &i2c_wdt_task.task_buffer);
    
    if(i2c_wdt_task.task_handle == NULL) return E_INVALID_VALUE;
    
    
    i2c_wdt_task.queue_handle = xQueueCreateStatic(QUEUE_I2C_WATCHDOG_SIZE, sizeof(uint8_t),
                                              i2c_wdt_task.queue_storage, &i2c_wdt_task.queue_buffer);
    
    if(i2c_wdt_task.queue_handle == NULL) return E_INVALID_VALUE;
    
    
    err_t err = E_NO_ERROR;
    size_t i;
    for(i = 0; i < DRIVE_TASK_I2C_WATCHDOG_COUNT; i ++){
        i2c_wdt_task.timer_handle[i] = xTimerCreateStatic("i2c_wdt_timer", pdMS_TO_TICKS(TIMER_I2C_WATCHDOG_PERIOD_MS),
                                                  pdFALSE, (void*)i, i2c_watchdog_timer_proc, &i2c_wdt_task.timer_buffer[i]);
        if(i2c_wdt_task.timer_handle[i] == NULL) err = E_INVALID_VALUE;
    }
    
    return err;
}

static void i2c_watchdog_task_proc(void* arg)
{
    uint8_t i2c_number = 0;
    for(;;){
        if(xQueueReceive(i2c_wdt_task.queue_handle, &i2c_number, portMAX_DELAY) == pdTRUE){
            if(i2c_number < DRIVE_TASK_I2C_WATCHDOG_COUNT){
                if(i2c_wdt_task.reset_proc[i2c_number]) i2c_wdt_task.reset_proc[i2c_number](i2c_number);
            }
        }
    }
}

static void i2c_watchdog_timer_proc(TimerHandle_t timer)
{
    uint8_t i2c_number = (uint8_t)(uint32_t)pvTimerGetTimerID(timer);
    
    xQueueSendToBack(i2c_wdt_task.queue_handle, &i2c_number, portMAX_DELAY);
}

void drive_task_i2c_watchdog_set_reset_callback(uint32_t i2c_number, reset_i2c_proc_t reset_proc)
{
    if(i2c_number >= DRIVE_TASK_I2C_WATCHDOG_COUNT) return;
    
    i2c_wdt_task.reset_proc[i2c_number] = reset_proc;
}

void drive_task_i2c_watchdog_start(uint32_t i2c_number, uint32_t timeout)
{
    if(i2c_number >= DRIVE_TASK_I2C_WATCHDOG_COUNT) return;
    
    xTimerChangePeriod(i2c_wdt_task.timer_handle[i2c_number], pdMS_TO_TICKS(timeout), portMAX_DELAY);
}

void drive_task_i2c_watchdog_reset(uint32_t i2c_number)
{
    if(i2c_number >= DRIVE_TASK_I2C_WATCHDOG_COUNT) return;
    
    xTimerStart(i2c_wdt_task.timer_handle[i2c_number], portMAX_DELAY);
}

void drive_task_i2c_watchdog_stop(uint32_t i2c_number)
{
    if(i2c_number >= DRIVE_TASK_I2C_WATCHDOG_COUNT) return;
    
    xTimerStop(i2c_wdt_task.timer_handle[i2c_number], portMAX_DELAY);
}
