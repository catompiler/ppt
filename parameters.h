/**
 * @file parameters.h Список параметров.
 */

#ifndef PARAMETERS_H
#define	PARAMETERS_H

#include "settings.h"
#include "drive_phase_state.h"
#include "drive_triacs.h"
#include "ramp.h"



// Число реальных параметров.
#define PARAMETERS_REAL_COUNT 29
// Число виртуальных параметров.
#define PARAMETERS_VIRT_COUNT 0
// Общее число параметров.
#define PARAMETERS_COUNT (PARAMETERS_REAL_COUNT + PARAMETERS_VIRT_COUNT)

//! Макрос для сокращения декларации дробного значения fixed32.
#define F32(A, B) fixed32_make_from_fract(A, B)
//! Макрос для сокращения декларации целого значения fixed32.
#define F32I(I) fixed32_make_from_int(I)

// Дескрипторы параметров.
PARAM_DESCRS(parameters_descrs, PARAMETERS_COUNT) {
    PARAM_DESCR(PARAM_ID_U_NOM,           PARAM_TYPE_UINT,      0,       1000,      220,        0),
    PARAM_DESCR(PARAM_ID_U_NOM_ALLOW_VAR, PARAM_TYPE_UINT,      0,       10,        5,          0), // %
    PARAM_DESCR(PARAM_ID_U_NOM_CRIT_VAR,  PARAM_TYPE_UINT,      0,       20,        10,         0), // %
    PARAM_DESCR(PARAM_ID_U_ZERO_NOISE,    PARAM_TYPE_FRACT_100, F32I(0), F32I(10),  F32I(10),   0),
    PARAM_DESCR(PARAM_ID_I_ZERO_NOISE,    PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32(1, 10), 0),
    PARAM_DESCR(PARAM_ID_I_ROT_ZERO_NOISE,PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32I(2),    0),
    PARAM_DESCR(PARAM_ID_U_ROT_NOM,       PARAM_TYPE_UINT,      0,       600,       400,        0),
    PARAM_DESCR(PARAM_ID_U_ROT_ALLOW_VAR, PARAM_TYPE_UINT,      0,       10,        2,          0), // %
    PARAM_DESCR(PARAM_ID_U_ROT_CRIT_VAR,  PARAM_TYPE_UINT,      0,       20,        5,          0), // %
    PARAM_DESCR(PARAM_ID_I_ROT_NOM,       PARAM_TYPE_FRACT_10,  F32I(0), F32I(200), F32I(10),   0),
    PARAM_DESCR(PARAM_ID_I_ROT_ALLOW_VAR, PARAM_TYPE_UINT,      0,       100,       5,          0), // %
    PARAM_DESCR(PARAM_ID_I_ROT_CRIT_VAR,  PARAM_TYPE_UINT,      0,       100,       10,         0), // %
    PARAM_DESCR(PARAM_ID_I_EXC,           PARAM_TYPE_FRACT_10,  F32I(0), F32I(20),  F32(65,100),    0), //F32I(8)
    PARAM_DESCR(PARAM_ID_I_EXC_ALLOW_VAR, PARAM_TYPE_UINT,      0,       100,       10,         0), // %
    PARAM_DESCR(PARAM_ID_I_EXC_CRIT_VAR,  PARAM_TYPE_UINT,      0,       100,       20,         0), // %
    
    PARAM_DESCR(PARAM_ID_EXC_PHASE,       PARAM_TYPE_UINT,      PHASE_A, PHASE_C,   PHASE_B,      0),
    PARAM_DESCR(PARAM_ID_EXC_MODE,        PARAM_TYPE_UINT, DRIVE_TRIACS_EXC_FIXED, DRIVE_TRIACS_EXC_REGULATED, DRIVE_TRIACS_EXC_FIXED, 0),
            
    PARAM_DESCR(PARAM_ID_RAMP_TIME,       PARAM_TYPE_UINT, RAMP_TIME_MIN, RAMP_TIME_MAX, RAMP_TIME_DEFAULT, 0),
    
    PARAM_DESCR(PARAM_ID_ROT_PID_K_P,     PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32(1, 10),   0), // Kp.
    PARAM_DESCR(PARAM_ID_ROT_PID_K_I,     PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32(1, 10),   0), // Ki.
    PARAM_DESCR(PARAM_ID_ROT_PID_K_D,     PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32I(0),      0), // Kd.
    PARAM_DESCR(PARAM_ID_EXC_PID_K_P,     PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32(10, 1),   0), // Kp.
    PARAM_DESCR(PARAM_ID_EXC_PID_K_I,     PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32(10, 1),   0), // Ki.
    PARAM_DESCR(PARAM_ID_EXC_PID_K_D,     PARAM_TYPE_FRACT_100, F32I(0), F32I(100), F32I(0),      0), // Kd.
    
    PARAM_DESCR(PARAM_ID_ROT_STOP_TIME,   PARAM_TYPE_UINT,      0,       100,       10,           0),
    PARAM_DESCR(PARAM_ID_EXC_STOP_TIME,   PARAM_TYPE_UINT,      0,       100,       5,            0),
    PARAM_DESCR(PARAM_ID_EXC_START_TIME,  PARAM_TYPE_UINT,      0,       100,       5,            0),
    
    PARAM_DESCR(PARAM_ID_TRIACS_PAIRS_OPEN_TIME,   PARAM_TYPE_UINT,      0,         1000,         100,     0),
    PARAM_DESCR(PARAM_ID_TRIAC_EXC_OPEN_TIME,      PARAM_TYPE_UINT,      0,         1000,         200,     0),
};

#undef F32I
#undef F32

#endif	/* PARAMETERS_H */
