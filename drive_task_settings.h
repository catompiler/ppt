/**
 * @file drive_task_settings.h Задача настроек.
 */
#ifndef DRIVE_TASK_SETTINGS_H
#define DRIVE_TASK_SETTINGS_H

#include "errors/errors.h"
#include <stdint.h>


/**
 * Создаёт задачу настроек.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_settings_init(uint32_t priority);

/**
 * Запускает применение настроек.
 */
extern void drive_task_settings_apply(void);

/**
 * Запускает сохранение параметров.
 */
extern void drive_task_settings_save(void);


#endif /* DRIVE_TASK_SETTINGS_H */
