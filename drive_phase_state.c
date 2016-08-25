#include "drive_phase_state.h"
#include "defs/defs.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>


//! Тип состояния фазы.
typedef struct _DrivePhaseState {
    phase_t cur_phase; //!< Текущая фаза.
    drive_dir_t drive_dir; //!< Направление.
    drive_phase_errors_t phase_errs; //!< Ошибки.
    TIM_TypeDef* timer_cnt; //!< Таймер - счётчик времени между фазами.
    phase_time_t phases_time[PHASES_COUNT]; //!< Время между датчиками нуля.
    drive_phase_state_error_callback_t error_callback; //!< Каллбэк ошибки фаз.
} phase_state_t;

static phase_state_t state;



ALWAYS_INLINE static void drive_phase_state_set_error(drive_phase_error_t error)
{
    state.phase_errs |= error;
}

static void drive_phase_state_error_occured(drive_phase_error_t error)
{
    drive_phase_state_set_error(error);
    if(state.error_callback) state.error_callback();
}

ALWAYS_INLINE static void drive_phase_state_set_time(phase_t phase, phase_time_t time)
{
    if(phase != PHASE_UNK){
        state.phases_time[phase - 1] = time;
    }
}

ALWAYS_INLINE static phase_time_t drive_phase_state_get_cur_time(void)
{
    if(state.timer_cnt){
        return TIM_GetCounter(state.timer_cnt);
    }
    return 0;
}

ALWAYS_INLINE static phase_time_t drive_phase_state_delta(phase_time_t time)
{
    if(time < PHASE_TIME_US) return PHASE_TIME_US - time;
    return time - PHASE_TIME_US;
}

ALWAYS_INLINE static bool drive_phase_state_time_check(phase_time_t time)
{
    if(drive_phase_state_delta(time) > PHASE_DELTA_US_MAX) return false;
    return true;
}

ALWAYS_INLINE static void drive_phase_state_reset_timer(void)
{
    if(state.timer_cnt){
        TIM_SetCounter(state.timer_cnt, 0);
    }
}

ALWAYS_INLINE static void drive_phase_state_stop_timer(void)
{
    if(state.timer_cnt){
        TIM_Cmd(state.timer_cnt, DISABLE);
    }
}

ALWAYS_INLINE static void drive_phase_state_start_timer(void)
{
    if(state.timer_cnt){
        TIM_SetCounter(state.timer_cnt, 0);
        TIM_Cmd(state.timer_cnt, ENABLE);
    }
}

static void drive_phase_state_a_process(phase_t phase)
{
    drive_dir_t drive_dir;
    switch(phase){
        default:
        case PHASE_UNK:
            drive_phase_state_error_occured(PHASE_INVALID);
            return;
        case PHASE_A:
            drive_phase_state_error_occured(PHASE_BC_ERROR);
            return;
        case PHASE_B:
            drive_dir = DRIVE_DIR_FORW;
            break;
        case PHASE_C:
            drive_dir = DRIVE_DIR_BACKW;
            break;
    }
    
    if(state.drive_dir == DRIVE_DIR_UNK){
        state.drive_dir = drive_dir;
    }else if(state.drive_dir != drive_dir){
        if(state.drive_dir == DRIVE_DIR_FORW){
            drive_phase_state_error_occured(PHASE_B_ERROR);
        }else{//DRIVE_DIR_BACKW
            drive_phase_state_error_occured(PHASE_C_ERROR);
        }
    }
    
    state.cur_phase = phase;
}

static void drive_phase_state_b_process(phase_t phase)
{
    drive_dir_t drive_dir;
    switch(phase){
        default:
        case PHASE_UNK:
            drive_phase_state_error_occured(PHASE_INVALID);
            return;
        case PHASE_B:
            drive_phase_state_error_occured(PHASE_AC_ERROR);
            return;
        case PHASE_C:
            drive_dir = DRIVE_DIR_FORW;
            break;
        case PHASE_A:
            drive_dir = DRIVE_DIR_BACKW;
            break;
    }
    
    if(state.drive_dir == DRIVE_DIR_UNK){
        state.drive_dir = drive_dir;
    }else if(state.drive_dir != drive_dir){
        if(state.drive_dir == DRIVE_DIR_FORW){
            drive_phase_state_error_occured(PHASE_C_ERROR);
        }else{//DRIVE_DIR_BACKW
            drive_phase_state_error_occured(PHASE_A_ERROR);
        }
    }
    
    state.cur_phase = phase;
}

static void drive_phase_state_c_process(phase_t phase)
{
    drive_dir_t drive_dir;
    switch(phase){
        default:
        case PHASE_UNK:
            drive_phase_state_error_occured(PHASE_INVALID);
            return;
        case PHASE_C:
            drive_phase_state_error_occured(PHASE_AB_ERROR);
            return;
        case PHASE_A:
            drive_dir = DRIVE_DIR_FORW;
            break;
        case PHASE_B:
            drive_dir = DRIVE_DIR_BACKW;
            break;
    }
    
    if(state.drive_dir == DRIVE_DIR_UNK){
        state.drive_dir = drive_dir;
    }else if(state.drive_dir != drive_dir){
        if(state.drive_dir == DRIVE_DIR_FORW){
            drive_phase_state_error_occured(PHASE_A_ERROR);
        }else{//DRIVE_DIR_BACKW
            drive_phase_state_error_occured(PHASE_B_ERROR);
        }
    }
    
    state.cur_phase = phase;
}

err_t drive_phase_state_init(void)
{
    memset(&state, 0x0, sizeof(phase_state_t));
    
    return E_NO_ERROR;
}

void drive_phase_state_set_error_callback(drive_phase_state_error_callback_t callback)
{
    state.error_callback = callback;
}

err_t drive_phase_state_set_timer(TIM_TypeDef* TIM)
{
    if(TIM == NULL) return E_NULL_POINTER;
    
    state.timer_cnt = TIM;
    
    return E_NO_ERROR;
}

void drive_phase_state_handle(phase_t phase)
{
    phase_time_t time = drive_phase_state_get_cur_time();
    drive_phase_state_start_timer();
    
    switch(state.cur_phase){
        case PHASE_UNK:
            state.cur_phase = phase;
            return;
        case PHASE_A:
            drive_phase_state_a_process(phase);
            break;
        case PHASE_B:
            drive_phase_state_b_process(phase);
            break;
        case PHASE_C:
            drive_phase_state_c_process(phase);
            break;
    }
    
    drive_phase_state_set_time(phase, time);
    
    if(!drive_phase_state_time_check(time)){
        switch(phase){
            default:
                break;
            case PHASE_A:
                drive_phase_state_error_occured(PHASE_A_TIME_ERROR);
                break;
            case PHASE_B:
                drive_phase_state_error_occured(PHASE_B_TIME_ERROR);
                break;
            case PHASE_C:
                drive_phase_state_error_occured(PHASE_C_TIME_ERROR);
                break;
        }
    }
}

drive_phase_errors_t drive_phase_state_errors(void)
{
    return state.phase_errs;
}

phase_t drive_phase_sate_current_phase(void)
{
    return state.cur_phase;
}

drive_dir_t drive_phase_state_direction(void)
{
    return state.drive_dir;
}

phase_time_t drive_phase_state_phase_time(phase_t phase)
{
    if(phase == PHASE_UNK) return 0;
    
    return state.phases_time[phase - 1];
}

#define NEXT_PHASE_TABLE

phase_t drive_phase_state_next_phase(phase_t phase, drive_dir_t dir)
{
    if(phase == PHASE_UNK) return PHASE_UNK;
    if(dir == DRIVE_DIR_UNK) return PHASE_UNK;
    
#ifdef NEXT_PHASE_TABLE
    
#define PHASES_TABLE_SIZE_DIRS 2
#define PHASES_TABLE_SIZE_PHASES 3
    static const phase_t next_phase_table[PHASES_TABLE_SIZE_DIRS][PHASES_TABLE_SIZE_PHASES] = {
        {PHASE_B, PHASE_C, PHASE_A},
        {PHASE_C, PHASE_A, PHASE_B}
    };
    
    return next_phase_table[dir - 1][phase - 1];
    
#undef PHASES_TABLE_SIZE_PHASES
#undef PHASES_TABLE_SIZE_DIRS
    
#else
    
    // Обработаем фазу.
    switch(phase){
        case PHASE_A:
            return (dir == DRIVE_DIR_FORW) ? PHASE_B : PHASE_C;
            break;
        case PHASE_B:
            return (dir == DRIVE_DIR_FORW) ? PHASE_C : PHASE_A;
            break;
        case PHASE_C:
            return (dir == DRIVE_DIR_FORW) ? PHASE_A : PHASE_B;
            break;
        default:
            break;
    }
    
    return PHASE_UNK;
    
#endif
}

void drive_phase_state_clear_errors(void)
{
    state.phase_errs = PHASE_NO_ERROR;
}

void drive_phase_state_reset(void)
{
    state.cur_phase = PHASE_UNK;
    state.drive_dir = DRIVE_DIR_UNK;
    state.phase_errs = PHASE_NO_ERROR;
    drive_phase_state_stop_timer();
    drive_phase_state_reset_timer();
}

void drive_phase_state_timer_irq_handler(void)
{
    TIM_ClearITPendingBit(state.timer_cnt, TIM_IT_Update);
    
    switch(drive_phase_state_next_phase(state.cur_phase, state.drive_dir)){
        default:
            break;
        case PHASE_A:
            drive_phase_state_error_occured(PHASE_A_TIME_ERROR);
            break;
        case PHASE_B:
            drive_phase_state_error_occured(PHASE_B_TIME_ERROR);
            break;
        case PHASE_C:
            drive_phase_state_error_occured(PHASE_C_TIME_ERROR);
            break;
    }
}
