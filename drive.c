#include "drive.h"
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

//! Перевод времени открытия в тики таймера.
#define OPEN_TIME_TO_TICKS(T) (((uint32_t)T * TRIACS_TIM_PERIOD) / TRIACS_TIM_PERIOD_US)
//! Перевод тиков таймера в время открытия.
#define OPEN_TICKS_TO_TIME(T) (((uint32_t)T * TRIACS_TIM_PERIOD_US) / TRIACS_TIM_PERIOD)


//! Число периодов калибровки питания.
#define DRIVE_POWER_CALIBRATION_PERIODS 5

//! Необходимые для готовности флаги.
#define DRIVE_READY_FLAGS (DRIVE_FLAG_POWER_DATA_AVAIL)


//! Тип структуры таймера для открытия тиристора.
typedef struct _Timer_Triacs {
    TIM_TypeDef* timer; //!< Таймер для включения тиристоров.
    triac_pair_number_t triacs_a; //!< Пара тиристоров А.
    triac_pair_number_t triacs_b; //!< Пара тиристоров B.
} timer_triacs_t;

//! Структура привода.
typedef struct _Drive {
    drive_flags_t flags; //!< Флаги.
    drive_state_t state; //!< Состояние.
    drive_errors_t errors; //!< Ошибка.
    drive_power_calibration_t power_calibration; //!< Состояние калибровки питания.
    phase_t power_calibration_phase; //!< Фаза начала калибровки питания.
    size_t power_calibration_periods; //!< Число периодов калибровки питания.
    
    reference_t reference; //!< Задание.
    uint16_t timer_angle_ticks; //!< Угол открытия тиристора - значение регистра сравнения таймера.
    uint16_t triacs_open_tim_ticks; //!< Время открытия тиристоров в тиках таймера.
    
    triac_pair_t triac_pairs[TRIAC_PAIRS_COUNT]; //!< Пары тиристоров.
    triac_t triac_exc; //!< Тиристор возбуждения.
    
    timer_triacs_t timers_triacs[TRIACS_TIMERS_COUNT]; //!< Тиристоры таймеров и таймеры.
    size_t current_timer_triacs; //!< Текущий индекс таймеров тиристоров.
    power_value_t power_values[POWER_VALUES_COUNT]; //!< Значение каналов АЦП.
    power_t power; //!< Питание.
} drive_t;

//! Состояние привода.
static drive_t drive;


//! Алиас для текущего значения угла открытия тиристоров в тиках таймера.
#define CURRENT_TIMER_ANGLE_TICKS (drive.timer_angle_ticks)



err_t drive_init(void)
{
    memset(&drive, 0x0, sizeof(drive_t));
    
    drive.flags = DRIVE_FLAG_NONE;
    drive.state = DRIVE_STATE_IDLE;
    drive.errors = DRIVE_ERROR_NONE;
    
    drive.triacs_open_tim_ticks = TRIACS_TIM_OPEN_TIME_DEFAULT;
    
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

ALWAYS_INLINE static void drive_set_flag(drive_flag_t flag)
{
    drive.flags |= flag;
}

ALWAYS_INLINE static void drive_clear_flag(drive_flag_t flag)
{
    drive.flags &= ~flag;
}

bool drive_flag(drive_flag_t flag)
{
    return (drive.flags & flag) != 0;
}

drive_flags_t drive_flags(void)
{
    return drive.flags;
}

drive_state_t drive_state(void)
{
    return drive.state;
}

ALWAYS_INLINE static void drive_set_error(drive_error_t error)
{
    drive.errors |= error;
}

ALWAYS_INLINE static void drive_clear_error(drive_error_t error)
{
    drive.errors &= ~error;
}

bool drive_error(drive_error_t error)
{
    return (drive.errors & error) != 0;
}

drive_errors_t drive_errors(void)
{
    return drive.errors;
}

drive_power_calibration_t drive_power_calibration(void)
{
    return drive.power_calibration;
}

reference_t drive_reference(void)
{
    return drive.reference;
}

err_t drive_set_reference(reference_t reference)
{
    // 0 ... 100 == 12000 (TRIACS_TIM_ANGLE_TICKS_MAX) ... 0
    if(reference > REFERENCE_MAX) return E_OUT_OF_RANGE;
    
    drive.reference = reference;
    
    if(reference < REFERENCE_MIN){
        drive.timer_angle_ticks = 0;//TRIACS_TIM_ANGLE_TICKS_MAX;
    }else{
        drive.timer_angle_ticks = (uint32_t)reference * TRIACS_TIM_ANGLE_TICKS_MAX / 100;
    }
    return E_NO_ERROR;
}

bool drive_ready(void)
{
    return (drive.errors) == 0 &&
           ((drive.flags & DRIVE_READY_FLAGS) == DRIVE_READY_FLAGS);
}

bool drive_start(void)
{
    return false;
}

bool drive_stop(void)
{
    return false;
}

bool drive_running(void)
{
    return false;
}

uint16_t drive_triacs_open_time_us(void)
{
    return OPEN_TICKS_TO_TIME(drive.triacs_open_tim_ticks);
}

err_t drive_set_triacs_open_time_us(uint16_t time)
{
    drive.triacs_open_tim_ticks = OPEN_TIME_TO_TICKS(time);
    
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

err_t drive_set_triac_timer(size_t index, TIM_TypeDef* TIM)
{
    if(index >= TRIACS_TIMERS_COUNT) return E_OUT_OF_RANGE;
    
    drive.timers_triacs[index].timer = TIM;
    
    return E_NO_ERROR;
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
 * @param angle_ticks Угол открытия (0 ... TRIACS_TIM_ANGLE_TICKS_MAX)
 */
static void timer_triacs_setup_next(triac_pair_number_t triacs_a, triac_pair_number_t triacs_b, uint16_t angle_ticks)
{
    // Если выходим за границу - возврат.
    if(angle_ticks >= TRIACS_TIM_ANGLE_TICKS_MAX) return;
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
    TIM_SetCompare1(tim_trcs->timer, (TRIACS_TIM_ANGLE_TICKS_MAX) - angle_ticks);
    // Закрытие первой пары тиристоров.
    TIM_SetCompare2(tim_trcs->timer, (TRIACS_TIM_ANGLE_TICKS_MAX + drive.triacs_open_tim_ticks) - angle_ticks);
    // Открытие второй пары тиристоров.
    TIM_SetCompare3(tim_trcs->timer, (TRIACS_TIM_ANGLE_TICKS_MAX + TRIACS_TIM_OFFSET) - angle_ticks);
    // Закрытие второй пары тиристоров.
    TIM_SetCompare4(tim_trcs->timer, (TRIACS_TIM_ANGLE_TICKS_MAX + TRIACS_TIM_OFFSET + drive.triacs_open_tim_ticks) - angle_ticks);
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

void drive_triacs_timer0_irq_handler(void)
{
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
                drive.power_calibration_phase = phase;
                drive.power_calibration = DRIVE_PWR_CALIBRATION_RUNNING;
            }
            break;
        case DRIVE_PWR_CALIBRATION_RUNNING:
            
            if(phase == drive.power_calibration_phase){
                
                if(++ drive.power_calibration_periods >= DRIVE_POWER_CALIBRATION_PERIODS){
                    power_calc_values(&drive.power, POWER_CHANNELS);
                    
                    power_calibrate(&drive.power, POWER_CHANNELS);
                    drive_set_flag(DRIVE_FLAG_POWER_CALIBRATED);
                    drive.power_calibration = DRIVE_PWR_CALIBRATION_DONE;
                }
            }
            break;
        case DRIVE_PWR_CALIBRATION_DONE:
        default:
            break;
    }
}

static void drive_check_power_idle(void)
{
    #warning add idle power check here.
}

static void drive_check_power_running(void)
{
    #warning add running power check here.
}

static void drive_check_power(void)
{
    #warning add power check here.
    
    if(drive.state == DRIVE_STATE_RUNNING){
        drive_check_power_running();
    }else{
        drive_check_power_idle();
    }
}

/**
 * Производит вычисление значений питания и калибровку, если это необходимо.
 * @param phase Текущая фаза.
 */
static void drive_process_power(phase_t phase)
{
    if(!drive_flag(DRIVE_FLAG_POWER_CALIBRATED)){
        drive_process_power_calibration(phase);
    }else
    if(phase == drive.power_calibration_phase){
        power_calc_values(&drive.power, POWER_CHANNELS);

        if(power_data_avail(&drive.power, POWER_CHANNELS)){
            drive_set_flag(DRIVE_FLAG_POWER_DATA_AVAIL);
            drive_check_power();
        }else{
            drive_clear_flag(DRIVE_FLAG_POWER_DATA_AVAIL);
        }
    }
}

static err_t drive_setup_triacs(phase_t phase)
{
    // Нужна определённая фаза.
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    // Нужно какое-либо направление.
    if(phase_state_drive_direction() == DRIVE_DIR_UNK) return E_INVALID_VALUE;
    #warning add run drive state handle here.
    
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
    
    timer_triacs_setup_next(triacs_seq[triacs_index], triacs_seq[triacs_index + 1], CURRENT_TIMER_ANGLE_TICKS);
    
    return E_NO_ERROR;
}

err_t drive_process_null_sensor(phase_t phase)
{
    // Обработаем текущую фазу.
    phase_state_handle(phase);
    
    // Настроем открытие тиристоров.
    drive_setup_triacs(phase);
    
    // Обработка калибровки питания.
    drive_process_power(phase);
    
    #warning add power fail detection code here.
    
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
    return power_data_avail(&drive.power, channels);
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
