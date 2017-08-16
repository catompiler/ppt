/**
 * @file translation_ru.h Файл перевода на русский язык.
 */

#ifndef TRANSLATION_RU_H
#define TRANSLATION_RU_H

#include "localization/localization.h"
#include "translations_ids.h"

/**
 * Русский перевод.
 */
TEXT_TRS_BEGIN(text_trs_ru)
TEXT_TR(TR_ID_UNITS_V, "В")
TEXT_TR(TR_ID_UNITS_A, "А")
TEXT_TR(TR_ID_UNITS_OHM, "Ом")
TEXT_TR(TR_ID_UNITS_OHM_SHORT, "Ω")
TEXT_TR(TR_ID_UNITS_KW, "кВт")
TEXT_TR(TR_ID_UNITS_TORQUE, "Н•м")
TEXT_TR(TR_ID_UNITS_TORQUE_SHORT, "Η") // Нм
TEXT_TR(TR_ID_UNITS_S, "с")
TEXT_TR(TR_ID_UNITS_MS, "мс")
TEXT_TR(TR_ID_UNITS_US, "мкс")
TEXT_TR(TR_ID_UNITS_HOUR, "ч")
TEXT_TR(TR_ID_UNITS_PERCENT, "%")
TEXT_TR(TR_ID_UNITS_DEGREE, "°")
TEXT_TR(TR_ID_UNITS_DEGREE_CELSIUS, "°C")
TEXT_TR(TR_ID_UNITS_RPM, "об/м")  
TEXT_TR(TR_ID_UNITS_RPM_SHORT, "Ξ") // RPM
TEXT_TR(TR_ID_ENUM_PHASE_UNK, "Не задано")
TEXT_TR(TR_ID_ENUM_PHASE_A, "Фаза A")
TEXT_TR(TR_ID_ENUM_PHASE_B, "Фаза B")
TEXT_TR(TR_ID_ENUM_PHASE_C, "Фаза C")
TEXT_TR(TR_ID_ENUM_NO, "Нет")
TEXT_TR(TR_ID_ENUM_YES, "Да")
TEXT_TR(TR_ID_ENUM_EXC_MODE_FIXED, "Фиксированное")
TEXT_TR(TR_ID_ENUM_EXC_MODE_REGULATED, "Регулируемое")
TEXT_TR(TR_ID_ENUM_EXC_MODE_EXTERNAL, "Внешнее")
TEXT_TR(TR_ID_ENUM_STOP_MODE_NORMAL, "Нормальный")
TEXT_TR(TR_ID_ENUM_STOP_MODE_FAST, "Быстрый")
TEXT_TR(TR_ID_ENUM_STOP_MODE_COAST, "Выбег")
TEXT_TR(TR_ID_ENUM_PROT_ACTION_IGNORE, "Игнорировать")
TEXT_TR(TR_ID_ENUM_PROT_ACTION_WARNING, "Предупреждение")
TEXT_TR(TR_ID_ENUM_PROT_ACTION_FAST_STOP, "Быстрый останов")
TEXT_TR(TR_ID_ENUM_PROT_ACTION_COAST_STOP, "Останов выбегом")
TEXT_TR(TR_ID_ENUM_PROT_ACTION_EMERGENCY_STOP, "Экстренный останов")
TEXT_TR(TR_ID_ENUM_DIO_IN_NONE, "Не подключен")
TEXT_TR(TR_ID_ENUM_DIO_IN_START_STOP, "Запуск/Останов")
TEXT_TR(TR_ID_ENUM_DIO_IN_EMERGENCY_STOP, "Экстренный останов")
TEXT_TR(TR_ID_ENUM_DIO_IN_REFERENCE_INC, "Инкремент задания")
TEXT_TR(TR_ID_ENUM_DIO_IN_REFERENCE_DEC, "Декремент задания")
TEXT_TR(TR_ID_ENUM_DIO_IN_CLEAR_ERRORS, "Сброс ошибок")
TEXT_TR(TR_ID_ENUM_DIO_IN_USER, "Пользовательский")
TEXT_TR(TR_ID_ENUM_DIO_OUT_NONE, "Не подключен")
TEXT_TR(TR_ID_ENUM_DIO_OUT_OK, "Привод в порядке")
TEXT_TR(TR_ID_ENUM_DIO_OUT_READY, "Готовность")
TEXT_TR(TR_ID_ENUM_DIO_OUT_RUNNING, "Работа")
TEXT_TR(TR_ID_ENUM_DIO_OUT_ERROR, "Ошибка")
TEXT_TR(TR_ID_ENUM_DIO_OUT_WARNING, "Предупреждение")
TEXT_TR(TR_ID_ENUM_DIO_OUT_USER, "Пользовательский")
TEXT_TR(TR_ID_MENU_SUPPLY, "Питание")
TEXT_TR(TR_ID_MENU_SUPPLY_IN, "Сеть")
TEXT_TR(TR_ID_MENU_U_NOM, "Ном. U сети")
TEXT_TR(TR_ID_MENU_I_NOM, "Ном. I сети")

TEXT_TR(TR_ID_MENU_MOTOR, "Двигатель")
TEXT_TR(TR_ID_MENU_MOTOR_P_NOM, "Номинальная мощность")
TEXT_TR(TR_ID_MENU_MOTOR_RPM_NOM, "Номинальные обороты")
TEXT_TR(TR_ID_MENU_MOTOR_RPM_MAX, "Максимальные обороты")
TEXT_TR(TR_ID_MENU_MOTOR_U_ROT_NOM, "Номинальное U якоря")
TEXT_TR(TR_ID_MENU_MOTOR_I_ROT_NOM, "Номинальный I якоря")
TEXT_TR(TR_ID_MENU_MOTOR_U_EXC_NOM, "Номинальное U возбуждения")
TEXT_TR(TR_ID_MENU_MOTOR_I_EXC_NOM, "Номинальный I возбуждения")
TEXT_TR(TR_ID_MENU_MOTOR_EFF_NOM, "Номинальный КПД")
TEXT_TR(TR_ID_MENU_MOTOR_R_ROT_NOM, "Номинальное R якоря")

TEXT_TR(TR_ID_MENU_ROTOR, "Якорь")
TEXT_TR(TR_ID_MENU_EXCITATION, "Возбуждение")
TEXT_TR(TR_ID_MENU_I_EXC, "I возбуждения")
TEXT_TR(TR_ID_MENU_EXC_PHASE, "Фаза возбуждения")
TEXT_TR(TR_ID_MENU_EXC_MODE, "Режим возбуждения")
TEXT_TR(TR_ID_MENU_RAMP, "Разгон и торможение")
TEXT_TR(TR_ID_MENU_RAMP_START_TIME, "Время разгона 0-100%")
TEXT_TR(TR_ID_MENU_RAMP_STOP_TIME, "Время торможения 100-0%")
TEXT_TR(TR_ID_MENU_RAMP_FAST_STOP_TIME, "Время быстрого торможения 100-0%")
TEXT_TR(TR_ID_MENU_RAMP_REFERENCE_TIME, "Время изменения задания 0-100%")
TEXT_TR(TR_ID_MENU_RAMP_STOP_MODE, "Режим останова")
TEXT_TR(TR_ID_MENU_PID, "ПИД регулятор")
TEXT_TR(TR_ID_MENU_PID_ROT, "ПИД якоря")
TEXT_TR(TR_ID_MENU_PID_EXC, "ПИД возбуждения")
TEXT_TR(TR_ID_MENU_PID_SPD, "ПИД скорости")
TEXT_TR(TR_ID_MENU_PID_K_P, "Коэффициент П")
TEXT_TR(TR_ID_MENU_PID_K_I, "Коэффициент И")
TEXT_TR(TR_ID_MENU_PID_K_D, "Коэффициент Д")
TEXT_TR(TR_ID_MENU_START_STOP, "Запуск и останов")
TEXT_TR(TR_ID_MENU_ROT_STOP_TIME, "Время останова якоря")
TEXT_TR(TR_ID_MENU_EXC_STOP_TIME, "Время останова возбуждения")
TEXT_TR(TR_ID_MENU_EXC_START_TIME, "Время запуска возбуждения")
TEXT_TR(TR_ID_MENU_PHASES_CHECK_TIME, "Время ожидания фаз")
TEXT_TR(TR_ID_MENU_ZERO_SENSOR_TIME, "Время срабатывания датчиков нуля")
TEXT_TR(TR_ID_MENU_TRIACS, "Тиристоры")
TEXT_TR(TR_ID_MENU_TRIACS_OPEN_TIME, "Время открытия")
TEXT_TR(TR_ID_MENU_TRIACS_PAIRS_OPEN_TIME, "Силовые")
TEXT_TR(TR_ID_MENU_TRIAC_EXC_OPEN_TIME, "Возбуждения")
TEXT_TR(TR_ID_MENU_TRIACS_OPEN_ANGLES, "Углы открытия")
TEXT_TR(TR_ID_MENU_TRIACS_OPEN_ANGLE_MIN, "Минимальный угол")
TEXT_TR(TR_ID_MENU_TRIACS_OPEN_ANGLE_MAX, "Максимальный угол")
TEXT_TR(TR_ID_MENU_PROTECTION, "Защита")
TEXT_TR(TR_ID_MENU_CUTOFF, "Отсечка")
TEXT_TR(TR_ID_MENU_PROT_LEVEL, "Отклонение")
TEXT_TR(TR_ID_MENU_PROT_ENABLE, "Включено")
TEXT_TR(TR_ID_MENU_PROT_LEVEL_TIME_MS, "Время отклонения")
TEXT_TR(TR_ID_MENU_PROT_LATCH_ENABLE, "Защёлка")
TEXT_TR(TR_ID_MENU_PROT_ACTION, "Действие")
TEXT_TR(TR_ID_MENU_PROT_OVERFLOW, "Повышение")
TEXT_TR(TR_ID_MENU_PROT_UNDERFLOW, "Понижение")
TEXT_TR(TR_ID_MENU_PROT_ERROR_LEVEL, "Уровень ошибки")
TEXT_TR(TR_ID_MENU_PROT_WARNING_LEVEL, "Уровень предупреждения")
TEXT_TR(TR_ID_MENU_THERMAL_OVERLOAD_PROT, "Тепловая защита")
TEXT_TR(TR_ID_MENU_THERMAL_OVERLOAD_PROT_TIME_2I, "Время работы при перегрузе 2x")
TEXT_TR(TR_ID_MENU_EMERGENCY_STOP, "Экстренный останов")
TEXT_TR(TR_ID_MENU_POWER_VOLTAGE, "Напряжение сети")
TEXT_TR(TR_ID_MENU_POWER_CURRENT, "Ток сети")
TEXT_TR(TR_ID_MENU_ROTOR_VOLTAGE, "Напряжение якоря")
TEXT_TR(TR_ID_MENU_ROTOR_CURRENT, "Ток якоря")
TEXT_TR(TR_ID_MENU_EXC_CURRENT, "Ток возбуждения")
TEXT_TR(TR_ID_MENU_ZERO_DRIFT, "Отклонения нулей")
TEXT_TR(TR_ID_MENU_CALCULATION, "Вычисления")
TEXT_TR(TR_ID_MENU_CALC_PHASE_CURRENT, "Вычислять ток для фазы")
TEXT_TR(TR_ID_MENU_CONFIGURATION, "Конфигурация")
TEXT_TR(TR_ID_MENU_MAIN_CONFIG, "Общие")
TEXT_TR(TR_ID_MENU_COMMUNICATION, "Коммуникация")
TEXT_TR(TR_ID_MENU_MODBUS_BAUD, "Скорость Modbus RTU")
TEXT_TR(TR_ID_MENU_MODBUS_ADDRESS, "Адрес Modbus RTU")
TEXT_TR(TR_ID_MENU_DIGITAL_INPUTS, "Цифровые входа")
TEXT_TR(TR_ID_MENU_DIGITAL_OUTPUTS, "Цифровые выхода")
TEXT_TR(TR_ID_MENU_DIO_TYPE, "Тип")
TEXT_TR(TR_ID_MENU_DIO_INVERSION, "Инверсия")
TEXT_TR(TR_ID_MENU_DIGITAL_IO, "Цифровые входа/выхода")
TEXT_TR(TR_ID_MENU_DIGITAL_IO_SETTINGS, "Параметры цифровых входов/выходов")
TEXT_TR(TR_ID_MENU_DIGITAL_IO_DEADTIME_MS, "Время игнорирования изменения состояния входа")
TEXT_TR(TR_ID_MENU_DIO_INPUT1, "Вход 1")
TEXT_TR(TR_ID_MENU_DIO_INPUT2, "Вход 2")
TEXT_TR(TR_ID_MENU_DIO_INPUT3, "Вход 3")
TEXT_TR(TR_ID_MENU_DIO_INPUT4, "Вход 4")
TEXT_TR(TR_ID_MENU_DIO_INPUT5, "Вход 5")
TEXT_TR(TR_ID_MENU_DIO_OUTPUT1, "Выход 1")
TEXT_TR(TR_ID_MENU_DIO_OUTPUT2, "Выход 2")
TEXT_TR(TR_ID_MENU_DIO_OUTPUT3, "Выход 3")
TEXT_TR(TR_ID_MENU_DIO_OUTPUT4, "Выход 4")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULTS, "Коэффициенты АЦП")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Ua, "Коэффициент Ua")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Ub, "Коэффициент Ub")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Uc, "Коэффициент Uc")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Urot, "Коэффициент Urot")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Ia, "Коэффициент Ia")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Ib, "Коэффициент Ib")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Ic, "Коэффициент Ic")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Irot, "Коэффициент Irot")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Iexc, "Коэффициент Iexc")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Iref, "Коэффициент Iref")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Ifan, "Коэффициент Ifan")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA, "Калибровочные данные")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Ua, "Значение калибровки Ua")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Ub, "Значение калибровки Ub")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Uc, "Значение калибровки Uc")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Urot, "Значение калибровки Urot")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Ia, "Значение калибровки Ia")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Ib, "Значение калибровки Ib")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Ic, "Значение калибровки Ic")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Irot, "Значение калибровки Irot")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Iexc, "Значение калибровки Iexc")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Iref, "Значение калибровки Iref")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Ifan, "Значение калибровки Ifan")
TEXT_TR(TR_ID_MENU_VALUE_MULTS, "Коэффициенты значений")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Ua, "Коэффициент Ua")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Ub, "Коэффициент Ub")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Uc, "Коэффициент Uc")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Urot, "Коэффициент Urot")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Ia, "Коэффициент Ia")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Ib, "Коэффициент Ib")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Ic, "Коэффициент Ic")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Irot, "Коэффициент Irot")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Iexc, "Коэффициент Iexc")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Iref, "Коэффициент Iref")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Ifan, "Коэффициент Ifan")
TEXT_TR(TR_ID_MENU_PHASE_SYNC, "Синхронизация с фазами")
TEXT_TR(TR_ID_MENU_PHASE_SYNC_ACCURACY, "Точность синхронизации")
TEXT_TR(TR_ID_MENU_PHASE_SYNC_PID, "ПИД-регулятор ФАПЧ")
TEXT_TR(TR_ID_MENU_PHASES_PROT, "Защита фаз")
TEXT_TR(TR_ID_MENU_PHASE_STATE_PROT, "Состояние фаз")
TEXT_TR(TR_ID_MENU_PHASES_ANGLES_PROT, "Угол между фазами")
TEXT_TR(TR_ID_MENU_PHASES_SYNC_PROT, "Синхронизация с фазами")
TEXT_TR(TR_ID_MENU_ROT_BREAK_PROT, "Обрыв якоря")
TEXT_TR(TR_ID_MENU_HEATSINK_TEMP_PROT, "Перегрев радиатора")
TEXT_TR(TR_ID_MENU_GUI, "Настройки панели (GUI)")
TEXT_TR(TR_ID_MENU_GUI_TILE_WARNINGS, "Вывод предупреждений")
TEXT_TR(TR_ID_MENU_GUI_TILES, "Плитки")
TEXT_TR(TR_ID_MENU_GUI_TILE_1, "Плитка 1")
TEXT_TR(TR_ID_MENU_GUI_TILE_2, "Плитка 2")
TEXT_TR(TR_ID_MENU_GUI_TILE_3, "Плитка 3")
TEXT_TR(TR_ID_MENU_GUI_TILE_4, "Плитка 4")          
TEXT_TR(TR_ID_MENU_GUI_BUZZER, "Звуковое оповещение")
TEXT_TR(TR_ID_MENU_GUI_PASSWORD_ADMIN, "Пароль администратора")
TEXT_TR(TR_ID_MENU_GUI_PASSWORD_ROOT, "Пароль ROOT")
TEXT_TR(TR_ID_MENU_GUI_LANGUAGE, "Язык")
TEXT_TR(TR_ID_MENU_COMMANDS, "Команды")
TEXT_TR(TR_ID_MENU_CMD_START_STOP, "Старт/Стоп")
TEXT_TR(TR_ID_MENU_CMD_ENABLE_DRIVE, "Enable drive")
TEXT_TR(TR_ID_MENU_CMD_SETTINGS_SAVE, "Записать параметры в память")
TEXT_TR(TR_ID_MENU_CMD_STATE_SAVE, "Записать состояние в память")
TEXT_TR(TR_ID_MENU_CMD_SETTINGS_DEFAULT, "Уст. настр. по умолч.")  
TEXT_TR(TR_ID_MENU_CMD_TEST_LEDS, "Проверка светодиодов")  
TEXT_TR(TR_ID_MENU_CMD_CLEAR_ERRORS, "Сброс ошибок")  
TEXT_TR(TR_ID_MENU_CMD_CLEAR_EVENTS, "Очистить историю событий") 
        
TEXT_TR(TR_ID_MENU_STATUS, "Статус привода")
TEXT_TR(TR_ID_MENU_EVENTS, "История событий")
TEXT_TR(TR_ID_MENU_MEASUREMENTS, "Измерения") 

TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_POWER_U_A, "Напр. фазы A")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_POWER_U_B, "Напр. фазы B")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_POWER_U_C, "Напр. фазы C")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_POWER_U_ROT, "Напр. якоря")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_POWER_I_A, "Ток фазы A")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_POWER_I_B, "Ток фазы B")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_POWER_I_C, "Ток фазы C")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_POWER_I_ROT, "Ток якоря")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_POWER_I_EXC, "Ток возб.")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_POWER_I_FAN, "Ток вент.")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_POWER_I_REF, "Ток 4-20 мА")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_HEATSINK_TEMP, "Темп. радиат.")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_HEATSINK_FAN_RPM, "Об. вент. радаиат.")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE, "Угол откр. тир-ов")
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_TRIAC_EXC_OPEN_ANGLE, "Уг.откр.тир.возб.")
/////////// Мотор ////////////
// Вычисленный коэффициент полезного действия.
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_MOTOR_EFF, "Выч. КПД")
// Вычисленное сопротивление якоря.
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_MOTOR_R_ROT, "Выч. R якоря")
// Вычисленные обороты.
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_MOTOR_RPM, "Выч. обороты")
// Вычисленный момент.
TEXT_TR(TR_ID_MENU_MESS_PARAM_ID_MOTOR_TORQUE, "Выч. момент")
        
// Цифровые входа
TEXT_TR(TR_ID_MENU_DIGITAL_INPUTS_STATE, "Цифровые входа")
TEXT_TR(TR_ID_MENU_DIGITAL_IN_1_STATE, "Вход 1")
TEXT_TR(TR_ID_MENU_DIGITAL_IN_2_STATE, "Вход 2")
TEXT_TR(TR_ID_MENU_DIGITAL_IN_3_STATE, "Вход 3")
TEXT_TR(TR_ID_MENU_DIGITAL_IN_4_STATE, "Вход 4")
TEXT_TR(TR_ID_MENU_DIGITAL_IN_5_STATE, "Вход 5")
// Цифровые выхода
TEXT_TR(TR_ID_MENU_DIGITAL_OUTPUTS_STATE, "Цифровые выхода")
TEXT_TR(TR_ID_MENU_DIGITAL_OUT_1_STATE, "Выход 1")
TEXT_TR(TR_ID_MENU_DIGITAL_OUT_2_STATE, "Выход 2")
TEXT_TR(TR_ID_MENU_DIGITAL_OUT_3_STATE, "Выход 3")
TEXT_TR(TR_ID_MENU_DIGITAL_OUT_4_STATE, "Выход 4") 
        
// Наработка привода
TEXT_TR(TR_ID_MENU_RUNTIMES, "Наработка привода")
// Время работы после последнего включения
TEXT_TR(TR_ID_MENU_LAST_RUNTIME, "Работа последн. пуска")
// Время работы
TEXT_TR(TR_ID_MENU_RUNTIME, "Общая работа")
// Время работы вентиляторов
TEXT_TR(TR_ID_MENU_FAN_RUNTIME, "Работа вентиляторов")
// Время во вкл.состоянии (включая готовность)
TEXT_TR(TR_ID_MENU_LIFETIME, "Время включения")
            
//TEXT_TR(TR_ID_MENU_, "")
//TEXT_TR(TR_ID_ENUM_, "")
//TEXT_TR(TR_ID_, "")
    
// справка меню
TEXT_TR(TR_ID_HELP_POWER, "Питание")
TEXT_TR(TR_ID_HELP_POWER_IN, "Номинальное напряжение и ток сети Номинальное напряжение и ток сети Номинальное напряжение и ток сети Номинальное напряжение и ток сети Номинальное напряжение и ток сети")
TEXT_TR(TR_ID_HELP_U_NOM, "Номинальное напряжение сети в вольтах")
TEXT_TR(TR_ID_HELP_I_NOM, "Номинальный ток сети в амперах")
TEXT_TR(TR_ID_HELP_ROTOR, "Напряжение и ток якоря")
TEXT_TR(TR_ID_HELP_U_ROT_NOM, "Номинальное напряжение якоря в вольтах")
TEXT_TR(TR_ID_HELP_I_ROT_NOM, "Номинальный ток якоря в амперах")

TEXT_TR(TR_ID_FAN_CONTROL, "Управление вентиляторами")
TEXT_TR(TR_ID_FAN_CONTROL_ENABLE, "Разрешение управления вентиляторами")
TEXT_TR(TR_ID_FAN_TEMP_MIN, "Температура включения вентилятора на минимум")
TEXT_TR(TR_ID_FAN_TEMP_MAX, "Температура включения вентилятора на максимум")
TEXT_TR(TR_ID_FAN_RPM_MIN, "Начальные обороты вентилятора")
TEXT_TR(TR_ID_FAN_ECO_MODE_ENABLE, "Разрешение эко-режима")
TEXT_TR(TR_ID_FAN_ECO_COOLING_TIME, "Время охлаждения в эко-режиме")
TEXT_TR(TR_ID_FAN_I_NOM, "Номинальный ток вентилятора")
TEXT_TR(TR_ID_FAN_I_ZERO_NOISE, "Шум нуля тока вентилятора")
TEXT_TR(TR_ID_FAN_PROT_TIME, "Время срабатывания защиты вентилятора")
TEXT_TR(TR_ID_FAN_PROT_OVF_LEVEL, "Уровень предупреждения превышения тока вентилятора")

TEXT_TR(TR_ID_MENU_REGULATOR, "Регулятор")
TEXT_TR(TR_ID_MENU_REGULATOR_MODE, "Режим регулятора")
TEXT_TR(TR_ID_ENUM_REGULATOR_MODE_SPEED, "Поддержание скорости")
TEXT_TR(TR_ID_ENUM_REGULATOR_MODE_TORQUE, "Поддержание момента")

TEXT_TR(TR_ID_ENUM_BUZZER_ON, "Включено")
TEXT_TR(TR_ID_ENUM_BUZZER_OFF, "Без звука")
  
TEXT_TR(TR_ID_ENUM_TILES_DRIVE_POWER_Ua, "Напр. фазы A")
TEXT_TR(TR_ID_ENUM_TILES_DRIVE_POWER_Ia, "Ток фазы A")
TEXT_TR(TR_ID_ENUM_TILES_DRIVE_POWER_Ub, "Напр. фазы B")
TEXT_TR(TR_ID_ENUM_TILES_DRIVE_POWER_Ib, "Ток фазы B") 
TEXT_TR(TR_ID_ENUM_TILES_DRIVE_POWER_Uc, "Напр. фазы C")
TEXT_TR(TR_ID_ENUM_TILES_DRIVE_POWER_Ic, "Ток фазы C")   
TEXT_TR(TR_ID_ENUM_TILES_DRIVE_POWER_Urot, "Напр. якоря")
TEXT_TR(TR_ID_ENUM_TILES_DRIVE_POWER_Irot, "Ток якоря")
TEXT_TR(TR_ID_ENUM_TILES_DRIVE_POWER_Iexc, "Ток возб.")
TEXT_TR(TR_ID_ENUM_TILES_DRIVE_POWER_Iref, "Ток 4-20 мА")
TEXT_TR(TR_ID_ENUM_TILES_DRIVE_POWER_U_ROT_WIRES, "Напр.якоря б/п")  
TEXT_TR(TR_ID_ENUM_TILES_DRIVE_POWER_Ifan, "Ток вент.")    
TEXT_TR(TR_ID_ENUM_TILES_HEATSINK_TEMP, "Темп. радиат.") 
TEXT_TR(TR_ID_ENUM_TILES_HEATSINK_FAN_RPM, "Об. вент. рад.") 
TEXT_TR(TR_ID_ENUM_TILES_TRIACS_PAIRS_OPEN_ANGLE, "Уг.откр.тир.") 
TEXT_TR(TR_ID_ENUM_TILES_TRIAC_EXC_OPEN_ANGLE, "Уг.о.тир.возб.") 
TEXT_TR(TR_ID_ENUM_TILES_MOTOR_EFF, "Выч. КПД") 
TEXT_TR(TR_ID_ENUM_TILES_MOTOR_R_ROT, "Выч. R якоря") 
TEXT_TR(TR_ID_ENUM_TILES_MOTOR_RPM, "Выч. обороты") 
TEXT_TR(TR_ID_ENUM_TILES_MOTOR_TORQUE, "Выч. момент") 
        
//! Язык интерфейса        
TEXT_TR(TR_ID_ENUM_LANGUAGES_RU, "Русский (RU)") 
TEXT_TR(TR_ID_ENUM_LANGUAGES_EN, "Английский (EN)")         
 
//! Состояния цифровых входов/выходов
TEXT_TR(TR_ID_ENUM_DIGITAL_STATES_ON, "Включен (1)") 
TEXT_TR(TR_ID_ENUM_DIGITAL_STATES_OFF, "Выключен (0)")  

//! Перечисления плиток для выбора отображения текста ошибок и предупреждений
TEXT_TR(TR_ID_ENUM_GUI_TILE_1, "Пл.1") 
TEXT_TR(TR_ID_ENUM_GUI_TILE_2, "Пл.2") 
TEXT_TR(TR_ID_ENUM_GUI_TILE_3, "Пл.3") 
TEXT_TR(TR_ID_ENUM_GUI_TILE_4, "Пл.4") 
TEXT_TR(TR_ID_ENUM_GUI_TILE_NONE, "Нет") 
        
//! Напряжения фаз
TEXT_TR(TR_ID_HOME_PHASE_A_VOLTAGE, "Напряжение\nфазы A")
TEXT_TR(TR_ID_HOME_PHASE_B_VOLTAGE, "Напряжение\nфазы B")
TEXT_TR(TR_ID_HOME_PHASE_C_VOLTAGE, "Напряжение\nфазы C")
//! Токи фаз
TEXT_TR(TR_ID_HOME_PHASE_A_CURRENT, "Ток фазы A")
TEXT_TR(TR_ID_HOME_PHASE_B_CURRENT, "Ток фазы B")
TEXT_TR(TR_ID_HOME_PHASE_C_CURRENT, "Ток фазы C")
        
// главный экран
TEXT_TR(TR_ID_HOME_ANCHOR_VOLTAGE, "Напряжение\nякоря")
TEXT_TR(TR_ID_HOME_U_ROT_WIRES, "Напряжение\nна якоре")        
TEXT_TR(TR_ID_HOME_ANCHOR_CURRENT, "Ток\nякоря")
TEXT_TR(TR_ID_HOME_EXCITATION_CURRENT, "Ток\nвозбуждения")
TEXT_TR(TR_ID_HOME_POWER_I_FAN, "Ток\nвентилятора")
TEXT_TR(TR_ID_HOME_POWER_I_REF, "Ток\n4-20 мА")
TEXT_TR(TR_ID_HOME_HEATSINK_TEMP, "Температура\nрадиатора")
TEXT_TR(TR_ID_HOME_HEATSINK_FAN_RPM, "Обороты\nвентилятора")
TEXT_TR(TR_ID_HOME_TRIACS_PAIRS_OPEN_ANGLE, "Угол откр.\nосн.тиристоров")
TEXT_TR(TR_ID_HOME_TRIAC_EXC_OPEN_ANGLE, "Угол откр.сим.\nвозбуждения")   
/////////// Мотор ////////////
// Вычисленный коэффициент полезного действия.
TEXT_TR(TR_ID_HOME_MOTOR_EFF, "Вычисленный\nКПД")
// Вычисленное сопротивление якоря.
TEXT_TR(TR_ID_HOME_MOTOR_R_ROT, "Сопротивление\nякоря")
// Вычисленные обороты.
TEXT_TR(TR_ID_HOME_MOTOR_RPM, "Вычисленные\nобороты")
// Вычисленный момент.
TEXT_TR(TR_ID_HOME_MOTOR_TORQUE, "Вычисленный\nмомент")
        
// меню
TEXT_TR(TR_ID_MENU_HEADER_TITLE, "Меню")
// запрос пароля
TEXT_TR(TR_ID_MENU_PASSWORD_REQUEST, "Доступ по паролю:")
// запрос пароля "Нажмите Esc чтобы выйти"
TEXT_TR(TR_ID_MENU_PASSWORD_REQUEST_ESC, "Нажмите Esc чтобы выйти") 
// Ошибка выполнения команды
TEXT_TR(TR_ID_MENU_COMMAND_RESULT_FAIL, "Ошибка выполнения")
// Успешное выполнение команды
TEXT_TR(TR_ID_MENU_COMMAND_RESULT_SUCCESS, "Команда выполнена")       
     
// Значения, отображаемые 
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUES, "Отображаемые значения")
// Напряжение фазы
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_U, "Напряжение фазы")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ALARM_MAX, "Ошибка (макс.)")
// Напряжение якоря
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT, "Напряжение якоря")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_U_ROT_ALARM_MAX, "Ошибка (макс.)")
// Ток фазы
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I, "Ток фазы")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ALARM_MAX, "Ошибка (макс.)")
// Ток якоря
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT, "Ток якоря")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_ROT_ALARM_MAX, "Ошибка (макс.)")
// Ток возбуждения
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC, "Ток возбуждения")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_EXC_ALARM_MAX, "Ошибка (макс.)")
// Температура радиатора
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP, "Температура радиатора")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_TEMP_ALARM_MAX, "Ошибка (макс.)")
// Ток вентилятора
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN, "Ток вентилятора")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_FAN_ALARM_MAX, "Ошибка (макс.)")
// Ток 4-20.
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF, "Ток 4-20 мА")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_POWER_I_REF_ALARM_MAX, "Ошибка (макс.)")
// Обороты вентилятора радиатора.
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM, "Обороты вент. рад.")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_HEATSINK_FAN_RPM_ALARM_MAX, "Ошибка (макс.)")
// Угол открытия тиристоров.
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE, "Угол откр. тир.")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_TRIACS_PAIRS_OPEN_ANGLE_ALARM_MAX, "Ошибка (макс.)")
// Угол открытия возбуждения.
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE, "Угол откр. возб.")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_TRIAC_EXC_OPEN_ANGLE_ALARM_MAX, "Ошибка (макс.)")
/////////// Мотор //////////// 
// Вычисленный коэффициент полезного действия.
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF, "Вычисленный КПД")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_EFF_ALARM_MAX, "Ошибка (макс.)")
// Вычисленное сопротивление якоря.
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT, "Выч.сопр.якоря")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT_WARN_MAX, "Предупр. (макс.)") 
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_R_ROT_ALARM_MAX, "Ошибка (макс.)")
// Вычисленные обороты. 
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM, "Выч. обороты")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_RPM_ALARM_MAX, "Ошибка (макс.)")
// Вычисленный момент.
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE, "Выч. момент")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE_ALARM_MIN, "Ошибка (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE_WARN_MIN, "Предупр. (мин.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE_WARN_MAX, "Предупр. (макс.)")
TEXT_TR(TR_ID_MENU_GUI_TILE_VALUE_MOTOR_TORQUE_ALARM_MAX, "Ошибка (макс.)")
        
// Список событий пуст        
TEXT_TR(TR_ID_MENU_EVENTS_NONE, "Список событий пуст")
// Тип события - Состояние
TEXT_TR(TR_ID_MENU_EVENT_STATUS, "Событие: состояние")
// Тип события - Предупреждение
TEXT_TR(TR_ID_MENU_EVENT_WARNING, "Событие: предупреждение")
// Тип события - Ошибка
TEXT_TR(TR_ID_MENU_EVENT_ERROR, "Событие: ошибка")
// Инициализация
TEXT_TR(TR_ID_MENU_EVENT_DRIVE_STATE_INIT, "Состояние: инициализация")
// Калибровка питания.
TEXT_TR(TR_ID_MENU_EVENT_DRIVE_STATE_CALIBRATION, "Состояние: калибровка питания")
// Простой (готовность).
TEXT_TR(TR_ID_MENU_EVENT_DRIVE_STATE_IDLE, "Состояние: готовность")
// Останов.
TEXT_TR(TR_ID_MENU_EVENT_DRIVE_STATE_STOP, "Состояние: останов")
// Запуск.
TEXT_TR(TR_ID_MENU_EVENT_DRIVE_STATE_START, "Состояние: запуск")
// Работа.
TEXT_TR(TR_ID_MENU_EVENT_DRIVE_STATE_RUN, "Состояние: работа")
// Останов при ошибке.
TEXT_TR(TR_ID_MENU_EVENT_DRIVE_STATE_STOP_ERROR, "Состояние: останов при ошибке")
// Ошибка.
TEXT_TR(TR_ID_MENU_EVENT_DRIVE_STATE_ERROR, "Состояние: ошибка")        
// Формат вывода задания в подсказке события      
TEXT_TR(TR_ID_MENU_EVENT_DRIVE_REFERENCE_FORMAT, "Задание: ")
        
//! Описание ошибок привода
TEXT_TR(TR_ID_DRIVE_ERROR_POWER_DATA_NOT_AVAIL, "E#1: Данные питания не поступают с АЦП.")
TEXT_TR(TR_ID_DRIVE_ERROR_POWER_INVALID, "E#2: Неправильные значения питания.")
TEXT_TR(TR_ID_DRIVE_ERROR_EMERGENCY_STOP, "E#4: Аварийный останов.")
TEXT_TR(TR_ID_DRIVE_ERROR_PHASE, "E#8: Ошибка состояния фаз.")
TEXT_TR(TR_ID_DRIVE_ERROR_PHASE_ANGLE, "E#10: Ошибка угла между фазами.")
TEXT_TR(TR_ID_DRIVE_ERROR_PHASE_SYNC, "E#20: Ошибка синхронизации фаз.")
TEXT_TR(TR_ID_DRIVE_ERROR_THERMAL_OVERLOAD, "E#40: Тепловая защита.")
TEXT_TR(TR_ID_DRIVE_ERROR_ROT_BREAK, "E#80: Обрыв якоря.")
TEXT_TR(TR_ID_DRIVE_ERROR_HEATSINK_TEMP, "E#200: Перегрев радиатора.")
TEXT_TR(TR_ID_DRIVE_ERROR_ROT_MEASURE_BREAK, "E#100: Обрыв измерения якоря.")
TEXT_TR(TR_ID_DRIVE_ERROR_SENSOR, "E#400: Ошибка датчиков.")        
// питания
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Ua, "EP#1: Низкое напряжение фазы A.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Ua, "EP#2: Высокое напряжение фазы A.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Ub, "EP#3: Низкое напряжение фазы B.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Ub, "EP#4: Высокое напряжение фазы B.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Uc, "EP#5: Низкое напряжение фазы C.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Uc, "EP#6: Высокое напряжение фазы C.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Urot, "EP#7: Низкое напряжение якоря.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Urot, "EP#8: Высокое напряжение якоря.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Ia, "EP#9: Низкий ток фазы A.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Ia, "EP#10: Высокий ток фазы A.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Ib, "EP#11: Низкий ток фазы B.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Ib, "EP#12: Высокий ток фазы B.")   
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Ic, "EP#13: Низкий ток фазы C.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Ic, "EP#14: Высокий ток фазы C.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Irot, "EP#15: Низкий ток якоря.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Irot, "EP#16: Высокий ток якоря.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Iexc, "EP#17: Низкий ток возбуждения")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Iexc, "EP#18: Высокий ток возбуждения.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Ifan, "EP#19: Низкий ток вентилятора.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Ifan, "EP#20: Высокий ток вентилятора.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Iref, "EP#21: Низкий ток 4-20 мА.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Iref, "EP#22: Высокий ток 4-20 мА.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_IDLE_Ia, "EP#23: Нулевой ток фазы A.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_IDLE_Ib, "EP#24: Нулевой ток фазы B.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_IDLE_Ic, "EP#25: Нулевой ток фазы C.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_IDLE_Urot, "EP#26: Нулевое напряжение якоря.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_IDLE_Irot, "EP#27: Нулевой ток якоря.")
TEXT_TR(TR_ID_DRIVE_POWER_ERROR_IDLE_Iexc, "EP#28: Нулевой ток возбуждения.")
//! Описание предупреждений привода
TEXT_TR(TR_ID_DRIVE_WARNING_POWER, "W#2: Предупреждение по питанию.")
TEXT_TR(TR_ID_DRIVE_WARNING_PHASE_ANGLE, "W#10: Ошибка угла между фазами.")
TEXT_TR(TR_ID_DRIVE_WARNING_PHASE_SYNC, "W#20: Ошибка синхронизации фаз.")
TEXT_TR(TR_ID_DRIVE_WARNING_THERMAL_OVERLOAD, "W#40: Перегрев.")
TEXT_TR(TR_ID_DRIVE_WARNING_FAN_FAIL, "W#100: Ошибка вентилятора.")
TEXT_TR(TR_ID_DRIVE_WARNING_HEATSINK_TEMP, "W#200: Перегрев радиатора.")
TEXT_TR(TR_ID_DRIVE_WARNING_TRIAC, "W#1000: Ошибка тиристоров.")
TEXT_TR(TR_ID_DRIVE_WARNING_SENSOR, "W#400: Ошибка датчиков.")
// питания
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Ua, "WP#1: Низкое напряжение фазы A.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Ua, "WP#2: Высокое напряжение фазы A.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Ub, "WP#3: Низкое напряжение фазы B.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Ub, "WP#4: Высокое напряжение фазы B.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Uc, "WP#5: Низкое напряжение фазы C.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Uc, "WP#6: Высокое напряжение фазы C.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Urot, "WP#7: Низкое напряжение якоря.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Urot, "WP#8: Высокое напряжение якоря.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Ia, "WP#9: Низкий ток фазы A.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Ia, "WP#10: Высокий ток фазы A.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Ib, "WP#11: Низкий ток фазы B.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Ib, "WP#12: Высокий ток фазы B.")   
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Ic, "WP#13: Низкий ток фазы C.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Ic, "WP#14: Высокий ток фазы C.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Irot, "WP#15: Низкий ток якоря.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Irot, "WP#16: Высокий ток якоря.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Iexc, "WP#17: Низкий ток возбуждения")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Iexc, "WP#18: Высокий ток возбуждения.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Ifan, "WP#19: Низкий ток вентилятора.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Ifan, "WP#20: Высокий ток вентилятора.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Iref, "WP#21: Низкий ток 4-20 мА.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Iref, "WP#22: Высокий ток 4-20 мА.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_IDLE_Ia, "WP#23: Нулевой ток фазы A.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_IDLE_Ib, "WP#24: Нулевой ток фазы B.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_IDLE_Ic, "WP#25: Нулевой ток фазы C.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_IDLE_Urot, "WP#26: Нулевое напряжение якоря.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_IDLE_Irot, "WP#27: Нулевой ток якоря.")
TEXT_TR(TR_ID_DRIVE_POWER_WARNING_IDLE_Iexc, "WP#28: Нулевой ток возбуждения.")

TEXT_TR(TR_ID_MENU_VERSION, "Версия")    
TEXT_TR(TR_ID_MENU_VERSION_DATE, "Дата прошивки") 
        
TEXT_TRS_END()

#endif /* TRANSLATION_RU_H */
