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

//! Режим регулятора.
typedef enum _Drive_Regulator_Mode {
    DRIVE_REGULATOR_MODE_SPEED = 0,  //!< Поддержание скорости (напряжения).
    DRIVE_REGULATOR_MODE_TORQUE = 1 //!< Поддержание момента (тока).
} drive_regulator_mode_t;

//! Тип задания.
typedef ramp_reference_t reference_t;
//! Минимальное задание.
#define REFERENCE_MIN 0
#define REFERENCE_MIN_F 0
//! Максимальное задание.
#define REFERENCE_MAX 100
#define REFERENCE_MAX_F 0x640000

/**
 * Инициализирует регулятор привода.
 * @return Код ошибки.
 */
extern err_t drive_regulator_init(void);

/**
 * Получает ПИД-регулятор скорости.
 * Для отладки.
 * @return ПИД-регулятор скорости.
 */
extern pid_controller_t* drive_regulator_spd_pid(void);

/**
 * Получает ПИД-регулятор тока ротора.
 * Для отладки.
 * @return ПИД-регулятор тока ротора.
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
 * Получает режим регулятора.
 * @return Режим регулятора.
 */
extern drive_regulator_mode_t drive_regulator_mode(void);

/**
 * Устанавливает режим регулятора.
 * @param mode Режим регулятора.
 */
extern void drive_regulator_set_mode(drive_regulator_mode_t mode);

/**
 * Получает значение задания.
 * @return Задание.
 */
extern reference_t drive_regulator_reference(void);

/**
 * Получает значение текущего задания (с учётом разгона).
 * @return Текущее задание.
 */
extern fixed32_t drive_regulator_current_reference(void);

/**
 * Устанавливает значение задания.
 * @param reference Задание.
 * @return Код ошибки.
 */
extern err_t drive_regulator_set_reference(reference_t reference);

/**
 * Увеличивает значение задания.
 * @return Флаг изменения задания.
 */
extern void drive_regulator_inc_reference(void);

/**
 * Уменьшает значение задания.
 * @return Флаг изменения задания.
 */
extern void drive_regulator_dec_reference(void);

/**
 * Подстраивает текущее значение разгона
 * согласно выходному значению питания.
 */
extern void drive_regulator_adjust_current_reference(void);

/**
 * Выполняет запуск.
 */
extern void drive_regulator_start(void);

/**
 * Выполняет останов.
 */
extern void drive_regulator_stop(void);

/**
 * Выполняет быстрый останов.
 */
extern void drive_regulator_fast_stop(void);

/**
 * Устанавливает время изменения задания от 0 до 100%.
 * @param time Время изменения задания.
 * @return Код ошибки.
 */
extern err_t drive_regulator_set_reference_time(ramp_time_t time);

/**
 * Устанавливает время разгона от 0 до 100%.
 * @param time Время разгона.
 * @return Код ошибки.
 */
extern err_t drive_regulator_set_start_time(ramp_time_t time);

/**
 * Устанавливает время останова от 100 до 0%.
 * @param time Время разгона.
 * @return Код ошибки.
 */
extern err_t drive_regulator_set_stop_time(ramp_time_t time);

/**
 * Устанавливает время быстрого останова от 100 до 0%.
 * @param time Время разгона.
 * @return Код ошибки.
 */
extern err_t drive_regulator_set_fast_stop_time(ramp_time_t time);

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
 * Устанавливает коэффициенты ПИД-регулятора скорости.
 * @param kp Коэффициент пропорционального звена.
 * @param ki Коэффициент интегрального звена.
 * @param kd Коэффициент дифференциального звена.
 */
extern void drive_regulator_set_spd_pid(fixed32_t kp, fixed32_t ki, fixed32_t kd);

/**
 * Устанавливает пределы значений ПИД-регулятора скорости.
 * @param pid_min Минимальное значение ПИД.
 * @param pid_max Максимальное значение ПИД.
 */
extern void drive_regulator_spd_pid_clamp(fixed32_t pid_min, fixed32_t pid_max);

/**
 * Устанавливает коэффициенты ПИД-регулятора тока ротора.
 * @param kp Коэффициент пропорционального звена.
 * @param ki Коэффициент интегрального звена.
 * @param kd Коэффициент дифференциального звена.
 */
extern void drive_regulator_set_rot_pid(fixed32_t kp, fixed32_t ki, fixed32_t kd);

/**
 * Устанавливает пределы значений ПИД-регулятора тока ротора.
 * @param pid_min Минимальное значение ПИД.
 * @param pid_max Максимальное значение ПИД.
 */
extern void drive_regulator_rot_pid_clamp(fixed32_t pid_min, fixed32_t pid_max);

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
 * Получает значение номинального тока якоря.
 * @return Номинальный ток якоря.
 */
extern fixed32_t drive_regulator_rot_nom_current(void);

/**
 * Устанавливает значение номинального тока якоря.
 * @param current Номинальный ток якоря.
 */
extern void drive_regulator_set_rot_nom_current(fixed32_t current);

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
 * Получает значение текущей ЭДС задания (с учётом разгона).
 * @return Текущее напряжение задания.
 */
extern fixed32_t drive_regulator_current_rpm_ref(void);

/**
 * Получает значение текущего тока задания (с учётом разгона).
 * @return Текущий ток задания.
 */
extern fixed32_t drive_regulator_current_i_ref(void);

/**
 * Получает задание тока от регулятора скорости.
 * @return Задание тока от регулятора скорости.
 */
extern fixed32_t drive_regulator_rot_speed_current_ref(void);

/**
 * Получает угол открытия для ротора.
 * @return Угол открытия для ротора.
 */
extern fixed32_t drive_regulator_rot_open_angle(void);

/**
 * Получает угол открытия для возбуждения.
 * @return Угол открытия для возбуждения.
 */
extern fixed32_t drive_regulator_exc_open_angle(void);

/**
 * Выполняет ПИД-регулирование.
 * @return Флаг регулировки.
 */
extern bool drive_regulator_regulate(void);

#endif /* DRIVE_REGULATOR_H */
