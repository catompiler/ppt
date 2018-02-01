/**
 * @file drive_triacs.h Библиотека тиристоров привода.
 */

#ifndef DRIVE_TRIACS_H
#define DRIVE_TRIACS_H

#include <stm32f10x.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "fixed/fixed32.h"
#include "errors/errors.h"
#include "drive_phase_state.h"


// Тиристоры.
//! Число тиристоров.
#define TRIACS_COUNT 6
//! Тиристор неизвестен.
#define TRIAC_UNKNOWN (-2)
//! Тиристор отсутствует.
#define TRIAC_NONE (-1)
//! Тиристор 1.
#define TRIAC_1 0
//! Тиристор 2.
#define TRIAC_2 1
//! Тиристор 3.
#define TRIAC_3 2
//! Тиристор 4.
#define TRIAC_4 3
//! Тиристор 5.
#define TRIAC_5 4
//! Тиристор 6.
#define TRIAC_6 5

//! Проверка валидности номера тиристора.
#define TRIAC_VALID(T) (((T) >= 0) && ((T) < TRIACS_COUNT))

//! Тип номера тиристора.
typedef int8_t triac_number_t;

//! Тип положения тиристора в паре.
typedef enum _Triac_Pos {
    TRIAC_POS_HI = 0, //!< Верхний тиристор.
    TRIAC_POS_LO = 1 //!< Нижний тиристор.
} triac_pos_t;

// Тиристорные пары.
//! Число пар тиристоров.
#define TRIAC_PAIRS_COUNT 6
// Алиасы пар тиристоров.
//! Пара неизвестна.
#define TRIAC_PAIR_UNKNOWN (-2)
//! Пара отсутствует.
#define TRIAC_PAIR_NONE (-1)
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

//! Проверка валидности номера тиристорной пары.
#define TRIAC_PAIR_VALID(T) (((T) >= 0) && ((T) < TRIAC_PAIRS_COUNT))

//! Тип номера тиристорной пары.
typedef int8_t triac_pair_number_t;


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
#define TRIAC_EXC_TIM_PERIOD (TRIAC_EXC_TIM_TICKS - 1)
//! Предделитель.
#define TRIAC_EXC_TIM_PRESCALER (40 - 1)
//! Время в тиках таймера открытия тиристоров.
#define TRIAC_EXC_TIM_OPEN_TIME_DEFAULT (180)


//! Максимальный угол открытия тиристоров.
#define TRIACS_PAIRS_ANGLE_MAX (120)
//! Максимальный угол открытия тиристоров в fixed32.
#define TRIACS_PAIRS_ANGLE_MAX_F (fixed32_make_from_int(TRIACS_PAIRS_ANGLE_MAX))
//! Минимальный угол открытия тиристоров.
#define TRIACS_PAIRS_ANGLE_MIN (0)
//! Минимальный угол открытия тиристоров в fixed32.
#define TRIACS_PAIRS_ANGLE_MIN_F (fixed32_make_from_int(TRIACS_PAIRS_ANGLE_MIN))
//! Максимальный угол включения симистора возбуждения.
#define TRIAC_EXC_ANGLE_MAX (180)
//! Максимальный угол включения симистора возбуждения в fixed32.
#define TRIAC_EXC_ANGLE_MAX_F (fixed32_make_from_int(TRIAC_EXC_ANGLE_MAX))
//! Минимальный угол включения симистора возбуждения.
#define TRIAC_EXC_ANGLE_MIN (30)
//! Минимальный угол включения симистора возбуждения в fixed32.
#define TRIAC_EXC_ANGLE_MIN_F (fixed32_make_from_int(TRIAC_EXC_ANGLE_MIN))

//! Перевод времени открытия в тики таймера.
#define TIME_TO_TICKS(T) (((int32_t)T * 18) / 10) // 72M / 40 тиков : 1M мкс
//! Перевод тиков таймера в время открытия.
#define TICKS_TO_TIME(T) (((int32_t)T * 10) / 18) // 1M мкс : 72M / 40 тиков


//! Режим возбуждения.
typedef enum _Drive_Triacs_Exc_Mode {
    DRIVE_TRIACS_EXC_FIXED = 0, //!< Фиксированная подача возбуждения.
    DRIVE_TRIACS_EXC_REGULATED = 1, //!< Регулирование тока возбуждения.
    DRIVE_TRIACS_EXC_EXTERNAL = 2 //!< Внешнее возбуждение.
} drive_triacs_exc_mode_t;


/**
 * Открываемые за период между
 * датчиками нуля тиристорные пары.
 */
typedef enum _Drive_Triacs_Open_Pair {
    DRIVE_TRIACS_OPEN_PAIR_NONE   = 0,
    DRIVE_TRIACS_OPEN_PAIR_FIRST  = 1,
    DRIVE_TRIACS_OPEN_PAIR_SECOND = 2,
    DRIVE_TRIACS_OPEN_PAIR_ALL    = 3,
} drive_triacs_open_pair_t;


//! Тип калбэка открытия пары тиристоров.
typedef void (*drive_triacs_open_pair_callback_t)(triac_pair_number_t pair);


/**
 * Инициализирует тиристоры привода.
 * @return Код ошибки.
 */
extern err_t drive_triacs_init(void);

/**
 * Получает каллбэк открытия пары тиристоров.
 * @return каллбэк открытия пары тиристоров.
 */
extern drive_triacs_open_pair_callback_t drive_triacs_open_pair_callback(void);

/**
 * Устанавливает каллбэк открытия пары тиристоров.
 * @param callback каллбэк открытия пары тиристоров.
 */
extern void drive_triacs_set_open_pair_callback(drive_triacs_open_pair_callback_t callback);

/**
 * Получает фазы с соответсвующим состоянием тиристоров
 * согласно текущей паре тиристоров.
 * @param pair_number Пара тиристоров.
 * @param open_hi Фаза с открытым верхним тиристором.
 * @param open_lo Фаза с открытым нижним тиристором.
 * @param closed Фаза с закрытыми тиристорами.
 * @return Флаг получения фаз.
 */
extern bool drive_triacs_phases_by_pair(triac_pair_number_t pair_number, phase_t* open_hi, phase_t* open_lo, phase_t* closed);

/**
 * Получает флаг открытия тиристора по фазе и положению.
 * @param pair_number Пара тиристоров.
 * @param phase Фаза.
 * @param pos Положение тиристора.
 * @return Флаг открытия тиристора.
 */
extern bool drive_triacs_phase_triac_is_open(triac_pair_number_t pair_number, phase_t phase, triac_pos_t pos);

/**
 * Получает флаг открытия тиристора по номеру тиристора.
 * @param pair_number Пара тиристоров.
 * @param triac_number Номер тиристора.
 * @return Флаг открытия тиристора.
 */
extern bool drive_triacs_triac_is_open(triac_pair_number_t pair_number, triac_number_t triac_number);

/**
 * Получает номер тиристора по фазе и положению.
 * @param phase Фаза.
 * @param pos Положение тиристора.
 * @return Номер тиристора.
 */
extern triac_number_t drive_triacs_phase_triac_by_pos(phase_t phase, triac_pos_t pos);

/**
 * Получает разрешение подачи импульсов на тиристорные пары.
 * @return Разрешение подачи импульсов на тиристорные пары.
 */
extern bool drive_triacs_pairs_enabled(void);

/**
 * Устанавливает разрешение подачи импульсов на тиристорные пары.
 * @param enabled Разрешение подачи импульсов на тиристорные пары.
 */
extern void drive_triacs_set_pairs_enabled(bool enabled);

/**
 * Получает разрешение подачи импульсов на симистор возбуждения.
 * @return Разрешение подачи импульсов на симистор возбуждения.
 */
extern bool drive_triacs_exc_enabled(void);

/**
 * Устанавливает разрешение подачи импульсов на симистор возбуждения.
 * @param enabled Разрешение подачи импульсов на симистор возбуждения.
 */
extern void drive_triacs_set_exc_enabled(bool enabled);

/**
 * Получает режим возбуждения.
 * @return Режим возбуждения.
 */
extern drive_triacs_exc_mode_t drive_triacs_exc_mode(void);

/**
 * Устанавливает режим возбуждения.
 * @param mode Режим возбуждения.
 */
extern void drive_triacs_set_exc_mode(drive_triacs_exc_mode_t mode);

/**
 * Останавливает открытие тиристоров.
 */
extern void drive_triacs_stop(void);

/**
 * Получает открытую пару тиристоров.
 * @return Открытая пара тиристоров.
 */
extern triac_pair_number_t drive_triacs_opened_pair(void);

/**
 * Получает последнюю открытую пару тиристоров.
 * @return Последняя открытая пара тиристоров.
 */
extern triac_pair_number_t drive_triacs_last_opened_pair(void);

/**
 * Устанавливает интервал углов открытия тиристорных пар.
 * @param angle_min Минимальный угол открытия.
 * @param angle_max Максимальный угол открытия.
 * @return Код ошибки.
 */
extern err_t drive_triacs_clamp_pairs_open_angle(fixed32_t angle_min, fixed32_t angle_max);

/**
 * Устанавливает интервал углов открытия симистора возбуждения.
 * @param angle_min Минимальный угол открытия.
 * @param angle_max Максимальный угол открытия.
 * @return Код ошибки.
 */
extern err_t drive_triacs_clamp_exc_open_angle(fixed32_t angle_min, fixed32_t angle_max);

/**
 * Получает угол открытия тиристорных пар.
 * @return Угол открытия.
 */
extern fixed32_t drive_triacs_pairs_open_angle(void);

/**
 * Получает начальный угол открытия тиристорных пар.
 * @return Угол открытия.
 */
extern fixed32_t drive_triacs_pairs_start_open_angle(void);

/**
 * Устанавливает угол открытия тиристорных пар.
 * @param angle Угол открытия.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_pairs_open_angle(fixed32_t angle);

/**
 * Получает угол открытия симистора возбуждения.
 * @return Угол открытия.
 */
extern fixed32_t drive_triacs_exc_open_angle(void);

/**
 * Получает начальный угол открытия симистора возбуждения.
 * @return Угол открытия.
 */
extern fixed32_t drive_triacs_exc_start_open_angle(void);

/**
 * Устанавливает угол открытия симистора возбуждения.
 * @param angle Угол открытия.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_exc_open_angle(fixed32_t angle);

/**
 * Получает время открытия тиристоров.
 * @return Время открытия тиристоров в мкс.
 */
extern uint16_t drive_triacs_pairs_open_time_us(void);

/**
 * Устанавливает время открытия тиристоров.
 * @param time Время открытия тиристоров в мкс.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_pairs_open_time_us(uint16_t time);

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
 * Получает время задержки открытия тиристорных пар в мкс.
 * @return Время задержки открытия тиристорных пар в мкс.
 */
extern uint16_t drive_triacs_pairs_open_delay_us(void);

/**
 * Устанавливает время задержки открытия тиристорных пар в мкс.
 * @param time Время задержки открытия тиристорных пар в мкс.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_pairs_open_delay_us(uint16_t time);

/**
 * Получает время задержки открытия симистора возбуждения в мкс.
 * @return Время задержки открытия симистора возбуждения в мкс.
 */
extern uint16_t drive_triacs_exc_open_delay_us(void);

/**
 * Устанавливает время задержки открытия симистора возбуждения в мкс.
 * @param time Время задержки открытия симистора возбуждения в мкс.
 * @return Код ошибки.
 */
extern err_t drive_triacs_set_exc_open_delay_us(uint16_t time);

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
 * Настраивает таймер открытия тиристорной пары.
 * @param phase Текущая фаза.
 * @param last_open_phase Фаза последней открытой пары тиристоров.
 * @param offset Компенсация времени до запуска открытия тиристоров.
 * @return Код ошибки.
 */
extern err_t drive_triacs_setup_next_pair(phase_t phase, phase_t last_open_phase, int16_t offset);

/**
 * Настраивает таймер открытия симистора возбуждения.
 * @param phase Текущая фаза.
 * @param last_open_phase Фаза последней открытой симистора возбуждения.
 * @param offset Компенсация времени до запуска открытия симистора.
 * @return Код ошибки.
 */
extern err_t drive_triacs_setup_exc(phase_t phase, phase_t last_open_phase, int16_t offset);

#endif /* DRIVE_TRIACS_H */
