/**
 * @file drive_phase_state.h Библиотека отслеживания состояния фаз.
 */

#ifndef DRIVE_PHASE_STATE_H
#define DRIVE_PHASE_STATE_H

#include <stm32f10x.h>
#include <stdint.h>
#include "errors/errors.h"


/*
 * Конфигурация таймера для отсчёта времени между датчиками нуля.
 */
//! Число тиков.
#define DRIVE_PHASE_STATE_TIMER_TICKS (30000)
//! Период в микросекундах.
#define DRIVE_PHASE_STATE_TIMER_PERIOD_US (30000)
//! Период.
#define DRIVE_PHASE_STATE_TIMER_PERIOD (DRIVE_PHASE_STATE_TIMER_TICKS - 1)
//! Предделитель.
#define DRIVE_PHASE_STATE_TIMER_PRESCALER (72 - 1)


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
    PHASE_NO_ERROR = 0,
    PHASE_INVALID,
    PHASE_A_ERROR,
    PHASE_B_ERROR,
    PHASE_C_ERROR,
    PHASE_AB_ERROR,
    PHASE_BC_ERROR,
    PHASE_AC_ERROR,
    PHASE_A_TIME_ERROR,
    PHASE_B_TIME_ERROR,
    PHASE_C_TIME_ERROR
} drive_phase_error_t;


/*
 * Функции отслеживания состояния фаз.
 */

/**
 * Инициализирует состояние фаз.
 * @return Код ошибки.
 */
extern err_t drive_phase_state_init(void);

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
 * Получает ошибку.
 * @return Ошибка.
 */
extern drive_phase_error_t drive_phase_state_error(void);

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
 * Получает следующую фазу после заданной при заданном направлении вращения.
 * @param phase Фаза.
 * @param dir Направление вращения.
 * @return Следующая фаза.
 */
extern phase_t drive_phase_state_next_phase(phase_t phase, drive_dir_t dir);

/**
 * Сбрасывает ошибку.
 */
extern void drive_phase_state_clear_error(void);

/**
 * Сбрасывает состояние.
 */
extern void drive_phase_state_reset(void);

#endif  //DRIVE_PHASE_STATE_H
