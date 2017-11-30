/**
 * @file drive_task_sync.h Задача обработки синхронизации с фазами.
 */

#ifndef DRIVE_TASK_SYNC_H
#define DRIVE_TASK_SYNC_H

#include "errors/errors.h"
#include <stdint.h>
#include <stdbool.h>


/**
 * Инициалиазирует задачу обработки нуля сети.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_sync_init(uint32_t priority);

/**
 * Обрабатывает ноль сети.
 * @return Флаг необходимости переключения контекста.
 */
extern bool drive_task_sync_process_isr(void);


#endif /* DRIVE_TASK_SYNC_H */

