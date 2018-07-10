/**
 * @file drive_task_utils.h Задача утилит.
 */
#ifndef DRIVE_TASK_UTILS_H
#define DRIVE_TASK_UTILS_H

#include "errors/errors.h"
#include <stdint.h>

typedef void (*drive_task_utils_callback_t)(void);


/**
 * Создаёт задачу настроек.
 * @param priority Приоритет.
 * @return Код ошибки.
 */
extern err_t drive_task_utils_init(uint32_t priority);

/**
 * Устанавливает каллбэк сброса UI.
 * @param callback Каллбэк.
 */
extern void drive_task_utils_set_reset_ui_callback(drive_task_utils_callback_t callback);

/**
 * Запускает применение настроек.
 * @return Код ошибки.
 */
extern err_t drive_task_settings_apply(void);

/**
 * Сбрасывает UI.
 * @return Код ошибки.
 */
extern err_t drive_task_utils_reset_ui(void);

#endif /* DRIVE_TASK_UTILS_H */
