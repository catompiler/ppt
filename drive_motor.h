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
 * Выполняет расчёт.
 */
extern void drive_motor_calculate(void);

/**
 * Получает сопротивление якоря.
 * @return Сопротивление якоря.
 */
extern fixed32_t drive_motor_r_rot(void);

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
 * Получает напряжение на якоре с учётом проводов.
 * @return Напряжение на якоре с учётом проводов.
 */
extern fixed32_t drive_motor_u_rot_wires(void);

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

#endif /* DRIVE_MOTOR_H */

