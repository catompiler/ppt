/**
 * @file drive_task_main.h Основная задача привода.
 */

#ifndef DRIVE_TASK_MAIN_H
#define DRIVE_TASK_MAIN_H

#include "errors/errors.h"
#include <stdint.h>
#include <stdbool.h>


/**
 * Инициалиазирует задачу привода.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_main_init(uint32_t priority);

/**
 * Выполняет очередную итерацию действий и вычислений.
 * @return Флаг необходимости переключения контекста.
 */
extern bool drive_task_main_process_isr(void);


#endif /* DRIVE_TASK_MAIN_H */

