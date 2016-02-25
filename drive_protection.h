/**
 * @file drive_protection.h Библиотека функций защиты привода.
 */

#ifndef DRIVE_PROTECTION_H
#define DRIVE_PROTECTION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "fixed/fixed32.h"


typedef enum _Drive_Pwr_Check_Res {
    DRIVE_PWR_CHECK_NORMAL = 0,
    DRIVE_PWR_CHECK_ALLOW_UNDERFLOW,
    DRIVE_PWR_CHECK_ALLOW_OVERFLOW,
    DRIVE_PWR_CHECK_CRIT_UNDERFLOW,
    DRIVE_PWR_CHECK_CRIT_OVERFLOW,
} drive_pwr_check_res_t;


/**
 * Инициализирует защиту привода.
 * @return Флаг успешной инициализации.
 */
extern bool drive_protection_init(void);

/**
 * Устанавливает входное напряжение.
 * @param u_in Входное напряжения, В.
 * @param allow_delta Допустимое отклонение, %.
 * @param crit_delta Критическое отклонение, %.
 */
extern void drive_protection_set_input_voltage(fixed32_t u_in, uint32_t allow_delta, uint32_t crit_delta);

/**
 * Устанавливает шум нулевого напряжения.
 * @param u_noize Шум нулевого напряжения.
 */
extern void drive_protection_set_zero_voltage_noise(fixed32_t u_noize);

/**
 * Устанавливает шум нулевого тока.
 * @param u_noize Шум нулевого тока.
 */
extern void drive_protection_set_zero_current_noise(fixed32_t i_noize);

/**
 * Устанавливает ток возбуждения.
 * @param i_exc Ток возбуждения.
 * @param allow_delta Допустимое отклонение.
 * @param crit_delta Критическое отклонение.
 */
extern void drive_protection_set_exc_current(fixed32_t i_exc, uint32_t allow_delta, uint32_t crit_delta);

/**
 * Устанавливает напряжение якоря.
 * @param i_exc Напряжение якоря.
 * @param allow_delta Допустимое отклонение.
 * @param crit_delta Критическое отклонение.
 */
extern void drive_protection_set_rot_voltage(fixed32_t u_rot, uint32_t allow_delta, uint32_t crit_delta);

/**
 * Выполняет проверку входного напряжения.
 * @param voltage Напряжение.
 * @return Результат проверки.
 */
extern drive_pwr_check_res_t drive_protection_check_input_voltage(fixed32_t voltage);

/**
 * Выполняет проверку отсутствия напряжения.
 * @param voltage Напряжение.
 * @return Результат проверки.
 */
extern drive_pwr_check_res_t drive_protection_check_zero_voltage(fixed32_t voltage);

/**
 * Выполняет проверку отсутствия тока.
 * @param current Ток.
 * @return Результат проверки.
 */
extern drive_pwr_check_res_t drive_protection_check_zero_current(fixed32_t current);

/**
 * Выполняет проверку тока возбуждения.
 * @param current Ток.
 * @return Результат проверки.
 */
extern drive_pwr_check_res_t drive_protection_check_exc_current(fixed32_t current);

/**
 * Выполняет проверку напряжения ротора.
 * @param voltage Напряжение.
 * @return Результат проверки.
 */
extern drive_pwr_check_res_t drive_protection_check_rot_voltage(fixed32_t voltage);

#endif /* DRIVE_PROTECTION_H */
