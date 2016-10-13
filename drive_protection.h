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

typedef enum _Drive_Top_Check_Res {
    DRIVE_TOP_CHECK_NORMAL = 0,
    DRIVE_TOP_CHECK_HEATING,
    DRIVE_TOP_CHECK_COOLING,
    DRIVE_TOP_CHECK_OVERHEAT
} drive_top_check_res_t;

typedef enum _Drive_Break_Check_Res {
    DRIVE_BREAK_CHECK_NORMAL = 0,
    DRIVE_BREAK_CHECK_FAIL
} drive_break_check_res_t;

/**
 * Инициализирует защиту привода.
 * @return Флаг успешной инициализации.
 */
extern bool drive_protection_init(void);

/**
 * Инициализирует тепловую защиту.
 * (Thermal Overload Protection).
 * @param t6 Время срабатывания защиты при шестикратной перегрузке по току.
 * @return Флаг успешной инициализации.
 */
extern bool drive_protection_init_top(fixed32_t t6);

/**
 * Обрабатывает нагрев/охлаждение под действием тока за заданный интервал времени.
 * @param I_rot Текущий ток ротора.
 * @param dt Интервал времени действия тока.
 */
extern void drive_protection_top_process(fixed32_t I_rot, fixed32_t dt);

/**
 * Производит проверку текущего состояния тепловой защиты.
 * @return Результат проверки тепловой защиты.
 */
extern drive_top_check_res_t drive_protection_top_check(void);

/**
 * Получает флаг готовности привода по тепловой защите.
 * @return Флаг готовности привода по тепловой защите.
 */
extern bool drive_protection_top_ready(void);

/**
 * Получает флаг допустимости результата проверки.
 * @param check_res Результат проверки,
 * @return Флаг допустимости результата провераки.
 */
extern bool drive_protection_is_allow(drive_pwr_check_res_t check_res);

/**
 * Устанавливает входное напряжение.
 * @param u_in Входное напряжения, В.
 * @param allow_delta Допустимое отклонение, %.
 * @param crit_delta Критическое отклонение, %.
 */
extern void drive_protection_set_input_voltage(fixed32_t u_in, uint32_t allow_delta, uint32_t crit_delta);

/**
 * Устанавливает токовую отсечку по фазам.
 * @param i_in_cutoff Значение токовой отсечки, А.
 */
extern void drive_protection_set_phases_current_cutoff(fixed32_t i_in_cutoff);

/**
 * Устанавливает шум нулевого напряжения.
 * @param u_noize Шум нулевого напряжения.
 */
extern void drive_protection_set_zero_voltage_noise(fixed32_t u_noize);

/**
 * Устанавливает шум нулевого тока.
 * @param i_noize Шум нулевого тока.
 */
extern void drive_protection_set_zero_current_noise(fixed32_t i_noize);

/**
 * Устанавливает шум нулевого тока ротора.
 * @param i_noize Шум нулевого тока ротора.
 */
extern void drive_protection_set_rot_zero_current_noise(fixed32_t i_noize);

/**
 * Устанавливает шум нулевого тока возбуждения.
 * @param i_noize Шум нулевого тока возбуждения.
 */
extern void drive_protection_set_exc_zero_current_noise(fixed32_t i_noize);

/**
 * Устанавливает ток возбуждения.
 * @param i_exc Ток возбуждения.
 * @param allow_delta Допустимое отклонение.
 * @param crit_delta Критическое отклонение.
 */
extern void drive_protection_set_exc_current(fixed32_t i_exc, uint32_t allow_delta, uint32_t crit_delta);

/**
 * Устанавливает напряжение якоря.
 * @param u_rot Напряжение якоря.
 * @param allow_delta Допустимое отклонение.
 * @param crit_delta Критическое отклонение.
 */
extern void drive_protection_set_rot_voltage(fixed32_t u_rot, uint32_t allow_delta, uint32_t crit_delta);

/**
 * Устанавливает ток якоря.
 * @param i_rot Ток якоря.
 * @param allow_delta Допустимое отклонение.
 * @param crit_delta Критическое отклонение.
 * @param cutoff_mult Множитель токовой отсечки.
 */
extern void drive_protection_set_rot_current(fixed32_t i_rot, uint32_t allow_delta, uint32_t crit_delta, uint32_t cutoff_mult);

/**
 * Выполняет проверку входного напряжения.
 * @param voltage Напряжение.
 * @return Результат проверки.
 */
extern drive_pwr_check_res_t drive_protection_check_input_voltage(fixed32_t voltage);

/**
 * Выполняет проверку входных токов.
 * @param current Ток.
 * @return Результат проверки.
 */
extern drive_pwr_check_res_t drive_protection_check_input_current(fixed32_t current);

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
 * Выполняет проверку отсутствия тока ротора.
 * @param current Ток ротора.
 * @return Результат проверки.
 */
extern drive_pwr_check_res_t drive_protection_check_rot_zero_current(fixed32_t current);

/**
 * Выполняет проверку отсутствия тока возбуждения.
 * @param current Ток возбуждения.
 * @return Результат проверки.
 */
extern drive_pwr_check_res_t drive_protection_check_exc_zero_current(fixed32_t current);

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

/**
 * Выполняет проверку тока ротора.
 * @param current Ток.
 * @return Результат проверки.
 */
extern drive_pwr_check_res_t drive_protection_check_rot_current(fixed32_t current);

/**
 * Выполняет проверку обрыва ротора.
 * @param voltage Напряжение.
 * @param current Ток.
 * @param u_ref Текущее напряжение задания.
 * @return Результат проверки.
 */
extern drive_break_check_res_t drive_protection_check_rot_break(fixed32_t voltage, fixed32_t current, fixed32_t u_ref);

#endif /* DRIVE_PROTECTION_H */
