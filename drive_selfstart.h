/**
 * @file drive_selfstart.h Библиотека самозапуска привода.
 */

#ifndef DRIVE_SELFSTART_H
#define DRIVE_SELFSTART_H

#include "errors/errors.h"
#include <stdbool.h>

/**
 * Инициализирует самозапуск.
 */
extern void drive_selfstart_init(void);

/**
 * Обновляет настройки самозапуска.
 */
extern void drive_selfstart_update_settings(void);

/**
 * Получает флаг разрешения самозапуска.
 * @return Флаг разрешения самозапуска.
 */
extern bool drive_selfstart_enabled(void);

/**
 * Оповещает самозапуск о возникновении ошибки. 
 */
extern void drive_selfstart_on_error(void);

/**
 * Получает флаг процесса самозапуска.
 * @return Флаг процесса самозапуска.
 */
extern bool drive_selfstart_starting(void);

/**
 * Получает флаг допустимости сброса ошибок.
 * @return Флаг допустимости сброса ошибок.
 */
extern bool drive_selfstart_can_clear_errors(void);

/**
 * Получает флаг необходимости сброса ошибок.
 * @return Флаг необходимости сброса ошибок.
 */
extern bool drive_selfstart_need_clear_errors(void);

/**
 * Оповещает самозапуск о сбросе ошибок.
 */
extern void drive_selfstart_errors_cleared(void);

/**
 * Получает флаг необходимости запуска привода.
 * @return Флаг необходимости запуска привода.
 */
extern bool drive_selfstart_need_start(void);

/**
 * Оповещает самозапуск о запуске.
 * Устанавливает таймаут следующего самозапуска.
 */
extern void drive_selfstart_started(void);

/**
 * Завершает самозапуск.
 */
extern void drive_selfstart_done(void);

#endif /* DRIVE_SELFSTART_H */
