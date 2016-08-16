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
 * Допустимый шим нуля по току ротора, А.
 */
#define PARAM_ID_I_ROT_ZERO_NOISE 15
/**
 * Допустимый шим нуля по току возбуждения, А.
 */
#define PARAM_ID_I_EXC_ZERO_NOISE 16
/**
 * Номинальное напряжение якоря двигателя, В.
 */
#define PARAM_ID_U_ROT_NOM 20
/**
 * Разрешённое отклонение напряжения якоря, %.
 */
#define PARAM_ID_U_ROT_ALLOW_VAR 21
/**
 * Критическое отклонение напряжения якоря, %.
 */
#define PARAM_ID_U_ROT_CRIT_VAR 22
/**
 * Номинальный ток якоря двигателя, А.
 */
#define PARAM_ID_I_ROT_NOM 23
/**
 * Кратность токовой отсечки тока якоря.
 */
#define PARAM_ID_I_ROT_CUTOFF_MULT 24
/**
 * Разрешённое отклонение от тока якоря, %.
 */
#define PARAM_ID_I_ROT_ALLOW_VAR 25
/**
 * Критическое отклонение от тока якоря, %.
 */
#define PARAM_ID_I_ROT_CRIT_VAR 26
/**
 * Ток возбуждения двигателя, А.
 */
#define PARAM_ID_I_EXC 27
/**
 * Разрешённое отклонение от тока возбуждения, %.
 */
#define PARAM_ID_I_EXC_ALLOW_VAR 28
/**
 * Критическое отклонение от тока возбуждения, %.
 */
#define PARAM_ID_I_EXC_CRIT_VAR 29
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
/**
 * Время срабатывания тепловой защиты при шестикратном перегрузе по току.
 */
#define PARAM_ID_THERMAL_OVERLOAD_PROT_TIME_6I 100
/**
 * Данные калибровки канала АЦП Ua.
 */
#define PARAM_ID_CALIBRATION_DATA_Ua 200
/**
 * Данные калибровки канала АЦП Ub.
 */
#define PARAM_ID_CALIBRATION_DATA_Ub 201
/**
 * Данные калибровки канала АЦП Uc.
 */
#define PARAM_ID_CALIBRATION_DATA_Uc 202
/**
 * Данные калибровки канала АЦП Urot.
 */
#define PARAM_ID_CALIBRATION_DATA_Urot 203
/**
 * Данные калибровки канала АЦП Ia.
 */
#define PARAM_ID_CALIBRATION_DATA_Ia 204
/**
 * Данные калибровки канала АЦП Ib.
 */
#define PARAM_ID_CALIBRATION_DATA_Ib 205
/**
 * Данные калибровки канала АЦП Ic.
 */
#define PARAM_ID_CALIBRATION_DATA_Ic 206
/**
 * Данные калибровки канала АЦП Irot.
 */
#define PARAM_ID_CALIBRATION_DATA_Irot 207
/**
 * Данные калибровки канала АЦП Iexc.
 */
#define PARAM_ID_CALIBRATION_DATA_Iexc 208
/**
 * Данные калибровки канала АЦП Iref.
 */
#define PARAM_ID_CALIBRATION_DATA_Iref 209
/**
 * Данные калибровки канала АЦП Ifan.
 */
#define PARAM_ID_CALIBRATION_DATA_Ifan 210
/**
 * Скорость соединения Modbus RTU.
 */
#define PARAM_ID_MODBUS_BAUD 500
/**
 * Отладочный параметр 0
 */
#define PARAM_ID_DEBUG_0 1000
/**
 * Отладочный параметр 1
 */
#define PARAM_ID_DEBUG_1 1001
/**
 * Отладочный параметр 2
 */
#define PARAM_ID_DEBUG_2 1002
/**
 * Отладочный параметр 3
 */
#define PARAM_ID_DEBUG_3 1003
/**
 * Отладочный параметр 4
 */
#define PARAM_ID_DEBUG_4 1004
/**
 * Отладочный параметр 5
 */
#define PARAM_ID_DEBUG_5 1005
/**
 * Отладочный параметр 6
 */
#define PARAM_ID_DEBUG_6 1006
/**
 * Отладочный параметр 7
 */
#define PARAM_ID_DEBUG_7 1007
/**
 * Отладочный параметр 8
 */
#define PARAM_ID_DEBUG_8 1008
/**
 * Отладочный параметр 9
 */
#define PARAM_ID_DEBUG_9 1009
/**
 * Напряжение фазы A.
 */
#define PARAM_ID_POWER_U_A 1020
/**
 * Напряжение фазы B.
 */
#define PARAM_ID_POWER_U_B 1021
/**
 * Напряжение фазы C.
 */
#define PARAM_ID_POWER_U_C 1022
/**
 * Напряжение якоря.
 */
#define PARAM_ID_POWER_U_ROT 1023
/**
 * Ток фазы A.
 */
#define PARAM_ID_POWER_I_A 1024
/**
 * Ток фазы B.
 */
#define PARAM_ID_POWER_I_B 1025
/**
 * Ток фазы C.
 */
#define PARAM_ID_POWER_I_C 1026
/**
 * Ток якоря.
 */
#define PARAM_ID_POWER_I_ROT 1027
/**
 * Ток возбуждения.
 */
#define PARAM_ID_POWER_I_EXC 1028


#endif	/* PARAMETERS_IDS_H */

