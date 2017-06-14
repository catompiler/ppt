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

