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
#include "drive_triacs.h"

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

// Алиасы каналов значений токов и напряжений.
#define DRIVE_POWER_CHANNEL_Ua POWER_CHANNEL_0
#define DRIVE_POWER_CHANNEL_Ia POWER_CHANNEL_1
#define DRIVE_POWER_CHANNEL_Ub POWER_CHANNEL_2
#define DRIVE_POWER_CHANNEL_Ib POWER_CHANNEL_3
#define DRIVE_POWER_CHANNEL_Uc POWER_CHANNEL_4
#define DRIVE_POWER_CHANNEL_Ic POWER_CHANNEL_5
#define DRIVE_POWER_CHANNEL_Urot POWER_CHANNEL_6
#define DRIVE_POWER_CHANNEL_Irot POWER_CHANNEL_7
#define DRIVE_POWER_CHANNEL_Iexc POWER_CHANNEL_8
#define DRIVE_POWER_CHANNEL_Iref POWER_CHANNEL_9
#define DRIVE_POWER_CHANNEL_Ifan POWER_CHANNEL_10

//! Каналы АЦП.
#define DRIVE_POWER_CHANNELS (POWER_CHANNEL_0 | POWER_CHANNEL_1 | POWER_CHANNEL_2 |\
                              POWER_CHANNEL_3 | POWER_CHANNEL_4 | POWER_CHANNEL_5 |\
                              POWER_CHANNEL_6 | POWER_CHANNEL_7 | POWER_CHANNEL_8 |\
                              POWER_CHANNEL_9 | POWER_CHANNEL_10)

//! Частота измерений АЦП.
#define DRIVE_POWER_ADC_FREQ 6400

//! Частота сети.
#define DRIVE_POWER_FREQ 50

//! Число итераций накопления и обработки данных за период.
#define DRIVE_POWER_PERIOD_ITERS 3

//! Число итераций накопления и обработки данных по-умолчанию.
#define DRIVE_POWER_PROCESSING_ITERS_DEFAULT 3


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
#define DRIVE_POWER_OSC_CHANNEL_LEN (225) // 0.07 / (0.00015625 * 2) + 1

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
 * Обновляет настройки питания привода.
 * @return Код ошибки.
 */
extern err_t drive_power_update_settings(void);

/**
 * Сбрасывает состояние питания привода.
 */
extern void drive_power_reset(void);

/**
 * Получает фазу для которой нужно вычислять ток.
 * @return Фаза с вычислением тока.
 */
extern phase_t drive_power_phase_calc_current(void);

/**
 * Получает фазу для которой нужно вычислять напряжение.
 * @return Фаза с вычислением напряжения.
 */
extern phase_t drive_power_phase_calc_voltage(void);

/**
 * Получает флаг вычисления тока якоря.
 * @return Флаг вычисления тока якоря.
 */
extern bool drive_power_rot_calc_current(void);

/**
 * Получает флаг вычисления напряжения якоря.
 * @return Флаг вычисления напряжения якоря.
 */
extern bool drive_power_rot_calc_voltage(void);

/**
 * Устанавливает фазу для которой нужно вычислять ток.
 * @param phase Фаза с вычислением тока.
 */
extern void drive_power_set_phase_calc_current(phase_t phase);

/**
 * Устанавливает фазу для которой нужно вычислять напряжение.
 * @param phase Фаза с вычислением напряжения.
 */
extern void drive_power_set_phase_calc_voltage(phase_t phase);

/**
 * Устанавливает флаг вычисления тока якоря.
 * @param Флаг вычисления тока якоря.
 */
extern void drive_power_set_rot_calc_current(bool calc);

/**
 * Устанавливает флаг вычисления напряжения якоря.
 * @param Флаг вычисления напряжения якоря.
 */
extern void drive_power_set_rot_calc_voltage(bool calc);

/**
 * Получает количество итераций накопления данных.
 * @return Количество итераций накопления данных.
 */
extern size_t drive_power_processing_iters(void);

/**
 * Устанавливает количество итераций накопления данных.
 * @param periods Количество итераций накопления данных.
 * @return Код ошибки.
 */
extern err_t drive_power_set_processing_iters(size_t iters);

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
 * Прекращает запись осциллограмм.
 */
extern void drive_power_oscillogram_pause(void);

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
 * Получает флаг необходимости останова записи осциллограмм.
 * @return Флаг необходимости останова записи осциллограмм.
 */
extern bool drive_power_oscillogram_is_paused(void);

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
 * Обрабатывает накопленные данные АЦП.
 * @param channels Маска каналов АЦП.
 * @return Код ошибки.
 */
extern err_t drive_power_process_accumulated_data(power_channels_t channels);

/**
 * Получает флаг ошибки тиристоров.
 * @return Флаг ошибки тиристоров.
 */
extern bool drive_power_triacs_fail(void);

/**
 * Получает количество открытий тиристора.
 * @param triac_number Номер тиристора.
 * @return Количество открытий тиристора.
 */
extern size_t drive_power_triac_open_count(triac_number_t triac_number);

/**
 * Вычисляет значения каналов АЦП.
 * @param channels Маска каналов АЦП.
 * @param err Код ошибки.
 * @return Флаг обработки результатов, независимо от ошибки.
 */
extern bool drive_power_calc_values(power_channels_t channels, err_t* err);

/**
 * Записывает текущее значение нуля в калиброванное.
 * @param channels Маска каналов АЦП.
 * @return Код ошибки.
 */
extern err_t drive_power_calibrate(power_channels_t channels);

/**
 * Получает множитель значения канала АЦП.
 * @param channel Номер канала АЦП.
 * @return Множитель значения канала АЦП.
 */
extern fixed32_t drive_power_adc_value_multiplier(size_t channel);

/**
 * Устанавливает множитель значения канала АЦП.
 * @param channel Номер канала АЦП.
 * @param data Множитель значения канала АЦП.
 */
extern void drive_power_set_adc_value_multiplier(size_t channel, fixed32_t mult);

/**
 * Получает калибровочные данные канала АЦП.
 * @param channel Номер канала АЦП.
 * @return Калибровочные данные канала АЦП.
 */
extern uint16_t drive_power_calibration_data(size_t channel);

/**
 * Устанавливает калибровочные данные канала АЦП.
 * @param channel Номер канала АЦП.
 * @param data Калибровочные данные канала АЦП.
 */
extern void drive_power_set_calibration_data(size_t channel, uint16_t data);

/**
 * Получает множитель значения канала.
 * @param channel Номер канала.
 * @return Множитель значения канала.
 */
extern fixed32_t drive_power_value_multiplier(size_t channel);

/**
 * Устанавливает множитель значения канала.
 * @param channel Номер канала.
 * @param data Множитель значения канала.
 */
extern void drive_power_set_value_multiplier(size_t channel, fixed32_t mult);

/**
 * Получает тип канала питания.
 * @param channel Номер канала.
 * @return Тип канала.
 */
extern power_channel_type_t drive_power_channel_type(size_t channel);

/**
 * Получает флаг доступности данных на всех заданных каналах АЦП.
 * @param channels Маска каналов АЦП.
 * @return Флаг доступности данных.
 */
extern bool drive_power_data_avail(power_channels_t channels);

/**
 * Получает флаг доступности новых данных на всех заданных каналах АЦП.
 * @param channels Каналы АЦП.
 * @return Флаг доступности новых данных.
 */
extern bool drive_power_new_data_avail(power_channels_t channels);

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

/**
 * Получает напряжение по углу открытия тиристоров.
 * @return Напряжение по углу открытия тиристоров.
 */
extern fixed32_t drive_power_open_angle_voltage(void);

#endif /* DRIVE_POWER_H */

