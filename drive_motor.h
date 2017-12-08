/**
 * @file drive_motor.h Библиотека двигатя привода.
 */

#ifndef DRIVE_MOTOR_H
#define DRIVE_MOTOR_H


#include "errors/errors.h"
#include "fixed/fixed32.h"
#include <stdbool.h>


/**
 * Инициализирует двигатель привода.
 * @return Код ошибки.
 */
extern err_t drive_motor_init(void);

/**
 * Обновляет настройки двигателя привода.
 * @return Код ошибки.
 */
extern err_t drive_motor_update_settings(void);

/**
 * Получает флаг возможности расчёта.
 * @return Флаг возможности расчёта.
 */
extern bool drive_motor_ready(void);

/**
 * Получает разрешение IR-компенсации.
 * @return Разрешение IR-компенсации.
 */
extern bool drive_motor_ir_compensation_enabled(void);

/**
 * Устанавливает разрешение IR-компенсации.
 * @param enabled Разрешение IR-компенсации.
 */
extern void drive_motor_set_ir_compensation_enabled(bool enabled);

/**
 * Выполняет расчёт.
 */
extern void drive_motor_calculate(void);

/**
 * Получает номинальную ЭДС двигателя.
 * @return Номинальная ЭДС двигателя.
 */
extern fixed32_t drive_motor_e_nom(void);

/**
 * Получает номинальные обороты двигателя.
 * @return Номинальные обороты двигателя.
 */
extern fixed32_t drive_motor_rpm_nom(void);

/**
 * Получает максимальные обороты двигателя.
 * @return Максимальные обороты двигателя.
 */
extern fixed32_t drive_motor_rpm_max(void);

/**
 * Получает номинальный момент двигателя.
 * @return Номинальный момент двигателя.
 */
extern fixed32_t drive_motor_m_nom(void);

/**
 * Получает сопротивление якоря.
 * @return Сопротивление якоря.
 */
extern fixed32_t drive_motor_r_rot(void);

/**
 * Получает индуктивность якоря.
 * @return Индуктивность якоря.
 */
extern fixed32_t drive_motor_l_rot(void);

/**
 * Получает сопротивление возбуждения.
 * @return Сопротивление возбуждения.
 */
extern fixed32_t drive_motor_r_exc(void);

/**
 * Получает сопротивление проводов до двигателя.
 * @return Сопротивление проводов до двигателя.
 */
extern fixed32_t drive_motor_r_wires(void);

/**
 * Получает падение напряжения на проводах до двигателя.
 * @return Падение напряжения на проводах до двигателя.
 */
extern fixed32_t drive_motor_u_wires(void);

/**
 * Получает текущие обороты двигателя.
 * @return Текущие обороты двигателя.
 */
extern fixed32_t drive_motor_rpm(void);

/**
 * Получает текущий момент двигателя.
 * @return Текущий момент двигателя.
 */
extern fixed32_t drive_motor_torque(void);

/**
 * Получает ЭДС двигателя.
 * @return ЭДС двигателя.
 */
extern fixed32_t drive_motor_e_rot(void);

/**
 * Получает текущие максимальные обороты двигателя.
 * @return Текущие максимальные обороты двигателя.
 */
extern fixed32_t drive_motor_rpm_cur_max(void);

#endif /* DRIVE_MOTOR_H */

