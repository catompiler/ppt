#include "drive_protection.h"
#include <string.h>
#include "utils/utils.h"
#include "settings.h"
//#include "defs/defs.h"

//! Перегруз по току, при котором задано время срабатывания.
#define DRIVE_TOP_NOMINAL_OVERCURRENT 6

//! Структура тепловой защиты.
typedef struct _Drive_TOP {
    fixed32_t k_pie_max; //!< Коэффициентв зависимости времени срабатывания от перегрузки по току.
    fixed32_t cur_pie; //!< Текущее тепло двигателя.
    bool overheat; //!< Флаг перегрева.
} drive_top_t;

//! Тип структуры защиты привода.
typedef struct _Drive_Protection {
    fixed32_t U_in; //!< Номинальное напряжение, В.
    fixed32_t U_in_allow_delta; //!< Допустимое отклонение номинального напряжения, В.
    fixed32_t U_in_crit_delta; //!< Критическое отклонение номинального напряжения, В.
    fixed32_t I_in_cutoff; //!< Токовая отсечка по фазам.
    fixed32_t U_zero_noise; //!< Шум напряжения нуля, В.
    fixed32_t I_zero_noise; //!< Шум тока нуля, А.
    fixed32_t I_rot_zero_noise; //!< Шум нуля тока ротора, А.
    fixed32_t I_exc_zero_noise; //!< Шум нуля тока возбуждения, А.
    fixed32_t I_exc; //!< Номинальный ток возбуждения.
    fixed32_t I_exc_allow_delta;
    fixed32_t I_exc_crit_delta;
    fixed32_t U_rot; //!< Номинальное напряжение возбуждения якоря.
    fixed32_t U_rot_allow_delta;
    fixed32_t U_rot_crit_delta;
    fixed32_t I_rot; //!< Номинальный ток якоря.
    fixed32_t I_rot_cutoff;
    fixed32_t I_rot_allow_delta;
    fixed32_t I_rot_crit_delta;
    drive_top_t top; //!< Тепловая защита.
} drive_protection_t;

//! Структура защиты привода.
static drive_protection_t drive_prot;


bool drive_protection_init(void)
{
    memset(&drive_prot, 0x0, sizeof(drive_protection_t));
    
    return true;
}

bool drive_protection_init_top(fixed32_t t6)
{
    drive_prot.top.k_pie_max = (DRIVE_TOP_NOMINAL_OVERCURRENT * DRIVE_TOP_NOMINAL_OVERCURRENT - 1) * t6;
    //drive_prot.top.cur_pie = 0;
    
    return true;
}

void drive_protection_top_process(fixed32_t I_rot, fixed32_t dt)
{
    if(I_rot > drive_prot.I_rot){
        // Q_heat = (i^2 - 1) * dt
        fixed32_t i = fixed32_div((int64_t)I_rot, drive_prot.I_rot);
        fixed32_t q = fixed32_mul((int64_t)i, i);
                  q = q - fixed32_make_from_int(1);
                  q = fixed32_mul((int64_t)q, dt);
        // Интегрируем нагрев.
        drive_prot.top.cur_pie += q;
        // Клампим.
        if(drive_prot.top.cur_pie >= drive_prot.top.k_pie_max){
            drive_prot.top.cur_pie = drive_prot.top.k_pie_max;
            drive_prot.top.overheat = true;
        }
    }else if(drive_prot.top.cur_pie > 0){
        // Q_cool = (pie / max_pie + 0.58) * dt
        fixed32_t q = drive_prot.top.cur_pie;
                  q = fixed32_div((int64_t)q, drive_prot.top.k_pie_max);
                  q += fixed32_make_from_fract(58, 100);
                  q = fixed32_mul((int64_t)q, dt);
        // Интегрируем охлаждение.
        drive_prot.top.cur_pie -= q;
        // Клампим.
        if(drive_prot.top.cur_pie <= 0){
            drive_prot.top.cur_pie = 0;
            drive_prot.top.overheat = false;
        }
    }
}

drive_top_check_res_t drive_protection_top_check(void)
{
    if(drive_prot.top.cur_pie == 0) return DRIVE_TOP_CHECK_NORMAL;
    if(drive_prot.top.cur_pie == drive_prot.top.k_pie_max) return DRIVE_TOP_CHECK_OVERHEAT;
    if(drive_prot.top.overheat) return DRIVE_TOP_CHECK_COOLING;
    return DRIVE_TOP_CHECK_HEATING;
}

bool drive_protection_top_ready(void)
{
    return !drive_prot.top.overheat;
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

void drive_protection_set_phases_current_cutoff(fixed32_t i_in_cutoff)
{
    drive_prot.I_in_cutoff = i_in_cutoff;
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

void drive_protection_set_exc_zero_current_noise(fixed32_t i_noize)
{
    drive_prot.I_exc_zero_noise = i_noize;
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

void drive_protection_set_rot_current(fixed32_t i_rot, uint32_t allow_delta, uint32_t crit_delta, uint32_t cutoff_mult)
{
    drive_prot.I_rot = i_rot;
    drive_prot.I_rot_allow_delta = i_rot * allow_delta / 100;
    drive_prot.I_rot_crit_delta = i_rot * crit_delta / 100;
    drive_prot.I_rot_cutoff = i_rot * cutoff_mult;
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

drive_pwr_check_res_t drive_protection_check_input_current(fixed32_t current)
{
    current = fixed_abs(current);
    
    if(current >= drive_prot.I_in_cutoff) return DRIVE_PWR_CHECK_CRIT_OVERFLOW;
    
    return DRIVE_PWR_CHECK_NORMAL;
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

drive_pwr_check_res_t drive_protection_check_exc_zero_current(fixed32_t current)
{
    if(drive_protection_check_zero(current, drive_prot.I_exc_zero_noise)){
        return DRIVE_PWR_CHECK_NORMAL;
    }
    return (current < drive_prot.I_exc_zero_noise) ? DRIVE_PWR_CHECK_CRIT_UNDERFLOW : DRIVE_PWR_CHECK_CRIT_OVERFLOW;
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
    if(drive_protection_check_range(current, -drive_prot.I_rot_allow_delta, drive_prot.I_rot_cutoff)){
        return DRIVE_PWR_CHECK_NORMAL;
    }
    if(drive_protection_check_range(current, -drive_prot.I_rot_crit_delta, drive_prot.I_rot_cutoff)){
        return (current < drive_prot.I_rot) ? DRIVE_PWR_CHECK_ALLOW_UNDERFLOW : DRIVE_PWR_CHECK_ALLOW_OVERFLOW;
    }
    return (current < drive_prot.I_rot) ? DRIVE_PWR_CHECK_CRIT_UNDERFLOW : DRIVE_PWR_CHECK_CRIT_OVERFLOW;
}

drive_break_check_res_t drive_protection_check_rot_break(fixed32_t voltage, fixed32_t current, fixed32_t u_ref)
{
    if(u_ref <= drive_prot.U_rot_crit_delta) return DRIVE_BREAK_CHECK_NORMAL;
    
    drive_pwr_check_res_t chk_res = drive_protection_check_zero_voltage(voltage);
    if(chk_res == DRIVE_PWR_CHECK_CRIT_UNDERFLOW || chk_res == DRIVE_PWR_CHECK_CRIT_OVERFLOW){
        return DRIVE_BREAK_CHECK_NORMAL;
    }
    
    chk_res = drive_protection_check_rot_zero_current(current);
    if(chk_res == DRIVE_PWR_CHECK_CRIT_UNDERFLOW || chk_res == DRIVE_PWR_CHECK_CRIT_OVERFLOW){
        return DRIVE_BREAK_CHECK_NORMAL;
    }
    
    return DRIVE_BREAK_CHECK_FAIL;
}
