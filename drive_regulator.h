/**
 * @file drive_regulator.h Библиотека регулирования привода.
 */

#ifndef DRIVE_REGULATOR_H
#define DRIVE_REGULATOR_H

#include <stdbool.h>
#include "errors/errors.h"
#include "fixed/fixed32.h"
#include "pid_controller/pid_controller.h"
#include "ramp.h"

//! Тип состояния регулятора привода.
typedef enum _Drive_Regulator_State {
    DRIVE_REGULATOR_STATE_IDLE = 0,
    DRIVE_REGULATOR_STATE_START,
    DRIVE_REGULATOR_STATE_RUN,
    DRIVE_REGULATOR_STATE_STOP
} drive_regulator_state_t;

//! Тип задания.
typedef ramp_reference_t reference_t;
//! Минимальное задание.
#define REFERENCE_MIN 0
//! Максимальное задание.
#define REFERENCE_MAX 95

/**
 * Инициализирует регулятор привода.
 * @return Код ошибки.
 */
extern err_t drive_regulator_init(void);

/**
 * Получает ПИД-регулятор напряжения ротора.
 * Для отладки.
 * @return ПИД-регулятор напряжения ротора.
 */
extern pid_controller_t* drive_regulator_rot_pid(void);

/**
 * Получает ПИД-регулятор тока возбуждения.
 * Для отладки.
 * @return ПИД-регулятор тока возбуждения.
 */
extern pid_controller_t* drive_regulator_exc_pid(void);

/**
 * Получает состояние регулятора привода.
 * @return Состояние регулятора привода.
 */
extern drive_regulator_state_t drive_regulator_state(void);

/**
 * Получает значение задания.
 */
extern reference_t drive_regulator_reference(void);

/**
 * Устанавливает значение задания.
 * @param reference Задание.
 * @return Код ошибки.
 */
extern err_t drive_regulator_set_reference(reference_t reference);

/**
 * Запускает регулирование.
 */
extern void drive_regulator_start(void);

/**
 * Останавливает регулирование.
 */
extern void drive_regulator_stop(void);

/**
 * Устанавливает время разгона от 0 до 100%.
 * @param time Время разгона.
 * @return Код ошибки.
 */
extern err_t drive_regulator_set_ramp_time(ramp_time_t time);

/**
 * Получает разрешение регулирования напряжения якоря.
 * @return Разрешение регулирования напряжения якоря.
 */
extern bool drive_regulator_rot_enabled(void);

/**
 * Устанавливает разрешение регулирования напряжения якоря.
 * @param enabled Разрешение регулирования напряжения якоря.
 */
extern void drive_regulator_set_rot_enabled(bool enabled);

/**
 * Получает разрешение регулирования тока возбуждения.
 * @return Разрешение регулирования тока возбуждения.
 */
extern bool drive_regulator_exc_enabled(void);

/**
 * Устанавливает разрешение регулирования тока возбуждения.
 * @param enabled Разрешение регулирования тока возбуждения.
 */
extern void drive_regulator_set_exc_enabled(bool enabled);

/**
 * Устанавливает коэффициенты ПИД-регулятора напряжения ротора.
 * @param kp Коэффициент пропорционального звена.
 * @param ki Коэффициент интегрального звена.
 * @param kd Коэффициент дифференциального звена.
 */
extern void drive_regulator_set_rot_pid(fixed32_t kp, fixed32_t ki, fixed32_t kd);

/**
 * Устанавливает пределы значений ПИД-регулятора напряжения ротора.
 * @param pid_min Минимальное значение ПИД.
 * @param pid_max Максимальное значение ПИД.
 */
extern void drive_regulator_rot_pid_clamp(fixed32_t pid_min, fixed32_t pid_max);

/**
 * Получает значение ПИД-регулятора напряжения ротора.
 * @return Значение ПИД-регулятора напряжения ротора.
 */
extern fixed32_t drive_regulator_rot_pid_value(void);

/**
 * Устанавливает коэффициенты ПИД-регулятора тока возбуждения.
 * @param kp Коэффициент пропорционального звена.
 * @param ki Коэффициент интегрального звена.
 * @param kd Коэффициент дифференциального звена.
 */
extern void drive_regulator_set_exc_pid(fixed32_t kp, fixed32_t ki, fixed32_t kd);

/**
 * Устанавливает пределы значений ПИД-регулятора тока возбуждения.
 * @param pid_min Минимальное значение ПИД.
 * @param pid_max Максимальное значение ПИД.
 */
extern void drive_regulator_exc_pid_clamp(fixed32_t pid_min, fixed32_t pid_max);

/**
 * Получает значение ПИД-регулятора тока возбуждения.
 * @return Значение ПИД-регулятора тока возбуждения.
 */
extern fixed32_t drive_regulator_exc_pid_value(void);

/**
 * Получает значение номинального напряжения якоря.
 * @return Номинальное напряжение якоря.
 */
extern fixed32_t drive_regulator_rot_nom_voltage(void);

/**
 * Устанавливает значение номинального напряжения якоря.
 * @param voltage Номинальное напряжение якоря.
 */
extern void drive_regulator_set_rot_nom_voltage(fixed32_t voltage);

/**
 * Получает значение тока возбуждения.
 * @return Ток возбуждения.
 */
extern fixed32_t drive_regulator_exc_current(void);

/**
 * Устанавливает значение тока возбуждения.
 * @param voltage Ток возбуждения.
 */
extern void drive_regulator_set_exc_current(fixed32_t current);

/**
 * Выполняет ПИД-регулирование.
 * @param dt Интервал времени с прошлого регулирования.
 * @param u_rot_back Напряжения ротора по обратной связи.
 * @param i_exc_back Ток возбуждения по обратной связи.
 * @return Флаг регулировки.
 */
extern bool drive_regulator_regulate(fixed32_t u_rot_back, fixed32_t i_exc_back);

#endif /* DRIVE_REGULATOR_H */
