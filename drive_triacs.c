#include "drive_triacs.h"
#include <string.h>
#include "defs/defs.h"
#include "utils/utils.h"
#include "triac.h"
#include "triac_pair.h"



//! Максимальный угол открытия тиристоров в тиках таймера.
#define TRIACS_TIM_MAX_TICKS (TRIACS_TIM_TICKS / 3)
//! Максимальный угол открытия тиристоров.
#define TRIACS_TIM_MAX_TICKS_ANGLE (360 / 3)
//! Смещение между первой и второй парой тиристоров.
#define TRIACS_TIM_OFFSET (TRIACS_TIM_MAX_TICKS / 2)

//! Минимальный угол включения симистора возбуждения в тиках таймера.
#define TRIAC_EXC_MAX_TICKS (TRIAC_EXC_TIM_TICKS / 2)
//! Минимальный угол включения симистора возбуждения.
#define TRIAC_EXC_MAX_TICKS_ANGLE (360 / 2)
//! Смещение между полупериодами.
#define TRIAC_EXC_TIM_HALF_CYCLE_OFFSET (TRIAC_EXC_TIM_TICKS / 2)
//! Смещение до начала периода.
#define TRIAC_EXC_OFFSET (TRIAC_EXC_TIM_TICKS * 30 / 360)



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
    triac_pair_number_t triacs_b; //!< Пара тиристоров B.
} timer_triacs_t;

//! Тип структуры тиристоров привода.
typedef struct _Drive_Triacs {
    bool pairs_enabled; //!< Разрешение подачи импульсов на тиристорные пары.
    bool exc_enabled; //!< Разрешение подачи импульсов на симистор возбуждения.
    
    uint16_t triacs_pairs_angle_ticks; //!< Угол открытия тиристорных пар - значение регистра сравнения таймера.
    uint16_t triacs_pairs_open_ticks; //!< Время открытия тиристорных пар в тиках таймера.
    
    uint16_t triac_exc_angle_ticks; //!< Угол открытия симистора возбуждения - значение регистра сравнения таймера.
    uint16_t triac_exc_open_ticks; //!< Время открытия симистора возбуждения в тиках таймера.
    
    triac_pair_t triac_pairs[TRIAC_PAIRS_COUNT]; //!< Пары тиристоров.
    triac_t triac_exc; //!< Тиристор возбуждения.
    
    timer_triacs_t timers_triacs[TRIACS_TIMERS_COUNT]; //!< Тиристоры таймеров и таймеры.
    size_t current_timer_triacs; //!< Текущий индекс таймеров тиристоров.
    
    TIM_TypeDef* timer_exc; //!< Таймер для открытия симистора возбуждения.
    
    phase_t phase_exc; //!< Фаза возбуждения.
} drive_triacs_t;


static drive_triacs_t drive_triacs;



err_t drive_triacs_init(void)
{
    memset(&drive_triacs, 0x0, sizeof(drive_triacs_t));
    
    drive_triacs.triacs_pairs_open_ticks = TRIACS_TIM_OPEN_TIME_DEFAULT;
    drive_triacs.triac_exc_open_ticks = TRIAC_EXC_TIM_OPEN_TIME_DEFAULT;
    
    return E_NO_ERROR;
}

bool drive_triacs_pairs_enabled(void)
{
    return drive_triacs.pairs_enabled;
}

void drive_triacs_pairs_set_enabled(bool enabled)
{
    drive_triacs.pairs_enabled = enabled;
}

bool drive_triacs_exc_enabled(void)
{
    return drive_triacs.exc_enabled;
}

void drive_triacs_exc_set_enabled(bool enabled)
{
    drive_triacs.exc_enabled = enabled;
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
    for(i = 0; i < TRIACS_TIMERS_COUNT; i ++) {
        triac_pair_close(get_triac_pair(drive_triacs.timers_triacs[i].triacs_a));
        triac_pair_close(get_triac_pair(drive_triacs.timers_triacs[i].triacs_b));
    }
    triac_close(&drive_triacs.triac_exc);
}

void drive_triacs_stop(void)
{
    drive_triacs_stop_timers();
    drive_triacs_close_triacs();
}

err_t drive_triacs_set_pairs_open_angle(uint32_t angle)
{
    err_t err = E_NO_ERROR;
    if(angle < TRIACS_PAIRS_ANGLE_MIN){
        angle = 0;
        err = E_OUT_OF_RANGE;
    }
    if(angle > TRIACS_PAIRS_ANGLE_MAX){
        angle = TRIACS_PAIRS_ANGLE_MAX;
        err = E_OUT_OF_RANGE;
    }
    drive_triacs.triacs_pairs_angle_ticks = TRIACS_TIM_MAX_TICKS * angle / TRIACS_TIM_MAX_TICKS_ANGLE;
    return err;
}

err_t drive_triacs_set_exc_open_angle(uint32_t angle)
{
    err_t err = E_NO_ERROR;
    if(angle < TRIAC_EXC_ANGLE_MIN){
        angle = 0;
        err = E_OUT_OF_RANGE;
    }
    if(angle > TRIAC_EXC_ANGLE_MAX){
        angle = TRIACS_PAIRS_ANGLE_MAX;
        err = E_OUT_OF_RANGE;
    }
    drive_triacs.triacs_pairs_angle_ticks = TRIAC_EXC_MAX_TICKS * angle / TRIAC_EXC_MAX_TICKS_ANGLE;
    return err;
}

uint16_t drive_triacs_open_time_us(void)
{
    return OPEN_TICKS_TO_TIME(drive_triacs.triacs_pairs_open_ticks);
}

err_t drive_triacs_set_open_time_us(uint16_t time)
{
    if(time == 0) return E_INVALID_VALUE;
    
    drive_triacs.triacs_pairs_open_ticks = OPEN_TIME_TO_TICKS(time);
    
    return E_NO_ERROR;
}

uint16_t drive_triacs_exc_open_time_us(void)
{
    return OPEN_TICKS_TO_TIME(drive_triacs.triac_exc_open_ticks);
}

err_t drive_triacs_set_exc_open_time_us(uint16_t time)
{
    if(time == 0) return E_INVALID_VALUE;
    
    drive_triacs.triac_exc_open_ticks = OPEN_TIME_TO_TICKS(time);
    
    return E_NO_ERROR;
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
    if(triac_pair >= TRIAC_PAIRS_COUNT) return E_OUT_OF_RANGE;
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

void drive_triacs_timer0_irq_handler(void)
{
    timer_triacs_t* tim_triacs = get_timer_triacs(TRIACS_TIMER_0);
    TIM_TypeDef* TIM = tim_triacs->timer;
    // Если нужно открыть тиристорную пару 1.
    if(TIM_GetITStatus(TIM, TRIACS_A_OPEN_CHANNEL_IT) != RESET){
        // Если подача импульсов на тиристорные пары разрешена.
        if(drive_triacs.pairs_enabled){
            triac_pair_open(
                    get_triac_pair(tim_triacs->triacs_a)
                );}
        TIM_ClearITPendingBit(TIM, TRIACS_A_OPEN_CHANNEL_IT);
    } // Если нужно закрыть тиристорную пару 1.
    if(TIM_GetITStatus(TIM, TRIACS_A_CLOSE_CHANNEL_IT) != RESET){
        triac_pair_close(
                    get_triac_pair(tim_triacs->triacs_a)
                );
        TIM_ClearITPendingBit(TIM, TRIACS_A_CLOSE_CHANNEL_IT);
    } // Если нужно открыть тиристорную пару 2.
    if(TIM_GetITStatus(TIM, TRIACS_B_OPEN_CHANNEL_IT) != RESET){
        // Если подача импульсов на тиристорные пары разрешена.
        if(drive_triacs.pairs_enabled){
        triac_pair_open(
                    get_triac_pair(tim_triacs->triacs_b)
                );}
        TIM_ClearITPendingBit(TIM, TRIACS_B_OPEN_CHANNEL_IT);
    } // Если нужно закрыть тиристорную пару 2.
    if(TIM_GetITStatus(TIM, TRIACS_B_CLOSE_CHANNEL_IT) != RESET){
        triac_pair_close(
                    get_triac_pair(tim_triacs->triacs_b)
                );
        TIM_ClearITPendingBit(TIM, TRIACS_B_CLOSE_CHANNEL_IT);
    }
}

void drive_triacs_timer1_irq_handler(void)
{
    timer_triacs_t* tim_triacs = get_timer_triacs(TRIACS_TIMER_1);
    TIM_TypeDef* TIM = tim_triacs->timer;
    // Если нужно открыть тиристорную пару 1.
    if(TIM_GetITStatus(TIM, TRIACS_A_OPEN_CHANNEL_IT) != RESET){
        // Если подача импульсов на тиристорные пары разрешена.
        if(drive_triacs.pairs_enabled){
        triac_pair_open(
                    get_triac_pair(tim_triacs->triacs_a)
                );}
        TIM_ClearITPendingBit(TIM, TRIACS_A_OPEN_CHANNEL_IT);
    } // Если нужно закрыть тиристорную пару 1.
    if(TIM_GetITStatus(TIM, TRIACS_A_CLOSE_CHANNEL_IT) != RESET){
        triac_pair_close(
                    get_triac_pair(tim_triacs->triacs_a)
                );
        TIM_ClearITPendingBit(TIM, TRIACS_A_CLOSE_CHANNEL_IT);
    } // Если нужно открыть тиристорную пару 2.
    if(TIM_GetITStatus(TIM, TRIACS_B_OPEN_CHANNEL_IT) != RESET){
        // Если подача импульсов на тиристорные пары разрешена.
        if(drive_triacs.pairs_enabled){
        triac_pair_open(
                    get_triac_pair(tim_triacs->triacs_b)
                );}
        TIM_ClearITPendingBit(TIM, TRIACS_B_OPEN_CHANNEL_IT);
    } // Если нужно закрыть тиристорную пару 2.
    if(TIM_GetITStatus(TIM, TRIACS_B_CLOSE_CHANNEL_IT) != RESET){
        triac_pair_close(
                    get_triac_pair(tim_triacs->triacs_b)
                );
        TIM_ClearITPendingBit(TIM, TRIACS_B_CLOSE_CHANNEL_IT);
    }
}

void drive_triacs_exc_timer_irq_handler(void)
{
    TIM_TypeDef* TIM = drive_triacs.timer_exc;
    // Если нужно открыть симистор первого полупериода.
    if(TIM_GetITStatus(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL_IT) != RESET){
        // Если подача импульсов на симистор возбуждения разрешена.
        if(drive_triacs.exc_enabled){
            triac_open(&drive_triacs.triac_exc);
        }
        TIM_ClearITPendingBit(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL_IT);
    } // Если нужно закрыть симистор первого полупериода.
    if(TIM_GetITStatus(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL_IT) != RESET){
        triac_close(&drive_triacs.triac_exc);
        TIM_ClearITPendingBit(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL_IT);
    } // Если нужно открыть симистор второго полупериода.
    if(TIM_GetITStatus(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL_IT) != RESET){
        // Если подача импульсов на симистор возбуждения разрешена.
        if(drive_triacs.exc_enabled){
            triac_open(&drive_triacs.triac_exc);
        }
        TIM_ClearITPendingBit(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL_IT);
    } // Если нужно закрыть симистор второго полупериода.
    if(TIM_GetITStatus(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL_IT) != RESET){
        triac_close(&drive_triacs.triac_exc);
        TIM_ClearITPendingBit(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL_IT);
    }
}

/**
 * Инициализирует таймер для открытия пар тиристоров.
 * @param triacs_a Номер первой пары тиристоров.
 * @param triacs_b Номер второй пары тиристоров.
 */
static void timer_triacs_setup_next(triac_pair_number_t triacs_a, triac_pair_number_t triacs_b)
{
    // Получим следующий свободный таймер тиристоров.
    timer_triacs_t* tim_trcs = timer_triacs_next();
    // Остановим таймер.
    TIM_Cmd(tim_trcs->timer, DISABLE);
    // Сбросим счётчик.
    TIM_SetCounter(tim_trcs->timer, 0);
    // Установим тиристорные пары таймера.
    // Первая пара тиристоров.
    tim_trcs->triacs_a = triacs_a;
    // Вторая пара тиристоров.
    tim_trcs->triacs_b = triacs_b;
    // Установим каналы таймера.
    // Открытие первой пары тиристоров.
    TIM_SetCompare1(tim_trcs->timer, (TRIACS_TIM_MAX_TICKS) - drive_triacs.triacs_pairs_angle_ticks);
    // Закрытие первой пары тиристоров.
    TIM_SetCompare2(tim_trcs->timer, (TRIACS_TIM_MAX_TICKS + drive_triacs.triacs_pairs_open_ticks) - drive_triacs.triacs_pairs_angle_ticks);
    // Открытие второй пары тиристоров.
    TIM_SetCompare3(tim_trcs->timer, (TRIACS_TIM_MAX_TICKS + TRIACS_TIM_OFFSET) - drive_triacs.triacs_pairs_angle_ticks);
    // Закрытие второй пары тиристоров.
    TIM_SetCompare4(tim_trcs->timer, (TRIACS_TIM_MAX_TICKS + TRIACS_TIM_OFFSET + drive_triacs.triacs_pairs_open_ticks) - drive_triacs.triacs_pairs_angle_ticks);
    //! Запустим таймер.
    TIM_Cmd(tim_trcs->timer, ENABLE);
}

err_t drive_triacs_setup_next_pairs(phase_t phase)
{
    // Нужна определённая фаза.
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    // Направление вращения.
    drive_dir_t dir = phase_state_drive_direction();
    // Нужно какое-либо направление.
    if(dir == DRIVE_DIR_UNK) return E_INVALID_VALUE;
    // Нужен угол открытия.
    if(drive_triacs.triacs_pairs_angle_ticks == 0) return E_INVALID_VALUE;
    
    // Если подача импульсов на тиристорные пары запрещена.
    if(!drive_triacs.pairs_enabled){
        return E_NO_ERROR;
    }
    
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
    
    timer_triacs_setup_next(triacs_seq[triacs_index], triacs_seq[triacs_index + 1]);
    
    return E_NO_ERROR;
}

/**
 * Инициализирует таймер для открытия симистора возбуждения.
 */
static void timer_triac_exc_setup(void)
{
    // Остановим таймер.
    TIM_Cmd(drive_triacs.timer_exc, DISABLE);
    // Сбросим счётчик.
    TIM_SetCounter(drive_triacs.timer_exc, 0);
    // Установим каналы таймера.
    // Открытие первой пары тиристоров.
    TIM_SetCompare1(drive_triacs.timer_exc, (TRIAC_EXC_MAX_TICKS + TRIAC_EXC_OFFSET) - drive_triacs.triac_exc_angle_ticks);
    // Закрытие первой пары тиристоров.
    TIM_SetCompare2(drive_triacs.timer_exc, (TRIAC_EXC_MAX_TICKS + TRIAC_EXC_OFFSET + drive_triacs.triac_exc_open_ticks) -
                                            drive_triacs.triac_exc_angle_ticks);
    // Открытие второй пары тиристоров.
    TIM_SetCompare3(drive_triacs.timer_exc, (TRIAC_EXC_MAX_TICKS + TRIAC_EXC_OFFSET + TRIAC_EXC_TIM_HALF_CYCLE_OFFSET) -
                                            drive_triacs.triac_exc_angle_ticks);
    // Закрытие второй пары тиристоров.
    TIM_SetCompare4(drive_triacs.timer_exc, (TRIAC_EXC_MAX_TICKS + TRIAC_EXC_OFFSET + TRIAC_EXC_TIM_HALF_CYCLE_OFFSET +
                                            drive_triacs.triac_exc_open_ticks) - drive_triacs.triac_exc_angle_ticks);
    //! Запустим таймер.
    TIM_Cmd(drive_triacs.timer_exc, ENABLE);
}

/**
 * Настраивает таймер открытия симистора возбуждения.
 * @param phase Текущая фаза.
 * @return Код ошибки.
 */
err_t drive_triacs_setup_exc(phase_t phase)
{
    // Нужна определённая фаза.
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    // Направление вращения.
    drive_dir_t dir = phase_state_drive_direction();
    // Нужно какое-либо направление.
    if(dir == DRIVE_DIR_UNK) return E_INVALID_VALUE;
    // Нужен угол открытия.
    if(drive_triacs.triac_exc_angle_ticks == 0) return E_INVALID_VALUE;
    
    // Если подача импульсов на симистор возбуждения запрещена.
    if(!drive_triacs.exc_enabled){
        return E_NO_ERROR;
    }
    
    phase_t exc_ctl_phase = drive_triacs.phase_exc;
    
    if(dir == DRIVE_DIR_BACKW){
        exc_ctl_phase = phase_state_next_phase(phase, DRIVE_DIR_BACKW);
    }
    
    if(exc_ctl_phase == phase){
        timer_triac_exc_setup();
    }
    
    return E_NO_ERROR;
}
