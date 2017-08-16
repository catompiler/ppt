/**
 * @file translation_en.h Файл перевода на английский язык.
 */

#ifndef TRANSLATION_EN_H
#define TRANSLATION_EN_H

#include "localization/localization.h"
#include "translations_ids.h"

/**
 * Английский перевод.
 */
TEXT_TRS_BEGIN(text_trs_en)
TEXT_TR(TR_ID_UNITS_V, "V")
TEXT_TR(TR_ID_UNITS_A, "A")
TEXT_TR(TR_ID_UNITS_OHM, "Ohm")
TEXT_TR(TR_ID_UNITS_KW, "kW")
TEXT_TR(TR_ID_UNITS_TORQUE, "N•m")
TEXT_TR(TR_ID_UNITS_S, "s")
TEXT_TR(TR_ID_UNITS_MS, "ms")
TEXT_TR(TR_ID_UNITS_US, "us")
TEXT_TR(TR_ID_UNITS_HOUR, "h")
TEXT_TR(TR_ID_UNITS_PERCENT, "%")
TEXT_TR(TR_ID_UNITS_DEGREE, "°")
TEXT_TR(TR_ID_UNITS_DEGREE_CELSIUS, "°C")
TEXT_TR(TR_ID_ENUM_PHASE_UNK, "Not set")
TEXT_TR(TR_ID_ENUM_PHASE_A, "Phase A")
TEXT_TR(TR_ID_ENUM_PHASE_B, "Phase B")
TEXT_TR(TR_ID_ENUM_PHASE_C, "Phase C")
TEXT_TR(TR_ID_ENUM_NO, "No")
TEXT_TR(TR_ID_ENUM_YES, "Yes")
TEXT_TR(TR_ID_ENUM_EXC_MODE_FIXED, "Fixed")
TEXT_TR(TR_ID_ENUM_EXC_MODE_REGULATED, "Regulated")
TEXT_TR(TR_ID_ENUM_EXC_MODE_EXTERNAL, "External")
TEXT_TR(TR_ID_ENUM_STOP_MODE_NORMAL, "Normal")
TEXT_TR(TR_ID_ENUM_STOP_MODE_FAST, "Fast")
TEXT_TR(TR_ID_ENUM_STOP_MODE_COAST, "Coast")
TEXT_TR(TR_ID_ENUM_PROT_ACTION_IGNORE, "Ignore")
TEXT_TR(TR_ID_ENUM_PROT_ACTION_WARNING, "Warning")
TEXT_TR(TR_ID_ENUM_PROT_ACTION_FAST_STOP, "Fast stop")
TEXT_TR(TR_ID_ENUM_PROT_ACTION_COAST_STOP, "Coast stop")
TEXT_TR(TR_ID_ENUM_PROT_ACTION_EMERGENCY_STOP, "Emergency stop")
TEXT_TR(TR_ID_ENUM_DIO_IN_NONE, "Not connected")
TEXT_TR(TR_ID_ENUM_DIO_IN_START_STOP, "Start/Stop")
TEXT_TR(TR_ID_ENUM_DIO_IN_EMERGENCY_STOP, "Emergency stop")
TEXT_TR(TR_ID_ENUM_DIO_IN_REFERENCE_INC, "Reference inc")
TEXT_TR(TR_ID_ENUM_DIO_IN_REFERENCE_DEC, "Reference dec")
TEXT_TR(TR_ID_ENUM_DIO_IN_CLEAR_ERRORS, "Clear errors")
TEXT_TR(TR_ID_ENUM_DIO_IN_USER, "User")
TEXT_TR(TR_ID_ENUM_DIO_OUT_NONE, "Not connected")
TEXT_TR(TR_ID_ENUM_DIO_OUT_OK, "Drive ok")
TEXT_TR(TR_ID_ENUM_DIO_OUT_READY, "Ready")
TEXT_TR(TR_ID_ENUM_DIO_OUT_RUNNING, "Running")
TEXT_TR(TR_ID_ENUM_DIO_OUT_ERROR, "Error")
TEXT_TR(TR_ID_ENUM_DIO_OUT_WARNING, "Warning")
TEXT_TR(TR_ID_ENUM_DIO_OUT_USER, "User")
TEXT_TR(TR_ID_MENU_SUPPLY, "Power")
TEXT_TR(TR_ID_MENU_SUPPLY_IN, "Supply")
TEXT_TR(TR_ID_MENU_U_NOM, "Nominal U")
TEXT_TR(TR_ID_MENU_I_NOM, "Nominal I")

TEXT_TR(TR_ID_MENU_MOTOR, "Motor")
TEXT_TR(TR_ID_MENU_MOTOR_P_NOM, "Nominal power")
TEXT_TR(TR_ID_MENU_MOTOR_RPM_NOM, "Nominal RPM")
TEXT_TR(TR_ID_MENU_MOTOR_RPM_MAX, "Maximum RPM")
TEXT_TR(TR_ID_MENU_MOTOR_U_ROT_NOM, "Nominal U rot")
TEXT_TR(TR_ID_MENU_MOTOR_I_ROT_NOM, "Nominal I rot")
TEXT_TR(TR_ID_MENU_MOTOR_U_EXC_NOM, "Nominal U exc")
TEXT_TR(TR_ID_MENU_MOTOR_I_EXC_NOM, "Nominal I exc")
TEXT_TR(TR_ID_MENU_MOTOR_EFF_NOM, "Nominal efficiency")
TEXT_TR(TR_ID_MENU_MOTOR_R_ROT_NOM, "Nominal R rot")

TEXT_TR(TR_ID_MENU_ROTOR, "Rotor")
TEXT_TR(TR_ID_MENU_EXCITATION, "Field")
TEXT_TR(TR_ID_MENU_I_EXC, "Field I")
TEXT_TR(TR_ID_MENU_EXC_PHASE, "Field phase")
TEXT_TR(TR_ID_MENU_EXC_MODE, "Field mode")
TEXT_TR(TR_ID_MENU_RAMP, "Ramp")
TEXT_TR(TR_ID_MENU_RAMP_START_TIME, "Ramp start time 0-100%")
TEXT_TR(TR_ID_MENU_RAMP_STOP_TIME, "Ramp stop time 100-0%")
TEXT_TR(TR_ID_MENU_RAMP_FAST_STOP_TIME, "Ramp fast stop time 100-0%")
TEXT_TR(TR_ID_MENU_RAMP_REFERENCE_TIME, "Reference time 0-100%")
TEXT_TR(TR_ID_MENU_RAMP_STOP_MODE, "Stop mode")
TEXT_TR(TR_ID_MENU_PID, "PID controller")
TEXT_TR(TR_ID_MENU_PID_ROT, "Rotor PID")
TEXT_TR(TR_ID_MENU_PID_EXC, "Field PID")
TEXT_TR(TR_ID_MENU_PID_SPD, "Speed PID")
TEXT_TR(TR_ID_MENU_PID_K_P, "Coefficient P")
TEXT_TR(TR_ID_MENU_PID_K_I, "Coefficient I")
TEXT_TR(TR_ID_MENU_PID_K_D, "Coefficient D")
TEXT_TR(TR_ID_MENU_START_STOP, "Start and stop")
TEXT_TR(TR_ID_MENU_ROT_STOP_TIME, "Rotor stop time")
TEXT_TR(TR_ID_MENU_EXC_STOP_TIME, "Field stop time")
TEXT_TR(TR_ID_MENU_EXC_START_TIME, "Field start time")
TEXT_TR(TR_ID_MENU_PHASES_CHECK_TIME, "Phases wait time")
TEXT_TR(TR_ID_MENU_ZERO_SENSOR_TIME, "Zero sensors time")
TEXT_TR(TR_ID_MENU_TRIACS, "Triacs")
TEXT_TR(TR_ID_MENU_TRIACS_OPEN_TIME, "Open time")
TEXT_TR(TR_ID_MENU_TRIACS_PAIRS_OPEN_TIME, "Power")
TEXT_TR(TR_ID_MENU_TRIAC_EXC_OPEN_TIME, "Field")
TEXT_TR(TR_ID_MENU_TRIACS_OPEN_ANGLES, "Open angles")
TEXT_TR(TR_ID_MENU_TRIACS_OPEN_ANGLE_MIN, "Min angle")
TEXT_TR(TR_ID_MENU_TRIACS_OPEN_ANGLE_MAX, "Max angle")
TEXT_TR(TR_ID_MENU_PROTECTION, "Protection")
TEXT_TR(TR_ID_MENU_CUTOFF, "Cutoff")
TEXT_TR(TR_ID_MENU_PROT_LEVEL, "Level")
TEXT_TR(TR_ID_MENU_PROT_ENABLE, "Enable")
TEXT_TR(TR_ID_MENU_PROT_LEVEL_TIME_MS, "Level time")
TEXT_TR(TR_ID_MENU_PROT_LATCH_ENABLE, "Latch")
TEXT_TR(TR_ID_MENU_PROT_ACTION, "Action")
TEXT_TR(TR_ID_MENU_PROT_OVERFLOW, "Overflow")
TEXT_TR(TR_ID_MENU_PROT_UNDERFLOW, "Underflow")
TEXT_TR(TR_ID_MENU_PROT_ERROR_LEVEL, "Error level")
TEXT_TR(TR_ID_MENU_PROT_WARNING_LEVEL, "Warning level")
TEXT_TR(TR_ID_MENU_THERMAL_OVERLOAD_PROT, "Thermal overload protection")
TEXT_TR(TR_ID_MENU_THERMAL_OVERLOAD_PROT_TIME_2I, "2x overload work time")
TEXT_TR(TR_ID_MENU_EMERGENCY_STOP, "Emergency stop")
TEXT_TR(TR_ID_MENU_POWER_VOLTAGE, "Supply voltage")
TEXT_TR(TR_ID_MENU_POWER_CURRENT, "Supply current")
TEXT_TR(TR_ID_MENU_ROTOR_VOLTAGE, "Rotor voltage")
TEXT_TR(TR_ID_MENU_ROTOR_CURRENT, "Rotor current")
TEXT_TR(TR_ID_MENU_EXC_CURRENT, "Field current")
TEXT_TR(TR_ID_MENU_ZERO_DRIFT, "Zero drift")
TEXT_TR(TR_ID_MENU_CALCULATION, "Calculations")
TEXT_TR(TR_ID_MENU_CALC_PHASE_CURRENT, "Calc phase current")
TEXT_TR(TR_ID_MENU_COMMUNICATION, "Communication")
TEXT_TR(TR_ID_MENU_MODBUS_BAUD, "Modbus RTU baud")
TEXT_TR(TR_ID_MENU_MODBUS_ADDRESS, "Modbus RTU address")
TEXT_TR(TR_ID_MENU_DIGITAL_INPUTS, "Digital inputs")
TEXT_TR(TR_ID_MENU_DIGITAL_OUTPUTS, "Digital outputs")
TEXT_TR(TR_ID_MENU_DIO_TYPE, "Type")
TEXT_TR(TR_ID_MENU_DIO_INVERSION, "Inversion")
TEXT_TR(TR_ID_MENU_DIGITAL_IO, "Digital inputs/outputs")
TEXT_TR(TR_ID_MENU_DIGITAL_IO_SETTINGS, "Digital inputs/outputs settings")
TEXT_TR(TR_ID_MENU_DIGITAL_IO_DEADTIME_MS, "Digital input dead time")
TEXT_TR(TR_ID_MENU_DIO_INPUT1, "Input 1")
TEXT_TR(TR_ID_MENU_DIO_INPUT2, "Input 2")
TEXT_TR(TR_ID_MENU_DIO_INPUT3, "Input 3")
TEXT_TR(TR_ID_MENU_DIO_INPUT4, "Input 4")
TEXT_TR(TR_ID_MENU_DIO_INPUT5, "Input 5")
TEXT_TR(TR_ID_MENU_DIO_OUTPUT1, "Output 1")
TEXT_TR(TR_ID_MENU_DIO_OUTPUT2, "Output 2")
TEXT_TR(TR_ID_MENU_DIO_OUTPUT3, "Output 3")
TEXT_TR(TR_ID_MENU_DIO_OUTPUT4, "Output 4")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULTS, "ADC Coefficients")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Ua, "Coefficient Ua")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Ub, "Coefficient Ub")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Uc, "Coefficient Uc")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Urot, "Coefficient Urot")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Ia, "Coefficient Ia")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Ib, "Coefficient Ib")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Ic, "Coefficient Ic")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Irot, "Coefficient Irot")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Iexc, "Coefficient Iexc")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Iref, "Coefficient Iref")
TEXT_TR(TR_ID_MENU_ADC_VALUE_MULT_Ifan, "Coefficient Ifan")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA, "Calibration data")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Ua, "Calibration data Ua")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Ub, "Calibration data Ub")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Uc, "Calibration data Uc")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Urot, "Calibration data Urot")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Ia, "Calibration data Ia")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Ib, "Calibration data Ib")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Ic, "Calibration data Ic")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Irot, "Calibration data Irot")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Iexc, "Calibration data Iexc")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Iref, "Calibration data Iref")
TEXT_TR(TR_ID_MENU_ADC_CALIBRATION_DATA_Ifan, "Calibration data Ifan")
TEXT_TR(TR_ID_MENU_VALUE_MULTS, "Value coefficients")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Ua, "Coefficient Ua")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Ub, "Coefficient Ub")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Uc, "Coefficient Uc")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Urot, "Coefficient Urot")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Ia, "Coefficient Ia")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Ib, "Coefficient Ib")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Ic, "Coefficient Ic")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Irot, "Coefficient Irot")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Iexc, "Coefficient Iexc")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Iref, "Coefficient Iref")
TEXT_TR(TR_ID_MENU_VALUE_MULT_Ifan, "Coefficient Ifan")
TEXT_TR(TR_ID_MENU_PHASE_SYNC, "Phases sync")
TEXT_TR(TR_ID_MENU_PHASE_SYNC_ACCURACY, "Sync accuracy")
TEXT_TR(TR_ID_MENU_PHASE_SYNC_PID, "PLL PID")
TEXT_TR(TR_ID_MENU_GUI, "GUI settings")
TEXT_TR(TR_ID_MENU_GUI_BUZZER, "Sound notification")
TEXT_TR(TR_ID_MENU_COMMANDS, "Commands")
TEXT_TR(TR_ID_MENU_CMD_START_STOP, "Start/Stop")
TEXT_TR(TR_ID_MENU_PHASES_PROT, "Phases prot")
TEXT_TR(TR_ID_MENU_PHASE_STATE_PROT, "Phases state")
TEXT_TR(TR_ID_MENU_PHASES_ANGLES_PROT, "Phases angles")
TEXT_TR(TR_ID_MENU_PHASES_SYNC_PROT, "Phases sync")
TEXT_TR(TR_ID_MENU_ROT_BREAK_PROT, "Rotor break")
TEXT_TR(TR_ID_MENU_HEATSINK_TEMP_PROT, "Heatsink overtemp")
//TEXT_TR(TR_ID_MENU_, "")
//TEXT_TR(TR_ID_ENUM_, "")
//TEXT_TR(TR_ID_, "")


TEXT_TR(TR_ID_FAN_CONTROL, "Fan control")
TEXT_TR(TR_ID_FAN_CONTROL_ENABLE, "Fan control enable")
TEXT_TR(TR_ID_FAN_TEMP_MIN, "Temp fan min")
TEXT_TR(TR_ID_FAN_TEMP_MAX, "Temp fan max")
TEXT_TR(TR_ID_FAN_RPM_MIN, "Fan rpm min")
TEXT_TR(TR_ID_FAN_ECO_MODE_ENABLE, "Eco-mode enable")
TEXT_TR(TR_ID_FAN_ECO_COOLING_TIME, "Eco-mode cooling time")
TEXT_TR(TR_ID_FAN_I_NOM, "Fan nominal current")
TEXT_TR(TR_ID_FAN_I_ZERO_NOISE, "Fan current zero noize")
TEXT_TR(TR_ID_FAN_PROT_TIME, "Fan protection activation time")
TEXT_TR(TR_ID_FAN_PROT_OVF_LEVEL, "Fan protection overflow warning level")

TEXT_TR(TR_ID_MENU_REGULATOR, "Regulator")
TEXT_TR(TR_ID_MENU_REGULATOR_MODE, "Regulator mode")
TEXT_TR(TR_ID_ENUM_REGULATOR_MODE_SPEED, "Const speed")
TEXT_TR(TR_ID_ENUM_REGULATOR_MODE_TORQUE, "Const torque")

// главный экран
TEXT_TR(TR_ID_HOME_ANCHOR_VOLTAGE, "Anchor\nvoltage")
TEXT_TR(TR_ID_HOME_ANCHOR_CURRENT, "Anchor\ncurrent")
TEXT_TR(TR_ID_HOME_EXCITATION_CURRENT, "Excitation\ncurrent")
TEXT_TR(TR_ID_HOME_HEATSINK_TEMP, "Radiator\ntemperature")
// меню
TEXT_TR(TR_ID_MENU_HEADER_TITLE, "Menu")

TEXT_TRS_END()

#endif /* TRANSLATION_EN_H */

