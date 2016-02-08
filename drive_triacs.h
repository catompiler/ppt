/**
 * @file drive_triacs.h Библиотека тиристоров привода.
 */

#ifndef DRIVE_TRIACS_H
#define DRIVE_TRIACS_H

#include <stm32f10x.h>
#include <stddef.h>
#include "errors/errors.h"
#include "phase_state/phase_state.h"


// Тиристоры.
//! Число пар тиристоров.
#define TRIAC_PAIRS_COUNT 6
// Алиасы пар тиристоров.
//! Пара 3 - 6.
#define TRIAC_PAIR_3_6  0
//! Пара 3 - 2.
#define TRIAC_PAIR_3_2  1
//! Пара 5 - 2.
#define TRIAC_PAIR_5_2  2
//! Пара 5 - 4.
#define TRIAC_PAIR_5_4  3
//! Пара 1 - 4.
#define TRIAC_PAIR_1_4  4
//! Пара 1 - 6.
#define TRIAC_PAIR_1_6  5

//! Тип номера тиристора.
typedef size_t triac_pair_number_t;


/**
 * Таймеры для открытия тиристорных пар.
 * Используются каналы сравнения:
 * Канал1 - Первая пара тиристоров, открытие.
 * Канал2 - Первая пара тиристоров, закрытие.
 * Канал3 - Вторая пара тиристоров, открытие.
 * Канал4 - Вторая пара тиристоров, закрытие.
 */
//! Число таймеров для открытия тиристоров.
#define TRIACS_TIMERS_COUNT 2
// Номера таймеров для открытия тиристоров.
//! Таймер 0.
#define TRIACS_TIMER_0 0
//! Таймер 1.
#define TRIACS_TIMER_1 1
//! Число необходимых каналов выхода/сравнения на таймер.
#define TRIACS_TIMER_OC_CHANNELS 4
//! Канал открытия тиристорной пары А.
#define TRIACS_A_OPEN_CHANNEL      TIM_Channel_1
//! Флаг прерывания канала открытия тиристорной пары А.
#define TRIACS_A_OPEN_CHANNEL_IT   TIM_IT_CC1
//! Канал закрытия тиристорной пары А.
#define TRIACS_A_CLOSE_CHANNEL     TIM_Channel_2
//! Флаг прерывания канала закрытия тиристорной пары А.
#define TRIACS_A_CLOSE_CHANNEL_IT  TIM_IT_CC2
//! Канал открытия тиристорной пары Б.
#define TRIACS_B_OPEN_CHANNEL      TIM_Channel_3
//! Флаг прерывания канала открытия тиристорной пары Б.
#define TRIACS_B_OPEN_CHANNEL_IT   TIM_IT_CC3
//! Канал закрытия тиристорной пары Б.
#define TRIACS_B_CLOSE_CHANNEL     TIM_Channel_4
//! Флаг прерывания канала закрытия тиристорной пары Б.
#define TRIACS_B_CLOSE_CHANNEL_IT  TIM_IT_CC4

//! Параметры таймеров для открытия тиристорных пар.
//! Число тиков за период.
#define TRIACS_TIM_TICKS (36000UL)
//! Период в микросекундах.
#define TRIACS_TIM_PERIOD_US (20000)
//! Значение счётчика за период.
#define TRIACS_TIM_PERIOD (TRIACS_TIM_TICKS - 1)
//! Предделитель.
#define TRIACS_TIM_PRESCALER (40 - 1)
//! Время в тиках таймера открытия тиристоров.
#define TRIACS_TIM_OPEN_TIME_DEFAULT (180)


/**
 * Таймер октрытия симистора возбуждения.
 * Используются каналы сравнения:
 * Канал1 - Первый полупериод, открытие.
 * Канал2 - Первый полупериод, закрытие.
 * Канал3 - Второй полупериод, открытие.
 * Канал4 - Второй полупериод, закрытие.
 */
//! Число таймеров для открытия тиристоров.
#define TRIAC_EXC_TIMERS_COUNT 1
//! Число необходимых каналов выхода/сравнения на таймер.
#define TRIAC_EXC_TIMER_OC_CHANNELS 4
//! Канал открытия в первом полупериоде.
#define TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL      TIM_Channel_1
//! Флаг прерывания канала открытия в первом полупериоде.
#define TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL_IT   TIM_IT_CC1
//! Канал закрытия в первом полупериоде.
#define TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL     TIM_Channel_2
//! Флаг прерывания канала закрытия в первом полупериоде.
#define TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL_IT  TIM_IT_CC2
//! Канал открытия во втором полупериоде.
#define TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL      TIM_Channel_3
//! Флаг прерывания канала открытия во втором полупериоде.
#define TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL_IT   TIM_IT_CC3
//! Канал закрытия во втором полупериоде.
#define TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL     TIM_Channel_4
//! Флаг прерывания канала закрытия во втором полупериоде.
#define TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL_IT  TIM_IT_CC4


//! Параметры таймеров для открытия симистора возбуждения.
//! Число тиков за период.
#define TRIAC_EXC_TIM_TICKS (36000UL)
//! Период в микросекундах.
#define TRIAC_EXC_TIM_PERIOD_US (20000)
//! Значение счётчика за период.
#define TRIAC_EXC_TIM_PERIOD (TRIACS_TIM_TICKS - 1)
//! Предделитель.
#define TRIAC_EXC_TIM_PRESCALER (40 - 1)
//! Время в тиках таймера открытия тиристоров.
#define TRIAC_EXC_TIM_OPEN_TIME_DEFAULT (180)


//! Максимальный угол открытия тиристоров.
#define TRIACS_PAIRS_ANGLE_MAX (120)
//! Минимальный угол открытия тиристоров.
#define TRIACS_PAIRS_ANGLE_MIN (6)
//! Максимальный угол включения симистора возбуждения.
#define TRIAC_EXC_ANGLE_MAX (170)
//! Минимальный угол включения симистора возбуждения.
#define TRIAC_EXC_ANGLE_MIN (30)

//! Перевод времени открытия в тики таймера.
#define OPEN_TIME_TO_TICKS(T) (((uint32_t)T * TRIACS_TIM_PERIOD) / TRIACS_TIM_PERIOD_US)
//! Перевод тиков таймера в время открытия.
#define OPEN_TICKS_TO_TIME(T) (((uint32_t)T * TRIACS_TIM_PERIOD_US) / TRIACS_TIM_PERIOD)


/**
 * Инициализирует тиристоры привода.
 * @return Код ошибки.
 */
extern err_t drive_triacs_init(void);

/**
 * Останавливает открытие тиристоров.
 */
extern void drive_triacs_stop(void);

/**
 * Устанавливает угол открытия тиристорных пар.
 * @param angle Угол открытия.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_pairs_open_angle(uint32_t angle);

/**
 * Устанавливает угол открытия симистора возбуждения.
 * @param angle Угол открытия.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_exc_open_angle(uint32_t angle);

/**
 * Получает время открытия тиристоров.
 * @return Время открытия тиристоров в мкс.
 */
extern uint16_t drive_triacs_open_time_us(void);

/**
 * Устанавливает время открытия тиристоров.
 * @param time Время открытия тиристоров в мкс.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_open_time_us(uint16_t time);

/**
 * Получает время открытия симистора возбуждения.
 * @return Время открытия симистора возбуждения в мкс.
 */
extern uint16_t drive_triacs_exc_open_time_us(void);

/**
 * Устанавливает время открытия симистора возбуждения.
 * @param time Время открытия симистора возбуждения в мкс.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_exc_open_time_us(uint16_t time);

/**
 * Получает фазу возбуждения.
 * @return Фаза возбуждения.
 */
extern phase_t drive_triacs_exc_phase(void);

/**
 * Устанавливает фазу возбуждения.
 * @param phase Фаза возбуждения.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_exc_phase(phase_t phase);

/**
 * Устанавливает порт вывода для заданной пары тиристоров.
 * @param triac_pair Номер пары тиристоров.
 * @param GPIO_a Порт тиристора А.
 * @param pin_a Пин тиристора А.
 * @param GPIO_b Порт тиристора Б.
 * @param pin_b Пин тиристора Б.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_pair_gpio(triac_pair_number_t triac_pair, GPIO_TypeDef* GPIO_a, uint16_t pin_a, GPIO_TypeDef* GPIO_b, uint16_t pin_b);

/**
 * Устанавливает порт тиристора возбуждения.
 * @param GPIO Порт тиристора возбуждения.
 * @param pin Пин тиристора возбуждения.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_exc_gpio(GPIO_TypeDef* GPIO, uint16_t pin);

/**
 * Устанавливает таймер для открытия тиристоров.
 * @param index Индекс таймера.
 * @param TIM Таймер.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_pairs_timer(size_t index, TIM_TypeDef* TIM);

/**
 * Устанавливает таймер для открытия симистора возбуждения.
 * @param TIM Таймер.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_exc_timer(TIM_TypeDef* TIM);

/**
 * Обработчик прерывания таймера 0 открытия тиристоров.
 */
extern void drive_triacs_timer0_irq_handler(void);

/**
 * Обработчик прерывания таймера 1 открытия тиристоров.
 */
extern void drive_triacs_timer1_irq_handler(void);

/**
 * Обработчик прерывания таймера открытия тиристора возбуждения.
 */
extern void drive_triacs_exc_timer_irq_handler(void);

/**
 * Настраивает таймер открытия тиристорных пар.
 * @param phase Текущая фаза.
 * @return Код ошибки.
 */
extern err_t drive_triacs_setup_next_pairs(phase_t phase);

/**
 * Настраивает таймер открытия симистора возбуждения.
 * @param phase Текущая фаза.
 * @param exc_phase Фаза возбуждения.
 * @return Код ошибки.
 */
extern err_t drive_triacs_setup_exc(phase_t phase);

#endif /* DRIVE_TRIACS_H */
