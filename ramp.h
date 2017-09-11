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



//! Режим останова.
typedef enum _Ramp_Stop_Mode {
    RAMP_STOP_MODE_NORMAL = 0, //!< Нормальный останов.
    RAMP_STOP_MODE_FAST   = 1  //!< Быстрый останов.
} ramp_stop_mode_t;

//! Тип структуры разгона.
typedef struct _Ramp {
    fixed32_t current_ref; //!< Текущее задание.
    fixed32_t target_ref; //!< Установленное задание.
    fixed32_t reference_step; //!< Шаг задания за период.
    fixed32_t start_step; //!< Шаг разгона за период.
    fixed32_t stop_step; //!< Шаг останова за период.
    fixed32_t fast_stop_step; //!< Шаг быстрого останова за период.
    ramp_stop_mode_t stop_mode; //!< Режим останова.
} ramp_t;

//! Тип задания разгона.
typedef uint32_t ramp_reference_t;

//! Минимальное задание.
#define RAMP_REFERENCE_MIN 0
#define RAMP_REFERENCE_MIN_F (fixed32_make_from_int(RAMP_REFERENCE_MIN))
//! Максимальное задание.
#define RAMP_REFERENCE_MAX 100
#define RAMP_REFERENCE_MAX_F (fixed32_make_from_int(RAMP_REFERENCE_MAX))

//! Тип времени разгона.
typedef fixed32_t ramp_time_t;

//! Минимальное время разгона.
#define RAMP_TIME_MIN 0x1999 //0.1

//! Максимальное время разгона.
#define RAMP_TIME_MAX 0x12c0000 //300

//! Время изменения задания по-умолчанию.
#define RAMP_REFERENCE_TIME_DEFAULT 0xa0000 //10

//! Время разгона по-умолчанию.
#define RAMP_START_TIME_DEFAULT 0x1e0000 //30

//! Время разгона по-умолчанию.
#define RAMP_STOP_TIME_DEFAULT 0x140000 //20

//! Время разгона по-умолчанию.
#define RAMP_FAST_STOP_TIME_DEFAULT 0x20000 //2



/**
 * Инициализирует разгон.
 * @param ramp Разгон.
 * @return Код ошибки.
 */
extern err_t ramp_init(ramp_t* ramp);

/**
 * Устанавливает время изменения задания от 0 до 100%.
 * @param ramp Разгон.
 * @param time Время изменения задания.
 * @param step_dt Время шага разгона.
 * @return Код ошибки.
 */
extern err_t ramp_set_reference_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt);

/**
 * Устанавливает время разгона.
 * @param ramp Разгон.
 * @param time Время разгона.
 * @param step_dt Время шага разгона.
 * @return Код ошибки.
 */
extern err_t ramp_set_start_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt);

/**
 * Устанавливает время останова.
 * @param ramp Разгон.
 * @param time Время останова.
 * @param step_dt Время шага разгона.
 * @return Код ошибки.
 */
extern err_t ramp_set_stop_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt);

/**
 * Устанавливает время быстрого останова.
 * @param ramp Разгон.
 * @param time Время быстрого останова.
 * @param step_dt Время шага разгона.
 * @return Код ошибки.
 */
extern err_t ramp_set_fast_stop_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt);

/**
 * Получает режим останова.
 * @param ramp Разгон.
 * @return Режим останова.
 */
ALWAYS_INLINE static ramp_stop_mode_t ramp_stop_mode(ramp_t* ramp)
{
    return ramp->stop_mode;
}

/**
 * Устанавливает режим останова.
 * @param ramp Разгон.
 * @param mode Режим останова.
 */
ALWAYS_INLINE static void ramp_set_stop_mode(ramp_t* ramp, ramp_stop_mode_t mode)
{
    ramp->stop_mode = mode;
}

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
    return ramp->reference_step;
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
 * Устанавливает текущее задание согласно текущему
 * значению питания.
 * @param ramp Разгон.
 * @param cur_val Текущее значение питания.
 * @param max_val Значение при 100% задания.
 */
extern void ramp_adjust_current_reference(ramp_t* ramp, fixed32_t cur_val, fixed32_t max_val);

/**
 * Вычисляет очередной шаг разгона.
 * @param ramp Разгон.
 * @return Флаг завершения разгона.
 */
extern bool ramp_calc_step(ramp_t* ramp);

#endif	/* RAMP_H */
