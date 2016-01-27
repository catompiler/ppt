#include "drive.h"
#include "settings.h"
#include "ramp.h"
#include <string.h>


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


//! Максимальный угол открытия тиристоров в тиках таймера.
#define TRIACS_TIM_ANGLE_TICKS_MAX (TRIACS_TIM_TICKS / 3)
//! Смещение между первой и второй парой тиристоров.
#define TRIACS_TIM_OFFSET (TRIACS_TIM_ANGLE_TICKS_MAX / 2)
//! Минимальный угол включения симистора возбуждения.
#define TRIAC_EXC_MIN_ANGLE (30)
//! Максимальный угол включения симистора возбуждения.
#define TRIAC_EXC_MAX_ANGLE (170)
//! Минимальный угол включения симистора возбуждения в тиках таймера.
#define TRIAC_EXC_ANGLE_TICKS_MIN (TRIAC_EXC_TIM_TICKS * TRIAC_EXC_MIN_ANGLE / 360)
//! Минимальный угол включения симистора возбуждения в тиках таймера.
#define TRIAC_EXC_ANGLE_TICKS_MAX (TRIAC_EXC_TIM_TICKS * TRIAC_EXC_MAX_ANGLE / 360)
//! Смещение между полупериодами.
#define TRIAC_EXC_TIM_HALF_CYCLE_OFFSET (TRIAC_EXC_TIM_TICKS / 2)
//! Смещение до начала периода фазы возбуждения.
#define TRIAC_EXC_TIM_OFFSET TRIAC_EXC_ANGLE_TICKS_MIN

//! Перевод времени открытия в тики таймера.
#define OPEN_TIME_TO_TICKS(T) (((uint32_t)T * TRIACS_TIM_PERIOD) / TRIACS_TIM_PERIOD_US)
//! Перевод тиков таймера в время открытия.
#define OPEN_TICKS_TO_TIME(T) (((uint32_t)T * TRIACS_TIM_PERIOD_US) / TRIACS_TIM_PERIOD)


//! Число периодов калибровки питания.
#define DRIVE_POWER_CALIBRATION_PERIODS 5

//! Необходимые для готовности флаги.
#define DRIVE_READY_FLAGS (DRIVE_FLAG_POWER_DATA_AVAIL)


//! Тип структуры таймера для открытия тиристорных пар.
typedef struct _Timer_Triacs {
    TIM_TypeDef* timer; //!< Таймер для включения тиристоров.
    triac_pair_number_t triacs_a; //!< Пара тиристоров А.
    triac_pair_number_t triacs_b; //!< Пара тиристоров B.
} timer_triacs_t;

//! Тип структуры настроек привода.
typedef struct _Drive_Settings {
    fixed32_t U_nom; //!< Номинальное напряжение, В.
    fixed32_t U_nom_delta_allow; //!< Допустимое отклонение номинального напряжения, В.
    fixed32_t U_nom_delta_crit; //!< Критическое отклонение номинального напряжения, В.
    fixed32_t U_zero_noise; //!< Шум напряжения нуля, В.
    fixed32_t I_zero_noise; //!< Шум тока нуля, А.
    phase_t exc_phase; //!< Фаза возбуждения.
} drive_settings_t;

//! Структура привода.
typedef struct _Drive {
    drive_flags_t flags; //!< Флаги.
    drive_state_t state; //!< Состояние.
    drive_errors_t errors; //!< Ошибки.
    drive_warnings_t warnings; //!< Предупреждения.
    drive_power_errors_t power_errors; //!< Ошибки питания.
    drive_power_calibration_t power_calibration; //!< Состояние калибровки питания.
    phase_t power_phase; //!< Фаза начала калибровки питания.
    size_t power_calibration_periods; //!< Число периодов калибровки питания.
    
    uint16_t triacs_pairs_angle_ticks; //!< Угол открытия тиристорных пар - значение регистра сравнения таймера.
    uint16_t triacs_pairs_open_ticks; //!< Время открытия тиристорных пар в тиках таймера.
    
    uint16_t triac_exc_angle_ticks; //!< Угол открытия симистора возбуждения - значение регистра сравнения таймера.
    uint16_t triac_exc_open_ticks; //!< Время открытия симистора возбуждения в тиках таймера.
    
    triac_pair_t triac_pairs[TRIAC_PAIRS_COUNT]; //!< Пары тиристоров.
    triac_t triac_exc; //!< Тиристор возбуждения.
    
    timer_triacs_t timers_triacs[TRIACS_TIMERS_COUNT]; //!< Тиристоры таймеров и таймеры.
    size_t current_timer_triacs; //!< Текущий индекс таймеров тиристоров.
    
    TIM_TypeDef* timer_exc; //!< Таймер для открытия симистора возбуждения.
    
    power_value_t power_values[POWER_VALUES_COUNT]; //!< Значение каналов АЦП.
    power_t power; //!< Питание.
    
    ramp_t ramp; //!< Разгон.
    
    drive_settings_t settings; //!< Настройки.
} drive_t;

//! Состояние привода.
static drive_t drive;


//! Алиас для текущего значения угла открытия тиристоров в тиках таймера.
#define CURRENT_TIMER_ANGLE_TICKS (drive.triacs_pairs_angle_ticks)


/*
 * Функции реализации.
 */

/**
 * Получение состояния.
 * @return Состояние.
 */
ALWAYS_INLINE static drive_state_t drive_get_state(void)
{
    return drive.state;
}

/**
 * Установка состояния.
 * @param state Состояние.
 */
ALWAYS_INLINE static void drive_set_state(drive_state_t state)
{
    drive.state = state;
}

/**
 * Установка флага.
 * @param flag Флаг.
 */
ALWAYS_INLINE static void drive_set_flag(drive_flags_t flag)
{
    drive.flags |= flag;
}

/**
 * Снятие флага.
 * @param flag Флаг.
 */
ALWAYS_INLINE static void drive_clear_flag(drive_flags_t flag)
{
    drive.flags &= ~flag;
}

/**
 * Получает состояние флагов.
 * @param flags Флаги.
 * @return Состояния флагов.
 */
ALWAYS_INLINE static bool drive_flags_is_set(drive_flags_t flags)
{
    return (drive.flags & flags) == flags;
}

/**
 * Установка ошибки.
 * @param error Ошибка.
 */
ALWAYS_INLINE static void drive_set_error(drive_errors_t error)
{
    drive.errors |= error;
}

/**
 * Снятие ошибки.
 * @param error Ошибка.
 */
ALWAYS_INLINE static void drive_clear_error(drive_errors_t error)
{
    drive.errors &= ~error;
}

/**
 * Установка предупреждения.
 * @param warning Предупреждение.
 */
ALWAYS_INLINE static void drive_set_warning(drive_warnings_t warning)
{
    drive.warnings |= warning;
}

/**
 * Снятие предупреждения.
 * @param warning Предупреждение.
 */
ALWAYS_INLINE static void drive_clear_warning(drive_warnings_t warning)
{
    drive.warnings &= ~warning;
}

/**
 * Установка ошибки питания.
 * @param error Ошибка.
 */
ALWAYS_INLINE static void drive_set_power_error(drive_power_errors_t error)
{
    drive.power_errors |= error;
}

/**
 * Снятие ошибки питания.
 * @param error Ошибка.
 */
ALWAYS_INLINE static void drive_clear_power_error(drive_power_errors_t error)
{
    drive.power_errors &= ~error;
}

/**
 * Получает текущуий таймер тиристоров.
 * @return Текущий таймер тиристоров.
 */
ALWAYS_INLINE static timer_triacs_t* timer_triacs_current(void)
{
    return &drive.timers_triacs[drive.current_timer_triacs];
}

/**
 * Устанавливает следующий таймер тиристоров.
 * @return Следующий таймер тиристоров.
 */
static timer_triacs_t* timer_triacs_next(void)
{
    if(++ drive.current_timer_triacs >= TRIACS_TIMERS_COUNT){
        drive.current_timer_triacs = 0;
    }
    return timer_triacs_current();
}

/**
 * Инициализирует таймер для открытия пар тиристоров.
 * @param triacs_a Номер первой пары тиристоров.
 * @param triacs_b Номер второй пары тиристоров.
 */
static void timer_triacs_setup_next(triac_pair_number_t triacs_a, triac_pair_number_t triacs_b)
{
    // Если выходим за границу - возврат.
    if(drive.triacs_pairs_angle_ticks > TRIACS_TIM_ANGLE_TICKS_MAX) return;
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
    TIM_SetCompare1(tim_trcs->timer, (TRIACS_TIM_ANGLE_TICKS_MAX) - drive.triacs_pairs_angle_ticks);
    // Закрытие первой пары тиристоров.
    TIM_SetCompare2(tim_trcs->timer, (TRIACS_TIM_ANGLE_TICKS_MAX + drive.triacs_pairs_open_ticks) - drive.triacs_pairs_angle_ticks);
    // Открытие второй пары тиристоров.
    TIM_SetCompare3(tim_trcs->timer, (TRIACS_TIM_ANGLE_TICKS_MAX + TRIACS_TIM_OFFSET) - drive.triacs_pairs_angle_ticks);
    // Закрытие второй пары тиристоров.
    TIM_SetCompare4(tim_trcs->timer, (TRIACS_TIM_ANGLE_TICKS_MAX + TRIACS_TIM_OFFSET + drive.triacs_pairs_open_ticks) - drive.triacs_pairs_angle_ticks);
    //! Запустим таймер.
    TIM_Cmd(tim_trcs->timer, ENABLE);
}

/**
 * Получает тиристорную пару с заданным номером.
 * @param n Номер тиристорной пары.
 * @return Тиристорная пара.
 */
ALWAYS_INLINE static triac_pair_t* get_triac_pair(triac_pair_number_t n)
{
    return &drive.triac_pairs[n];
}

/**
 * Получает таймер тиристоров с заданным номером.
 * @param n Номер таймера тиристоров.
 * @return Таймер тиристоров.
 */
ALWAYS_INLINE static timer_triacs_t* get_timer_triacs(triac_pair_number_t n)
{
    return &drive.timers_triacs[n];
}

/**
 * Инициализирует таймер для открытия симистора возбуждения.
 */
static void timer_triac_exc_setup(void)
{
    // Если выходим за границу - возврат.
    if(drive.triac_exc_angle_ticks > TRIAC_EXC_ANGLE_TICKS_MAX ||
       drive.triac_exc_angle_ticks < TRIAC_EXC_ANGLE_TICKS_MIN) return;
    // Остановим таймер.
    TIM_Cmd(drive.timer_exc, DISABLE);
    // Сбросим счётчик.
    TIM_SetCounter(drive.timer_exc, 0);
    // Установим каналы таймера.
    // Открытие первой пары тиристоров.
    TIM_SetCompare1(drive.timer_exc, (TRIAC_EXC_ANGLE_TICKS_MAX + TRIAC_EXC_TIM_OFFSET) -
                                      drive.triac_exc_angle_ticks);
    // Закрытие первой пары тиристоров.
    TIM_SetCompare2(drive.timer_exc, (TRIAC_EXC_ANGLE_TICKS_MAX + TRIAC_EXC_TIM_OFFSET +
                                      drive.triac_exc_open_ticks) - drive.triac_exc_angle_ticks);
    // Открытие второй пары тиристоров.
    TIM_SetCompare3(drive.timer_exc, (TRIAC_EXC_ANGLE_TICKS_MAX + TRIAC_EXC_TIM_OFFSET +
                                      TRIAC_EXC_TIM_HALF_CYCLE_OFFSET) - drive.triac_exc_angle_ticks);
    // Закрытие второй пары тиристоров.
    TIM_SetCompare4(drive.timer_exc, (TRIAC_EXC_ANGLE_TICKS_MAX + TRIAC_EXC_TIM_OFFSET +
                                      TRIAC_EXC_TIM_HALF_CYCLE_OFFSET + drive.triac_exc_open_ticks) -
                                      drive.triac_exc_angle_ticks);
    //! Запустим таймер.
    TIM_Cmd(drive.timer_exc, ENABLE);
}

/**
 * Настраивает таймер открытия тиристорных пар.
 * @param phase Текущая фаза.
 * @return Код ошибки.
 */
static err_t drive_setup_triacs_pairs_timer(phase_t phase)
{
    // Нужна определённая фаза.
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    // Нужно какое-либо направление.
    if(phase_state_drive_direction() == DRIVE_DIR_UNK) return E_INVALID_VALUE;
    
    // Индекс пары тиристоров.
    size_t triacs_index = 0;
    // Последовательность тиристоров.
    const triac_pair_number_t* triacs_seq = NULL;
    
    drive_dir_t dir = phase_state_drive_direction();
    
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
 * Настраивает таймер открытия симистора возбуждения.
 * @param phase Текущая фаза.
 * @return Код ошибки.
 */
static err_t drive_setup_triac_exc_timer(phase_t phase)
{
    // Нужна определённая фаза.
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    // Нужно какое-либо направление.
    if(phase_state_drive_direction() == DRIVE_DIR_UNK) return E_INVALID_VALUE;
    
    phase_t exc_ctl_phase = drive.settings.exc_phase;
    
    if(phase_state_drive_direction() == DRIVE_DIR_BACKW){
        exc_ctl_phase = phase_state_next_phase(phase, DRIVE_DIR_BACKW);
    }
    
    if(exc_ctl_phase == phase){
        timer_triac_exc_setup();
    }
    
    return E_NO_ERROR;
}

/*
 * Обработка возникновения ошибки.
 */

/**
 * Останавливает таймеры открытия тиристоров и симистора возбуждения.
 */
static void drive_error_stop_timers(void)
{
    size_t i;
    for(i = 0; i < TRIACS_TIMERS_COUNT; i ++) {
        TIM_Cmd(drive.timers_triacs[i].timer, DISABLE);
    }
    TIM_Cmd(drive.timer_exc, DISABLE);
}

/**
 * Закрывает тиристоры и симистор возбуждения.
 */
static void drive_error_close_triacs(void)
{
    size_t i;
    for(i = 0; i < TRIACS_TIMERS_COUNT; i ++) {
        triac_pair_close(get_triac_pair(drive.timers_triacs[i].triacs_a));
        triac_pair_close(get_triac_pair(drive.timers_triacs[i].triacs_b));
    }
    triac_close(&drive.triac_exc);
}

/**
 * Обработчик возникновения ошибки.
 * @param error Ошибка.
 */
static void drive_error_occured(drive_errors_t error)
{
    drive_set_error(error);
    
    if(drive_get_state() != DRIVE_STATE_ERROR){
        drive_set_state(DRIVE_STATE_ERROR);
        drive_error_stop_timers();
        drive_error_close_triacs();
    }
}

/**
 * Обработчик возникновения ошибки питания.
 * @param error Ошибка питания.
 */
static void drive_power_error_occured(drive_power_errors_t error)
{
    drive_set_power_error(error);
    drive_error_occured(DRIVE_ERROR_POWER_INVALID);
}

/*
 * Обработка состояний привода
 * с необходимыми функциями.
 */

/*
 * Состояние начальной инициализации.
 */

/**
 * Производит калибровку питания.
 * @param phase Текущая фаза.
 */
static void drive_process_power_calibration(phase_t phase)
{
    switch(drive.power_calibration){
        case DRIVE_PWR_CALIBRATION_NONE:
            
            power_calc_values(&drive.power, POWER_CHANNELS);
            
            if(power_data_avail(&drive.power, POWER_CHANNELS)){
                drive.power_phase = phase;
                drive.power_calibration = DRIVE_PWR_CALIBRATION_RUNNING;
            }
            break;
        case DRIVE_PWR_CALIBRATION_RUNNING:
            
            if(phase == drive.power_phase){
                
                if(++ drive.power_calibration_periods >= DRIVE_POWER_CALIBRATION_PERIODS){
                    power_calc_values(&drive.power, POWER_CHANNELS);
                    
                    power_calibrate(&drive.power, POWER_CHANNELS);
                    
                    drive_set_flag(DRIVE_FLAG_POWER_CALIBRATED);
                    drive.power_calibration = DRIVE_PWR_CALIBRATION_DONE;
                }
            }
            break;
        case DRIVE_PWR_CALIBRATION_DONE:
            drive_set_state(DRIVE_STATE_IDLE);
        default:
            break;
    }
}

/**
 * Обработка состояния начальной инициализации привода.
 * @param phase Фаза.
 * @return Код ошибки.
 */
static err_t drive_state_process_init(phase_t phase)
{
    // Нужна определённая фаза.
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    
    drive_process_power_calibration(phase);
    
    return E_NO_ERROR;
}

/*
 * Состояние простоя (готовность).
 */

// Результаты сравнения.
// Больше.
#define PWR_GREATER 1
// Меньше.
#define PWR_LESS -1
// Равен.
#define PWR_EQUAL 0

/**
 * Сравнение значения канала с заданным.
 * @param channel Канал АЦП.
 * @param normal Нормальное значение.
 * @param max_delta Допуск.
 * @return Результат сравнения.
 */
static int drive_compare_power_value(size_t channel, fixed32_t normal, fixed32_t max_delta)
{
    fixed32_t value = power_channel_real_value_avg(&drive.power, channel);
    fixed32_t delta = value - normal;
    
    if(delta < -max_delta) return PWR_LESS;
    if(delta >  max_delta) return PWR_GREATER;
    return PWR_EQUAL;
}

/**
 * Сравнивает канал входного напряжения с заданным отклонением.
 * @param channel Канал АЦП.
 * @param delta_percents Допуск в вольтах.
 * @return Результат сравнения.
 */
static int drive_compare_input_voltage(size_t channel, fixed32_t delta)
{
    return drive_compare_power_value(channel, drive.settings.U_nom, delta);
}

/**
 * Сравнение канала тока с нулём.
 * @param channel Канала АЦП.
 * @return Результат сравнения.
 */
static int drive_compare_zero_current(size_t channel)
{
    return drive_compare_power_value(channel, 0, drive.settings.I_zero_noise);
}

/**
 * Сравнение канала напряжения с нулём.
 * @param channel Канала АЦП.
 * @return Результат сравнения.
 */
static int drive_compare_zero_voltage(size_t channel)
{
    return drive_compare_power_value(channel, 0, drive.settings.U_zero_noise);
}

/**
 * Устанавливает флаг в зависимости от результата сравнения.
 * @param cmp Результат сравнения.
 * @param set_proc Функция установки флага.
 * @param less Флаг при значении "меньше".
 * @param greater Флаг при значении "больше".
 */
static void drive_compare_set_flag(int cmp, void (*set_proc)(uint32_t), uint32_t less, uint32_t greater)
{
    switch(cmp){
        case PWR_LESS:
            set_proc(less);
            break;
        case PWR_GREATER:
            set_proc(greater);
            break;
        case PWR_EQUAL:
        default:
            break;
    }
}

/**
 * Проверяет значение входов питания в состоянии простоя (готовности).
 */
static void drive_check_power_idle(void)
{
    // Напряжения - превышение критической разности.
    // A.
    drive_compare_set_flag(drive_compare_input_voltage(POWER_VALUE_Ua, drive.settings.U_nom_delta_crit),
                           drive_power_error_occured, DRIVE_POWER_ERROR_UNDERFLOW_Ua, DRIVE_POWER_ERROR_OVERFLOW_Ua);
    // B.
    drive_compare_set_flag(drive_compare_input_voltage(POWER_VALUE_Ub, drive.settings.U_nom_delta_crit),
                           drive_power_error_occured, DRIVE_POWER_ERROR_UNDERFLOW_Uc, DRIVE_POWER_ERROR_OVERFLOW_Ub);
    // C.
    drive_compare_set_flag(drive_compare_input_voltage(POWER_VALUE_Uc, drive.settings.U_nom_delta_crit),
                           drive_power_error_occured, DRIVE_POWER_ERROR_UNDERFLOW_Uc, DRIVE_POWER_ERROR_OVERFLOW_Uc);
    // Rot.
    drive_compare_set_flag(drive_compare_zero_voltage(POWER_VALUE_Urot),
                           drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Urot, DRIVE_POWER_ERROR_IDLE_Urot);
    // Токи - отклонения от нуля.
    // A.
    drive_compare_set_flag(drive_compare_zero_current(POWER_VALUE_Ia),
                           drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Ia, DRIVE_POWER_ERROR_IDLE_Ia);
    // B.
    drive_compare_set_flag(drive_compare_zero_current(POWER_VALUE_Ib),
                           drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Ic, DRIVE_POWER_ERROR_IDLE_Ib);
    // C.
    drive_compare_set_flag(drive_compare_zero_current(POWER_VALUE_Ic),
                           drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Ic, DRIVE_POWER_ERROR_IDLE_Ic);
    // Exc.
    drive_compare_set_flag(drive_compare_zero_current(POWER_VALUE_Iexc),
                           drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Iexc, DRIVE_POWER_ERROR_IDLE_Iexc);
    // Rot.
    drive_compare_set_flag(drive_compare_zero_current(POWER_VALUE_Irot),
                           drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Irot, DRIVE_POWER_ERROR_IDLE_Irot);
    // Напряжения - превышение допустимой разности.
    // A.
    drive_compare_set_flag(drive_compare_input_voltage(POWER_VALUE_Ua, drive.settings.U_nom_delta_allow),
                           drive_set_warning, DRIVE_WARNING_POWER_UNDERFLOW_Ua, DRIVE_WARNING_POWER_OVERFLOW_Ua);
    // B.
    drive_compare_set_flag(drive_compare_input_voltage(POWER_VALUE_Ub, drive.settings.U_nom_delta_allow),
                           drive_set_warning, DRIVE_WARNING_POWER_UNDERFLOW_Uc, DRIVE_WARNING_POWER_OVERFLOW_Ub);
    // C.
    drive_compare_set_flag(drive_compare_input_voltage(POWER_VALUE_Uc, drive.settings.U_nom_delta_allow),
                           drive_set_warning, DRIVE_WARNING_POWER_UNDERFLOW_Uc, DRIVE_WARNING_POWER_OVERFLOW_Uc);
}

/**
 * Обрабатывает данные питания в состоянии простоя (готовности).
 * @param phase Текущая фаза.
 */
static void drive_process_power_idle(phase_t phase)
{
    if(phase == drive.power_phase){
        power_calc_values(&drive.power, POWER_CHANNELS);

        if(power_data_avail(&drive.power, POWER_CHANNELS)){
            drive_set_flag(DRIVE_FLAG_POWER_DATA_AVAIL);
            drive_check_power_idle();
        }else{
            drive_clear_flag(DRIVE_FLAG_POWER_DATA_AVAIL);
            drive_error_occured(DRIVE_ERROR_POWER_DATA_NOT_AVAIL);
        }
    }
}

/**
 * Обработка состояния простоя(готовности) привода.
 * @param phase Фаза.
 * @return Код ошибки.
 */
static err_t drive_state_process_idle(phase_t phase)
{
    // Нужна определённая фаза.
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    
    drive_process_power_idle(phase);
    
    return E_NO_ERROR;
}

/*
 * Состояние работы.
 */

/**
 * Функция регулировки.
 */
static void drive_regulate(void)
{
    if(drive_reference() < REFERENCE_MIN){
        drive.triacs_pairs_angle_ticks = 0;//TRIACS_TIM_ANGLE_TICKS_MAX;
        drive.triac_exc_angle_ticks = 0;//TRIACS_TIM_ANGLE_TICKS_MAX;
    }else{
        drive.triacs_pairs_angle_ticks = (uint32_t)drive_reference() * TRIACS_TIM_ANGLE_TICKS_MAX / 100;
        drive.triac_exc_angle_ticks = (uint32_t)drive_reference() * TRIAC_EXC_ANGLE_TICKS_MAX / 100;
    }
}

/**
 * Проверяет значение входов питания в состоянии работы.
 */
static void drive_check_power_running(void)
{
    // Напряжения - превышение критической разности.
    // A.
    drive_compare_set_flag(drive_compare_input_voltage(POWER_VALUE_Ua, drive.settings.U_nom_delta_crit),
                           drive_power_error_occured, DRIVE_POWER_ERROR_UNDERFLOW_Ua, DRIVE_POWER_ERROR_OVERFLOW_Ua);
    // B.
    drive_compare_set_flag(drive_compare_input_voltage(POWER_VALUE_Ub, drive.settings.U_nom_delta_crit),
                           drive_power_error_occured, DRIVE_POWER_ERROR_UNDERFLOW_Uc, DRIVE_POWER_ERROR_OVERFLOW_Ub);
    // C.
    drive_compare_set_flag(drive_compare_input_voltage(POWER_VALUE_Uc, drive.settings.U_nom_delta_crit),
                           drive_power_error_occured, DRIVE_POWER_ERROR_UNDERFLOW_Uc, DRIVE_POWER_ERROR_OVERFLOW_Uc);
    // Rot.
    //drive_compare_set_flag(drive_compare_zero_voltage(POWER_VALUE_Urot),
    //                       drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Urot, DRIVE_POWER_ERROR_IDLE_Urot);
    // Токи - отклонения от нуля.
    // A.
    //drive_compare_set_flag(drive_compare_zero_current(POWER_VALUE_Ia),
    //                       drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Ia, DRIVE_POWER_ERROR_IDLE_Ia);
    // B.
    //drive_compare_set_flag(drive_compare_zero_current(POWER_VALUE_Ib),
    //                       drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Ic, DRIVE_POWER_ERROR_IDLE_Ib);
    // C.
    //drive_compare_set_flag(drive_compare_zero_current(POWER_VALUE_Ic),
    //                       drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Ic, DRIVE_POWER_ERROR_IDLE_Ic);
    // Exc.
    //drive_compare_set_flag(drive_compare_zero_current(POWER_VALUE_Iexc),
    //                       drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Iexc, DRIVE_POWER_ERROR_IDLE_Iexc);
    // Rot.
    //drive_compare_set_flag(drive_compare_zero_current(POWER_VALUE_Irot),
    //                       drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Irot, DRIVE_POWER_ERROR_IDLE_Irot);
    // Напряжения - превышение допустимой разности.
    // A.
    drive_compare_set_flag(drive_compare_input_voltage(POWER_VALUE_Ua, drive.settings.U_nom_delta_allow),
                           drive_set_warning, DRIVE_WARNING_POWER_UNDERFLOW_Ua, DRIVE_WARNING_POWER_OVERFLOW_Ua);
    // B.
    drive_compare_set_flag(drive_compare_input_voltage(POWER_VALUE_Ub, drive.settings.U_nom_delta_allow),
                           drive_set_warning, DRIVE_WARNING_POWER_UNDERFLOW_Uc, DRIVE_WARNING_POWER_OVERFLOW_Ub);
    // C.
    drive_compare_set_flag(drive_compare_input_voltage(POWER_VALUE_Uc, drive.settings.U_nom_delta_allow),
                           drive_set_warning, DRIVE_WARNING_POWER_UNDERFLOW_Uc, DRIVE_WARNING_POWER_OVERFLOW_Uc);
}

/**
 * Обрабатывает данные питания в состоянии работы.
 * @param phase Текущая фаза.
 */
static void drive_process_power_running(phase_t phase)
{
    if(phase == drive.power_phase){
        power_calc_values(&drive.power, POWER_CHANNELS);

        if(power_data_avail(&drive.power, POWER_CHANNELS)){
            drive_set_flag(DRIVE_FLAG_POWER_DATA_AVAIL);
            drive_check_power_running();
            
            if(drive_ready()){
                ramp_calc_step(&drive.ramp);
                drive_regulate();
            }
        }else{
            drive_clear_flag(DRIVE_FLAG_POWER_DATA_AVAIL);
            drive_error_occured(DRIVE_ERROR_POWER_DATA_NOT_AVAIL);
        }
    }
}

/**
 * Обработка состояния работы привода.
 * @param phase Фаза.
 * @return Код ошибки.
 */
static err_t drive_state_process_running(phase_t phase)
{
    // Нужна определённая фаза.
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    // Нужно какое-либо направление.
    if(phase_state_drive_direction() == DRIVE_DIR_UNK) return E_INVALID_VALUE;
    
    drive_setup_triacs_pairs_timer(phase);
    drive_setup_triac_exc_timer(phase);
    
    drive_process_power_running(phase);
    
    return E_NO_ERROR;
}


/*
 * Состояние ошибки.
 */

/**
 * Обработка состояния ошибки привода.
 * @param phase Фаза.
 * @return Код ошибки.
 */
static err_t drive_state_process_error(phase_t phase)
{
    // Нужна определённая фаза.
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    // Нужно какое-либо направление.
    if(phase_state_drive_direction() == DRIVE_DIR_UNK) return E_INVALID_VALUE;
    
    if(drive.errors == DRIVE_ERROR_NONE){
        drive.state = DRIVE_STATE_IDLE;
    }
    
    return E_NO_ERROR;
}




/*
 * Интерфейсный функции.
 * Документация в drive.h
 */

err_t drive_init(void)
{
    memset(&drive, 0x0, sizeof(drive_t));
    
    ramp_init(&drive.ramp);
    
    drive_update_settings();
    
    drive.flags = DRIVE_FLAG_NONE;
    drive.state = DRIVE_STATE_IDLE;
    drive.errors = DRIVE_ERROR_NONE;
    
    drive.triacs_pairs_open_ticks = TRIACS_TIM_OPEN_TIME_DEFAULT;
    drive.triac_exc_open_ticks = TRIAC_EXC_TIM_OPEN_TIME_DEFAULT;
    
    power_value_init(&drive.power_values[0],POWER_CHANNEL_AC, 0x10000); // Ia
    power_value_init(&drive.power_values[1],POWER_CHANNEL_AC, 0x10000); // Ua
    power_value_init(&drive.power_values[2],POWER_CHANNEL_AC, 0x10000); // Ib
    power_value_init(&drive.power_values[3],POWER_CHANNEL_AC, 0x10000); // Ub
    power_value_init(&drive.power_values[4],POWER_CHANNEL_AC, 0x10000); // Ic
    power_value_init(&drive.power_values[5],POWER_CHANNEL_AC, 0x10000); // Uc
    power_value_init(&drive.power_values[6],POWER_CHANNEL_DC, 0x10000); // Irot
    power_value_init(&drive.power_values[7],POWER_CHANNEL_DC, 0x10000); // Urot
    power_value_init(&drive.power_values[8],POWER_CHANNEL_DC, 0x10000); // Iexc
    power_value_init(&drive.power_values[9],POWER_CHANNEL_DC, 0x10000); // Iref
    power_value_init(&drive.power_values[10],POWER_CHANNEL_DC, 0x10000); // Ifan
    
    power_init(&drive.power, drive.power_values, POWER_CHANNELS);
    
    return E_NO_ERROR;
}

err_t drive_update_settings(void)
{
    drive.settings.U_nom = settings_valuef(PARAM_ID_U_NOM);
    drive.settings.U_nom_delta_allow = settings_valueu(PARAM_ID_U_NOM_ALLOW_VAR) * drive.settings.U_nom / 100;
    drive.settings.U_nom_delta_crit = settings_valueu(PARAM_ID_U_NOM_CRIT_VAR) * drive.settings.U_nom / 100;
    drive.settings.U_zero_noise = settings_valuef(PARAM_ID_U_ZERO_NOISE);
    drive.settings.I_zero_noise = settings_valuef(PARAM_ID_I_ZERO_NOISE);
    drive.settings.exc_phase = settings_valueu(PARAM_ID_EXC_PHASE);
    ramp_set_time(&drive.ramp, settings_valueu(PARAM_ID_RAMP_TIME));

    return E_NO_ERROR;
}

bool drive_flag(drive_flag_t flag)
{
    return drive_flags_is_set(flag);
}

drive_flags_t drive_flags(void)
{
    return drive.flags;
}

drive_state_t drive_state(void)
{
    return drive.state;
}

bool drive_error(drive_error_t error)
{
    return (drive.errors & error) != 0;
}

drive_errors_t drive_errors(void)
{
    return drive.errors;
}

bool drive_warning(drive_warning_t warning)
{
    return (drive.warnings & warning) != 0;
}

drive_warnings_t drive_warnings(void)
{
    return drive.warnings;
}

bool drive_power_error(drive_power_error_t error)
{
    return (drive.power_errors & error) != 0;
}

drive_power_errors_t drive_power_errors(void)
{
    return drive.power_errors;
}

drive_power_calibration_t drive_power_calibration(void)
{
    return drive.power_calibration;
}

reference_t drive_reference(void)
{
    return ramp_target_reference(&drive.ramp);
}

err_t drive_set_reference(reference_t reference)
{
    // 0 ... 100 == 12000 (TRIACS_TIM_ANGLE_TICKS_MAX) ... 0
    if(reference > REFERENCE_MAX) return E_OUT_OF_RANGE;
    
    return ramp_set_target_reference(&drive.ramp, reference);
}

bool drive_ready(void)
{
    return (drive.errors) == 0 && drive_flags_is_set(DRIVE_READY_FLAGS);
}

bool drive_start(void)
{
    if(!drive_ready())
        return false;
    if(drive.state == DRIVE_STATE_IDLE){
        drive.state = DRIVE_STATE_RUNNING;
    }
    return true;
}

bool drive_stop(void)
{
    if(drive.state == DRIVE_STATE_RUNNING){
        drive.state = DRIVE_STATE_IDLE;
    }
    return true;
}

bool drive_running(void)
{
    return drive.state == DRIVE_STATE_RUNNING;
}

uint16_t drive_triacs_open_time_us(void)
{
    return OPEN_TICKS_TO_TIME(drive.triacs_pairs_open_ticks);
}

err_t drive_set_triacs_open_time_us(uint16_t time)
{
    if(time == 0) return E_INVALID_VALUE;
    
    drive.triacs_pairs_open_ticks = OPEN_TIME_TO_TICKS(time);
    
    return E_NO_ERROR;
}

uint16_t drive_triac_exc_open_time_us(void)
{
    return OPEN_TICKS_TO_TIME(drive.triac_exc_open_ticks);
}

err_t drive_set_triac_exc_open_time_us(uint16_t time)
{
    if(time == 0) return E_INVALID_VALUE;
    
    drive.triac_exc_open_ticks = OPEN_TIME_TO_TICKS(time);
    
    return E_NO_ERROR;
}

err_t drive_set_exc_phase(phase_t phase)
{
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    
    drive.settings.exc_phase = phase;
    
    return E_NO_ERROR;
}

err_t drive_set_triac_pair_gpio(triac_pair_number_t triac_pair, GPIO_TypeDef* GPIO_a, uint16_t pin_a, GPIO_TypeDef* GPIO_b, uint16_t pin_b)
{
    if(triac_pair >= TRIAC_PAIRS_COUNT) return E_OUT_OF_RANGE;
    return triac_pair_init(&drive.triac_pairs[triac_pair], GPIO_a, pin_a, GPIO_b, pin_b);
}

err_t drive_set_triac_exc_gpio(GPIO_TypeDef* GPIO, uint16_t pin)
{
    return triac_init(&drive.triac_exc, GPIO, pin);
}

err_t drive_set_triacs_pairs_timer(size_t index, TIM_TypeDef* TIM)
{
    if(index >= TRIACS_TIMERS_COUNT) return E_OUT_OF_RANGE;
    if(TIM == NULL) return E_NULL_POINTER;
    
    drive.timers_triacs[index].timer = TIM;
    
    return E_NO_ERROR;
}

err_t drive_set_triac_exc_timer(TIM_TypeDef* TIM)
{
    if(TIM == NULL) return E_NULL_POINTER;
    
    drive.timer_exc = TIM;
    
    return E_NO_ERROR;
}

void drive_triacs_timer0_irq_handler(void)
{
#warning do not open triacs if error.
    timer_triacs_t* tim_triacs = get_timer_triacs(TRIACS_TIMER_0);
    TIM_TypeDef* TIM = tim_triacs->timer;
    // Если нужно открыть тиристорную пару 1.
    if(TIM_GetITStatus(TIM, TRIACS_A_OPEN_CHANNEL_IT) != RESET){
        triac_pair_open(
                    get_triac_pair(tim_triacs->triacs_a)
                );
        TIM_ClearITPendingBit(TIM, TRIACS_A_OPEN_CHANNEL_IT);
    } // Если нужно закрыть тиристорную пару 1.
    if(TIM_GetITStatus(TIM, TRIACS_A_CLOSE_CHANNEL_IT) != RESET){
        triac_pair_close(
                    get_triac_pair(tim_triacs->triacs_a)
                );
        TIM_ClearITPendingBit(TIM, TRIACS_A_CLOSE_CHANNEL_IT);
    } // Если нужно открыть тиристорную пару 2.
    if(TIM_GetITStatus(TIM, TRIACS_B_OPEN_CHANNEL_IT) != RESET){
        triac_pair_open(
                    get_triac_pair(tim_triacs->triacs_b)
                );
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
#warning do not open triacs if error.
    timer_triacs_t* tim_triacs = get_timer_triacs(TRIACS_TIMER_1);
    TIM_TypeDef* TIM = tim_triacs->timer;
    // Если нужно открыть тиристорную пару 1.
    if(TIM_GetITStatus(TIM, TRIACS_A_OPEN_CHANNEL_IT) != RESET){
        triac_pair_open(
                    get_triac_pair(tim_triacs->triacs_a)
                );
        TIM_ClearITPendingBit(TIM, TRIACS_A_OPEN_CHANNEL_IT);
    } // Если нужно закрыть тиристорную пару 1.
    if(TIM_GetITStatus(TIM, TRIACS_A_CLOSE_CHANNEL_IT) != RESET){
        triac_pair_close(
                    get_triac_pair(tim_triacs->triacs_a)
                );
        TIM_ClearITPendingBit(TIM, TRIACS_A_CLOSE_CHANNEL_IT);
    } // Если нужно открыть тиристорную пару 2.
    if(TIM_GetITStatus(TIM, TRIACS_B_OPEN_CHANNEL_IT) != RESET){
        triac_pair_open(
                    get_triac_pair(tim_triacs->triacs_b)
                );
        TIM_ClearITPendingBit(TIM, TRIACS_B_OPEN_CHANNEL_IT);
    } // Если нужно закрыть тиристорную пару 2.
    if(TIM_GetITStatus(TIM, TRIACS_B_CLOSE_CHANNEL_IT) != RESET){
        triac_pair_close(
                    get_triac_pair(tim_triacs->triacs_b)
                );
        TIM_ClearITPendingBit(TIM, TRIACS_B_CLOSE_CHANNEL_IT);
    }
}

void drive_triac_exc_timer_irq_handler(void)
{
#warning do not open triacs if error.
    TIM_TypeDef* TIM = drive.timer_exc;
    // Если нужно открыть симистор первого полупериода.
    if(TIM_GetITStatus(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL_IT) != RESET){
        triac_open(&drive.triac_exc);
        TIM_ClearITPendingBit(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL_IT);
    } // Если нужно закрыть симистор первого полупериода.
    if(TIM_GetITStatus(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL_IT) != RESET){
        triac_close(&drive.triac_exc);
        TIM_ClearITPendingBit(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL_IT);
    } // Если нужно открыть симистор второго полупериода.
    if(TIM_GetITStatus(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL_IT) != RESET){
        triac_open(&drive.triac_exc);
        TIM_ClearITPendingBit(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL_IT);
    } // Если нужно закрыть симистор второго полупериода.
    if(TIM_GetITStatus(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL_IT) != RESET){
        triac_close(&drive.triac_exc);
        TIM_ClearITPendingBit(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL_IT);
    }
}

err_t drive_process_null_sensor(phase_t phase)
{
    // Обработаем текущую фазу.
    phase_state_handle(phase);
    
    switch(drive.state){
        case DRIVE_STATE_INIT:
            return drive_state_process_init(phase);
        case DRIVE_STATE_IDLE:
            return drive_state_process_idle(phase);
        case DRIVE_STATE_RUNNING:
            return drive_state_process_running(phase);
        case DRIVE_STATE_ERROR:
            return drive_state_process_error(phase);
    }
    
    return E_NO_ERROR;
}

err_t drive_process_power_adc_values(power_channels_t channels, uint16_t* adc_values)
{
    return power_process_adc_values(&drive.power, channels, adc_values);
}

err_t drive_calc_power_values(power_channels_t channels)
{
    return power_calc_values(&drive.power, channels);
}

err_t drive_calibrate_power(power_channels_t channels)
{
    return power_calibrate(&drive.power, channels);
}

bool drive_power_data_avail(power_channels_t channels)
{
    return drive_flags_is_set(DRIVE_FLAG_POWER_DATA_AVAIL);
}

int16_t drive_power_channel_raw_value(size_t channel)
{
    return power_channel_raw_value(&drive.power, channel);
}

int16_t drive_power_channel_raw_value_avg(size_t channel)
{
    return power_channel_raw_value_avg(&drive.power, channel);
}

int16_t drive_power_channel_raw_value_rms(size_t channel)
{
    return power_channel_raw_value_rms(&drive.power, channel);
}

fixed32_t drive_power_channel_real_value(size_t channel)
{
    return power_channel_real_value(&drive.power, channel);
}

fixed32_t drive_power_channel_real_value_avg(size_t channel)
{
    return power_channel_real_value_avg(&drive.power, channel);
}

fixed32_t drive_power_channel_real_value_rms(size_t channel)
{
    return power_channel_real_value_rms(&drive.power, channel);
}
