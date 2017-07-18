#include "drive.h"
#include "settings.h"
#include "ramp.h"
#include "pid_controller/pid_controller.h"
#include "drive_regulator.h"
#include "drive_protection.h"
#include "drive_phase_sync.h"
#include "drive_tasks.h"
#include "drive_motor.h"
#include <string.h>
#include <stdio.h>


//! Минимальное значение PID-регулятора скорости.
#define DRIVE_SPD_PID_VALUE_MIN 0

//! Максимальное значение PID-регулятора напряжения якоря.
#define DRIVE_ROT_PID_VALUE_MAX TRIACS_PAIRS_ANGLE_MAX_F // 120.0
//! Минимальное значение PID-регулятора напряжения якоря.
#define DRIVE_ROT_PID_VALUE_MIN TRIACS_PAIRS_ANGLE_MIN_F

//! Максимальное значение PID-регулятора тока возбуждения.
//#define DRIVE_EXC_PID_VALUE_MAX (TRIAC_EXC_ANGLE_MAX_F - TRIAC_EXC_ANGLE_MIN_F) // 180.0 - 30.0
#define DRIVE_EXC_PID_VALUE_MAX TRIAC_EXC_ANGLE_MAX_F
//! Минимальное значение PID-регулятора тока возбуждения.
//#define DRIVE_EXC_PID_VALUE_MIN 0
#define DRIVE_EXC_PID_VALUE_MIN TRIAC_EXC_ANGLE_MIN_F
//! Добавочный угол открытия тиристора возбуждения.
//#define DRIVE_EXC_PID_VALUE_DELTA TRIAC_EXC_ANGLE_MIN_F // 30.0

//! Коэффициент П-звена ПИД-регулятора синхронизации с фазами.
//#define DRIVE_PHASE_SYNC_PLL_PIC_Kp (fixed32_make_from_fract(18, 1))
//! Коэффициент И-звена ПИД-регулятора синхронизации с фазами.
//#define DRIVE_PHASE_SYNC_PLL_PIC_Ki (fixed32_make_from_fract(15, 10))

//! Точность синхронизации с фазами.
//#define DRIVE_PHASE_SYNC_ACCURACY (fixed32_make_from_fract(3, 1))

//#define DRIVE_PHASE_SYNC_DEBUG

//! Число периодов калибровки питания.
#define DRIVE_POWER_CALIBRATION_ITERS 15

//! Число периодов вычисления питания.
#define DRIVE_POWER_CALCULATION_ITERS 0

//! Необходимые для готовности флаги.
#define DRIVE_READY_FLAGS (DRIVE_FLAG_POWER_DATA_AVAIL)

//! dt Тепловой защиты.
#define DRIVE_TOP_DT 0x1b5 //0.006667 с

//! dt Цифровых входов, мс.
#define DRIVE_DIO_DT 0x6AAC0 //6.667 мс.


//! Тип структуры настроек привода.
typedef struct _Drive_Settings {
    drive_stop_mode_t stop_mode; //!< Режим нормального останова привода.
    uint16_t stop_rot_iters; //!< Время остановки ротора в итерациях.
    uint16_t stop_exc_iters; //!< Время остановки возбуждения в итерациях.
    uint16_t start_exc_iters; //!< Время остановки возбуждения в итерациях.
    uint16_t check_phases_iters; //!< Время проверки состояния фаз сети.
    uint16_t zero_sensor_time; //!< Время срабатывания датчика нуля.
} drive_settings_t;

//! Тип структуры обновляемых параметров.
typedef struct _Drive_Parameters {
    param_t* param_u_a;
    param_t* param_u_b;
    param_t* param_u_c;
    param_t* param_u_rot;
    param_t* param_i_a;
    param_t* param_i_b;
    param_t* param_i_c;
    param_t* param_i_rot;
    param_t* param_i_exc;
    param_t* param_i_ref;
    param_t* param_i_fan;
    param_t* param_calc_u_rot;
    param_t* param_pairs_angle;
    param_t* param_exc_angle;
    // цифровые входа
    param_t* param_digital_in_1;
    param_t* param_digital_in_2;
    param_t* param_digital_in_3;
    param_t* param_digital_in_4;
    param_t* param_digital_in_5;
    // цифровые выхода
    param_t* param_digital_out_1;
    param_t* param_digital_out_2;
    param_t* param_digital_out_3;
    param_t* param_digital_out_4;
    // ПИДы.
    param_t* param_pid_rot_spd_val;
    param_t* param_pid_rot_cur_val;
    param_t* param_pid_exc_cur_val;
} drive_parameters_t;

//! Структура привода.
typedef struct _Drive {
    TIM_TypeDef* tim_null; //!< Таймер искусственных датчиков нуля.
    drive_init_state_t init_state; //!< Состояние инициализации.
    drive_flags_t flags; //!< Флаги.
    drive_status_t status; //!< Статус.
    drive_state_t state; //!< Состояние.
    drive_state_t saved_state; //!< Сохранённое состояние.
    drive_errors_t errors; //!< Ошибки.
    drive_warnings_t warnings; //!< Предупреждения.
    drive_power_errors_t power_errors; //!< Ошибки питания.
    drive_power_warnings_t power_warnings; //!< Предупреждения питания.
    drive_power_errors_t power_cutoff_errors; //!< Ошибки отсечки питания.
    //drive_phase_angle_errors_t phase_angle_errors; //!< Ошибки углов между фазами.
    //drive_phase_angle_warnings_t phase_angle_warnings; //!< Предупреждения углов между фазами.
    drive_power_calibration_t power_calibration_state; //!< Состояние калибровки питания.
    drive_starting_t starting_state; //!< Состояние запуска привода.
    drive_stopping_t stopping_state; //!< Состояние останова привода.
    drive_stop_mode_t stopping_cur_mode; //!< Режим текущего останова привода.
    drive_state_t stopping_next_state; //!< Состояние привода после останова.
    drive_err_stopping_t err_stopping_state; //!< Состояние останова привода при ошибке.
    drive_settings_t settings; //!< Настройки.
    drive_parameters_t params; //!< Обновляемые параметры.
    uint16_t iters_counter; //!< Счётчик итераций при ожидании таймаутов.
    drive_error_callback_t on_error_occured; //!< Каллбэк при ошибке.
    drive_warning_callback_t on_warning_occured; //!< Каллбэк при предупреждении.
} drive_t;

//! Состояние привода.
static drive_t drive;


#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))


//! Структура данных защиты в каждом состоянии.
typedef struct _Drive_Prot_Data {
    drive_power_errors_t errors_mask; //!< Маска ошибок.
    drive_power_errors_t warnings_mask; //!< Маска предупреждений.
} drive_prot_data_t;

//! Маска ошибок защиты по входным напряжениям и токам (только превышение).
#define PROT_ERRORS_MASK_IN_OVF (\
    DRIVE_POWER_ERROR_OVERFLOW_Ua |\
    DRIVE_POWER_ERROR_OVERFLOW_Ub |\
    DRIVE_POWER_ERROR_OVERFLOW_Uc |\
    DRIVE_POWER_ERROR_OVERFLOW_Ia |\
    DRIVE_POWER_ERROR_OVERFLOW_Ib |\
    DRIVE_POWER_ERROR_OVERFLOW_Ic  \
)

//! Маска предупреждений защиты по входным напряжениям и токам (только превышение).
#define PROT_WARNINGS_MASK_IN_OVF (\
    DRIVE_POWER_WARNING_OVERFLOW_Ua |\
    DRIVE_POWER_WARNING_OVERFLOW_Ub |\
    DRIVE_POWER_WARNING_OVERFLOW_Uc |\
    DRIVE_POWER_WARNING_OVERFLOW_Ia |\
    DRIVE_POWER_WARNING_OVERFLOW_Ib |\
    DRIVE_POWER_WARNING_OVERFLOW_Ic  \
)

//! Маска ошибок защиты по входным напряжениям и токам.
#define PROT_ERRORS_MASK_IN (\
    DRIVE_POWER_ERROR_UNDERFLOW_Ua | DRIVE_POWER_ERROR_OVERFLOW_Ua |\
    DRIVE_POWER_ERROR_UNDERFLOW_Ub | DRIVE_POWER_ERROR_OVERFLOW_Ub |\
    DRIVE_POWER_ERROR_UNDERFLOW_Uc | DRIVE_POWER_ERROR_OVERFLOW_Uc |\
    DRIVE_POWER_ERROR_UNDERFLOW_Ia | DRIVE_POWER_ERROR_OVERFLOW_Ia |\
    DRIVE_POWER_ERROR_UNDERFLOW_Ib | DRIVE_POWER_ERROR_OVERFLOW_Ib |\
    DRIVE_POWER_ERROR_UNDERFLOW_Ic | DRIVE_POWER_ERROR_OVERFLOW_Ic  \
)

//! Маска предупреждений защиты по входным напряжениям и токам.
#define PROT_WARNINGS_MASK_IN (\
    DRIVE_POWER_WARNING_UNDERFLOW_Ua | DRIVE_POWER_WARNING_OVERFLOW_Ua |\
    DRIVE_POWER_WARNING_UNDERFLOW_Ub | DRIVE_POWER_WARNING_OVERFLOW_Ub |\
    DRIVE_POWER_WARNING_UNDERFLOW_Uc | DRIVE_POWER_WARNING_OVERFLOW_Uc |\
    DRIVE_POWER_WARNING_UNDERFLOW_Ia | DRIVE_POWER_WARNING_OVERFLOW_Ia |\
    DRIVE_POWER_WARNING_UNDERFLOW_Ib | DRIVE_POWER_WARNING_OVERFLOW_Ib |\
    DRIVE_POWER_WARNING_UNDERFLOW_Ic | DRIVE_POWER_WARNING_OVERFLOW_Ic  \
)

//! Маска ошибок защиты по значениям нуля в простое.
#define PROT_ERRORS_MASK_IDLE (\
    DRIVE_POWER_ERROR_IDLE_Ia | DRIVE_POWER_ERROR_IDLE_Urot |\
    DRIVE_POWER_ERROR_IDLE_Ib | DRIVE_POWER_ERROR_IDLE_Irot |\
    DRIVE_POWER_ERROR_IDLE_Ic | DRIVE_POWER_ERROR_IDLE_Iexc  \
)

//! Маска предупреждений защиты по значениям нуля в простое.
#define PROT_WARNINGS_MASK_IDLE (\
    DRIVE_POWER_WARNING_IDLE_Ia | DRIVE_POWER_WARNING_IDLE_Urot |\
    DRIVE_POWER_WARNING_IDLE_Ib | DRIVE_POWER_WARNING_IDLE_Irot |\
    DRIVE_POWER_WARNING_IDLE_Ic | DRIVE_POWER_WARNING_IDLE_Iexc  \
)

//! Маска ошибок защиты по напряжению и току якоря.
#define PROT_ERRORS_MASK_ROT (\
    DRIVE_POWER_ERROR_UNDERFLOW_Urot | DRIVE_POWER_ERROR_OVERFLOW_Urot |\
    DRIVE_POWER_ERROR_UNDERFLOW_Irot | DRIVE_POWER_ERROR_OVERFLOW_Irot  \
)

//! Маска предупреждений защиты по напряжению и току якоря.
#define PROT_WARNINGS_MASK_ROT (\
    DRIVE_POWER_WARNING_UNDERFLOW_Urot | DRIVE_POWER_WARNING_OVERFLOW_Urot |\
    DRIVE_POWER_WARNING_UNDERFLOW_Irot | DRIVE_POWER_WARNING_OVERFLOW_Irot  \
)

//! Маска ошибок защиты по току возбуждения.
#define PROT_ERRORS_MASK_EXC (\
    DRIVE_POWER_ERROR_UNDERFLOW_Iexc | DRIVE_POWER_ERROR_OVERFLOW_Iexc  \
)

//! Маска предупреждений защиты по току возбуждения.
#define PROT_WARNINGS_MASK_EXC (\
    DRIVE_POWER_WARNING_UNDERFLOW_Iexc | DRIVE_POWER_WARNING_OVERFLOW_Iexc  \
)


static const size_t drive_prot_items[] = {
    DRIVE_PROT_PWR_ITEM_FAULT_OVF_Ua, DRIVE_PROT_PWR_ITEM_FAULT_UDF_Ua, DRIVE_PROT_PWR_ITEM_FAULT_OVF_Ia,
    DRIVE_PROT_PWR_ITEM_FAULT_OVF_Ub, DRIVE_PROT_PWR_ITEM_FAULT_UDF_Ub, DRIVE_PROT_PWR_ITEM_FAULT_OVF_Ib,
    DRIVE_PROT_PWR_ITEM_FAULT_OVF_Uc, DRIVE_PROT_PWR_ITEM_FAULT_UDF_Uc, DRIVE_PROT_PWR_ITEM_FAULT_OVF_Ic,
    DRIVE_PROT_PWR_ITEM_FAULT_OVF_Urot, DRIVE_PROT_PWR_ITEM_FAULT_OVF_Irot,
    DRIVE_PROT_PWR_ITEM_FAULT_OVF_Iexc, DRIVE_PROT_PWR_ITEM_FAULT_UDF_Iexc,
    DRIVE_PROT_PWR_ITEM_WARN_OVF_Ua, DRIVE_PROT_PWR_ITEM_WARN_UDF_Ua, DRIVE_PROT_PWR_ITEM_WARN_OVF_Ia,
    DRIVE_PROT_PWR_ITEM_WARN_OVF_Ub, DRIVE_PROT_PWR_ITEM_WARN_UDF_Ub, DRIVE_PROT_PWR_ITEM_WARN_OVF_Ib,
    DRIVE_PROT_PWR_ITEM_WARN_OVF_Uc, DRIVE_PROT_PWR_ITEM_WARN_UDF_Uc, DRIVE_PROT_PWR_ITEM_WARN_OVF_Ic,
    DRIVE_PROT_PWR_ITEM_WARN_OVF_Urot, DRIVE_PROT_PWR_ITEM_WARN_OVF_Irot,
    DRIVE_PROT_PWR_ITEM_WARN_OVF_Iexc, DRIVE_PROT_PWR_ITEM_WARN_UDF_Iexc,
    DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Ia, DRIVE_PROT_PWR_ITEM_WARN_IDLE_Ia,
    DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Ib, DRIVE_PROT_PWR_ITEM_WARN_IDLE_Ib,
    DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Ic, DRIVE_PROT_PWR_ITEM_WARN_IDLE_Ic,
    DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Urot, DRIVE_PROT_PWR_ITEM_WARN_IDLE_Urot,
    DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Irot, DRIVE_PROT_PWR_ITEM_WARN_IDLE_Irot,
    DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Iexc, DRIVE_PROT_PWR_ITEM_WARN_IDLE_Iexc
};
#define DRIVE_CHECK_PROT_ITEMS_COUNT ARRAY_LEN(drive_prot_items)

static const size_t drive_prot_cutoff_items[] = {
    DRIVE_PROT_PWR_ITEM_CUTOFF_Ua, DRIVE_PROT_PWR_ITEM_CUTOFF_Ub, DRIVE_PROT_PWR_ITEM_CUTOFF_Uc,
    DRIVE_PROT_PWR_ITEM_CUTOFF_Ia, DRIVE_PROT_PWR_ITEM_CUTOFF_Ib, DRIVE_PROT_PWR_ITEM_CUTOFF_Ic,
    DRIVE_PROT_PWR_ITEM_CUTOFF_Urot, DRIVE_PROT_PWR_ITEM_CUTOFF_Irot, DRIVE_PROT_PWR_ITEM_CUTOFF_Iexc
};
#define DRIVE_CHECK_CUTOFF_PROT_ITEMS_COUNT ARRAY_LEN(drive_prot_cutoff_items)

#define PROT_CUTOFF_ITEMS_ERRORS_MASK\
    (DRIVE_POWER_ERROR_OVERFLOW_Ua | DRIVE_POWER_ERROR_OVERFLOW_Ub | DRIVE_POWER_ERROR_OVERFLOW_Uc |\
     DRIVE_POWER_ERROR_OVERFLOW_Ia | DRIVE_POWER_ERROR_OVERFLOW_Ib | DRIVE_POWER_ERROR_OVERFLOW_Ic |\
     DRIVE_POWER_ERROR_OVERFLOW_Urot | DRIVE_POWER_ERROR_OVERFLOW_Irot | DRIVE_POWER_ERROR_OVERFLOW_Iexc)

/*#define PROT_CUTOFF_ITEMS_WARNINGS_MASK\
    (DRIVE_POWER_WARNING_OVERFLOW_Ua | DRIVE_POWER_WARNING_OVERFLOW_Ub | DRIVE_POWER_WARNING_OVERFLOW_Uc |\
     DRIVE_POWER_WARNING_OVERFLOW_Ia | DRIVE_POWER_WARNING_OVERFLOW_Ib | DRIVE_POWER_WARNING_OVERFLOW_Ic |\
     DRIVE_POWER_WARNING_OVERFLOW_Urot | DRIVE_POWER_WARNING_OVERFLOW_Irot | DRIVE_POWER_WARNING_OVERFLOW_Iexc)*/

// INIT
#define PROT_ITEMS_INIT_ERRORS_MASK     PROT_ERRORS_MASK_IN_OVF
#define PROT_ITEMS_INIT_WARNINGS_MASK   PROT_WARNINGS_MASK_IN_OVF
// CALIBRATION
#define PROT_ITEMS_CALIBRATION_ERRORS_MASK     DRIVE_POWER_ERROR_NONE
#define PROT_ITEMS_CALIBRATION_WARNINGS_MASK   DRIVE_POWER_WARNING_NONE
// IDLE
#define PROT_ITEMS_IDLE_ERRORS_MASK     PROT_ERRORS_MASK_IN | PROT_ERRORS_MASK_IDLE
#define PROT_ITEMS_IDLE_WARNINGS_MASK   PROT_WARNINGS_MASK_IN | PROT_WARNINGS_MASK_IDLE
// START
#define PROT_ITEMS_START_ERRORS_MASK    PROT_ERRORS_MASK_IN | PROT_ERRORS_MASK_ROT | DRIVE_POWER_ERROR_OVERFLOW_Iexc
#define PROT_ITEMS_START_WARNINGS_MASK  PROT_WARNINGS_MASK_IN | PROT_WARNINGS_MASK_ROT | DRIVE_POWER_WARNING_OVERFLOW_Iexc
// RUN
#define PROT_ITEMS_RUN_ERRORS_MASK      PROT_ERRORS_MASK_IN | PROT_ERRORS_MASK_ROT | PROT_ERRORS_MASK_EXC
#define PROT_ITEMS_RUN_WARNINGS_MASK    PROT_WARNINGS_MASK_IN | PROT_WARNINGS_MASK_ROT | PROT_WARNINGS_MASK_EXC
// STOP
#define PROT_ITEMS_STOP_ERRORS_MASK     PROT_ERRORS_MASK_IN | PROT_ERRORS_MASK_ROT | PROT_ERRORS_MASK_EXC
#define PROT_ITEMS_STOP_WARNINGS_MASK   PROT_WARNINGS_MASK_IN | PROT_WARNINGS_MASK_ROT | PROT_WARNINGS_MASK_EXC
// STOP ERROR
#define PROT_ITEMS_STOP_ERROR_ERRORS_MASK   PROT_ITEMS_STOP_ERRORS_MASK//DRIVE_POWER_ERROR_NONE
#define PROT_ITEMS_STOP_ERROR_WARNINGS_MASK PROT_ITEMS_STOP_WARNINGS_MASK//DRIVE_POWER_WARNING_NONE
// ERROR
#define PROT_ITEMS_ERROR_ERRORS_MASK    DRIVE_POWER_ERROR_NONE
#define PROT_ITEMS_ERROR_WARNINGS_MASK  DRIVE_POWER_WARNING_NONE

// External exc inverse mask.
// Errors.
#define PROT_ITEMS_EXT_EXC_ERRORS_INV_MASK (~(PROT_ERRORS_MASK_EXC | DRIVE_POWER_ERROR_IDLE_Iexc))
// Warnings.
#define PROT_ITEMS_EXT_EXC_WARNINGS_INV_MASK (~(PROT_WARNINGS_MASK_EXC | DRIVE_POWER_WARNING_IDLE_Iexc))


static const drive_prot_data_t drive_prot_data[] = {
    {PROT_ITEMS_INIT_ERRORS_MASK,           PROT_ITEMS_INIT_WARNINGS_MASK}, // Init
    {PROT_ITEMS_CALIBRATION_ERRORS_MASK,    PROT_ITEMS_CALIBRATION_WARNINGS_MASK}, // Calibration
    {PROT_ITEMS_IDLE_ERRORS_MASK,           PROT_ITEMS_IDLE_WARNINGS_MASK}, // Idle
    {PROT_ITEMS_START_ERRORS_MASK,          PROT_ITEMS_START_WARNINGS_MASK}, // Start
    {PROT_ITEMS_RUN_ERRORS_MASK,            PROT_ITEMS_RUN_WARNINGS_MASK}, // Run
    {PROT_ITEMS_STOP_ERRORS_MASK,           PROT_ITEMS_STOP_WARNINGS_MASK}, // Stop
    {PROT_ITEMS_STOP_ERROR_ERRORS_MASK,     PROT_ITEMS_STOP_ERROR_WARNINGS_MASK}, // Stop error
    {PROT_ITEMS_ERROR_ERRORS_MASK,          PROT_ITEMS_ERROR_WARNINGS_MASK}  // Error
};
#define DRIVE_PROT_DATA_COUNT ARRAY_LEN(drive_prot_data)


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
 * Устанавливает маски ошибок и предупреждений питания.
 * @param state Состояние привода.
 */
static void drive_set_prot_masks(drive_state_t state)
{
    drive_power_errors_t errors_mask = DRIVE_POWER_ERROR_ALL;
    drive_power_warnings_t warnings_mask = DRIVE_POWER_WARNING_ALL;
    
    if(state < DRIVE_PROT_DATA_COUNT){
        errors_mask = drive_prot_data[state].errors_mask;
        warnings_mask = drive_prot_data[state].warnings_mask;
    }
    
    if(drive_triacs_exc_mode() == DRIVE_TRIACS_EXC_EXTERNAL){
        errors_mask &= PROT_ITEMS_EXT_EXC_ERRORS_INV_MASK;
        warnings_mask &= PROT_ITEMS_EXT_EXC_WARNINGS_INV_MASK;
    }
    
    drive_protection_power_set_errs_mask(errors_mask);
    drive_protection_power_set_warn_mask(warnings_mask);
    
    if(state == DRIVE_STATE_INIT ||
       state == DRIVE_STATE_ERROR){
        drive_protection_item_set_masked(DRIVE_PROT_ITEM_FAULT_PHASE_STATE, false);
        drive_protection_item_set_masked(DRIVE_PROT_ITEM_FAULT_PHASES_ANGLES, false);
        drive_protection_item_set_masked(DRIVE_PROT_ITEM_FAULT_PHASES_SYNC, false);
    }else{
        drive_protection_item_set_masked(DRIVE_PROT_ITEM_FAULT_PHASE_STATE, true);
        drive_protection_item_set_masked(DRIVE_PROT_ITEM_FAULT_PHASES_ANGLES, true);
        drive_protection_item_set_masked(DRIVE_PROT_ITEM_FAULT_PHASES_SYNC, true);
    }
    
    if(state == DRIVE_STATE_RUN ||
       state == DRIVE_STATE_START ||
       state == DRIVE_STATE_STOP){
        drive_protection_item_set_masked(DRIVE_PROT_ITEM_ROT_BREAK, true);
        drive_protection_item_set_masked(DRIVE_PROT_ITEM_WARN_TRIACS, true);
    }else{
        drive_protection_item_set_masked(DRIVE_PROT_ITEM_ROT_BREAK, false);
        drive_protection_item_set_masked(DRIVE_PROT_ITEM_WARN_TRIACS, false);
    }
    
    drive_protection_item_set_masked(DRIVE_PROT_ITEM_FAN, true);
    drive_protection_item_set_masked(DRIVE_PROT_ITEM_WARN_HEATSINK_TEMP, true);
    drive_protection_item_set_masked(DRIVE_PROT_ITEM_FAULT_HEATSINK_TEMP, true);
}

/**
 * Устанавливает маски ошибок и предупреждений питания.
 */
static void drive_update_prot_masks(void)
{
    drive_state_t state = drive_get_state();
    
    drive_set_prot_masks(state);
}

/**
 * Изменяет маски ошибок и предупреждений питания.
 * @param state_from Текущее состояние.
 * @param state_to Необходимое состояние.
 */
static void drive_change_prot_masks(drive_state_t state_from, drive_state_t state_to)
{
    bool from_start = (state_from == DRIVE_STATE_START);
    bool to_stop = ((state_to == DRIVE_STATE_STOP) || (state_to == DRIVE_STATE_STOP_ERROR));
    
    bool from_stop = (state_from == DRIVE_STATE_STOP);
    bool to_start = (state_to == DRIVE_STATE_START);
    
    bool start_to_stop = from_start && to_stop;
    bool stop_to_start = from_stop && to_start;
    
    if(!(start_to_stop || stop_to_start)){
        drive_set_prot_masks(state_to);
    }
}

/**
 * Установка состояния.
 * @param state Состояние.
 */
static void drive_set_state(drive_state_t state)
{
    if(drive.state == state) return;
    
    drive_change_prot_masks(drive.state, state);
    
    drive.state = state;
    
    switch(drive.state){
        case DRIVE_STATE_INIT:
            drive.init_state = DRIVE_INIT_BEGIN;
            drive.status = DRIVE_STATUS_INIT;
            break;
        case DRIVE_STATE_CALIBRATION:
            drive.power_calibration_state = DRIVE_PWR_CALIBRATION_BEGIN;
            break;
        case DRIVE_STATE_IDLE:
            drive.status = DRIVE_STATUS_IDLE;
            break;
        case DRIVE_STATE_START:
            drive.starting_state = DRIVE_STARTING_BEGIN;
            drive.status = DRIVE_STATUS_RUN;
            break;
        case DRIVE_STATE_RUN:
            drive.status = DRIVE_STATUS_RUN;
            break;
        case DRIVE_STATE_STOP:
            drive.stopping_state = DRIVE_STOPPING_BEGIN;
            drive.status = DRIVE_STATUS_RUN;
            break;
        case DRIVE_STATE_STOP_ERROR:
            drive.err_stopping_state = DRIVE_ERR_STOPPING_BEGIN;
            drive.status = DRIVE_STATUS_ERROR;
            break;
        case DRIVE_STATE_ERROR:
            drive.status = DRIVE_STATUS_ERROR;
            break;
    }
}

/**
 * Восстанавливает состояние привода.
 */
ALWAYS_INLINE static void drive_save_state(void)
{
    drive.saved_state = drive.state;
}

/**
 * Восстанавливает состояние привода.
 */
ALWAYS_INLINE static void drive_restore_state(void)
{
    drive.state = drive.saved_state;
}

/**
 * Устанавливает сохранённое состояние привода.
 */
ALWAYS_INLINE static void drive_set_saved_state(drive_state_t state)
{
    drive.saved_state = state;
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
static void drive_clear_warning(drive_warnings_t warning)
{
    drive.warnings &= ~warning;
}

/**
 * Установка ошибки питания.
 * @param error Ошибка.
 */
static void drive_set_power_error(drive_power_errors_t error)
{
    drive.power_errors |= error;
    drive_set_error(DRIVE_ERROR_POWER_INVALID);
}

/**
 * Снятие ошибки питания.
 * @param error Ошибка.
 */
static void drive_clear_power_error(drive_power_errors_t error)
{
    drive.power_errors &= ~error;
    if(drive.power_errors == DRIVE_POWER_ERROR_NONE &&
       drive.power_cutoff_errors == DRIVE_POWER_ERROR_NONE){
        drive_clear_error(DRIVE_ERROR_POWER_INVALID);
    }
}

/**
 * Получает флаг возникновения новой ошибки питания.
 * @param error Ошибка питания.
 * @return Флаг возникновения новой ошибки питания.
 */
/*ALWAYS_INLINE static bool drive_is_new_power_error(drive_power_error_t error)
{
    return (drive.power_errors & error) == 0;
}*/

/**
 * Установка предупреждения питания.
 * @param error Предупреждение.
 */
static void drive_set_power_warning(drive_power_warnings_t warning)
{
    drive.power_warnings |= warning;
    drive_set_warning(DRIVE_WARNING_POWER);
}

/**
 * Снятие предупреждения питания.
 * @param warning Предупреждение.
 */
static void drive_clear_power_warning(drive_power_warnings_t warning)
{
    drive.power_warnings &= ~warning;
    if(drive.power_warnings == DRIVE_POWER_WARNING_NONE) drive_clear_warning(DRIVE_WARNING_POWER);
}

/**
 * Получает флаг возникновения нового предупреждения питания.
 * @param warning Предупреждение питания.
 * @return Флаг возникновения нового предупреждения питания.
 */
/*ALWAYS_INLINE static bool drive_is_new_power_warning(drive_power_warning_t warning)
{
    return (drive.power_warnings & warning) == 0;
}*/

/**
 * Установка ошибки отсечки питания.
 * @param error Ошибка.
 */
static void drive_set_power_cutoff_error(drive_power_errors_t error)
{
    drive.power_cutoff_errors |= error;
    drive_set_error(DRIVE_ERROR_POWER_INVALID);
}

/**
 * Снятие ошибки отсечки питания.
 * @param error Ошибка.
 */
/*static void drive_clear_power_cutoff_error(drive_power_errors_t error)
{
    drive.power_cutoff_errors &= ~error;
    if(drive.power_errors == DRIVE_POWER_ERROR_NONE &&
       drive.power_cutoff_errors == DRIVE_POWER_ERROR_NONE){
        drive_clear_error(DRIVE_ERROR_POWER_INVALID);
    }
}*/


/**
 * Установка ошибки углов между фазами.
 * @param error Ошибка.
 */
/*static void drive_set_phase_angle_error(drive_phase_angle_errors_t error)
{
    drive.phase_angle_errors |= error;
    drive_set_error(DRIVE_ERROR_PHASE_ANGLE);
}*/

/**
 * Снятие ошибки углов между фазами.
 * @param error Ошибка.
 */
/*static void drive_clear_phase_angle_error(drive_phase_angle_errors_t error)
{
    drive.phase_angle_errors &= ~error;
    if(drive.phase_angle_errors == DRIVE_PHASE_ANGLE_NO_ERROR) drive_clear_error(DRIVE_ERROR_PHASE_ANGLE);
}*/

/**
 * Установка предупреждения углов между фазами.
 * @param warning Предупреждение.
 */
/*static void drive_set_phase_angle_warning(drive_phase_angle_warnings_t warning)
{
    drive.phase_angle_warnings |= warning;
    drive_set_warning(DRIVE_WARNING_PHASE_ANGLE);
}*/

/**
 * Снятие предупреждения углов между фазами.
 * @param warning Предупреждение.
 */
/*static void drive_clear_phase_angle_warning(drive_phase_angle_warnings_t warning)
{
    drive.phase_angle_warnings &= ~warning;
    if(drive.phase_angle_warnings == DRIVE_PHASE_ANGLE_NO_WARNING) drive_clear_warning(DRIVE_WARNING_PHASE_ANGLE);
}*/

/*
 * Обработка возникновения ошибки.
 */

/**
 * Обработчик возникновения ошибки.
 */
static void drive_on_error(void)
{
    if(drive.status != DRIVE_STATUS_ERROR){
        if(drive.on_error_occured){
            drive.on_error_occured();
        }
    }
}

/**
 * Обработчик возникновения ошибки.
 * @param error Ошибка.
 */
static void drive_error_occured(drive_errors_t error)
{
    drive_set_error(error);

    if(drive.status != DRIVE_STATUS_ERROR){
        drive_on_error();
    }
}

/**
 * Обработчик возникновения предупреждения.
 */
static void drive_on_warning(void)
{
    if(drive.status != DRIVE_STATUS_ERROR){
        if(drive.on_warning_occured){
            drive.on_warning_occured();
        }
    }
}

/**
 * Обработчик возникновения предупреждения.
 * @param error Предупреждение.
 */
/*static void drive_warning_occured(drive_warnings_t warning)
{
    drive_set_warning(warning);

    if(drive.status != DRIVE_STATUS_ERROR){
        drive_on_warning();
    }
}*/

/**
 * Обработчик возникновения ошибки питания.
 * @param error Ошибка питания.
 */
/*static void drive_power_error_occured(drive_power_errors_t error)
{
    drive_set_power_error(error);
    drive_error_occured(DRIVE_ERROR_POWER_INVALID);
}*/

/**
 * Обработчик возникновения предупреждения питания.
 * @param warning Предупреждение питания.
 */
/*static void drive_power_warning_occured(drive_power_warnings_t warning)
{
    drive_set_power_warning(warning);
    drive_warning_occured(DRIVE_WARNING_POWER);
}*/

/**
 * Обработчик возникновения ошибки фаз.
 */
/*static void drive_on_phase_error_occured(void)
{
    if(drive.state != DRIVE_STATE_INIT)
        drive_error_occured(DRIVE_ERROR_PHASE);
}*/

/**
 * Прекращает подачу питания на двигатель.
 */
static void drive_supply_off(void)
{
    // Остановить таймеры открытия тиристоров.
    drive_triacs_stop();
    // Запретить открытие тиристоров.
    drive_triacs_set_pairs_enabled(false);
    drive_triacs_set_exc_enabled(false);
    // Остановить регулятор.
    drive_regulator_stop();
    // Запретить регулирование якоря и возбуждения.
    drive_regulator_set_rot_enabled(false);
    drive_regulator_set_exc_enabled(false);
}

/**
 * Устанавливает параметры останова привода.
 * @param fast_stop Флаг быстрого останова.
 * @param stop_to_error Флаг останова в состояние ошибки.
 */
static void drive_setup_stop(drive_stop_mode_t stop_mode, drive_state_t next_state)
{
    drive.stopping_cur_mode = stop_mode;
    drive.stopping_next_state = next_state;
}

static void drive_stop_normal(void)
{
    drive_setup_stop(DRIVE_STOP_MODE_NORMAL, DRIVE_STATE_IDLE);
    drive_set_state(DRIVE_STATE_STOP);
}

static void drive_stop_fast(void)
{
    drive_setup_stop(DRIVE_STOP_MODE_FAST, DRIVE_STATE_IDLE);
    drive_set_state(DRIVE_STATE_STOP);
}

static void drive_stop_coast(void)
{
    drive_supply_off();
    drive_set_state(DRIVE_STATE_IDLE);
}

/*static void drive_error_stop_normal(void)
{
    drive_setup_stop(DRIVE_STOP_MODE_NORMAL, DRIVE_STATE_ERROR);
    drive_set_state(DRIVE_STATE_STOP_ERROR);
}*/

static void drive_error_stop_fast(void)
{
    drive_setup_stop(DRIVE_STOP_MODE_FAST, DRIVE_STATE_ERROR);
    drive_set_state(DRIVE_STATE_STOP_ERROR);
}

static void drive_error_stop_coast(void)
{
    drive_supply_off();
    drive_set_state(DRIVE_STATE_ERROR);
}

static void drive_error_stop_cutoff(void)
{
    // Отключить питание двигателя.
    drive_supply_off();
    // Перейти в состояние ошибки.
    drive_set_state(DRIVE_STATE_ERROR);
}

/**
 * Обрабатывает действие останова по ошибке элемента защиты.
 * @param action Действие элемента защиты.
 * @return Флаг обработки элемента защиты.
 */
static bool drive_prot_error_stop_by_action(drive_prot_action_t action)
{
    switch(action){
        case DRIVE_PROT_ACTION_IGNORE:
            return false;
        case DRIVE_PROT_ACTION_WARNING:
            break;
        case DRIVE_PROT_ACTION_COAST_STOP:
            drive_error_stop_coast();
            break;
        case DRIVE_PROT_ACTION_FAST_STOP:
            drive_error_stop_fast();
            break;
        case DRIVE_PROT_ACTION_EMERGENCY_STOP:
            drive_error_stop_cutoff();
            break;
    }
    return true;
}

/**
 * Обрабатывает действие останова элемента защиты.
 * @param action Действие элемента защиты.
 * @return Флаг обработки элемента защиты.
 */
static bool drive_prot_stop_by_action(drive_prot_action_t action)
{
    switch(action){
        case DRIVE_PROT_ACTION_IGNORE:
            return false;
        case DRIVE_PROT_ACTION_WARNING:
            break;
        case DRIVE_PROT_ACTION_COAST_STOP:
            drive_stop_coast();
            break;
        case DRIVE_PROT_ACTION_FAST_STOP:
            drive_stop_fast();
            break;
        case DRIVE_PROT_ACTION_EMERGENCY_STOP:
            drive_error_stop_cutoff();
            break;
    }
    return true;
}

/**
 * Обрабатывает событие действия элемента защиты.
 * @param action Действие элемента защиты.
 * @return Флаг обработки элемента защиты.
 */
static bool drive_prot_event_by_action(drive_prot_action_t action)
{
    switch(action){
        case DRIVE_PROT_ACTION_IGNORE:
            return false;
        case DRIVE_PROT_ACTION_WARNING:
            drive_on_warning();
            break;
        case DRIVE_PROT_ACTION_COAST_STOP:
        case DRIVE_PROT_ACTION_FAST_STOP:
        case DRIVE_PROT_ACTION_EMERGENCY_STOP:
            drive_on_error();
            break;
    }
    return true;
}

/**
 * Сравнивает действия элементы защиты исходя из жёсткости реакции.
 * Например, EMERGENCY_STOP > FAST_STOP и WARNING > IGNORE.
 * @param lact Действие элемента защиты.
 * @param ract Действие элемента защиты.
 * @return Результат сравнения элементов защиты.
 *  1 - lact больше ract;
 * -1 - lact меньше ract;
 *  0 - lact равен  ract.
 */
static int drive_prot_action_compare(drive_prot_action_t lact, drive_prot_action_t ract)
{
    if((int)lact > (int)ract) return 1;
    if((int)lact < (int)ract) return -1;
    return 0;
}

/**
 * Получает наиболее жёсткое действие элемента защиты.
 * @param lact Действие элемента защиты.
 * @param ract Действие элемента защиты.
 * @return Наиболее жёсткое действие элемента защиты.
 */
static drive_prot_action_t drive_prot_get_hard_action(drive_prot_action_t lact, drive_prot_action_t ract)
{
    if(drive_prot_action_compare(lact, ract) > 0) return lact;
    return ract;
}

/**
 * Выполняет проверку общего элемента защиты.
 * @param item Элемент защиты.
 * @param error Ошибка элемента защиты.
 * @param warning Предупреждение элемента защиты.
 * @return результирующее действие.
 */
static drive_prot_action_t drive_check_prot_item(drive_prot_index_t item, drive_error_t error, drive_warning_t warning)
{
    if(drive_protection_check_item(item)){
        if(error != DRIVE_ERROR_NONE) drive_set_error(error);
        if(warning != DRIVE_WARNING_NONE) drive_set_warning(warning);
        
        return drive_protection_item_action(item);
        
    }else if(!drive_protection_item_active(item)){
        if(error != DRIVE_ERROR_NONE) drive_clear_error(error);
        if(warning != DRIVE_WARNING_NONE) drive_clear_warning(warning);
    }
    
    return DRIVE_PROT_ACTION_IGNORE;
}

/**
 * Проверяет значение входов питания.
 */
static void drive_check_prots(void)
{
    // Предупреждения и ошибки элемента защиты.
    drive_power_warnings_t item_warnings;
    drive_power_errors_t item_errors;
    
    // Наиболее жёсткое действие.
    drive_prot_action_t res_action = DRIVE_PROT_ACTION_IGNORE;
    
    // Индекс элемента защиты.
    drive_prot_index_t index;
    
    size_t i;
    for(i = 0; i < DRIVE_CHECK_PROT_ITEMS_COUNT; i ++){
        
        index = drive_prot_items[i];
        
        item_warnings = drive_protection_power_item_warning(index);
        item_errors = drive_protection_power_item_error(index);
        
        if(drive_protection_power_check_item(index, NULL, NULL)){
            
            res_action = drive_prot_get_hard_action(res_action,
                                drive_protection_power_item_action(index));
            
        }
        if(drive_protection_power_item_active(index)){
            
            if(item_warnings != DRIVE_POWER_WARNING_NONE){
                drive_set_power_warning(item_warnings);
            }
            
            if(item_errors != DRIVE_POWER_ERROR_NONE){
                drive_set_power_error(item_errors);
            }
        }else{
            
            if(item_warnings != DRIVE_POWER_WARNING_NONE){
                drive_clear_power_warning(item_warnings);
            }
            
            if(item_errors != DRIVE_POWER_ERROR_NONE){
                drive_clear_power_error(item_errors);
            }
        }
    }
    
    // TOP.
    drive_top_check_res_t top_check = drive_protection_top_check();
    switch(top_check){
        case DRIVE_TOP_CHECK_NORMAL:
            drive_clear_warning(DRIVE_WARNING_THERMAL_OVERLOAD);
            drive_clear_error(DRIVE_ERROR_THERMAL_OVERLOAD);
            break;
        case DRIVE_TOP_CHECK_HEATING:
            drive_set_warning(DRIVE_WARNING_THERMAL_OVERLOAD);
            break;
        case DRIVE_TOP_CHECK_OVERHEAT:
            res_action = drive_prot_get_hard_action(res_action,
                                    drive_protection_top_action());
        case DRIVE_TOP_CHECK_COOLING:
            drive_set_error(DRIVE_ERROR_THERMAL_OVERLOAD);
            break;
    }
    
    
    // Прочие защиты.
    
    res_action = drive_prot_get_hard_action(res_action,
            drive_check_prot_item(DRIVE_PROT_ITEM_FAULT_PHASE_STATE, DRIVE_ERROR_PHASE, DRIVE_WARNING_NONE));
    
    res_action = drive_prot_get_hard_action(res_action,
            drive_check_prot_item(DRIVE_PROT_ITEM_FAULT_PHASES_ANGLES, DRIVE_ERROR_PHASE_ANGLE, DRIVE_WARNING_NONE));
    
    res_action = drive_prot_get_hard_action(res_action,
            drive_check_prot_item(DRIVE_PROT_ITEM_WARN_PHASES_ANGLES, DRIVE_ERROR_NONE, DRIVE_WARNING_PHASE_ANGLE));
    
    res_action = drive_prot_get_hard_action(res_action,
            drive_check_prot_item(DRIVE_PROT_ITEM_FAULT_PHASES_SYNC, DRIVE_ERROR_PHASE_SYNC, DRIVE_WARNING_NONE));
    
    res_action = drive_prot_get_hard_action(res_action,
            drive_check_prot_item(DRIVE_PROT_ITEM_WARN_PHASES_SYNC, DRIVE_ERROR_NONE, DRIVE_WARNING_PHASE_SYNC));
    
    res_action = drive_prot_get_hard_action(res_action,
            drive_check_prot_item(DRIVE_PROT_ITEM_ROT_BREAK, DRIVE_ERROR_ROT_BREAK, DRIVE_WARNING_NONE));
    
    res_action = drive_prot_get_hard_action(res_action,
            drive_check_prot_item(DRIVE_PROT_ITEM_FAN, DRIVE_ERROR_NONE, DRIVE_WARNING_FAN_FAIL));
    
    res_action = drive_prot_get_hard_action(res_action,
            drive_check_prot_item(DRIVE_PROT_ITEM_FAULT_HEATSINK_TEMP, DRIVE_ERROR_HEATSINK_TEMP, DRIVE_WARNING_NONE));
    
    res_action = drive_prot_get_hard_action(res_action,
            drive_check_prot_item(DRIVE_PROT_ITEM_WARN_HEATSINK_TEMP, DRIVE_ERROR_NONE, DRIVE_WARNING_HEATSINK_TEMP));
    
    res_action = drive_prot_get_hard_action(res_action,
            drive_check_prot_item(DRIVE_PROT_ITEM_WARN_TRIACS, DRIVE_ERROR_NONE, DRIVE_WARNING_TRIAC));
    
    // Если требуется действие.
    if(res_action != DRIVE_PROT_ACTION_IGNORE){
        if(drive_prot_event_by_action(res_action)){
            drive_prot_stop_by_action(res_action);
        }
    }
}

/**
 * Проверяет значение входов питания на момент получения данных АЦП.
 */
static void drive_check_power_inst(void)
{
    drive_power_errors_t errors = DRIVE_POWER_ERROR_NONE;
    //drive_power_warnings_t warnings = DRIVE_POWER_WARNING_NONE;
    
    drive_protection_power_set_cutoff_errs_mask(PROT_CUTOFF_ITEMS_ERRORS_MASK);
    //drive_protection_power_set_cutoff_warn_mask(PROT_CUTOFF_ITEMS_WARNINGS_MASK);
    
    if(drive_protection_power_check_items(drive_prot_cutoff_items, DRIVE_CHECK_CUTOFF_PROT_ITEMS_COUNT, NULL, &errors)){
        
        if(errors != DRIVE_POWER_ERROR_NONE){
            drive_set_power_cutoff_error(errors);
            drive_on_error();
            drive_error_stop_cutoff();
        }
    }
}


/*
 * Обработка состояний привода.
 */

//! Макрос для обновления параметра значения питания.
#define DRIVE_UPDATE_POWER_PARAM(PARAM, CHANNEL)\
    do {\
        if(PARAM) settings_param_set_valuef(PARAM, drive_power_channel_real_value(CHANNEL));\
    }while(0)

/**
 * Обновляет значения питания в параметрах.
 */
static void drive_update_virtual_parameters(void)
{
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_u_a, DRIVE_POWER_Ua);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_u_b, DRIVE_POWER_Ub);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_u_c, DRIVE_POWER_Uc);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_u_rot, DRIVE_POWER_Urot);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_i_a, DRIVE_POWER_Ia);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_i_b, DRIVE_POWER_Ib);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_i_c, DRIVE_POWER_Ic);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_i_rot, DRIVE_POWER_Irot);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_i_exc, DRIVE_POWER_Iexc);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_i_ref, DRIVE_POWER_Iref);
    DRIVE_UPDATE_POWER_PARAM(drive.params.param_i_fan, DRIVE_POWER_Ifan);
    
    DRIVE_UPDATE_PARAM_FIXED(drive.params.param_calc_u_rot, drive_power_open_angle_voltage());
    
    DRIVE_UPDATE_PARAM_FIXED(drive.params.param_pairs_angle, drive_triacs_pairs_start_open_angle());
    DRIVE_UPDATE_PARAM_FIXED(drive.params.param_exc_angle, drive_triacs_exc_start_open_angle());
}

//! Макрос для обновления параметра состояния цифрового входа.
#define DRIVE_UPDATE_DIGITAL_INPUT_STATE_PARAM(PARAM, INPUT)\
    do {\
        if(PARAM) settings_param_set_valueu(PARAM, (uint32_t)drive_dio_input_state(INPUT));\
    }while(0)
//! Макрос для обновления параметра состояния цифрового выхода.
#define DRIVE_UPDATE_DIGITAL_OUTPUT_STATE_PARAM(PARAM, OUTPUT)\
    do {\
        if(PARAM) settings_param_set_valueu(PARAM, (uint32_t)drive_dio_output_state(OUTPUT));\
    }while(0)
/**
 * Обновляет значения состояний цифровых входов/выходов в параметрах.
 */
static void drive_update_digital_state_parameters(void)
{
    // цифровые входа
    DRIVE_UPDATE_DIGITAL_INPUT_STATE_PARAM(drive.params.param_digital_in_1, DRIVE_DIO_INPUT_1);
    DRIVE_UPDATE_DIGITAL_INPUT_STATE_PARAM(drive.params.param_digital_in_2, DRIVE_DIO_INPUT_2);
    DRIVE_UPDATE_DIGITAL_INPUT_STATE_PARAM(drive.params.param_digital_in_3, DRIVE_DIO_INPUT_3);
    DRIVE_UPDATE_DIGITAL_INPUT_STATE_PARAM(drive.params.param_digital_in_4, DRIVE_DIO_INPUT_4);
    DRIVE_UPDATE_DIGITAL_INPUT_STATE_PARAM(drive.params.param_digital_in_5, DRIVE_DIO_INPUT_5);
    // цифровые выхода
    DRIVE_UPDATE_DIGITAL_OUTPUT_STATE_PARAM(drive.params.param_digital_out_1, DRIVE_DIO_OUTPUT_1);
    DRIVE_UPDATE_DIGITAL_OUTPUT_STATE_PARAM(drive.params.param_digital_out_2, DRIVE_DIO_OUTPUT_2);
    DRIVE_UPDATE_DIGITAL_OUTPUT_STATE_PARAM(drive.params.param_digital_out_3, DRIVE_DIO_OUTPUT_3);
    DRIVE_UPDATE_DIGITAL_OUTPUT_STATE_PARAM(drive.params.param_digital_out_4, DRIVE_DIO_OUTPUT_4);
}

//! Макрос для обновления параметра калибровочных данных.
#define DRIVE_UPDATE_CALIBRATION_PARAM(PARAM_ID, CHANNEL)\
    do {\
        param_t* p = settings_param_by_id(PARAM_ID);\
        if(p) settings_param_set_valueu(p, drive_power_calibration_data(CHANNEL));\
    }while(0)

/**
 * Обновляет калибровочные данные в параметрах.
 */
static void drive_update_clibration_parameters(void)
{
    DRIVE_UPDATE_CALIBRATION_PARAM(PARAM_ID_ADC_CALIBRATION_DATA_Ua, DRIVE_POWER_Ua);
    DRIVE_UPDATE_CALIBRATION_PARAM(PARAM_ID_ADC_CALIBRATION_DATA_Ub, DRIVE_POWER_Ub);
    DRIVE_UPDATE_CALIBRATION_PARAM(PARAM_ID_ADC_CALIBRATION_DATA_Uc, DRIVE_POWER_Uc);
    DRIVE_UPDATE_CALIBRATION_PARAM(PARAM_ID_ADC_CALIBRATION_DATA_Urot, DRIVE_POWER_Urot);
    DRIVE_UPDATE_CALIBRATION_PARAM(PARAM_ID_ADC_CALIBRATION_DATA_Ia, DRIVE_POWER_Ia);
    DRIVE_UPDATE_CALIBRATION_PARAM(PARAM_ID_ADC_CALIBRATION_DATA_Ib, DRIVE_POWER_Ib);
    DRIVE_UPDATE_CALIBRATION_PARAM(PARAM_ID_ADC_CALIBRATION_DATA_Ic, DRIVE_POWER_Ic);
    DRIVE_UPDATE_CALIBRATION_PARAM(PARAM_ID_ADC_CALIBRATION_DATA_Irot, DRIVE_POWER_Irot);
    DRIVE_UPDATE_CALIBRATION_PARAM(PARAM_ID_ADC_CALIBRATION_DATA_Iexc, DRIVE_POWER_Iexc);
    DRIVE_UPDATE_CALIBRATION_PARAM(PARAM_ID_ADC_CALIBRATION_DATA_Iref, DRIVE_POWER_Iref);
    DRIVE_UPDATE_CALIBRATION_PARAM(PARAM_ID_ADC_CALIBRATION_DATA_Ifan, DRIVE_POWER_Ifan);
}

/**
 * Обновляет параметры со значениями ПИД-регуляторов.
 */
static void drive_update_pid_parameters(void)
{
    DRIVE_UPDATE_PARAM_FIXED(drive.params.param_pid_rot_spd_val, drive_regulator_rot_speed_current_ref());
    DRIVE_UPDATE_PARAM_FIXED(drive.params.param_pid_rot_cur_val, drive_regulator_rot_open_angle());
    DRIVE_UPDATE_PARAM_FIXED(drive.params.param_pid_exc_cur_val, drive_regulator_exc_open_angle());
}

/**
 * Обрабатывает цифровые входа привода.
 */
static void drive_process_digital_inputs(void)
{
    drive_dio_process_inputs(DRIVE_DIO_DT);
    
    drive_dio_state_t state = DRIVE_DIO_OFF;
    
    if(drive_dio_input_get_type_state(DRIVE_DIO_IN_EMERGENCY_STOP, &state)){
        if(state) drive_emergency_stop();
    }
    
    if(drive_dio_input_get_type_state(DRIVE_DIO_IN_START_STOP, &state)){
        if(state){
            drive_start();
        }else{ //DRIVE_DIO_OFF
            drive_stop();
        }
    }
    
    if(drive_dio_input_get_type_state(DRIVE_DIO_IN_REFERENCE_INC, &state)){
        if(state) drive_regulator_inc_reference();
    }
    
    if(drive_dio_input_get_type_state(DRIVE_DIO_IN_REFERENCE_DEC, &state)){
        if(state) drive_regulator_dec_reference();
    }
    
    if(drive_dio_input_get_type_state(DRIVE_DIO_IN_CLEAR_ERRORS, &state)){
        if(state) drive_clear_errors();
    }
}

/**
 * Обновляет цифровые выхода привода.
 */
static void drive_update_digital_outputs(void)
{
    drive_dio_set_output_type_state(DRIVE_DIO_OUT_OK,      drive_ok());
    drive_dio_set_output_type_state(DRIVE_DIO_OUT_READY,   drive_ready());
    drive_dio_set_output_type_state(DRIVE_DIO_OUT_RUNNING, drive_running());
    drive_dio_set_output_type_state(DRIVE_DIO_OUT_WARNING, drive_warnings() != DRIVE_WARNING_NONE);
    drive_dio_set_output_type_state(DRIVE_DIO_OUT_ERROR,   drive_errors() != DRIVE_ERROR_NONE);
}

/**
 * Обрабатывает тепловую защиту. 
 */
static void drive_process_top(void)
{
    if(drive_flags_is_set(DRIVE_FLAG_POWER_DATA_AVAIL)){
        drive_protection_top_process(drive_power_channel_real_value(DRIVE_POWER_Irot), DRIVE_TOP_DT);
    }
}

/**
 * Получает флаг допустимости открытия тиристоров.
 * @return Флаг допустимости открытия тиристоров.
 */
static bool drive_can_open_triacs(void)
{
    if(drive_phase_state_errors() != PHASE_NO_ERROR) return false;
    if(!drive_phase_sync_synchronized()) return  false;
    //if(drive_dio_output_type_state(DRIVE_DIO_OUT_USER) == DRIVE_DIO_ON) return false;
    
    return true;
}

/**
 * Регулировка привода.
 * @return Флаг регулировки привода.
 */
static bool drive_regulate(void)
{
    if(!drive_power_new_data_avail(DRIVE_POWER_CHANNELS)) return false;
    
    if(drive_can_open_triacs()){
        fixed32_t U_rot = drive_power_channel_real_value(DRIVE_POWER_Urot);
        fixed32_t I_rot = drive_power_channel_real_value(DRIVE_POWER_Irot);
        fixed32_t I_exc = drive_power_channel_real_value(DRIVE_POWER_Iexc);

        drive_regulator_regulate(U_rot, I_rot, I_exc);

        fixed32_t rot_angle = drive_regulator_rot_open_angle();
        fixed32_t exc_angle = drive_regulator_exc_open_angle();// + DRIVE_EXC_PID_VALUE_DELTA;

        drive_triacs_set_pairs_open_angle(rot_angle);
        //drive_triacs_set_pairs_open_angle(fixed32_make_from_int(30));
        drive_triacs_set_exc_open_angle(exc_angle);
        //drive_triacs_set_exc_open_angle(fixed32_make_from_int(90));

        //pid_controller_t* pid_spd = drive_regulator_spd_pid();
        //pid_controller_t* pid_rot = drive_regulator_rot_pid();

        //settings_param_set_valuef(settings_param_by_id(PARAM_ID_DEBUG_6), pid_spd->value);
        //settings_param_set_valuef(settings_param_by_id(PARAM_ID_DEBUG_7), pid_rot->value);
        //settings_param_set_valuef(settings_param_by_id(PARAM_ID_DEBUG_0), pid->prev_i);
        
        //drive_update_pid_parameters();
        
        return true;
    }else{
        drive_regulator_adjust_current_reference();
    }
    
    return false;
}

/**
 * Запускает открытие тиристоров.
 */
static void drive_setup_triacs_open(phase_t phase, phase_time_t sensor_time)
{
    drive_triacs_setup_exc(phase, sensor_time);
    drive_triacs_setup_next_pairs(phase, sensor_time);
}

/**
 * Обработка состояния ошибки привода.
 * @return Код ошибки.
 */
static err_t drive_state_process_error(void)
{
    return E_NO_ERROR;
}

/**
 * Обработка состояния работы привода.
 * @return Код ошибки.
 */
static err_t drive_state_process_running(void)
{
    drive_regulate();
    
    return E_NO_ERROR;
}

/**
 * Функция останова привода с торможением.
 * @param fast_stop Флаг необходимости быстрого останова.
 * @return Флаг завершения останова.
 */
static bool drive_state_process_stop_brake(bool fast_stop)
{
    bool wait_done = false;
    
    drive_regulate();
    
    switch(drive.stopping_state){
        default:
        case DRIVE_STOPPING_NONE:
            break;
        case DRIVE_STOPPING_BEGIN:
            if(fast_stop){
                drive_regulator_fast_stop();
            }else{
                drive_regulator_stop();
            }
            drive.stopping_state = DRIVE_STOPPING_RAMP;
            break;
        case DRIVE_STOPPING_RAMP:
            if(drive_regulator_state() == DRIVE_REGULATOR_STATE_IDLE){
                drive_triacs_set_pairs_enabled(false);
                drive_regulator_set_rot_enabled(false);
                drive.stopping_state = DRIVE_STOPPING_WAIT_ROT;
                drive.iters_counter = 0;
            }
            break;
        case DRIVE_STOPPING_WAIT_ROT:
            if(drive_protection_power_item_allow(DRIVE_PROT_PWR_ITEM_WARN_IDLE_Urot) &&
               drive_protection_power_item_stable(DRIVE_PROT_PWR_ITEM_WARN_IDLE_Urot)){
                wait_done = true;
            }else if(drive.iters_counter >= drive.settings.stop_rot_iters){
                wait_done = true;
            }else{
                drive.iters_counter ++;
            }
            
            if(wait_done){
                drive_protection_power_set_errs_mask_flags(DRIVE_POWER_ERROR_IDLE_Urot);
                drive_protection_power_set_warn_mask_flags(DRIVE_POWER_WARNING_IDLE_Urot);
                if(drive_triacs_exc_mode() != DRIVE_TRIACS_EXC_EXTERNAL){
                    drive_protection_power_reset_errs_mask_flags(DRIVE_POWER_ERROR_UNDERFLOW_Iexc);
                    drive_protection_power_reset_warn_mask_flags(DRIVE_POWER_WARNING_UNDERFLOW_Iexc);
                    drive_triacs_set_exc_enabled(false);
                    drive_regulator_set_exc_enabled(false);
                }
                drive.stopping_state = DRIVE_STOPPING_WAIT_EXC;
                drive.iters_counter = 0;
            }
            
            break;
        case DRIVE_STOPPING_WAIT_EXC:
            if(drive_triacs_exc_mode() == DRIVE_TRIACS_EXC_EXTERNAL){
                wait_done = true;
            }else if(drive_protection_power_item_allow(DRIVE_PROT_PWR_ITEM_WARN_IDLE_Iexc) &&
                     drive_protection_power_item_stable(DRIVE_PROT_PWR_ITEM_WARN_IDLE_Iexc)){
                wait_done = true;
            }else if(drive.iters_counter >= drive.settings.stop_exc_iters){
                wait_done = true;
            }else{
                drive.iters_counter ++;
            }
            
            if(wait_done){
                if(drive_triacs_exc_mode() != DRIVE_TRIACS_EXC_EXTERNAL){
                    drive_protection_power_set_errs_mask_flags(DRIVE_POWER_ERROR_IDLE_Iexc);
                    drive_protection_power_set_warn_mask_flags(DRIVE_POWER_WARNING_IDLE_Iexc);
                }
                drive.stopping_state = DRIVE_STOPPING_DONE;
                drive.iters_counter = 0;
                return true;
            }
            
            break;
        case DRIVE_STOPPING_DONE:
            return true;
    }
    
    return false;
}

/**
 * Функция останова привода выбегом.
 * @return Флаг завершения останова.
 */
static bool drive_state_process_stop_coast(void)
{
    drive_supply_off();
    return true;
}

/**
 * Обработка состояния останова привода.
 * @return Код ошибки.
 */
static err_t drive_state_process_stop(void)
{
    bool stopping_done = false;
    
    switch(drive.stopping_cur_mode){
        case DRIVE_STOP_MODE_NORMAL:
            stopping_done = drive_state_process_stop_brake(false);
            break;
        case DRIVE_STOP_MODE_FAST:
            stopping_done = drive_state_process_stop_brake(true);
            break;
        case DRIVE_STOP_MODE_COAST:
            stopping_done = drive_state_process_stop_coast();
            break;
    }
    if(stopping_done){
        drive_set_state(drive.stopping_next_state);
    }
    
    return E_NO_ERROR;
}

/**
 * Обработка состояния останова привода при ошибке.
 * @return Код ошибки.
 */
static err_t drive_state_process_stop_error(void)
{
    return drive_state_process_stop();
}

/**
 * Обработка состояния запуска привода.
 * @return Код ошибки.
 */
static err_t drive_state_process_start(void)
{
    bool wait_exc_done = false;
    
    drive_regulate();
    
    switch(drive.starting_state){
        default:
        case DRIVE_STARTING_NONE:
            return E_NO_ERROR;
        case DRIVE_STARTING_BEGIN:
            if(drive_triacs_exc_mode() != DRIVE_TRIACS_EXC_EXTERNAL){
                drive_triacs_set_exc_enabled(true);
                if(drive_triacs_exc_mode() == DRIVE_TRIACS_EXC_REGULATED){
                    drive_regulator_set_exc_enabled(true);
                }
            }
            drive_regulator_start();
            drive.starting_state = DRIVE_STARTING_WAIT_EXC;
            drive.iters_counter = 0;
            break;
        case DRIVE_STARTING_WAIT_EXC:
            if(drive_triacs_exc_mode() == DRIVE_TRIACS_EXC_EXTERNAL){
                wait_exc_done = true;
            }else if(drive_protection_power_item_allow(DRIVE_PROT_PWR_ITEM_WARN_UDF_Iexc) &&
                     drive_protection_power_item_stable(DRIVE_PROT_PWR_ITEM_WARN_UDF_Iexc)){
                wait_exc_done = true;
            }else{
                if(drive.iters_counter >= drive.settings.start_exc_iters){
                    wait_exc_done = true;
                }else{
                    drive.iters_counter ++;
                }
            }
            
            if(wait_exc_done){
                if(drive_triacs_exc_mode() != DRIVE_TRIACS_EXC_EXTERNAL){
                    drive_protection_power_set_errs_mask_flags(DRIVE_POWER_ERROR_UNDERFLOW_Iexc);
                    drive_protection_power_set_warn_mask_flags(DRIVE_POWER_WARNING_UNDERFLOW_Iexc);
                }
                drive_triacs_set_pairs_enabled(true);
                drive_regulator_set_rot_enabled(true);
                drive.starting_state = DRIVE_STARTING_RAMP;
                drive.iters_counter = 0;
            }
            
            break;
        case DRIVE_STARTING_RAMP:
            //if(drive_regulator_state() == DRIVE_REGULATOR_STATE_RUN){
                drive.starting_state = DRIVE_STARTING_DONE;
                drive_set_state(DRIVE_STATE_RUN);
            //}
            break;
        case DRIVE_STARTING_DONE:
            drive_set_state(DRIVE_STATE_RUN);
            break;
    }
    
    return E_NO_ERROR;
}

/**
 * Обработка состояния простоя(готовности) привода.
 * @return Код ошибки.
 */
static err_t drive_state_process_idle(void)
{
    return E_NO_ERROR;
}

/**
 * Производит калибровку питания.
 * @return Код ошибки.
 */
static err_t drive_state_process_power_calibration(void)
{
    switch(drive.power_calibration_state){
        default:
        case DRIVE_PWR_CALIBRATION_NONE:
            break;
        case DRIVE_PWR_CALIBRATION_BEGIN:
            drive_clear_flag(DRIVE_FLAG_POWER_CALIBRATED);
            drive.power_calibration_state = DRIVE_PWR_CALIBRATION_RUNNING;
            //drive_power_set_processing_iters(DRIVE_POWER_CALIBRATION_ITERS);
            break;
        case DRIVE_PWR_CALIBRATION_RUNNING:
            //if(drive_power_new_data_avail(DRIVE_POWER_CHANNELS)){
                drive_power_calibrate(DRIVE_POWER_CHANNELS);
                //drive_power_set_processing_iters(DRIVE_POWER_CALCULATION_ITERS);
                drive_set_flag(DRIVE_FLAG_POWER_CALIBRATED);
                drive.power_calibration_state = DRIVE_PWR_CALIBRATION_DONE;
                drive_update_clibration_parameters();
                //drive_set_state(DRIVE_STATE_IDLE);
                drive_restore_state();
            //}
            break;
        case DRIVE_PWR_CALIBRATION_DONE:
            drive_set_state(DRIVE_STATE_IDLE);
            break;
    }
    
    return E_NO_ERROR;
}

/**
 * Обработка состояния начальной инициализации привода.
 * @return Код ошибки.
 */
static err_t drive_state_process_init(void)
{
    switch(drive.init_state){
        case DRIVE_INIT_NONE:
            break;
        case DRIVE_INIT_BEGIN:
            drive_phase_state_reset();
            drive_phase_sync_reset();
            //drive_power_reset();
            //drive_power_reset_channels(DRIVE_POWER_CHANNELS);
            drive_clear_flag(DRIVE_READY_FLAGS);
            drive.init_state = DRIVE_INIT_WAIT_PHASES;
            drive.iters_counter = 0;
            break;
            
        case DRIVE_INIT_WAIT_PHASES:
            // Если ошибка фазы.
            if(drive_phase_state_errors() != PHASE_NO_ERROR){
                drive_phase_state_clear_errors();
                drive.iters_counter = 0;
                break;
            }
            
            if(drive_phase_state_direction() == DRIVE_DIR_UNK){
                drive.iters_counter = 0;
                break;
            }
            
            if(!drive_phase_sync_synchronized()){
                drive.iters_counter = 0;
                break;
            }
            
            if(++ drive.iters_counter >= drive.settings.check_phases_iters) {
                drive.init_state = DRIVE_INIT_WAIT_POWER;
            }
            
            break;
            
        case DRIVE_INIT_WAIT_POWER:
            if(drive_power_new_data_avail(DRIVE_POWER_CHANNELS)){
                drive.init_state = DRIVE_INIT_DONE;
                drive_set_state(DRIVE_STATE_IDLE);
            }
            break;
        case DRIVE_INIT_DONE:
            drive_set_state(DRIVE_STATE_IDLE);
            break;
    }
    return E_NO_ERROR;
}

/**
 * Обработка состояний привода.
 * @return Код ошибку.
 */
static err_t drive_states_process(void)
{
    switch(drive.state){
        case DRIVE_STATE_INIT:
            drive_state_process_init();
            break;
        case DRIVE_STATE_CALIBRATION:
            drive_state_process_power_calibration();
            break;
        case DRIVE_STATE_IDLE:
            drive_state_process_idle();
            break;
        case DRIVE_STATE_START:
            drive_state_process_start();
            break;
        case DRIVE_STATE_RUN:
            drive_state_process_running();
            break;
        case DRIVE_STATE_STOP:
            drive_state_process_stop();
            break;
        case DRIVE_STATE_STOP_ERROR:
            drive_state_process_stop_error();
            break;
        case DRIVE_STATE_ERROR:
            drive_state_process_error();
            break;
    }
    
    drive_process_digital_inputs();
    
    drive_update_digital_outputs();
    
    drive_update_virtual_parameters();
    
    drive_update_digital_state_parameters();
    
    drive_update_pid_parameters();
    
    return E_NO_ERROR;
}

static fixed32_t drive_get_null_timer_angle(void)
{
    if(!drive.tim_null) return 0;
    
    uint32_t tim_null_ticks = TIM_GetCounter(drive.tim_null);
    
    // time [0...40000) -> [0...120).
    // 40000:120 == 1000:3
    
    fixed32_t res = (fixed32_t)fixed32_make_from_fract((uint64_t)(tim_null_ticks * 3), 1000);
    
    return res;
}

static int16_t drive_get_null_timer_time(void)
{
    if(!drive.tim_null) return 0;
    
    uint16_t tim_null_time = TIM_GetCounter(drive.tim_null);
    
    // time [0...40000) -> [0...6666.(6)).
    // 40000:6666.(6) == 6:1
    
    int16_t res = (int16_t)(tim_null_time / 6);
    
    return res;
}


/*
 * Интерфейсный функции.
 * Документация в drive.h
 */

err_t drive_init(void)
{
    memset(&drive, 0x0, sizeof(drive_t));
    
    drive_phase_sync_init();
    drive_phase_sync_set_angle_callback(drive_get_null_timer_angle);
    
    drive_phase_state_init();
    //drive_phase_state_set_error_callback(drive_on_phase_error_occured);
    
    drive_regulator_init();
    
    drive_triacs_init();
    
    drive_power_init();
    drive_power_set_processing_iters(DRIVE_POWER_CALCULATION_ITERS);
    
    drive_protection_init();
    
    drive_dio_init();
    
    drive_phase_sync_pll_pid_clamp(
            -fixed32_make_from_int(DRIVE_NULL_TIMER_OFFSET_TICKS_MAX),
            fixed32_make_from_int(DRIVE_NULL_TIMER_OFFSET_TICKS_MAX)
            );
    
    drive_motor_init();
    
    drive_update_settings();
    
    drive_update_prot_masks();
    
    drive.params.param_u_a = settings_param_by_id(PARAM_ID_POWER_U_A);
    drive.params.param_u_b = settings_param_by_id(PARAM_ID_POWER_U_B);
    drive.params.param_u_c = settings_param_by_id(PARAM_ID_POWER_U_C);
    drive.params.param_u_rot = settings_param_by_id(PARAM_ID_POWER_U_ROT);
    drive.params.param_i_a = settings_param_by_id(PARAM_ID_POWER_I_A);
    drive.params.param_i_b = settings_param_by_id(PARAM_ID_POWER_I_B);
    drive.params.param_i_c = settings_param_by_id(PARAM_ID_POWER_I_C);
    drive.params.param_i_rot = settings_param_by_id(PARAM_ID_POWER_I_ROT);
    drive.params.param_i_exc = settings_param_by_id(PARAM_ID_POWER_I_EXC);
    drive.params.param_i_ref = settings_param_by_id(PARAM_ID_POWER_I_REF);
    drive.params.param_i_fan = settings_param_by_id(PARAM_ID_POWER_I_FAN);
    drive.params.param_calc_u_rot = settings_param_by_id(PARAM_ID_POWER_CALC_U_ROT);
    drive.params.param_pairs_angle = settings_param_by_id(PARAM_ID_TRIACS_PAIRS_OPEN_ANGLE);
    drive.params.param_exc_angle = settings_param_by_id(PARAM_ID_TRIAC_EXC_OPEN_ANGLE);
    
    // цифровые входа
    drive.params.param_digital_in_1 = settings_param_by_id(PARAM_ID_DIGITAL_IN_1_STATE);
    drive.params.param_digital_in_2 = settings_param_by_id(PARAM_ID_DIGITAL_IN_2_STATE);
    drive.params.param_digital_in_3 = settings_param_by_id(PARAM_ID_DIGITAL_IN_3_STATE);
    drive.params.param_digital_in_4 = settings_param_by_id(PARAM_ID_DIGITAL_IN_4_STATE);
    drive.params.param_digital_in_5 = settings_param_by_id(PARAM_ID_DIGITAL_IN_5_STATE);
    // цифровые выхода
    drive.params.param_digital_out_1 = settings_param_by_id(PARAM_ID_DIGITAL_OUT_1_STATE);
    drive.params.param_digital_out_2 = settings_param_by_id(PARAM_ID_DIGITAL_OUT_2_STATE);
    drive.params.param_digital_out_3 = settings_param_by_id(PARAM_ID_DIGITAL_OUT_3_STATE);
    drive.params.param_digital_out_4 = settings_param_by_id(PARAM_ID_DIGITAL_OUT_4_STATE);
    
    drive.params.param_pid_rot_spd_val = settings_param_by_id(PARAM_ID_PID_ROT_SPEED);
    drive.params.param_pid_rot_cur_val = settings_param_by_id(PARAM_ID_PID_ROT_CURRENT);
    drive.params.param_pid_exc_cur_val = settings_param_by_id(PARAM_ID_PID_EXC_CURRENT);
    
    //drive_set_state(DRIVE_STATE_INIT);
    drive.init_state = DRIVE_INIT_BEGIN;
    
    return E_NO_ERROR;
}

err_t drive_update_settings(void)
{
    drive_protection_update_settings();

    drive_power_set_phase_calc_current((phase_t)settings_valueu(PARAM_ID_CALC_PHASE_CURRENT));
    drive_power_set_phase_calc_voltage((phase_t)settings_valueu(PARAM_ID_CALC_PHASE_VOLTAGE));
    drive_power_set_rot_calc_current(settings_valueu(PARAM_ID_CALC_ROT_CURRENT));
    drive_power_set_rot_calc_voltage(settings_valueu(PARAM_ID_CALC_ROT_VOLTAGE));
    drive_power_set_exc_calc_current(settings_valueu(PARAM_ID_CALC_EXC_CURRENT));
    
    drive.settings.stop_mode = settings_valueu(PARAM_ID_RAMP_STOP_MODE);
    drive.settings.stop_rot_iters = settings_valueu(PARAM_ID_ROT_STOP_TIME) * DRIVE_NULL_TIMER_FREQ;
    drive.settings.stop_exc_iters = settings_valueu(PARAM_ID_EXC_STOP_TIME) * DRIVE_NULL_TIMER_FREQ;
    drive.settings.start_exc_iters = settings_valueu(PARAM_ID_EXC_START_TIME) * DRIVE_NULL_TIMER_FREQ;
    drive.settings.check_phases_iters =
            settings_valueu(PARAM_ID_PHASES_CHECK_TIME) * DRIVE_NULL_TIMER_FREQ / 1000;
    drive.settings.zero_sensor_time = settings_valueu(PARAM_ID_ZERO_SENSOR_TIME);
    
    drive_triacs_set_exc_mode(settings_valueu(PARAM_ID_EXC_MODE));
    drive_triacs_set_pairs_open_time_us(settings_valueu(PARAM_ID_TRIACS_PAIRS_OPEN_TIME));
    drive_triacs_set_exc_open_time_us(settings_valueu(PARAM_ID_TRIAC_EXC_OPEN_TIME));
    drive_triacs_set_exc_phase(settings_valueu(PARAM_ID_EXC_PHASE));
    
    drive_regulator_set_mode(settings_valueu(PARAM_ID_REGULATOR_MODE));
    
    drive_regulator_set_reference_time(settings_valuef(PARAM_ID_RAMP_REFERENCE_TIME));
    drive_regulator_set_start_time(settings_valuef(PARAM_ID_RAMP_START_TIME));
    drive_regulator_set_stop_time(settings_valuef(PARAM_ID_RAMP_STOP_TIME));
    drive_regulator_set_fast_stop_time(settings_valuef(PARAM_ID_RAMP_FAST_STOP_TIME));
    
    drive_regulator_set_rot_nom_voltage(settings_valuef(PARAM_ID_MOTOR_U_ROT_NOM));
    drive_regulator_set_rot_nom_current(settings_valuef(PARAM_ID_MOTOR_I_ROT_NOM));
    drive_regulator_set_exc_current(settings_valuef(PARAM_ID_I_EXC));
    drive_regulator_set_rot_pid(settings_valuef(PARAM_ID_ROT_PID_K_P),
                                settings_valuef(PARAM_ID_ROT_PID_K_I),
                                settings_valuef(PARAM_ID_ROT_PID_K_D));
    drive_regulator_set_exc_pid(settings_valuef(PARAM_ID_EXC_PID_K_P),
                                settings_valuef(PARAM_ID_EXC_PID_K_I),
                                settings_valuef(PARAM_ID_EXC_PID_K_D));
    drive_regulator_set_spd_pid(settings_valuef(PARAM_ID_SPD_PID_K_P),
                                settings_valuef(PARAM_ID_SPD_PID_K_I),
                                settings_valuef(PARAM_ID_SPD_PID_K_D));
    
    fixed32_t rot_angle_min = settings_valuef(PARAM_ID_TRIACS_PAIRS_ANGLE_MIN);
    fixed32_t rot_angle_max = settings_valuef(PARAM_ID_TRIACS_PAIRS_ANGLE_MAX);
    fixed32_t exc_angle_min = settings_valuef(PARAM_ID_TRIAC_EXC_ANGLE_MIN);
    fixed32_t exc_angle_max = settings_valuef(PARAM_ID_TRIAC_EXC_ANGLE_MAX);
    
    drive_triacs_clamp_pairs_open_angle(rot_angle_min, rot_angle_max);
    drive_triacs_clamp_exc_open_angle(exc_angle_min, exc_angle_max);
    
    drive_regulator_spd_pid_clamp(DRIVE_SPD_PID_VALUE_MIN, settings_valuef(PARAM_ID_MOTOR_I_ROT_NOM));
    drive_regulator_rot_pid_clamp(rot_angle_min, rot_angle_max);
    drive_regulator_exc_pid_clamp(exc_angle_min, exc_angle_max);
    
    drive_phase_sync_set_pll_pid(settings_valuef(PARAM_ID_PHASE_SYNC_PLL_PID_K_P),
                                 settings_valuef(PARAM_ID_PHASE_SYNC_PLL_PID_K_I),
                                 settings_valuef(PARAM_ID_PHASE_SYNC_PLL_PID_K_D));
    drive_phase_sync_set_accuracy(settings_valuef(PARAM_ID_PHASE_SYNC_ACCURACY));
    
    drive_motor_update_settings();
    
    drive_power_update_settings();
    
    drive_dio_input_setup(DRIVE_DIO_INPUT_1, settings_valueu(PARAM_ID_DIGITAL_IN_1_TYPE),
                                             settings_valueu(PARAM_ID_DIGITAL_IN_1_INVERSION));
    drive_dio_input_setup(DRIVE_DIO_INPUT_2, settings_valueu(PARAM_ID_DIGITAL_IN_2_TYPE),
                                             settings_valueu(PARAM_ID_DIGITAL_IN_2_INVERSION));
    drive_dio_input_setup(DRIVE_DIO_INPUT_3, settings_valueu(PARAM_ID_DIGITAL_IN_3_TYPE),
                                             settings_valueu(PARAM_ID_DIGITAL_IN_3_INVERSION));
    drive_dio_input_setup(DRIVE_DIO_INPUT_4, settings_valueu(PARAM_ID_DIGITAL_IN_4_TYPE),
                                             settings_valueu(PARAM_ID_DIGITAL_IN_4_INVERSION));
    drive_dio_input_setup(DRIVE_DIO_INPUT_5, settings_valueu(PARAM_ID_DIGITAL_IN_5_TYPE),
                                             settings_valueu(PARAM_ID_DIGITAL_IN_5_INVERSION));
    
    drive_dio_output_setup(DRIVE_DIO_OUTPUT_1, settings_valueu(PARAM_ID_DIGITAL_OUT_1_TYPE),
                                               settings_valueu(PARAM_ID_DIGITAL_OUT_1_INVERSION));
    drive_dio_output_setup(DRIVE_DIO_OUTPUT_2, settings_valueu(PARAM_ID_DIGITAL_OUT_2_TYPE),
                                               settings_valueu(PARAM_ID_DIGITAL_OUT_2_INVERSION));
    drive_dio_output_setup(DRIVE_DIO_OUTPUT_3, settings_valueu(PARAM_ID_DIGITAL_OUT_3_TYPE),
                                               settings_valueu(PARAM_ID_DIGITAL_OUT_3_INVERSION));
    drive_dio_output_setup(DRIVE_DIO_OUTPUT_4, settings_valueu(PARAM_ID_DIGITAL_OUT_4_TYPE),
                                               settings_valueu(PARAM_ID_DIGITAL_OUT_4_INVERSION));
    
    drive_dio_set_deadtime(settings_valuef(PARAM_ID_DIGITAL_IO_DEADTIME_MS));
    
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
    return ((drive.power_errors | drive.power_cutoff_errors) & error) != 0;
}

drive_power_errors_t drive_power_errors(void)
{
    return drive.power_errors | drive.power_cutoff_errors;
}

bool drive_power_warning(drive_power_warning_t warning)
{
    return (drive.power_warnings & warning) != 0;
}

drive_power_warnings_t drive_power_warnings(void)
{
    return drive.power_warnings;
}

/*drive_phase_angle_errors_t drive_phase_angle_errors(void)
{
    return drive.phase_angle_errors;
}

drive_phase_angle_warnings_t drive_phase_angle_warnings(void)
{
    return drive.phase_angle_warnings;
}*/

drive_init_state_t drive_init_state(void)
{
    return drive.init_state;
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

bool drive_ok(void)
{
    return drive_get_state() != DRIVE_STATE_ERROR &&
           drive_protection_top_ready() &&
           drive_motor_ready();
}

bool drive_ready(void)
{
    return (drive.errors == 0) &&
           (drive.state != DRIVE_STATE_INIT) &&
            drive_flags_is_set(DRIVE_READY_FLAGS) &&
            drive_protection_top_ready() &&
            drive_motor_ready();
}

bool drive_start(void)
{
    if(!drive_ready())
        return false;
    if(drive.state == DRIVE_STATE_IDLE ||
       drive.state == DRIVE_STATE_STOP){
        drive_set_state(DRIVE_STATE_START);
    }
    return true;
}

bool drive_stop(void)
{
    if(drive.state == DRIVE_STATE_RUN ||
       drive.state == DRIVE_STATE_START){
        switch(drive.settings.stop_mode){
            case DRIVE_STOP_MODE_NORMAL:
                drive_stop_normal();
                break;
            case DRIVE_STOP_MODE_FAST:
                drive_stop_fast();
                break;
            case DRIVE_STOP_MODE_COAST:
                drive_stop_coast();
                break;
        }
    }
    return true;
}

bool drive_emergency_stop(void)
{
    if(!(drive.errors & DRIVE_ERROR_EMERGENCY_STOP)){
        
        drive_prot_action_t action = drive_protection_emergency_stop_action();
        
        if(action != DRIVE_PROT_ACTION_IGNORE) drive_set_error(DRIVE_ERROR_EMERGENCY_STOP);
        
        if(drive_prot_event_by_action(action)){
            drive_prot_error_stop_by_action(action);
            return true;
        }
    }
    return false;
}

bool drive_calibrate_power(void)
{
    if(drive.state == DRIVE_STATE_IDLE ||
       drive.state == DRIVE_STATE_ERROR){
        drive_save_state();
        drive_set_state(DRIVE_STATE_CALIBRATION);
        
        return true;
    }
    return false;
}

bool drive_running(void)
{
    return drive.status == DRIVE_STATUS_RUN;
}

void drive_clear_errors(void)
{
    drive.errors = DRIVE_ERROR_NONE;
    drive.power_errors = DRIVE_POWER_ERROR_NONE;
    drive.warnings = DRIVE_WARNING_NONE;
    drive.power_warnings = DRIVE_POWER_WARNING_NONE;
    drive.power_cutoff_errors = DRIVE_POWER_ERROR_NONE;
    
    //drive.phase_angle_errors = DRIVE_PHASE_ANGLE_NO_ERROR;
    //drive.phase_angle_warnings = DRIVE_PHASE_ANGLE_NO_WARNING;
    
    drive_protection_power_clear_errors();
    drive_phase_state_clear_errors();
    drive_protection_clear_errors();
    
    if(drive.state == DRIVE_STATE_ERROR){
        drive_set_state(DRIVE_STATE_INIT);
    }
}

drive_error_callback_t drive_error_callback(void)
{
    return drive.on_error_occured;
}

void drive_set_error_callback(drive_error_callback_t callback)
{
    drive.on_error_occured = callback;
}

drive_warning_callback_t drive_warning_callback(void)
{
    return drive.on_warning_occured;
}

void drive_set_warning_callback(drive_warning_callback_t callback)
{
    drive.on_warning_occured = callback;
}

err_t drive_set_null_timer(TIM_TypeDef* TIM)
{
    if(TIM == NULL) return E_NULL_POINTER;
    
    drive.tim_null = TIM;
    
    return E_NO_ERROR;
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

/**
 * Обрабатывает действительно сработавший датчик нуля.
 * @param phase Фаза.
 * @return Код ошибки.
 */
static err_t drive_process_null_sensor_impl(phase_t phase, int16_t sensor_time)
{
    drive_phase_state_handle(phase);

    if(drive_can_open_triacs()){
        drive_setup_triacs_open(phase, sensor_time);
    }
    /*else{
        drive_tasks_write_status_event();
    }*/
    
    return E_NO_ERROR;
}

#ifdef USE_ZERO_SENSORS

#ifdef DRIVE_PHASE_SYNC_DEBUG
static int32_t zero_sens = 0;
#endif

err_t drive_process_null_sensor(phase_t phase, null_sensor_edge_t edge)
{
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    
    static phase_time_t sensors_time[PHASES_COUNT] = {0};
    static null_sensor_edge_t sensors_edges[PHASES_COUNT] = {0};
    
    phase_time_t time = drive_phase_state_time();
    phase_time_t prev_time = sensors_time[phase - 1];
    null_sensor_edge_t prev_edge = sensors_edges[phase - 1];
    
    if(edge == NULL_SENSOR_EDGE_TRAILING && prev_edge != edge){
        
        if(!drive_phase_state_has_time()){
            return E_NO_ERROR;
            //return drive_process_null_sensor_impl(phase, 0);
        }
        
        if(time >= prev_time){
            phase_time_t diff_time = time - prev_time;
#ifdef DRIVE_PHASE_SYNC_DEBUG
            zero_sens = diff_time;
#endif
            if(diff_time >= drive.settings.zero_sensor_time){
                //return E_NO_ERROR;
                return drive_process_null_sensor_impl(phase, diff_time);
            }
        }
    }
    
    sensors_edges[phase - 1] = edge;
    sensors_time[phase - 1] = time;
    
    return E_NO_ERROR;
}

#endif //USE_ZERO_SENSORS

#ifdef DRIVE_PHASE_SYNC_DEBUG
static int32_t angle_phA = 0;
static int32_t angle_phB = 0;
static int32_t angle_phC = 0;
static int32_t angle_pid_val = 0;
#endif

void drive_null_timer_irq_handler(void)
{
    if(drive.tim_null) TIM_ClearITPendingBit(drive.tim_null, TIM_IT_Update);
    
    phase_t phase = drive_phase_sync_next_phase();
    if(phase != PHASE_UNK){
        
        int16_t offset = 0;
        
        offset += drive_phase_sync_offset(phase);
        offset += drive_get_null_timer_time();
        
        drive_process_null_sensor_impl(phase, offset);
    }
    
    if(drive_phase_sync_process()){
    
        if(drive.tim_null){

            fixed32_t period_deltaf = drive_phase_sync_pll_pid_value();
            int16_t period_delta = (int16_t)fixed32_get_int(period_deltaf);

            drive.tim_null->ARR = DRIVE_NULL_TIMER_CNT_PERIOD - period_delta;
            
#ifdef DRIVE_PHASE_SYNC_DEBUG
            angle_pid_val = period_delta;
#endif
        }
    }
    
#ifdef DRIVE_PHASE_SYNC_DEBUG
    fixed32_t tmp_val = 0;
    drive_phase_sync_angle(PHASE_A, &tmp_val); angle_phA = (tmp_val);//fixed32_get_int
    drive_phase_sync_angle(PHASE_B, &tmp_val); angle_phB = (tmp_val);//fixed32_get_int
    drive_phase_sync_angle(PHASE_C, &tmp_val); angle_phC = (tmp_val);//fixed32_get_int
#endif
    
    if(drive_calculate_power() && drive_flags_is_set(DRIVE_FLAG_POWER_DATA_AVAIL)){
        drive_process_top();
        drive_check_prots();
    }
    
    drive_states_process();
}

err_t drive_process_power_adc_values(power_channels_t channels, uint16_t* adc_values)
{
    err_t err = drive_power_process_adc_values(channels, adc_values);
    
    drive_check_power_inst();
    
    drive_phase_sync_append_data();
    
    return err;
}

bool drive_calculate_power(void)
{
    err_t err = E_NO_ERROR;
    if(drive_power_calc_values(DRIVE_POWER_CHANNELS, &err)){

        if(err == E_NO_ERROR && drive_power_data_avail(DRIVE_POWER_CHANNELS)){
            
            drive_set_flag(DRIVE_FLAG_POWER_DATA_AVAIL);
            drive_motor_calculate();
            
        }else{
            drive_clear_flag(DRIVE_FLAG_POWER_DATA_AVAIL);
            drive_error_occured(DRIVE_ERROR_POWER_DATA_NOT_AVAIL);
        }
        return true;
    }
    return false;
}
