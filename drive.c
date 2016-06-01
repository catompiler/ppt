#include "drive.h"
#include "settings.h"
#include "ramp.h"
#include "pid_controller/pid_controller.h"
#include "drive_regulator.h"
#include "drive_protection.h"
#include <string.h>
#include <stdio.h>


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
#define DRIVE_POWER_CALCULATION_PERIODS 0

//! Необходимые для готовности флаги.
#define DRIVE_READY_FLAGS (DRIVE_FLAG_POWER_DATA_AVAIL)

//! Тип структуры настроек привода.
typedef struct _Drive_Settings {
    uint32_t stop_rot_periods; //!< Время остановки ротора в периодах.
    uint32_t stop_exc_periods; //!< Время остановки возбуждения в периодах.
    uint32_t start_exc_periods; //!< Время остановки возбуждения в периодах.
} drive_settings_t;

//! Тип структуры обновляемых параметров.
typedef struct _Drive_Parameters {
    param_t* param_u_a;
    param_t* param_u_b;
    param_t* param_u_c;
    param_t* param_u_rot;
    param_t* param_i_exc;
} drive_parameters_t;

//! Структура привода.
typedef struct _Drive {
    drive_flags_t flags; //!< Флаги.
    drive_status_t status; //!< Статус.
    drive_state_t state; //!< Состояние.
    drive_errors_t errors; //!< Ошибки.
    drive_warnings_t warnings; //!< Предупреждения.
    drive_power_errors_t power_errors; //!< Ошибки питания.
    drive_power_warnings_t power_warnings; //!< Предупреждения питания.
    drive_power_calibration_t power_calibration_state; //!< Состояние калибровки питания.
    drive_starting_t starting_state; //!< Состояние запуска привода.
    drive_stopping_t stopping_state; //!< Состояние останова привода.
    drive_err_stopping_t err_stopping_state; //!< Состояние останова привода при ошибке.
    drive_settings_t settings; //!< Настройки.
    drive_parameters_t params; //!< Обновляемые параметры.
    uint32_t start_stop_counter; //!< Счётчик периодов при запуске / останове.
    drive_error_callback_t on_error_occured; //!< Каллбэк при ошибке.
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
static void drive_set_state(drive_state_t state)
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

/**
 * Установка предупреждения питания.
 * @param error Предупреждение.
 */
ALWAYS_INLINE static void drive_set_power_warning(drive_power_warnings_t warning)
{
    drive.power_warnings |= warning;
}

/**
 * Снятие предупреждения питания.
 * @param warning Предупреждение.
 */
ALWAYS_INLINE static void drive_clear_power_warning(drive_power_warnings_t warning)
{
    drive.power_warnings &= ~warning;
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

    if(drive.status != DRIVE_STATUS_ERROR){
        if(drive.on_error_occured) drive.on_error_occured();
    }
    
    switch(drive_get_state()){
        case DRIVE_STATE_INIT:
        case DRIVE_STATE_CALIBRATION:
        case DRIVE_STATE_IDLE:
            drive_set_state(DRIVE_STATE_ERROR);
            break;
        case DRIVE_STATE_START:
        case DRIVE_STATE_RUN:
        case DRIVE_STATE_STOP:
            drive.err_stopping_state = DRIVE_ERR_STOPPING_STOP;
            drive_set_state(DRIVE_STATE_STOP_ERROR);
            drive_triacs_stop();
            break;
        case DRIVE_STATE_STOP_ERROR:
        case DRIVE_STATE_ERROR:
            break;
        default:
            break;
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

/**
 * Обработчик возникновения предупреждения питания.
 * @param warning Предупреждение питания.
 */
static void drive_power_warning_occured(drive_power_warnings_t warning)
{
    drive_set_power_warning(warning);
    drive_set_warning(DRIVE_WARNING_POWER);
}


/*
 * Общие функции обработки питания.
 */

static void drive_handle_power_check(drive_pwr_check_res_t res, uint32_t under_warning, uint32_t over_warning,
                                                                   uint32_t under_error,   uint32_t over_error)
{
    switch(res){
        default:
        case DRIVE_PWR_CHECK_NORMAL:
            break;
        case DRIVE_PWR_CHECK_ALLOW_UNDERFLOW:
            drive_power_warning_occured(under_warning);
            break;
        case DRIVE_PWR_CHECK_ALLOW_OVERFLOW:
            drive_power_warning_occured(over_warning);
            break;
        case DRIVE_PWR_CHECK_CRIT_UNDERFLOW:
            drive_power_error_occured(under_error);
            break;
        case DRIVE_PWR_CHECK_CRIT_OVERFLOW:
            drive_power_error_occured(over_error);
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
    drive_handle_power_check(
            drive_protection_check_input_voltage(drive_power_channel_real_value(DRIVE_POWER_Ua)),
            DRIVE_POWER_WARNING_UNDERFLOW_Ua, DRIVE_POWER_WARNING_OVERFLOW_Ua,
            DRIVE_POWER_ERROR_UNDERFLOW_Ua, DRIVE_POWER_ERROR_OVERFLOW_Ua
            );
    // B.
    drive_handle_power_check(
            drive_protection_check_input_voltage(drive_power_channel_real_value(DRIVE_POWER_Ub)),
            DRIVE_POWER_WARNING_UNDERFLOW_Ub, DRIVE_POWER_WARNING_OVERFLOW_Ub,
            DRIVE_POWER_ERROR_UNDERFLOW_Ub, DRIVE_POWER_ERROR_OVERFLOW_Ub
            );
    // C.
    drive_handle_power_check(
            drive_protection_check_input_voltage(drive_power_channel_real_value(DRIVE_POWER_Uc)),
            DRIVE_POWER_WARNING_UNDERFLOW_Uc, DRIVE_POWER_WARNING_OVERFLOW_Uc,
            DRIVE_POWER_ERROR_UNDERFLOW_Uc, DRIVE_POWER_ERROR_OVERFLOW_Uc
            );
}

/*
 * Состояние простоя (готовность).
 */

/**
 * Проверяет значение входов питания в состоянии простоя (готовности).
 */
static void drive_check_power_idle(void)
{
    // Напряжения.
    // Фазы.
    drive_check_power_u_in();
    // Напряжения - отклонения от нуля.
    // Rot.
    drive_handle_power_check(
            drive_protection_check_zero_voltage(drive_power_channel_real_value(DRIVE_POWER_Urot)), 0, 0,
            DRIVE_POWER_ERROR_IDLE_Urot, DRIVE_POWER_ERROR_IDLE_Urot
            );
    // Токи - отклонения от нуля.
    // Exc.
    drive_handle_power_check(
            drive_protection_check_zero_current(drive_power_channel_real_value(DRIVE_POWER_Iexc)), 0, 0,
            DRIVE_POWER_ERROR_IDLE_Iexc, DRIVE_POWER_ERROR_IDLE_Iexc
            );
    // Rot.
    drive_handle_power_check(
            drive_protection_check_rot_zero_current(drive_power_channel_real_value(DRIVE_POWER_Irot)), 0, 0,
            DRIVE_POWER_ERROR_IDLE_Irot, DRIVE_POWER_ERROR_IDLE_Irot
            );
}

/**
 * Проверяет значение входов питания в состоянии работы.
 */
static void drive_check_power_running(void)
{
    // Напряжения.
    // Фазы.
    drive_check_power_u_in();
    // Rot.
    drive_handle_power_check(
            drive_protection_check_rot_voltage(drive_power_channel_real_value(DRIVE_POWER_Urot)),
            DRIVE_POWER_WARNING_UNDERFLOW_Urot, DRIVE_POWER_WARNING_OVERFLOW_Urot,
            DRIVE_POWER_ERROR_UNDERFLOW_Urot, DRIVE_POWER_ERROR_OVERFLOW_Urot
            );
    // Токи.
    // Rot.
    drive_handle_power_check(
            drive_protection_check_rot_current(drive_power_channel_real_value(DRIVE_POWER_Irot)),
            DRIVE_POWER_WARNING_UNDERFLOW_Irot, DRIVE_POWER_WARNING_OVERFLOW_Irot,
            DRIVE_POWER_ERROR_UNDERFLOW_Irot, DRIVE_POWER_ERROR_OVERFLOW_Irot
            );
    // Exc.
    drive_handle_power_check(
            drive_protection_check_exc_current(drive_power_channel_real_value(DRIVE_POWER_Iexc)),
            DRIVE_POWER_WARNING_UNDERFLOW_Iexc, DRIVE_POWER_WARNING_OVERFLOW_Iexc,
            DRIVE_POWER_ERROR_UNDERFLOW_Iexc, DRIVE_POWER_ERROR_OVERFLOW_Iexc
            );
}


/*
 * Обработка состояний привода.
 */

//! Макрос для обновления параметра.
#define DRIVE_UPDATE_POWER_PARAM(PARAM, CHANNEL)\
    do {\
        if(PARAM) settings_param_set_valuef(PARAM, drive_power_channel_real_value(CHANNEL));\
    }while(0)
/**
 * Обновляет значения питания в параметрах.
 */
static void drive_update_power_parameters(void)
{
    if(!drive_flags_is_set(DRIVE_FLAG_POWER_DATA_AVAIL)) return;
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_u_a, DRIVE_POWER_Ua);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_u_b, DRIVE_POWER_Ub);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_u_c, DRIVE_POWER_Uc);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_u_rot, DRIVE_POWER_Urot);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_i_exc, DRIVE_POWER_Iexc);
}

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
 * @return Флаг регулировки привода.
 */
static bool drive_regulate(void)
{
    if(drive_flags_is_set(DRIVE_FLAG_POWER_DATA_AVAIL)){
            fixed32_t U_rot = drive_power_channel_real_value(DRIVE_POWER_Urot);
            fixed32_t I_exc = drive_power_channel_real_value(DRIVE_POWER_Iexc);
            
            drive_regulator_regulate(U_rot, I_exc);
            
            fixed32_t rot_pid_val = drive_regulator_rot_pid_value();
            fixed32_t exc_pid_val = drive_regulator_exc_pid_value();
            
            drive_triacs_set_pairs_open_angle(rot_pid_val);
            drive_triacs_set_exc_open_angle(exc_pid_val);
            //drive_triacs_set_exc_open_angle(fixed32_make_from_int(120));
            
            //pid_controller_t* pid = drive_regulator_rot_pid();
            
            //printf("PID: %d - %d = %d\r\n", (int)pid->prev_i, (int)pid->prev_e, (int)pid->value);
            
            //settings_param_set_valuef(settings_param_by_id(PARAM_ID_DEBUG_0), rot_pid_val);
            //settings_param_set_valuef(settings_param_by_id(PARAM_ID_DEBUG_0), pid->prev_i);
        
        return true;
    }
    
    return false;
}

/**
 * Запускает открытие тиристоров.
 */
static void drive_setup_triacs_open(phase_t phase)
{
    drive_triacs_setup_exc(phase);
    drive_triacs_setup_next_pairs(phase);
}

/**
 * Обработка состояния останова привода при ошибке.
 * @param phase Фаза.
 * @return Код ошибки.
 */
static err_t drive_state_process_stop_error(phase_t phase)
{
    switch(drive.err_stopping_state){
        default:
        case DRIVE_ERR_STOPPING_NONE:
            return E_NO_ERROR;
        case DRIVE_ERR_STOPPING_STOP:
            drive_triacs_set_pairs_enabled(false);
            drive_regulator_set_rot_enabled(false);
            
            drive_setup_triacs_open(phase);
            
            if(drive_calculate_power(phase)){
                drive_regulate();
            }
            
            drive.err_stopping_state = DRIVE_ERR_STOPPING_WAIT_ROT;
            break;
        case DRIVE_ERR_STOPPING_WAIT_ROT:
            drive_regulator_stop();
            drive_triacs_set_exc_enabled(false);
            drive_regulator_set_exc_enabled(false);
        case DRIVE_ERR_STOPPING_WAIT_EXC:
            drive.err_stopping_state = DRIVE_ERR_STOPPING_DONE;
        case DRIVE_ERR_STOPPING_DONE:
            drive_calculate_power(phase);
            drive_set_state(DRIVE_STATE_ERROR);
            break;
    }
    
    return E_NO_ERROR;
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
    if(drive_phase_state_direction() == DRIVE_DIR_UNK) return E_INVALID_VALUE;
    
    drive_calculate_power(phase);
    
    return E_NO_ERROR;
}

/**
 * Обработка состояния работы привода.
 * @param phase Фаза.
 * @return Код ошибки.
 */
static err_t drive_state_process_running(phase_t phase)
{
    // Нужно какое-либо направление.
    if(drive_phase_state_direction() == DRIVE_DIR_UNK) return E_INVALID_VALUE;
    
    drive_setup_triacs_open(phase);
    
    if(drive_calculate_power(phase)){
        if(drive_flags_is_set(DRIVE_FLAG_POWER_DATA_AVAIL)){
            drive_check_power_running();
        }
        drive_regulate();
    }
    
    return E_NO_ERROR;
}

/**
 * Обработка состояния останова привода.
 * @param phase Фаза.
 * @return Код ошибки.
 */
static err_t drive_state_process_stop(phase_t phase)
{
    drive_pwr_check_res_t check_res = DRIVE_PWR_CHECK_NORMAL;
    
    drive_setup_triacs_open(phase);
    
    if(drive_calculate_power(phase)){
        drive_regulate();
    }
    
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
                drive_triacs_set_pairs_enabled(false);
                drive_regulator_set_rot_enabled(false);
                drive.stopping_state = DRIVE_STOPPING_WAIT_ROT;
                drive.start_stop_counter = 0;
            }
            break;
        case DRIVE_STOPPING_WAIT_ROT:
            if(++ drive.start_stop_counter >= drive.settings.stop_rot_periods){
                drive_triacs_set_exc_enabled(false);
                drive_regulator_set_exc_enabled(false);
                drive.stopping_state = DRIVE_STOPPING_WAIT_EXC;
                drive.start_stop_counter = 0;
            }
            break;
        case DRIVE_STOPPING_WAIT_EXC:
            check_res = drive_protection_check_zero_current(drive_power_channel_real_value(DRIVE_POWER_Iexc));

            if(drive_protection_is_allow(check_res)){
                drive_set_state(DRIVE_STATE_IDLE);
                drive.stopping_state = DRIVE_STOPPING_DONE;
                drive.start_stop_counter = 0;
            }else if(++ drive.start_stop_counter >= drive.settings.stop_exc_periods){
                drive_handle_power_check(check_res, 0, 0,
                        DRIVE_POWER_ERROR_IDLE_Iexc, DRIVE_POWER_ERROR_IDLE_Iexc
                        );
            }
            break;
        case DRIVE_STOPPING_DONE:
            drive_set_state(DRIVE_STATE_IDLE);
            break;
    }
    
    return E_NO_ERROR;
}

/**
 * Обработка состояния запуска привода.
 * @param phase Фаза.
 * @return Код ошибки.
 */
static err_t drive_state_process_start(phase_t phase)
{
    drive_pwr_check_res_t check_res = DRIVE_PWR_CHECK_NORMAL;
    
    drive_setup_triacs_open(phase);
    
    if(drive_calculate_power(phase)){
        drive_regulate();
    }
    
    switch(drive.starting_state){
        default:
        case DRIVE_STARTING_NONE:
            return E_NO_ERROR;
        case DRIVE_STARTING_START:
            drive_triacs_set_exc_enabled(true);
            if(drive_triacs_exc_mode() == DRIVE_TRIACS_EXC_REGULATED){
                drive_regulator_set_exc_enabled(true);
            }
            drive_regulator_start();
            drive.starting_state = DRIVE_STARTING_WAIT_EXC;
            drive.start_stop_counter = 0;
            break;
        case DRIVE_STARTING_WAIT_EXC:
            check_res = drive_protection_check_exc_current(drive_power_channel_real_value(DRIVE_POWER_Iexc));
            
            if(drive_protection_is_allow(check_res)){
                drive_triacs_set_pairs_enabled(true);
                drive_regulator_set_rot_enabled(true);
                drive.starting_state = DRIVE_STARTING_RAMP;
                drive.start_stop_counter = 0;
            }else if(++ drive.start_stop_counter >= drive.settings.start_exc_periods){
                drive_handle_power_check(check_res,
                    DRIVE_POWER_WARNING_UNDERFLOW_Iexc, DRIVE_POWER_WARNING_OVERFLOW_Iexc,
                    DRIVE_POWER_ERROR_UNDERFLOW_Iexc, DRIVE_POWER_ERROR_OVERFLOW_Iexc
                    );
            }
            
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
                    drive_set_state(DRIVE_STATE_IDLE);
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
    drive_phase_state_handle(phase);
    
    // Если ошибка фазы.
    if(drive_phase_state_errors() != PHASE_NO_ERROR){
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
            drive_state_process_stop_error(phase);
            break;
        case DRIVE_STATE_ERROR:
            drive_state_process_error(phase);
            break;
    }
    
    drive_update_power_parameters();
    
    return E_NO_ERROR;
}


/*
 * Интерфейсный функции.
 * Документация в drive.h
 */

err_t drive_init(void)
{
    memset(&drive, 0x0, sizeof(drive_t));
    
    drive_phase_state_init();
    
    drive_regulator_init();
    
    drive_triacs_init();
    
    drive_power_init();
    
    drive_protection_init();
    
    drive_regulator_rot_pid_clamp(DRIVE_ROT_PID_VALUE_MIN, DRIVE_ROT_PID_VALUE_MAX);
    drive_regulator_exc_pid_clamp(DRIVE_EXC_PID_VALUE_MIN, DRIVE_EXC_PID_VALUE_MAX);
    
    drive_update_settings();
    
    drive.flags = DRIVE_FLAG_NONE;
    drive.status = DRIVE_STATUS_INIT;
    drive.state = DRIVE_STATE_INIT;
    drive.errors = DRIVE_ERROR_NONE;
    drive.starting_state = DRIVE_STARTING_NONE;
    drive.stopping_state = DRIVE_STOPPING_NONE;
    drive.err_stopping_state = DRIVE_ERR_STOPPING_NONE;
    
    drive.on_error_occured = NULL;
    
    drive.params.param_u_a = settings_param_by_id(PARAM_ID_POWER_U_A);
    drive.params.param_u_b = settings_param_by_id(PARAM_ID_POWER_U_B);
    drive.params.param_u_c = settings_param_by_id(PARAM_ID_POWER_U_C);
    drive.params.param_u_rot = settings_param_by_id(PARAM_ID_POWER_U_ROT);
    drive.params.param_i_exc = settings_param_by_id(PARAM_ID_POWER_I_EXC);
    
    return E_NO_ERROR;
}

err_t drive_update_settings(void)
{
    drive_protection_set_input_voltage(settings_valuef(PARAM_ID_U_NOM),
                                       settings_valueu(PARAM_ID_U_NOM_ALLOW_VAR),
                                       settings_valueu(PARAM_ID_U_NOM_CRIT_VAR));
    drive_protection_set_zero_voltage_noise(settings_valuef(PARAM_ID_U_ZERO_NOISE));
    drive_protection_set_zero_current_noise(settings_valuef(PARAM_ID_I_ZERO_NOISE));
    drive_protection_set_rot_zero_current_noise(settings_valuef(PARAM_ID_I_ROT_ZERO_NOISE));
    drive_protection_set_rot_voltage(settings_valuef(PARAM_ID_U_ROT_NOM),
                                     settings_valueu(PARAM_ID_U_ROT_ALLOW_VAR),
                                     settings_valueu(PARAM_ID_U_ROT_CRIT_VAR));
    drive_protection_set_rot_current(settings_valuef(PARAM_ID_I_ROT_NOM),
                                     settings_valueu(PARAM_ID_I_ROT_ALLOW_VAR),
                                     settings_valueu(PARAM_ID_I_ROT_CRIT_VAR));
    drive_protection_set_exc_current(settings_valuef(PARAM_ID_I_EXC),
                                     settings_valueu(PARAM_ID_I_EXC_ALLOW_VAR),
                                     settings_valueu(PARAM_ID_I_EXC_CRIT_VAR));

    drive.settings.stop_rot_periods = settings_valueu(PARAM_ID_ROT_STOP_TIME) * DRIVE_POWER_NULL_SENSORS_FREQ;
    drive.settings.stop_exc_periods = settings_valueu(PARAM_ID_EXC_STOP_TIME) * DRIVE_POWER_NULL_SENSORS_FREQ;
    drive.settings.start_exc_periods = settings_valueu(PARAM_ID_EXC_START_TIME) * DRIVE_POWER_NULL_SENSORS_FREQ;
    drive_triacs_set_exc_mode(settings_valueu(PARAM_ID_EXC_MODE));
    drive_triacs_set_pairs_open_time_us(settings_valueu(PARAM_ID_TRIACS_PAIRS_OPEN_TIME));
    drive_triacs_set_exc_open_time_us(settings_valueu(PARAM_ID_TRIAC_EXC_OPEN_TIME));
    drive_triacs_set_exc_phase(settings_valueu(PARAM_ID_EXC_PHASE));
    drive_regulator_set_ramp_time(settings_valuei(PARAM_ID_RAMP_TIME));
    drive_regulator_set_rot_nom_voltage(settings_valuef(PARAM_ID_U_ROT_NOM));
    drive_regulator_set_exc_current(settings_valuef(PARAM_ID_I_EXC));
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

bool drive_power_warning(drive_power_warning_t warning)
{
    return (drive.power_warnings & warning) != 0;
}

drive_power_warnings_t drive_power_warnings(void)
{
    return drive.power_warnings;
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

drive_err_stopping_t drive_err_stopping(void)
{
    return drive.err_stopping_state;
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

drive_error_callback_t drive_error_callback(void)
{
    return drive.on_error_occured;
}

void drive_set_error_callback(drive_error_callback_t callback)
{
    drive.on_error_occured = callback;
}

void drive_triac_pairs_timer0_irq_handler(void)
{
    drive_triacs_timer0_irq_handler();
}

void drive_triac_pairs_timer1_irq_handler(void)
{
    drive_triacs_timer1_irq_handler();
}

void drive_triac_exc_timer_irq_handler(void)
{
    drive_triacs_exc_timer_irq_handler();
}

err_t drive_process_null_sensor(phase_t phase)
{
    return drive_states_process(phase);
}
