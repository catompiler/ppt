/**
 * @file parameters_ids.h Идентификаторы параметров.
 */

#ifndef PARAMETERS_IDS_H
#define	PARAMETERS_IDS_H

// Идентификаторы.


////////////////////////////////////
// Общие настройки с загрузчиком. //
////////////////////////////////////

/*
 * Modbus.
 */
/**
 * Скорость.
 */
#define PARAM_ID_MODBUS_BAUD 1
/**
 * Чётность.
 */
#define PARAM_ID_MODBUS_PARITY 2
/**
 * Стоповые биты.
 */
#define PARAM_ID_MODBUS_STOP_BITS 3
/**
 * Адрес устройства.
 */
#define PARAM_ID_MODBUS_ADDRESS 4
/**
 * Контрольная сумма общих параметров.
 */
#define PARAM_ID_SHARED_CRC 10

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

/*
 * Двигатель.
 */
/**
 * Номинальная мощность двигателя.
 */
#define PARAM_ID_MOTOR_P_NOM    200
/**
 * Номинальные обороты.
 */
#define PARAM_ID_MOTOR_RPM_NOM  201
/**
 * Максимальные обороты.
 */
#define PARAM_ID_MOTOR_RPM_MAX  202
/**
 * Число полюсов.
 */
#define PARAM_ID_MOTOR_POLES  205
/**
 * Номинальное напряжение якоря двигателя, В.
 */
#define PARAM_ID_MOTOR_U_ROT_NOM      210
/**
 * Номинальный ток якоря двигателя, А.
 */
#define PARAM_ID_MOTOR_I_ROT_NOM      211
/**
 * Номинальное напряжение возбуждения двигателя, В.
 */
#define PARAM_ID_MOTOR_U_EXC_NOM      212
/**
 * Номинальный ток возбуждения двигателя, А.
 */
#define PARAM_ID_MOTOR_I_EXC_NOM      213
/**
 * Номинальный коэффициент полезного действия.
 */
#define PARAM_ID_MOTOR_EFF_NOM        214
/**
 * Номинальное сопротивление якоря.
 */
#define PARAM_ID_MOTOR_R_ROT_NOM      215
/**
 * Номинальное сопротивление возбуждения.
 */
#define PARAM_ID_MOTOR_R_EXC_NOM      216
/**
 * Номинальная индуктивность якоря.
 */
#define PARAM_ID_MOTOR_L_ROT_NOM      217
/**
 * Максимальный ток якоря.
 */
#define PARAM_ID_MOTOR_I_ROT_MAX      220
/**
 * Сопротивление проводов до двигателя.
 */
#define PARAM_ID_MOTOR_R_WIRES        230
/**
 * Максимальное выходное напряжение, В.
 */
#define PARAM_ID_U_ROT_MAX      231


///////////////////////
// Параметры работы. //
///////////////////////

/**
 * Фаза для которой необходимо вычислять ток.
 */
#define PARAM_ID_CALC_PHASE_CURRENT 1100

/**
 * Фаза для которой необходимо вычислять напряжение.
 */
#define PARAM_ID_CALC_PHASE_VOLTAGE 1101

/**
 * Необходимость вычисления тока якоря.
 */
#define PARAM_ID_CALC_ROT_CURRENT 1105

/**
 * Необходимость вычисления напряжения якоря.
 */
#define PARAM_ID_CALC_ROT_VOLTAGE 1106

/**
 * Необходимость вычисления тока возбуждения.
 */
#define PARAM_ID_CALC_EXC_CURRENT 1107

/**
 * Регулятор.
 */
/**
 * Режим регулятора.
 */
#define PARAM_ID_REGULATOR_MODE 1125
/**
 * Разрешение IR-компенсации.
 */
#define PARAM_ID_REGULATOR_IR_COMPENSATION 1130

/*
 * Перегруз.
 */
/*
 * Разрешение перегруза.
 */
#define PARAM_ID_REGULATOR_OVERLOAD_ENABLED 1135
/*
 * Режим.
 */
#define PARAM_ID_REGULATOR_OVERLOAD_MODE 1136
/*
 * Базовый ток.
 */
#define PARAM_ID_REGULATOR_OVERLOAD_BASE_CURRENT 1137
/*
 * Максимальный ток.
 */
#define PARAM_ID_REGULATOR_OVERLOAD_MAX_CURRENT 1138
/*
 * Время перегруза.
 */
#define PARAM_ID_REGULATOR_OVERLOAD_TIME 1139
/*
 * Период перегруза.
 */
#define PARAM_ID_REGULATOR_OVERLOAD_PERIOD 1140
/*
 * Зона нечувствительности.
 */
#define PARAM_ID_REGULATOR_OVERLOAD_DEAD_ZONE 1141

/*
 * Возбуждение.
 */
/**
 * Режим возбуждения.
 */
#define PARAM_ID_EXC_MODE  1150
/**
 * Фаза возбуждения.
 */
#define PARAM_ID_EXC_PHASE 1151
/**
 * Поддерживаемый ток возбуждения двигателя, А.
 */
#define PARAM_ID_I_EXC     1152

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
 * Самозапуск.
 */
/**
 * Разрешение.
 */
#define PARAM_ID_SELFSTART_ENABLED 1220
/**
 * Период самозапуска.
 */
#define PARAM_ID_SELFSTART_PERIOD 1221
/**
 * Задержка.
 */
#define PARAM_ID_SELFSTART_DELAY 1222
/**
 * Сброс ошибок.
 */
#define PARAM_ID_SELFSTART_CLEAR_ERRORS_ENABLED 1223
/**
 * Попытки сброса ошибок.
 */
#define PARAM_ID_SELFSTART_CLEAR_ERRORS_ATTEMPTS 1224
/**
 * Период попыток сброса ошибок.
 */
#define PARAM_ID_SELFSTART_CLEAR_ERRORS_PERIOD 1225

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
/**
 * Отключение возбуждения при нулевой скорости.
 */
/**
 * Разрешение.
 */
#define PARAM_ID_ZERO_SPEED_EXC_OFF_ENABLED 1320
/**
 * Таймаут.
 */
#define PARAM_ID_ZERO_SPEED_EXC_OFF_TIMEOUT 1321

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
 * Тиристоры - компенсация задержки прохождения сигнала.
 */
/*
 * Компенсация по тиристорным парам.
 */
#define PARAM_ID_TRIACS_PAIRS_OPEN_DELAY 1355
/*
 * Компенсация по симистору возбуждения.
 */
#define PARAM_ID_TRIAC_EXC_OPEN_DELAY 1356

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
 * Тиристоры - гребёнка.
 */
/**
 * Использование гребёнки при открытии тиристорных пар.
 */
#define PARAM_ID_TRIACS_PAIRS_PULSE_TRAIN_ENABLED 1420
/**
 * Использование гребёнки при открытии тиристоров возбуждения.
 */
#define PARAM_ID_TRIAC_EXC_PULSE_TRAIN_ENABLED 1421
/**
 * Длительность гребёнки при открытии тиристорных пар.
 */
#define PARAM_ID_TRIACS_PAIRS_PULSE_TRAIN_WIDTH 1425
/**
 * Длительность гребёнки при открытии тиристоров возбуждения.
 */
#define PARAM_ID_TRIAC_EXC_PULSE_TRAIN_WIDTH 1426
/**
 * Коэффициент заполнения гребёнки при открытии тиристорных пар.
 */
#define PARAM_ID_TRIACS_PAIRS_PULSE_TRAIN_DUTY_RATIO 1430
/**
 * Коэффициент заполнения гребёнки при открытии тиристоров возбуждения.
 */
#define PARAM_ID_TRIAC_EXC_PULSE_TRAIN_DUTY_RATIO 1431
/**
 * Минимальный угол формирования гребёнки при открытии тиристорных пар.
 */
#define PARAM_ID_TRIACS_PAIRS_PULSE_TRAIN_ANGLE_MIN 1435
/**
 * Минимальный угол формирования гребёнки при открытии тиристоров возбуждения.
 */
#define PARAM_ID_TRIAC_EXC_PULSE_TRAIN_ANGLE_MIN 1436

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


/*
 * Самонастройка.
 */

/*
 * Угол открытия для самонастройки.
 */
#define PARAM_ID_SELFTUNE_OPEN_ANGLE 1610
/*
 * Использовать медианный фильтр для устранения помех.
 */
#define PARAM_ID_SELFTUNE_USE_MID_FILTER 1611
/*
 * Количесвто значений для вычисления dI/dt.
 */
#define PARAM_ID_SELFTUNE_DIDT_AVG_COUNT 1612
/*
 * Количество выборок данных.
 */
#define PARAM_ID_SELFTUNE_ITERS_COUNT 1613
/*
 * Время паузы между выборкаит данных.
 */
#define PARAM_ID_SELFTUNE_PAUSE_TIME_MS 1614
/*
 * Использовать АБ-фильтра.
 */
#define PARAM_ID_SELFTUNE_USE_AB_FILTER 1615
/*
 * Вес АБ-фильтра.
 */
#define PARAM_ID_SELFTUNE_AB_FILTER_WEIGHT 1616



/////////////
// Защита. //
/////////////

/*
 * Общие настройки защиты.
 */
/**
 * Записывать осциллограмму при предупреждении.
 */
#define PARAM_ID_PROT_WARNING_WRITE_OSC 2010

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
 * Время срабатывания тепловой защиты при двухкратном перегрузе по току.
 */
#define PARAM_ID_THERMAL_OVERLOAD_PROT_TIME_2I 2121
/**
 * Действие при срабатывании тепловой защиты по току.
 */
#define PARAM_ID_THERMAL_OVERLOAD_PROT_ACTION 2122
/**
 * Зона нечувствительности.
 */
#define PARAM_ID_THERMAL_OVERLOAD_PROT_DEAD_ZONE 2123


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
 * Обрыв измерения якоря.
 */
//! Разрешение.
#define PARAM_ID_PROT_ROT_MEASURE_BREAK_ENABLED 2140
//! Значение.
#define PARAM_ID_PROT_ROT_MEASURE_BREAK_VALUE 2141
//! Время отклонения, мс.
#define PARAM_ID_PROT_ROT_MEASURE_BREAK_TIME_MS 2142
//! Разрешение защёлки.
#define PARAM_ID_PROT_ROT_MEASURE_BREAK_LATCH_ENABLE 2143
//! Действие.
#define PARAM_ID_PROT_ROT_MEASURE_BREAK_ACTION 2144

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

/*
 * Защита тиристоров.
 */
/*
 * Уровень предупреждения.
 */
//! Разрешение.
#define PARAM_ID_PROT_TRIACS_WARN_ENABLED 2300
//! Значение минимального угла.
#define PARAM_ID_PROT_TRIACS_WARN_MIN_ANGLE 2301
//! Значение минимального тока.
#define PARAM_ID_PROT_TRIACS_WARN_MIN_CURRENT 2302
//! Время отклонения, мс.
#define PARAM_ID_PROT_TRIACS_WARN_TIME_MS 2303
//! Разрешение защёлки.
#define PARAM_ID_PROT_TRIACS_WARN_LATCH_ENABLE 2304
//! Действие.
#define PARAM_ID_PROT_TRIACS_WARN_ACTION 2305

/*
 * Защита датчиков.
 */
/*
 * Защита датчиков напряжения сети.
 */
//! Разрешение.
#define PARAM_ID_PROT_SENSORS_U_IN_ENABLED 2500
//! Нижняя граница значений АЦП.
#define PARAM_ID_PROT_SENSORS_U_IN_ADC_RANGE_MIN 2501
//! Верхняя граница значений АЦП.
#define PARAM_ID_PROT_SENSORS_U_IN_ADC_RANGE_MAX 2502
//! Программное вычисление.
#define PARAM_ID_PROT_SENSORS_U_IN_EMULATION_ENABLED 2503
//! Разрешение защёлки.
#define PARAM_ID_PROT_SENSORS_U_IN_LATCH_ENABLE 2504
//! Действие.
#define PARAM_ID_PROT_SENSORS_U_IN_ACTION 2505
/*
 * Защита датчиков тока сети.
 */
//! Разрешение.
#define PARAM_ID_PROT_SENSORS_I_IN_ENABLED 2510
//! Нижняя граница значений АЦП.
#define PARAM_ID_PROT_SENSORS_I_IN_ADC_RANGE_MIN 2511
//! Верхняя граница значений АЦП.
#define PARAM_ID_PROT_SENSORS_I_IN_ADC_RANGE_MAX 2512
//! Программное вычисление.
#define PARAM_ID_PROT_SENSORS_I_IN_EMULATION_ENABLED 2513
//! Разрешение защёлки.
#define PARAM_ID_PROT_SENSORS_I_IN_LATCH_ENABLE 2514
//! Действие.
#define PARAM_ID_PROT_SENSORS_I_IN_ACTION 2515
/*
 * Защита датчика напряжения якоря.
 */
//! Разрешение.
#define PARAM_ID_PROT_SENSORS_U_ROT_ENABLED 2520
//! Нижняя граница значений АЦП.
#define PARAM_ID_PROT_SENSORS_U_ROT_ADC_RANGE_MIN 2521
//! Верхняя граница значений АЦП.
#define PARAM_ID_PROT_SENSORS_U_ROT_ADC_RANGE_MAX 2522
//! Программное вычисление.
#define PARAM_ID_PROT_SENSORS_U_ROT_EMULATION_ENABLED 2523
//! Разрешение защёлки.
#define PARAM_ID_PROT_SENSORS_U_ROT_LATCH_ENABLE 2524
//! Действие.
#define PARAM_ID_PROT_SENSORS_U_ROT_ACTION 2525
/*
 * Защита датчика тока якоря.
 */
//! Разрешение.
#define PARAM_ID_PROT_SENSORS_I_ROT_ENABLED 2530
//! Нижняя граница значений АЦП.
#define PARAM_ID_PROT_SENSORS_I_ROT_ADC_RANGE_MIN 2531
//! Верхняя граница значений АЦП.
#define PARAM_ID_PROT_SENSORS_I_ROT_ADC_RANGE_MAX 2532
//! Программное вычисление.
#define PARAM_ID_PROT_SENSORS_I_ROT_EMULATION_ENABLED 2533
//! Разрешение защёлки.
#define PARAM_ID_PROT_SENSORS_I_ROT_LATCH_ENABLE 2534
//! Действие.
#define PARAM_ID_PROT_SENSORS_I_ROT_ACTION 2535
/*
 * Защита датчика тока возбуждения.
 */
//! Разрешение.
#define PARAM_ID_PROT_SENSORS_I_EXC_ENABLED 2540
//! Нижняя граница значений АЦП.
#define PARAM_ID_PROT_SENSORS_I_EXC_ADC_RANGE_MIN 2541
//! Верхняя граница значений АЦП.
#define PARAM_ID_PROT_SENSORS_I_EXC_ADC_RANGE_MAX 2542
//! Программное вычисление.
#define PARAM_ID_PROT_SENSORS_I_EXC_EMULATION_ENABLED 2543
//! Разрешение защёлки.
#define PARAM_ID_PROT_SENSORS_I_EXC_LATCH_ENABLE 2544
//! Действие.
#define PARAM_ID_PROT_SENSORS_I_EXC_ACTION 2545


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


/////////////////////
// Входа - выхода. //
/////////////////////

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
 * Вывод предупреждений на плитке
 */
#define PARAM_ID_MENU_GUI_TILE_WARNINGS 6070
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

/////////// Мотор ////////////
// Вычисленный коэффициент полезного действия.
#define PARAM_ID_MOTOR_EFF_ALARM_MIN            6410
#define PARAM_ID_MOTOR_EFF_WARN_MIN             6411
#define PARAM_ID_MOTOR_EFF_WARN_MAX             6412
#define PARAM_ID_MOTOR_EFF_ALARM_MAX            6413
// Вычисленное сопротивление якоря.
#define PARAM_ID_MOTOR_R_ROT_ALARM_MIN          6420
#define PARAM_ID_MOTOR_R_ROT_WARN_MIN           6421
#define PARAM_ID_MOTOR_R_ROT_WARN_MAX           6422
#define PARAM_ID_MOTOR_R_ROT_ALARM_MAX          6423
// Вычисленные обороты.
#define PARAM_ID_MOTOR_RPM_ALARM_MIN            6430
#define PARAM_ID_MOTOR_RPM_WARN_MIN             6431
#define PARAM_ID_MOTOR_RPM_WARN_MAX             6432
#define PARAM_ID_MOTOR_RPM_ALARM_MAX            6433
// Вычисленный момент.
#define PARAM_ID_MOTOR_TORQUE_ALARM_MIN         6440
#define PARAM_ID_MOTOR_TORQUE_WARN_MIN          6441
#define PARAM_ID_MOTOR_TORQUE_WARN_MAX          6442
#define PARAM_ID_MOTOR_TORQUE_ALARM_MAX         6443

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
/**
 * Множитель данных канала Erot.
 */
#define PARAM_ID_VALUE_MULTIPLIER_Erot 7151
/*
 * Усреднение действующих значений.
 */
/**
 * Время усреднения канала Ua.
 */
#define PARAM_ID_AVERAGING_TIME_Ua 7160
/**
 * Время усреднения канала Ub.
 */
#define PARAM_ID_AVERAGING_TIME_Ub 7161
/**
 * Время усреднения канала Uc.
 */
#define PARAM_ID_AVERAGING_TIME_Uc 7162
/**
 * Время усреднения канала Urot.
 */
#define PARAM_ID_AVERAGING_TIME_Urot 7163
/**
 * Время усреднения канала Ia.
 */
#define PARAM_ID_AVERAGING_TIME_Ia 7164
/**
 * Время усреднения канала Ib.
 */
#define PARAM_ID_AVERAGING_TIME_Ib 7165
/**
 * Время усреднения канала Ic.
 */
#define PARAM_ID_AVERAGING_TIME_Ic 7166
/**
 * Время усреднения канала Irot.
 */
#define PARAM_ID_AVERAGING_TIME_Irot 7167
/**
 * Время усреднения канала Iexc.
 */
#define PARAM_ID_AVERAGING_TIME_Iexc 7168
/**
 * Время усреднения канала Iref.
 */
#define PARAM_ID_AVERAGING_TIME_Iref 7169
/**
 * Время усреднения канала Ifan.
 */
#define PARAM_ID_AVERAGING_TIME_Ifan 7170
/**
 * Время усреднения канала Erot.
 */
#define PARAM_ID_AVERAGING_TIME_Erot 7171
/**
 * Время усреднения канала оборотов.
 */
#define PARAM_ID_AVERAGING_TIME_RPM 7180
/**
 * Время усреднения канала момента.
 */
#define PARAM_ID_AVERAGING_TIME_TORQUE 7181


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
 * Вычисленное напряжение якоря.
 */
#define PARAM_ID_POWER_CALC_U_ROT 8150
/**
 * Падение напряжения на проводах до двигателя.
 */
#define PARAM_ID_POWER_U_WIRES 8155
/**
 * Вычисленная ЭДС двигателя.
 */
#define PARAM_ID_POWER_E_ROT 8160

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

/*
 * Мотор.
 */
/**
 * Вычисленный коэффициент полезного действия.
 */
#define PARAM_ID_MOTOR_EFF        8450
/**
 * Вычисленное сопротивление якоря.
 */
#define PARAM_ID_MOTOR_R_ROT      8451
/**
 * Вычисленное сопротивление возбуждения.
 */
#define PARAM_ID_MOTOR_R_EXC      8452
/**
 * Вычисленная индуктивность якоря.
 */
#define PARAM_ID_MOTOR_L_ROT      8453
/**
 * Номинальная ЭДС двигателя.
 */
#define PARAM_ID_MOTOR_E_NOM      8454
/**
 * Номинальный момент двигателя.
 */
#define PARAM_ID_MOTOR_M_NOM      8455
/**
 * Вычисленные обороты.
 */
#define PARAM_ID_MOTOR_RPM        8500
/**
 * Вычисленный момент.
 */
#define PARAM_ID_MOTOR_TORQUE     8501
/**
 * Вычисленная ЭДС двигателя.
 */
#define PARAM_ID_MOTOR_E          8502
/**
 * Вычисленные текущие максимальные обороты.
 */
#define PARAM_ID_MOTOR_CUR_RPM_MAX    8503


///////////////////////////
// Отладочные параметры. //
///////////////////////////

/*
 * ПИД
 */
/**
 * ПИД синхронизации с фазами.
 */
#define PARAM_ID_PID_PHASE_SYNC 9050
/**
 * ПИД регулятора тока возбуждения.
 */
#define PARAM_ID_PID_EXC_CURRENT 9051
/**
 * ПИД регулятора скорости.
 */
#define PARAM_ID_PID_ROT_SPEED 9052
/**
 * ПИД регулятора тока якоря.
 */
#define PARAM_ID_PID_ROT_CURRENT 9053

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

