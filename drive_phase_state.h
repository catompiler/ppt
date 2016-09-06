/**
 * @file drive_phase_state.h Библиотека отслеживания состояния фаз.
 */

#ifndef DRIVE_PHASE_STATE_H
#define DRIVE_PHASE_STATE_H

#include <stm32f10x.h>
#include <stdint.h>
#include "errors/errors.h"


//! Число фаз.
#define PHASES_COUNT 3

//! Идеальное время между фазами.
#define PHASE_TIME_US 6667
//! Максимальная разница во времени между фазами.
#define PHASE_DELTA_US_MAX 250 // 0.25 мс
//! Минимальный допустимый интервал времени между фазами.
#define PHASE_TIME_US_MIN (PHASE_TIME_US - PHASE_DELTA_US_MAX)
//! Максимальный допустимый интервал времени между фазами.
#define PHASE_TIME_US_MAX (PHASE_TIME_US + PHASE_DELTA_US_MAX)


/*
 * Конфигурация таймера для отсчёта времени между датчиками нуля.
 */
//! Число тиков.
#define DRIVE_PHASE_STATE_TIMER_CNT_TICKS (PHASE_TIME_US_MAX + 1)
//! Период в микросекундах.
#define DRIVE_PHASE_STATE_TIMER_CNT_PERIOD_US (DRIVE_PHASE_STATE_TIMER_CNT_TICKS)
//! Период.
#define DRIVE_PHASE_STATE_TIMER_CNT_PERIOD (DRIVE_PHASE_STATE_TIMER_CNT_TICKS - 1)
//! Предделитель.
#define DRIVE_PHASE_STATE_TIMER_CNT_PRESCALER (72 - 1)


/*
 * Структуры для отслеживания состояния фаз.
 */

//! Тип фазы.
typedef enum _Phase {
    PHASE_UNK = 0,
    PHASE_A = 1,
    PHASE_B = 2,
    PHASE_C = 3
} phase_t;

//! Тип направления.
typedef enum _DriveDir {
    DRIVE_DIR_UNK = 0,
    DRIVE_DIR_FORW = 1,
    DRIVE_DIR_BACKW = 2
} drive_dir_t;

//! Тип ошибки.
typedef enum _DrivePhaseErr {
    PHASE_NO_ERROR     = 0,
    PHASE_INVALID      = 0x1,
    PHASE_A_ERROR      = 0x2,
    PHASE_B_ERROR      = 0x4,
    PHASE_C_ERROR      = 0x8,
    PHASE_AB_ERROR     = 0x10,
    PHASE_BC_ERROR     = 0x20,
    PHASE_AC_ERROR     = 0x40,
    PHASE_A_TIME_ERROR = 0x80,
    PHASE_B_TIME_ERROR = 0x100,
    PHASE_C_TIME_ERROR = 0x200
} drive_phase_error_t;

//! Тип ошибок фаз.
typedef uint32_t drive_phase_errors_t;

//! Тип времени между фазами.
typedef uint16_t phase_time_t;

//! Каллбэк ошибки фаз.
typedef void (*drive_phase_state_error_callback_t)(void);

/*
 * Функции отслеживания состояния фаз.
 */

/**
 * Инициализирует состояние фаз.
 * @return Код ошибки.
 */
extern err_t drive_phase_state_init(void);

/**
 * Устанавливает каллбэк при ошибке фаз.
 * @param callback Каллбэк.
 */
extern void drive_phase_state_set_error_callback(drive_phase_state_error_callback_t callback);

/**
 * Устанавливает таймер для отсчёта интервалов между датчиками нуля.
 * @param TIM Таймер.
 * @return Код ошибки.
 */
extern err_t drive_phase_state_set_timer(TIM_TypeDef* TIM);

/**
 * Обрабатывает фазу.
 * @param phase Фаза.
 */
extern void drive_phase_state_handle(phase_t phase);

/**
 * Получает ошибки.
 * @return Ошибки.
 */
extern drive_phase_errors_t drive_phase_state_errors(void);

/**
 * Получает текущую фазу.
 * @return Текущая фаза.
 */
extern phase_t drive_phase_sate_current_phase(void);

/**
 * Получает направление.
 * @return Направление.
 */
extern drive_dir_t drive_phase_state_direction(void);

/**
 * Получает время между датчиками нуля фазы.
 * @param phase Фаза.
 * @return Время между датчиками нуля.
 */
extern phase_time_t drive_phase_state_phase_time(phase_t phase);

/**
 * Получает следующую фазу после заданной при заданном направлении вращения.
 * @param phase Фаза.
 * @param dir Направление вращения.
 * @return Следующая фаза.
 */
extern phase_t drive_phase_state_next_phase(phase_t phase, drive_dir_t dir);

/**
 * Сбрасывает ошибку.
 */
extern void drive_phase_state_clear_errors(void);

/**
 * Сбрасывает состояние.
 */
extern void drive_phase_state_reset(void);

/**
 * Обрабатывает прерывание переполнения таймера отсчёта времени между фазами.
 */
extern void drive_phase_state_timer_irq_handler(void);

#endif  //DRIVE_PHASE_STATE_H
