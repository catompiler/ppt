/**
 * @file drive_nvdata.h Библиотека энергонезависимых данных привода.
 */

#ifndef DRIVE_NVDATA_H
#define DRIVE_NVDATA_H

#include <stdint.h>
#include <stdbool.h>
#include "errors/errors.h"
#include <sys/types.h>


/**
 * Получает флаг валидности энергонезависимых данных привода.
 * @return Флаг валидности энергонезависимых данных привода.
 */
extern bool drive_nvdata_valid(void);

/**
 * Очищает энергонезависимые данные привода.
 * @return Код ошибки.
 */
extern void drive_nvdata_clear(void);

/**
 * Получает общее время включения привода.
 * @return Общее время включения привода.
 */
extern time_t drive_nvdata_lifetime(void);

/**
 * Устанавливает общее время включения привода.
 * @param lifetime Общее время включения привода.
 */
extern void drive_nvdata_set_lifetime(time_t lifetime);

/**
 * Получает общее время работы привода.
 * @return Общее время работы привода.
 */
extern time_t drive_nvdata_runtime(void);

/**
 * Устанавливает общее время работы привода.
 * @param lifetime Общее время работы привода.
 */
extern void drive_nvdata_set_runtime(time_t runtime);

#endif /* DRIVE_NVDATA_H */

