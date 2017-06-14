#include "drive_motor.h"
#include "drive_power.h"
#include "drive_triacs.h"
#include <string.h>
#include "settings.h"


//! Структура двигателя привода.
typedef struct _Drive_Motor {
    bool valid; //!< Флаг допустимости параметров двигателя.
    fixed32_t I_exc; //!< Номинальный ток возбуждения.
    fixed32_t R_rot; //!< Сопротивление якоря.
    fixed32_t Cn; //!< Коэффициент для расчёта оборотов.
    fixed32_t Cm; //!< Коэффициент для расчёта момента.
    param_t* param_rpm; //!< Параметр с оборотами двигателя.
    param_t* param_torque; //!< Параметр с моментом двигателя.
    fixed32_t rpm; //!< Текущие обороты двигателя.
    fixed32_t torque; //!< Текущий момент двигателя.
} drive_motor_t;


#define _30_div_PI (0x98C9F)
#define _r_alpha (0x8000)
#define K_R75 (0x13D71)


static drive_motor_t motor;


err_t drive_motor_init(void)
{
    memset(&motor, 0x0, sizeof(drive_motor_t));
    
    motor.param_rpm = settings_param_by_id(PARAM_ID_MOTOR_RPM);
    motor.param_torque = settings_param_by_id(PARAM_ID_MOTOR_TORQUE);
    
    return E_NO_ERROR;
}

err_t drive_motor_update_settings(void)
{
    fixed32_t P_f = settings_valuef(PARAM_ID_MOTOR_P_NOM); // кВт.
    
    int32_t N = settings_valueu(PARAM_ID_MOTOR_RPM_NOM); // об/мин.
    
    int32_t U_rot = settings_valueu(PARAM_ID_MOTOR_U_ROT_NOM); // В.
    fixed32_t I_rot_f = settings_valuef(PARAM_ID_MOTOR_I_ROT_NOM); // А.
    
    int32_t U_exc = settings_valueu(PARAM_ID_MOTOR_U_EXC_NOM); // В.
    fixed32_t I_exc_f = settings_valuef(PARAM_ID_MOTOR_I_EXC_NOM); // А.
    
    fixed32_t eff_f = settings_valuef(PARAM_ID_MOTOR_EFF_NOM) / 100; // % -> доли.
    
    int64_t P_W_f = (int64_t)P_f * 1000;
    
    if(eff_f == 0){
        fixed32_t P_e = ((int64_t)U_rot * I_rot_f + (int64_t)U_exc * I_exc_f); // Вт -> кВт.
        eff_f = fixed32_div(P_W_f, P_e);
    }
    
    if(eff_f <= 0 || eff_f >= fixed32_make_from_int(1)){
        motor.valid = false;
        return E_INVALID_VALUE;
    }
    
    int64_t N_Irot_Iexc = N * I_rot_f;
    N_Irot_Iexc = fixed32_mul(N_Irot_Iexc, I_exc_f);
    
    fixed32_t CeF = fixed32_div(P_W_f, N_Irot_Iexc);
    fixed32_t CeM = fixed32_mul((int64_t)CeF, _30_div_PI);
    
    fixed32_t R_rot = settings_valuef(PARAM_ID_MOTOR_R_ROT_NOM);
    
    if(R_rot == 0){
        fixed32_t U_rot_f = fixed32_make_from_int(U_rot);
        fixed32_t eff_inv = fixed32_make_from_int(1) - eff_f;
        R_rot = fixed32_div((int64_t)U_rot_f, I_rot_f);
        R_rot = fixed32_mul((int64_t)R_rot, eff_inv);
        R_rot = fixed32_mul((int64_t)R_rot, _r_alpha);
    }
    
    if(R_rot == 0){
        motor.valid = false;
        return E_INVALID_VALUE;
    }
    
    fixed32_t R_rot75 = fixed32_mul((int64_t)R_rot, K_R75);
    
    motor.I_exc = I_exc_f;
    motor.Cn = CeF;
    motor.Cm = CeM;
    motor.R_rot = R_rot75;
    
    param_t* p_eff = settings_param_by_id(PARAM_ID_MOTOR_EFF);
    DRIVE_UPDATE_PARAM_FIXED(p_eff, eff_f);
    
    param_t* p_r = settings_param_by_id(PARAM_ID_MOTOR_R_ROT);
    DRIVE_UPDATE_PARAM_FIXED(p_r, R_rot);
    
    motor.valid = true;
    
    return E_NO_ERROR;
}

bool drive_motor_ready(void)
{
    return motor.valid;
}

void drive_motor_calculate(void)
{
    if(!motor.valid) return;
    
    fixed32_t U_rot = drive_power_channel_real_value(DRIVE_POWER_Urot);
    fixed32_t I_rot = drive_power_channel_real_value(DRIVE_POWER_Irot);
    fixed32_t I_exc = 0;
    
    if(drive_triacs_exc_mode() != DRIVE_TRIACS_EXC_EXTERNAL){
        I_exc = drive_power_channel_real_value(DRIVE_POWER_Iexc);
    }else{
        I_exc = motor.I_exc;
    }
    
    fixed32_t U_R_rot = fixed32_mul((int64_t)I_rot, motor.R_rot);
    fixed32_t U_minus_IR = U_rot - U_R_rot;
    fixed32_t C_Iexc = fixed32_mul((int64_t)motor.Cn, I_exc);
    fixed32_t N = fixed32_div((int64_t)U_minus_IR, C_Iexc);
    
    fixed32_t I_rot_I_exc = fixed32_mul((int64_t)I_rot, I_exc);
    fixed32_t M = fixed32_mul((int64_t)motor.Cm, I_rot_I_exc);
    
    motor.rpm = N;
    motor.torque = M;
    
    DRIVE_UPDATE_PARAM_FIXED(motor.param_rpm, N);
    DRIVE_UPDATE_PARAM_FIXED(motor.param_torque, M);
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
