/**
 * @file parameters_list.h Список параметров.
 */

#ifndef PARAMETERS_LIST_H
#define	PARAMETERS_LIST_H

#include "parameters_ids.h"
#include "translations_ids.h"
#include "localization/localization.h"
#include "settings.h"

//! Макрос для сокращения декларации дробного значения fixed32.
#define F32(A, B) fixed32_make_from_fract(A, B)
//! Макрос для сокращения декларации целого значения fixed32.
#define F32I(I) fixed32_make_from_int(I)
//! Макрос для обозначения отсутствующей единицы измерения.
#define NOUNITS (NULL)

// Число реальных параметров.
#define PARAMETERS_REAL_COUNT 223
// Число виртуальных параметров.
#define PARAMETERS_VIRT_COUNT 19
// Общее число параметров.
#define PARAMETERS_COUNT (PARAMETERS_REAL_COUNT + PARAMETERS_VIRT_COUNT)

// Дескрипторы параметров.
PARAM_DESCRS(parameters_descrs, PARAMETERS_COUNT) {
    PARAM_DESCR(PARAM_ID_U_NOM,           PARAM_TYPE_UINT,      0,       1000,      220,        0, TEXT(TR_ID_UNITS_V)),
    PARAM_DESCR(PARAM_ID_I_NOM,           PARAM_TYPE_UINT,      0,       1000,      160,        0, TEXT(TR_ID_UNITS_A)),
    PARAM_DESCR(PARAM_ID_CALC_PHASE_CURRENT,  PARAM_TYPE_UINT,  0,          3,        0,        0, NOUNITS),
    PARAM_DESCR(PARAM_ID_U_ROT_NOM,       PARAM_TYPE_UINT,      0,       600,       400,        0, TEXT(TR_ID_UNITS_V)),
    PARAM_DESCR(PARAM_ID_I_ROT_NOM,       PARAM_TYPE_FRACT_10,  F32I(0), F32I(200), F32I(80),   0, TEXT(TR_ID_UNITS_A)),
    PARAM_DESCR(PARAM_ID_I_EXC,           PARAM_TYPE_FRACT_100, F32I(0), F32I(20),  F32I(4),    0, TEXT(TR_ID_UNITS_A)),
    
    PARAM_DESCR(PARAM_ID_EXC_PHASE,       PARAM_TYPE_UINT,      PHASE_A, PHASE_C,   PHASE_C,      0, NOUNITS),
    PARAM_DESCR(PARAM_ID_EXC_MODE,        PARAM_TYPE_UINT, DRIVE_TRIACS_EXC_FIXED, DRIVE_TRIACS_EXC_REGULATED, DRIVE_TRIACS_EXC_REGULATED, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_RAMP_START_TIME,     PARAM_TYPE_FRACT_10, RAMP_TIME_MIN, RAMP_TIME_MAX, RAMP_START_TIME_DEFAULT,     0, TEXT(TR_ID_UNITS_S)),
    PARAM_DESCR(PARAM_ID_RAMP_STOP_TIME,      PARAM_TYPE_FRACT_10, RAMP_TIME_MIN, RAMP_TIME_MAX, RAMP_STOP_TIME_DEFAULT,      0, TEXT(TR_ID_UNITS_S)),
    PARAM_DESCR(PARAM_ID_RAMP_FAST_STOP_TIME, PARAM_TYPE_FRACT_10, RAMP_TIME_MIN, RAMP_TIME_MAX, RAMP_FAST_STOP_TIME_DEFAULT, 0, TEXT(TR_ID_UNITS_S)),
    PARAM_DESCR(PARAM_ID_RAMP_REFERENCE_TIME, PARAM_TYPE_FRACT_10, RAMP_TIME_MIN, RAMP_TIME_MAX, RAMP_REFERENCE_TIME_DEFAULT, 0, TEXT(TR_ID_UNITS_S)),
    PARAM_DESCR(PARAM_ID_RAMP_STOP_MODE,      PARAM_TYPE_UINT,        0,        2,        0,        0,  NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PHASE_SYNC_ACCURACY,    PARAM_TYPE_FRACT_10,  F32I(0), F32I(10),  F32I(3),     0,  TEXT(TR_ID_UNITS_DEGREE)),
    PARAM_DESCR(PARAM_ID_PHASE_SYNC_PLL_PID_K_P, PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32(15, 1),  0, NOUNITS), // Kp.
    PARAM_DESCR(PARAM_ID_PHASE_SYNC_PLL_PID_K_I, PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32(15, 10), 0, NOUNITS), // Ki.
    PARAM_DESCR(PARAM_ID_PHASE_SYNC_PLL_PID_K_D, PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32I(0),     0, NOUNITS), // Kd.
    
    PARAM_DESCR(PARAM_ID_ROT_PID_K_P,     PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32(25, 100), 0, NOUNITS), // Kp.
    PARAM_DESCR(PARAM_ID_ROT_PID_K_I,     PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32(7, 100),  0, NOUNITS), // Ki.
    PARAM_DESCR(PARAM_ID_ROT_PID_K_D,     PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32I(0),      0, NOUNITS), // Kd.
    PARAM_DESCR(PARAM_ID_EXC_PID_K_P,     PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32(25, 1),   0, NOUNITS), // Kp.
    PARAM_DESCR(PARAM_ID_EXC_PID_K_I,     PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32(10, 1),   0, NOUNITS), // Ki.
    PARAM_DESCR(PARAM_ID_EXC_PID_K_D,     PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32I(0),      0, NOUNITS), // Kd.
    
    PARAM_DESCR(PARAM_ID_ROT_STOP_TIME,   PARAM_TYPE_UINT,      0,       100,       10,           0, TEXT(TR_ID_UNITS_S)),
    PARAM_DESCR(PARAM_ID_EXC_STOP_TIME,   PARAM_TYPE_UINT,      0,       100,       10,           0, TEXT(TR_ID_UNITS_S)),
    PARAM_DESCR(PARAM_ID_EXC_START_TIME,  PARAM_TYPE_UINT,      0,       100,       10,           0, TEXT(TR_ID_UNITS_S)),
    
    PARAM_DESCR(PARAM_ID_PHASES_CHECK_TIME,  PARAM_TYPE_UINT,    0,      1000,      100,          0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_ZERO_SENSOR_TIME,   PARAM_TYPE_UINT,    0,      1000,      100,          0, TEXT(TR_ID_UNITS_US)),
    
    PARAM_DESCR(PARAM_ID_TRIACS_PAIRS_OPEN_TIME,   PARAM_TYPE_UINT,  0,  1000,   200,   0, TEXT(TR_ID_UNITS_US)),
    PARAM_DESCR(PARAM_ID_TRIAC_EXC_OPEN_TIME,      PARAM_TYPE_UINT,  0,  1000,   200,   0, TEXT(TR_ID_UNITS_US)),
    
    
    PARAM_DESCR(PARAM_ID_THERMAL_OVERLOAD_PROT_ENABLE,  PARAM_TYPE_UINT,      0,         1,       1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_THERMAL_OVERLOAD_PROT_TIME_6I, PARAM_TYPE_FRACT_100, 0, F32I(100), F32I(5), 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_THERMAL_OVERLOAD_PROT_ACTION,  PARAM_TYPE_UINT,      0,         4,       4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_EMERGENCY_STOP_ACTION,         PARAM_TYPE_UINT,      0,         4,       4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_PHASES_TIME_ENABLED,      PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_TIME_TIME_MS,      PARAM_TYPE_UINT, 0, 60000,  40, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_TIME_ACTION,       PARAM_TYPE_UINT, 0,     4,   4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_PHASES_ANGLES_FAULT_ENABLED,      PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_ANGLES_FAULT_VALUE,        PARAM_TYPE_UINT, 0,   120,   6, 0, TEXT(TR_ID_UNITS_DEGREE)),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_ANGLES_FAULT_TIME_MS,      PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_ANGLES_FAULT_LATCH_ENABLE, PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_ANGLES_FAULT_ACTION,       PARAM_TYPE_UINT, 0,     4,   4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_PHASES_ANGLES_WARN_ENABLED,       PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_ANGLES_WARN_VALUE,         PARAM_TYPE_UINT, 0,   120,   3, 0, TEXT(TR_ID_UNITS_DEGREE)),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_ANGLES_WARN_TIME_MS,       PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_ANGLES_WARN_LATCH_ENABLE,  PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_ANGLES_WARN_ACTION,        PARAM_TYPE_UINT, 0,     4,   1, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_PHASES_SYNC_FAULT_ENABLED,      PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_SYNC_FAULT_TIME_MS,      PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_SYNC_FAULT_LATCH_ENABLE, PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_SYNC_FAULT_ACTION,       PARAM_TYPE_UINT, 0,     4,   4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_PHASES_SYNC_WARN_ENABLED,       PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_SYNC_WARN_TIME_MS,       PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_SYNC_WARN_LATCH_ENABLE,  PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_PHASES_SYNC_WARN_ACTION,        PARAM_TYPE_UINT, 0,     4,   1, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_U_IN_CUTOFF_LEVEL_VALUE,      PARAM_TYPE_UINT, 0,  1000,  20, 0, TEXT(TR_ID_UNITS_PERCENT)),
    
    PARAM_DESCR(PARAM_ID_PROT_U_IN_OVF_FAULT_ENABLE,        PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_VALUE,   PARAM_TYPE_UINT, 0,  1000,  10, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_TIME_MS, PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_OVF_FAULT_LATCH_ENABLE,  PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_OVF_FAULT_ACTION,        PARAM_TYPE_UINT, 0,     4,   4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_U_IN_UDF_FAULT_ENABLE,        PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_VALUE,   PARAM_TYPE_UINT, 0,  1000,  10, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_TIME_MS, PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_UDF_FAULT_LATCH_ENABLE,  PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_UDF_FAULT_ACTION,        PARAM_TYPE_UINT, 0,     4,   4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_U_IN_OVF_WARN_ENABLE,         PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_VALUE,    PARAM_TYPE_UINT, 0,  1000,   5, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_TIME_MS,  PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_OVF_WARN_LATCH_ENABLE,   PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_OVF_WARN_ACTION,         PARAM_TYPE_UINT, 0,     4,   1, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_U_IN_UDF_WARN_ENABLE,         PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_VALUE,    PARAM_TYPE_UINT, 0,  1000,   5, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_TIME_MS,  PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_UDF_WARN_LATCH_ENABLE,   PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_IN_UDF_WARN_ACTION,         PARAM_TYPE_UINT, 0,     4,   1, 0, NOUNITS),
    
    
    PARAM_DESCR(PARAM_ID_PROT_I_IN_CUTOFF_LEVEL_VALUE,      PARAM_TYPE_UINT, 0,  1000,  20, 0, TEXT(TR_ID_UNITS_PERCENT)),
    
    PARAM_DESCR(PARAM_ID_PROT_I_IN_OVF_FAULT_ENABLE,        PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_VALUE,   PARAM_TYPE_UINT, 0,  1000,  10, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_TIME_MS, PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_OVF_FAULT_LATCH_ENABLE,  PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_OVF_FAULT_ACTION,        PARAM_TYPE_UINT, 0,     4,   4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_I_IN_OVF_WARN_ENABLE,         PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_VALUE,    PARAM_TYPE_UINT, 0,  1000,   5, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_TIME_MS,  PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_OVF_WARN_LATCH_ENABLE,   PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_OVF_WARN_ACTION,         PARAM_TYPE_UINT, 0,     4,   1, 0, NOUNITS),
    
    
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_CUTOFF_LEVEL_VALUE,      PARAM_TYPE_UINT, 0,  1000,  20, 0, TEXT(TR_ID_UNITS_PERCENT)),
    
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_OVF_FAULT_ENABLE,        PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_VALUE,   PARAM_TYPE_UINT, 0,  1000,  10, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_TIME_MS, PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_OVF_FAULT_LATCH_ENABLE,  PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_OVF_FAULT_ACTION,        PARAM_TYPE_UINT, 0,     4,   4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_OVF_WARN_ENABLE,         PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_VALUE,    PARAM_TYPE_UINT, 0,  1000,   5, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_TIME_MS,  PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_OVF_WARN_LATCH_ENABLE,   PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_OVF_WARN_ACTION,         PARAM_TYPE_UINT, 0,     4,   1, 0, NOUNITS),
    
    
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_CUTOFF_LEVEL_VALUE,      PARAM_TYPE_UINT, 0,  1000,  20, 0, TEXT(TR_ID_UNITS_PERCENT)),
    
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_OVF_FAULT_ENABLE,        PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_VALUE,   PARAM_TYPE_UINT, 0,  1000,  10, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_TIME_MS, PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_OVF_FAULT_LATCH_ENABLE,  PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_OVF_FAULT_ACTION,        PARAM_TYPE_UINT, 0,     4,   4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_OVF_WARN_ENABLE,         PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_VALUE,    PARAM_TYPE_UINT, 0,  1000,   5, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_TIME_MS,  PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_OVF_WARN_LATCH_ENABLE,   PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_OVF_WARN_ACTION,         PARAM_TYPE_UINT, 0,     4,   1, 0, NOUNITS),
    
    
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_CUTOFF_LEVEL_VALUE,      PARAM_TYPE_UINT, 0,  1000,  20, 0, TEXT(TR_ID_UNITS_PERCENT)),
    
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_OVF_FAULT_ENABLE,        PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_VALUE,   PARAM_TYPE_UINT, 0,  1000,  10, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_TIME_MS, PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_OVF_FAULT_LATCH_ENABLE,  PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_OVF_FAULT_ACTION,        PARAM_TYPE_UINT, 0,     4,   4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_UDF_FAULT_ENABLE,        PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_VALUE,   PARAM_TYPE_UINT, 0,  1000,  10, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_TIME_MS, PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_UDF_FAULT_LATCH_ENABLE,  PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_UDF_FAULT_ACTION,        PARAM_TYPE_UINT, 0,     4,   4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_OVF_WARN_ENABLE,         PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_VALUE,    PARAM_TYPE_UINT, 0,  1000,   5, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_TIME_MS,  PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_OVF_WARN_LATCH_ENABLE,   PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_OVF_WARN_ACTION,         PARAM_TYPE_UINT, 0,     4,   1, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_UDF_WARN_ENABLE,         PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_VALUE,    PARAM_TYPE_UINT, 0,  1000,   5, 0, TEXT(TR_ID_UNITS_PERCENT)),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_TIME_MS,  PARAM_TYPE_UINT, 0, 60000, 100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_UDF_WARN_LATCH_ENABLE,   PARAM_TYPE_UINT, 0,     1,   1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_UDF_WARN_ACTION,         PARAM_TYPE_UINT, 0,     4,   1, 0, NOUNITS),
    
    
    PARAM_DESCR(PARAM_ID_PROT_I_IN_IDLE_FAULT_ENABLE,        PARAM_TYPE_UINT,      0,         1,         1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_VALUE,   PARAM_TYPE_FRACT_100, 0, F32I(100),  F32I(10), 0, TEXT(TR_ID_UNITS_A)),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_TIME_MS, PARAM_TYPE_UINT,      0,     60000,       100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_IDLE_FAULT_LATCH_ENABLE,  PARAM_TYPE_UINT,      0,         1,         1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_IDLE_FAULT_ACTION,        PARAM_TYPE_UINT,      0,         4,         4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_I_IN_IDLE_WARN_ENABLE,         PARAM_TYPE_UINT,      0,         1,         1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_VALUE,    PARAM_TYPE_FRACT_100, 0, F32I(100),   F32I(5), 0, TEXT(TR_ID_UNITS_A)),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_TIME_MS,  PARAM_TYPE_UINT,      0,     60000,       100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_IDLE_WARN_LATCH_ENABLE,   PARAM_TYPE_UINT,      0,         1,         1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_IN_IDLE_WARN_ACTION,         PARAM_TYPE_UINT,      0,         4,         1, 0, NOUNITS),
    
    
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_IDLE_FAULT_ENABLE,        PARAM_TYPE_UINT,      0,         1,        1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_VALUE,   PARAM_TYPE_FRACT_100, 0, F32I(100), F32I(10), 0, TEXT(TR_ID_UNITS_V)),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_TIME_MS, PARAM_TYPE_UINT,      0,     60000,      100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_IDLE_FAULT_LATCH_ENABLE,  PARAM_TYPE_UINT,      0,         1,        1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_IDLE_FAULT_ACTION,        PARAM_TYPE_UINT,      0,         4,        4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_IDLE_WARN_ENABLE,         PARAM_TYPE_UINT,      0,         1,        1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_VALUE,    PARAM_TYPE_FRACT_100, 0, F32I(100),  F32I(5), 0, TEXT(TR_ID_UNITS_V)),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_TIME_MS,  PARAM_TYPE_UINT,      0,     60000,      100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_IDLE_WARN_LATCH_ENABLE,   PARAM_TYPE_UINT,      0,         1,        1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_U_ROT_IDLE_WARN_ACTION,         PARAM_TYPE_UINT,      0,         4,        1, 0, NOUNITS),
    
    
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_IDLE_FAULT_ENABLE,        PARAM_TYPE_UINT,      0,         1,        1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_VALUE,   PARAM_TYPE_FRACT_100, 0, F32I(100), F32I(10), 0, TEXT(TR_ID_UNITS_A)),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_TIME_MS, PARAM_TYPE_UINT,      0,     60000,      100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_IDLE_FAULT_LATCH_ENABLE,  PARAM_TYPE_UINT,      0,         1,        1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_IDLE_FAULT_ACTION,        PARAM_TYPE_UINT,      0,         4,        4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_IDLE_WARN_ENABLE,         PARAM_TYPE_UINT,      0,         1,        1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_VALUE,    PARAM_TYPE_FRACT_100, 0, F32I(100),  F32I(5), 0, TEXT(TR_ID_UNITS_A)),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_TIME_MS,  PARAM_TYPE_UINT,      0,     60000,      100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_IDLE_WARN_LATCH_ENABLE,   PARAM_TYPE_UINT,      0,         1,        1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_ROT_IDLE_WARN_ACTION,         PARAM_TYPE_UINT,      0,         4,        1, 0, NOUNITS),
    
    
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_IDLE_FAULT_ENABLE,        PARAM_TYPE_UINT,      0,         1,        1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_VALUE,   PARAM_TYPE_FRACT_100, 0, F32I(100),  F32I(1), 0, TEXT(TR_ID_UNITS_A)),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_TIME_MS, PARAM_TYPE_UINT,      0,     60000,      100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_IDLE_FAULT_LATCH_ENABLE,  PARAM_TYPE_UINT,      0,         1,        1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_IDLE_FAULT_ACTION,        PARAM_TYPE_UINT,      0,         4,        4, 0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_IDLE_WARN_ENABLE,         PARAM_TYPE_UINT,      0,         1,        1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_VALUE,    PARAM_TYPE_FRACT_100, 0, F32I(100),F32(5,10), 0, TEXT(TR_ID_UNITS_A)),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_TIME_MS,  PARAM_TYPE_UINT,      0,     60000,      100, 0, TEXT(TR_ID_UNITS_MS)),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_IDLE_WARN_LATCH_ENABLE,   PARAM_TYPE_UINT,      0,         1,        1, 0, NOUNITS),
    PARAM_DESCR(PARAM_ID_PROT_I_EXC_IDLE_WARN_ACTION,         PARAM_TYPE_UINT,      0,         4,        1, 0, NOUNITS),
    
    
    PARAM_DESCR(PARAM_ID_ADC_VALUE_MULTIPLIER_Ua,    PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_VALUE_MULTIPLIER_Ub,    PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_VALUE_MULTIPLIER_Uc,    PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_VALUE_MULTIPLIER_Urot,  PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_VALUE_MULTIPLIER_Ia,    PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_VALUE_MULTIPLIER_Ib,    PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_VALUE_MULTIPLIER_Ic,    PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_VALUE_MULTIPLIER_Irot,  PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_VALUE_MULTIPLIER_Iexc,  PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_VALUE_MULTIPLIER_Iref,  PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_VALUE_MULTIPLIER_Ifan,  PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_ADC_CALIBRATION_DATA_Ua,    PARAM_TYPE_UINT,  0,    4096,     2048,    0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_CALIBRATION_DATA_Ub,    PARAM_TYPE_UINT,  0,    4096,     2048,    0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_CALIBRATION_DATA_Uc,    PARAM_TYPE_UINT,  0,    4096,     2048,    0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_CALIBRATION_DATA_Urot,  PARAM_TYPE_UINT,  0,    4096,     2048,    0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_CALIBRATION_DATA_Ia,    PARAM_TYPE_UINT,  0,    4096,     2048,    0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_CALIBRATION_DATA_Ib,    PARAM_TYPE_UINT,  0,    4096,     2048,    0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_CALIBRATION_DATA_Ic,    PARAM_TYPE_UINT,  0,    4096,     2048,    0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_CALIBRATION_DATA_Irot,  PARAM_TYPE_UINT,  0,    4096,     2048,    0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_CALIBRATION_DATA_Iexc,  PARAM_TYPE_UINT,  0,    4096,     2048,    0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_CALIBRATION_DATA_Iref,  PARAM_TYPE_UINT,  0,    4096,     2048,    0, NOUNITS),
    PARAM_DESCR(PARAM_ID_ADC_CALIBRATION_DATA_Ifan,  PARAM_TYPE_UINT,  0,    4096,     2048,    0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_VALUE_MULTIPLIER_Ua,    PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_VALUE_MULTIPLIER_Ub,    PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_VALUE_MULTIPLIER_Uc,    PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_VALUE_MULTIPLIER_Urot,  PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_VALUE_MULTIPLIER_Ia,    PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_VALUE_MULTIPLIER_Ib,    PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_VALUE_MULTIPLIER_Ic,    PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_VALUE_MULTIPLIER_Irot,  PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_VALUE_MULTIPLIER_Iexc,  PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_VALUE_MULTIPLIER_Iref,  PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    PARAM_DESCR(PARAM_ID_VALUE_MULTIPLIER_Ifan,  PARAM_TYPE_FRACT_10000,  0,  F32I(2),  F32I(1),  0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_DIGITAL_IN_1_TYPE,      PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_IN_1_INVERSION, PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_IN_2_TYPE,      PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_IN_2_INVERSION, PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_IN_3_TYPE,      PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_IN_3_INVERSION, PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_IN_4_TYPE,      PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_IN_4_INVERSION, PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_IN_5_TYPE,      PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_IN_5_INVERSION, PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_DIGITAL_OUT_1_TYPE,      PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_OUT_1_INVERSION, PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_OUT_2_TYPE,      PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_OUT_2_INVERSION, PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_OUT_3_TYPE,      PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_OUT_3_INVERSION, PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_OUT_4_TYPE,      PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    PARAM_DESCR(PARAM_ID_DIGITAL_OUT_4_INVERSION, PARAM_TYPE_UINT,  0,    6,    0,   0, NOUNITS),
    
    PARAM_DESCR(PARAM_ID_DIGITAL_IO_DEADTIME_MS,  PARAM_TYPE_UINT,  0, 1000,  100,   0, TEXT(TR_ID_UNITS_MS)),
    
    PARAM_DESCR(PARAM_ID_MODBUS_BAUD,     PARAM_TYPE_UINT,  1200,  57600,  9600,  0, NOUNITS),
    
    // Отладка.
    PARAM_DESCR(PARAM_ID_DEBUG_0, PARAM_TYPE_INT,        0, 0, 0, PARAM_FLAG_VIRTUAL, NOUNITS),
    PARAM_DESCR(PARAM_ID_DEBUG_1, PARAM_TYPE_INT,        0, 0, 0, PARAM_FLAG_VIRTUAL, NOUNITS),
    PARAM_DESCR(PARAM_ID_DEBUG_2, PARAM_TYPE_UINT,       0, 0, 0, PARAM_FLAG_VIRTUAL, NOUNITS),
    PARAM_DESCR(PARAM_ID_DEBUG_3, PARAM_TYPE_UINT,       0, 0, 0, PARAM_FLAG_VIRTUAL, NOUNITS),
    PARAM_DESCR(PARAM_ID_DEBUG_4, PARAM_TYPE_FRACT_10,   0, 0, 0, PARAM_FLAG_VIRTUAL, NOUNITS),
    PARAM_DESCR(PARAM_ID_DEBUG_5, PARAM_TYPE_FRACT_10,   0, 0, 0, PARAM_FLAG_VIRTUAL, NOUNITS),
    PARAM_DESCR(PARAM_ID_DEBUG_6, PARAM_TYPE_FRACT_100,  0, 0, 0, PARAM_FLAG_VIRTUAL, NOUNITS),
    PARAM_DESCR(PARAM_ID_DEBUG_7, PARAM_TYPE_FRACT_100,  0, 0, 0, PARAM_FLAG_VIRTUAL, NOUNITS),
    PARAM_DESCR(PARAM_ID_DEBUG_8, PARAM_TYPE_FRACT_1000, 0, 0, 0, PARAM_FLAG_VIRTUAL, NOUNITS),
    PARAM_DESCR(PARAM_ID_DEBUG_9, PARAM_TYPE_FRACT_1000, 0, 0, 0, PARAM_FLAG_VIRTUAL, NOUNITS),
    
    // Питание.
    PARAM_DESCR(PARAM_ID_POWER_U_A,   PARAM_TYPE_FRACT_10,  0, 0, 0, PARAM_FLAG_VIRTUAL, TEXT(TR_ID_UNITS_V)),
    PARAM_DESCR(PARAM_ID_POWER_U_B,   PARAM_TYPE_FRACT_10,  0, 0, 0, PARAM_FLAG_VIRTUAL, TEXT(TR_ID_UNITS_V)),
    PARAM_DESCR(PARAM_ID_POWER_U_C,   PARAM_TYPE_FRACT_10,  0, 0, 0, PARAM_FLAG_VIRTUAL, TEXT(TR_ID_UNITS_V)),
    PARAM_DESCR(PARAM_ID_POWER_U_ROT, PARAM_TYPE_FRACT_10,   0, 0, 0, PARAM_FLAG_VIRTUAL, TEXT(TR_ID_UNITS_V)),
    PARAM_DESCR(PARAM_ID_POWER_I_A,   PARAM_TYPE_FRACT_100,  0, 0, 0, PARAM_FLAG_VIRTUAL, TEXT(TR_ID_UNITS_A)),
    PARAM_DESCR(PARAM_ID_POWER_I_B,   PARAM_TYPE_FRACT_100,  0, 0, 0, PARAM_FLAG_VIRTUAL, TEXT(TR_ID_UNITS_A)),
    PARAM_DESCR(PARAM_ID_POWER_I_C,   PARAM_TYPE_FRACT_100,  0, 0, 0, PARAM_FLAG_VIRTUAL, TEXT(TR_ID_UNITS_A)),
    PARAM_DESCR(PARAM_ID_POWER_I_ROT, PARAM_TYPE_FRACT_10,   0, 0, 0, PARAM_FLAG_VIRTUAL, TEXT(TR_ID_UNITS_A)),
    PARAM_DESCR(PARAM_ID_POWER_I_EXC, PARAM_TYPE_FRACT_1000, 0, 0, 0, PARAM_FLAG_VIRTUAL, TEXT(TR_ID_UNITS_A)),
};

#undef NOUNITS
#undef F32I
#undef F32

#endif	/* PARAMETERS_LIST_H */
