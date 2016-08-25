/**
 * @file drive_power.h Библиотека питания привода.
 */

#ifndef DRIVE_POWER_H
#define DRIVE_POWER_H

#include <stm32f10x.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "power.h"
#include "errors/errors.h"
#include "drive_phase_state.h"

//! Питание.
//! Число значений каналов АЦП.
#define DRIVE_POWER_CHANNELS_COUNT 11

// Алиасы значений токов и напряжений.
#define DRIVE_POWER_Ua 0
#define DRIVE_POWER_Ia 1
#define DRIVE_POWER_Ub 2
#define DRIVE_POWER_Ib 3
#define DRIVE_POWER_Uc 4
#define DRIVE_POWER_Ic 5
#define DRIVE_POWER_Urot 6
#define DRIVE_POWER_Irot 7
#define DRIVE_POWER_Iexc 8
#define DRIVE_POWER_Iref 9
#define DRIVE_POWER_Ifan 10

//! Каналы АЦП.
#define DRIVE_POWER_CHANNELS (POWER_CHANNEL_0 | POWER_CHANNEL_1 | POWER_CHANNEL_2 |\
                              POWER_CHANNEL_3 | POWER_CHANNEL_4 | POWER_CHANNEL_5 |\
                              POWER_CHANNEL_6 | POWER_CHANNEL_7 | POWER_CHANNEL_8 |\
                              POWER_CHANNEL_9 | POWER_CHANNEL_10)

//! Частота сети.
#define DRIVE_POWER_FREQ 50

//! Частота срабатывания датчиков нуля.
#define DRIVE_POWER_NULL_SENSORS_FREQ (DRIVE_POWER_FREQ * 3)

//! Число периодов накопления и обработки данных по-умолчанию.
#define DRIVE_POWER_PROCESSING_PERIODS_DEFAULT 1


//! Тип значения в осциллограмме (fixed11_5_t).
typedef int16_t osc_value_t;

//! Число бит дробной части значения осциллограммы.
#define OSC_VALUE_FRACT_BITS 5

//! Число капналов осциллограмм.
#define DRIVE_POWER_OSC_CHANNELS_COUNT 9

//! Осциллограмма Ua.
#define DRIVE_POWER_OSC_CHANNEL_Ua 0
//! Осциллограмма Ub.
#define DRIVE_POWER_OSC_CHANNEL_Ub 1
//! Осциллограмма Uc.
#define DRIVE_POWER_OSC_CHANNEL_Uc 2
//! Осциллограмма Ia.
#define DRIVE_POWER_OSC_CHANNEL_Ia 3
//! Осциллограмма Ib.
#define DRIVE_POWER_OSC_CHANNEL_Ib 4
//! Осциллограмма Ic.
#define DRIVE_POWER_OSC_CHANNEL_Ic 5
//! Осциллограмма Urot.
#define DRIVE_POWER_OSC_CHANNEL_Urot 6
//! Осциллограмма Irot.
#define DRIVE_POWER_OSC_CHANNEL_Irot 7
//! Осциллограмма Iexc.
#define DRIVE_POWER_OSC_CHANNEL_Iexc 8

//! Длина канала осциллограммы (1.75 периода до и после аварии).
#define DRIVE_POWER_OSC_CHANNEL_LEN (224) // (128 / 2) * (1.75 * 2) * 9 * 2 == 4032

//! Размер в байтах канала осциллограммы.
#define DRIVE_POWER_OSC_CHANNEL_SIZE (DRIVE_POWER_OSC_CHANNEL_LEN * sizeof(osc_value_t))

//! Время канала осциллограммы (1.75 * 2 * 20)
#define DRIVE_POWER_OSC_CHANNEL_TIME_MS (70)

//! Структура канала осциллограммы.
#pragma pack(push, 1)
typedef struct _Drive_Power_Osc_Channel {
    osc_value_t data[DRIVE_POWER_OSC_CHANNEL_LEN]; //!< Данные осцилограммы.
} drive_power_osc_channel_t;
#pragma pack(pop)


/**
 * Инициализирует питание привода.
 * @return Код ошибки.
 */
extern err_t drive_power_init(void);

/**
 * Сбрасывает состояние питания привода.
 */
extern void drive_power_reset(void);

/**
 * Получает фазу начала измерений питания.
 * @return Фаза питания.
 */
extern phase_t drive_power_phase(void);

/**
 * Устанавливает фазу начала измерений питания.
 * @param phase Фаза питания.
 * @return Код ошибки.
 */
extern err_t drive_power_set_phase(phase_t phase);

/**
 * Получает количество периодов накопления данных.
 * @return Количество периодов накопления данных.
 */
extern size_t drive_power_processing_periods(void);

/**
 * Устанавливает количество периодов накопления данных.
 * @param periods Количество периодов накопления данных.
 * @return Код ошибки.
 */
extern err_t drive_power_set_processing_periods(size_t periods);

/**
 * Получает количество осциллограмм.
 * @return Количество осциллограмм.
 */
extern size_t drive_power_osc_channels_count(void);

/**
 * Получает длину осциллограмм.
 * @return Длина осциллограмм.
 */
extern size_t drive_power_oscillogram_length(void);

/**
 * Получает заполненность осциллограмм данными.
 * @return Заполненность осциллограмм данными.
 */
extern bool drive_power_oscillogram_full(void);

/**
 * Прекращает запись осциллограмм при заполнении
 * следующей половины буфера.
 */
extern void drive_power_oscillogram_half_pause(void);

/**
 * Получает флаг останова записи осциллограмм.
 * @return Флаг останова записи осциллограмм.
 */
extern bool drive_power_oscillogram_paused(void);

/**
 * Возобнавляет запись осциллограмм.
 */
extern void drive_power_oscillogram_resume(void);

/*
 * Получает индекс начала буфера осциллограмм.
 * @return Индекс начала буфера осциллограмм.
 */
extern size_t drive_power_oscillogram_start_index(void);

/*
 * Получает индекс половины буфера осциллограмм.
 * @return Индекс половины буфера осциллограмм.
 */
extern size_t drive_power_oscillogram_half_index(void);

/*
 * Получает индекс конца буфера осциллограмм.
 * @return Индекс конца буфера осциллограмм.
 */
extern size_t drive_power_oscillogram_end_index(void);

/**
 * Получает следующий после заданного индекс.
 * @param index Индекс.
 * @return Следующий индекс после заданного.
 */
extern size_t drive_power_oscillogram_next_index(size_t index);

/**
 * Получает данные осциллограммы.
 * @param osc_channel Канал осциллограммы.
 * @param index Индекс данных.
 * @return Данные осциллограммы.
 */
extern osc_value_t drive_power_osc_channel_data(size_t osc_channel, size_t index);

/**
 * Получает осциллограмму с заданным индексом.
 * @param osc_channel Канал осциллограммы.
 * @param osc Осциллограмма.
 * @return Код ошибки.
 */
extern err_t drive_power_osc_channel_get(size_t osc_channel, drive_power_osc_channel_t* osc);

/**
 * Преобразует значение fixed32_t в значение осциллограммы.
 * @param value Значение fixed32_t.
 * @return Значение осциллограммы в fixed32_t.
 */
ALWAYS_INLINE static osc_value_t drive_power_osc_value_from_fixed32(fixed32_t value)
{
    return (osc_value_t)(value >> (sizeof(osc_value_t) * 8 - OSC_VALUE_FRACT_BITS));
}

/**
 * Преобразует значение осциллограммы в fixed32_t.
 * @param value Значение осциллограммы.
 * @return Значение осциллограммы в fixed32_t.
 */
ALWAYS_INLINE static fixed32_t drive_power_osc_value_to_fixed32(osc_value_t value)
{
    return (((fixed32_t)value) << (sizeof(osc_value_t) * 8 - OSC_VALUE_FRACT_BITS));
}

/**
 * Обрабатывает очередные значения АЦП.
 * @param channels Маска каналов АЦП.
 * @param adc_values Значения АЦП.
 * @return Код ошибки.
 */
extern err_t drive_power_process_adc_values(power_channels_t channels, uint16_t* adc_values);

/**
 * Вычисляет значения каналов АЦП.
 * @param channels Маска каналов АЦП.
 * @param err Код ошибки.
 * @return Флаг обработки результатов, независимо от ошибки.
 */
extern bool drive_power_calc_values(power_channels_t channels, phase_t phase, err_t* err);

/**
 * Записывает текущее значение нуля в калиброванное.
 * @param channels Маска каналов АЦП.
 * @return Код ошибки.
 */
extern err_t drive_power_calibrate(power_channels_t channels);

/**
 * Получает калибровочные данные канала АЦП.
 * @param channel Номер канала АЦП.
 * @return Калибровочные данные канала АЦП.
 */
extern uint16_t drive_power_calibration_data(size_t channel);

/**
 * Получает калибровочные данные канала АЦП.
 * @param channel Номер канала АЦП.
 * @param data Калибровочные данные канала АЦП.
 */
extern void drive_power_set_calibration_data(size_t channel, uint16_t data);

/**
 * Получает флаг доступности данных на всех заданных каналах АЦП.
 * @param channels Маска каналов АЦП.
 * @return Флаг доступности данных.
 */
extern bool drive_power_data_avail(power_channels_t channels);

/**
 * Сбрасывает накопленные данные каналов АЦП.
 * @param channels Каналы АЦП.
 * @return Код ошибки.
 */
extern err_t drive_power_reset_channels(power_channels_t channels);

/**
 * Получает сырое последнее мгновенное значение канала АЦП.
 * @param channel Номер канала.
 * @return Сырое последнее мгновенное значение канала АЦП.
 */
extern int16_t drive_power_channel_raw_value_inst(size_t channel);

/**
 * Получает сырое значение канала АЦП.
 * @param channel Номер канала.
 * @return Сырое значение канала АЦП.
 */
extern int16_t drive_power_channel_raw_value(size_t channel);

/**
 * Получает реальное последнее мгновенное значение канала АЦП.
 * @param channel Номер канала.
 * @return Реальное последнее мгновенное значение канала АЦП.
 */
extern fixed32_t drive_power_channel_real_value_inst(size_t channel);

/**
 * Получает реальное значение канала АЦП.
 * @param channel Номер канала.
 * @return Реальное значение канала АЦП.
 */
extern fixed32_t drive_power_channel_real_value(size_t channel);

#endif /* DRIVE_POWER_H */

