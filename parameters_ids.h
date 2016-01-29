/**
 * @file parameters_ids.h Идентификаторы параметров.
 */

#ifndef PARAMETERS_IDS_H
#define	PARAMETERS_IDS_H

// Идентификаторы.
/**
 * Номинальное напряжение, В.
 */
#define PARAM_ID_U_NOM 1
/**
 * Разрешённое отклонение от номинального напряжения, %.
 */
#define PARAM_ID_U_NOM_ALLOW_VAR 2
/**
 * Критическое отклонение от номинального напряжения, %.
 */
#define PARAM_ID_U_NOM_CRIT_VAR 3
/**
 * Допустимый шим нуля по напряжению, В.
 */
#define PARAM_ID_U_ZERO_NOISE 4
/**
 * Допустимый шим нуля по току, А.
 */
#define PARAM_ID_I_ZERO_NOISE 5
/**
 * Фаза возбуждения.
 */
#define PARAM_ID_EXC_PHASE 6
/**
 * Время разгона.
 */
#define PARAM_ID_RAMP_TIME 7
/**
 * Коэффициент пропорционального звена ПИД-регулятора напряжения якоря.
 */
#define PARAM_ID_ROT_PID_K_P 8
/**
 * Коэффициент интегрального звена ПИД-регулятора напряжения якоря.
 */
#define PARAM_ID_ROT_PID_K_I 9
/**
 * Коэффициент дифференциального звена ПИД-регулятора напряжения якоря.
 */
#define PARAM_ID_ROT_PID_K_D 10
/**
 * Коэффициент пропорционального звена ПИД-регулятора тока возбуждения.
 */
#define PARAM_ID_EXC_PID_K_P 11
/**
 * Коэффициент интегрального звена ПИД-регулятора тока возбуждения.
 */
#define PARAM_ID_EXC_PID_K_I 12
/**
 * Коэффициент дифференциального звена ПИД-регулятора тока возбуждения.
 */
#define PARAM_ID_EXC_PID_K_D 13
/**
 * Номинальное напряжение якоря двигателя, В.
 */
#define PARAM_ID_U_ROT_NOM 14
/**
 * Номинальный ток якоря двигателя, А.
 */
#define PARAM_ID_I_ROT_NOM 15
/**
 * Ток возбуждения двигателя, А.
 */
#define PARAM_ID_I_EXC 16
/**
 * Время ожидания остановки ротора при выключении, с.
 */
#define PARAM_ID_ROT_STOP_TIME 17
/**
 * Время ожидания остановки возбуждения при выключении, с.
 */
#define PARAM_ID_EXC_STOP_TIME 18
/**
 * Время ожидания включения возбуждения при выключении, с.
 */
#define PARAM_ID_EXC_START_TIME 19


#endif	/* PARAMETERS_IDS_H */

