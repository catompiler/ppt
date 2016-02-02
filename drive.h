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

//! Флаги привода - тип флага.
typedef enum _Drive_Flag {
    DRIVE_FLAG_NONE             = 0x0,
    DRIVE_FLAG_POWER_CALIBRATED = 0x1,
    DRIVE_FLAG_POWER_DATA_AVAIL = 0x2,
    DRIVE_FLAG_POWER_GOOD_Ua    = 0x4,
    DRIVE_FLAG_POWER_GOOD_Ub    = 0x8,
    DRIVE_FLAG_POWER_GOOD_Uc    = 0x10,
    DRIVE_FLAG_POWER_GOOD_Urot  = 0x20,
    DRIVE_FLAG_POWER_GOOD_Ia    = 0x40,
    DRIVE_FLAG_POWER_GOOD_Ib    = 0x80,
    DRIVE_FLAG_POWER_GOOD_Ic    = 0x100,
    DRIVE_FLAG_POWER_GOOD_Irot  = 0x200,
    DRIVE_FLAG_POWER_GOOD_Iexc  = 0x400,
    DRIVE_FLAG_POWER_GOOD_Ifan  = 0x800,
    DRIVE_FLAG_POWER_GOOD_Iref  = 0x1000
} drive_flag_t;

//! Тип флагов привода.
typedef uint32_t drive_flags_t;

//! Тип статуса привода.
typedef enum _Drive_Status {
    DRIVE_STATUS_INIT       = 0,
    DRIVE_STATUS_IDLE       = 1,
    DRIVE_STATUS_RUN        = 2,
    DRIVE_STATUS_ERROR      = 3
} drive_status_t;

//! Тип ошибки привода.
typedef enum _Drive_Error {
    DRIVE_ERROR_NONE                 = 0x0, //!< Нет ошибки.
    DRIVE_ERROR_POWER_DATA_NOT_AVAIL = 0x1, //!< Данные питания не поступают с АЦП.
    DRIVE_ERROR_POWER_INVALID        = 0x2, //!< Неправильные значения питания, см. drive_power_error_t.
    DRIVE_ERROR_EMERGENCY_STOP       = 0x4 //!< Аварийный останов (Грибок).
} drive_error_t;

//! Тип ошибок привода.
typedef uint32_t drive_errors_t;

//! Тип ошибки питания привода.
typedef enum _Drive_Power_Error {
    DRIVE_POWER_ERROR_NONE           = 0x0,
    DRIVE_POWER_ERROR_UNDERFLOW_Ua   = 0x1,
    DRIVE_POWER_ERROR_OVERFLOW_Ua    = 0x2,
    DRIVE_POWER_ERROR_UNDERFLOW_Ub   = 0x4,
    DRIVE_POWER_ERROR_OVERFLOW_Ub    = 0x8,
    DRIVE_POWER_ERROR_UNDERFLOW_Uc   = 0x10,
    DRIVE_POWER_ERROR_OVERFLOW_Uc    = 0x20,
    DRIVE_POWER_ERROR_UNDERFLOW_Urot = 0x40,
    DRIVE_POWER_ERROR_OVERFLOW_Urot  = 0x80,
    DRIVE_POWER_ERROR_IDLE_Urot      = 0x100,
    DRIVE_POWER_ERROR_UNDERFLOW_Ia   = 0x200,
    DRIVE_POWER_ERROR_OVERFLOW_Ia    = 0x400,
    DRIVE_POWER_ERROR_IDLE_Ia        = 0x800,
    DRIVE_POWER_ERROR_UNDERFLOW_Ib   = 0x1000,
    DRIVE_POWER_ERROR_OVERFLOW_Ib    = 0x2000,
    DRIVE_POWER_ERROR_IDLE_Ib        = 0x4000,
    DRIVE_POWER_ERROR_UNDERFLOW_Ic   = 0x8000,
    DRIVE_POWER_ERROR_OVERFLOW_Ic    = 0x10000,
    DRIVE_POWER_ERROR_IDLE_Ic        = 0x20000,
    DRIVE_POWER_ERROR_UNDERFLOW_Irot = 0x40000,
    DRIVE_POWER_ERROR_OVERFLOW_Irot  = 0x80000,
    DRIVE_POWER_ERROR_IDLE_Irot      = 0x100000,
    DRIVE_POWER_ERROR_UNDERFLOW_Iexc = 0x200000,
    DRIVE_POWER_ERROR_OVERFLOW_Iexc  = 0x400000,
    DRIVE_POWER_ERROR_IDLE_Iexc      = 0x800000,
    DRIVE_POWER_ERROR_UNDERFLOW_Ifan = 0x1000000,
    DRIVE_POWER_ERROR_OVERFLOW_Ifan  = 0x2000000,
    DRIVE_POWER_ERROR_UNDERFLOW_Iref = 0x4000000,
    DRIVE_POWER_ERROR_OVERFLOW_Iref  = 0x8000000
} drive_power_error_t;

//! Тип ошибок питания привода.
typedef uint32_t drive_power_errors_t;

typedef enum _Drive_Warning {
    DRIVE_WARNING_NONE               = 0x0,
    DRIVE_WARNING_POWER_OVERFLOW_Ua  = 0x1,
    DRIVE_WARNING_POWER_UNDERFLOW_Ua = 0x2,
    DRIVE_WARNING_POWER_OVERFLOW_Ub  = 0x4,
    DRIVE_WARNING_POWER_UNDERFLOW_Ub = 0x8,
    DRIVE_WARNING_POWER_OVERFLOW_Uc  = 0x10,
    DRIVE_WARNING_POWER_UNDERFLOW_Uc = 0x20
} drive_warning_t;

typedef uint32_t drive_warnings_t;

//! Перечисление состояний калибровки питания.
typedef enum _Drive_Power_Calibration {
    DRIVE_PWR_CALIBRATION_NONE = 0, //!< Нет калибровки.
    DRIVE_PWR_CALIBRATION_RUNNING, //!< Калибровка выполняется.
    DRIVE_PWR_CALIBRATION_DONE //!< Калибровка выполнена.
} drive_power_calibration_t;

//! Перечисление состояний запуска привода.
typedef enum _Drive_Starting {
    DRIVE_STARTING_NONE = 0, //!< Не запускается.
    DRIVE_STARTING_START = 1, //!< Нужно запустить.
    DRIVE_STARTING_WAIT_EXC = 2, //!< Ожидание возбуждения.
    DRIVE_STARTING_RAMP = 3, //!< Разгон.
    DRIVE_STARTING_DONE = 4 //!< Запущен.
} drive_starting_t;

//! Перечисление состояний останова привода.
typedef enum _Drive_Stopping {
    DRIVE_STOPPING_NONE = 0, //!< Не останавливается.
    DRIVE_STOPPING_STOP = 1, //!< Нужно остановить.
    DRIVE_STOPPING_RAMP = 2, //!< Торможение.
    DRIVE_STOPPING_WAIT_ROT = 3, //!< Ожидане остановки якоря.
    DRIVE_STOPPING_WAIT_EXC = 4, //!< Ожидане возвращения к нулю возбуждения.
    DRIVE_STOPPING_DONE = 5 //!< Остановлен.
} drive_stopping_t;

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
#define POWER_VALUE_Ua 0
#define POWER_VALUE_Ia 1
#define POWER_VALUE_Ub 2
#define POWER_VALUE_Ib 3
#define POWER_VALUE_Uc 4
#define POWER_VALUE_Ic 5
#define POWER_VALUE_Urot 6
#define POWER_VALUE_Irot 7
#define POWER_VALUE_Iexc 8
#define POWER_VALUE_Iref 9
#define POWER_VALUE_Ifan 10

//! Каналы АЦП.
#define POWER_CHANNELS (POWER_CHANNEL_0 | POWER_CHANNEL_1 | POWER_CHANNEL_2 |\
                        POWER_CHANNEL_3 | POWER_CHANNEL_4 | POWER_CHANNEL_5 |\
                        POWER_CHANNEL_6 | POWER_CHANNEL_7 | POWER_CHANNEL_8 |\
                        POWER_CHANNEL_9 | POWER_CHANNEL_10)

//! Частота сети.
#define POWER_FREQ 50

/**
 * Инициализирует привод.
 * @return Код ошибки.
 */
extern err_t drive_init(void);

/**
 * Обновляет настройки привода.
 * @return Код ошибки.
 */
extern err_t drive_update_settings(void);

/**
 * Получает состояние флага привода.
 * @param flag Флаг.
 * @return Состояние флага.
 */
extern bool drive_flag(drive_flag_t flag);

/**
 * Получает состояние флагов привода.
 * @return Состояние флагов привода.
 */
extern drive_flags_t drive_flags(void);

/**
 * Получает статус привода.
 * @return Статус привода.
 */
extern drive_status_t drive_status(void);

/**
 * Получает наличие ошибки привода.
 * @param error Ошибка привода.
 * @return Наличие ошибки привода.
 */
extern bool drive_error(drive_error_t error);

/**
 * Получает ошибки привода.
 * @return Ошибки привода.
 */
extern drive_errors_t drive_errors(void);

/**
 * Получает наличие предупреждения привода.
 * @param warning Предупреждение привода.
 * @return Наличие предупреждения привода.
 */
extern bool drive_warning(drive_warning_t warning);

/**
 * Получает предупреждения привода.
 * @return Предупреждения привода.
 */
extern drive_warnings_t drive_warnings(void);

/**
 * Получает наличие ошибки питания привода.
 * @param error Ошибка питания привода.
 * @return Наличие ошибки питания привода.
 */
extern bool drive_power_error(drive_power_error_t error);

/**
 * Получает ошибки питания привода.
 * @return Ошибки питания привода.
 */
extern drive_power_errors_t drive_power_errors(void);

/**
 * Получает состояние калибровки питания привода.
 * @return Состояние калибровки питания привода.
 */
extern drive_power_calibration_t drive_power_calibration(void);

/**
 * Получает состояние запуска привода.
 * @return Состояние запуска привода.
 */
extern drive_starting_t drive_starting(void);

/**
 * Получает состояние останова привода.
 * @return Состояние останова привода.
 */
extern drive_stopping_t drive_stopping(void);

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
 * Получает флаг готовности привода.
 * @return Флаг готовности привода.
 */
extern bool drive_ready(void);

/**
 * Запускает привод.
 * @return Флаг запуска привода.
 */
extern bool drive_start(void);

/**
 * Останавливает привод.
 * @return Флаг останова привода.
 */
extern bool drive_stop(void);

/**
 * Получает флаг работы привода.
 * @return Флаг работы привода.
 */
extern bool drive_running(void);

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
 * Получает время открытия симистора возбуждения.
 * @return Время открытия симистора возбуждения в мкс.
 */
extern uint16_t drive_triac_exc_open_time_us(void);

/**
 * Устанавливает время открытия симистора возбуждения.
 * @param time Время открытия симистора возбуждения в мкс.
 * @return Код ошибки.
 */
extern err_t drive_set_triac_exc_open_time_us(uint16_t time);

/**
 * Устанавливает фазу возбуждения.
 * @param phase Фаза возбуждения.
 * @return Код ошибки.
 */
extern err_t drive_set_exc_phase(phase_t phase);

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
extern err_t drive_set_triacs_pairs_timer(size_t index, TIM_TypeDef* TIM);

/**
 * Устанавливает таймер для открытия симистора возбуждения.
 * @param TIM Таймер.
 * @return Код ошибки.
 */
extern err_t drive_set_triac_exc_timer(TIM_TypeDef* TIM);

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
extern void drive_triac_exc_timer_irq_handler(void);

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
