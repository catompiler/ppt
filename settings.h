/* 
 * @file settings.h Библиотека настроек.
 */

#ifndef SETTINGS_H
#define	SETTINGS_H

#include <stdint.h>
#include <stdbool.h>
#include "fixed/fixed32.h"
#include "errors/errors.h"


/**
 * Инициализирует настройки значениями по-умолчанию.
 */
extern void settings_init(void);

/**
 * Читает настройки.
 * @return Код ошибки.
 */
extern err_t settings_read(void);

/**
 * Записывает настройки.
 * @return Код ошибки.
 */
extern err_t settings_write(void);

/**
 * Получает флаг только для чтения настроек.
 * @return Флаг только для чтения настроек.
 */
extern bool settings_readonly(void);

/**
 * Устанавливает флаг только для чтения настроек.
 * @param readonly Флаг только для чтения настроек.
 */
extern void settings_set_readonly(bool readonly);

/**
 * Получает номинальное напряжение, В.
 * @return Номинальное напряжение, В.
 */
extern uint8_t settings_nominal_voltage(void);

/**
 * Устанавливает номинальное напряжение, В.
 * @param voltage Номинальное напряжение, В.
 * @return Флаг успешности установки.
 */
extern bool settings_set_nominal_voltage(uint8_t voltage);

/**
 * Получает разрешённое отклонение от номинального напряжения, %.
 * @return Разрешённое отклонение от номинального напряжения, %.
 */
extern uint8_t settings_allowed_nominal_voltage_variation(void);

/**
 * Устанавливает разрешённое отклонение от номинального напряжения, %.
 * @param variation Разрешённое отклонение от номинального напряжения, %.
 * @return Флаг успешности установки.
 */
extern bool settings_set_allowed_nominal_voltage_variation(uint8_t variation);

/**
 * Получает разрешённое отклонение от номинального напряжения, %.
 * @return Разрешённое отклонение от номинального напряжения, %.
 */
extern uint8_t settings_critical_nominal_voltage_variation(void);

/**
 * Устанавливает разрешённое отклонение от номинального напряжения, %.
 * @param variation Разрешённое отклонение от номинального напряжения, %.
 * @return Флаг успешности установки.
 */
extern bool settings_set_critical_nominal_voltage_variation(uint8_t variation);

/**
 * Получает допустимый шум нуля по напряжению, мВ.
 * @return Допустимый шим нуля по напряжению, мВ.
 */
extern uint16_t settings_zero_voltage_noise(void);

/**
 * Устанавливает допустимый шум нуля по напряжению, мВ.
 * @param noise Допустимый шум нуля по напряжению, мВ.
 * @return Флаг успешности установки.
 */
extern bool settings_set_zero_voltage_noise(uint16_t noise);

/**
 * Получает допустимый шум нуля по току, мА.
 * @return Допустимый шим нуля по току, мА.
 */
extern uint16_t settings_zero_current_noise(void);

/**
 * Устанавливает допустимый шум нуля по току, мА.
 * @param noise Допустимый шум нуля по току, мА.
 * @return Флаг успешности установки.
 */
extern bool settings_set_zero_current_noise(uint16_t noise);

#endif	/* SETTINGS_H */
