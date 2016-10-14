#include "drive_regulator.h"
#include "pid_controller/pid_controller.h"
#include "utils/utils.h"
#include <string.h>


//! dt PID-регулятора.
#define DRIVE_PID_DT 0x1b5//0x51f

//! dt PID-регулятора.
#define DRIVE_RAMP_DT 0x1b5//0x51f


//! Тип структуры регулятора привода.
typedef struct _Drive_Regulator {
    drive_regulator_state_t state; //!< Состояние регулятора.
    
    fixed32_t reference; //!< Задание.
    
    ramp_t rot_ramp; //!< Разгон.
    
    bool rot_enabled; //!< Разрешение регулирования напряжения якоря.
    bool exc_enabled; //!< Разрешение регулирования тока возбуждения.
    
    fixed32_t U_rot_nom; //!< Номинальное напряжение возбуждения якоря.
    fixed32_t I_exc; //!< Номинальный ток возбуждения.
    fixed32_t u_rot_ref; //!< Текущее напряжение задания.
    
    pid_controller_t rot_pid; //!< ПИД-регулятор напряжения якоря.
    pid_controller_t exc_pid; //!< ПИД-регулятор тока возбуждения.
} drive_regulator_t;


static drive_regulator_t regulator;

err_t drive_regulator_init(void)
{
    memset(&regulator, 0x0, sizeof(drive_regulator_t));
    
    ramp_init(&regulator.rot_ramp);
    pid_controller_init(&regulator.rot_pid, 0, 0, 0);
    pid_controller_init(&regulator.exc_pid, 0, 0, 0);
    
    return E_NO_ERROR;
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

reference_t drive_regulator_reference(void)
{
    return fixed32_get_int(regulator.reference);
}

fixed32_t drive_regulator_current_reference(void)
{
    return ramp_current_reference(&regulator.rot_ramp);
}

err_t drive_regulator_set_reference(reference_t reference)
{
    // 0 ... 100 == 12000 (TRIACS_TIM_ANGLE_TICKS_MAX) ... 0
    if(reference > REFERENCE_MAX) return E_OUT_OF_RANGE;
    
    err_t err = E_NO_ERROR;
    
    if(regulator.state == DRIVE_REGULATOR_STATE_RUN ||
       regulator.state == DRIVE_REGULATOR_STATE_START){
        err = ramp_set_target_reference(&regulator.rot_ramp, reference);
    }
    
    if(err == E_NO_ERROR){
        regulator.reference = fixed32_make_from_int(reference);
    }
    
    return err;
}

void drive_regulator_inc_reference(void)
{
    fixed32_t new_ref = regulator.reference + ramp_reference_step(&regulator.rot_ramp);
    if(new_ref > REFERENCE_MAX_F) new_ref = REFERENCE_MAX_F;
    regulator.reference = new_ref;
    
    if(regulator.state == DRIVE_REGULATOR_STATE_RUN ||
       regulator.state == DRIVE_REGULATOR_STATE_START){
        ramp_set_target_reference(&regulator.rot_ramp, fixed32_get_int(regulator.reference));
    }
}

void drive_regulator_dec_reference(void)
{
    fixed32_t new_ref = regulator.reference - ramp_reference_step(&regulator.rot_ramp);
    if(new_ref < REFERENCE_MIN_F) new_ref = REFERENCE_MIN_F;
    regulator.reference = new_ref;
    
    if(regulator.state == DRIVE_REGULATOR_STATE_RUN ||
       regulator.state == DRIVE_REGULATOR_STATE_START){
        ramp_set_target_reference(&regulator.rot_ramp, fixed32_get_int(regulator.reference));
    }
}

void drive_regulator_start(void)
{
    if(regulator.state == DRIVE_REGULATOR_STATE_IDLE ||
       regulator.state == DRIVE_REGULATOR_STATE_STOP){
        regulator.state = DRIVE_REGULATOR_STATE_START;
        ramp_set_target_reference(&regulator.rot_ramp, fixed32_get_int(regulator.reference));
    }
}

void drive_regulator_stop(void)
{
    if(regulator.state == DRIVE_REGULATOR_STATE_RUN ||
       regulator.state == DRIVE_REGULATOR_STATE_START){
        regulator.state = DRIVE_REGULATOR_STATE_STOP;
        ramp_set_target_reference(&regulator.rot_ramp, 0);
    }
}

err_t drive_regulator_set_ramp_time(ramp_time_t time)
{
    return ramp_set_time(&regulator.rot_ramp, time, DRIVE_RAMP_DT);
}

bool drive_regulator_rot_enabled(void)
{
    return regulator.rot_enabled;
}

void drive_regulator_set_rot_enabled(bool enabled)
{
    regulator.rot_enabled = enabled;
    
    if(!enabled){
        pid_controller_reset(&regulator.rot_pid);
        ramp_reset_reference(&regulator.rot_ramp);
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

void drive_regulator_set_rot_pid(fixed32_t kp, fixed32_t ki, fixed32_t kd)
{
    pid_controller_set_kp(&regulator.rot_pid, kp);
    pid_controller_set_ki(&regulator.rot_pid, ki);
    pid_controller_set_kd(&regulator.rot_pid, kd);
}

void drive_regulator_rot_pid_clamp(fixed32_t pid_min, fixed32_t pid_max)
{
    pid_controller_clamp(&regulator.rot_pid, pid_min, pid_max);
}

fixed32_t drive_regulator_rot_pid_value(void)
{
    return pid_controller_value(&regulator.rot_pid);
    //debug
    //#warning return pid controller value
    //return regulator.rot_ramp.current_ref * 120 / 100;
}

void drive_regulator_set_exc_pid(fixed32_t kp, fixed32_t ki, fixed32_t kd)
{
    pid_controller_set_kp(&regulator.exc_pid, kp);
    pid_controller_set_ki(&regulator.exc_pid, ki);
    pid_controller_set_kd(&regulator.exc_pid, kd);
}

void drive_regulator_exc_pid_clamp(fixed32_t pid_min, fixed32_t pid_max)
{
    pid_controller_clamp(&regulator.exc_pid, pid_min, pid_max);
}

fixed32_t drive_regulator_exc_pid_value(void)
{
    return pid_controller_value(&regulator.exc_pid);
}

fixed32_t drive_regulator_rot_nom_voltage(void)
{
    return regulator.U_rot_nom;
}

void drive_regulator_set_rot_nom_voltage(fixed32_t voltage)
{
    regulator.U_rot_nom = voltage;
}

fixed32_t drive_regulator_exc_current(void)
{
    return regulator.I_exc;
}

void drive_regulator_set_exc_current(fixed32_t current)
{
    regulator.I_exc = current;
}

fixed32_t drive_regulator_current_u_ref(void)
{
    return regulator.u_rot_ref;
}

static void drive_regulator_regulate_impl(fixed32_t u_rot_back, fixed32_t i_exc_back)
{
    if(regulator.rot_enabled){
        ramp_calc_step(&regulator.rot_ramp);
        fixed32_t ramp_cur_ref = ramp_current_reference(&regulator.rot_ramp) / 100;
        
        regulator.u_rot_ref = fixed32_mul((int64_t)regulator.U_rot_nom, ramp_cur_ref);
        
        fixed32_t u_rot_e = regulator.u_rot_ref - u_rot_back;
        pid_controller_calculate(&regulator.rot_pid, u_rot_e, DRIVE_PID_DT);
    }
    
    if(regulator.exc_enabled){
        fixed32_t i_exc_e = regulator.I_exc - i_exc_back;
        pid_controller_calculate(&regulator.exc_pid, i_exc_e, DRIVE_PID_DT);
    }
}

bool drive_regulator_regulate(fixed32_t u_rot_back, fixed32_t i_exc_back)
{
    switch(regulator.state){
        default:
        case DRIVE_REGULATOR_STATE_IDLE:
            return false;
        case DRIVE_REGULATOR_STATE_START:
            drive_regulator_regulate_impl(u_rot_back, i_exc_back);
            if(ramp_current_reference(&regulator.rot_ramp) >= regulator.reference){
                regulator.state = DRIVE_REGULATOR_STATE_RUN;
            }
            break;
        case DRIVE_REGULATOR_STATE_RUN:
            drive_regulator_regulate_impl(u_rot_back, i_exc_back);
            break;
        case DRIVE_REGULATOR_STATE_STOP:
            drive_regulator_regulate_impl(u_rot_back, i_exc_back);
            if(ramp_done(&regulator.rot_ramp)){
                regulator.state = DRIVE_REGULATOR_STATE_IDLE;
            }
            break;
    }
    
    return true;
}
