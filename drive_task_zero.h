/**
 * @file drive_task_zero.h Задача обработки нуля сети.
 */

#ifndef DRIVE_TASK_ZERO_H
#define DRIVE_TASK_ZERO_H

#include "errors/errors.h"
#include <stdint.h>
#include <stdbool.h>


/**
 * Инициалиазирует задачу обработки нуля сети.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_zero_init(uint32_t priority);

/**
 * Обрабатывает ноль сети.
 * @return Флаг необходимости переключения контекста.
 */
extern bool drive_task_zero_process_isr(void);


#endif /* DRIVE_TASK_ZERO_H */

