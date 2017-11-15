/**
 * @file drive_task_buzz.h Задача бибикания бузером.
 */

#ifndef DRIVE_TASK_BUZZ_H
#define DRIVE_TASK_BUZZ_H

#include "errors/errors.h"
#include <stdint.h>


/**
 * Инициалиазирует задачу бибикания бузером.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_buzz_init(uint32_t priority);

/**
 * Бибикате бузером.
 */
extern void drive_task_buzz_beep(void);


#endif /* DRIVE_TASK_BUZZ_H */

