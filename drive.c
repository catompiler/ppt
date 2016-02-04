#include "drive.h"
#include "settings.h"
#include "ramp.h"
#include "pid_controller/pid_controller.h"
#include <string.h>


//! Максимальное значение PID-регулятора напряжения якоря.
#define DRIVE_ROT_PID_VALUE_MAX 10000
//! Минимальное значение PID-регулятора напряжения якоря.
#define DRIVE_ROT_PID_VALUE_MIN 0

//! Максимальное значение PID-регулятора тока возбуждения.
#define DRIVE_EXC_PID_VALUE_MAX 10000
//! Минимальное значение PID-регулятора тока возбуждения.
#define DRIVE_EXC_PID_VALUE_MIN 0

//! dt PID-регулятора.
#define DRIVE_PID_DT 0x51f


//! Число периодов калибровки питания.
#define DRIVE_POWER_CALIBRATION_PERIODS 5

//! Необходимые для готовности флаги.
#define DRIVE_READY_FLAGS (DRIVE_FLAG_POWER_DATA_AVAIL)

//! Тип состояния машины состояний привода.
typedef enum _Drive_State {
    DRIVE_STATE_INIT = 0,
    DRIVE_STATE_CALIBRATION,
    DRIVE_STATE_IDLE,
    DRIVE_STATE_START,
    DRIVE_STATE_RUN,
    DRIVE_STATE_STOP,
    DRIVE_STATE_STOP_ERROR,
    DRIVE_STATE_ERROR
} drive_state_t;

//! Тип структуры настроек привода.
typedef struct _Drive_Settings {
    fixed32_t U_nom; //!< Номинальное напряжение, В.
    fixed32_t U_nom_delta_allow; //!< Допустимое отклонение номинального напряжения, В.
    fixed32_t U_nom_delta_crit; //!< Критическое отклонение номинального напряжения, В.
    fixed32_t U_zero_noise; //!< Шум напряжения нуля, В.
    fixed32_t I_zero_noise; //!< Шум тока нуля, А.
    fixed32_t U_rot_nom; //!< Номинальное возбуждение якоря.
    fixed32_t I_rot_nom; //!< Номинальный ток якоря.
    fixed32_t I_exc; //!< Номинальный ток возбуждения.
    phase_t phase_exc; //!< Фаза возбуждения.
    uint32_t stop_time_rot; //!< Время остановки ротора в периодах.
    uint32_t stop_time_exc; //!< Время остановки возбуждения в периодах.
    uint32_t start_time_exc; //!< Время остановки возбуждения в периодах.
} drive_settings_t;

//! Структура привода.
typedef struct _Drive {
    drive_flags_t flags; //!< Флаги.
    drive_status_t status; //!< Статус.
    drive_state_t state; //!< Состояние.
    drive_errors_t errors; //!< Ошибки.
    drive_warnings_t warnings; //!< Предупреждения.
    drive_power_errors_t power_errors; //!< Ошибки питания.
    drive_power_calibration_t power_calibration; //!< Состояние калибровки питания.
    drive_starting_t starting_state; //!< Состояние запуска привода.
    drive_stopping_t stopping_state; //!< Состояние останова привода.
    phase_t power_phase; //!< Фаза начала калибровки питания.
    size_t power_calibration_periods; //!< Число периодов калибровки питания.
    
    power_value_t power_values[POWER_VALUES_COUNT]; //!< Значение каналов АЦП.
    power_t power; //!< Питание.
    
    ramp_t ramp; //!< Разгон.
    
    pid_controller_t rot_pid; //!< ПИД-регулятор напряжения якоря.
    pid_controller_t exc_pid; //!< ПИД-регулятор тока возбуждения.
    
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
ALWAYS_INLINE static drive_status_t drive_get_state(void)
{
    return drive.status;
}

/**
 * Установка состояния.
 * @param state Состояние.
 */
ALWAYS_INLINE static void drive_set_state(drive_status_t state)
{
    drive.status = state;
}

/**
 * Получение состояния калибровки.
 * @return Состояние калибровки.
 */
ALWAYS_INLINE static drive_power_calibration_t drive_get_calibration_state(void)
{
    return drive.power_calibration;
}

/**
 * Установка состояния калибровки.
 * @param state Состояние калибровки.
 */
ALWAYS_INLINE static void drive_set_calibration_state(drive_power_calibration_t state)
{
    drive.power_calibration = state;
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

ALWAYS_INLINE static phase_t drive_power_phase(void)
{
    return drive.power_phase;
}

ALWAYS_INLINE static void drive_set_power_phase(phase_t phase)
{
    drive.power_phase = phase;
}


/*
 * Обработка возникновения ошибки.
 */

/**
 * Обработчик возникновения ошибки.
 * @param error Ошибка.
 */
static void drive_error_occured(drive_errors_t error)
{
    drive_set_error(error);
    
    if(drive_get_state() != DRIVE_STATUS_ERROR){
        drive_set_state(DRIVE_STATUS_ERROR);
        drive_triacs_stop();
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
 * Общие функции обработки питания.
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
 * Проверка напряжений фаз.
 */
static void drive_check_power_u_in(void)
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
 * Обработка данных с АЦП.
 * @param phase Текущая фаза.
 * @return Флаг успешной обработки данных питания.
 */
static bool drive_process_power(phase_t phase)
{
    if(phase == drive.power_phase){
        power_calc_values(&drive.power, POWER_CHANNELS);

        if(power_data_avail(&drive.power, POWER_CHANNELS)){
            drive_set_flag(DRIVE_FLAG_POWER_DATA_AVAIL);
            return true;
        }
        drive_clear_flag(DRIVE_FLAG_POWER_DATA_AVAIL);
        drive_error_occured(DRIVE_ERROR_POWER_DATA_NOT_AVAIL);
    }
    return false;
}

/*
 * Обработка состояний привода
 * с необходимыми функциями.
 */

/*
 * Состояние начальной инициализации.
 */

/**
 * Обработка состояния начальной инициализации привода.
 * @param phase Фаза.
 * @return Код ошибки.
 */
static err_t drive_state_process_init(phase_t phase)
{
    drive_set_power_phase(phase);
    power_reset_channels(&drive.power, POWER_CHANNELS);
    
    return E_NO_ERROR;
}

/*
 * Состояние калибровки питания.
 */

/**
 * Производит калибровку питания.
 * @param phase Текущая фаза.
 */
static void drive_state_process_power_calibration(phase_t phase)
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
            drive_set_state(DRIVE_STATUS_IDLE);
        default:
            break;
    }
}

/*
 * Состояние простоя (готовность).
 */

/**
 * Проверяет значение входов питания в состоянии простоя (готовности).
 */
static void drive_check_power_idle(void)
{
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
    // Напряжения фаз.
    
#warning adc calibration - uncomment.
    //drive_check_power_u_in();
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
    ramp_calc_step(&drive.ramp);
    
    if(ramp_current_reference(&drive.ramp) < REFERENCE_MIN){
        drive_triacs_set_pairs_open_angle(0);
        drive_triacs_set_exc_open_angle(0);
    }else if(ramp_current_reference(&drive.ramp) > (REFERENCE_MAX * 3 / 4)){
        return;
    }else{
        //drive.triacs_pairs_angle_ticks = (uint32_t)ramp_current_reference(&drive.ramp) * TRIACS_TIM_ANGLE_TICKS_MAX / 100;
        //drive.triac_exc_angle_ticks = (uint32_t)ramp_current_reference(&drive.ramp) * TRIAC_EXC_ANGLE_TICKS_MAX / 100;
        /*
        fixed32_t u_rot = power_channel_real_value_avg(&drive.power, POWER_VALUE_Urot);
        fixed32_t u_rot_ref = (int64_t)drive.settings.U_rot_nom * ramp_current_reference(&drive.ramp) / 100;
        pid_controller_calculate(&drive.rot_pid, u_rot_ref - u_rot, DRIVE_PID_DT);
        drive.triacs_pairs_angle_ticks = pid_controller_value(&drive.rot_pid);
        */
        drive_triacs_set_pairs_open_angle((uint32_t)ramp_current_reference(&drive.ramp) * TRIACS_PAIRS_ANGLE_MAX / 100);
        drive_triacs_set_exc_open_angle((uint32_t)ramp_current_reference(&drive.ramp) * TRIAC_EXC_ANGLE_MAX / 100);
    }
}

/**
 * Проверяет значение входов питания в состоянии работы.
 */
static void drive_check_power_running(void)
{
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
    
    // Напряжения фаз.
#warning adc calibration - uncomment.
    //drive_check_power_u_in();
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
    
    drive_triacs_setup_next_pairs(phase);
    drive_triacs_setup_exc(phase, drive.settings.phase_exc);
    
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
        drive.status = DRIVE_STATUS_IDLE;
    }
    
    return E_NO_ERROR;
}

static err_t drive_process_states(phase_t phase)
{
    if(phase == PHASE_UNK){
        return E_INVALID_VALUE;
    }
    
    // Обработаем текущую фазу.
    phase_state_handle(phase);
    
    switch(drive.state){
        case DRIVE_STATE_INIT:
            break;
        case DRIVE_STATE_CALIBRATION:
            break;
        case DRIVE_STATE_IDLE:
            break;
        case DRIVE_STATE_START:
            break;
        case DRIVE_STATE_RUN:
            break;
        case DRIVE_STATE_STOP:
            break;
        case DRIVE_STATE_STOP_ERROR:
            break;
        case DRIVE_STATE_ERROR:
            break;
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
    pid_controller_init(&drive.rot_pid, 0, 0, 0);
    pid_controller_clamp(&drive.rot_pid, DRIVE_ROT_PID_VALUE_MIN, DRIVE_ROT_PID_VALUE_MAX);
    pid_controller_init(&drive.exc_pid, 0, 0, 0);
    pid_controller_clamp(&drive.exc_pid, DRIVE_EXC_PID_VALUE_MIN, DRIVE_EXC_PID_VALUE_MAX);
    
    drive_update_settings();
    
    drive.flags = DRIVE_FLAG_NONE;
    drive.status = DRIVE_STATUS_INIT;
    drive.state = DRIVE_STATE_INIT;
    drive.errors = DRIVE_ERROR_NONE;
    drive.starting_state = DRIVE_STARTING_NONE;
    drive.stopping_state = DRIVE_STOPPING_NONE;
    
    drive_triacs_init();
    
    power_value_init(&drive.power_values[POWER_VALUE_Ua],POWER_CHANNEL_AC, 0x4c14); // Ua
    power_value_init(&drive.power_values[POWER_VALUE_Ia],POWER_CHANNEL_AC, 0x10000); // Ia
    power_value_init(&drive.power_values[POWER_VALUE_Ub],POWER_CHANNEL_AC, 0x4c4d); // Ub
    power_value_init(&drive.power_values[POWER_VALUE_Ib],POWER_CHANNEL_AC, 0x10000); // Ib
    power_value_init(&drive.power_values[POWER_VALUE_Uc],POWER_CHANNEL_AC, 0x4cb7); // Uc
    power_value_init(&drive.power_values[POWER_VALUE_Ic],POWER_CHANNEL_AC, 0x10000); // Ic
    power_value_init(&drive.power_values[POWER_VALUE_Urot],POWER_CHANNEL_AC, 0x7276); // Urot
    power_value_init(&drive.power_values[POWER_VALUE_Irot],POWER_CHANNEL_DC, 0x10000); // Irot
    power_value_init(&drive.power_values[POWER_VALUE_Iexc],POWER_CHANNEL_DC, 0x10000); // Iexc
    power_value_init(&drive.power_values[POWER_VALUE_Iref],POWER_CHANNEL_DC, 0x10000); // Iref
    power_value_init(&drive.power_values[POWER_VALUE_Ifan],POWER_CHANNEL_DC, 0x10000); // Ifan
    
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
    drive.settings.U_rot_nom = settings_valuef(PARAM_ID_U_ROT_NOM);
    drive.settings.I_rot_nom = settings_valuef(PARAM_ID_I_ROT_NOM);
    drive.settings.I_exc = settings_valuef(PARAM_ID_I_EXC);
    drive.settings.phase_exc = settings_valueu(PARAM_ID_EXC_PHASE);
    drive.settings.stop_time_rot = settings_valueu(PARAM_ID_ROT_STOP_TIME) * POWER_FREQ;
    drive.settings.stop_time_exc = settings_valueu(PARAM_ID_EXC_STOP_TIME) * POWER_FREQ;
    drive.settings.start_time_exc = settings_valueu(PARAM_ID_EXC_START_TIME) * POWER_FREQ;
    ramp_set_time(&drive.ramp, settings_valueu(PARAM_ID_RAMP_TIME));
    pid_controller_set_kp(&drive.rot_pid, settings_valuef(PARAM_ID_ROT_PID_K_P));
    pid_controller_set_ki(&drive.rot_pid, settings_valuef(PARAM_ID_ROT_PID_K_I));
    pid_controller_set_kd(&drive.rot_pid, settings_valuef(PARAM_ID_ROT_PID_K_D));
    pid_controller_set_kp(&drive.exc_pid, settings_valuef(PARAM_ID_EXC_PID_K_P));
    pid_controller_set_ki(&drive.exc_pid, settings_valuef(PARAM_ID_EXC_PID_K_I));
    pid_controller_set_kd(&drive.exc_pid, settings_valuef(PARAM_ID_EXC_PID_K_D));

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

drive_status_t drive_status(void)
{
    return drive.status;
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

drive_starting_t drive_starting(void)
{
    return drive.starting_state;
}

drive_stopping_t drive_stopping(void)
{
    return drive.stopping_state;
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
    if(drive.status == DRIVE_STATUS_IDLE){
        drive.starting_state = DRIVE_STARTING_NONE;
        drive.status = DRIVE_STATUS_RUN;
    }
    return true;
}

bool drive_stop(void)
{
    if(drive.status == DRIVE_STATUS_RUN){
        drive.status = DRIVE_STATUS_IDLE;
    }
    return true;
}

bool drive_running(void)
{
    return drive.status == DRIVE_STATUS_RUN;
}

err_t drive_set_exc_phase(phase_t phase)
{
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    
    drive.settings.phase_exc = phase;
    
    return E_NO_ERROR;
}

void drive_triac_pairs_timer0_irq_handler(void)
{
#warning do not open triacs if error.
    if(drive.status != DRIVE_STATUS_RUN) return;
    
    drive_triacs_timer0_irq_handler();
}

void drive_triac_pairs_timer1_irq_handler(void)
{
#warning do not open triacs if error.
    if(drive.status != DRIVE_STATUS_RUN) return;
    
    drive_triacs_timer0_irq_handler();
}

void drive_triac_exc_timer_irq_handler(void)
{
#warning do not open triacs if error.
    if(drive.status != DRIVE_STATUS_RUN) return;
    
    drive_triacs_exc_timer_irq_handler();
}

err_t drive_process_null_sensor(phase_t phase)
{
    return drive_process_states(phase);
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
