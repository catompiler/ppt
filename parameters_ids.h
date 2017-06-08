/**
 * @file parameters_ids.h Идентификаторы параметров.
 */

#ifndef PARAMETERS_IDS_H
#define	PARAMETERS_IDS_H

// Идентификаторы.

//////////////////////
// Общие параметры. //
//////////////////////

/*
 * Питание - напряжения и токи.
 */
/**
 * Номинальное фазное напряжение, В.
 */
#define PARAM_ID_U_NOM 100
/**
 * Номинальный фазный ток, А.
 */
#define PARAM_ID_I_NOM 101
/**
 * Номинальное напряжение якоря двигателя, В.
 */
#define PARAM_ID_U_ROT_NOM 110
/**
 * Номинальный ток якоря двигателя, А.
 */
#define PARAM_ID_I_ROT_NOM 111
/**
 * Ток возбуждения двигателя, А.
 */
#define PARAM_ID_I_EXC 120


///////////////////////
// Параметры работы. //
///////////////////////

/**
 * Фаза для которой необходимо вычислять ток.
 */
#define PARAM_ID_CALC_PHASE_CURRENT 1100

/**
 * Регулятор.
 */
/**
 * Режим регулятора.
 */
#define PARAM_ID_REGULATOR_MODE 1125

/*
 * Возбуждение.
 */
/**
 * Фаза возбуждения.
 */
#define PARAM_ID_EXC_PHASE 1150
/**
 * Режим возбуждения.
 */
#define PARAM_ID_EXC_MODE 1151

/*
 * Рампа.
 */
/**
 * Время разгона 0-100%.
 */
#define PARAM_ID_RAMP_START_TIME 1200
/**
 * Время торможения 100-0%.
 */
#define PARAM_ID_RAMP_STOP_TIME 1201
/**
 * Время быстрого торможения 100-0%.
 */
#define PARAM_ID_RAMP_FAST_STOP_TIME 1202
/**
 * Время изменения задания 0-100%.
 */
#define PARAM_ID_RAMP_REFERENCE_TIME 1203
/**
 * Режим останова.
 */
#define PARAM_ID_RAMP_STOP_MODE 1204

/*
 * Синхронизация с фазами.
 */
/**
 * Точность синхронизации с фазами.
 */
#define PARAM_ID_PHASE_SYNC_ACCURACY 1250
/**
 * Коэффициент пропорционального звена ПИД-регулятора ФАПЧ синхронизации с фазами.
 */
#define PARAM_ID_PHASE_SYNC_PLL_PID_K_P 1251
/**
 * Коэффициент интегрального звена ПИД-регулятора ФАПЧ синхронизации с фазами.
 */
#define PARAM_ID_PHASE_SYNC_PLL_PID_K_I 1252
/**
 * Коэффициент дифференциального звена ПИД-регулятора ФАПЧ синхронизации с фазами.
 */
#define PARAM_ID_PHASE_SYNC_PLL_PID_K_D 1253

/*
 * ПИД якоря и возбуждения.
 */
/**
 * Коэффициент пропорционального звена ПИД-регулятора скорости.
 */
#define PARAM_ID_SPD_PID_K_P 1260
/**
 * Коэффициент интегрального звена ПИД-регулятора скорости.
 */
#define PARAM_ID_SPD_PID_K_I 1261
/**
 * Коэффициент дифференциального звена ПИД-регулятора скорости.
 */
#define PARAM_ID_SPD_PID_K_D 1262
/**
 * Коэффициент пропорционального звена ПИД-регулятора тока якоря.
 */
#define PARAM_ID_ROT_PID_K_P 1270
/**
 * Коэффициент интегрального звена ПИД-регулятора тока якоря.
 */
#define PARAM_ID_ROT_PID_K_I 1271
/**
 * Коэффициент дифференциального звена ПИД-регулятора тока якоря.
 */
#define PARAM_ID_ROT_PID_K_D 1272
/**
 * Коэффициент пропорционального звена ПИД-регулятора тока возбуждения.
 */
#define PARAM_ID_EXC_PID_K_P 1280
/**
 * Коэффициент интегрального звена ПИД-регулятора тока возбуждения.
 */
#define PARAM_ID_EXC_PID_K_I 1281
/**
 * Коэффициент дифференциального звена ПИД-регулятора тока возбуждения.
 */
#define PARAM_ID_EXC_PID_K_D 1282

/*
 * Время ожидания запуска и останова.
 */
/**
 * Время ожидания остановки ротора при выключении, с.
 */
#define PARAM_ID_ROT_STOP_TIME 1300
/**
 * Время ожидания остановки возбуждения при выключении, с.
 */
#define PARAM_ID_EXC_STOP_TIME 1301
/**
 * Время ожидания включения возбуждения при выключении, с.
 */
#define PARAM_ID_EXC_START_TIME 1302
/**
 * Время проверки фаз сети, мс.
 */
#define PARAM_ID_PHASES_CHECK_TIME 1305
/**
 * Время срабатывания датчика нуля, мкс.
 */
#define PARAM_ID_ZERO_SENSOR_TIME 1310

/*
 * Тиристоры - время открытия.
 */
/**
 * Длительность импульса открытия тиристоров, мкс.
 */
#define PARAM_ID_TRIACS_PAIRS_OPEN_TIME 1350
/**
 * Длительность импульса открытия симистора возбуждения, мкс.
 */
#define PARAM_ID_TRIAC_EXC_OPEN_TIME 1351

/*
 * Тиристоры - углы открытия.
 */
/**
 * Минимальный угол открытия тиристорных пар.
 */
#define PARAM_ID_TRIACS_PAIRS_ANGLE_MIN 1400
/**
 * Максимальный угол открытия тиристорных пар.
 */
#define PARAM_ID_TRIACS_PAIRS_ANGLE_MAX 1401
/**
 * Минимальный угол открытия симистора возбуждения.
 */
#define PARAM_ID_TRIAC_EXC_ANGLE_MIN 1410
/**
 * Максимальный угол открытия симистора возбуждения.
 */
#define PARAM_ID_TRIAC_EXC_ANGLE_MAX 1411

/*
 * Управление вентиляторами.
 */
/**
 * Разрешение управления вентиляторами.
 */
#define PARAM_ID_FAN_CONTROL_ENABLE 1500
/**
 * Температура включения вентилятора.
 */
#define PARAM_ID_FAN_TEMP_MIN 1501
/**
 * Температура включения вентилятора на максимум.
 */
#define PARAM_ID_FAN_TEMP_MAX 1502
/**
 * Начальные обороты вентилятора, %.
 */
#define PARAM_ID_FAN_RPM_MIN 1503
/**
 * Разрешение эко-режима.
 */
#define PARAM_ID_FAN_ECO_MODE_ENABLE 1504
/**
 * Время охлаждения в эко-режиме.
 */
#define PARAM_ID_FAN_ECO_COOLING_TIME 1505
/*
 * Номинальный ток вентилятора, А.
 */
#define PARAM_ID_FAN_I_NOM 1510
/*
 * Шум нуля тока вентилятора, А.
 */
#define PARAM_ID_FAN_I_ZERO_NOISE 1511
/**
 * Время срабатывания защиты вентилятора, мс.
 */
#define PARAM_ID_FAN_PROT_TIME 1515
/**
 * Значение уровеня предупреждения превышения тока вентилятора, %.
 */
#define PARAM_ID_FAN_PROT_OVF_LEVEL 1516


/////////////
// Защита. //
/////////////

/**
 * Грибок.
 */
/**
 * Действие при срабатывании грибка.
 */
#define PARAM_ID_EMERGENCY_STOP_ACTION  2100

/*
 * Тепловая защита.
 */
/**
 * Разрешение тепловой защиты по току.
 */
#define PARAM_ID_THERMAL_OVERLOAD_PROT_ENABLE 2120
/**
 * Время срабатывания тепловой защиты при шестикратном перегрузе по току.
 */
#define PARAM_ID_THERMAL_OVERLOAD_PROT_TIME_6I 2121
/**
 * Действие при срабатывании тепловой защиты по току.
 */
#define PARAM_ID_THERMAL_OVERLOAD_PROT_ACTION 2122


/*
 * Обрыв якоря.
 */
//! Разрешение.
#define PARAM_ID_PROT_ROT_BREAK_ENABLED 2130
//! Значение.
#define PARAM_ID_PROT_ROT_BREAK_VALUE 2131
//! Время отклонения, мс.
#define PARAM_ID_PROT_ROT_BREAK_TIME_MS 2132
//! Разрешение защёлки.
#define PARAM_ID_PROT_ROT_BREAK_LATCH_ENABLE 2133
//! Действие.
#define PARAM_ID_PROT_ROT_BREAK_ACTION 2134

/*
 * Время между фазами.
 */
//! Разрешение.
#define PARAM_ID_PROT_PHASES_STATE_ENABLED 2200
//! Время отклонения, мс.
#define PARAM_ID_PROT_PHASES_STATE_TIME_MS 2201
//! Действие.
#define PARAM_ID_PROT_PHASES_STATE_ACTION 2202
/*
 * Углы между фазами.
 */
/*
 * Ошибки.
 */
//! Разрешение.
#define PARAM_ID_PROT_PHASES_ANGLES_FAULT_ENABLED 2210
//! Значение.
#define PARAM_ID_PROT_PHASES_ANGLES_FAULT_VALUE 2211
//! Время отклонения, мс.
#define PARAM_ID_PROT_PHASES_ANGLES_FAULT_TIME_MS 2212
//! Разрешение защёлки.
#define PARAM_ID_PROT_PHASES_ANGLES_FAULT_LATCH_ENABLE 2213
//! Действие.
#define PARAM_ID_PROT_PHASES_ANGLES_FAULT_ACTION 2214
/*
 * Предупреждения.
 */
//! Разрешение.
#define PARAM_ID_PROT_PHASES_ANGLES_WARN_ENABLED 2220
//! Значение.
#define PARAM_ID_PROT_PHASES_ANGLES_WARN_VALUE 2221
//! Время отклонения, мс.
#define PARAM_ID_PROT_PHASES_ANGLES_WARN_TIME_MS 2222
//! Разрешение защёлки.
#define PARAM_ID_PROT_PHASES_ANGLES_WARN_LATCH_ENABLE 2223
//! Действие.
#define PARAM_ID_PROT_PHASES_ANGLES_WARN_ACTION 2224
/*
 * Синхронизация с фазами.
 */
/*
 * Ошибки.
 */
//! Разрешение.
#define PARAM_ID_PROT_PHASES_SYNC_FAULT_ENABLED 2230
//! Значение.
#define PARAM_ID_PROT_PHASES_SYNC_FAULT_VALUE 2231
//! Время отклонения, мс.
#define PARAM_ID_PROT_PHASES_SYNC_FAULT_TIME_MS 2232
//! Разрешение защёлки.
#define PARAM_ID_PROT_PHASES_SYNC_FAULT_LATCH_ENABLE 2233
//! Действие.
#define PARAM_ID_PROT_PHASES_SYNC_FAULT_ACTION 2234
/*
 * Предупреждения.
 */
//! Разрешение.
#define PARAM_ID_PROT_PHASES_SYNC_WARN_ENABLED 2240
//! Значение.
#define PARAM_ID_PROT_PHASES_SYNC_WARN_VALUE 2241
//! Время отклонения, мс.
#define PARAM_ID_PROT_PHASES_SYNC_WARN_TIME_MS 2242
//! Разрешение защёлки.
#define PARAM_ID_PROT_PHASES_SYNC_WARN_LATCH_ENABLE 2243
//! Действие.
#define PARAM_ID_PROT_PHASES_SYNC_WARN_ACTION 2244

/*
 * Защита от перегрева.
 */
/*
 * Ошибки.
 */
//! Разрешение.
#define PARAM_ID_PROT_HEATSINK_TEMP_FAULT_ENABLED 2250
//! Значение.
#define PARAM_ID_PROT_HEATSINK_TEMP_FAULT_VALUE 2251
//! Действие.
#define PARAM_ID_PROT_HEATSINK_TEMP_FAULT_ACTION 2252
/*
 * Предупреждения.
 */
//! Разрешение.
#define PARAM_ID_PROT_HEATSINK_TEMP_WARN_ENABLED 2260
//! Значение.
#define PARAM_ID_PROT_HEATSINK_TEMP_WARN_VALUE 2261
//! Действие.
#define PARAM_ID_PROT_HEATSINK_TEMP_WARN_ACTION 2262



/////////////////////
// Защита питания. //
/////////////////////

/*
 * Напряжение.
 */
/*
 * Ошибки.
 */
/*
 * Отсечка.
 */
//! Уровень, %.
#define PARAM_ID_PROT_U_IN_CUTOFF_LEVEL_VALUE 3100
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_IN_OVF_FAULT_ENABLE 3110
//! Уровень, %.
#define PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_VALUE 3111
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_TIME_MS 3112
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_IN_OVF_FAULT_LATCH_ENABLE 3113
//! Действие.
#define PARAM_ID_PROT_U_IN_OVF_FAULT_ACTION 3114
/*
 * Понижение.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_IN_UDF_FAULT_ENABLE 3120
//! Уровень, %.
#define PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_VALUE 3121
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_TIME_MS 3122
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_IN_UDF_FAULT_LATCH_ENABLE 3123
//! Действие.
#define PARAM_ID_PROT_U_IN_UDF_FAULT_ACTION 3124
/*
 * Предупреждения.
 */
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_IN_OVF_WARN_ENABLE 3130
//! Уровень, %.
#define PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_VALUE 3131
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_TIME_MS 3132
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_IN_OVF_WARN_LATCH_ENABLE 3133
//! Действие.
#define PARAM_ID_PROT_U_IN_OVF_WARN_ACTION 3134
/*
 * Понижение.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_IN_UDF_WARN_ENABLE 3140
//! Уровень, %.
#define PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_VALUE 3141
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_TIME_MS 3142
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_IN_UDF_WARN_LATCH_ENABLE 3143
//! Действие.
#define PARAM_ID_PROT_U_IN_UDF_WARN_ACTION 3144
/*
 * Ток.
 */
/*
 * Ошибки.
 */
/*
 * Отсечка.
 */
//! Уровень, %.
#define PARAM_ID_PROT_I_IN_CUTOFF_LEVEL_VALUE 3150
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_IN_OVF_FAULT_ENABLE 3160
//! Уровень, %.
#define PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_VALUE 3161
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_TIME_MS 3162
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_IN_OVF_FAULT_LATCH_ENABLE 3163
//! Действие.
#define PARAM_ID_PROT_I_IN_OVF_FAULT_ACTION 3164
/*
 * Предупреждения.
 */
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_IN_OVF_WARN_ENABLE 3170
//! Уровень, %.
#define PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_VALUE 3171
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_TIME_MS 3172
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_IN_OVF_WARN_LATCH_ENABLE 3173
//! Действие.
#define PARAM_ID_PROT_I_IN_OVF_WARN_ACTION 3174
/*
 * Якорь.
 */
/*
 * Напряжение.
 */
/*
 * Ошибки.
 */
/*
 * Отсечка.
 */
//! Уровень, %.
#define PARAM_ID_PROT_U_ROT_CUTOFF_LEVEL_VALUE 3200
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_ROT_OVF_FAULT_ENABLE 3210
//! Уровень, %.
#define PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_VALUE 3211
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_TIME_MS 3212
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_ROT_OVF_FAULT_LATCH_ENABLE 3213
//! Действие.
#define PARAM_ID_PROT_U_ROT_OVF_FAULT_ACTION 3214
/*
 * Предупреждения.
 */
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_ROT_OVF_WARN_ENABLE 3220
//! Уровень, %.
#define PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_VALUE 3221
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_TIME_MS 3222
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_ROT_OVF_WARN_LATCH_ENABLE 3223
//! Действие.
#define PARAM_ID_PROT_U_ROT_OVF_WARN_ACTION 3224
/*
 * Ток.
 */
/*
 * Ошибки.
 */
/*
 * Отсечка.
 */
//! Уровень, %.
#define PARAM_ID_PROT_I_ROT_CUTOFF_LEVEL_VALUE 3230
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_ROT_OVF_FAULT_ENABLE 3240
//! Уровень, %.
#define PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_VALUE 3241
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_TIME_MS 3242
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_ROT_OVF_FAULT_LATCH_ENABLE 3243
//! Действие.
#define PARAM_ID_PROT_I_ROT_OVF_FAULT_ACTION 3244
/*
 * Предупреждения.
 */
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_ROT_OVF_WARN_ENABLE 3250
//! Уровень, %.
#define PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_VALUE 3251
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_TIME_MS 3252
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_ROT_OVF_WARN_LATCH_ENABLE 3253
//! Действие.
#define PARAM_ID_PROT_I_ROT_OVF_WARN_ACTION 3254
/*
 * Возбуждение.
 */
/*
 * Ошибки.
 */
/*
 * Отсечка.
 */
//! Уровень, %.
#define PARAM_ID_PROT_I_EXC_CUTOFF_LEVEL_VALUE 3300
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_EXC_OVF_FAULT_ENABLE 3310
//! Уровень, %.
#define PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_VALUE 3311
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_TIME_MS 3312
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_EXC_OVF_FAULT_LATCH_ENABLE 3313
//! Действие.
#define PARAM_ID_PROT_I_EXC_OVF_FAULT_ACTION 3314
/*
 * Понижение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_EXC_UDF_FAULT_ENABLE 3320
//! Уровень, %.
#define PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_VALUE 3321
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_TIME_MS 3322
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_EXC_UDF_FAULT_LATCH_ENABLE 3323
//! Действие.
#define PARAM_ID_PROT_I_EXC_UDF_FAULT_ACTION 3324
/*
 * Предупреждения.
 */
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_EXC_OVF_WARN_ENABLE 3330
//! Уровень, %.
#define PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_VALUE 3331
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_TIME_MS 3332
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_EXC_OVF_WARN_LATCH_ENABLE 3333
//! Действие.
#define PARAM_ID_PROT_I_EXC_OVF_WARN_ACTION 3334
/*
 * Понижение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_EXC_UDF_WARN_ENABLE 3340
//! Уровень, %.
#define PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_VALUE 3341
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_TIME_MS 3342
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_EXC_UDF_WARN_LATCH_ENABLE 3343
//! Действие.
#define PARAM_ID_PROT_I_EXC_UDF_WARN_ACTION 3344


///////////////////////////////////////////////////////
// Отклонения нулей (допустимые значения в простое). //
///////////////////////////////////////////////////////
/*
 * Ток фаз.
 */
/*
 * Ошибки.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_IN_IDLE_FAULT_ENABLE 4100
//! Уровень, А.
#define PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_VALUE 4101
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_TIME_MS 4102
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_IN_IDLE_FAULT_LATCH_ENABLE 4103
//! Действие.
#define PARAM_ID_PROT_I_IN_IDLE_FAULT_ACTION 4104
/*
 * Предупреждения.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_IN_IDLE_WARN_ENABLE 4110
//! Уровень, А.
#define PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_VALUE 4111
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_TIME_MS 4112
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_IN_IDLE_WARN_LATCH_ENABLE 4113
//! Действие.
#define PARAM_ID_PROT_I_IN_IDLE_WARN_ACTION 4114
/*
 * Напряжение якоря.
 */
/*
 * Ошибки.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_ROT_IDLE_FAULT_ENABLE 4120
//! Уровень, В.
#define PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_VALUE 4121
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_TIME_MS 4122
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_ROT_IDLE_FAULT_LATCH_ENABLE 4123
//! Действие.
#define PARAM_ID_PROT_U_ROT_IDLE_FAULT_ACTION 4124
/*
 * Предупреждения.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_ROT_IDLE_WARN_ENABLE 4130
//! Уровень, В.
#define PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_VALUE 4131
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_TIME_MS 4132
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_ROT_IDLE_WARN_LATCH_ENABLE 4133
//! Действие.
#define PARAM_ID_PROT_U_ROT_IDLE_WARN_ACTION 4134
/*
 * Ток якоря.
 */
/*
 * Ошибки.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_ROT_IDLE_FAULT_ENABLE 4140
//! Уровень, А.
#define PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_VALUE 4141
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_TIME_MS 4142
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_ROT_IDLE_FAULT_LATCH_ENABLE 4143
//! Действие.
#define PARAM_ID_PROT_I_ROT_IDLE_FAULT_ACTION 4144
/*
 * Предупреждения.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_ROT_IDLE_WARN_ENABLE 4150
//! Уровень, А.
#define PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_VALUE 4151
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_TIME_MS 4152
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_ROT_IDLE_WARN_LATCH_ENABLE 4153
//! Действие.
#define PARAM_ID_PROT_I_ROT_IDLE_WARN_ACTION 4154
/*
 * Ток возбуждения.
 */
/*
 * Ошибки.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_EXC_IDLE_FAULT_ENABLE 4160
//! Уровень, А.
#define PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_VALUE 4161
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_TIME_MS 4162
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_EXC_IDLE_FAULT_LATCH_ENABLE 4163
//! Действие.
#define PARAM_ID_PROT_I_EXC_IDLE_FAULT_ACTION 4164
/*
 * Предупреждения.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_EXC_IDLE_WARN_ENABLE 4170
//! Уровень, А.
#define PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_VALUE 4171
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_TIME_MS 4172
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_EXC_IDLE_WARN_LATCH_ENABLE 4173
//! Действие.
#define PARAM_ID_PROT_I_EXC_IDLE_WARN_ACTION 4174


/////////////////
// Интерфейсы. //
/////////////////

/*
 * Modbus.
 */
/**
 * Скорость соединения Modbus RTU.
 */
#define PARAM_ID_MODBUS_BAUD 5100
/**
 * Адрес устройства Modbus RTU.
 */
#define PARAM_ID_MODBUS_ADDRESS 5101

/**
 * Параметры цифровых входов/выходов.
 */
/**
 * Время игнорирования изменения цифровых входов.
 */
#define PARAM_ID_DIGITAL_IO_DEADTIME_MS 5200
/*
 * Цифровые входа.
 */
/**
 * Тип цифрового входа 1.
 */
#define PARAM_ID_DIGITAL_IN_1_TYPE 5210
/**
 * Инверсия цифрового входа 1.
 */
#define PARAM_ID_DIGITAL_IN_1_INVERSION 5211
/**
 * Тип цифрового входа 2.
 */
#define PARAM_ID_DIGITAL_IN_2_TYPE 5212
/**
 * Инверсия цифрового входа 2.
 */
#define PARAM_ID_DIGITAL_IN_2_INVERSION 5213
/**
 * Тип цифрового входа 3.
 */
#define PARAM_ID_DIGITAL_IN_3_TYPE 5214
/**
 * Инверсия цифрового входа 3.
 */
#define PARAM_ID_DIGITAL_IN_3_INVERSION 5215
/**
 * Тип цифрового входа 4.
 */
#define PARAM_ID_DIGITAL_IN_4_TYPE 5216
/**
 * Инверсия цифрового входа 4.
 */
#define PARAM_ID_DIGITAL_IN_4_INVERSION 5217
/**
 * Тип цифрового входа 5.
 */
#define PARAM_ID_DIGITAL_IN_5_TYPE 5218
/**
 * Инверсия цифрового входа 5.
 */
#define PARAM_ID_DIGITAL_IN_5_INVERSION 5219

/*
 * Цифровые выхода.
 */
/**
 * Тип цифрового выхода 1.
 */
#define PARAM_ID_DIGITAL_OUT_1_TYPE 5240
/**
 * Инверсия цифрового выхода 1.
 */
#define PARAM_ID_DIGITAL_OUT_1_INVERSION 5241
/**
 * Тип цифрового выхода 2.
 */
#define PARAM_ID_DIGITAL_OUT_2_TYPE 5242
/**
 * Инверсия цифрового выхода 2.
 */
#define PARAM_ID_DIGITAL_OUT_2_INVERSION 5243
/**
 * Тип цифрового выхода 3.
 */
#define PARAM_ID_DIGITAL_OUT_3_TYPE 5244
/**
 * Инверсия цифрового выхода 3.
 */
#define PARAM_ID_DIGITAL_OUT_3_INVERSION 5245
/**
 * Тип цифрового выхода 4.
 */
#define PARAM_ID_DIGITAL_OUT_4_TYPE 5246
/**
 * Инверсия цифрового выхода 4.
 */
#define PARAM_ID_DIGITAL_OUT_4_INVERSION 5247


////////////////////////////////////////
// Настройки интерфейса панели (GUI). //
////////////////////////////////////////

/**
 * Язык интерфейса
 */
#define PARAM_ID_GUI_LANGUAGE 6050
/**
 * Плитки (выбор отображаемых значений)
 */
#define PARAM_ID_GUI_TILE_1 6101
#define PARAM_ID_GUI_TILE_2 6102
#define PARAM_ID_GUI_TILE_3 6103
#define PARAM_ID_GUI_TILE_4 6104
/**
 * Работа зуммера (звукового оповещения)
 */
#define PARAM_ID_GUI_BUZZER 6151
/**
 * Пароль администратора
 */
#define PARAM_ID_GUI_PASSWORD_ADMIN 6200
/**
 * Пароль рута
 */
#define PARAM_ID_GUI_PASSWORD_ROOT 6201

/*
 * GUI: диапазоны индикации WARN и ALARM статусов значений, отображаемых на плитках.
 */
/**
 * Напряжение фазы.
 */
#define PARAM_ID_POWER_U_ALARM_MIN    6300
#define PARAM_ID_POWER_U_WARN_MIN     6301
#define PARAM_ID_POWER_U_WARN_MAX     6302
#define PARAM_ID_POWER_U_ALARM_MAX    6303
/**
 * Напряжение якоря.
 */
#define PARAM_ID_POWER_U_ROT_ALARM_MIN    6310
#define PARAM_ID_POWER_U_ROT_WARN_MIN     6311
#define PARAM_ID_POWER_U_ROT_WARN_MAX     6312
#define PARAM_ID_POWER_U_ROT_ALARM_MAX    6313
/**
 * Ток фазы.
 */
#define PARAM_ID_POWER_I_ALARM_MIN    6320
#define PARAM_ID_POWER_I_WARN_MIN     6321
#define PARAM_ID_POWER_I_WARN_MAX     6322
#define PARAM_ID_POWER_I_ALARM_MAX    6323
/**
 * Ток якоря.
 */
#define PARAM_ID_POWER_I_ROT_ALARM_MIN    6330
#define PARAM_ID_POWER_I_ROT_WARN_MIN     6331
#define PARAM_ID_POWER_I_ROT_WARN_MAX     6332
#define PARAM_ID_POWER_I_ROT_ALARM_MAX    6333
/**
 * Ток возбуждения.
 */
#define PARAM_ID_POWER_I_EXC_ALARM_MIN    6340
#define PARAM_ID_POWER_I_EXC_WARN_MIN     6341
#define PARAM_ID_POWER_I_EXC_WARN_MAX     6342
#define PARAM_ID_POWER_I_EXC_ALARM_MAX    6343
/**
 * Температура радиатора.
 */
#define PARAM_ID_HEATSINK_TEMP_ALARM_MIN    6350
#define PARAM_ID_HEATSINK_TEMP_WARN_MIN     6351
#define PARAM_ID_HEATSINK_TEMP_WARN_MAX     6352
#define PARAM_ID_HEATSINK_TEMP_ALARM_MAX    6353
/**
 * Ток вентилятора.
 */
#define PARAM_ID_POWER_I_FAN_ALARM_MIN    6360
#define PARAM_ID_POWER_I_FAN_WARN_MIN    6361
#define PARAM_ID_POWER_I_FAN_WARN_MAX    6362
#define PARAM_ID_POWER_I_FAN_ALARM_MAX    6363
/**
 * Ток 4-20.
 */      
#define PARAM_ID_POWER_I_REF_ALARM_MIN    6370
#define PARAM_ID_POWER_I_REF_WARN_MIN    6371
#define PARAM_ID_POWER_I_REF_WARN_MAX    6372
#define PARAM_ID_POWER_I_REF_ALARM_MAX    6373
/**
 * Обороты вентилятора радиатора.
 */
#define PARAM_ID_HEATSINK_FAN_RPM_ALARM_MIN    6380
#define PARAM_ID_HEATSINK_FAN_RPM_WARN_MIN    6381
#define PARAM_ID_HEATSINK_FAN_RPM_WARN_MAX    6382
#define PARAM_ID_HEATSINK_FAN_RPM_ALARM_MAX    6383
/**
 * Угол открытия тиристоров.
 */        
#define PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MIN    6390
#define PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_WARN_MIN    6391
#define PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_WARN_MAX    6392
#define PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MAX    6393
/**
 * Угол открытия возбуждения.
 */          
#define PARAM_ID_TRIAC_EXC_OPEN_ANGLE_ALARM_MIN    6400
#define PARAM_ID_TRIAC_EXC_OPEN_ANGLE_WARN_MIN    6401
#define PARAM_ID_TRIAC_EXC_OPEN_ANGLE_WARN_MAX    6402
#define PARAM_ID_TRIAC_EXC_OPEN_ANGLE_ALARM_MAX    6403

//////////
// АЦП. //
//////////
/*
 * Коэффициенты АЦП.
 */
/**
 * Множитель данных канала АЦП Ua.
 */
#define PARAM_ID_ADC_VALUE_MULTIPLIER_Ua 7100
/**
 * Множитель данных канала АЦП Ub.
 */
#define PARAM_ID_ADC_VALUE_MULTIPLIER_Ub 7101
/**
 * Множитель данных канала АЦП Uc.
 */
#define PARAM_ID_ADC_VALUE_MULTIPLIER_Uc 7102
/**
 * Множитель данных канала АЦП Urot.
 */
#define PARAM_ID_ADC_VALUE_MULTIPLIER_Urot 7103
/**
 * Множитель данных канала АЦП Ia.
 */
#define PARAM_ID_ADC_VALUE_MULTIPLIER_Ia 7104
/**
 * Множитель данных канала АЦП Ib.
 */
#define PARAM_ID_ADC_VALUE_MULTIPLIER_Ib 7105
/**
 * Множитель данных канала АЦП Ic.
 */
#define PARAM_ID_ADC_VALUE_MULTIPLIER_Ic 7106
/**
 * Множитель данных канала АЦП Irot.
 */
#define PARAM_ID_ADC_VALUE_MULTIPLIER_Irot 7107
/**
 * Множитель данных канала АЦП Iexc.
 */
#define PARAM_ID_ADC_VALUE_MULTIPLIER_Iexc 7108
/**
 * Множитель данных канала АЦП Iref.
 */
#define PARAM_ID_ADC_VALUE_MULTIPLIER_Iref 7109
/**
 * Множитель данных канала АЦП Ifan.
 */
#define PARAM_ID_ADC_VALUE_MULTIPLIER_Ifan 7110

/*
 * Данные калибровки нуля АЦП.
 */
/**
 * Данные калибровки канала АЦП Ua.
 */
#define PARAM_ID_ADC_CALIBRATION_DATA_Ua 7120
/**
 * Данные калибровки канала АЦП Ub.
 */
#define PARAM_ID_ADC_CALIBRATION_DATA_Ub 7121
/**
 * Данные калибровки канала АЦП Uc.
 */
#define PARAM_ID_ADC_CALIBRATION_DATA_Uc 7122
/**
 * Данные калибровки канала АЦП Urot.
 */
#define PARAM_ID_ADC_CALIBRATION_DATA_Urot 7123
/**
 * Данные калибровки канала АЦП Ia.
 */
#define PARAM_ID_ADC_CALIBRATION_DATA_Ia 7124
/**
 * Данные калибровки канала АЦП Ib.
 */
#define PARAM_ID_ADC_CALIBRATION_DATA_Ib 7125
/**
 * Данные калибровки канала АЦП Ic.
 */
#define PARAM_ID_ADC_CALIBRATION_DATA_Ic 7126
/**
 * Данные калибровки канала АЦП Irot.
 */
#define PARAM_ID_ADC_CALIBRATION_DATA_Irot 7127
/**
 * Данные калибровки канала АЦП Iexc.
 */
#define PARAM_ID_ADC_CALIBRATION_DATA_Iexc 7128
/**
 * Данные калибровки канала АЦП Iref.
 */
#define PARAM_ID_ADC_CALIBRATION_DATA_Iref 7129
/**
 * Данные калибровки канала АЦП Ifan.
 */
#define PARAM_ID_ADC_CALIBRATION_DATA_Ifan 7130
/*
 * Коэффициенты действующих значений.
 */
/**
 * Множитель данных канала Ua.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Ua 7140
/**
 * Множитель данных канала Ub.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Ub 7141
/**
 * Множитель данных канала Uc.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Uc 7142
/**
 * Множитель данных канала Urot.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Urot 7143
/**
 * Множитель данных канала Ia.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Ia 7144
/**
 * Множитель данных канала Ib.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Ib 7145
/**
 * Множитель данных канала Ic.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Ic 7146
/**
 * Множитель данных канала Irot.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Irot 7147
/**
 * Множитель данных канала Iexc.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Iexc 7148
/**
 * Множитель данных канала Iref.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Iref 7149
/**
 * Множитель данных канала Ifan.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Ifan 7150


////////////////////////////
// Виртуальные параметры. //
////////////////////////////

/*
 * Данные питания.
 */
/**
 * Напряжение фазы A.
 */
#define PARAM_ID_POWER_U_A 8100
/**
 * Напряжение фазы B.
 */
#define PARAM_ID_POWER_U_B 8101
/**
 * Напряжение фазы C.
 */
#define PARAM_ID_POWER_U_C 8102
/**
 * Напряжение якоря.
 */
#define PARAM_ID_POWER_U_ROT 8103
/**
 * Ток фазы A.
 */
#define PARAM_ID_POWER_I_A 8104
/**
 * Ток фазы B.
 */
#define PARAM_ID_POWER_I_B 8105
/**
 * Ток фазы C.
 */
#define PARAM_ID_POWER_I_C 8106
/**
 * Ток якоря.
 */
#define PARAM_ID_POWER_I_ROT 8107
/**
 * Ток возбуждения.
 */
#define PARAM_ID_POWER_I_EXC 8108
/**
 * Ток 4-20 мА.
 */
#define PARAM_ID_POWER_I_REF 8109
/**
 * Ток вентилятора.
 */
#define PARAM_ID_POWER_I_FAN 8110

/**
 * Температура радиатора.
 */
#define PARAM_ID_HEATSINK_TEMP 8200

/**
 * Обороты вентилятора охлаждения радиатора.
 */
#define PARAM_ID_HEATSINK_FAN_RPM 8201

/**
 * Угол открытия основных тиристоров.
 */
#define PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE 8300

/**
 * Угол открытия основных симистора возбуждения.
 */
#define PARAM_ID_TRIAC_EXC_OPEN_ANGLE 8301

/*
 * Данные состояния цифровых входов.
 */
/**
 * Состояние цифрового входа 1.
 */
#define PARAM_ID_DIGITAL_IN_1_STATE     8351
/**
 * Состояние цифрового входа 2.
 */
#define PARAM_ID_DIGITAL_IN_2_STATE     8352
/**
 * Состояние цифрового входа 3.
 */
#define PARAM_ID_DIGITAL_IN_3_STATE     8353
/**
 * Состояние цифрового входа 4.
 */
#define PARAM_ID_DIGITAL_IN_4_STATE     8354
/**
 * Состояние цифрового входа 5.
 */
#define PARAM_ID_DIGITAL_IN_5_STATE     8355

/*
 * Данные состояния цифровых выходов.
 */
/**
 * Состояние цифрового выхода 1.
 */
#define PARAM_ID_DIGITAL_OUT_1_STATE     8361
/**
 * Состояние цифрового выхода 2.
 */
#define PARAM_ID_DIGITAL_OUT_2_STATE     8362
/**
 * Состояние цифрового выхода 3.
 */
#define PARAM_ID_DIGITAL_OUT_3_STATE     8363
/**
 * Состояние цифрового выхода 4.
 */
#define PARAM_ID_DIGITAL_OUT_4_STATE     8364

/*
 * Наработка.
 */
/**
 * Общее время включения.
 */
#define PARAM_ID_LIFETIME        8400
/**
 * Общее время работы.
 */
#define PARAM_ID_RUNTIME         8401
/**
 * Общее время работы вентилятора.
 */
#define PARAM_ID_FAN_RUNTIME     8402
/**
 * Время работы после включения.
 */
#define PARAM_ID_LAST_RUNTIME    8403

///////////////////////////
// Отладочные параметры. //
///////////////////////////

/**
 * Отладочный параметр 0
 */
#define PARAM_ID_DEBUG_0 9100
/**
 * Отладочный параметр 1
 */
#define PARAM_ID_DEBUG_1 9101
/**
 * Отладочный параметр 2
 */
#define PARAM_ID_DEBUG_2 9102
/**
 * Отладочный параметр 3
 */
#define PARAM_ID_DEBUG_3 9103
/**
 * Отладочный параметр 4
 */
#define PARAM_ID_DEBUG_4 9104
/**
 * Отладочный параметр 5
 */
#define PARAM_ID_DEBUG_5 9105
/**
 * Отладочный параметр 6
 */
#define PARAM_ID_DEBUG_6 9106
/**
 * Отладочный параметр 7
 */
#define PARAM_ID_DEBUG_7 9107
/**
 * Отладочный параметр 8
 */
#define PARAM_ID_DEBUG_8 9108
/**
 * Отладочный параметр 9
 */
#define PARAM_ID_DEBUG_9 9109

#endif	/* PARAMETERS_IDS_H */

