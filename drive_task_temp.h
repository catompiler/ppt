/**
 * @file drive_task_temp.h Задача обновления температуры радиатора.
 */

#ifndef DRIVE_TASK_TEMP_H
#define DRIVE_TASK_TEMP_H

#include "errors/errors.h"
#include <stdint.h>


/**
 * Инициалиазирует задачу обновления температуры радиатора.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_temp_init(uint32_t priority);

#endif /* DRIVE_TASK_TEMP_H */

