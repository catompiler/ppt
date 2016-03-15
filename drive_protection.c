#include "drive_protection.h"
#include <string.h>
#include "utils/utils.h"
//#include "defs/defs.h"


//! Тип структуры защиты привода.
typedef struct _Drive_Protection {
    fixed32_t U_in; //!< Номинальное напряжение, В.
    fixed32_t U_in_allow_delta; //!< Допустимое отклонение номинального напряжения, В.
    fixed32_t U_in_crit_delta; //!< Критическое отклонение номинального напряжения, В.
    fixed32_t U_zero_noise; //!< Шум напряжения нуля, В.
    fixed32_t I_zero_noise; //!< Шум тока нуля, А.
    fixed32_t I_rot_zero_noise; //!< Шум тока нуля ротора, А.
    fixed32_t I_exc; //!< Номинальный ток возбуждения.
    fixed32_t I_exc_allow_delta;
    fixed32_t I_exc_crit_delta;
    fixed32_t U_rot; //!< Номинальное напряжение возбуждения якоря.
    fixed32_t U_rot_allow_delta;
    fixed32_t U_rot_crit_delta;
    fixed32_t I_rot; //!< Номинальный ток якоря.
    fixed32_t I_rot_allow_delta;
    fixed32_t I_rot_crit_delta;
} drive_protection_t;

//! Структура защиты привода.
static drive_protection_t drive_prot;


bool drive_protection_init(void)
{
    memset(&drive_prot, 0x0, sizeof(drive_protection_t));
    
    return true;
}

bool drive_protection_is_allow(drive_pwr_check_res_t check_res)
{
    if(check_res == DRIVE_PWR_CHECK_CRIT_UNDERFLOW) return false;
    if(check_res == DRIVE_PWR_CHECK_CRIT_OVERFLOW) return false;
    return true;
}

void drive_protection_set_input_voltage(fixed32_t u_in, uint32_t allow_delta, uint32_t crit_delta)
{
    drive_prot.U_in = u_in;
    drive_prot.U_in_allow_delta = u_in * allow_delta / 100;
    drive_prot.U_in_crit_delta = u_in * crit_delta / 100;
}

void drive_protection_set_zero_voltage_noise(fixed32_t u_noize)
{
    drive_prot.U_zero_noise = u_noize;
}

void drive_protection_set_zero_current_noise(fixed32_t i_noize)
{
    drive_prot.I_zero_noise = i_noize;
}

void drive_protection_set_rot_zero_current_noise(fixed32_t i_noize)
{
    drive_prot.I_rot_zero_noise = i_noize;
}

void drive_protection_set_exc_current(fixed32_t i_exc, uint32_t allow_delta, uint32_t crit_delta)
{
    drive_prot.I_exc = i_exc;
    drive_prot.I_exc_allow_delta = i_exc * allow_delta / 100;
    drive_prot.I_exc_crit_delta = i_exc * crit_delta / 100;
}

void drive_protection_set_rot_voltage(fixed32_t u_rot, uint32_t allow_delta, uint32_t crit_delta)
{
    drive_prot.U_rot = u_rot;
    drive_prot.U_rot_allow_delta = u_rot * allow_delta / 100;
    drive_prot.U_rot_crit_delta = u_rot * crit_delta / 100;
}

void drive_protection_set_rot_current(fixed32_t i_rot, uint32_t allow_delta, uint32_t crit_delta)
{
    drive_prot.I_rot = i_rot;
    drive_prot.I_rot_allow_delta = i_rot * allow_delta / 100;
    drive_prot.I_rot_crit_delta = i_rot * crit_delta / 100;
}

static bool drive_protection_check_delta(fixed32_t ref_value, fixed32_t cur_value, fixed32_t delta)
{
    cur_value -= ref_value;
    cur_value = fixed_abs(cur_value);
    
    return cur_value <= delta;
}

static bool drive_protection_check_range(fixed32_t cur_value, fixed32_t min_val, fixed32_t max_val)
{
    if(cur_value < min_val) return false;
    if(cur_value > max_val) return false;
    return true;
}

static bool drive_protection_check_zero(fixed32_t value, fixed32_t delta)
{
    return value <= delta;
}

drive_pwr_check_res_t drive_protection_check_input_voltage(fixed32_t voltage)
{
    if(drive_protection_check_delta(drive_prot.U_in, voltage, drive_prot.U_in_allow_delta)){
        return DRIVE_PWR_CHECK_NORMAL;
    }
    if(drive_protection_check_delta(drive_prot.U_in, voltage, drive_prot.U_in_crit_delta)){
        return (voltage < drive_prot.U_in) ? DRIVE_PWR_CHECK_ALLOW_UNDERFLOW : DRIVE_PWR_CHECK_ALLOW_OVERFLOW;
    }
    return (voltage < drive_prot.U_in) ? DRIVE_PWR_CHECK_CRIT_UNDERFLOW : DRIVE_PWR_CHECK_CRIT_OVERFLOW;
}

drive_pwr_check_res_t drive_protection_check_zero_voltage(fixed32_t voltage)
{
    if(drive_protection_check_zero(voltage, drive_prot.U_zero_noise)){
        return DRIVE_PWR_CHECK_NORMAL;
    }
    return (voltage < drive_prot.U_zero_noise) ? DRIVE_PWR_CHECK_CRIT_UNDERFLOW : DRIVE_PWR_CHECK_CRIT_OVERFLOW;
}

drive_pwr_check_res_t drive_protection_check_zero_current(fixed32_t current)
{
    if(drive_protection_check_zero(current, drive_prot.I_zero_noise)){
        return DRIVE_PWR_CHECK_NORMAL;
    }
    return (current < drive_prot.I_zero_noise) ? DRIVE_PWR_CHECK_CRIT_UNDERFLOW : DRIVE_PWR_CHECK_CRIT_OVERFLOW;
}

drive_pwr_check_res_t drive_protection_check_rot_zero_current(fixed32_t current)
{
    if(drive_protection_check_zero(current, drive_prot.I_rot_zero_noise)){
        return DRIVE_PWR_CHECK_NORMAL;
    }
    return (current < drive_prot.I_rot_zero_noise) ? DRIVE_PWR_CHECK_CRIT_UNDERFLOW : DRIVE_PWR_CHECK_CRIT_OVERFLOW;
}

drive_pwr_check_res_t drive_protection_check_exc_current(fixed32_t current)
{
    if(drive_protection_check_delta(drive_prot.I_exc, current, drive_prot.I_exc_allow_delta)){
        return DRIVE_PWR_CHECK_NORMAL;
    }
    if(drive_protection_check_delta(drive_prot.I_exc, current, drive_prot.I_exc_crit_delta)){
        return (current < drive_prot.I_exc) ? DRIVE_PWR_CHECK_ALLOW_UNDERFLOW : DRIVE_PWR_CHECK_ALLOW_OVERFLOW;
    }
    return (current < drive_prot.I_exc) ? DRIVE_PWR_CHECK_CRIT_UNDERFLOW : DRIVE_PWR_CHECK_CRIT_OVERFLOW;
}

drive_pwr_check_res_t drive_protection_check_rot_voltage(fixed32_t voltage)
{
    if(drive_protection_check_range(voltage, -drive_prot.U_rot_allow_delta, drive_prot.U_rot + drive_prot.U_rot_allow_delta)){
        return DRIVE_PWR_CHECK_NORMAL;
    }
    if(drive_protection_check_range(voltage, -drive_prot.U_rot_crit_delta, drive_prot.U_rot + drive_prot.U_rot_crit_delta)){
        return (voltage < drive_prot.U_rot) ? DRIVE_PWR_CHECK_ALLOW_UNDERFLOW : DRIVE_PWR_CHECK_ALLOW_OVERFLOW;
    }
    return (voltage < drive_prot.U_rot) ? DRIVE_PWR_CHECK_CRIT_UNDERFLOW : DRIVE_PWR_CHECK_CRIT_OVERFLOW;
}

drive_pwr_check_res_t drive_protection_check_rot_current(fixed32_t current)
{
    if(drive_protection_check_range(current, -drive_prot.I_rot_allow_delta, drive_prot.I_rot + drive_prot.I_rot_allow_delta)){
        return DRIVE_PWR_CHECK_NORMAL;
    }
    if(drive_protection_check_range(current, -drive_prot.I_rot_crit_delta, drive_prot.I_rot + drive_prot.I_rot_crit_delta)){
        return (current < drive_prot.I_rot) ? DRIVE_PWR_CHECK_ALLOW_UNDERFLOW : DRIVE_PWR_CHECK_ALLOW_OVERFLOW;
    }
    return (current < drive_prot.I_rot) ? DRIVE_PWR_CHECK_CRIT_UNDERFLOW : DRIVE_PWR_CHECK_CRIT_OVERFLOW;
}