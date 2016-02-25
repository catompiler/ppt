/**
 * @file parameters_ids.h Идентификаторы параметров.
 */

#ifndef PARAMETERS_IDS_H
#define	PARAMETERS_IDS_H

// Идентификаторы.
/**
 * Номинальное напряжение, В.
 */
#define PARAM_ID_U_NOM 10
/**
 * Разрешённое отклонение от номинального напряжения, %.
 */
#define PARAM_ID_U_NOM_ALLOW_VAR 11
/**
 * Критическое отклонение от номинального напряжения, %.
 */
#define PARAM_ID_U_NOM_CRIT_VAR 12
/**
 * Допустимый шим нуля по напряжению, В.
 */
#define PARAM_ID_U_ZERO_NOISE 13
/**
 * Допустимый шим нуля по току, А.
 */
#define PARAM_ID_I_ZERO_NOISE 14
/**
 * Номинальное напряжение якоря двигателя, В.
 */
#define PARAM_ID_U_ROT_NOM 15
/**
 * Разрешённое отклонение напряжения якоря, %.
 */
#define PARAM_ID_U_ROT_ALLOW_VAR 16
/**
 * Критическое отклонение напряжения якоря, %.
 */
#define PARAM_ID_U_ROT_CRIT_VAR 17
/**
 * Номинальный ток якоря двигателя, А.
 */
#define PARAM_ID_I_ROT_NOM 18
/**
 * Ток возбуждения двигателя, А.
 */
#define PARAM_ID_I_EXC 19
/**
 * Разрешённое отклонение от тока возбуждения, %.
 */
#define PARAM_ID_I_EXC_ALLOW_VAR 20
/**
 * Критическое отклонение от тока возбуждения, %.
 */
#define PARAM_ID_I_EXC_CRIT_VAR 21
/**
 * Фаза возбуждения.
 */
#define PARAM_ID_EXC_PHASE 50
/**
 * Режим возбуждения.
 */
#define PARAM_ID_EXC_MODE 51
/**
 * Время разгона.
 */
#define PARAM_ID_RAMP_TIME 60
/**
 * Коэффициент пропорционального звена ПИД-регулятора напряжения якоря.
 */
#define PARAM_ID_ROT_PID_K_P 70
/**
 * Коэффициент интегрального звена ПИД-регулятора напряжения якоря.
 */
#define PARAM_ID_ROT_PID_K_I 71
/**
 * Коэффициент дифференциального звена ПИД-регулятора напряжения якоря.
 */
#define PARAM_ID_ROT_PID_K_D 72
/**
 * Коэффициент пропорционального звена ПИД-регулятора тока возбуждения.
 */
#define PARAM_ID_EXC_PID_K_P 73
/**
 * Коэффициент интегрального звена ПИД-регулятора тока возбуждения.
 */
#define PARAM_ID_EXC_PID_K_I 74
/**
 * Коэффициент дифференциального звена ПИД-регулятора тока возбуждения.
 */
#define PARAM_ID_EXC_PID_K_D 75
/**
 * Время ожидания остановки ротора при выключении, с.
 */
#define PARAM_ID_ROT_STOP_TIME 80
/**
 * Время ожидания остановки возбуждения при выключении, с.
 */
#define PARAM_ID_EXC_STOP_TIME 81
/**
 * Время ожидания включения возбуждения при выключении, с.
 */
#define PARAM_ID_EXC_START_TIME 82
/**
 * Длительность импульса открытия тиристоров, мкс.
 */
#define PARAM_ID_TRIACS_PAIRS_OPEN_TIME 90
/**
 * Длительность импульса открытия симистора возбуждения, мкс.
 */
#define PARAM_ID_TRIAC_EXC_OPEN_TIME 91

#endif	/* PARAMETERS_IDS_H */

