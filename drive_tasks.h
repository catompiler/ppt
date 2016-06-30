/**
 * @file drive_tasks.h Библиотека задач привода.
 */

#ifndef DRIVE_TASKS_H
#define DRIVE_TASKS_H

#include "errors/errors.h"
#include "future/future.h"

/**
 * Обновляет настройки.
 * @return Код ошибки.
 */
extern err_t drive_tasks_apply_settings(void);

/**
 * Сохраняет настройки.
 * @return Код ошибки.
 */
extern err_t drive_tasks_save_settings(void);

/**
 * Записывает событие
 * ошибки привода.
 * @return Код ошибки.
 */
extern err_t drive_tasks_write_error_event(void);

#endif /* DRIVE_TASKS_H */

