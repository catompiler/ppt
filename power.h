/**
 * @file power.h Библиотека вычисления значений напряжений и токов с АЦП.
 */

#ifndef POWER_H
#define	POWER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "mid_filter/mid_filter3i.h"
#include "fixed/fixed32.h"
#include "errors/errors.h"
#include "defs/defs.h"


//! Частота измерений АЦП.
#define POWER_ADC_FREQ 3200

//! Частота сети.
#define POWER_FREQ 50

//! Число измерений ADC за период.
#define POWER_ADC_MEASUREMENTS_PER_PERIOD (POWER_ADC_FREQ / POWER_FREQ)

//! Время между измерениями ADC.
#define POWER_ADC_DT_F (fixed32_make_from_fract(1, POWER_ADC_FREQ))

//! Число итераций накопления и обработки данных за период.
#define POWER_PERIOD_ITERS 3

//! Максимальное число итераций накопления и обработки данных за период.
#define POWER_PERIOD_ITERS_MAX 3


#define POWER_IGNORE_BITS 0
#if POWER_IGNORE_BITS != 0
#define POWER_IGNORE_BITS_MASK (~((1<<(POWER_IGNORE_BITS)) - 1))
#else
#define POWER_IGNORE_BITS_MASK (~0)
#endif


//! Число элементов в буфере фильтра.
#define POWER_FILTER_SIZE_MAX (POWER_PERIOD_ITERS_MAX)

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
 * Тип значения буфера фильтра.
 */
typedef struct _Power_Filter_Value {
    int32_t sum; //!< Сумма значений.
    uint16_t count; //!< Число значений.
} power_filter_value_t;
/**
 * Структура фильтра значений.
 */
typedef struct _Power_Filter {
    power_filter_value_t buffer[POWER_FILTER_SIZE_MAX]; //!< Буфер значений.
    uint16_t index; //!< Текущий индекс.
    uint16_t count; //!< Число элементов в буфере.
    uint16_t size; //!< Максимальное число элементов в буфере.
} power_filter_t;

/**
 * Структура значения, полученного с канала АЦП.
 */
typedef struct _Power_Value {
    power_channel_type_t type; //!< Тип канала.
    fixed32_t adc_mult; //!< Коэффициент пропорциональности АЦП.
    fixed32_t value_mult; //!< Коэффициент пропорциональности значений.
    int16_t raw_zero_cal; //!< Калиброванное значение нуля.
    int16_t raw_zero_cur; //!< Текущее значение нуля.
    uint16_t raw_adc_value_inst; //!< Сырое значение АЦП без учёта средней точки (мгновенное).
    int16_t raw_value_inst; //!< Сырое значение с АЦП (мгновенное).
    int16_t raw_value; //!< Сырое значение с АЦП.
    fixed32_t real_value_inst; //!< Значение в СИ (мгновенное).
    fixed32_t real_value; //!< Значение в СИ.
    int32_t sum; //!< Сумма значений.
    int32_t sum_zero; //!< Сумма значений для вычисления нуля.
    uint16_t count; //!< Число значений.
    uint16_t count_zero; //!< Число значений нуля.
    mid_filter3i_t filter_mid_adc; //!< Медианный фильтр значений АЦП.
    power_filter_t filter_value; //!< Фильтр значенией.
    power_filter_t filter_zero; //!< Фильтр нуля.
    bool is_soft; //!< Флаг программного вычисления канала.
    bool calibrated; //!< Флаг калибровки.
    bool data_avail; //!< Флаг доступности данных.
    bool adc_filter_enabled; //!< Разрешение фильтрации данных АЦП.
} power_value_t;

//! Инициализирует структуру значения канала АЦП по месту объявления.
#define MAKE_POWER_CHANNEL(arg_type, arg_k) { .type = arg_type, .k = arg_k,\
                                    .raw_zero_cal = 0, .raw_zero_cur = 0,\
                                    .raw_value_inst = 0, .raw_value = 0,\
                                    .real_value_inst = 0, .real_value = 0,\
                                    .sum = 0, .sum_zero = 0,\
                                    .count = 0, .count_zero = 0,\
                                    .filter_value = {0}, .filter_zero = {0},\
                                    .is_soft = false, .calibrated = false,\
                                    .data_avail = false, adc_filter_enabled = true }

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
 * @param filter_size Размер фильтра накомления значений.
 * @param k Коэффициент пропорциональности LSB^-1.
 * @return Код ошибки.
 */
extern err_t power_value_init(power_value_t* value, power_channel_type_t type, size_t filter_size, fixed32_t k);

/**
 * Инициализирует структуру питания.
 * @param power Питание.
 * @param channels Значения каналов АЦП.
 * @param channels_count Число значений каналов АЦП.
 * @return Код ошибки.
 */
extern err_t power_init(power_t* power, power_value_t* channels, size_t channels_count);

/**
 * Устанавливает программное вычисление для заданного канала.
 * @param power Питание.
 * @param channel Номер канала.
 * @param is_soft Флаг программного вычисления.
 * @return Код ошибки.
 */
extern err_t power_set_soft_channel(power_t* power, size_t channel, bool is_soft);

/**
 * Устанавливает значение программно вычисляемого канала.
 * @param power Питание.
 * @param channel Номер канала.
 * @param value Значенеи канала.
 * @return Код ошибки.
 */
extern err_t power_process_soft_channel_value(power_t* power, size_t channel, fixed32_t value);

/**
 * Обрабатывает очередные значения АЦП.
 * @param power Питание.
 * @param channels Маска каналов АЦП.
 * @param adc_values Значения АЦП.
 * @return Код ошибки.
 */
extern err_t power_process_adc_values(power_t* power, power_channels_t channels, uint16_t* adc_values);

/**
 * Обрабатывает накопленные данные АЦП.
 * @param power Питание.
 * @param channels Маска каналов АЦП.
 * @return Код ошибки.
 */
extern err_t power_process_accumulated_data(power_t* power, power_channels_t channels);

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
 * Получает множитель значения канала АЦП.
 * @param power Питание.
 * @param channel Номер канала АЦП.
 * @return Множитель значения канала АЦП.
 */
ALWAYS_INLINE static fixed32_t power_adc_value_multiplier(power_t* power, size_t channel)
{
    return power->channels[channel].adc_mult;
}

/**
 * Устанавливает множитель значения канала АЦП.
 * @param power Питание.
 * @param channel Номер канала АЦП.
 * @param data Множитель значения канала АЦП.
 */
ALWAYS_INLINE static void power_set_adc_value_multiplier(power_t* power, size_t channel, fixed32_t mult)
{
    power->channels[channel].adc_mult = mult;
}

/**
 * Получает калибровочные данные канала АЦП.
 * @param power Питание.
 * @param channel Номер канала АЦП.
 * @return Калибровочные данные канала АЦП.
 */
ALWAYS_INLINE static uint16_t power_adc_calibration_data(power_t* power, size_t channel)
{
    return (uint16_t)power->channels[channel].raw_zero_cal;
}

/**
 * Получает калибровочные данные канала АЦП.
 * @param power Питание.
 * @param channel Номер канала АЦП.
 * @param data Калибровочные данные канала АЦП.
 */
ALWAYS_INLINE static void power_set_adc_calibration_data(power_t* power, size_t channel, uint16_t data)
{
    power->channels[channel].raw_zero_cal = (int16_t)data;
}

/**
 * Получает множитель значения канала.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Множитель значения канала.
 */
ALWAYS_INLINE static fixed32_t power_value_multiplier(power_t* power, size_t channel)
{
    return power->channels[channel].value_mult;
}

/**
 * Устанавливает множитель значения канала.
 * @param power Питание.
 * @param channel Номер канала.
 * @param data Множитель значения канала.
 */
ALWAYS_INLINE static void power_set_value_multiplier(power_t* power, size_t channel, fixed32_t mult)
{
    power->channels[channel].value_mult = mult;
}

/**
 * Получает флаг доступности данных на всех заданных каналах АЦП.
 * @param power Питание.
 * @param channels Маска каналов АЦП.
 * @return Флаг доступности данных.
 */
extern bool power_data_avail(const power_t* power, power_channels_t channels);

/**
 * Получает флаг заполнения фильтра данных на всех заданных каналах АЦП.
 * @param power Питание.
 * @param channels Маска каналов АЦП.
 * @return Флаг заполнения фильтра данных.
 */
extern bool power_data_filter_filled(const power_t* power, power_channels_t channels);

/**
 * Сбрасывает накопленные данные каналов.
 * @param power Питание.
 * @param channels Маска каналов АЦП.
 * @return Код ошибки.
 */
extern err_t power_reset_channels(power_t* power, power_channels_t channels);

/**
 * Получает тип канала питания.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Тип канала.
 */
ALWAYS_INLINE static power_channel_type_t power_channel_type(const power_t* power, size_t channel)
{
    return power->channels[channel].type;
}

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
 * Получает флаг разрешения фильтрации значений АЦП.
 * @param power Питание.
 * @param channel Номер канала питания.
 * @return Флаг разрешения фильтрации значений АЦП.
 */
ALWAYS_INLINE static bool power_channel_adc_filter_enabled(const power_t* power, size_t channel)
{
    return power->channels[channel].adc_filter_enabled;
}

/**
 * Получает флаг разрешения фильтрации значений АЦП.
 * @param power Питание.
 * @param channel Номер канала питания.
 * @param enabled Разрешение.
 * @return Флаг разрешения фильтрации значений АЦП.
 */
ALWAYS_INLINE static void power_channel_set_adc_filter_enabled(power_t* power, size_t channel, bool enabled)
{
    power->channels[channel].adc_filter_enabled = enabled;
}

/**
 * Получает флаг заполнения фильтра данных.
 * @param power Питание.
 * @return Флаг заполнения фильтра данных данных.
 */
ALWAYS_INLINE static bool power_channel_data_filter_filled(const power_t* power, size_t channel)
{
    return power->channels[channel].filter_value.count == power->channels[channel].filter_value.size;
}

/**
 * Получает сырое последнее мгновенное значение канала АЦП.
 * Без учёта средней точки.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Сырое последнее мгновенное значение канала АЦП.
 */
ALWAYS_INLINE static uint16_t power_channel_raw_adc_value_inst(const power_t* power, size_t channel)
{
    return power->channels[channel].raw_adc_value_inst;
}

/**
 * Получает сырое последнее мгновенное значение канала АЦП.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Сырое последнее мгновенное значение канала АЦП.
 */
ALWAYS_INLINE static int16_t power_channel_raw_value_inst(const power_t* power, size_t channel)
{
    return power->channels[channel].raw_value_inst;
}

/**
 * Получает сырое значение канала АЦП.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Сырое значение канала АЦП.
 */
ALWAYS_INLINE static int16_t power_channel_raw_value(const power_t* power, size_t channel)
{
    return power->channels[channel].raw_value;
}

/**
 * Получает реальное последнее мгновенное значение канала АЦП.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Реальное последнее мгновенное значение канала АЦП.
 */
ALWAYS_INLINE static fixed32_t power_channel_real_value_inst(const power_t* power, size_t channel)
{
    return power->channels[channel].real_value_inst;
}

/**
 * Получает реальное значение канала АЦП.
 * @param power Питание.
 * @param channel Номер канала.
 * @return Реальное значение канала АЦП.
 */
ALWAYS_INLINE static fixed32_t power_channel_real_value(const power_t* power, size_t channel)
{
    return power->channels[channel].real_value;
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

