/**
 * @file drive_task_utils.h Задача утилит.
 */
#ifndef DRIVE_TASK_UTILS_H
#define DRIVE_TASK_UTILS_H

#include "errors/errors.h"
#include <stdint.h>


/**
 * Создаёт задачу настроек.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_utils_init(uint32_t priority);

/**
 * Запускает применение настроек.
 */
extern void drive_task_settings_apply(void);

#endif /* DRIVE_TASK_UTILS_H */
