/**
 * @file parameters_ids.h Идентификаторы параметров.
 */

#ifndef PARAMETERS_IDS_H
#define	PARAMETERS_IDS_H

// Идентификаторы.

/*
 * Питание - напряжения и токи.
 */
/**
 * Номинальное фазное напряжение, В.
 */
#define PARAM_ID_U_NOM 10
/**
 * Номинальный фазный ток, А.
 */
#define PARAM_ID_I_NOM 11
/**
 * Фаза для которой необходимо вычислять ток.
 */
#define PARAM_ID_CALC_PHASE_CURRENT 12
/**
 * Номинальное напряжение якоря двигателя, В.
 */
#define PARAM_ID_U_ROT_NOM 30
/**
 * Номинальный ток якоря двигателя, А.
 */
#define PARAM_ID_I_ROT_NOM 31
/**
 * Ток возбуждения двигателя, А.
 */
#define PARAM_ID_I_EXC 40

/*
 * Возбуждение.
 */
/**
 * Фаза возбуждения.
 */
#define PARAM_ID_EXC_PHASE 45
/**
 * Режим возбуждения.
 */
#define PARAM_ID_EXC_MODE 46

/*
 * Рампа.
 */
/**
 * Время разгона 0-100%.
 */
#define PARAM_ID_RAMP_START_TIME 50
/**
 * Время торможения 100-0%.
 */
#define PARAM_ID_RAMP_STOP_TIME 51
/**
 * Время быстрого торможения 100-0%.
 */
#define PARAM_ID_RAMP_FAST_STOP_TIME 52
/**
 * Время изменения задания 0-100%.
 */
#define PARAM_ID_RAMP_REFERENCE_TIME 53
/**
 * Режим останова.
 */
#define PARAM_ID_RAMP_STOP_MODE 60

/*
 * Синхронизация с фазами.
 */
/**
 * Точность синхронизации с фазами.
 */
#define PARAM_ID_PHASE_SYNC_ACCURACY 65
/**
 * Коэффициент пропорционального звена ПИД-регулятора ФАПЧ синхронизации с фазами.
 */
#define PARAM_ID_PHASE_SYNC_PLL_PID_K_P 67
/**
 * Коэффициент интегрального звена ПИД-регулятора ФАПЧ синхронизации с фазами.
 */
#define PARAM_ID_PHASE_SYNC_PLL_PID_K_I 68
/**
 * Коэффициент дифференциального звена ПИД-регулятора ФАПЧ синхронизации с фазами.
 */
#define PARAM_ID_PHASE_SYNC_PLL_PID_K_D 69

/*
 * ПИД якоря и возбуждения.
 */
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

/*
 * Время ожидания запуска и останова.
 */
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
 * Время проверки фаз сети, мс.
 */
#define PARAM_ID_PHASES_CHECK_TIME 85
/**
 * Время срабатывания датчика нуля, мкс.
 */
#define PARAM_ID_ZERO_SENSOR_TIME 87

/*
 * Тиристоры - время открытия.
 */
/**
 * Длительность импульса открытия тиристоров, мкс.
 */
#define PARAM_ID_TRIACS_PAIRS_OPEN_TIME 90
/**
 * Длительность импульса открытия симистора возбуждения, мкс.
 */
#define PARAM_ID_TRIAC_EXC_OPEN_TIME 91

/*
 * Защита.
 */
/*
 * Тепловая защита.
 */
/**
 * Разрешение тепловой защиты по току.
 */
#define PARAM_ID_THERMAL_OVERLOAD_PROT_ENABLE 100
/**
 * Время срабатывания тепловой защиты при шестикратном перегрузе по току.
 */
#define PARAM_ID_THERMAL_OVERLOAD_PROT_TIME_6I 101
/**
 * Действие при срабатывании тепловой защиты по току.
 */
#define PARAM_ID_THERMAL_OVERLOAD_PROT_ACTION 102

/**
 * Грибок.
 */
/**
 * Действие при срабатывании грибка.
 */
#define PARAM_ID_EMERGENCY_STOP_ACTION  105

/*
 * Защита питания.
 */
/*
 * Сеть.
 */
/*
 * Чередование фаз.
 */
//! Разрешение.
#define PARAM_ID_PHASES_PROT_ENABLED 180
//! Время отклонения, мс.
#define PARAM_ID_PHASES_PROT_TIME_MS 181
//! Действие.
#define PARAM_ID_PHASES_PROT_ACTION 182
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
#define PARAM_ID_PROT_U_IN_CUTOFF_LEVEL_VALUE 209
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_IN_OVF_FAULT_ENABLE 210
//! Уровень, %.
#define PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_VALUE 211
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_TIME_MS 212
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_IN_OVF_FAULT_LATCH_ENABLE 213
//! Действие.
#define PARAM_ID_PROT_U_IN_OVF_FAULT_ACTION 215
/*
 * Понижение.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_IN_UDF_FAULT_ENABLE 220
//! Уровень, %.
#define PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_VALUE 221
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_TIME_MS 222
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_IN_UDF_FAULT_LATCH_ENABLE 223
//! Действие.
#define PARAM_ID_PROT_U_IN_UDF_FAULT_ACTION 225
/*
 * Предупреждения.
 */
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_IN_OVF_WARN_ENABLE 230
//! Уровень, %.
#define PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_VALUE 231
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_TIME_MS 232
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_IN_OVF_WARN_LATCH_ENABLE 233
//! Действие.
#define PARAM_ID_PROT_U_IN_OVF_WARN_ACTION 235
/*
 * Понижение.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_IN_UDF_WARN_ENABLE 240
//! Уровень, %.
#define PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_VALUE 241
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_TIME_MS 242
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_IN_UDF_WARN_LATCH_ENABLE 243
//! Действие.
#define PARAM_ID_PROT_U_IN_UDF_WARN_ACTION 245
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
#define PARAM_ID_PROT_I_IN_CUTOFF_LEVEL_VALUE 249
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_IN_OVF_FAULT_ENABLE 250
//! Уровень, %.
#define PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_VALUE 251
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_TIME_MS 252
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_IN_OVF_FAULT_LATCH_ENABLE 253
//! Действие.
#define PARAM_ID_PROT_I_IN_OVF_FAULT_ACTION 255
/*
 * Предупреждения.
 */
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_IN_OVF_WARN_ENABLE 260
//! Уровень, %.
#define PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_VALUE 261
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_TIME_MS 262
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_IN_OVF_WARN_LATCH_ENABLE 263
//! Действие.
#define PARAM_ID_PROT_I_IN_OVF_WARN_ACTION 265
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
#define PARAM_ID_PROT_U_ROT_CUTOFF_LEVEL_VALUE 269
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_ROT_OVF_FAULT_ENABLE 270
//! Уровень, %.
#define PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_VALUE 271
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_TIME_MS 272
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_ROT_OVF_FAULT_LATCH_ENABLE 273
//! Действие.
#define PARAM_ID_PROT_U_ROT_OVF_FAULT_ACTION 275
/*
 * Предупреждения.
 */
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_ROT_OVF_WARN_ENABLE 280
//! Уровень, %.
#define PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_VALUE 281
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_TIME_MS 282
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_ROT_OVF_WARN_LATCH_ENABLE 283
//! Действие.
#define PARAM_ID_PROT_U_ROT_OVF_WARN_ACTION 285
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
#define PARAM_ID_PROT_I_ROT_CUTOFF_LEVEL_VALUE 289
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_ROT_OVF_FAULT_ENABLE 290
//! Уровень, %.
#define PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_VALUE 291
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_TIME_MS 292
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_ROT_OVF_FAULT_LATCH_ENABLE 293
//! Действие.
#define PARAM_ID_PROT_I_ROT_OVF_FAULT_ACTION 295
/*
 * Предупреждения.
 */
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_ROT_OVF_WARN_ENABLE 300
//! Уровень, %.
#define PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_VALUE 301
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_TIME_MS 302
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_ROT_OVF_WARN_LATCH_ENABLE 303
//! Действие.
#define PARAM_ID_PROT_I_ROT_OVF_WARN_ACTION 305
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
#define PARAM_ID_PROT_I_EXC_CUTOFF_LEVEL_VALUE 309
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_EXC_OVF_FAULT_ENABLE 310
//! Уровень, %.
#define PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_VALUE 311
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_TIME_MS 312
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_EXC_OVF_FAULT_LATCH_ENABLE 313
//! Действие.
#define PARAM_ID_PROT_I_EXC_OVF_FAULT_ACTION 315
/*
 * Понижение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_EXC_UDF_FAULT_ENABLE 320
//! Уровень, %.
#define PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_VALUE 321
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_TIME_MS 322
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_EXC_UDF_FAULT_LATCH_ENABLE 323
//! Действие.
#define PARAM_ID_PROT_I_EXC_UDF_FAULT_ACTION 325
/*
 * Предупреждения.
 */
/*
 * Превышение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_EXC_OVF_WARN_ENABLE 330
//! Уровень, %.
#define PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_VALUE 331
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_TIME_MS 332
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_EXC_OVF_WARN_LATCH_ENABLE 333
//! Действие.
#define PARAM_ID_PROT_I_EXC_OVF_WARN_ACTION 335
/*
 * Понижение.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_EXC_UDF_WARN_ENABLE 340
//! Уровень, %.
#define PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_VALUE 341
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_TIME_MS 342
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_EXC_UDF_WARN_LATCH_ENABLE 343
//! Действие.
#define PARAM_ID_PROT_I_EXC_UDF_WARN_ACTION 345
/*
 * Отклонения нулей (допустимые значения в простое).
 */
/*
 * Ток фаз.
 */
/*
 * Ошибки.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_IN_IDLE_FAULT_ENABLE 350
//! Уровень, А.
#define PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_VALUE 351
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_TIME_MS 352
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_IN_IDLE_FAULT_LATCH_ENABLE 353
//! Действие.
#define PARAM_ID_PROT_I_IN_IDLE_FAULT_ACTION 354
/*
 * Предупреждения.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_IN_IDLE_WARN_ENABLE 360
//! Уровень, А.
#define PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_VALUE 361
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_TIME_MS 362
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_IN_IDLE_WARN_LATCH_ENABLE 363
//! Действие.
#define PARAM_ID_PROT_I_IN_IDLE_WARN_ACTION 364
/*
 * Напряжение якоря.
 */
/*
 * Ошибки.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_ROT_IDLE_FAULT_ENABLE 370
//! Уровень, В.
#define PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_VALUE 371
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_TIME_MS 372
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_ROT_IDLE_FAULT_LATCH_ENABLE 373
//! Действие.
#define PARAM_ID_PROT_U_ROT_IDLE_FAULT_ACTION 374
/*
 * Предупреждения.
 */
//! Разрешение.
#define PARAM_ID_PROT_U_ROT_IDLE_WARN_ENABLE 380
//! Уровень, В.
#define PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_VALUE 381
//! Время отклонения, мс.
#define PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_TIME_MS 382
//! Разрешение защёлки.
#define PARAM_ID_PROT_U_ROT_IDLE_WARN_LATCH_ENABLE 383
//! Действие.
#define PARAM_ID_PROT_U_ROT_IDLE_WARN_ACTION 384
/*
 * Ток якоря.
 */
/*
 * Ошибки.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_ROT_IDLE_FAULT_ENABLE 390
//! Уровень, А.
#define PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_VALUE 391
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_TIME_MS 392
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_ROT_IDLE_FAULT_LATCH_ENABLE 393
//! Действие.
#define PARAM_ID_PROT_I_ROT_IDLE_FAULT_ACTION 394
/*
 * Предупреждения.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_ROT_IDLE_WARN_ENABLE 400
//! Уровень, А.
#define PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_VALUE 401
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_TIME_MS 402
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_ROT_IDLE_WARN_LATCH_ENABLE 403
//! Действие.
#define PARAM_ID_PROT_I_ROT_IDLE_WARN_ACTION 404
/*
 * Ток возбуждения.
 */
/*
 * Ошибки.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_EXC_IDLE_FAULT_ENABLE 410
//! Уровень, А.
#define PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_VALUE 411
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_TIME_MS 412
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_EXC_IDLE_FAULT_LATCH_ENABLE 413
//! Действие.
#define PARAM_ID_PROT_I_EXC_IDLE_FAULT_ACTION 414
/*
 * Предупреждения.
 */
//! Разрешение.
#define PARAM_ID_PROT_I_EXC_IDLE_WARN_ENABLE 420
//! Уровень, А.
#define PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_VALUE 421
//! Время отклонения, мс.
#define PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_TIME_MS 422
//! Разрешение защёлки.
#define PARAM_ID_PROT_I_EXC_IDLE_WARN_LATCH_ENABLE 423
//! Действие.
#define PARAM_ID_PROT_I_EXC_IDLE_WARN_ACTION 424

/*
 * АЦП.
 */
/*
 * Коэффициенты.
 */
/**
 * Множитель данных канала АЦП Ua.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Ua 500
/**
 * Множитель данных канала АЦП Ub.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Ub 501
/**
 * Множитель данных канала АЦП Uc.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Uc 502
/**
 * Множитель данных канала АЦП Urot.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Urot 503
/**
 * Множитель данных канала АЦП Ia.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Ia 504
/**
 * Множитель данных канала АЦП Ib.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Ib 505
/**
 * Множитель данных канала АЦП Ic.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Ic 506
/**
 * Множитель данных канала АЦП Irot.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Irot 507
/**
 * Множитель данных канала АЦП Iexc.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Iexc 508
/**
 * Множитель данных канала АЦП Iref.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Iref 509
/**
 * Множитель данных канала АЦП Ifan.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Ifan 510

/*
 * Данные калибровки нуля.
 */
/**
 * Данные калибровки канала АЦП Ua.
 */
#define PARAM_ID_CALIBRATION_DATA_Ua 520
/**
 * Данные калибровки канала АЦП Ub.
 */
#define PARAM_ID_CALIBRATION_DATA_Ub 521
/**
 * Данные калибровки канала АЦП Uc.
 */
#define PARAM_ID_CALIBRATION_DATA_Uc 522
/**
 * Данные калибровки канала АЦП Urot.
 */
#define PARAM_ID_CALIBRATION_DATA_Urot 523
/**
 * Данные калибровки канала АЦП Ia.
 */
#define PARAM_ID_CALIBRATION_DATA_Ia 524
/**
 * Данные калибровки канала АЦП Ib.
 */
#define PARAM_ID_CALIBRATION_DATA_Ib 525
/**
 * Данные калибровки канала АЦП Ic.
 */
#define PARAM_ID_CALIBRATION_DATA_Ic 526
/**
 * Данные калибровки канала АЦП Irot.
 */
#define PARAM_ID_CALIBRATION_DATA_Irot 527
/**
 * Данные калибровки канала АЦП Iexc.
 */
#define PARAM_ID_CALIBRATION_DATA_Iexc 528
/**
 * Данные калибровки канала АЦП Iref.
 */
#define PARAM_ID_CALIBRATION_DATA_Iref 529
/**
 * Данные калибровки канала АЦП Ifan.
 */
#define PARAM_ID_CALIBRATION_DATA_Ifan 530

/*
 * Цифровые входа и выхода.
 */
/*
 * Цифровые входа.
 */
/**
 * Тип цифрового входа 1.
 */
#define PARAM_ID_DIGITAL_IN_1_TYPE 600
/**
 * Инверсия цифрового входа 1.
 */
#define PARAM_ID_DIGITAL_IN_1_INVERSION 601
/**
 * Тип цифрового входа 2.
 */
#define PARAM_ID_DIGITAL_IN_2_TYPE 602
/**
 * Инверсия цифрового входа 2.
 */
#define PARAM_ID_DIGITAL_IN_2_INVERSION 603
/**
 * Тип цифрового входа 3.
 */
#define PARAM_ID_DIGITAL_IN_3_TYPE 604
/**
 * Инверсия цифрового входа 3.
 */
#define PARAM_ID_DIGITAL_IN_3_INVERSION 605
/**
 * Тип цифрового входа 4.
 */
#define PARAM_ID_DIGITAL_IN_4_TYPE 606
/**
 * Инверсия цифрового входа 4.
 */
#define PARAM_ID_DIGITAL_IN_4_INVERSION 607
/**
 * Тип цифрового входа 5.
 */
#define PARAM_ID_DIGITAL_IN_5_TYPE 608
/**
 * Инверсия цифрового входа 5.
 */
#define PARAM_ID_DIGITAL_IN_5_INVERSION 609

/*
 * Цифровые выхода.
 */
/**
 * Тип цифрового выхода 1.
 */
#define PARAM_ID_DIGITAL_OUT_1_TYPE 620
/**
 * Инверсия цифрового выхода 1.
 */
#define PARAM_ID_DIGITAL_OUT_1_INVERSION 621
/**
 * Тип цифрового выхода 2.
 */
#define PARAM_ID_DIGITAL_OUT_2_TYPE 622
/**
 * Инверсия цифрового выхода 2.
 */
#define PARAM_ID_DIGITAL_OUT_2_INVERSION 623
/**
 * Тип цифрового выхода 3.
 */
#define PARAM_ID_DIGITAL_OUT_3_TYPE 624
/**
 * Инверсия цифрового выхода 3.
 */
#define PARAM_ID_DIGITAL_OUT_3_INVERSION 625
/**
 * Тип цифрового выхода 4.
 */
#define PARAM_ID_DIGITAL_OUT_4_TYPE 626
/**
 * Инверсия цифрового выхода 4.
 */
#define PARAM_ID_DIGITAL_OUT_4_INVERSION 627

/**
 * Параметры цифровых входов/выходов.
 */
/**
 * Время игнорирования изменения цифровых входов.
 */
#define PARAM_ID_DIGITAL_IO_DEADTIME_MS 640

/*
 * Modbus.
 */
/**
 * Скорость соединения Modbus RTU.
 */
#define PARAM_ID_MODBUS_BAUD 700

/*
 * Настройки интерфейса панели (GUI)
 */
/**
 * Работа зуммера (звукового оповещения)
 */
#define PARAM_ID_GUI_BUZZER 800
/**
 * Пароль администратора
 */
#define PARAM_ID_GUI_PASSWORD_ADMIN 804
/**
 * Пароль рута
 */
#define PARAM_ID_GUI_PASSWORD_ROOT 808

/*
 * Отладочные параметры.
 */
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

/*
 * Данные питания.
 */
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

