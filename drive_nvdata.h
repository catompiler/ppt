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
 * Инициализирует энергонезависимые данные привода.
 */
extern void drive_nvdata_init(void);

/**
 * Чистает энергонезависимые данные привода из флеш-памяти.
 * @return Код ошибки.
 */
extern err_t drive_nvdata_read(void);

/**
 * Записывает энергонезависимые данные привода во флеш-память.
 * @return Код ошибки.
 */
extern err_t drive_nvdata_write(void);

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
 * Обновляет виртуальные параметры,
 * связанные со временем.
 */
extern void drive_nvdata_update_time_params(void);

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
 * Увеличивает время включения привода.
 */
extern void drive_nvdata_inc_lifetime(void);


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

/**
 * Увеличивает время работы привода.
 */
extern void drive_nvdata_inc_runtime(void);


/**
 * Получает время работы привода после включения.
 * @return Время работы привода после включения.
 */
extern time_t drive_nvdata_last_runtime(void);

/**
 * Устанавливает время работы привода после включения.
 * @param lifetime Время работы привода после включения.
 */
extern void drive_nvdata_set_last_runtime(time_t runtime);

/**
 * Увеличивает время работы привода после включения.
 */
extern void drive_nvdata_inc_last_runtime(void);


/**
 * Получает общее время работы вентилятора привода.
 * @return Общее время работы вентилятора привода.
 */
extern time_t drive_nvdata_fan_runtime(void);

/**
 * Устанавливает общее время работы вентилятора привода.
 * @param time Общее время работы вентилятора привода.
 */
extern void drive_nvdata_set_fan_runtime(time_t runtime);

/**
 * Увеличивает время работы вентилятора привода.
 * @param rpm_percents Обороты вентилятора в % от номинальных.
 */
extern void drive_nvdata_inc_fan_runtime(uint32_t rpm_percents);

/**
 * Сбрасывает время работы вентилятора привода.
 */
extern void drive_nvdata_reset_fan_runtime(void);

#endif /* DRIVE_NVDATA_H */

