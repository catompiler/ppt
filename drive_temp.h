/**
 * @file drive_temp.h Библиотека температуры привода.
 */

#ifndef DRIVE_TEMP_H
#define DRIVE_TEMP_H

#include "errors/errors.h"
#include "fixed/fixed32.h"
#include "lm75/lm75.h"
#include <sys/time.h>


//! Тип функции сброса датчика/шины.
typedef void (*drive_temp_sensor_reset_proc_t)(void);

/**
 * Тип функции установки оборотов вентилятора.
 * @param rpm_percents Обороты вентилятора в %.
 */
typedef void(*drive_temp_set_fan_rpm_proc_t)(uint32_t rpm_percents);

//! Структура инициализации температуры привода.
typedef struct _Drive_Temp_Init {
    lm75_t* heatsink_sensor; //!< Датчик температуры радиатора.
    drive_temp_sensor_reset_proc_t heatsink_sensor_reset_proc; //!< Функция сброса датчика температуры радиатора.
    struct timeval* heatsink_sensor_io_timeout; //!< Таймаут обмена данными с датчиком температуры радиатора.
    struct timeval* heatsink_sensor_timeout; //!< Таймаут попыток обмена данными с датчиком температуры радиатора.
    struct timeval* update_interval; //!< Интервал обновления температур.
    drive_temp_set_fan_rpm_proc_t set_fan_rpm_proc; //!< Функция установки оборотов вентилятора.
} drive_temp_init_t;


/**
 * Инициализирует температуру привода.
 * @param temp_is Структура инициализации.
 * @return Код ошибки.
 */
extern err_t drive_temp_init(drive_temp_init_t* temp_is);

/**
 * Обновляет настройки температуры привода.
 */
extern void drive_temp_update_settings(void);

/**
 * Обновляет температуру привода.
 * @return Флаг обновления температуры.
 */
extern bool drive_temp_update(void);

/**
 * Выполняет регулирование оборотов вентилятора.
 */
extern void drive_temp_regulate_fan(void);

/**
 * Получает доступность температуры радиатора.
 * @return Доступность температуры радиатора.
 */
extern bool drive_temp_heatsink_temp_avail(void);

/**
 * Получает температуру радиатора.
 * @return Температура радиатора.
 */
extern fixed32_t drive_temp_heatsink_temp(void);

/**
 * Получает обороты вентилятора в %.
 * @return Обороты вентилятора в %.
 */
extern uint32_t drive_temp_fan_rpm(void);

/**
 * Получает флаг работы вентилятора.
 * @return Флаг работы вентилятора.
 */
extern bool drive_temp_fan_running(void);

#endif /* DRIVE_TEMP_H */
