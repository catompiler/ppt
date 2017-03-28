/**
 * @file translations_ids.h Список идентификаторов текстов перевода.
 */

#ifndef TRANSLATIONS_IDS_H
#define TRANSLATIONS_IDS_H

/**
 * Единицы измерения.
 */
//! Вольты.
#define TR_ID_UNITS_V 100
//! Амперы.
#define TR_ID_UNITS_A 101
//! Секунды.
#define TR_ID_UNITS_S 110
//! Миллисекунды.
#define TR_ID_UNITS_MS 111
//! Микросекунды.
#define TR_ID_UNITS_US 112
//! Проценты.
#define TR_ID_UNITS_PERCENT 120
//! Градусы.
#define TR_ID_UNITS_DEGREE 125
//! Градусы цельсия.
#define TR_ID_UNITS_DEGREE_CELSIUS 130

/**
 * Перечисления меню.
 */
/**
 * Значения фаз.
 */
//! Фаза не задана.
#define TR_ID_ENUM_PHASE_UNK 200
//! Фаза A.
#define TR_ID_ENUM_PHASE_A   201
//! Фаза B.
#define TR_ID_ENUM_PHASE_B   202
//! Фаза C.
#define TR_ID_ENUM_PHASE_C   203

/**
 * Перечисление Да/Нет.
 */
//! Нет.
#define TR_ID_ENUM_NO   205
//! Да.
#define TR_ID_ENUM_YES  206

/**
 * Перечисление режима возбуждения.
 */
//! Фиксированное.
#define TR_ID_ENUM_EXC_MODE_FIXED       207
//! Регулируемое.
#define TR_ID_ENUM_EXC_MODE_REGULATED   208

/**
 * Перечисление режима останова.
 */
//! Нормальный.
#define TR_ID_ENUM_STOP_MODE_NORMAL     210
//! Быстрый.
#define TR_ID_ENUM_STOP_MODE_FAST       211
//! Выбег.
#define TR_ID_ENUM_STOP_MODE_COAST      212

/**
 * Действие элемента защиты.
 */
//! Игнорировать.
#define TR_ID_ENUM_PROT_ACTION_IGNORE           215
//! Предупреждение.
#define TR_ID_ENUM_PROT_ACTION_WARNING          216
//! Быстрый останов.
#define TR_ID_ENUM_PROT_ACTION_FAST_STOP        217
//! Останов выбегом.
#define TR_ID_ENUM_PROT_ACTION_COAST_STOP       218
//! Экстренный останов.
#define TR_ID_ENUM_PROT_ACTION_EMERGENCY_STOP   219

/**
 * Перечисление типа цифровых входов.
 */
//! Не подключен.
#define TR_ID_ENUM_DIO_IN_NONE              220
//! Запуск/Останов.
#define TR_ID_ENUM_DIO_IN_START_STOP        221
//! Экстренный останов.
#define TR_ID_ENUM_DIO_IN_EMERGENCY_STOP    222
//! Инкремент задания.
#define TR_ID_ENUM_DIO_IN_REFERENCE_INC     223
//! Декремент задания.
#define TR_ID_ENUM_DIO_IN_REFERENCE_DEC     224
//! Сброс ошибок.
#define TR_ID_ENUM_DIO_IN_CLEAR_ERRORS      225
//! Пользовательский.
#define TR_ID_ENUM_DIO_IN_USER              226

/**
 * Перечисление типа цифровых выходов..
 */
//! Не подключен.
#define TR_ID_ENUM_DIO_OUT_NONE         230
//! Привод в порядке.
#define TR_ID_ENUM_DIO_OUT_OK           231
//! Готовность.
#define TR_ID_ENUM_DIO_OUT_READY        232
//! Работа.
#define TR_ID_ENUM_DIO_OUT_RUNNING      233
//! Ошибка.
#define TR_ID_ENUM_DIO_OUT_ERROR        234
//! Предупреждение.
#define TR_ID_ENUM_DIO_OUT_WARNING      235
//! Пользовательский.
#define TR_ID_ENUM_DIO_OUT_USER         236

/**
 * Меню.
 */
//! Питание.
#define TR_ID_MENU_POWER            250
//! Сеть.
#define TR_ID_MENU_POWER_IN         251
//! Ном. U сети.
#define TR_ID_MENU_U_NOM            252
//! Ном. I сети.
#define TR_ID_MENU_I_NOM            253
//! Якорь.
#define TR_ID_MENU_ROTOR            260
//! Ном. U якоря.
#define TR_ID_MENU_U_ROT_NOM        261
//! Ном. I якоря.
#define TR_ID_MENU_I_ROT_NOM        262
//! Возбуждение.
#define TR_ID_MENU_EXCITATION       270
//! I возбуждения.
#define TR_ID_MENU_I_EXC            271
//! Фаза возбуждения.
#define TR_ID_MENU_EXC_PHASE        272
//! Режим возбуждения.
#define TR_ID_MENU_EXC_MODE         273
//! Разгон и торможение.
#define TR_ID_MENU_RAMP             280
//! Время разгона 0-100%.
#define TR_ID_MENU_RAMP_START_TIME  281
//! Время торможения 100-0%.
#define TR_ID_MENU_RAMP_STOP_TIME   282
//! Время быстрого торможения 100-0%.
#define TR_ID_MENU_RAMP_FAST_STOP_TIME  283
//! Время изменения задания 0-100%.
#define TR_ID_MENU_RAMP_REFERENCE_TIME  284
//! Режим останова.
#define TR_ID_MENU_RAMP_STOP_MODE   285
//! ПИД регулятор.
#define TR_ID_MENU_PID              290
//! ПИД якоря.
#define TR_ID_MENU_PID_ROT          291
//! ПИД возбуждения.
#define TR_ID_MENU_PID_EXC          292
//! Коэффициент П.
#define TR_ID_MENU_PID_K_P          295
//! Коэффициент И.
#define TR_ID_MENU_PID_K_I          296
//! Коэффициент Д.
#define TR_ID_MENU_PID_K_D          297
//! Запуск и останов.
#define TR_ID_MENU_START_STOP       300
//! Время останова якоря.
#define TR_ID_MENU_ROT_STOP_TIME    301
//! Время останова возбуждения.
#define TR_ID_MENU_EXC_STOP_TIME    302
//! Время запуска возбуждения.
#define TR_ID_MENU_EXC_START_TIME   303
//! Время ожидания фаз.
#define TR_ID_MENU_PHASES_CHECK_TIME 304
//! Время ожидания фаз.
#define TR_ID_MENU_ZERO_SENSOR_TIME 305
//! Тиристоры.
#define TR_ID_MENU_TRIACS           310
//! Время открытия.
#define TR_ID_MENU_TRIACS_OPEN_TIME 311
//! Силовые.
#define TR_ID_MENU_TRIACS_PAIRS_OPEN_TIME 312
//! Возбуждения.
#define TR_ID_MENU_TRIAC_EXC_OPEN_TIME 313
//! Защита.
#define TR_ID_MENU_PROTECTION       320
//! Отсечка.
#define TR_ID_MENU_CUTOFF           321
//! Отклонение.
#define TR_ID_MENU_PROT_LEVEL       322
//! Включено.
#define TR_ID_MENU_PROT_ENABLE      323
//! Время отклонения.
#define TR_ID_MENU_PROT_LEVEL_TIME_MS 324
//! Защёлка.
#define TR_ID_MENU_PROT_LATCH_ENABLE  325
//! Действие.
#define TR_ID_MENU_PROT_ACTION      326
//! Повышение.
#define TR_ID_MENU_PROT_OVERFLOW    327
//! Понижение.
#define TR_ID_MENU_PROT_UNDERFLOW   328
//! Уровень ошибки.
#define TR_ID_MENU_PROT_ERROR_LEVEL 329
//! Уровень предупреждения.
#define TR_ID_MENU_PROT_WARNING_LEVEL 330
//! Тепловая защита.
#define TR_ID_MENU_THERMAL_OVERLOAD_PROT 340
//! Время работы при перегрузе 6x.
#define TR_ID_MENU_THERMAL_OVERLOAD_PROT_TIME_6I 341
//! Экстренный останов.
#define TR_ID_MENU_EMERGENCY_STOP   342
//! Напряжение сети.
#define TR_ID_MENU_POWER_VOLTAGE    343
//! Ток сети.
#define TR_ID_MENU_POWER_CURRENT    344
//! Напряжение якоря.
#define TR_ID_MENU_ROTOR_VOLTAGE    345
//! Ток якоря.
#define TR_ID_MENU_ROTOR_CURRENT    346
//! Ток возбуждения.
#define TR_ID_MENU_EXC_CURRENT      347
//! Отклонения нулей.
#define TR_ID_MENU_ZERO_DRIFT       350
//! Вычисления.
#define TR_ID_MENU_CALCULATION      360
//! Вычислять ток для фазы.
#define TR_ID_MENU_CALC_PHASE_CURRENT 361
//! Коммуникация.
#define TR_ID_MENU_COMMUNICATION    365
//! Скорость Modbus RTU.
#define TR_ID_MENU_MODBUS_BAUD      366
//! Цифровые входа.
#define TR_ID_MENU_DIGITAL_INPUTS   370
//! Цифровые выхода.
#define TR_ID_MENU_DIGITAL_OUTPUTS  371
//! Тип.
#define TR_ID_MENU_DIO_TYPE         372
//! Инверсия.
#define TR_ID_MENU_DIO_INVERSION    373
//! Цифровые входа/выхода.
#define TR_ID_MENU_DIGITAL_IO       374
//! Параметры Цифровых входов/выходов.
#define TR_ID_MENU_DIGITAL_IO_SETTINGS 375
//! Время игнорирования изменения состояния входа.
#define TR_ID_MENU_DIGITAL_IO_DEADTIME_MS 376
//! Вход 1.
#define TR_ID_MENU_DIO_INPUT1       380
//! Вход 2.
#define TR_ID_MENU_DIO_INPUT2       381
//! Вход 3.
#define TR_ID_MENU_DIO_INPUT3       382
//! Вход 4.
#define TR_ID_MENU_DIO_INPUT4       383
//! Вход 5.
#define TR_ID_MENU_DIO_INPUT5       384
//! Выход 1.
#define TR_ID_MENU_DIO_OUTPUT1      390
//! Выход 2.
#define TR_ID_MENU_DIO_OUTPUT2      391
//! Выход 3.
#define TR_ID_MENU_DIO_OUTPUT3      392
//! Выход 4.
#define TR_ID_MENU_DIO_OUTPUT4      393
//! Коэффициенты АЦП.
#define TR_ID_MENU_ADC_VALUE_MULTS  400
//! Коэффициент Ua.
#define TR_ID_MENU_ADC_VALUE_MULT_Ua    401
//! Коэффициент Ub.
#define TR_ID_MENU_ADC_VALUE_MULT_Ub    402
//! Коэффициент Uc.
#define TR_ID_MENU_ADC_VALUE_MULT_Uc    403
//! Коэффициент Urot.
#define TR_ID_MENU_ADC_VALUE_MULT_Urot  404
//! Коэффициент Ia.
#define TR_ID_MENU_ADC_VALUE_MULT_Ia    405
//! Коэффициент Ib.
#define TR_ID_MENU_ADC_VALUE_MULT_Ib    406
//! Коэффициент Ic.
#define TR_ID_MENU_ADC_VALUE_MULT_Ic    407
//! Коэффициент Irot.
#define TR_ID_MENU_ADC_VALUE_MULT_Irot  408
//! Коэффициент Iexc.
#define TR_ID_MENU_ADC_VALUE_MULT_Iexc  409
//! Коэффициент Iref.
#define TR_ID_MENU_ADC_VALUE_MULT_Iref  410
//! Коэффициент Ifan.
#define TR_ID_MENU_ADC_VALUE_MULT_Ifan  411
//! Калибровочные данные.
#define TR_ID_MENU_CALIBRATION_DATA     420
//! Коэффициент Ua.
#define TR_ID_MENU_CALIBRATION_DATA_Ua    421
//! Коэффициент Ub.
#define TR_ID_MENU_CALIBRATION_DATA_Ub    422
//! Коэффициент Uc.
#define TR_ID_MENU_CALIBRATION_DATA_Uc    423
//! Коэффициент Urot.
#define TR_ID_MENU_CALIBRATION_DATA_Urot  424
//! Коэффициент Ia.
#define TR_ID_MENU_CALIBRATION_DATA_Ia    425
//! Коэффициент Ib.
#define TR_ID_MENU_CALIBRATION_DATA_Ib    426
//! Коэффициент Ic.
#define TR_ID_MENU_CALIBRATION_DATA_Ic    427
//! Коэффициент Irot.
#define TR_ID_MENU_CALIBRATION_DATA_Irot  428
//! Коэффициент Iexc.
#define TR_ID_MENU_CALIBRATION_DATA_Iexc  429
//! Коэффициент Iref.
#define TR_ID_MENU_CALIBRATION_DATA_Iref  430
//! Коэффициент Ifan.
#define TR_ID_MENU_CALIBRATION_DATA_Ifan  431
// //! .
// #define TR_ID_MENU_

//! Меню синхронизации с фазами.
#define TR_ID_MENU_PHASE_SYNC           450
//! Точность синхронизации с фазами.
#define TR_ID_MENU_PHASE_SYNC_ACCURACY  451
//! Меню ПИД синхронизации с фазами.
#define TR_ID_MENU_PHASE_SYNC_PID       452

//! Питание.
#define TR_ID_HELP_POWER            550
//! Сеть.
#define TR_ID_HELP_POWER_IN         551
//! Ном. U сети.
#define TR_ID_HELP_U_NOM            552
//! Ном. I сети.
#define TR_ID_HELP_I_NOM            553
//! Якорь.
#define TR_ID_HELP_ROTOR            560
//! Ном. U якоря.
#define TR_ID_HELP_U_ROT_NOM        561
//! Ном. I якоря.
#define TR_ID_HELP_I_ROT_NOM        562

// /**
//  * .
//  */
// //! .
// #define TR_ID_ENUM_

// /**
//  *
//  */
// //!
// #define TR_ID_

//! Напряжение якоря
#define TR_ID_HOME_ANCHOR_VOLTAGE 910
//! Ток якоря
#define TR_ID_HOME_ANCHOR_CURRENT 920
//! Ток возбуждения
#define TR_ID_HOME_EXCITATION_CURRENT 930
//! Температура радиатора
#define TR_ID_HOME_RADIATOR_TEMPERATURE 940

//! Заголовок меню
#define TR_ID_MENU_HEADER_TITLE  1010

#endif /* TRANSLATIONS_IDS_H */

