/**
 * @file drive_selftuning.h Библиотека самонастройки привода.
 */

#ifndef DRIVE_SELFTUNING_H
#define DRIVE_SELFTUNING_H


#include "errors/errors.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "fixed/fixed32.h"


typedef void* drive_selftuning_data_iter_t;


/**
 * Инициализирует самонастройку привода.
 * @return Код ошибки.
 */
extern err_t drive_selftuning_init(void);

/**
 * Сбрасывает самонастройку привода.
 */
extern void drive_selftuning_reset(void);

/**
 * Устанавливает флаг сбора данных самонастройки.
 */
extern void drive_selftuning_set_data_collecting(bool collecting);

/**
 * Получает флаг сбора данных для самонастройки.
 */
extern bool drive_selftuning_data_collecting(void);

/**
 * Получает флаг окончания сбора данных для самонастройки.
 */
extern bool drive_selftuning_data_collected(void);

/**
 * Сбрасывает внутренний счётчик времени данных АЦП.
 */
extern void drive_selftuning_reset_adc_time(void);

/**
 * Устанавливает кратность АЦП.
 * @param adc_rate кратность АЦП.
 */
extern void drive_selftuning_set_adc_rate(uint32_t adc_rate);

/**
 * Добавляет точку для самонастройки.
 * @param U_rot_adc Данные АЦП напряжения якоря.
 * @param I_rot_adc Данные АЦП тока якоря.
 * @return Флаг добавления точки.
 */
extern bool drive_selftuning_put(uint16_t U_rot_adc, uint16_t I_rot_adc);

/**
 * Вычисляет значения данных ацп.
 */
extern void drive_selftuning_calculate_adc_data(void);

/**
 * Получает итератор данных самонастройки.
 * @return Итератор данных.
 */
extern drive_selftuning_data_iter_t drive_selftuning_data_iter_begin(void);

/**
 * Получает следующий итератор данных самонастройки.
 * @param iter Итератор данных.
 * @return Следующий итератор данных.
 */
extern drive_selftuning_data_iter_t drive_selftuning_data_iter_next(drive_selftuning_data_iter_t iter);

/**
 * Получает флаг нахождения итератора в конце данных,
 * @param iter Итератор данных.
 * @return Флаг конца данных.
 */
extern bool drive_selftuning_data_iter_at_end(drive_selftuning_data_iter_t iter);

/**
 * Получает напряжение якоря текущих данных.
 * @param iter Итератор данных.
 * @return Напряжение якоря.
 */
extern fixed32_t drive_selftuning_data_iter_urot(drive_selftuning_data_iter_t iter);

/**
 * Получает ток якоря текущих данных.
 * @param iter Итератор данных.
 * @return Ток якоря.
 */
extern fixed32_t drive_selftuning_data_iter_irot(drive_selftuning_data_iter_t iter);

/**
 * Получает время текущих данных.
 * @param iter Итератор данных.
 * @return Время.
 */
extern fixed32_t drive_selftuning_data_iter_time(drive_selftuning_data_iter_t iter);

#endif /* DRIVE_SELFTUNING_H */

