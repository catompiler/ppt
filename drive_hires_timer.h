/**
 * @file drive_hires_timer.h Библиотека таймера высокого разрешения привода.
 */

#ifndef DRIVE_HIRES_TIMER_H
#define DRIVE_HIRES_TIMER_H

#include "errors/errors.h"
#include <stm32f10x.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>



// Параметры таймера.
//! Период таймера в тиках.
#define DRIVE_HIRES_TIMER_PERIOD_TICKS 1000
//! Период таймера в мкс.
#define DRIVE_HIRES_TIMER_PERIOD_US 1000
//! Предделитель таймера.
#define DRIVE_HIRES_TIMER_PRESCALER 72



/**
 * Инициализирует высокоточный таймер привода.
 * @param TIM Периферия таймера.
 * @return Код ошибки.
 */
extern err_t drive_hires_timer_init(TIM_TypeDef* timer);

/**
 * Обработчик прерывания переполнения таймера высокого разрешения.
 */
extern void drive_hires_timer_irq_handler(void);

/**
 * Получает разрешение прерывания переполнения таймера.
 * @return Флаг разрешения прерывания.
 */
extern bool drive_hires_timer_irq_enabled(void);

/**
 * Устанавливает разрешение прерывания переполнения таймера.
 * @param enabled Флаг разрешения прерывания.
 */
extern void drive_hires_timer_irq_set_enabled(bool enabled);

/**
 * Получает флаг режима однократного срабатывания.
 * @return Флаг режима однократного срабатывания.
 */
extern bool drive_hires_timer_one_pulse_mode(void);

/**
 * Устанавливает флаг режима однократного срабатывания.
 * @return Флаг режима однократного срабатывания.
 */
extern bool drive_hires_timer_one_pulse_mode(void);

/**
 * Получает флаг работы таймера.
 * @return Флаг работы таймера.
 */
extern bool drive_hires_timer_running(void);

/**
 * Устанавливает флаг работы таймера.
 * @param running Флаг работы таймера.
 */
extern void drive_hires_timer_set_running(bool running);

/**
 * Запускает таймер.
 */
extern void drive_hires_timer_start(void);

/**
 * Останавливает таймер.
 */
extern void drive_hires_timer_stop(void);

/**
 * Сбрасывает счётчик таймера.
 */
extern void drive_hires_timer_reset(void);

/**
 * Получает время работы таймера.
 * Переполняется каждые 4294967.295 секунд.
 * @param tv Время.
 */
extern void drive_hires_timer_value(struct timeval* tv);

#endif /* DRIVE_HIRES_TIMER_H */

