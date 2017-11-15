/**
 * @file drive_task_i2c_watchdog.h Задача сброса i2c по тайм-ауту.
 */
#ifndef DRIVE_TASK_I2C_WATCHDOG_H
#define DRIVE_TASK_I2C_WATCHDOG_H

#include "errors/errors.h"
#include <stdint.h>


// Число шин i2c.
#define DRIVE_TASK_I2C_WATCHDOG_COUNT 2


//! Функция сброса i2c.
typedef void (*reset_i2c_proc_t)(uint32_t i2c_number);

/**
 * Создаёт задачу сторожа i2c.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_i2c_watchdog_init(uint32_t priority);

/**
 * Устанавливает функцию сброса i2c.
 * @param reset_proc Функция сброса i2c.
 */
extern void drive_task_i2c_watchdog_set_reset_callback(uint32_t i2c_number, reset_i2c_proc_t reset_proc);

/**
 * Запускает сторожевой таймер.
 * @param timeout Тайм-аут в мс.
 */
extern void drive_task_i2c_watchdog_start(uint32_t i2c_number, uint32_t timeout);

/**
 * Сбрасывает сторожевой таймер.
 */
extern void drive_task_i2c_watchdog_reset(uint32_t i2c_number);

/**
 * Останавливает сторожевой таймер.
 */
extern void drive_task_i2c_watchdog_stop(uint32_t i2c_number);


#endif /* DRIVE_TASK_I2C_WATCHDOG_H */

