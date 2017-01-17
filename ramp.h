/**
 * @file ramp.h Библиотека разгона.
 */

#ifndef RAMP_H
#define	RAMP_H

#include <stdint.h>
#include <stdbool.h>
#include "defs/defs.h"
#include "errors/errors.h"
#include "fixed/fixed32.h"


//! Тип структуры разгона.
typedef struct _Ramp {
    fixed32_t current_ref; //!< Текущее задание.
    fixed32_t target_ref; //!< Установленное задание.
    fixed32_t step_ref; //!< Шаг за период.
} ramp_t;

//! Тип задания разгона.
typedef uint32_t ramp_reference_t;

//! Минимальное задание.
#define RAMP_REFERENCE_MIN 0
//! Максимальное задание.
#define RAMP_REFERENCE_MAX 100

//! Тип времени разгона.
typedef int32_t ramp_time_t;

//! Максимальное время разгона.
#define RAMP_TIME_MIN 1

//! Максимальное время разгона.
#define RAMP_TIME_MAX 300

//! Время разгона по-умолчанию.
#define RAMP_TIME_DEFAULT 30



/**
 * Инициализирует разгон.
 * @param ramp Разгон.
 * @return Код ошибки.
 */
extern err_t ramp_init(ramp_t* ramp);

/**
 * Устанавливает время разгона.
 * @param ramp Разгон.
 * @param time Время разгона.
 * @param step_dt Время шага разгона.
 * @return Код ошибки.
 */
extern err_t ramp_set_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt);

/**
 * Устанавливает целевое задание разгона.
 * @param ramp Разгон.
 * @param reference Задание.
 * @return Целевое задание.
 */
extern err_t ramp_set_target_reference(ramp_t* ramp, ramp_reference_t reference);

/**
 * Сбрасывает текущее задание.
 * @param ramp Разгон.
 */
ALWAYS_INLINE static void ramp_reset_current(ramp_t* ramp)
{
    ramp->current_ref = 0;
}

/**
 * Сбрасывает текущее и целевое задание.
 * @param ramp Разгон.
 */
ALWAYS_INLINE static void ramp_reset_reference(ramp_t* ramp)
{
    ramp->current_ref = 0;
    ramp->target_ref = 0;
}

/**
 * Заканчивает разгон.
 * @param ramp Разгон.
 */
ALWAYS_INLINE static void ramp_flush(ramp_t* ramp)
{
    ramp->current_ref = ramp->target_ref;
}

/**
 * Получает целевое задание разгона.
 * @param ramp Разгон.
 * @return Целевое задание разгона.
 */
ALWAYS_INLINE static ramp_reference_t ramp_target_reference(ramp_t* ramp)
{
    return fixed32_get_int(ramp->target_ref);
}

/**
 * Получает текущее задание разгона.
 * @param ramp Разгон.
 * @return Текущее задание разгона.
 */
ALWAYS_INLINE static fixed32_t ramp_current_reference(ramp_t* ramp)
{
    return ramp->current_ref;
}

/**
 * Получает шаг изменения задания.
 * @param ramp Разгон.
 * @return Шаг изменения задания.
 */
ALWAYS_INLINE static fixed32_t ramp_reference_step(ramp_t* ramp)
{
    return ramp->step_ref;
}

/**
 * Получает флаг завершения разгона или торможения.
 * @param ramp Разгон.
 * @return Флаг завершения разгона или торможения.
 */
ALWAYS_INLINE static bool ramp_done(ramp_t* ramp)
{
    return ramp->current_ref == ramp->target_ref;
}

/**
 * Увеличивает задание.
 * @param ramp Разгон.
 * @return Флаг увеличения задания.
 */
extern bool ramp_inc_reference(ramp_t* ramp);

/**
 * Уменьшает задание.
 * @param ramp Разгон.
 * @return Флаг увеличения задания.
 */
extern bool ramp_dec_reference(ramp_t* ramp);

/**
 * Вычисляет очередной шаг разгона.
 * @param ramp Разгон.
 * @return Флаг завершения разгона.
 */
extern bool ramp_calc_step(ramp_t* ramp);

#endif	/* RAMP_H */
