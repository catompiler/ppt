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
#include "phase_state/phase_state.h"

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

//! Число периодов накопления и обработки данных по-умолчанию.
#define DRIVE_POWER_PROCESSING_PERIODS_DEFAULT 1


/**
 * Инициализирует питание привода.
 * @return Код ошибки.
 */
extern err_t drive_power_init(void);

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

#endif /* DRIVE_POWER_H */

