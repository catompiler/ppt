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
//! Обороты.
#define TR_ID_UNITS_RPM 135

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
//! ПИД скорости.
#define TR_ID_MENU_PID_SPD          293
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
//! Углы открытия.
#define TR_ID_MENU_TRIACS_OPEN_ANGLES  314
//! Минимальный угол.
#define TR_ID_MENU_TRIACS_OPEN_ANGLE_MIN 315
//! Максимальный угол.
#define TR_ID_MENU_TRIACS_OPEN_ANGLE_MAX 316
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
//! Адрес Modbus RTU.
#define TR_ID_MENU_MODBUS_ADDRESS   367
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
#define TR_ID_MENU_ADC_CALIBRATION_DATA     420
//! Коэффициент Ua.
#define TR_ID_MENU_ADC_CALIBRATION_DATA_Ua    421
//! Коэффициент Ub.
#define TR_ID_MENU_ADC_CALIBRATION_DATA_Ub    422
//! Коэффициент Uc.
#define TR_ID_MENU_ADC_CALIBRATION_DATA_Uc    423
//! Коэффициент Urot.
#define TR_ID_MENU_ADC_CALIBRATION_DATA_Urot  424
//! Коэффициент Ia.
#define TR_ID_MENU_ADC_CALIBRATION_DATA_Ia    425
//! Коэффициент Ib.
#define TR_ID_MENU_ADC_CALIBRATION_DATA_Ib    426
//! Коэффициент Ic.
#define TR_ID_MENU_ADC_CALIBRATION_DATA_Ic    427
//! Коэффициент Irot.
#define TR_ID_MENU_ADC_CALIBRATION_DATA_Irot  428
//! Коэффициент Iexc.
#define TR_ID_MENU_ADC_CALIBRATION_DATA_Iexc  429
//! Коэффициент Iref.
#define TR_ID_MENU_ADC_CALIBRATION_DATA_Iref  430
//! Коэффициент Ifan.
#define TR_ID_MENU_ADC_CALIBRATION_DATA_Ifan  431
//! Коэффициенты значений.
#define TR_ID_MENU_VALUE_MULTS  432
//! Коэффициент Ua.
#define TR_ID_MENU_VALUE_MULT_Ua    433
//! Коэффициент Ub.
#define TR_ID_MENU_VALUE_MULT_Ub    434
//! Коэффициент Uc.
#define TR_ID_MENU_VALUE_MULT_Uc    435
//! Коэффициент Urot.
#define TR_ID_MENU_VALUE_MULT_Urot  436
//! Коэффициент Ia.
#define TR_ID_MENU_VALUE_MULT_Ia    437
//! Коэффициент Ib.
#define TR_ID_MENU_VALUE_MULT_Ib    438
//! Коэффициент Ic.
#define TR_ID_MENU_VALUE_MULT_Ic    439
//! Коэффициент Irot.
#define TR_ID_MENU_VALUE_MULT_Irot  440
//! Коэффициент Iexc.
#define TR_ID_MENU_VALUE_MULT_Iexc  441
//! Коэффициент Iref.
#define TR_ID_MENU_VALUE_MULT_Iref  442
//! Коэффициент Ifan.
#define TR_ID_MENU_VALUE_MULT_Ifan  443
// //! .
// #define TR_ID_MENU_

//! Меню синхронизации с фазами.
#define TR_ID_MENU_PHASE_SYNC           450
//! Точность синхронизации с фазами.
#define TR_ID_MENU_PHASE_SYNC_ACCURACY  451
//! Меню ПИД синхронизации с фазами.
#define TR_ID_MENU_PHASE_SYNC_PID       452

//! Меню защиты фаз.
#define TR_ID_MENU_PHASES_PROT        460
//! Защита последовательности фаз.
#define TR_ID_MENU_PHASE_STATE_PROT   461
//! Защита по углам между фазами.
#define TR_ID_MENU_PHASES_ANGLES_PROT 462
//! Защита по углам между фазами.
#define TR_ID_MENU_PHASES_SYNC_PROT   463
//! Меню защиты от обрыва якоря.
#define TR_ID_MENU_ROT_BREAK_PROT   464
//! Меню защиты по температуре радиатора.
#define TR_ID_MENU_HEATSINK_TEMP_PROT 465

//! Меню интерфейса (GUI)
#define TR_ID_MENU_GUI              471
//! Плитки (выбор отображаемых измерений)
#define TR_ID_MENU_GUI_TILES        472
#define TR_ID_MENU_GUI_TILE_1        473
#define TR_ID_MENU_GUI_TILE_2        474
#define TR_ID_MENU_GUI_TILE_3        475
#define TR_ID_MENU_GUI_TILE_4        476
//! Работа зуммера (звукового оповещения)
#define TR_ID_MENU_GUI_BUZZER       477
//! Работа зуммера (звукового оповещения)
#define TR_ID_MENU_GUI_PASSWORD_ADMIN       478
//! Работа зуммера (звукового оповещения)
#define TR_ID_MENU_GUI_PASSWORD_ROOT       479

#define TR_ID_MENU_GUI_LANGUAGE             480

//! Меню Команды
#define TR_ID_MENU_COMMANDS     481
//! Старт/стоп
#define TR_ID_MENU_CMD_START_STOP       482
//! Enable/drive
#define TR_ID_MENU_CMD_ENABLE_DRIVE     483 
//! Сохранить настройки
#define TR_ID_MENU_CMD_SETTINGS_SAVE    484
//! Сохранить состояние
#define TR_ID_MENU_CMD_STATE_SAVE       485 
//! Настройки по умолчанию
#define TR_ID_MENU_CMD_SETTINGS_DEFAULT 486
//! Тест светодиодов
#define TR_ID_MENU_CMD_TEST_LEDS        487
//! Сброс ошибок
#define TR_ID_MENU_CMD_CLEAR_EVENTS     488

//! Меню Статус привода
#define TR_ID_MENU_STATUS 490
//! История событий
#define TR_ID_MENU_EVENTS 491
//! Измерения
#define TR_ID_MENU_MEASUREMENTS 492

#define TR_ID_MENU_MESS_PARAM_ID_POWER_U_A                      511
#define TR_ID_MENU_MESS_PARAM_ID_POWER_U_B                      512
#define TR_ID_MENU_MESS_PARAM_ID_POWER_U_C                      513
#define TR_ID_MENU_MESS_PARAM_ID_POWER_U_ROT                    514
#define TR_ID_MENU_MESS_PARAM_ID_POWER_I_A                      515
#define TR_ID_MENU_MESS_PARAM_ID_POWER_I_B                      516
#define TR_ID_MENU_MESS_PARAM_ID_POWER_I_C                      517
#define TR_ID_MENU_MESS_PARAM_ID_POWER_I_ROT                    518
#define TR_ID_MENU_MESS_PARAM_ID_POWER_I_EXC                    519
#define TR_ID_MENU_MESS_PARAM_ID_POWER_I_FAN                    520
#define TR_ID_MENU_MESS_PARAM_ID_POWER_I_REF                    521
#define TR_ID_MENU_MESS_PARAM_ID_HEATSINK_TEMP                  522
#define TR_ID_MENU_MESS_PARAM_ID_HEATSINK_FAN_RPM               523
#define TR_ID_MENU_MESS_PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE        524
#define TR_ID_MENU_MESS_PARAM_ID_TRIAC_EXC_OPEN_ANGLE           525

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



//! Управление вентиляторами.
#define TR_ID_FAN_CONTROL            600
//! Разрешение управления вентиляторами.
#define TR_ID_FAN_CONTROL_ENABLE     601
//! Температура включения вентилятора.
#define TR_ID_FAN_TEMP_MIN           602
//! Температура включения вентилятора на максимум.
#define TR_ID_FAN_TEMP_MAX           603
//! Начальные обороты вентилятора, %.
#define TR_ID_FAN_RPM_MIN            604
//! Разрешение эко-режима.
#define TR_ID_FAN_ECO_MODE_ENABLE    605
//! Время охлаждения в эко-режиме.
#define TR_ID_FAN_ECO_COOLING_TIME   606
//! Номинальный ток вентилятора.
#define TR_ID_FAN_I_NOM              607
//! Шум нуля тока вентилятора, А.
#define TR_ID_FAN_I_ZERO_NOISE       608
//! Время срабатывания защиты вентилятора, мс.
#define TR_ID_FAN_PROT_TIME          609
//! Значение уровеня предупреждения превышения тока вентилятора, %.
#define TR_ID_FAN_PROT_OVF_LEVEL     610


//! Настройки интерфейса

//! Перечисление работы зуммера
//! Зуммер включен
#define TR_ID_ENUM_BUZZER_ON        710
//! Зуммер отключен
#define TR_ID_ENUM_BUZZER_OFF       711
//! Измеренные значения
#define TR_ID_ENUM_TILES_DRIVE_POWER_Ua     720
#define TR_ID_ENUM_TILES_DRIVE_POWER_Ia     721 
#define TR_ID_ENUM_TILES_DRIVE_POWER_Ub     722 
#define TR_ID_ENUM_TILES_DRIVE_POWER_Ib     723 
#define TR_ID_ENUM_TILES_DRIVE_POWER_Uc     724
#define TR_ID_ENUM_TILES_DRIVE_POWER_Ic     725     
#define TR_ID_ENUM_TILES_DRIVE_POWER_Urot   726   
#define TR_ID_ENUM_TILES_DRIVE_POWER_Irot   727 
#define TR_ID_ENUM_TILES_DRIVE_POWER_Iexc   728
#define TR_ID_ENUM_TILES_DRIVE_POWER_Iref   729
#define TR_ID_ENUM_TILES_DRIVE_POWER_Ifan   730 
#define TR_ID_ENUM_HEATSINK_TEMP            740
#define TR_ID_ENUM_HEATSINK_FAN_RPM         742
#define TR_ID_ENUM_TRIACS_PAIRS_OPEN_ANGLE  744
#define TR_ID_ENUM_TRIAC_EXC_OPEN_ANGLE     746
//! Языки интерфейса
#define TR_ID_ENUM_LANGUAGES_RU             750
#define TR_ID_ENUM_LANGUAGES_EN             751
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

//! Напряжения фаз
#define TR_ID_HOME_PHASE_A_VOLTAGE 901
#define TR_ID_HOME_PHASE_B_VOLTAGE 902
#define TR_ID_HOME_PHASE_C_VOLTAGE 903
//! Токи фаз
#define TR_ID_HOME_PHASE_A_CURRENT 906
#define TR_ID_HOME_PHASE_B_CURRENT 907
#define TR_ID_HOME_PHASE_C_CURRENT 908
//! Напряжение якоря
#define TR_ID_HOME_ANCHOR_VOLTAGE 910
//! Ток якоря
#define TR_ID_HOME_ANCHOR_CURRENT 920
//! Ток возбуждения
#define TR_ID_HOME_EXCITATION_CURRENT 930
//! Ток вентилятора.
#define TR_ID_HOME_POWER_I_FAN  934
//! Ток 4-20 мА.
#define TR_ID_HOME_POWER_I_REF  936
//! Температура радиатора
#define TR_ID_HOME_HEATSINK_TEMP 940
//! Обороты вентилятора охлаждения радиатора
#define TR_ID_HOME_HEATSINK_FAN_RPM  942 
//! Угол открытия основных тиристоров.
#define TR_ID_HOME_TRIACS_PAIRS_OPEN_ANGLE 944
//! Угол открытия основных симистора возбуждения.
#define TR_ID_HOME_TRIAC_EXC_OPEN_ANGLE 946
        

//! Заголовок меню
#define TR_ID_MENU_HEADER_TITLE  1010

//! Запрос доступа к элементам меню 
#define TR_ID_MENU_PASSWORD_REQUEST 1012
//! Сообщение: Нажмите Esc для выхода
#define TR_ID_MENU_PASSWORD_REQUEST_ESC 1013

// Значения, отображаемые 
#define TR_ID_MENU_GUI_TILE_VALUES                      1210
// Напряжение фазы
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_U               1220
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ALARM_MIN     1221
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_U_WARN_MIN      1222
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_U_WARN_MAX      1223
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ALARM_MAX     1224
// Напряжение якоря
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT               1230
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_ALARM_MIN     1231
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_WARN_MIN      1232
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_WARN_MAX      1233
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_ALARM_MAX     1234
// Ток фазы
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I                        1240
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ALARM_MIN              1241
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_WARN_MIN               1242
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_WARN_MAX               1243
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ALARM_MAX              1244
// Ток якоря
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT                        1250
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_ALARM_MIN              1251
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_WARN_MIN               1252
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_WARN_MAX               1253
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_ALARM_MAX              1254
// Ток возбуждения
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC                        1260
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_ALARM_MIN              1261
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_WARN_MIN               1262
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_WARN_MAX               1263
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_ALARM_MAX              1264
// Температура радиатора
#define TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP                        1270
#define TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_ALARM_MIN              1271
#define TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_WARN_MIN               1272
#define TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_WARN_MAX               1273
#define TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_ALARM_MAX              1274
// Ток вентилятора
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN                        1280
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_ALARM_MIN              1281
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_WARN_MIN               1282
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_WARN_MAX               1283
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_ALARM_MAX              1284
// Ток 4-20.
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF                        1290
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_ALARM_MIN              1291
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_WARN_MIN               1292
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_WARN_MAX               1293
#define TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_ALARM_MAX              1294
// Обороты вентилятора радиатора.
#define TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM                        1300
#define TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_ALARM_MIN              1301
#define TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_WARN_MIN               1302
#define TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_WARN_MAX               1303
#define TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_ALARM_MAX              1304
// Угол открытия тиристоров.
#define TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE                 1310
#define TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MIN       1311
#define TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_WARN_MIN        1312
#define TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_WARN_MAX        1313
#define TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MAX       1314
// Угол открытия возбуждения.
#define TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE                     1320
#define TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_ALARM_MIN           1321
#define TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_WARN_MIN            1322
#define TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_WARN_MAX            1323
#define TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_ALARM_MAX           1324

//! Описание ошибок привода
#define TR_ID_DRIVE_ERROR_POWER_DATA_NOT_AVAIL  2101 //!< Данные питания не поступают с АЦП.
#define TR_ID_DRIVE_ERROR_POWER_INVALID         2102 //!< Неправильные значения питания, см. drive_power_error_t.
#define TR_ID_DRIVE_ERROR_EMERGENCY_STOP        2103 //!< Аварийный останов (Грибок).
#define TR_ID_DRIVE_ERROR_PHASE                 2104 //!< Ошибка состояния фаз.
#define TR_ID_DRIVE_ERROR_PHASE_ANGLE           2105 //!< Ошибка угла между фазами.
#define TR_ID_DRIVE_ERROR_PHASE_SYNC            2106 //!< Ошибка синхронизации фаз.
#define TR_ID_DRIVE_ERROR_THERMAL_OVERLOAD      2107 //!< Тепловая защита.
#define TR_ID_DRIVE_ERROR_ROT_BREAK             2108 //!< Обрыв якоря.
//! Описание предупреждений привода
#define TR_ID_DRIVE_WARNING_POWER                     2202 //!< Предупреждение по питанию.
#define TR_ID_DRIVE_WARNING_PHASE_ANGLE               2203 //!< Ошибка угла между фазами.
#define TR_ID_DRIVE_WARNING_PHASE_SYNC                2204 //!< Ошибка синхронизации фаз.
#define TR_ID_DRIVE_WARNING_THERMAL_OVERLOAD          2205 //!< Перегрев.

#endif /* TRANSLATIONS_IDS_H */

