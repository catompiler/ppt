/**
 * @file drive_task_triacs.h Задача управления тиристорами.
 */

#ifndef DRIVE_TASK_TRIACS_H
#define DRIVE_TASK_TRIACS_H

#include "errors/errors.h"
#include <stdint.h>
#include <stdbool.h>


/**
 * Инициалиазирует задачу управления тиристорами.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_triacs_init(uint32_t priority);

/**
 * Запускает задачу управления тиристорами.
 * @return Флаг необходимости переключения контекста.
 */
extern bool drive_task_triacs_process_isr(void);


#endif /* DRIVE_TASK_TRIACS_H */

