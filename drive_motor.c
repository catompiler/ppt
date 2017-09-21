#include "drive_motor.h"
#include "drive_power.h"
#include "drive_triacs.h"
#include "drive_protection.h"
#include "drive.h"
#include <string.h>
#include "settings.h"


//! Структура двигателя привода.
typedef struct _Drive_Motor {
    bool valid; //!< Флаг допустимости параметров двигателя.
    bool ir_comp_enabled; //!< Разрешение IR-компенсации.
    fixed32_t Eff; //!< КПД.
    fixed32_t E_nom; //!< Номинальная ЭДС двигателя.
    fixed32_t RPM_nom; //!< Номинальные обороты.
    fixed32_t M_nom; //!< Номинальный момент.
    fixed32_t I_exc; //!< Номинальный ток возбуждения.
    fixed32_t R_rot; //!< Сопротивление якоря.
    fixed32_t L_rot; //!< Сопротивление якоря.
    fixed32_t R_exc; //!< Сопротивление возбуждения.
    fixed32_t R_wires; //!< Сопротивление проводов до двигателя.
    fixed32_t Cn; //!< Коэффициент для расчёта оборотов.
    fixed32_t Cm; //!< Коэффициент для расчёта момента.
    param_t* param_rpm; //!< Параметр с оборотами двигателя.
    param_t* param_torque; //!< Параметр с моментом двигателя.
    param_t* param_e_rot; //!< Параметр с ЭДС двигателя.
    param_t* param_u_wires; //!< Параметр с падением напряжения на проводах.
    fixed32_t rpm; //!< Текущие обороты двигателя.
    fixed32_t torque; //!< Текущий момент двигателя.
    fixed32_t E_rot; //!< ЭДС двигателя.
    fixed32_t U_wires; //!< Падение напряжения на проводах.
} drive_motor_t;


#define _30_div_PI (0x98C9F)
#define _r_alpha (0x8000)
#define K_R75 (0x13D71)
#define BETA (3)


static drive_motor_t motor;


err_t drive_motor_init(void)
{
    memset(&motor, 0x0, sizeof(drive_motor_t));
    
    motor.param_rpm = settings_param_by_id(PARAM_ID_MOTOR_RPM);
    motor.param_torque = settings_param_by_id(PARAM_ID_MOTOR_TORQUE);
    motor.param_e_rot = settings_param_by_id(PARAM_ID_MOTOR_E);
    motor.param_u_wires = settings_param_by_id(PARAM_ID_POWER_U_WIRES);
    
    return E_NO_ERROR;
}

static void drive_motor_update_calc_params(void)
{
    param_t* p_eff = settings_param_by_id(PARAM_ID_MOTOR_EFF);
    DRIVE_UPDATE_PARAM_FIXED(p_eff, motor.Eff * 100);
    
    param_t* p_r_rot = settings_param_by_id(PARAM_ID_MOTOR_R_ROT);
    DRIVE_UPDATE_PARAM_FIXED(p_r_rot, fixed32_div((int64_t)motor.R_rot, K_R75));
    
    param_t* p_r_exc = settings_param_by_id(PARAM_ID_MOTOR_R_EXC);
    DRIVE_UPDATE_PARAM_FIXED(p_r_exc, fixed32_div((int64_t)motor.R_exc, K_R75));
    
    param_t* p_l_rot = settings_param_by_id(PARAM_ID_MOTOR_L_ROT);
    DRIVE_UPDATE_PARAM_FIXED(p_l_rot, motor.L_rot * 1000);
    
    param_t* p_e_nom = settings_param_by_id(PARAM_ID_MOTOR_E_NOM);
    DRIVE_UPDATE_PARAM_FIXED(p_e_nom, motor.E_nom);
    
    param_t* p_m_nom = settings_param_by_id(PARAM_ID_MOTOR_M_NOM);
    DRIVE_UPDATE_PARAM_FIXED(p_m_nom, motor.M_nom);
}

err_t drive_motor_update_settings(void)
{
    fixed32_t P_f = settings_valuef(PARAM_ID_MOTOR_P_NOM); // кВт.
    
    int32_t RPM_nom = settings_valueu(PARAM_ID_MOTOR_RPM_NOM); // об/мин.
    
    int32_t poles = settings_valueu(PARAM_ID_MOTOR_POLES); // Число полюсов.
    
    int32_t U_rot = settings_valueu(PARAM_ID_MOTOR_U_ROT_NOM); // В.
    fixed32_t U_rot_f = fixed32_make_from_int(U_rot);
    fixed32_t I_rot_f = settings_valuef(PARAM_ID_MOTOR_I_ROT_NOM); // А.
    
    int32_t U_exc = settings_valueu(PARAM_ID_MOTOR_U_EXC_NOM); // В.
    fixed32_t I_exc_f = settings_valuef(PARAM_ID_MOTOR_I_EXC_NOM); // А.
    
    fixed32_t eff_f = settings_valuef(PARAM_ID_MOTOR_EFF_NOM) / 100; // % -> доли.
    
    int64_t P_W_f = (int64_t)P_f * 1000; // кВт -> Вт.
    
    if(P_f == 0 || RPM_nom == 0 ||
       U_rot == 0 || I_rot_f == 0 ||
       U_exc == 0 || I_exc_f == 0){
        motor.valid = false;
        return E_INVALID_VALUE;
    }
    
    if(eff_f == 0){
        int64_t P_e = ((int64_t)U_rot * I_rot_f + (int64_t)U_exc * I_exc_f);
        eff_f = fixed32_div(P_W_f, P_e);
    }
    
    if(eff_f <= 0 || eff_f >= fixed32_make_from_int(1)){
        motor.valid = false;
        return E_INVALID_VALUE;
    }
    
    int64_t N_Irot_Iexc = (int64_t)RPM_nom * I_rot_f;
    N_Irot_Iexc = fixed32_mul(N_Irot_Iexc, I_exc_f);
    
    fixed32_t CeF = fixed32_div(P_W_f, N_Irot_Iexc);
    fixed32_t CmF = fixed32_mul((int64_t)CeF, _30_div_PI);
    
    fixed32_t R_rot = settings_valuef(PARAM_ID_MOTOR_R_ROT_NOM);
    
    if(R_rot == 0){
        //fixed32_t U_rot_f = fixed32_make_from_int(U_rot);
        fixed32_t eff_inv = fixed32_make_from_int(1) - eff_f;
        R_rot = fixed32_div((int64_t)U_rot_f, I_rot_f);
        R_rot = fixed32_mul((int64_t)R_rot, eff_inv);
        R_rot = fixed32_mul((int64_t)R_rot, _r_alpha);
    }
    
    if(R_rot <= 0){
        motor.valid = false;
        return E_INVALID_VALUE;
    }
    fixed32_t R_rot75 = fixed32_mul((int64_t)R_rot, K_R75);
    
    fixed32_t R_exc = settings_valuef(PARAM_ID_MOTOR_R_EXC_NOM);
    
    if(R_exc == 0){
        fixed32_t U_exc_f = fixed32_make_from_int(U_exc);
        R_exc = fixed32_div((int64_t)U_exc_f, I_exc_f);
    }
    
    if(R_exc <= 0){
        motor.valid = false;
        return E_INVALID_VALUE;
    }
    fixed32_t R_exc75 = fixed32_mul((int64_t)R_exc, K_R75);
    
    fixed32_t L_rot_mH = settings_valuef(PARAM_ID_MOTOR_L_ROT_NOM);
    fixed32_t L_rot = L_rot_mH / 1000;
    
    if(L_rot == 0){
        // L = (BETA * Unom) / (p * N * Inom);
        fixed32_t Unom4 = BETA * U_rot_f;
        int32_t pN = poles * RPM_nom;
        fixed32_t Unom4_pN = Unom4 / pN;
        L_rot = fixed32_div((int64_t)Unom4_pN, I_rot_f);
    }
    
    if(L_rot <= 0){
        motor.valid = false;
        return E_INVALID_VALUE;
    }
    
    fixed32_t R_wires = settings_valuef(PARAM_ID_MOTOR_R_WIRES);
    
    // Enom.
    fixed32_t Inom_R = fixed32_mul((int64_t)I_rot_f, R_rot75);
    fixed32_t E_nom = U_rot_f - Inom_R;
    
    // Mnom.
    fixed32_t M_nom = fixed32_mul((int64_t)CmF, I_rot_f);
    
    motor.Eff = eff_f;
    motor.I_exc = I_exc_f;
    motor.Cn = CeF;
    motor.Cm = CmF;
    motor.R_rot = R_rot75;
    motor.R_exc = R_exc75;
    motor.L_rot = L_rot;
    motor.R_wires = R_wires;
    motor.E_nom = E_nom;
    motor.M_nom = M_nom;
    motor.RPM_nom = fixed32_make_from_int(RPM_nom);
    
    drive_motor_update_calc_params();
    
    motor.valid = true;
    
    return E_NO_ERROR;
}

bool drive_motor_ready(void)
{
    return motor.valid;
}

bool drive_motor_ir_compensation_enabled(void)
{
    return motor.ir_comp_enabled;
}

void drive_motor_set_ir_compensation_enabled(bool enabled)
{
    motor.ir_comp_enabled = enabled;
}

void drive_motor_calculate(void)
{
    if(!motor.valid) return;
    
    fixed32_t N = 0;
    fixed32_t M = 0;
    fixed32_t E_rot = 0;
    fixed32_t U_wires = 0;
    
    fixed32_t I_rot = 0;
    fixed32_t I_exc = 0;
    
    I_rot = drive_power_channel_real_value(DRIVE_POWER_Irot);
    
    U_wires = fixed32_mul((int64_t)I_rot, motor.R_wires);
    
    E_rot = drive_power_channel_real_value(DRIVE_POWER_Erot);
    if(motor.ir_comp_enabled) E_rot -= U_wires;
    
    bool i_exc_zero = false;
    
    if(drive_triacs_exc_mode() != DRIVE_TRIACS_EXC_EXTERNAL){
        i_exc_zero = drive_protection_is_allow(drive_protection_check_exc_zero_current());
        if(!i_exc_zero) I_exc = drive_power_channel_real_value(DRIVE_POWER_Iexc);
    }else{
        I_exc = motor.I_exc;
    }
    
    if(!i_exc_zero){
        fixed32_t U_minus_IR = E_rot;
        fixed32_t C_Iexc = fixed32_mul((int64_t)motor.Cn, I_exc);
        N = fixed32_div((int64_t)U_minus_IR, C_Iexc);

        fixed32_t I_rot_I_exc = fixed32_mul((int64_t)I_rot, I_exc);
        M = fixed32_mul((int64_t)motor.Cm, I_rot_I_exc);
    }
    
    motor.rpm = N;
    motor.torque = M;
    motor.E_rot = E_rot;
    motor.U_wires = U_wires;
    
    DRIVE_UPDATE_PARAM_INT(motor.param_rpm, fixed32_get_int(N));
    DRIVE_UPDATE_PARAM_FIXED(motor.param_torque, M);
    DRIVE_UPDATE_PARAM_FIXED(motor.param_e_rot, E_rot);
    DRIVE_UPDATE_PARAM_FIXED(motor.param_u_wires, U_wires);
}

fixed32_t drive_motor_e_nom(void)
{
    return motor.E_nom;
}

fixed32_t drive_motor_rpm_nom(void)
{
    return motor.RPM_nom;
}

fixed32_t drive_motor_m_nom(void)
{
    return motor.M_nom;
}

fixed32_t drive_motor_r_rot(void)
{
    return motor.R_rot;
}

fixed32_t drive_motor_l_rot(void)
{
    return motor.L_rot;
}

fixed32_t drive_motor_r_exc(void)
{
    return motor.R_exc;
}

fixed32_t drive_motor_r_wires(void)
{
    return motor.R_wires;
}

fixed32_t drive_motor_u_wires(void)
{
    return motor.U_wires;
}

fixed32_t drive_motor_rpm(void)
{
    if(!motor.valid) return 0;
    
    return motor.rpm;
}

fixed32_t drive_motor_torque(void)
{
    if(!motor.valid) return 0;
    
    return motor.torque;
}

fixed32_t drive_motor_e_rot(void)
{
    if(!motor.valid) return 0;
    
    return  motor.E_rot;
}