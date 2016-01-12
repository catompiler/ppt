/**
 * @file drive.h Реализация логики привода.
 */

#ifndef DRIVE_H
#define	DRIVE_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "errors/errors.h"
#include "defs/defs.h"
#include "phase_state/phase_state.h"
#include "power.h"
#include "triac.h"
#include "triac_pair.h"

//! Тип флагов привода.
typedef uint32_t drive_flags_t;

//! Флаги привода - тип флага.
typedef enum _Drive_Flag {
    DRIVE_FLAG_NONE = 0,
    DRIVE_FLAG_POWER_CALIBRATED = 1,
    DRIVE_FLAG_
} drive_flag_t;

//! Тип состояния привода.
typedef enum _Drive_State {
    DRIVE_STATE_NONE = 0,
    DRIVE_STATE_
} drive_state_t;

//! Тип ошибки привода.
typedef enum _Drive_Error {
    DRIVE_ERROR_NONE = 0,
    DRIVE_ERROR_
} drive_error_t;

//! Перечисление состояний калибровки питания.
typedef enum _Drive_Power_Calibration {
    DRIVE_PWR_CALIBRATION_NONE = 0, //!< Нет калибровки.
    DRIVE_PWR_CALIBRATION_RUNNING, //!< Калибровка выполняется.
    DRIVE_PWR_CALIBRATION_DONE //!< Калибровка выполнена.
} drive_power_calibration_t;

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
 * Таймеры.
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

//! Параметры таймеров для открытия тиристора.
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


//! Тип задания.
typedef uint32_t reference_t;
//! Минимальное задание.
#define REFERENCE_MIN 5
//! Максимальное задание.
#define REFERENCE_MAX 95


//! Питание.
//! Число значений каналов АЦП.
#define POWER_VALUES_COUNT 11

// Алиасы значений токов и напряжений.
#define POWER_VALUE_Ia 0
#define POWER_VALUE_Ua 1
#define POWER_VALUE_Ib 2
#define POWER_VALUE_Ub 3
#define POWER_VALUE_Ic 4
#define POWER_VALUE_Uc 5
#define POWER_VALUE_Irot 6
#define POWER_VALUE_Urot 7
#define POWER_VALUE_Iexc 8
#define POWER_VALUE_Iref 9
#define POWER_VALUE_Ifan 10

//! Каналы АЦП.
#define POWER_CHANNELS (POWER_CHANNEL_0 | POWER_CHANNEL_1 | POWER_CHANNEL_2 |\
                        POWER_CHANNEL_3 | POWER_CHANNEL_4 | POWER_CHANNEL_5 |\
                        POWER_CHANNEL_6 | POWER_CHANNEL_7 | POWER_CHANNEL_8 |\
                        POWER_CHANNEL_9 | POWER_CHANNEL_10)

/**
 * Инициализирует привод.
 * @return Код ошибки.
 */
extern err_t drive_init(void);

/**
 * Получает состояние флага привода.
 * @param flag Флаг.
 * @return Состояние флага.
 */
extern bool drive_flag(drive_flag_t flag);

/**
 * Получает состояние привода.
 * @return Состояние привода.
 */
extern drive_state_t drive_state(void);

/**
 * Получает ошибку привода.
 * @return Ошибка привода.
 */
extern drive_error_t drive_error(void);

/**
 * Получает состояние калибровки питания привода.
 * @return Состояние калибровки питания привода.
 */
extern drive_power_calibration_t drive_power_calibration(void);

/**
 * Получает направление вращения.
 * @return Направление вращения.
 */
ALWAYS_INLINE static drive_dir_t drive_direction(void)
{
    return phase_state_drive_direction();
}

/**
 * Получает значение задания.
 */
extern reference_t drive_reference(void);

/**
 * Устанавливает значение задания.
 * @param reference Задание.
 * @return Код ошибки.
 */
extern err_t drive_set_reference(reference_t reference);

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
extern err_t drive_set_triacs_open_time_us(uint16_t time);

/**
 * Устанавливает порт вывода для заданной пары тиристоров.
 * @param triac_pair Номер пары тиристоров.
 * @param GPIO_a Порт тиристора А.
 * @param pin_a Пин тиристора А.
 * @param GPIO_b Порт тиристора Б.
 * @param pin_b Пин тиристора Б.
 * @return Код ошибки.
 */
extern err_t drive_set_triac_pair_gpio(triac_pair_number_t triac_pair, GPIO_TypeDef* GPIO_a, uint16_t pin_a, GPIO_TypeDef* GPIO_b, uint16_t pin_b);

/**
 * Устанавливает порт тиристора возбуждения.
 * @param GPIO Порт тиристора возбуждения.
 * @param pin Пин тиристора возбуждения.
 * @return Код ошибки.
 */
extern err_t drive_set_triac_exc_gpio(GPIO_TypeDef* GPIO, uint16_t pin);

/**
 * Устанавливает таймер для открытия тиристоров.
 * @param index Индекс таймера.
 * @param TIM Таймер.
 * @return Код ошибки.
 */
extern err_t drive_set_triac_timer(size_t index, TIM_TypeDef* TIM);

/**
 * Обработчик прерывания таймера 0 открытия тиристоров.
 */
extern void drive_triacs_timer0_irq_handler(void);

/**
 * Обработчик прерывания таймера 1 открытия тиристоров.
 */
extern void drive_triacs_timer1_irq_handler(void);

/**
 * Обрабатывает событие датчика нуля по заданной фазе.
 * @param phase Фаза.
 * @return Код ошибки.
 */
extern err_t drive_process_null_sensor(phase_t phase);

/**
 * Обрабатывает очередные значения АЦП.
 * @param channels Маска каналов АЦП.
 * @param adc_values Значения АЦП.
 * @return Код ошибки.
 */
extern err_t drive_process_power_adc_values(power_channels_t channels, uint16_t* adc_values);

/**
 * Вычисляет значения каналов АЦП.
 * @param channels Маска каналов АЦП.
 * @return Код ошибки.
 */
extern err_t drive_calc_power_values(power_channels_t channels);

/**
 * Записывает текущее значение нуля в калиброванное.
 * @param channels Маска каналов АЦП.
 * @return Код ошибки.
 */
extern err_t drive_calibrate_power(power_channels_t channels);

/**
 * Получает флаг доступности данных на всех заданных каналах АЦП.
 * @param channels Маска каналов АЦП.
 * @return Флаг доступности данных.
 */
extern bool drive_power_data_avail(power_channels_t channels);

/**
 * Получает сырое последнее значение канала АЦП.
 * @param channel Номер канала.
 * @return Сырое последнее значение канала АЦП.
 */
extern int16_t drive_power_channel_raw_value(size_t channel);

/**
 * Получает сырое среднее значение канала АЦП.
 * @param channel Номер канала.
 * @return Сырое среднее значение канала АЦП.
 */
extern int16_t drive_power_channel_raw_value_avg(size_t channel);

/**
 * Получает сырое RMS значение канала АЦП.
 * @param channel Номер канала.
 * @return Сырое RMS значение канала АЦП.
 */
extern int16_t drive_power_channel_raw_value_rms(size_t channel);

/**
 * Получает реальное последнее значение канала АЦП.
 * @param channel Номер канала.
 * @return Реальное последнее значение канала АЦП.
 */
extern fixed32_t drive_power_channel_real_value(size_t channel);

/**
 * Получает реальное среднее значение канала АЦП.
 * @param channel Номер канала.
 * @return Реальное среднее значение канала АЦП.
 */
extern fixed32_t drive_power_channel_real_value_avg(size_t channel);

/**
 * Получает реальное RMS значение канала АЦП.
 * @param channel Номер канала.
 * @return Реальное RMS значение канала АЦП.
 */
extern fixed32_t drive_power_channel_real_value_rms(size_t channel);

#endif	/* DRIVE_H */
