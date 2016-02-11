#include "drive.h"
#include "settings.h"
#include "ramp.h"
#include "pid_controller/pid_controller.h"
#include "drive_regulator.h"
#include <string.h>


//! Максимальное значение PID-регулятора напряжения якоря.
#define DRIVE_ROT_PID_VALUE_MAX 0x780000 // 120.0
//! Минимальное значение PID-регулятора напряжения якоря.
#define DRIVE_ROT_PID_VALUE_MIN 0

//! Максимальное значение PID-регулятора тока возбуждения.
#define DRIVE_EXC_PID_VALUE_MAX 0xAA0000 // 170.0
//! Минимальное значение PID-регулятора тока возбуждения.
#define DRIVE_EXC_PID_VALUE_MIN 0


//! Число периодов калибровки питания.
#define DRIVE_POWER_CALIBRATION_PERIODS 5

//! Число периодов вычисления питания.
#define DRIVE_POWER_CALCULATION_PERIODS 1

//! Необходимые для готовности флаги.
#define DRIVE_READY_FLAGS (DRIVE_FLAG_POWER_DATA_AVAIL)

//! Тип структуры настроек привода.
typedef struct _Drive_Settings {
    fixed32_t U_nom; //!< Номинальное напряжение, В.
    fixed32_t U_nom_delta_allow; //!< Допустимое отклонение номинального напряжения, В.
    fixed32_t U_nom_delta_crit; //!< Критическое отклонение номинального напряжения, В.
    fixed32_t U_zero_noise; //!< Шум напряжения нуля, В.
    fixed32_t I_zero_noise; //!< Шум тока нуля, А.
    fixed32_t U_rot_nom; //!< Номинальное напряжение возбуждения якоря.
    fixed32_t I_rot_nom; //!< Номинальный ток якоря.
    fixed32_t I_exc; //!< Номинальный ток возбуждения.
    uint32_t stop_rot_periods; //!< Время остановки ротора в периодах.
    uint32_t stop_exc_periods; //!< Время остановки возбуждения в периодах.
    uint32_t start_exc_periods; //!< Время остановки возбуждения в периодах.
} drive_settings_t;

//! Структура привода.
typedef struct _Drive {
    drive_flags_t flags; //!< Флаги.
    drive_status_t status; //!< Статус.
    drive_state_t state; //!< Состояние.
    drive_errors_t errors; //!< Ошибки.
    drive_warnings_t warnings; //!< Предупреждения.
    drive_power_errors_t power_errors; //!< Ошибки питания.
    drive_power_calibration_t power_calibration_state; //!< Состояние калибровки питания.
    drive_starting_t starting_state; //!< Состояние запуска привода.
    drive_stopping_t stopping_state; //!< Состояние останова привода.
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
    return drive.state;
}

/**
 * Установка состояния.
 * @param state Состояние.
 */
static void drive_set_state(drive_status_t state)
{
    drive.state = state;
    
    switch(drive.state){
        case DRIVE_STATE_INIT:
        case DRIVE_STATE_CALIBRATION:
            drive.status = DRIVE_STATUS_INIT;
            break;
        case DRIVE_STATE_IDLE:
            drive.status = DRIVE_STATUS_IDLE;
            break;
        case DRIVE_STATE_START:
        case DRIVE_STATE_RUN:
        case DRIVE_STATE_STOP:
            drive.status = DRIVE_STATUS_RUN;
            break;
        case DRIVE_STATE_STOP_ERROR:
        case DRIVE_STATE_ERROR:
            drive.status = DRIVE_STATUS_ERROR;
            break;
    }
}

/**
 * Получение состояния калибровки.
 * @return Состояние калибровки.
 */
ALWAYS_INLINE static drive_power_calibration_t drive_get_calibration_state(void)
{
    return drive.power_calibration_state;
}

/**
 * Установка состояния калибровки.
 * @param state Состояние калибровки.
 */
ALWAYS_INLINE static void drive_set_calibration_state(drive_power_calibration_t state)
{
    drive.power_calibration_state = state;
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
    fixed32_t value = drive_power_channel_real_value_avg(channel);
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
    drive_compare_set_flag(drive_compare_input_voltage(DRIVE_POWER_Ua, drive.settings.U_nom_delta_crit),
                           drive_power_error_occured, DRIVE_POWER_ERROR_UNDERFLOW_Ua, DRIVE_POWER_ERROR_OVERFLOW_Ua);
    // B.
    drive_compare_set_flag(drive_compare_input_voltage(DRIVE_POWER_Ub, drive.settings.U_nom_delta_crit),
                           drive_power_error_occured, DRIVE_POWER_ERROR_UNDERFLOW_Uc, DRIVE_POWER_ERROR_OVERFLOW_Ub);
    // C.
    drive_compare_set_flag(drive_compare_input_voltage(DRIVE_POWER_Uc, drive.settings.U_nom_delta_crit),
                           drive_power_error_occured, DRIVE_POWER_ERROR_UNDERFLOW_Uc, DRIVE_POWER_ERROR_OVERFLOW_Uc);
    // Rot.
    drive_compare_set_flag(drive_compare_zero_voltage(DRIVE_POWER_Urot),
                           drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Urot, DRIVE_POWER_ERROR_IDLE_Urot);
    
    // Напряжения - превышение допустимой разности.
    // A.
    drive_compare_set_flag(drive_compare_input_voltage(DRIVE_POWER_Ua, drive.settings.U_nom_delta_allow),
                           drive_set_warning, DRIVE_WARNING_POWER_UNDERFLOW_Ua, DRIVE_WARNING_POWER_OVERFLOW_Ua);
    // B.
    drive_compare_set_flag(drive_compare_input_voltage(DRIVE_POWER_Ub, drive.settings.U_nom_delta_allow),
                           drive_set_warning, DRIVE_WARNING_POWER_UNDERFLOW_Uc, DRIVE_WARNING_POWER_OVERFLOW_Ub);
    // C.
    drive_compare_set_flag(drive_compare_input_voltage(DRIVE_POWER_Uc, drive.settings.U_nom_delta_allow),
                           drive_set_warning, DRIVE_WARNING_POWER_UNDERFLOW_Uc, DRIVE_WARNING_POWER_OVERFLOW_Uc);
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
    drive_compare_set_flag(drive_compare_zero_current(DRIVE_POWER_Ia),
                           drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Ia, DRIVE_POWER_ERROR_IDLE_Ia);
    // B.
    drive_compare_set_flag(drive_compare_zero_current(DRIVE_POWER_Ib),
                           drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Ic, DRIVE_POWER_ERROR_IDLE_Ib);
    // C.
    drive_compare_set_flag(drive_compare_zero_current(DRIVE_POWER_Ic),
                           drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Ic, DRIVE_POWER_ERROR_IDLE_Ic);
    // Exc.
    drive_compare_set_flag(drive_compare_zero_current(DRIVE_POWER_Iexc),
                           drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Iexc, DRIVE_POWER_ERROR_IDLE_Iexc);
    // Rot.
    drive_compare_set_flag(drive_compare_zero_current(DRIVE_POWER_Irot),
                           drive_power_error_occured, DRIVE_POWER_ERROR_IDLE_Irot, DRIVE_POWER_ERROR_IDLE_Irot);
    // Напряжения фаз.
    
#warning adc calibration - uncomment.
    //drive_check_power_u_in();
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


/*
 * Обработка состояний привода.
 */

/**
 * Обработка данных с АЦП.
 * @param phase Текущая фаза.
 * @return Флаг обработки данных питания.
 */
static bool drive_calculate_power(phase_t phase)
{
    err_t err = E_NO_ERROR;
    if(drive_power_calc_values(DRIVE_POWER_CHANNELS, phase, &err)){

        if(err == E_NO_ERROR && drive_power_data_avail(DRIVE_POWER_CHANNELS)){
            drive_set_flag(DRIVE_FLAG_POWER_DATA_AVAIL);
        }else{
            drive_clear_flag(DRIVE_FLAG_POWER_DATA_AVAIL);
            drive_error_occured(DRIVE_ERROR_POWER_DATA_NOT_AVAIL);
        }
        return true;
    }
    return false;
}

/**
 * Регулировка привода.
 * @param phase Текущая фаза.
 * @return Флаг регулировки привода.
 */
static bool drive_regulate(phase_t phase)
{
    if(drive_flags_is_set(DRIVE_FLAG_POWER_DATA_AVAIL)){
        if(drive_power_phase() == phase){
            fixed32_t U_rot = drive_power_channel_real_value(DRIVE_POWER_Urot);
            fixed32_t I_exc = drive_power_channel_real_value(DRIVE_POWER_Iexc);
            
            drive_regulator_regulate(U_rot, I_exc);
            
            fixed32_t rot_pid_val = drive_regulator_rot_pid_value();
            fixed32_t exc_pid_val = drive_regulator_exc_pid_value();
            
            drive_triacs_set_pairs_open_angle(fixed32_get_int(rot_pid_val));
            drive_triacs_set_exc_open_angle(fixed32_get_int(exc_pid_val));
        }
        drive_triacs_setup_exc(phase);
        drive_triacs_setup_next_pairs(phase);
        
        return true;
    }
    
    return false;
}

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
    
    return E_NO_ERROR;
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
    
    if(drive_calculate_power(phase)){
        if(drive_flags_is_set(DRIVE_FLAG_POWER_DATA_AVAIL)){
            drive_check_power_running();
        }
    }
    
    drive_regulate(phase);
    
    return E_NO_ERROR;
}

/**
 * Обработка состояния останова привода.
 * @param phase Фаза.
 * @return Код ошибки.
 */
static err_t drive_state_process_stop(phase_t phase)
{
    drive_calculate_power(phase);
    
    switch(drive.stopping_state){
        default:
        case DRIVE_STOPPING_NONE:
            return E_NO_ERROR;
        case DRIVE_STOPPING_STOP:
            drive_regulator_stop();
            drive.stopping_state = DRIVE_STOPPING_RAMP;
            break;
        case DRIVE_STOPPING_RAMP:
            if(drive_regulator_state() == DRIVE_REGULATOR_STATE_IDLE){
                drive.stopping_state = DRIVE_STOPPING_WAIT_ROT;
            }
            break;
        case DRIVE_STOPPING_WAIT_ROT:
            drive_triacs_pairs_set_enabled(false);
            drive_regulator_set_rot_enabled(false);
            drive.stopping_state = DRIVE_STOPPING_WAIT_EXC;
            break;
        case DRIVE_STOPPING_WAIT_EXC:
            drive_triacs_exc_set_enabled(false);
            drive_regulator_set_exc_enabled(false);
            drive_set_state(DRIVE_STATE_IDLE);
            drive.stopping_state = DRIVE_STOPPING_DONE;
            break;
        case DRIVE_STOPPING_DONE:
            drive_set_state(DRIVE_STATE_IDLE);
            break;
    }
    
    drive_regulate(phase);
    
    return E_NO_ERROR;
}

/**
 * Обработка состояния запуска привода.
 * @param phase Фаза.
 * @return Код ошибки.
 */
static err_t drive_state_process_start(phase_t phase)
{
    drive_calculate_power(phase);
    
    switch(drive.starting_state){
        default:
        case DRIVE_STARTING_NONE:
            return E_NO_ERROR;
        case DRIVE_STARTING_START:
            drive_triacs_exc_set_enabled(true);
            drive_regulator_set_exc_enabled(true);
            drive_regulator_start();
            drive.starting_state = DRIVE_STARTING_WAIT_EXC;
            break;
        case DRIVE_STARTING_WAIT_EXC:
            drive_triacs_pairs_set_enabled(true);
            drive_regulator_set_rot_enabled(true);
            drive.starting_state = DRIVE_STARTING_RAMP;
            break;
        case DRIVE_STARTING_RAMP:
            if(drive_regulator_state() == DRIVE_REGULATOR_STATE_RUN){
                drive.starting_state = DRIVE_STARTING_DONE;
                drive_set_state(DRIVE_STATE_RUN);
            }
            break;
        case DRIVE_STARTING_DONE:
            drive_set_state(DRIVE_STATE_RUN);
            break;
    }
    
    drive_regulate(phase);
    
    return E_NO_ERROR;
}

/**
 * Обработка состояния простоя(готовности) привода.
 * @param phase Фаза.
 * @return Код ошибки.
 */
static err_t drive_state_process_idle(phase_t phase)
{
    if(drive_calculate_power(phase)){
        if(drive_flags_is_set(DRIVE_FLAG_POWER_DATA_AVAIL)){
            drive_check_power_idle();
        }
    }
    
    return E_NO_ERROR;
}

/**
 * Производит калибровку питания.
 * @param phase Текущая фаза.
 */
static void drive_state_process_power_calibration(phase_t phase)
{
    switch(drive.power_calibration_state){
        default:
        case DRIVE_PWR_CALIBRATION_NONE:
            break;
        case DRIVE_PWR_CALIBRATION_START:
            drive_clear_flag(DRIVE_FLAG_POWER_CALIBRATED);
            drive.power_calibration_state = DRIVE_PWR_CALIBRATION_RUNNING;
            drive_power_set_processing_periods(DRIVE_POWER_CALIBRATION_PERIODS);
            break;
        case DRIVE_PWR_CALIBRATION_RUNNING:
            if(drive_calculate_power(phase)){
                if(drive_flags_is_set(DRIVE_FLAG_POWER_DATA_AVAIL)){
                    drive_power_calibrate(DRIVE_POWER_CHANNELS);
                    drive_power_set_processing_periods(DRIVE_POWER_CALCULATION_PERIODS);
                    drive_set_flag(DRIVE_FLAG_POWER_CALIBRATED);
                    drive.power_calibration_state = DRIVE_PWR_CALIBRATION_DONE;
                }
            }
            break;
        case DRIVE_PWR_CALIBRATION_DONE:
            drive_set_state(DRIVE_STATE_IDLE);
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
    if(drive_power_phase() == PHASE_UNK){
        drive_power_set_phase(phase);
        drive_power_reset_channels(DRIVE_POWER_CHANNELS);
        drive_power_set_processing_periods(DRIVE_POWER_CALCULATION_PERIODS);
    }else if(drive_calculate_power(phase)){
        if(drive_flags_is_set(DRIVE_FLAG_POWER_DATA_AVAIL)){
            drive_set_calibration_state(DRIVE_PWR_CALIBRATION_START);
            drive_set_state(DRIVE_STATE_CALIBRATION);
        }
    }
    return E_NO_ERROR;
}

/**
 * Обработка состояний привода.
 * @param phase Фаза.
 * @return Код ошибку.
 */
static err_t drive_states_process(phase_t phase)
{
    if(phase == PHASE_UNK){
        return E_INVALID_VALUE;
    }
    
    // Обработаем текущую фазу.
    phase_state_handle(phase);
    
    // Если ошибка фазы.
    if(phase_state_error() != PHASE_NO_ERROR){
        // Обработаем её.
        drive_error_occured(DRIVE_ERROR_PHASE);
    }
    
    switch(drive.state){
        case DRIVE_STATE_INIT:
            drive_state_process_init(phase);
            break;
        case DRIVE_STATE_CALIBRATION:
            drive_state_process_power_calibration(phase);
            break;
        case DRIVE_STATE_IDLE:
            drive_state_process_idle(phase);
            break;
        case DRIVE_STATE_START:
            drive_state_process_start(phase);
            break;
        case DRIVE_STATE_RUN:
            drive_state_process_running(phase);
            break;
        case DRIVE_STATE_STOP:
            drive_state_process_stop(phase);
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
    
    drive_regulator_init();
    
    drive_regulator_rot_pid_clamp(DRIVE_ROT_PID_VALUE_MIN, DRIVE_ROT_PID_VALUE_MAX);
    drive_regulator_exc_pid_clamp(DRIVE_EXC_PID_VALUE_MIN, DRIVE_EXC_PID_VALUE_MAX);
    
    drive_update_settings();
    
    drive.flags = DRIVE_FLAG_NONE;
    drive.status = DRIVE_STATUS_INIT;
    drive.state = DRIVE_STATE_INIT;
    drive.errors = DRIVE_ERROR_NONE;
    drive.starting_state = DRIVE_STARTING_NONE;
    drive.stopping_state = DRIVE_STOPPING_NONE;
    
    drive_triacs_init();
    
    drive_power_init();
    
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
    drive_triacs_set_exc_phase(settings_valueu(PARAM_ID_EXC_PHASE));
    drive.settings.stop_rot_periods = settings_valueu(PARAM_ID_ROT_STOP_TIME) * DRIVE_POWER_FREQ;
    drive.settings.stop_exc_periods = settings_valueu(PARAM_ID_EXC_STOP_TIME) * DRIVE_POWER_FREQ;
    drive.settings.start_exc_periods = settings_valueu(PARAM_ID_EXC_START_TIME) * DRIVE_POWER_FREQ;
    drive_regulator_set_ramp_time(settings_valuei(PARAM_ID_RAMP_TIME));
    drive_regulator_set_rot_pid(settings_valuef(PARAM_ID_ROT_PID_K_P),
                                settings_valuef(PARAM_ID_ROT_PID_K_I),
                                settings_valuef(PARAM_ID_ROT_PID_K_D));
    drive_regulator_set_exc_pid(settings_valuef(PARAM_ID_EXC_PID_K_P),
                                settings_valuef(PARAM_ID_EXC_PID_K_I),
                                settings_valuef(PARAM_ID_EXC_PID_K_D));

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
    return drive.power_calibration_state;
}

drive_starting_t drive_starting(void)
{
    return drive.starting_state;
}

drive_stopping_t drive_stopping(void)
{
    return drive.stopping_state;
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
        drive.starting_state = DRIVE_STARTING_START;
        drive_set_state(DRIVE_STATE_START);
    }
    return true;
}

bool drive_stop(void)
{
    if(drive.status == DRIVE_STATUS_RUN){
        drive.stopping_state = DRIVE_STOPPING_STOP;
        drive_set_state(DRIVE_STATE_STOP);
    }
    return true;
}

bool drive_running(void)
{
    return drive.status == DRIVE_STATUS_RUN;
}

void drive_triac_pairs_timer0_irq_handler(void)
{
    drive_triacs_timer0_irq_handler();
}

void drive_triac_pairs_timer1_irq_handler(void)
{
    drive_triacs_timer0_irq_handler();
}

void drive_triac_exc_timer_irq_handler(void)
{
    drive_triacs_exc_timer_irq_handler();
}

err_t drive_process_null_sensor(phase_t phase)
{
    return drive_states_process(phase);
}
