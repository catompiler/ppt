/**
 * @file parameters.h Список параметров.
 */

#ifndef PARAMETERS_H
#define	PARAMETERS_H

#include "settings.h"



// Число реальных параметров.
#define PARAMETERS_REAL_COUNT 6
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
    PARAM_DESCR(PARAM_ID_U_NOM,           PARAM_TYPE_UINT, 0, 1000, 220, 0),
    PARAM_DESCR(PARAM_ID_U_NOM_ALLOW_VAR, PARAM_TYPE_UINT, 0, 10,   5,   0), // %
    PARAM_DESCR(PARAM_ID_U_NOM_CRIT_VAR,  PARAM_TYPE_UINT, 0, 20,   10,  0), // %
    PARAM_DESCR(PARAM_ID_U_ZERO_NOISE,    PARAM_TYPE_FRACT_1000, F32I(0), F32I(10), F32I(1),      0), // 1.0 В.
    PARAM_DESCR(PARAM_ID_I_ZERO_NOISE,    PARAM_TYPE_FRACT_1000, F32I(0), F32I(1),  F32(1, 100),  0), // 0.01 А.
    PARAM_DESCR(PARAM_ID_EXC_PHASE,       PARAM_TYPE_UINT, PHASE_A, PHASE_C, PHASE_A, 0)
};

#undef F32I
#undef F32

#endif	/* PARAMETERS_H */
