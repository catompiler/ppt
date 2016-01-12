/**
 * @file power.h Библиотека вычисления значений напряжений и токов с АЦП.
 */

#ifndef POWER_H
#define	POWER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "fixed/fixed32.h"
#include "errors/errors.h"
#include "defs/defs.h"



/**
 * Перечисление каналов АЦП.
 */
typedef enum _Power_Channel {
    POWER_CHANNEL_NONE = 0,
    POWER_CHANNEL_0 = 1,
    POWER_CHANNEL_1 = 2,
    POWER_CHANNEL_2 = 4,
    POWER_CHANNEL_3 = 8,
    POWER_CHANNEL_4 = 16,
    POWER_CHANNEL_5 = 32,
    POWER_CHANNEL_6 = 64,
    POWER_CHANNEL_7 = 128,
    POWER_CHANNEL_8 = 256,
    POWER_CHANNEL_9 = 512,
    POWER_CHANNEL_10 = 1024,
    POWER_CHANNEL_11 = 2048,
    POWER_CHANNEL_12 = 4096,
    POWER_CHANNEL_13 = 8192,
    POWER_CHANNEL_14 = 16384,
    POWER_CHANNEL_15 = 32768,
    POWER_CHANNEL_ALL = 65535
} power_channel_t;

//! Тип нескольких каналов АЦП.
typedef uint16_t power_channels_t;

//! Тип канала.
typedef enum _Power_Channel_Type {
    POWER_CHANNEL_DC = 0, //!< DC.
    POWER_CHANNEL_AC = 1, //!< AC.
} power_channel_type_t;

/**
 * Структура значения, полученного с канала АЦП.
 */
typedef struct _Power_Value {
    power_channel_type_t type; //!< Тип канала.
    fixed32_t k; //!< Коэффициент пропорциональности.
    int16_t raw_zero_cal; //!< Калиброванное значение нуля.
    int16_t raw_zero_cur; //!< Текущее значение нуля.
    int32_t sum_zero; //!< Сумма значений для вычисления нуля.
    int16_t raw_value; //!< Сырое значение с АЦП.
    int16_t raw_value_avg; //!< Сырое значение с АЦП (среднее).
    int16_t raw_value_rms; //!< Сырое значение с АЦП (RMS).
    fixed32_t real_value; //!< Значение в СИ.
    fixed32_t real_value_avg; //!< Значение в СИ (среднее).
    fixed32_t real_value_rms; //!< Значение в СИ (RMS).
    int32_t sum_avg; //!< Сумма значений (среднее).
    int32_t sum_rms; //!< Сумма значений (RMS).
    int32_t count; //!< Число значений.
    bool calibrated; //!< Флаг калибровки.
    bool data_avail; //!< Флаг доступности данных.
} power_value_t;

//! Инициализирует структуру значения канала АЦП по месту объявления.
#define MAKE_POWER_CHANNEL(arg_type, arg_k) { .type = arg_type, .k = arg_k,\
                                    .raw_zero_cal = 0, .raw_zero_cur = 0,\
                                    .raw_value = 0, .sum_zero = 0,\
                                    .raw_value_avg = 0, .raw_value_rms = 0,\
                                    .real_value = 0,\
                                    .real_value_avg = 0, .real_value_rms = 0,\
                                    .sum_avg = 0, .sum_rms = 0, .count = 0,\
                                    .calibrated = false, .data_avail = false }

/**
 * Структура питания.
 */
typedef struct _Power{
    power_value_t* channels; //!< Каналы АЦП.
    size_t channels_count; //!< Число каналов АЦП.
} power_t;

//! Инициализирует структуру питания по месту объявления.
#define MAKE_POWER(arg_channels, arg_count) { .channels = arg_channels, .channels_count = arg_count }


/**
 * Инициализирует канал АЦП.
 * @param channel Канал АЦП.
 * @param type Тип канала АЦП.
 * @param k Коэффициент пропорциональности LSB^-1.
 * @return Код ошибки.
 */
extern err_t power_value_init(power_value_t* value, power_channel_type_t type, fixed32_t k);

/**
 * Инициализирует структуру питания.
 * @param power Питание.
 * @param channels Значения каналов АЦП.
 * @param channels_count Число значений каналов АЦП.
 * @return Код ошибки.
 */
extern err_t power_init(power_t* power, power_value_t* channels, size_t channels_count);

/**
 * Обрабатывает очередные значения АЦП.
 * @param power Питание.
 * @param channels Маска каналов АЦП.
 * @param adc_values Значения АЦП.
 * @return Код ошибки.
 */
extern err_t power_process_adc_values(power_t* power, power_channels_t channels, uint16_t* adc_values);

/**
 * Вычисляет значения каналов АЦП.
 * @param power Питание.
 * @param channels Маска каналов АЦП.
 * @return Код ошибки.
 */
extern err_t power_calc_values(power_t* power, power_channels_t channels);

/**
 * Записывает текущее значение нуля в калиброванное.
 * @param power Питание.
 * @param channels Маска каналов АЦП.
 * @return Код ошибки.
 */
extern err_t power_calibrate(power_t* power, power_channels_t channels);

/**
 * Получает флаг доступности данных на всех заданных каналах АЦП.
 * @param power Питание.
 * @param channels Маска каналов АЦП.
 * @return Флаг доступности данных.
 */
extern bool power_data_avail(const power_t* power, power_channels_t channels);

/**
 * Получает флаг калибровки.
 * @param power Питание.
 * @return Флаг калибровки.
 */
ALWAYS_INLINE static bool power_channel_calibrated(const power_t* power, size_t channel)
{
    return power->channels[channel].calibrated;
}

/**
 * Получает флаг доступности данных.
 * @param power Питание.
 * @return Флаг доступности данных.
 */
ALWAYS_INLINE static bool power_channel_data_avail(const power_t* power, size_t channel)
{
    return power->channels[channel].data_avail;
}

/**
 * Получает сырое последнее значение канала АЦП.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Сырое последнее значение канала АЦП.
 */
ALWAYS_INLINE static int16_t power_channel_raw_value(const power_t* power, size_t channel)
{
    return power->channels[channel].raw_value;
}

/**
 * Получает сырое среднее значение канала АЦП.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Сырое среднее значение канала АЦП.
 */
ALWAYS_INLINE static int16_t power_channel_raw_value_avg(const power_t* power, size_t channel)
{
    return power->channels[channel].raw_value_avg;
}

/**
 * Получает сырое RMS значение канала АЦП.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Сырое RMS значение канала АЦП.
 */
ALWAYS_INLINE static int16_t power_channel_raw_value_rms(const power_t* power, size_t channel)
{
    return power->channels[channel].raw_value_rms;
}

/**
 * Получает реальное последнее значение канала АЦП.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Реальное последнее значение канала АЦП.
 */
ALWAYS_INLINE static fixed32_t power_channel_real_value(const power_t* power, size_t channel)
{
    return power->channels[channel].real_value;
}

/**
 * Получает реальное среднее значение канала АЦП.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Реальное среднее значение канала АЦП.
 */
ALWAYS_INLINE static fixed32_t power_channel_real_value_avg(const power_t* power, size_t channel)
{
    return power->channels[channel].real_value_avg;
}

/**
 * Получает реальное RMS значение канала АЦП.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Реальное RMS значение канала АЦП.
 */
ALWAYS_INLINE static fixed32_t power_channel_real_value_rms(const power_t* power, size_t channel)
{
    return power->channels[channel].real_value_rms;
}

/**
 * Получает сырое среднее калиброванное значение нуля канала АЦП.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Сырое среднее калиброванное значение нуля канала АЦП.
 */
ALWAYS_INLINE static int16_t power_channel_raw_zero_calibrated(const power_t* power, size_t channel)
{
    return power->channels[channel].raw_zero_cal;
}

/**
 * Получает сырое среднее текущее значение нуля канала АЦП.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Сырое среднее текущее значение нуля канала АЦП.
 */
ALWAYS_INLINE static int16_t power_channel_raw_zero_current(const power_t* power, size_t channel)
{
    return power->channels[channel].raw_zero_cur;
}

/**
 * Получает текущее число выборок канала АЦП.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Текущее число выборок канала АЦП.
 */
ALWAYS_INLINE static size_t power_channel_samples_count(const power_t* power, size_t channel)
{
    return (size_t)power->channels[channel].count;
}

#endif	/* POWER_H */

