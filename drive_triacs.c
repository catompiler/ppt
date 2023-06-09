#include "drive_triacs.h"
#include <string.h>
#include "defs/defs.h"
#include "utils/utils.h"
#include "triac.h"
#include "triac_pair.h"



//! Минимальный угол открытия тиристоров в тиках таймера.
#define TRIACS_TIM_MIN_TICKS (1)
//! Максимальный угол открытия тиристоров в тиках таймера.
#define TRIACS_TIM_MAX_TICKS (TRIACS_TIM_TICKS / 3)
//! Максимальный угол открытия тиристоров.
#define TRIACS_TIM_MAX_TICKS_ANGLE (360 / 3)
//! Смещение между первой и второй парой тиристоров.
#define TRIACS_TIM_OFFSET (TRIACS_TIM_MAX_TICKS / 2)


//! Минимальное число тиков таймера для открытия симистора возбуждения.
#define TRIAC_EXC_MIN_TICKS (1)
//! Минимальный угол включения симистора возбуждения в тиках таймера.
#define TRIAC_EXC_MAX_TICKS (TRIAC_EXC_TIM_TICKS / 2)
//! Минимальный угол включения симистора возбуждения.
#define TRIAC_EXC_MAX_TICKS_ANGLE (360 / 2)
//! Смещение между полупериодами.
#define TRIAC_EXC_TIM_HALF_CYCLE_OFFSET (TRIAC_EXC_TIM_TICKS / 2)
//! Смещение до начала периода.
#define TRIAC_EXC_OFFSET (TRIAC_EXC_TIM_TICKS * 30 / 360)


//! Максимальный коэффициент заполнения гребёнки.
#define TRIACS_PULSE_TRAIN_DUTY_RATIO_MAX (0x8000) // 0.5f


// Смещения в массивах последовательностей направления для датчиков нуля.
//! Датчик нуля фазы A.
#define PHASE_A_NULL_TRIAC_OFFSET 0
//! Датчик нуля фазы B.
#define PHASE_B_NULL_TRIAC_OFFSET 2
//! Датчик нуля фазы C.
#define PHASE_C_NULL_TRIAC_OFFSET 4

//! Последовательность открытия тиристоров для прямого направления.
static const triac_pair_number_t triac_open_seq_fwd[TRIAC_PAIRS_COUNT] = {
    TRIAC_PAIR_3_6,
    TRIAC_PAIR_3_2,
    TRIAC_PAIR_5_2,
    TRIAC_PAIR_5_4,
    TRIAC_PAIR_1_4,
    TRIAC_PAIR_1_6
};

//! Последовательность открытия тиристоров для обратного направления.
static const triac_pair_number_t triac_open_seq_bwd[TRIAC_PAIRS_COUNT] = {
    TRIAC_PAIR_5_2,
    TRIAC_PAIR_3_2,
    TRIAC_PAIR_1_4,
    TRIAC_PAIR_5_4,
    TRIAC_PAIR_3_6,
    TRIAC_PAIR_1_6,
};

//! Тип структуры таймера для открытия тиристорных пар.
typedef struct _Timer_Triacs {
    TIM_TypeDef* timer; //!< Таймер для включения тиристоров.
    triac_pair_number_t triacs_a; //!< Пара тиристоров А.
    //triac_pair_number_t triacs_b; //!< Пара тиристоров B.
    uint16_t pulse_train_remain; //!< Оставшееся время гребёнки.
    int16_t pulse_train_offset; //!< Смещение импульса открытия.
} timer_triacs_t;

//! Тип структуры тиристоров привода.
typedef struct _Drive_Triacs {
    bool pairs_enabled; //!< Разрешение подачи импульсов на тиристорные пары.
    bool exc_enabled; //!< Разрешение подачи импульсов на симистор возбуждения.
    
    fixed32_t triacs_pairs_min_angle; //!< Минимальный угол открытия тиристорных пар.
    fixed32_t triacs_pairs_max_angle; //!< Максимальный угол открытия тиристорных пар.
    
    fixed32_t triac_exc_min_angle; //!< Минимальный угол открытия симистора возбуждения.
    fixed32_t triac_exc_max_angle; //!< Максимальный угол открытия симистора возбуждения.
    fixed32_t triac_exc_max_angle_ticks; //!< Максимальный угол открытия симистора в тиках таймера.
    
    fixed32_t triacs_pairs_open_angle; //!< Угол открытия тиристорных пар.
    fixed32_t triac_exc_open_angle; //!< Угол открытия симистора возбуждения.
    
    uint16_t triacs_pairs_angle_ticks; //!< Угол открытия тиристорных пар - значение регистра сравнения таймера.
    uint16_t triacs_pairs_open_ticks; //!< Время открытия тиристорных пар в тиках таймера.
    uint16_t triacs_pairs_delay_ticks; //!< Время задержки открытия тиристорных пар в тиках таймера.
    
    uint16_t triac_exc_angle_ticks; //!< Угол открытия симистора возбуждения - значение регистра сравнения таймера.
    uint16_t triac_exc_open_ticks; //!< Время открытия симистора возбуждения в тиках таймера.
    uint16_t triac_exc_delay_ticks; //!< Время задержки открытия симистора возбуждения в тиках таймера.

    bool triacs_pairs_pt_enabled; //!< Разрешение гребёнки тиристорных пар.
    bool triac_exc_pt_enabled; //!< Разрешение гребёнки симистора возбуждения.

    fixed32_t triacs_pairs_pt_width; //!< Длина гребёнки тиристорных пар.
    uint16_t triacs_pairs_pt_width_ticks; //!< Длина гребёнки тиристорных пар в тиках таймера.
    fixed32_t triac_exc_pt_width; //!< Длина гребёнки симистора возбуждения.
    uint16_t triac_exc_pt_width_ticks; //!< Длина гребёнки симистора возбуждения в тиках таймера.

    fixed32_t triacs_pairs_pt_duty; //!< Коэффициент заполнения гребёнки тиристорных пар.
    uint16_t triacs_pairs_pt_open_delta; //!< Время между импульсами открытия гребёнки тиристорных пар.
    fixed32_t triac_exc_pt_duty; //!< Коэффициент заполнения гребёнки симистора возбуждения.
    uint16_t triac_exc_pt_open_delta; //!< Время между импульсами открытия гребёнки симистора возбуждения.

    fixed32_t triacs_pairs_pt_angle_min; //!< Минимальный угол гребёнки тиристорных пар.
    uint16_t triacs_pairs_pt_angle_min_ticks; //!< Минимальный угол гребёнки тиристорных пар в тиках таймера.
    fixed32_t triac_exc_pt_angle_min; //!< Минимальный угол гребёнки симистора возбуждения.
    uint16_t triac_exc_pt_angle_min_ticks; //!< Минимальный угол гребёнки симистора возбуждения в тиках таймера.

    triac_pair_t triac_pairs[TRIAC_PAIRS_COUNT]; //!< Пары тиристоров.
    triac_t triac_exc; //!< Тиристор возбуждения.
    
    timer_triacs_t timers_triacs[TRIACS_TIMERS_COUNT]; //!< Тиристоры таймеров и таймеры.
    size_t current_timer_triacs; //!< Текущий индекс таймеров тиристоров.
    
    TIM_TypeDef* timer_exc; //!< Таймер для открытия симистора возбуждения.
    uint16_t exc_pulse_train_remain_first; //!< Оставшееся время гребёнки симистора возбуждения первого полупериода.
    uint16_t exc_pulse_train_remain_second; //!< Оставшееся время гребёнки симистора возбуждения второго полупериода.
    int16_t exc_pulse_train_offset; //!< Смещение открытия.
    
    phase_t phase_exc; //!< Фаза возбуждения.
    drive_triacs_exc_mode_t exc_mode; //!< Режим возбуждения.
    
    triac_pair_number_t last_opened_pair; //!< Последняя открытая пара тиристоров.
    
    drive_triacs_open_pair_callback_t open_pair_callback;
} drive_triacs_t;


static drive_triacs_t drive_triacs;



err_t drive_triacs_init(void)
{
    memset(&drive_triacs, 0x0, sizeof(drive_triacs_t));
    
    drive_triacs.triacs_pairs_min_angle = TRIACS_PAIRS_ANGLE_MIN_F;
    drive_triacs.triacs_pairs_max_angle = TRIACS_PAIRS_ANGLE_MAX_F;
    
    drive_triacs.triac_exc_min_angle = TRIAC_EXC_ANGLE_MIN_F;
    drive_triacs.triac_exc_max_angle = TRIAC_EXC_ANGLE_MAX_F;
    drive_triacs.triac_exc_max_angle_ticks = TRIAC_EXC_MAX_TICKS;
    
    drive_triacs.triacs_pairs_open_ticks = TRIACS_TIM_OPEN_TIME_DEFAULT;
    drive_triacs.triac_exc_open_ticks = TRIAC_EXC_TIM_OPEN_TIME_DEFAULT;
    
    drive_triacs.triacs_pairs_delay_ticks = 0;
    drive_triacs.triac_exc_delay_ticks = 0;
    
    drive_triacs.last_opened_pair = TRIAC_PAIR_NONE;
    
    return E_NO_ERROR;
}

drive_triacs_open_pair_callback_t drive_triacs_open_pair_callback(void)
{
    return drive_triacs.open_pair_callback;
}

void drive_triacs_set_open_pair_callback(drive_triacs_open_pair_callback_t callback)
{
    drive_triacs.open_pair_callback = callback;
}

bool drive_triacs_phases_by_pair(triac_pair_number_t pair_number, phase_t* open_hi, phase_t* open_lo, phase_t* closed)
{
    static const phase_t open_hi_table[TRIAC_PAIRS_COUNT] = {
        PHASE_B, PHASE_B, PHASE_C, PHASE_C, PHASE_A, PHASE_A
    };
    static const phase_t open_lo_table[TRIAC_PAIRS_COUNT] = {
        PHASE_C, PHASE_A, PHASE_A, PHASE_B, PHASE_B, PHASE_C
    };
    static const phase_t closed_table[TRIAC_PAIRS_COUNT] = {
        PHASE_A, PHASE_C, PHASE_B, PHASE_A, PHASE_C, PHASE_B
    };
    
    if(!TRIAC_PAIR_VALID(pair_number)) return false;
    
    if(open_hi) *open_hi = open_hi_table[pair_number];
    if(open_lo) *open_lo = open_lo_table[pair_number];
    if(closed) *closed = closed_table[pair_number];
    
    return true;
}

bool drive_triacs_phase_triac_is_open(triac_pair_number_t pair_number, phase_t phase, triac_pos_t pos)
{
    phase_t open_hi = PHASE_UNK;
    phase_t open_lo = PHASE_UNK;
    phase_t closed = PHASE_UNK;
    
    if(!drive_triacs_phases_by_pair(pair_number, &open_hi, &open_lo, &closed)) return false;
    
    if(phase == closed) return false;
    
    if(phase == open_hi && pos == TRIAC_POS_HI) return true;
    if(phase == open_lo && pos == TRIAC_POS_LO) return true;
    
    return false;
}

bool drive_triacs_triac_is_open(triac_pair_number_t pair_number, triac_number_t triac_number)
{
    static const triac_number_t triacs_hi_table[TRIAC_PAIRS_COUNT] = {
        TRIAC_3, TRIAC_3, TRIAC_5, TRIAC_5, TRIAC_1, TRIAC_1
    };
    static const triac_number_t triacs_lo_table[TRIAC_PAIRS_COUNT] = {
        TRIAC_6, TRIAC_2, TRIAC_2, TRIAC_4, TRIAC_4, TRIAC_6
    };
    
    if(!TRIAC_PAIR_VALID(pair_number)) return false;
    if(!TRIAC_VALID(triac_number)) return false;
    
    if(triacs_hi_table[pair_number] == triac_number) return true;
    if(triacs_lo_table[pair_number] == triac_number) return true;
    
    return false;
}

triac_number_t drive_triacs_phase_triac_by_pos(phase_t phase, triac_pos_t pos)
{
    if(phase == PHASE_UNK) return TRIAC_UNKNOWN;
    
    static const triac_number_t phase_triacs_hi_table[PHASES_COUNT] = {
        TRIAC_1, TRIAC_3, TRIAC_5
    };
    static const triac_number_t phase_triacs_lo_table[PHASES_COUNT] = {
        TRIAC_2, TRIAC_4, TRIAC_6
    };
    
    if(pos == TRIAC_POS_HI) return phase_triacs_hi_table[phase - 1];
    return phase_triacs_lo_table[phase - 1];
}

bool drive_triacs_pairs_enabled(void)
{
    return drive_triacs.pairs_enabled;
}

void drive_triacs_set_pairs_enabled(bool enabled)
{
    if(enabled && !drive_triacs.pairs_enabled) drive_triacs.last_opened_pair = TRIAC_PAIR_NONE;
    drive_triacs.pairs_enabled = enabled;
}

bool drive_triacs_exc_enabled(void)
{
    return drive_triacs.exc_enabled;
}

void drive_triacs_set_exc_enabled(bool enabled)
{
    drive_triacs.exc_enabled = enabled;
    
    if(drive_triacs.exc_mode == DRIVE_TRIACS_EXC_FIXED){
        if(enabled){
            triac_open(&drive_triacs.triac_exc);
        }else{
            triac_close(&drive_triacs.triac_exc);
        }
    }
}

drive_triacs_exc_mode_t drive_triacs_exc_mode(void)
{
    return drive_triacs.exc_mode;
}

void drive_triacs_set_exc_mode(drive_triacs_exc_mode_t mode)
{
    drive_triacs.exc_mode = mode;
}

/**
 * Получает тиристорную пару с заданным номером.
 * @param n Номер тиристорной пары.
 * @return Тиристорная пара.
 */
ALWAYS_INLINE static triac_pair_t* get_triac_pair(triac_pair_number_t n)
{
    return &drive_triacs.triac_pairs[n];
}

/**
 * Останавливает таймеры открытия тиристоров и симистора возбуждения.
 */
static void drive_triacs_stop_timers(void)
{
    size_t i;
    for(i = 0; i < TRIACS_TIMERS_COUNT; i ++) {
        TIM_Cmd(drive_triacs.timers_triacs[i].timer, DISABLE);
    }
    TIM_Cmd(drive_triacs.timer_exc, DISABLE);
}

/**
 * Закрывает тиристоры и симистор возбуждения.
 */
static void drive_triacs_close_triacs(void)
{
    size_t i;
    for(i = 0; i < TRIAC_PAIRS_COUNT; i ++) {
        triac_pair_close(&drive_triacs.triac_pairs[i]);
    }
    triac_close(&drive_triacs.triac_exc);
}

void drive_triacs_stop(void)
{
    drive_triacs_stop_timers();
    drive_triacs_close_triacs();
}

triac_pair_number_t drive_triacs_opened_pair(void)
{
    if(!drive_triacs.pairs_enabled) return TRIAC_PAIR_NONE;
    
    return drive_triacs.last_opened_pair;
}

triac_pair_number_t drive_triacs_last_opened_pair(void)
{
    return drive_triacs.last_opened_pair;
}

err_t drive_triacs_clamp_pairs_open_angle(fixed32_t angle_min, fixed32_t angle_max)
{
    err_t err = E_NO_ERROR;
    
    if(angle_min < TRIACS_PAIRS_ANGLE_MIN_F){
        angle_min = TRIACS_PAIRS_ANGLE_MIN_F;
        err = E_OUT_OF_RANGE;
    }
    if(angle_max > TRIACS_PAIRS_ANGLE_MAX_F){
        angle_max = TRIACS_PAIRS_ANGLE_MAX_F;
        err = E_OUT_OF_RANGE;
    }
    
    drive_triacs.triacs_pairs_min_angle = angle_min;
    drive_triacs.triacs_pairs_max_angle = angle_max;
    
    return err;
}

err_t drive_triacs_clamp_exc_open_angle(fixed32_t angle_min, fixed32_t angle_max)
{
    err_t err = E_NO_ERROR;
    
    if(angle_min < TRIAC_EXC_ANGLE_MIN_F){
        angle_min = TRIAC_EXC_ANGLE_MIN_F;
        err = E_OUT_OF_RANGE;
    }
    if(angle_max > TRIAC_EXC_ANGLE_MAX_F){
        angle_max = TRIAC_EXC_ANGLE_MAX_F;
        err = E_OUT_OF_RANGE;
    }
    
    drive_triacs.triac_exc_min_angle = angle_min;
    drive_triacs.triac_exc_max_angle = angle_max;
    
    angle_max /= TRIAC_EXC_MAX_TICKS_ANGLE;
    drive_triacs.triac_exc_max_angle_ticks = fixed32_mul(angle_max, TRIAC_EXC_MAX_TICKS);

    return err;
}

fixed32_t drive_triacs_pairs_open_angle(void)
{
    if(!drive_triacs.pairs_enabled) return 0;
    return drive_triacs.triacs_pairs_open_angle;
}

fixed32_t drive_triacs_pairs_start_open_angle(void)
{
    return TRIACS_PAIRS_ANGLE_MAX_F - drive_triacs_pairs_open_angle();
}

err_t drive_triacs_set_pairs_open_angle(fixed32_t angle)
{
    err_t err = E_NO_ERROR;
    if(angle < drive_triacs.triacs_pairs_min_angle){
        angle = drive_triacs.triacs_pairs_min_angle;
        err = E_OUT_OF_RANGE;
    }
    if(angle > drive_triacs.triacs_pairs_max_angle){
        angle = drive_triacs.triacs_pairs_max_angle;
        err = E_OUT_OF_RANGE;
    }
    
    drive_triacs.triacs_pairs_open_angle = angle;
    
    angle /= TRIACS_TIM_MAX_TICKS_ANGLE;
    drive_triacs.triacs_pairs_angle_ticks = fixed32_mul(angle, TRIACS_TIM_MAX_TICKS);
    return err;
}

fixed32_t drive_triacs_exc_open_angle(void)
{
    if(!drive_triacs.exc_enabled) return 0;
    if(drive_triacs.exc_mode == DRIVE_TRIACS_EXC_EXTERNAL) return 0;
    if(drive_triacs.exc_mode == DRIVE_TRIACS_EXC_FIXED) return TRIAC_EXC_ANGLE_MAX_F;
    if(drive_triacs.exc_mode == DRIVE_TRIACS_EXC_FIXED_PULSE) return drive_triacs.triac_exc_max_angle;
    return drive_triacs.triac_exc_open_angle;
}

fixed32_t drive_triacs_exc_start_open_angle(void)
{
    return TRIAC_EXC_ANGLE_MAX_F - drive_triacs_exc_open_angle();
}

err_t drive_triacs_set_exc_open_angle(fixed32_t angle)
{
    err_t err = E_NO_ERROR;
    if(angle < drive_triacs.triac_exc_min_angle){
        angle = drive_triacs.triac_exc_min_angle;
        err = E_OUT_OF_RANGE;
    }
    if(angle > drive_triacs.triac_exc_max_angle){
        angle = drive_triacs.triac_exc_max_angle;
        err = E_OUT_OF_RANGE;
    }
    
    drive_triacs.triac_exc_open_angle = angle;
    
    angle /= TRIAC_EXC_MAX_TICKS_ANGLE;
    drive_triacs.triac_exc_angle_ticks = fixed32_mul(angle, TRIAC_EXC_MAX_TICKS);
    return err;
}

uint16_t drive_triacs_pairs_open_time_us(void)
{
    return TICKS_TO_TIME(drive_triacs.triacs_pairs_open_ticks);
}

err_t drive_triacs_set_pairs_open_time_us(uint16_t time)
{
    if(time == 0) return E_INVALID_VALUE;
    
    drive_triacs.triacs_pairs_open_ticks = TIME_TO_TICKS(time);
    
    return E_NO_ERROR;
}

uint16_t drive_triacs_exc_open_time_us(void)
{
    return TICKS_TO_TIME(drive_triacs.triac_exc_open_ticks);
}

err_t drive_triacs_set_exc_open_time_us(uint16_t time)
{
    if(time == 0) return E_INVALID_VALUE;
    
    drive_triacs.triac_exc_open_ticks = TIME_TO_TICKS(time);
    
    return E_NO_ERROR;
}

uint16_t drive_triacs_pairs_open_delay_us(void)
{
    return TICKS_TO_TIME(drive_triacs.triacs_pairs_delay_ticks);
}

err_t drive_triacs_set_pairs_open_delay_us(uint16_t time)
{
    drive_triacs.triacs_pairs_delay_ticks = TIME_TO_TICKS(time);
    
    return E_NO_ERROR;
}

uint16_t drive_triacs_exc_open_delay_us(void)
{
    return TICKS_TO_TIME(drive_triacs.triac_exc_delay_ticks);
}

err_t drive_triacs_set_exc_open_delay_us(uint16_t time)
{
    drive_triacs.triac_exc_delay_ticks = TIME_TO_TICKS(time);
    
    return E_NO_ERROR;
}

bool drive_triacs_pairs_pulse_train_enabled(void)
{
    return drive_triacs.triacs_pairs_pt_enabled;
}

void drive_triacs_set_pairs_pulse_train_enabled(bool enabled)
{
    drive_triacs.triacs_pairs_pt_enabled = enabled;
}

bool drive_triacs_exc_pulse_train_enabled(void)
{
    return drive_triacs.triac_exc_pt_enabled;
}

void drive_triacs_set_exc_pulse_train_enabled(bool enabled)
{
    drive_triacs.triac_exc_pt_enabled = enabled;
}

fixed32_t drive_triacs_pairs_pulse_train_width(void)
{
    return drive_triacs.triacs_pairs_pt_width;
}

void drive_triacs_set_pairs_pulse_train_width(fixed32_t width)
{
    fixed32_t angle_width = drive_triacs.triacs_pairs_max_angle -
                            drive_triacs.triacs_pairs_min_angle;

    width = CLAMP(width, 0, angle_width);

    drive_triacs.triacs_pairs_pt_width = width;

    width /= TRIACS_TIM_MAX_TICKS_ANGLE;
    drive_triacs.triacs_pairs_pt_width_ticks = fixed32_mul(width, TRIACS_TIM_MAX_TICKS);
}

fixed32_t drive_triacs_exc_pulse_train_width(void)
{
    return drive_triacs.triac_exc_pt_width;
}

void drive_triacs_set_exc_pulse_train_width(fixed32_t width)
{
    fixed32_t angle_width = drive_triacs.triac_exc_max_angle -
                            drive_triacs.triac_exc_min_angle;

    width = CLAMP(width, 0, angle_width);

    drive_triacs.triac_exc_pt_width = width;

    width /= TRIAC_EXC_MAX_TICKS_ANGLE;
    drive_triacs.triac_exc_pt_width_ticks = fixed32_mul(width, TRIAC_EXC_MAX_TICKS);
}

fixed32_t drive_triacs_pairs_pulse_train_duty_ratio(void)
{
    return drive_triacs.triacs_pairs_pt_duty;
}

void drive_triacs_set_pairs_pulse_train_duty_ratio(fixed32_t duty_ratio)
{
    duty_ratio = CLAMP(duty_ratio, 0, TRIACS_PULSE_TRAIN_DUTY_RATIO_MAX);

    fixed32_t delta_ratio = fixed32_make_from_int(1) - duty_ratio;

    // b = a * (1 - duty) / duty;
    // k = (1 - d) / d;
    fixed32_t k = fixed32_div((int64_t)delta_ratio, duty_ratio);

    int32_t open_ticks = drive_triacs.triacs_pairs_open_ticks;
    fixed32_t open_delta_ticks_f = open_ticks * k;

    drive_triacs.triacs_pairs_pt_open_delta = fixed32_get_int(open_delta_ticks_f);
}

fixed32_t drive_triacs_exc_pulse_train_duty_ratio(void)
{
    return drive_triacs.triac_exc_pt_duty;
}

void drive_triacs_set_exc_pulse_train_duty_ratio(fixed32_t duty_ratio)
{
    duty_ratio = CLAMP(duty_ratio, 0, TRIACS_PULSE_TRAIN_DUTY_RATIO_MAX);

    fixed32_t delta_ratio = fixed32_make_from_int(1) - duty_ratio;

    // b = a * (1 - duty) / duty;
    // k = (1 - d) / d;
    fixed32_t k = fixed32_div((int64_t)delta_ratio, duty_ratio);

    int32_t open_ticks = drive_triacs.triac_exc_open_ticks;
    fixed32_t open_delta_ticks_f = open_ticks * k;

    drive_triacs.triac_exc_pt_open_delta = fixed32_get_int(open_delta_ticks_f);
}

fixed32_t drive_triacs_pairs_pulse_train_angle_min(void)
{
    return drive_triacs.triacs_pairs_pt_angle_min;
}

void drive_triacs_set_pairs_pulse_train_angle_min(fixed32_t angle_min)
{
    angle_min = CLAMP(angle_min, TRIACS_PAIRS_ANGLE_MIN_F, TRIACS_PAIRS_ANGLE_MAX_F);

    drive_triacs.triacs_pairs_pt_angle_min = angle_min;

    angle_min /= TRIACS_TIM_MAX_TICKS_ANGLE;
    drive_triacs.triacs_pairs_pt_angle_min_ticks = fixed32_mul(angle_min, TRIACS_TIM_MAX_TICKS);
}

fixed32_t drive_triacs_exc_pulse_train_angle_min(void)
{
    return drive_triacs.triac_exc_pt_angle_min;
}

void drive_triacs_set_exc_pulse_train_angle_min(fixed32_t angle_min)
{
    angle_min = CLAMP(angle_min, TRIAC_EXC_ANGLE_MIN_F, TRIAC_EXC_ANGLE_MAX_F);

    drive_triacs.triac_exc_pt_angle_min = angle_min;

    angle_min /= TRIAC_EXC_MAX_TICKS_ANGLE;
    drive_triacs.triac_exc_pt_angle_min_ticks = fixed32_mul(angle_min, TRIAC_EXC_MAX_TICKS);
}

phase_t drive_triacs_exc_phase(void)
{
    return drive_triacs.phase_exc;
}

err_t drive_triacs_set_exc_phase(phase_t phase)
{
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    
    drive_triacs.phase_exc = phase;
    
    return E_NO_ERROR;
}

err_t drive_triacs_set_pair_gpio(triac_pair_number_t triac_pair, GPIO_TypeDef* GPIO_a, uint16_t pin_a, GPIO_TypeDef* GPIO_b, uint16_t pin_b)
{
    if(triac_pair >= TRIAC_PAIRS_COUNT || triac_pair < 0) return E_OUT_OF_RANGE;
    return triac_pair_init(&drive_triacs.triac_pairs[triac_pair], GPIO_a, pin_a, GPIO_b, pin_b);
}

err_t drive_triacs_set_exc_gpio(GPIO_TypeDef* GPIO, uint16_t pin)
{
    return triac_init(&drive_triacs.triac_exc, GPIO, pin);
}

err_t drive_triacs_set_pairs_timer(size_t index, TIM_TypeDef* TIM)
{
    if(index >= TRIACS_TIMERS_COUNT) return E_OUT_OF_RANGE;
    if(TIM == NULL) return E_NULL_POINTER;
    
    drive_triacs.timers_triacs[index].timer = TIM;
    
    return E_NO_ERROR;
}

err_t drive_triacs_set_exc_timer(TIM_TypeDef* TIM)
{
    if(TIM == NULL) return E_NULL_POINTER;
    
    drive_triacs.timer_exc = TIM;
    
    return E_NO_ERROR;
}

/**
 * Получает таймер тиристоров с заданным номером.
 * @param n Номер таймера тиристоров.
 * @return Таймер тиристоров.
 */
ALWAYS_INLINE static timer_triacs_t* get_timer_triacs(triac_pair_number_t n)
{
    return &drive_triacs.timers_triacs[n];
}

/**
 * Получает текущуий таймер тиристоров.
 * @return Текущий таймер тиристоров.
 */
ALWAYS_INLINE static timer_triacs_t* timer_triacs_current(void)
{
    return &drive_triacs.timers_triacs[drive_triacs.current_timer_triacs];
}

/**
 * Устанавливает следующий таймер тиристоров.
 * @return Следующий таймер тиристоров.
 */
static timer_triacs_t* timer_triacs_next(void)
{
    if(++ drive_triacs.current_timer_triacs >= TRIACS_TIMERS_COUNT){
        drive_triacs.current_timer_triacs = 0;
    }
    return timer_triacs_current();
}

ALWAYS_INLINE static void drive_triacs_on_open_pair(void)
{
    if(drive_triacs.open_pair_callback) drive_triacs.open_pair_callback(drive_triacs.last_opened_pair);
}

static void drive_triacs_pairs_pulse_train_setup_next(timer_triacs_t* tim_triacs)
{
    if(!drive_triacs.triacs_pairs_pt_enabled) return;

    uint16_t cur_ticks = TIM_GetCounter(tim_triacs->timer);
    uint16_t pulse = drive_triacs.triacs_pairs_open_ticks + drive_triacs.triacs_pairs_pt_open_delta;

    if(tim_triacs->pulse_train_remain < pulse) return;

    tim_triacs->pulse_train_remain -= pulse;

    uint16_t next_open_pulse = cur_ticks + drive_triacs.triacs_pairs_pt_open_delta;
    uint16_t next_close_pulse = cur_ticks + pulse;

    if(next_close_pulse < TRIACS_TIM_MAX_TICKS -
            drive_triacs.triacs_pairs_pt_angle_min_ticks - tim_triacs->pulse_train_offset){
        TIM_SetCompare1(tim_triacs->timer, next_open_pulse);
        TIM_SetCompare2(tim_triacs->timer, next_close_pulse);

        TIM_ITConfig(tim_triacs->timer, TRIACS_A_OPEN_CHANNEL_IT, ENABLE);
        TIM_ITConfig(tim_triacs->timer, TRIACS_A_CLOSE_CHANNEL_IT, ENABLE);
    }
}

static void drive_triacs_timer_irq_handler_impl(timer_triacs_t* tim_triacs)
{
    TIM_TypeDef* TIM = tim_triacs->timer;
    // Если нужно открыть тиристорную пару 1.
    if(TIM_GetITStatus(TIM, TRIACS_A_OPEN_CHANNEL_IT) != RESET){
        TIM_ClearITPendingBit(TIM, TRIACS_A_OPEN_CHANNEL_IT);
        TIM_ITConfig(TIM, TRIACS_A_OPEN_CHANNEL_IT, DISABLE);
        
        // Если подача импульсов на тиристорные пары разрешена.
        if(drive_triacs.pairs_enabled){
            triac_pair_open(
                    get_triac_pair(tim_triacs->triacs_a)
                );
            //drive_triacs.last_opened_pair = tim_triacs->triacs_a;
            drive_triacs.last_opened_pair = TRIAC_PAIR_UNKNOWN;//tim_triacs->triacs_a;
        }
    } // Если нужно закрыть тиристорную пару 1.
    if(TIM_GetITStatus(TIM, TRIACS_A_CLOSE_CHANNEL_IT) != RESET){
        TIM_ClearITPendingBit(TIM, TRIACS_A_CLOSE_CHANNEL_IT);
        TIM_ITConfig(TIM, TRIACS_A_CLOSE_CHANNEL_IT, DISABLE);
        
        triac_pair_close(
                get_triac_pair(tim_triacs->triacs_a)
            );
        if(drive_triacs.pairs_enabled){
            drive_triacs_pairs_pulse_train_setup_next(tim_triacs);
            drive_triacs.last_opened_pair = tim_triacs->triacs_a;
            drive_triacs_on_open_pair();
        }
    } // Если нужно открыть тиристорную пару 2.
    if(TIM_GetITStatus(TIM, TRIACS_B_OPEN_CHANNEL_IT) != RESET){
        TIM_ClearITPendingBit(TIM, TRIACS_B_OPEN_CHANNEL_IT);
        TIM_ITConfig(TIM, TRIACS_B_OPEN_CHANNEL_IT, DISABLE);
        // Если подача импульсов на тиристорные пары разрешена.
        /*if(drive_triacs.pairs_enabled){
            triac_pair_open(
                    get_triac_pair(tim_triacs->triacs_b)
                );
            //drive_triacs.last_opened_pair = tim_triacs->triacs_b;
            drive_triacs.last_opened_pair = TRIAC_PAIR_UNKNOWN;//tim_triacs->triacs_b;
        }*/
    } // Если нужно закрыть тиристорную пару 2.
    if(TIM_GetITStatus(TIM, TRIACS_B_CLOSE_CHANNEL_IT) != RESET){
        TIM_ClearITPendingBit(TIM, TRIACS_B_CLOSE_CHANNEL_IT);
        TIM_ITConfig(TIM, TRIACS_B_CLOSE_CHANNEL_IT, DISABLE);
            /*triac_pair_close(
                    get_triac_pair(tim_triacs->triacs_b)
                );
            if(drive_triacs.pairs_enabled){
                drive_triacs.last_opened_pair = tim_triacs->triacs_b;
            }*/
    }
}

void drive_triacs_timer0_irq_handler(void)
{
    timer_triacs_t* tim_triacs = get_timer_triacs(TRIACS_TIMER_0);
    drive_triacs_timer_irq_handler_impl(tim_triacs);
}

void drive_triacs_timer1_irq_handler(void)
{
    timer_triacs_t* tim_triacs = get_timer_triacs(TRIACS_TIMER_1);
    drive_triacs_timer_irq_handler_impl(tim_triacs);
}

static void drive_triacs_exc_pulse_train_setup_next_first(void)
{
    if(!drive_triacs.triac_exc_pt_enabled) return;

    uint16_t cur_ticks = TIM_GetCounter(drive_triacs.timer_exc);
    uint16_t pulse = drive_triacs.triac_exc_open_ticks + drive_triacs.triac_exc_pt_open_delta;

    if(drive_triacs.exc_pulse_train_remain_first < pulse) return;

    drive_triacs.exc_pulse_train_remain_first -= pulse;

    uint16_t next_open_pulse = cur_ticks + drive_triacs.triac_exc_pt_open_delta;
    uint16_t next_close_pulse = cur_ticks + pulse;

    if(next_close_pulse < TRIAC_EXC_MAX_TICKS + TRIAC_EXC_OFFSET -
            drive_triacs.triac_exc_pt_angle_min_ticks - drive_triacs.exc_pulse_train_offset){
        TIM_SetCompare1(drive_triacs.timer_exc, next_open_pulse);
        TIM_SetCompare2(drive_triacs.timer_exc, next_close_pulse);

        TIM_ITConfig(drive_triacs.timer_exc, TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL_IT, ENABLE);
        TIM_ITConfig(drive_triacs.timer_exc, TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL_IT, ENABLE);
    }
}

static void drive_triacs_exc_pulse_train_setup_next_second(void)
{
    if(!drive_triacs.triac_exc_pt_enabled) return;

    uint16_t cur_ticks = TIM_GetCounter(drive_triacs.timer_exc);
    uint16_t pulse = drive_triacs.triac_exc_open_ticks + drive_triacs.triac_exc_pt_open_delta;

    if(drive_triacs.exc_pulse_train_remain_second < pulse) return;

    drive_triacs.exc_pulse_train_remain_second -= pulse;

    uint16_t next_open_pulse = cur_ticks + drive_triacs.triac_exc_pt_open_delta;
    uint16_t next_close_pulse = cur_ticks + pulse;

    if(next_close_pulse < TRIAC_EXC_MAX_TICKS + TRIAC_EXC_OFFSET + TRIAC_EXC_TIM_HALF_CYCLE_OFFSET -
            drive_triacs.triac_exc_pt_angle_min_ticks - drive_triacs.exc_pulse_train_offset){
        TIM_SetCompare3(drive_triacs.timer_exc, next_open_pulse);
        TIM_SetCompare4(drive_triacs.timer_exc, next_close_pulse);

        TIM_ITConfig(drive_triacs.timer_exc, TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL_IT, ENABLE);
        TIM_ITConfig(drive_triacs.timer_exc, TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL_IT, ENABLE);
    }
}

void drive_triacs_exc_timer_irq_handler(void)
{
    TIM_TypeDef* TIM = drive_triacs.timer_exc;
    // Если нужно открыть симистор первого полупериода.
    if(TIM_GetITStatus(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL_IT) != RESET){
        TIM_ClearITPendingBit(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL_IT);
        TIM_ITConfig(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL_IT, DISABLE);
        
        // Если подача импульсов на симистор возбуждения разрешена.
        if(drive_triacs.exc_enabled){
            triac_open(&drive_triacs.triac_exc);
        }
    } // Если нужно закрыть симистор первого полупериода.
    if(TIM_GetITStatus(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL_IT) != RESET){
        TIM_ClearITPendingBit(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL_IT);
        TIM_ITConfig(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL_IT, DISABLE);
        
        triac_close(&drive_triacs.triac_exc);
        if(drive_triacs.exc_enabled){
            drive_triacs_exc_pulse_train_setup_next_first();
        }
    } // Если нужно открыть симистор второго полупериода.
    if(TIM_GetITStatus(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL_IT) != RESET){
        TIM_ClearITPendingBit(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL_IT);
        TIM_ITConfig(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL_IT, DISABLE);
        
        // Если подача импульсов на симистор возбуждения разрешена.
        if(drive_triacs.exc_enabled){
            triac_open(&drive_triacs.triac_exc);
        }
    } // Если нужно закрыть симистор второго полупериода.
    if(TIM_GetITStatus(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL_IT) != RESET){
        TIM_ClearITPendingBit(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL_IT);
        TIM_ITConfig(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL_IT, DISABLE);
        
        triac_close(&drive_triacs.triac_exc);
        if(drive_triacs.exc_enabled){
            drive_triacs_exc_pulse_train_setup_next_second();
        }
    }
}

ALWAYS_INLINE static uint16_t timer_triacs_clamp(int32_t ticks)
{
    if(ticks < TRIACS_TIM_MIN_TICKS) return TRIACS_TIM_MIN_TICKS;
    if(ticks > TRIACS_TIM_MAX_TICKS - 1) return TRIACS_TIM_MAX_TICKS - 1;
    return (uint16_t)ticks;
}

ALWAYS_INLINE static uint16_t timer_exc_clamp(int32_t ticks)
{
    if(ticks < TRIAC_EXC_MIN_TICKS) return TRIAC_EXC_MIN_TICKS;
    if(ticks > TRIAC_EXC_TIM_TICKS - 1) return TRIAC_EXC_TIM_TICKS - 1;
    return (uint16_t)ticks;
}

/**
 * Инициализирует таймер для открытия пары тиристоров.
 * @param triacs_pair Номер первой пары тиристоров.
 * @param offset_ticks Компенсация времени до запуска открытия тиристоров в тиках таймера.
 * @param angle_ticks Угол открытия в тиках таймера.
 * @param open_ticks Время открытия в тиках таймера.
 * @param delay_ticks Задержка открытия в тиках таймера.
 */
static void timer_triacs_setup_next_pair(triac_pair_number_t triacs_pair, int32_t offset_ticks,
                                         int32_t angle_ticks, int32_t open_ticks, int32_t delay_ticks)
{
    // Получим следующий свободный таймер тиристоров.
    timer_triacs_t* tim_trcs = timer_triacs_next();
    // Остановим таймер.
    TIM_Cmd(tim_trcs->timer, DISABLE);
    // Сбросим счётчик.
    TIM_SetCounter(tim_trcs->timer, 0);
    // Очистим флаги прерываний на открытие тиристоров.
    // Первая пара.
    TIM_ClearITPendingBit(tim_trcs->timer, TRIACS_A_OPEN_CHANNEL_IT);
    TIM_ClearITPendingBit(tim_trcs->timer, TRIACS_A_CLOSE_CHANNEL_IT);
    // Вторая пара.
    //TIM_ClearITPendingBit(tim_trcs->timer, TRIACS_B_OPEN_CHANNEL_IT);
    //TIM_ClearITPendingBit(tim_trcs->timer, TRIACS_B_CLOSE_CHANNEL_IT);
    // Закроем первую тиристорную пару.
    if(TRIAC_PAIR_VALID(tim_trcs->triacs_a)){
        triac_pair_close(get_triac_pair(tim_trcs->triacs_a));
    }
    // Закроем вторую тиристорную пару.
    //if(TRIAC_PAIR_VALID(tim_trcs->triacs_b)){
    //    triac_pair_close(get_triac_pair(tim_trcs->triacs_b));
    //}
    // Установим тиристорные пары таймера.
    // Первая пара тиристоров.
    tim_trcs->triacs_a = triacs_pair;
    // Вторая пара тиристоров.
    //tim_trcs->triacs_b = triacs_pair;
    // Время гребёнки.
    tim_trcs->pulse_train_remain = drive_triacs.triacs_pairs_pt_width_ticks;
    // Смещение открытия.
    tim_trcs->pulse_train_offset = offset_ticks + delay_ticks;
    // Тики таймера.
    // Угол открытия.
    //uint16_t angle_ticks = drive_triacs.triacs_pairs_angle_ticks;
    // Время открытия.
    //uint16_t open_ticks = drive_triacs.triacs_pairs_open_ticks;
    // Задержка открытия.
    //uint16_t delay_ticks = drive_triacs.triacs_pairs_delay_ticks;
    // Установим каналы таймера.
    // Открытие первой пары тиристоров.
    TIM_SetCompare1(tim_trcs->timer, timer_triacs_clamp((int32_t)(TRIACS_TIM_MAX_TICKS) -
                                     angle_ticks - offset_ticks -
                                     delay_ticks));
    // Закрытие первой пары тиристоров.
    TIM_SetCompare2(tim_trcs->timer, timer_triacs_clamp((int32_t)(TRIACS_TIM_MAX_TICKS + open_ticks) -
                                     angle_ticks - offset_ticks -
                                     delay_ticks));
    // Разрешим прерывания.
    TIM_ITConfig(tim_trcs->timer, TRIACS_A_OPEN_CHANNEL_IT, ENABLE);
    TIM_ITConfig(tim_trcs->timer, TRIACS_A_CLOSE_CHANNEL_IT, ENABLE);
    
    //! Запустим таймер.
    TIM_Cmd(tim_trcs->timer, ENABLE);
}

err_t drive_triacs_setup_next_pair(phase_t phase, phase_t last_open_phase, int16_t offset)
{
    // Нужна определённая фаза.
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    // Направление вращения.
    drive_dir_t dir = drive_phase_state_direction();
    // Нужно какое-либо направление.
    if(dir == DRIVE_DIR_UNK) return E_INVALID_VALUE;
    
    // Если подача импульсов на тиристорные пары запрещена.
    if(!drive_triacs.pairs_enabled){
        return E_NO_ERROR;
    }
    
    int32_t offset_ticks = TIME_TO_TICKS(offset);
    
    // Тики таймера.
    // Угол открытия.
    int32_t angle_ticks = drive_triacs.triacs_pairs_angle_ticks;
    // Время открытия.
    int32_t open_ticks = drive_triacs.triacs_pairs_open_ticks;
    // Задержка открытия.
    int32_t delay_ticks = drive_triacs.triacs_pairs_delay_ticks;

    // Нужен угол открытия не меньше смещения.
    if(angle_ticks <= offset_ticks) return E_NO_ERROR;
    
    // Индекс пары тиристоров.
    size_t triacs_index = 0;
    // Последовательность тиристоров.
    const triac_pair_number_t* triacs_seq = NULL;
    
    switch(dir){
        case DRIVE_DIR_FORW:
            // Вращение вперёд.
            triacs_seq = triac_open_seq_fwd;
            break;
        case DRIVE_DIR_BACKW:
            // Вращение назад.
            triacs_seq = triac_open_seq_bwd;
            break;
        default:
            return E_INVALID_VALUE;
    }
    
    // Обработаем фазу.
    switch(phase){
        case PHASE_A:
            // Фаза A - первые две пары тиристоров.
            triacs_index = PHASE_A_NULL_TRIAC_OFFSET;
            break;
        case PHASE_B:
            // Фаза B - вторые две пары тиристоров.
            triacs_index = PHASE_B_NULL_TRIAC_OFFSET;
            break;
        case PHASE_C:
            // Фаза C - третьи две пары тиристоров.
            triacs_index = PHASE_C_NULL_TRIAC_OFFSET;
            break;
        default:
            return E_INVALID_VALUE;
    }
    
    // Если для этой фазы пара уже была открыта.
    if(last_open_phase == phase){
        // Необходимо открыть следующую пару для этой фазы.
        triacs_index ++;
    }
    
    timer_triacs_setup_next_pair(triacs_seq[triacs_index], offset_ticks, angle_ticks,
                                 open_ticks, delay_ticks);
    
    return E_NO_ERROR;
}

/**
 * Инициализирует таймер для открытия симистора возбуждения.
 * @param offset_ticks Компенсация времени до запуска открытия симистора в тиках таймера.
 * @param angle_ticks Угол открытия в тиках таймера.
 * @param open_ticks Время открытия в тиках таймера.
 * @param delay_ticks Задержка открытия в тиках таймера.
 */
static void timer_triac_exc_setup(int32_t offset_ticks, int32_t angle_ticks,
                                  int32_t open_ticks, int32_t delay_ticks)
{
    // Остановим таймер.
    TIM_Cmd(drive_triacs.timer_exc, DISABLE);
    // Сбросим счётчик.
    TIM_SetCounter(drive_triacs.timer_exc, 0);
    // Очистить флаги прерываний.
    TIM_ClearITPendingBit(drive_triacs.timer_exc, TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL_IT);
    TIM_ClearITPendingBit(drive_triacs.timer_exc, TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL_IT);
    TIM_ClearITPendingBit(drive_triacs.timer_exc, TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL_IT);
    TIM_ClearITPendingBit(drive_triacs.timer_exc, TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL_IT);
    // Закроем симистор возбуждения.
    triac_close(&drive_triacs.triac_exc);
    // Время гребёнки.
    drive_triacs.exc_pulse_train_remain_first = drive_triacs.triac_exc_pt_width_ticks;
    drive_triacs.exc_pulse_train_remain_second = drive_triacs.triac_exc_pt_width_ticks;
    // Смещение открытия.
    drive_triacs.exc_pulse_train_offset = offset_ticks + delay_ticks;
    // Тики таймера.
    // Угол открытия.
    //uint16_t angle_ticks = drive_triacs.triac_exc_angle_ticks;
    // Время открытия.
    //uint16_t open_ticks = drive_triacs.triac_exc_open_ticks;
    // Задержка открытия.
    //uint16_t delay_ticks = drive_triacs.triac_exc_delay_ticks;
    // Установим каналы таймера.
    // Открытие первой пары тиристоров.
    TIM_SetCompare1(drive_triacs.timer_exc, timer_exc_clamp((TRIAC_EXC_MAX_TICKS + TRIAC_EXC_OFFSET) -
                                            angle_ticks - offset_ticks -
                                            delay_ticks));
    // Закрытие первой пары тиристоров.
    TIM_SetCompare2(drive_triacs.timer_exc, timer_exc_clamp((TRIAC_EXC_MAX_TICKS + TRIAC_EXC_OFFSET + open_ticks) -
                                            angle_ticks - offset_ticks -
                                            delay_ticks));
    // Открытие второй пары тиристоров.
    TIM_SetCompare3(drive_triacs.timer_exc, timer_exc_clamp((TRIAC_EXC_MAX_TICKS + TRIAC_EXC_OFFSET + TRIAC_EXC_TIM_HALF_CYCLE_OFFSET) -
                                            angle_ticks - offset_ticks -
                                            delay_ticks));
    // Закрытие второй пары тиристоров.
    TIM_SetCompare4(drive_triacs.timer_exc, timer_exc_clamp((TRIAC_EXC_MAX_TICKS + TRIAC_EXC_OFFSET + TRIAC_EXC_TIM_HALF_CYCLE_OFFSET +
                                            open_ticks) -
                                            angle_ticks - offset_ticks -
                                            delay_ticks));
    // Разрешить прерывания.
    TIM_ITConfig(drive_triacs.timer_exc, TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL_IT, ENABLE);
    TIM_ITConfig(drive_triacs.timer_exc, TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL_IT, ENABLE);
    TIM_ITConfig(drive_triacs.timer_exc, TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL_IT, ENABLE);
    TIM_ITConfig(drive_triacs.timer_exc, TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL_IT, ENABLE);
    // Запустим таймер.
    TIM_Cmd(drive_triacs.timer_exc, ENABLE);
}

/**
 * Настраивает таймер открытия симистора возбуждения.
 * @param phase Текущая фаза.
 * @return Код ошибки.
 */
err_t drive_triacs_setup_exc(phase_t phase, phase_t last_open_phase, int16_t offset)
{
    // Нужен режим регулирования.
    if(drive_triacs.exc_mode == DRIVE_TRIACS_EXC_EXTERNAL ||
       drive_triacs.exc_mode == DRIVE_TRIACS_EXC_FIXED) return E_NO_ERROR;
    // Нужна определённая фаза.
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    // Направление вращения.
    drive_dir_t dir = drive_phase_state_direction();
    // Нужно какое-либо направление.
    if(dir == DRIVE_DIR_UNK) return E_INVALID_VALUE;
    
    // Если ноль был уже пройден - не требуется запускать таймер.
    if(phase == last_open_phase) return E_NO_ERROR;
    
    // Если подача импульсов на симистор возбуждения запрещена.
    if(!drive_triacs.exc_enabled){
        return E_NO_ERROR;
    }
    
    int32_t offset_ticks = TIME_TO_TICKS(offset);

    // Тики таймера.
    // Угол открытия.
    int32_t angle_ticks = drive_triacs.triac_exc_angle_ticks;
    if(drive_triacs.exc_mode == DRIVE_TRIACS_EXC_FIXED_PULSE){
        angle_ticks = drive_triacs.triac_exc_max_angle_ticks;
    }
    // Время открытия.
    int32_t open_ticks = drive_triacs.triac_exc_open_ticks;
    // Задержка открытия.
    int32_t delay_ticks = drive_triacs.triac_exc_delay_ticks;
    
    // Нужен угол открытия не меньше смещения.
    if(angle_ticks <= offset_ticks) return E_NO_ERROR;

    phase_t exc_ctl_phase = drive_triacs.phase_exc;
    
    if(dir == DRIVE_DIR_BACKW){
        exc_ctl_phase = drive_phase_state_next_phase(exc_ctl_phase, DRIVE_DIR_BACKW);
    }
    
    if(exc_ctl_phase == phase){
        timer_triac_exc_setup(offset_ticks, angle_ticks,
                              open_ticks, delay_ticks);
    }
    
    return E_NO_ERROR;
}
