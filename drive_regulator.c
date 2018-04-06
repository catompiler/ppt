#include "drive_regulator.h"
#include "settings.h"
#include "pid_controller/pid_controller.h"
#include "utils/utils.h"
#include "drive_power.h"
#include "drive_motor.h"
#include "drive_overload.h"
#include <string.h>


//! dt рампы.
#define DRIVE_RAMP_DT 0x1b5//0x51f


//! Тип структуры регулятора привода.
typedef struct _Drive_Regulator {
    drive_regulator_state_t state; //!< Состояние регулятора.
    
    drive_regulator_mode_t mode; //!< Режим регулятора.
    
    reference_t reference; //!< Задание.
    
    ramp_t ramp; //!< Разгон.
    
    bool rot_enabled; //!< Разрешение регулирования якоря.
    bool exc_enabled; //!< Разрешение регулирования возбуждения.
    
    fixed32_t U_rot_nom; //!< Номинальное напряжение якоря.
    fixed32_t I_rot_nom; //!< Номинальный ток якоря.
    fixed32_t I_rot_max; //!< Максимальный ток якоря.
    fixed32_t I_exc; //!< Номинальный ток возбуждения.
    fixed32_t rpm_rot_ref; //!< Текущие обороты задания.
    fixed32_t i_rot_ref; //!< Текущий ток задания.
    
    pid_controller_t spd_pid; //!< ПИД-регулятор скорости.
    pid_controller_t rot_pid; //!< ПИД-регулятор тока якоря.
    pid_controller_t exc_pid; //!< ПИД-регулятор тока возбуждения.
} drive_regulator_t;


static drive_regulator_t regulator;

err_t drive_regulator_init(void)
{
    memset(&regulator, 0x0, sizeof(drive_regulator_t));
    
    ramp_init(&regulator.ramp);
    pid_controller_init(&regulator.spd_pid, 0, 0, 0);
    pid_controller_init(&regulator.rot_pid, 0, 0, 0);
    pid_controller_init(&regulator.exc_pid, 0, 0, 0);
    
    return E_NO_ERROR;
}

void drive_regulator_update_settings(void)
{
    drive_regulator_set_mode(settings_valueu(PARAM_ID_REGULATOR_MODE));
    
    drive_regulator_set_reference_time(settings_valuef(PARAM_ID_RAMP_REFERENCE_TIME));
    drive_regulator_set_start_time(settings_valuef(PARAM_ID_RAMP_START_TIME));
    drive_regulator_set_stop_time(settings_valuef(PARAM_ID_RAMP_STOP_TIME));
    drive_regulator_set_fast_stop_time(settings_valuef(PARAM_ID_RAMP_FAST_STOP_TIME));
    
    drive_regulator_set_rot_nom_voltage(settings_valuef(PARAM_ID_MOTOR_U_ROT_NOM));
    drive_regulator_set_rot_nom_current(settings_valuef(PARAM_ID_MOTOR_I_ROT_NOM));
    drive_regulator_set_exc_current(settings_valuef(PARAM_ID_I_EXC));
    drive_regulator_set_spd_pid(settings_valuef(PARAM_ID_SPD_PID_K_P),
                                settings_valuef(PARAM_ID_SPD_PID_K_I),
                                settings_valuef(PARAM_ID_SPD_PID_K_D));
    drive_regulator_set_rot_pid(settings_valuef(PARAM_ID_ROT_PID_K_P),
                                settings_valuef(PARAM_ID_ROT_PID_K_I),
                                settings_valuef(PARAM_ID_ROT_PID_K_D));
    drive_regulator_set_exc_pid(settings_valuef(PARAM_ID_EXC_PID_K_P),
                                settings_valuef(PARAM_ID_EXC_PID_K_I),
                                settings_valuef(PARAM_ID_EXC_PID_K_D));
    
    drive_regulator_set_max_rot_current(settings_valuef(PARAM_ID_MOTOR_I_ROT_MAX));
    drive_regulator_set_rot_open_angle_range(settings_valuef(PARAM_ID_TRIACS_PAIRS_ANGLE_MIN),
                                             settings_valuef(PARAM_ID_TRIACS_PAIRS_ANGLE_MAX));
    drive_regulator_set_exc_open_angle_range(settings_valuef(PARAM_ID_TRIAC_EXC_ANGLE_MIN),
                                             settings_valuef(PARAM_ID_TRIAC_EXC_ANGLE_MAX));
}

pid_controller_t* drive_regulator_spd_pid(void)
{
    return &regulator.spd_pid;
}

pid_controller_t* drive_regulator_rot_pid(void)
{
    return &regulator.rot_pid;
}

pid_controller_t* drive_regulator_exc_pid(void)
{
    return &regulator.exc_pid;
}

drive_regulator_state_t drive_regulator_state(void)
{
    return regulator.state;
}

drive_regulator_mode_t drive_regulator_mode(void)
{
    return regulator.mode;
}

void drive_regulator_set_mode(drive_regulator_mode_t mode)
{
    regulator.mode = mode;
}

reference_t drive_regulator_reference(void)
{
    return regulator.reference;
}

fixed32_t drive_regulator_current_reference(void)
{
    return ramp_current_reference(&regulator.ramp);
}

err_t drive_regulator_set_reference(reference_t reference)
{
    reference = CLAMP(reference, REFERENCE_MIN_F, REFERENCE_MAX_F);
    
    err_t err = E_NO_ERROR;
    
    if(regulator.state == DRIVE_REGULATOR_STATE_RUN ||
       regulator.state == DRIVE_REGULATOR_STATE_START){
        err = ramp_set_target_reference(&regulator.ramp, reference);
    }
    
    if(err == E_NO_ERROR){
        regulator.reference = reference;
    }
    
    return err;
}

void drive_regulator_inc_reference(void)
{
    fixed32_t new_ref = regulator.reference + ramp_reference_step(&regulator.ramp);
    if(new_ref > REFERENCE_MAX_F) new_ref = REFERENCE_MAX_F;
    regulator.reference = new_ref;
    
    if(regulator.state == DRIVE_REGULATOR_STATE_RUN ||
       regulator.state == DRIVE_REGULATOR_STATE_START){
        ramp_set_target_reference(&regulator.ramp, regulator.reference);
    }
}

void drive_regulator_dec_reference(void)
{
    fixed32_t new_ref = regulator.reference - ramp_reference_step(&regulator.ramp);
    if(new_ref < REFERENCE_MIN_F) new_ref = REFERENCE_MIN_F;
    regulator.reference = new_ref;
    
    if(regulator.state == DRIVE_REGULATOR_STATE_RUN ||
       regulator.state == DRIVE_REGULATOR_STATE_START){
        ramp_set_target_reference(&regulator.ramp, regulator.reference);
    }
}

void drive_regulator_adjust_cur_reference(void)
{
    if(regulator.state == DRIVE_REGULATOR_STATE_IDLE) return;
    
    fixed32_t cur_val = 0;
    fixed32_t max_val = 0;
    
    if(regulator.mode == DRIVE_REGULATOR_MODE_SPEED){
        cur_val = drive_power_channel_real_value(DRIVE_POWER_Urot);
        max_val = regulator.U_rot_nom;
    }else{ //DRIVE_REGULATOR_MODE_TORQUE
        cur_val = drive_power_channel_real_value(DRIVE_POWER_Irot);
        max_val = regulator.I_rot_nom;
    }
    
    ramp_adjust_current_reference(&regulator.ramp, cur_val, max_val);
    
    pid_controller_reset(&regulator.spd_pid);
    pid_controller_reset(&regulator.rot_pid);
}

void drive_regulator_start(void)
{
    if(regulator.state == DRIVE_REGULATOR_STATE_IDLE ||
       regulator.state == DRIVE_REGULATOR_STATE_STOP){
        
        regulator.state = DRIVE_REGULATOR_STATE_START;
        
        ramp_set_target_reference(&regulator.ramp, regulator.reference);
    }
}

void drive_regulator_stop(void)
{
    if(regulator.state == DRIVE_REGULATOR_STATE_RUN ||
       regulator.state == DRIVE_REGULATOR_STATE_START){
        
        regulator.state = DRIVE_REGULATOR_STATE_STOP;
        
        ramp_set_stop_mode(&regulator.ramp, RAMP_STOP_MODE_NORMAL);
        ramp_set_target_reference(&regulator.ramp, 0);
    }
}

void drive_regulator_fast_stop(void)
{
    if(regulator.state == DRIVE_REGULATOR_STATE_RUN ||
       regulator.state == DRIVE_REGULATOR_STATE_START ||
       regulator.state == DRIVE_REGULATOR_STATE_STOP){
        
        regulator.state = DRIVE_REGULATOR_STATE_STOP;
        
        ramp_set_stop_mode(&regulator.ramp, RAMP_STOP_MODE_FAST);
        ramp_set_target_reference(&regulator.ramp, 0);
    }
}

err_t drive_regulator_set_reference_time(ramp_time_t time)
{
    return ramp_set_reference_time(&regulator.ramp, time, DRIVE_RAMP_DT);
}

err_t drive_regulator_set_start_time(ramp_time_t time)
{
    return ramp_set_start_time(&regulator.ramp, time, DRIVE_RAMP_DT);
}

err_t drive_regulator_set_stop_time(ramp_time_t time)
{
    return ramp_set_stop_time(&regulator.ramp, time, DRIVE_RAMP_DT);
}

err_t drive_regulator_set_fast_stop_time(ramp_time_t time)
{
    return ramp_set_fast_stop_time(&regulator.ramp, time, DRIVE_RAMP_DT);
}

bool drive_regulator_rot_enabled(void)
{
    return regulator.rot_enabled;
}

void drive_regulator_set_rot_enabled(bool enabled)
{
    regulator.rot_enabled = enabled;
    
    if(!enabled){
        pid_controller_reset(&regulator.spd_pid);
        pid_controller_reset(&regulator.rot_pid);
        ramp_reset_reference(&regulator.ramp);
    }
}

bool drive_regulator_exc_enabled(void)
{
    return regulator.exc_enabled;
}

void drive_regulator_set_exc_enabled(bool enabled)
{
    regulator.exc_enabled = enabled;
    
    if(!enabled){
        pid_controller_reset(&regulator.exc_pid);
    }
}

void drive_regulator_set_spd_pid(fixed32_t kp, fixed32_t ki, fixed32_t kd)
{
    pid_controller_set_kp(&regulator.spd_pid, kp);
    pid_controller_set_ki(&regulator.spd_pid, ki);
    pid_controller_set_kd(&regulator.spd_pid, kd);
}

void drive_regulator_set_rot_pid(fixed32_t kp, fixed32_t ki, fixed32_t kd)
{
    pid_controller_set_kp(&regulator.rot_pid, kp);
    pid_controller_set_ki(&regulator.rot_pid, ki);
    pid_controller_set_kd(&regulator.rot_pid, kd);
}

void drive_regulator_set_exc_pid(fixed32_t kp, fixed32_t ki, fixed32_t kd)
{
    pid_controller_set_kp(&regulator.exc_pid, kp);
    pid_controller_set_ki(&regulator.exc_pid, ki);
    pid_controller_set_kd(&regulator.exc_pid, kd);
}

void drive_regulator_set_max_rot_current(fixed32_t I_max)
{
    regulator.I_rot_max = I_max;
    //pid_controller_clamp(&regulator.spd_pid, 0, I_max);
}

void drive_regulator_set_rot_open_angle_range(fixed32_t angle_min, fixed32_t angle_max)
{
    pid_controller_clamp(&regulator.rot_pid, angle_min, angle_max);
}

void drive_regulator_set_exc_open_angle_range(fixed32_t angle_min, fixed32_t angle_max)
{
    pid_controller_clamp(&regulator.exc_pid, angle_min, angle_max);
}

fixed32_t drive_regulator_rot_nom_voltage(void)
{
    return regulator.U_rot_nom;
}

void drive_regulator_set_rot_nom_voltage(fixed32_t voltage)
{
    regulator.U_rot_nom = voltage;
}

fixed32_t drive_regulator_rot_nom_current(void)
{
    return regulator.I_rot_nom;
}

void drive_regulator_set_rot_nom_current(fixed32_t current)
{
    regulator.I_rot_nom = current;
}

fixed32_t drive_regulator_exc_current(void)
{
    return regulator.I_exc;
}

void drive_regulator_set_exc_current(fixed32_t current)
{
    regulator.I_exc = current;
}

fixed32_t drive_regulator_current_rpm_ref(void)
{
    return regulator.rpm_rot_ref;
}

fixed32_t drive_regulator_current_i_ref(void)
{
    return regulator.i_rot_ref;
}

fixed32_t drive_regulator_rot_speed_current_ref(void)
{
    return pid_controller_value(&regulator.spd_pid);
}

fixed32_t drive_regulator_rot_open_angle(void)
{
    return pid_controller_value(&regulator.rot_pid);
    //debug
    //#warning return pid controller value
    //return regulator.rot_ramp.current_ref * 120 / 100;
}

fixed32_t drive_regulator_exc_open_angle(void)
{
    return pid_controller_value(&regulator.exc_pid);
}

bool drive_regulator_regulate_speed(fixed32_t dt)
{
    if(!regulator.rot_enabled) return false;

    fixed32_t rpm_rot_back = drive_motor_rpm();
    
    // Если обороты отрицательны - считаем их моментом на валу.
    if(rpm_rot_back < 0) rpm_rot_back = 0;
    
    fixed32_t rpm_rot_e = 0;

    rpm_rot_e = regulator.rpm_rot_ref - rpm_rot_back;
    pid_controller_calculate(&regulator.spd_pid, rpm_rot_e, dt);
    
    return true;
}

bool drive_regulator_regulate_current(fixed32_t dt)
{
    if(!regulator.rot_enabled) return false;
    
    if(drive_overload_enabled()){
        pid_controller_clamp(&regulator.spd_pid, 0, drive_overload_avail_current());
    }else{
        pid_controller_clamp(&regulator.spd_pid, 0, regulator.I_rot_max);
    }
    
    fixed32_t i_rot_back = drive_power_channel_real_value(DRIVE_POWER_Irot);
    
    fixed32_t i_rot_e = 0;
    
    if(regulator.mode == DRIVE_REGULATOR_MODE_SPEED){
        i_rot_e = pid_controller_value(&regulator.spd_pid) - i_rot_back;
    }else{ //DRIVE_REGULATOR_MODE_TORQUE
        i_rot_e = regulator.i_rot_ref - i_rot_back;
    }
    
    pid_controller_calculate(&regulator.rot_pid, i_rot_e, dt);
    
    return true;
}

bool drive_regulator_regulate_exc(fixed32_t dt)
{
    if(!regulator.exc_enabled) return false;
    
    fixed32_t i_exc_back = drive_power_channel_real_value(DRIVE_POWER_Iexc);
    fixed32_t i_exc_e = regulator.I_exc - i_exc_back;
    pid_controller_calculate(&regulator.exc_pid, i_exc_e, dt);
    
    return true;
}

static void drive_regulator_process_impl(void)
{
    if(regulator.rot_enabled){
        
        ramp_calc_step(&regulator.ramp);
        fixed32_t ramp_cur_ref = ramp_current_reference(&regulator.ramp) / 100; //0..100% -> 0.0..1.0
        
        if(regulator.mode == DRIVE_REGULATOR_MODE_SPEED){
            fixed32_t rpm_rot_max = drive_motor_rpm_max();
            fixed32_t rpm_rot_cur_max = drive_motor_rpm_cur_max();

            fixed32_t rpm_rot_ref = fixed32_mul((int64_t)rpm_rot_max, ramp_cur_ref);

            if(rpm_rot_ref > rpm_rot_cur_max){
                rpm_rot_ref = rpm_rot_cur_max;
                ramp_adjust_current_reference(&regulator.ramp, rpm_rot_cur_max, rpm_rot_max);
            }

            regulator.rpm_rot_ref = rpm_rot_ref;
            regulator.i_rot_ref = 0;
        }else{ // DRIVE_REGULATOR_MODE_TORQUE
            regulator.i_rot_ref = fixed32_mul((int64_t)regulator.I_rot_max, ramp_cur_ref);
            regulator.rpm_rot_ref = 0;
        }
    }
}

bool drive_regulator_process(void)
{
    switch(regulator.state){
        default:
        case DRIVE_REGULATOR_STATE_IDLE:
            return false;
        case DRIVE_REGULATOR_STATE_START:
            drive_regulator_process_impl();
            if(ramp_current_reference(&regulator.ramp) >= regulator.reference){
                regulator.state = DRIVE_REGULATOR_STATE_RUN;
            }
            break;
        case DRIVE_REGULATOR_STATE_RUN:
            drive_regulator_process_impl();
            break;
        case DRIVE_REGULATOR_STATE_STOP:
            drive_regulator_process_impl();
            if(ramp_done(&regulator.ramp)){
                regulator.state = DRIVE_REGULATOR_STATE_IDLE;
            }
            break;
    }
    
    return true;
}
