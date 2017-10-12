/**
 * @file drive_overload.h Библиотека перегруза привода.
 */

#ifndef DRIVE_OVERLOAD_H
#define DRIVE_OVERLOAD_H

#include "errors/errors.h"
#include "fixed/fixed32.h"
#include <stdbool.h>
#include <stdint.h>


//! Тип режима перегруза привода.
typedef enum _Drive_Overload_Mode {
    DRIVE_OVERLOAD_MODE_NORMAL = 0, //!< Нормальный режим счёта времени перегруза.
    DRIVE_OVERLOAD_MODE_THERMAL = 1 //!< Режим счёта времени перегруза I^2 * t.
} drive_overload_mode_t;


/**
 * Инициализирует перегруз привода.
 */
extern void drive_overload_init(void);

/**
 * Обновляет настройки перегруза привода.
 * @return Код ошибки.
 */
extern err_t drive_overload_update_settings(void);

/**
 * Получает флаг разрешения перегруза.
 * @return Флаг разрешения перегруза.
 */
extern bool drive_overload_enabled(void);

/**
 * Получает флаг достижения максимального перегруза.
 * @return Флаг достижения максимального перегруза.
 */
extern bool drive_overload_overloaded(void);

/**
 * Получает допустимый ток якоря.
 * @return Допустимый ток якоря.
 */
extern fixed32_t drive_overload_avail_current(void);

/**
 * Осуществляет контроль перегруза привода.
 * @param dt Интервал времени с предыдущей итерации.
 */
extern void drive_overload_process(fixed32_t dt);

#endif /* DRIVE_OVERLOAD_H */

