#include "drive_phase_state.h"
#include "defs/defs.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>


typedef enum _Phase_State {
    PHASE_STATE_UNK = 0,
    PHASE_STATE_A = 1,
    PHASE_STATE_B = 2,
    PHASE_STATE_C = 3,
    PHASE_STATE_A_FWD = 4,
    PHASE_STATE_B_FWD = 5,
    PHASE_STATE_C_FWD = 6,
    PHASE_STATE_A_BWD = 7,
    PHASE_STATE_B_BWD = 8,
    PHASE_STATE_C_BWD = 9,
    PHASE_STATE_ERROR = 10
} phase_state_t;

typedef struct _Phase_State_Values {
    phase_state_t state;
    drive_dir_t dir;
    drive_phase_error_t error;
} phase_state_values_t;

#define PHASE_STATES_COUNT 11
#define PHASE_VALUES_COUNT 4 /* UNK + A + B + C */

static const phase_state_values_t
phase_states [PHASE_STATES_COUNT][PHASE_VALUES_COUNT] = {
    { // PHASE_STATE_UNK
        { PHASE_STATE_UNK, DRIVE_DIR_UNK,     PHASE_NO_ERROR }, // PHASE_UNK
        { PHASE_STATE_A,   DRIVE_DIR_UNK,     PHASE_NO_ERROR }, // PHASE_A
        { PHASE_STATE_B,   DRIVE_DIR_UNK,     PHASE_NO_ERROR }, // PHASE_B
        { PHASE_STATE_C,   DRIVE_DIR_UNK,     PHASE_NO_ERROR }, // PHASE_C
    },
    { // PHASE_STATE_A
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_UNK_ERROR }, // PHASE_UNK
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_BC_ERROR }, // PHASE_A
        { PHASE_STATE_B_FWD, DRIVE_DIR_FORW,  PHASE_NO_ERROR }, // PHASE_B
        { PHASE_STATE_C_BWD, DRIVE_DIR_BACKW, PHASE_NO_ERROR }, // PHASE_C
    },
    { // PHASE_STATE_B
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_UNK_ERROR }, // PHASE_UNK
        { PHASE_STATE_A_BWD, DRIVE_DIR_BACKW, PHASE_NO_ERROR }, // PHASE_A
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_AC_ERROR }, // PHASE_B
        { PHASE_STATE_C_FWD, DRIVE_DIR_FORW,  PHASE_NO_ERROR }, // PHASE_C
    },
    { // PHASE_STATE_C
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_UNK_ERROR }, // PHASE_UNK
        { PHASE_STATE_A_FWD, DRIVE_DIR_FORW,  PHASE_NO_ERROR }, // PHASE_A
        { PHASE_STATE_B_BWD, DRIVE_DIR_BACKW, PHASE_NO_ERROR }, // PHASE_B
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_AB_ERROR }, // PHASE_C
    },
    { // PHASE_STATE_A_FWD
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_UNK_ERROR }, // PHASE_UNK
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_BC_ERROR }, // PHASE_A
        { PHASE_STATE_B_FWD, DRIVE_DIR_FORW,  PHASE_NO_ERROR }, // PHASE_B
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_B_ERROR }, // PHASE_C
    },
    { // PHASE_STATE_B_FWD
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_UNK_ERROR }, // PHASE_UNK
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_C_ERROR }, // PHASE_A
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_AC_ERROR }, // PHASE_B
        { PHASE_STATE_C_FWD, DRIVE_DIR_FORW,  PHASE_NO_ERROR }, // PHASE_C
    },
    { // PHASE_STATE_C_FWD
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_UNK_ERROR }, // PHASE_UNK
        { PHASE_STATE_A_FWD, DRIVE_DIR_FORW,  PHASE_NO_ERROR }, // PHASE_A
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_A_ERROR }, // PHASE_B
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_AB_ERROR }, // PHASE_C
    },
    { // PHASE_STATE_A_BWD
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_UNK_ERROR }, // PHASE_UNK
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_BC_ERROR }, // PHASE_A
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_C_ERROR }, // PHASE_B
        { PHASE_STATE_C_BWD, DRIVE_DIR_BACKW, PHASE_NO_ERROR }, // PHASE_C
    },
    { // PHASE_STATE_B_BWD
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_UNK_ERROR }, // PHASE_UNK
        { PHASE_STATE_A_BWD, DRIVE_DIR_BACKW, PHASE_NO_ERROR }, // PHASE_A
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_AC_ERROR }, // PHASE_B
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_A_ERROR }, // PHASE_C
    },
    { // PHASE_STATE_C_BWD
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_UNK_ERROR }, // PHASE_UNK
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_B_ERROR }, // PHASE_A
        { PHASE_STATE_B_BWD, DRIVE_DIR_BACKW, PHASE_NO_ERROR }, // PHASE_B
        { PHASE_STATE_ERROR, DRIVE_DIR_UNK,   PHASE_AB_ERROR }, // PHASE_C
    },
    { // PHASE_STATE_ERROR
        { PHASE_STATE_UNK, DRIVE_DIR_UNK,     PHASE_NO_ERROR }, // PHASE_UNK
        { PHASE_STATE_A, DRIVE_DIR_UNK,       PHASE_NO_ERROR }, // PHASE_A
        { PHASE_STATE_B, DRIVE_DIR_UNK,       PHASE_NO_ERROR }, // PHASE_B
        { PHASE_STATE_C, DRIVE_DIR_UNK,       PHASE_NO_ERROR }, // PHASE_C
    }/*,
    { // PHASE_STATE_
        { PHASE_STATE_, DRIVE_DIR_, PHASE__ERROR }, // PHASE_UNK
        { PHASE_STATE_, DRIVE_DIR_, PHASE__ERROR }, // PHASE_A
        { PHASE_STATE_, DRIVE_DIR_, PHASE__ERROR }, // PHASE_B
        { PHASE_STATE_, DRIVE_DIR_, PHASE__ERROR }, // PHASE_C
    },*/
};

#define PHASE_TIME_ERRORS_COUNT 3
static const drive_phase_error_t
phase_time_errors[PHASE_TIME_ERRORS_COUNT] = {
    PHASE_A_TIME_ERROR,
    PHASE_B_TIME_ERROR,
    PHASE_C_TIME_ERROR
};

//! Тип состояния фазы.
typedef struct _DrivePhaseState {
    phase_state_t state; //!< Состояние фаз.
    phase_t cur_phase; //!< Текущая фаза.
    drive_dir_t drive_dir; //!< Направление.
    drive_phase_errors_t phase_errs; //!< Ошибки.
    TIM_TypeDef* timer_cnt; //!< Таймер - счётчик времени между фазами.
    phase_time_t phases_time[PHASES_COUNT]; //!< Время между датчиками нуля.
    drive_phase_state_error_callback_t error_callback; //!< Каллбэк ошибки фаз.
} drive_phase_state_t;

static drive_phase_state_t state;


ALWAYS_INLINE static void drive_phase_state_on_error(void)
{
    if(state.error_callback) state.error_callback();
}

ALWAYS_INLINE static void drive_phase_state_set_time(phase_t phase, phase_time_t time)
{
    if(phase != PHASE_UNK){
        state.phases_time[phase - 1] = time;
    }
}

ALWAYS_INLINE static drive_phase_error_t drive_phase_state_get_time_error(phase_t phase)
{
    if(phase == PHASE_UNK) return PHASE_UNK_TIME_ERROR;
    
    return phase_time_errors[phase - 1];
}

ALWAYS_INLINE static phase_time_t drive_phase_state_get_cur_time(void)
{
    if(state.timer_cnt){
        //return TIM_GetCounter(state.timer_cnt);
        uint16_t time = TIM_GetCounter(state.timer_cnt);
        if(time != 0) return (phase_time_t)time;
        return PHASE_TIME_US_MAX + 1;
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

err_t drive_phase_state_init(void)
{
    memset(&state, 0x0, sizeof(drive_phase_state_t));
    
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
    drive_phase_state_stop_timer();
    phase_time_t time = drive_phase_state_get_cur_time();
    drive_phase_state_start_timer();
    
    const phase_state_values_t* state_values = 
                    &phase_states[state.state][phase];
    
    state.state = state_values->state;
    state.drive_dir = state_values->dir;
    state.phase_errs = state_values->error;
    state.cur_phase = phase;
    
    drive_phase_state_set_time(phase, time);
    
    if(!drive_phase_state_time_check(time)){
        state.phase_errs |= drive_phase_state_get_time_error(phase);
    }
    
    if(state.phase_errs != PHASE_NO_ERROR) drive_phase_state_on_error();
}

drive_phase_errors_t drive_phase_state_errors(void)
{
    return state.phase_errs;
}

phase_t drive_phase_state_current_phase(void)
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
    state.state = PHASE_STATE_UNK;
    drive_phase_state_stop_timer();
    drive_phase_state_reset_timer();
}

void drive_phase_state_process_phase_timeout(void)
{
    if(state.timer_cnt) TIM_ClearITPendingBit(state.timer_cnt, TIM_IT_Update);
    
    phase_time_t time = PHASE_TIME_US_MAX + 1;
    phase_t phase = drive_phase_state_next_phase(state.cur_phase, state.drive_dir);
    
    drive_phase_state_set_time(phase, time);
    
    state.phase_errs |= drive_phase_state_get_time_error(phase);
    
    drive_phase_state_on_error();
}
