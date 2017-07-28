#include "drive_protection.h"
#include <string.h>
#include "utils/utils.h"
#include "settings.h"
#include "drive_power.h"
#include "drive_phase_state.h"
#include "drive.h"
#include "drive_phase_sync.h"
#include "drive_temp.h"
//#include "defs/defs.h"



//! Тип базового элемента защиты.
typedef struct _Drive_Prot_Base_Item {
    bool enabled; //!< Разрешение элемента защиты.
    fixed32_t pie_inc; //!< Изменение накопления за итерацию.
    fixed32_t pie; //!< Текущее значение накопления срабатывания защиты.
    bool allow; //!< Флаг допустимого значение питания элемента защиты.
    bool active; //!< Флаг активации элемента защиты (недопустимое значение элемента защиты).
    bool latch_enabled; //!< Разрешение защёлки.
    bool hold_value; //!< Значение защёлки.
    drive_prot_action_t action; //!< Действие.
} drive_prot_base_item_t;


#define DRIVE_PROT_ITEM_ALLOW_DEFAULT false
#define DRIVE_PROT_ITEM_ACTIVE_DEFAULT false


#define DRIVE_PROT_PIE_MAX 0x10000
#define DRIVE_PROT_CALC_PIE_INC(time) (0x6aac1 / time) //6.667 * f32(1.0) / t_avail
#define DRIVE_PROT_CUTOFF_PIE_INC (DRIVE_PROT_PIE_MAX / 8)
#define DRIVE_PROT_SENSOR_PIE_INC (DRIVE_PROT_PIE_MAX / 4)


//! Перегруз по току, при котором задано время срабатывания.
#define DRIVE_TOP_NOMINAL_OVERCURRENT 6

//! Структура тепловой защиты.
typedef struct _Drive_TOP {
    bool enabled; //!< Разрешение тепловой защиты.
    fixed32_t k_pie_max; //!< Коэффициент зависимости времени срабатывания от перегрузки по току.
    fixed32_t cur_pie; //!< Текущее тепло двигателя.
    bool overheat; //!< Флаг перегрева.
    drive_prot_action_t action; //!< Действие при срабатывании.
} drive_top_t;

//! Структура защиты вентилятора.
typedef struct _Drive_Fan_Prot {
    bool enabled; //!< Разрешение защиты вентилятора.
    drive_prot_base_item_t item_idle; //!< Элемент нулевого тока.
    drive_prot_base_item_t item_ovf; //!< Элемент превышения тока.
    drive_prot_base_item_t item_udf; //!< Элемент понижения тока.
    fixed32_t I_fan_nom; //!< Номинальный ток вентилятора.
    fixed32_t I_fan_zero_noise; //!< Шум нуля тока вентилятора.
    fixed32_t I_fan_ovf; //!< Уровень срабатывания защиты по превышению тока.
} drive_fan_prot_t;

//! Структура защиты по температуре радиатора.
typedef struct _Drive_Temp_Prot {
    bool warn_enabled; //!< Разрешение предупреждения.
    bool fault_enabled; //!< Разрешение ошибки.
} drive_temp_prot_t;

//! Структура защиты тиристоров.
typedef struct _Drive_Triacs_Prot {
    fixed32_t min_angle; //!< Минимальный угол контроля открытия тиристоров.
    fixed32_t min_current; //!< Минимальный ток контроля открытия тиристоров.
} drive_triacs_prot_t;

//! Структура защиты датчика.
typedef struct _Drive_Sensor_Prot {
    uint16_t adc_range_min; //!< Минимальное допустимое значение АЦП.
    uint16_t adc_range_max; //!< Максимальное допустимое значение АЦП.
    bool emulation_enabled; //!< Разрешение программного вычисления.
} drive_sensor_prot_t;

//! Структура защиты датчиков.
typedef struct _Drive_Sensors_Prot {
    drive_sensor_prot_t sensor_u_in; //!< Датчики напряжения фаз.
    drive_sensor_prot_t sensor_u_rot; //!< Датчик напряжения якоря.
    drive_sensor_prot_t sensor_i_in; //!< Датчики тока фаз.
    drive_sensor_prot_t sensor_i_rot; //!< Датчик тока якоря.
    drive_sensor_prot_t sensor_i_exc; //!< Датчик тока возбуждения.
} drive_sensors_prot_t;

/*
 * Защита питания.
 */

//! Тип сравнения значений защиты.
typedef enum _Drive_Prot_Type {
    DRIVE_PROT_TYPE_OVF  = 0, //!< Защита от повышенного значения.
    DRIVE_PROT_TYPE_UDF  = 1, //!< Защита от пониженного значения.
    DRIVE_PROT_TYPE_ZERO = 2, //!< Защита от наличия ненулевого значения.
    DRIVE_PROT_TYPE_CUT  = 3, //!< Отсечка.
} drive_protection_type_t;

//! Тип флага элемента защиты.
typedef enum _Drive_Prot_Flag {
    DRIVE_PROT_FLAG_WRN = 0, //!< Предупреждение.
    DRIVE_PROT_FLAG_ERR = 1 //!< Ошибка.
} drive_protection_flag_t;

//! Тип элемента защиты питания.
typedef struct _Drive_Prot_Power_Descr {
    size_t power_channel; //!< Номер канала питания.
    drive_protection_type_t type; //!< Тип защиты.
    drive_protection_flag_t flag_type; //!< Тип флага.
    uint32_t flag; //!< Флаг при срабатывании защиты.
    param_id_t param_ref; //!< Опорный параметр.
    param_id_t param_ena; //!< Параметр разрешения защиты.
    param_id_t param_level; //!< Параметр уровня срабатывания защиты.
    param_id_t param_time; //!< Параметр времени срабатывания защиты.
    param_id_t param_latch_ena; //!< Параметр разрешения защёлки.
    param_id_t param_action; //!< Параметр действия.
} drive_protection_power_descr_t;

//! Тип элемента защиты питания.
typedef struct _Drive_Prot_Power_Item {
    drive_prot_base_item_t base_item; //!< Базовый элемент защиты.
    fixed32_t value_level; //!< Значение уровня срабатывания защиты.
} drive_protection_power_item_t;

#define PROT_PWR_DESCR(arg_pwr_ch, arg_type, arg_flag_type, arg_flag,\
                       arg_par_ref, arg_par_ena, arg_par_lvl, arg_par_time,\
                       arg_par_lch_ena, arg_par_act)\
    {\
        .power_channel = arg_pwr_ch, .type = arg_type, .flag_type = arg_flag_type, .flag = arg_flag,\
        .param_ref = arg_par_ref, .param_ena = arg_par_ena, .param_level = arg_par_lvl, .param_time = arg_par_time,\
        .param_latch_ena = arg_par_lch_ena, .param_action = arg_par_act\
    }


static const drive_protection_power_descr_t drive_prot_power_items_descrs[DRIVE_PROT_PWR_ITEMS_COUNT] = {
    PROT_PWR_DESCR(DRIVE_POWER_Ua, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ua, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_OVF_FAULT_ENABLE, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_IN_OVF_FAULT_ACTION),
    PROT_PWR_DESCR(DRIVE_POWER_Ua, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_UNDERFLOW_Ua, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_UDF_FAULT_ENABLE, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_UDF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_IN_UDF_FAULT_ACTION),
    
    PROT_PWR_DESCR(DRIVE_POWER_Ua, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Ua, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_OVF_WARN_ENABLE, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_IN_OVF_WARN_ACTION),
    PROT_PWR_DESCR(DRIVE_POWER_Ua, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_UNDERFLOW_Ua, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_UDF_WARN_ENABLE, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_UDF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_IN_UDF_WARN_ACTION),
    
    
    PROT_PWR_DESCR(DRIVE_POWER_Ub, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ub, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_OVF_FAULT_ENABLE, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_IN_OVF_FAULT_ACTION),
    PROT_PWR_DESCR(DRIVE_POWER_Ub, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_UNDERFLOW_Ub, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_UDF_FAULT_ENABLE, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_UDF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_IN_UDF_FAULT_ACTION),
    
    PROT_PWR_DESCR(DRIVE_POWER_Ub, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Ub, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_OVF_WARN_ENABLE, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_IN_OVF_WARN_ACTION),
    PROT_PWR_DESCR(DRIVE_POWER_Ub, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_UNDERFLOW_Ub, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_UDF_WARN_ENABLE, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_UDF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_IN_UDF_WARN_ACTION),
    
    
    PROT_PWR_DESCR(DRIVE_POWER_Uc, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Uc, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_OVF_FAULT_ENABLE, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_IN_OVF_FAULT_ACTION),
    PROT_PWR_DESCR(DRIVE_POWER_Uc, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_UNDERFLOW_Uc, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_UDF_FAULT_ENABLE, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_UDF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_IN_UDF_FAULT_ACTION),
    
    PROT_PWR_DESCR(DRIVE_POWER_Uc, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Uc, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_OVF_WARN_ENABLE, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_IN_OVF_WARN_ACTION),
    PROT_PWR_DESCR(DRIVE_POWER_Uc, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_UNDERFLOW_Uc, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_UDF_WARN_ENABLE, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_UDF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_IN_UDF_WARN_ACTION),
    
     
    PROT_PWR_DESCR(DRIVE_POWER_Ia, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ia, PARAM_ID_I_NOM,\
        PARAM_ID_PROT_I_IN_OVF_FAULT_ENABLE, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_IN_OVF_FAULT_ACTION),
    
    PROT_PWR_DESCR(DRIVE_POWER_Ia, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Ia, PARAM_ID_I_NOM,\
        PARAM_ID_PROT_I_IN_OVF_WARN_ENABLE, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_IN_OVF_WARN_ACTION),
    
    
    PROT_PWR_DESCR(DRIVE_POWER_Ib, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ib, PARAM_ID_I_NOM,\
        PARAM_ID_PROT_I_IN_OVF_FAULT_ENABLE, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_IN_OVF_FAULT_ACTION),
    
    PROT_PWR_DESCR(DRIVE_POWER_Ib, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Ib, PARAM_ID_I_NOM,\
        PARAM_ID_PROT_I_IN_OVF_WARN_ENABLE, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_IN_OVF_WARN_ACTION),
    
    
    PROT_PWR_DESCR(DRIVE_POWER_Ic, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ic, PARAM_ID_I_NOM,\
        PARAM_ID_PROT_I_IN_OVF_FAULT_ENABLE, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_IN_OVF_FAULT_ACTION),
    
    PROT_PWR_DESCR(DRIVE_POWER_Ic, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Ic, PARAM_ID_I_NOM,\
        PARAM_ID_PROT_I_IN_OVF_WARN_ENABLE, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_IN_OVF_WARN_ACTION),
    
    
    PROT_PWR_DESCR(DRIVE_POWER_Urot, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Urot, PARAM_ID_MOTOR_U_ROT_NOM,\
        PARAM_ID_PROT_U_ROT_OVF_FAULT_ENABLE, PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_ROT_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_ROT_OVF_FAULT_ACTION),
    PROT_PWR_DESCR(DRIVE_POWER_Urot, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Urot, PARAM_ID_MOTOR_U_ROT_NOM,\
        PARAM_ID_PROT_U_ROT_OVF_WARN_ENABLE, PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_ROT_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_ROT_OVF_WARN_ACTION),
    
    
    PROT_PWR_DESCR(DRIVE_POWER_Irot, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Irot, PARAM_ID_MOTOR_I_ROT_NOM,\
        PARAM_ID_PROT_I_ROT_OVF_FAULT_ENABLE, PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_ROT_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_ROT_OVF_FAULT_ACTION),
    PROT_PWR_DESCR(DRIVE_POWER_Irot, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Irot, PARAM_ID_MOTOR_I_ROT_NOM,\
        PARAM_ID_PROT_I_ROT_OVF_WARN_ENABLE, PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_ROT_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_ROT_OVF_WARN_ACTION),
    
    
    PROT_PWR_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Iexc, PARAM_ID_I_EXC,\
        PARAM_ID_PROT_I_EXC_OVF_FAULT_ENABLE, PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_EXC_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_EXC_OVF_FAULT_ACTION),
    PROT_PWR_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_UNDERFLOW_Iexc, PARAM_ID_I_EXC,\
        PARAM_ID_PROT_I_EXC_UDF_FAULT_ENABLE, PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_EXC_UDF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_EXC_UDF_FAULT_ACTION),
    PROT_PWR_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Iexc, PARAM_ID_I_EXC,\
        PARAM_ID_PROT_I_EXC_OVF_WARN_ENABLE, PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_EXC_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_EXC_OVF_WARN_ACTION),
    PROT_PWR_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_UNDERFLOW_Iexc, PARAM_ID_I_EXC,\
        PARAM_ID_PROT_I_EXC_UDF_WARN_ENABLE, PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_EXC_UDF_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_EXC_UDF_WARN_ACTION),
    
     
    PROT_PWR_DESCR(DRIVE_POWER_Ia, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_IDLE_Ia, 0,\
        PARAM_ID_PROT_I_IN_IDLE_FAULT_ENABLE, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_IDLE_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_IN_IDLE_FAULT_ACTION),
    
    PROT_PWR_DESCR(DRIVE_POWER_Ia, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_IDLE_Ia, 0,\
        PARAM_ID_PROT_I_IN_IDLE_WARN_ENABLE, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_IDLE_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_IN_IDLE_WARN_ACTION),
    
     
    PROT_PWR_DESCR(DRIVE_POWER_Ib, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_IDLE_Ib, 0,\
        PARAM_ID_PROT_I_IN_IDLE_FAULT_ENABLE, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_IDLE_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_IN_IDLE_FAULT_ACTION),
    
    PROT_PWR_DESCR(DRIVE_POWER_Ib, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_IDLE_Ib, 0,\
        PARAM_ID_PROT_I_IN_IDLE_WARN_ENABLE, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_IDLE_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_IN_IDLE_WARN_ACTION),
    
     
    PROT_PWR_DESCR(DRIVE_POWER_Ic, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_IDLE_Ic, 0,\
        PARAM_ID_PROT_I_IN_IDLE_FAULT_ENABLE, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_IDLE_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_IN_IDLE_FAULT_ACTION),
    
    PROT_PWR_DESCR(DRIVE_POWER_Ic, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_IDLE_Ic, 0,\
        PARAM_ID_PROT_I_IN_IDLE_WARN_ENABLE, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_IDLE_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_IN_IDLE_WARN_ACTION),
    
     
    PROT_PWR_DESCR(DRIVE_POWER_Urot, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_IDLE_Urot, 0,\
        PARAM_ID_PROT_U_ROT_IDLE_FAULT_ENABLE, PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_ROT_IDLE_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_ROT_IDLE_FAULT_ACTION),
    
    PROT_PWR_DESCR(DRIVE_POWER_Urot, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_IDLE_Urot, 0,\
        PARAM_ID_PROT_U_ROT_IDLE_WARN_ENABLE, PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_ROT_IDLE_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_ROT_IDLE_WARN_ACTION),
    
     
    PROT_PWR_DESCR(DRIVE_POWER_Irot, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_IDLE_Irot, 0,\
        PARAM_ID_PROT_I_ROT_IDLE_FAULT_ENABLE, PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_ROT_IDLE_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_ROT_IDLE_FAULT_ACTION),
    
    PROT_PWR_DESCR(DRIVE_POWER_Irot, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_IDLE_Irot, 0,\
        PARAM_ID_PROT_I_ROT_IDLE_WARN_ENABLE, PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_ROT_IDLE_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_ROT_IDLE_WARN_ACTION),
    
     
    PROT_PWR_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_IDLE_Iexc, 0,\
        PARAM_ID_PROT_I_EXC_IDLE_FAULT_ENABLE, PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_EXC_IDLE_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_EXC_IDLE_FAULT_ACTION),
    
    PROT_PWR_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_IDLE_Iexc, 0,\
        PARAM_ID_PROT_I_EXC_IDLE_WARN_ENABLE, PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_EXC_IDLE_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_EXC_IDLE_WARN_ACTION),

    
    PROT_PWR_DESCR(DRIVE_POWER_Ua, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ua, PARAM_ID_U_NOM,\
        0, PARAM_ID_PROT_U_IN_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_PWR_DESCR(DRIVE_POWER_Ub, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ub, PARAM_ID_U_NOM,\
        0, PARAM_ID_PROT_U_IN_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_PWR_DESCR(DRIVE_POWER_Uc, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Uc, PARAM_ID_U_NOM,\
        0, PARAM_ID_PROT_U_IN_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
     
    PROT_PWR_DESCR(DRIVE_POWER_Ia, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ia, PARAM_ID_I_NOM,\
        0, PARAM_ID_PROT_I_IN_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_PWR_DESCR(DRIVE_POWER_Ib, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ib, PARAM_ID_I_NOM,\
        0, PARAM_ID_PROT_I_IN_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_PWR_DESCR(DRIVE_POWER_Ic, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ic, PARAM_ID_I_NOM,\
        0, PARAM_ID_PROT_I_IN_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_PWR_DESCR(DRIVE_POWER_Urot, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Urot, PARAM_ID_MOTOR_U_ROT_NOM,\
        0, PARAM_ID_PROT_U_ROT_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_PWR_DESCR(DRIVE_POWER_Irot, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Irot, PARAM_ID_MOTOR_I_ROT_NOM,\
        0, PARAM_ID_PROT_I_ROT_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_PWR_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Iexc, PARAM_ID_MOTOR_I_EXC_NOM,\
        0, PARAM_ID_PROT_I_EXC_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
};

//#define DRIVE_PROT_ITEMS_COUNT (sizeof(drive_prot_items_descrs) / sizeof(drive_protection_descr_t))


/*
 * Общая защита.
 */

// Предварительное объявление структуры.
struct _Drive_Prot_Item;

/**
 * Тип функции проверки элемента защиты.
 * Должна возвращать допустимости
 * значения элемента защиты.
 */
typedef bool (*drive_prot_item_check_proc_t)(struct _Drive_Prot_Item* item);

//! Тип элемента защиты.
typedef struct _Drive_Prot_Descr {
    drive_prot_item_check_proc_t check_proc;
    param_id_t param_ena; //!< Параметр разрешения защиты.
    param_id_t param_level; //!< Параметр уровня срабатывания защиты.
    param_id_t param_time; //!< Параметр времени срабатывания защиты.
    param_id_t param_latch_ena; //!< Параметр разрешения защёлки.
    param_id_t param_action; //!< Параметр действия.
    drive_prot_action_t default_action; //!< Действие по-умолчанию.
} drive_protection_descr_t;

//! Тип элемента защиты.
typedef struct _Drive_Prot_Item {
    drive_prot_base_item_t base_item; //!< Базовый элемент защиты.
    bool masked; //!< Маска (разрешение срабатывания) элемента защиты.
    fixed32_t value_level; //!< Значение уровня срабатывания защиты.
} drive_protection_item_t;

/*
 * Прототипы функций проверки элементов защиты.
 */
// Проверка фаз.
static bool drive_prot_check_phase_state(drive_protection_item_t* item);
// Проверка углов фаз.
static bool drive_prot_check_phases_angles(drive_protection_item_t* item);
// Проверка синхронизации с фазами.
static bool drive_prot_check_phases_sync(drive_protection_item_t* item);
// Проверка обрыва якоря.
static bool drive_prot_check_rot_break(drive_protection_item_t* item);
// Проверка вентилятора.
static bool drive_prot_check_fan(drive_protection_item_t* item);
// Проверка температуры радиатора.
static bool drive_prot_check_heatsink_temp(drive_protection_item_t* item);
// Проверка тиристоров.
static bool drive_prot_check_triacs(drive_protection_item_t* item);
// Проверки датчиков.
// Напряжения сети.
static bool drive_prot_check_sensor_fault_u_a(drive_protection_item_t* item);
static bool drive_prot_check_sensor_warn_u_a(drive_protection_item_t* item);
static bool drive_prot_check_sensor_fault_u_b(drive_protection_item_t* item);
static bool drive_prot_check_sensor_warn_u_b(drive_protection_item_t* item);
static bool drive_prot_check_sensor_fault_u_c(drive_protection_item_t* item);
static bool drive_prot_check_sensor_warn_u_c(drive_protection_item_t* item);
// Токи сети.
static bool drive_prot_check_sensor_fault_i_a(drive_protection_item_t* item);
static bool drive_prot_check_sensor_warn_i_a(drive_protection_item_t* item);
static bool drive_prot_check_sensor_fault_i_b(drive_protection_item_t* item);
static bool drive_prot_check_sensor_warn_i_b(drive_protection_item_t* item);
static bool drive_prot_check_sensor_fault_i_c(drive_protection_item_t* item);
static bool drive_prot_check_sensor_warn_i_c(drive_protection_item_t* item);
// Напряжение якоря.
static bool drive_prot_check_sensor_fault_u_rot(drive_protection_item_t* item);
static bool drive_prot_check_sensor_warn_u_rot(drive_protection_item_t* item);
// Ток якоря.
static bool drive_prot_check_sensor_fault_i_rot(drive_protection_item_t* item);
static bool drive_prot_check_sensor_warn_i_rot(drive_protection_item_t* item);
// Ток возбуждения.
static bool drive_prot_check_sensor_fault_i_exc(drive_protection_item_t* item);
static bool drive_prot_check_sensor_warn_i_exc(drive_protection_item_t* item);


#define PROT_DESCR(arg_check_proc, arg_par_ena, arg_par_lvl, arg_par_time,\
                   arg_par_lch_ena, arg_par_act, arg_def_act)\
    {\
        .check_proc = arg_check_proc, .param_ena = arg_par_ena, .param_level = arg_par_lvl, .param_time = arg_par_time,\
        .param_latch_ena = arg_par_lch_ena, .param_action = arg_par_act, .default_action = arg_def_act\
    }


static const drive_protection_descr_t drive_prot_items_descrs[DRIVE_PROT_ITEMS_COUNT] = {
    PROT_DESCR(drive_prot_check_phase_state, PARAM_ID_PROT_PHASES_STATE_ENABLED, 0,
               PARAM_ID_PROT_PHASES_STATE_TIME_MS, 0, PARAM_ID_PROT_PHASES_STATE_ACTION, 0),
    PROT_DESCR(drive_prot_check_phases_angles, PARAM_ID_PROT_PHASES_ANGLES_FAULT_ENABLED, PARAM_ID_PROT_PHASES_ANGLES_FAULT_VALUE,
               PARAM_ID_PROT_PHASES_ANGLES_FAULT_TIME_MS, PARAM_ID_PROT_PHASES_ANGLES_FAULT_LATCH_ENABLE, PARAM_ID_PROT_PHASES_ANGLES_FAULT_ACTION, 0),
    PROT_DESCR(drive_prot_check_phases_angles, PARAM_ID_PROT_PHASES_ANGLES_WARN_ENABLED, PARAM_ID_PROT_PHASES_ANGLES_WARN_VALUE,
               PARAM_ID_PROT_PHASES_ANGLES_WARN_TIME_MS, PARAM_ID_PROT_PHASES_ANGLES_WARN_LATCH_ENABLE, PARAM_ID_PROT_PHASES_ANGLES_WARN_ACTION, 0),
    PROT_DESCR(drive_prot_check_phases_sync, PARAM_ID_PROT_PHASES_SYNC_FAULT_ENABLED, PARAM_ID_PROT_PHASES_SYNC_FAULT_VALUE,
               PARAM_ID_PROT_PHASES_SYNC_FAULT_TIME_MS, PARAM_ID_PROT_PHASES_SYNC_FAULT_LATCH_ENABLE, PARAM_ID_PROT_PHASES_SYNC_FAULT_ACTION, 0),
    PROT_DESCR(drive_prot_check_phases_sync, PARAM_ID_PROT_PHASES_SYNC_WARN_ENABLED, PARAM_ID_PROT_PHASES_SYNC_WARN_VALUE,
               PARAM_ID_PROT_PHASES_SYNC_WARN_TIME_MS, PARAM_ID_PROT_PHASES_SYNC_WARN_LATCH_ENABLE, PARAM_ID_PROT_PHASES_SYNC_WARN_ACTION, 0),
    PROT_DESCR(drive_prot_check_rot_break, PARAM_ID_PROT_ROT_BREAK_ENABLED, PARAM_ID_PROT_ROT_BREAK_VALUE,
               PARAM_ID_PROT_ROT_BREAK_TIME_MS, PARAM_ID_PROT_ROT_BREAK_LATCH_ENABLE, PARAM_ID_PROT_ROT_BREAK_ACTION, 0),
    PROT_DESCR(drive_prot_check_fan, PARAM_ID_FAN_CONTROL_ENABLE, 0, PARAM_ID_FAN_PROT_TIME, 0, 0, DRIVE_PROT_ACTION_WARNING),
    PROT_DESCR(drive_prot_check_heatsink_temp, PARAM_ID_PROT_HEATSINK_TEMP_FAULT_ENABLED, PARAM_ID_PROT_HEATSINK_TEMP_FAULT_VALUE,
               0, 0, PARAM_ID_PROT_HEATSINK_TEMP_FAULT_ACTION, 0),
    PROT_DESCR(drive_prot_check_heatsink_temp, PARAM_ID_PROT_HEATSINK_TEMP_WARN_ENABLED, PARAM_ID_PROT_HEATSINK_TEMP_WARN_VALUE,
               0, 0, PARAM_ID_PROT_HEATSINK_TEMP_WARN_ACTION, 0),
    PROT_DESCR(drive_prot_check_triacs, PARAM_ID_PROT_TRIACS_WARN_ENABLED, 0,
               PARAM_ID_PROT_TRIACS_WARN_TIME_MS, PARAM_ID_PROT_TRIACS_WARN_LATCH_ENABLE, PARAM_ID_PROT_TRIACS_WARN_ACTION, 0),
    // Защита датчиков.
    // Напряжения сети.
    PROT_DESCR(drive_prot_check_sensor_fault_u_a, PARAM_ID_PROT_SENSORS_U_IN_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_U_IN_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_U_IN_FAULT_ACTION, 0),
    PROT_DESCR(drive_prot_check_sensor_warn_u_a, PARAM_ID_PROT_SENSORS_U_IN_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_U_IN_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_U_IN_WARN_ACTION, 0),
    PROT_DESCR(drive_prot_check_sensor_fault_u_b, PARAM_ID_PROT_SENSORS_U_IN_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_U_IN_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_U_IN_FAULT_ACTION, 0),
    PROT_DESCR(drive_prot_check_sensor_warn_u_b, PARAM_ID_PROT_SENSORS_U_IN_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_U_IN_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_U_IN_WARN_ACTION, 0),
    PROT_DESCR(drive_prot_check_sensor_fault_u_c, PARAM_ID_PROT_SENSORS_U_IN_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_U_IN_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_U_IN_FAULT_ACTION, 0),
    PROT_DESCR(drive_prot_check_sensor_warn_u_c, PARAM_ID_PROT_SENSORS_U_IN_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_U_IN_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_U_IN_WARN_ACTION, 0),
    // Токи сети.
    PROT_DESCR(drive_prot_check_sensor_fault_i_a, PARAM_ID_PROT_SENSORS_I_IN_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_I_IN_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_I_IN_FAULT_ACTION, 0),
    PROT_DESCR(drive_prot_check_sensor_warn_i_a, PARAM_ID_PROT_SENSORS_I_IN_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_I_IN_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_I_IN_WARN_ACTION, 0),
    PROT_DESCR(drive_prot_check_sensor_fault_i_b, PARAM_ID_PROT_SENSORS_I_IN_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_I_IN_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_I_IN_FAULT_ACTION, 0),
    PROT_DESCR(drive_prot_check_sensor_warn_i_b, PARAM_ID_PROT_SENSORS_I_IN_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_I_IN_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_I_IN_WARN_ACTION, 0),
    PROT_DESCR(drive_prot_check_sensor_fault_i_c, PARAM_ID_PROT_SENSORS_I_IN_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_I_IN_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_I_IN_FAULT_ACTION, 0),
    PROT_DESCR(drive_prot_check_sensor_warn_i_c, PARAM_ID_PROT_SENSORS_I_IN_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_I_IN_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_I_IN_WARN_ACTION, 0),
    // Напряжение якоря.
    PROT_DESCR(drive_prot_check_sensor_fault_u_rot, PARAM_ID_PROT_SENSORS_U_ROT_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_U_ROT_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_U_ROT_FAULT_ACTION, 0),
    PROT_DESCR(drive_prot_check_sensor_warn_u_rot, PARAM_ID_PROT_SENSORS_U_ROT_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_U_ROT_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_U_ROT_WARN_ACTION, 0),
    // Ток якоря.
    PROT_DESCR(drive_prot_check_sensor_fault_i_rot, PARAM_ID_PROT_SENSORS_I_ROT_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_I_ROT_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_I_ROT_FAULT_ACTION, 0),
    PROT_DESCR(drive_prot_check_sensor_warn_i_rot, PARAM_ID_PROT_SENSORS_I_ROT_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_I_ROT_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_I_ROT_WARN_ACTION, 0),
    // Ток возбуждения.
    PROT_DESCR(drive_prot_check_sensor_fault_i_exc, PARAM_ID_PROT_SENSORS_I_EXC_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_I_EXC_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_I_EXC_FAULT_ACTION, 0),
    PROT_DESCR(drive_prot_check_sensor_warn_i_exc, PARAM_ID_PROT_SENSORS_I_EXC_ENABLED, 0, 0,
              PARAM_ID_PROT_SENSORS_I_EXC_LATCH_ENABLE, PARAM_ID_PROT_SENSORS_I_EXC_WARN_ACTION, 0),
};


//! Тип структуры защиты привода.
typedef struct _Drive_Protection {
    fixed32_t U_in; //!< Номинальное напряжение, В.
    fixed32_t U_rot; //!< Номинальное напряжение якоря.
    fixed32_t I_rot; //!< Номинальный ток якоря.
    fixed32_t I_exc; //!< Номинальный ток возбуждения.
    fixed32_t U_rot_idle; //!< Шум нуля напряжения якоря.
    fixed32_t I_rot_idle; //!< Шум нуля тока якоря.
    drive_protection_power_item_t prot_pwr_items[DRIVE_PROT_PWR_ITEMS_COUNT];
    drive_power_errors_t prot_errs_mask; //!< Маска ошибок защиты.
    drive_power_warnings_t prot_warn_mask; //!< Маска предупреждений защиты.
    drive_power_errors_t prot_cutoff_errs_mask; //!< Маска ошибок защиты отсечки.
    drive_power_warnings_t prot_cutoff_warn_mask; //!< Маска предупреждений защиты отсечки.
    drive_top_t top; //!< Тепловая защита.
    drive_fan_prot_t fan; //!< Защита вентилятора.
    drive_triacs_prot_t triacs; //!< Защита тиристоров.
    drive_sensors_prot_t sensors; //!< Защиты датчиков.
    drive_prot_action_t emergency_stop_action; //!< Действие при нажатии грибка.
    drive_protection_item_t prot_items[DRIVE_PROT_ITEMS_COUNT];
} drive_protection_t;

//! Структура защиты привода.
static drive_protection_t drive_prot;


/*
 * Функции базового элемента защиты.
 */

static void drive_prot_update_base_item_settings(drive_prot_base_item_t* item,
                        param_id_t param_ena, param_id_t param_time, param_id_t param_latch_ena, param_id_t param_action,
                        drive_prot_action_t default_action)
{
    // Если отсутствует элемент - возврат.
    if(item == NULL) return;
    
    // Разрешение элемента защиты.
    if(param_ena != 0){
        item->enabled = (bool)settings_valueu(param_ena);
    }else{
        // По-умолчанию - разрешение элемента защиты.
        item->enabled = true;
    }
    
    // Если элемент запрещён.
    if(!item->enabled){
        // Сбросим накопление.
        item->pie = 0;
    }
    // Если задан параметр допустимого времени отклонения.
    if(param_time != 0){
        // Допустимое время в мс.
        uint32_t avail_time = settings_valueu(param_time);
        // Если оно равно нулю - то будем реагировать сразу (максимальный инкремент), иначе вычислим приращение.
        if(avail_time != 0){
            item->pie_inc = DRIVE_PROT_CALC_PIE_INC(avail_time);
        }else{
            item->pie_inc = DRIVE_PROT_PIE_MAX;
        }
    }else{
        // По-умолчанию - максимальное приращение.
        item->pie_inc = DRIVE_PROT_PIE_MAX;
    }
    // Разрешение защёлки.
    if(param_latch_ena != 0){
        item->latch_enabled = (bool)settings_valueu(param_latch_ena);
    }else{
        item->latch_enabled = false;
    }
    // Если запрещён элемент или защёлка.
    if(!item->enabled || !item->latch_enabled){
        // Сбросим значение защёлки.
        item->hold_value = false;
    }
    // Действие при срабатывании элемента защиты.
    if(param_action != 0){
        item->action = settings_valueu(param_action);
    }else{
        // По-умолчанию - экстренное отключение.
        item->action = default_action;
    }
}

static bool drive_prot_check_base_item(drive_prot_base_item_t* item, bool masked, bool valid)
{
    if(!valid){
        item->pie += item->pie_inc;
        
        if(item->pie >= DRIVE_PROT_PIE_MAX){
            item->pie = DRIVE_PROT_PIE_MAX;
            
            if(masked){
                item->active = true;
                
                if(item->latch_enabled){
                    item->hold_value = true;
                }
            }else{
                if(item->latch_enabled){
                    item->active = item->hold_value;
                }else{
                    item->active = false;
                }
            }
        }
        
        item->allow = false;
        
    }else{ // value is normal
        item->pie -= item->pie_inc;
        
        if(item->pie < 0) item->pie = 0;
        
        if(item->latch_enabled){
            item->active = item->hold_value;
        }else{
            item->active = false;
        }
        
        item->allow = true;
    }
    
    return item->active;
}

static void drive_prot_base_item_reset(drive_prot_base_item_t* base_item)
{
    base_item->active = DRIVE_PROT_ITEM_ACTIVE_DEFAULT;
    base_item->allow = DRIVE_PROT_ITEM_ALLOW_DEFAULT;
    base_item->hold_value = false;
    base_item->pie = 0;
}

ALWAYS_INLINE static bool drive_prot_base_item_enabled(drive_prot_base_item_t* base_item)
{
    return base_item->enabled;
}

ALWAYS_INLINE static bool drive_prot_base_item_active(drive_prot_base_item_t* base_item)
{
    return base_item->active;
}

ALWAYS_INLINE static bool drive_prot_base_item_allow(drive_prot_base_item_t* base_item)
{
    return base_item->allow;
}

ALWAYS_INLINE static fixed32_t drive_prot_base_item_pie(drive_prot_base_item_t* base_item)
{
    return base_item->pie;
}

ALWAYS_INLINE static drive_prot_action_t drive_prot_base_item_action(drive_prot_base_item_t* base_item)
{
    return base_item->action;
}

static bool drive_prot_base_item_stable(drive_prot_base_item_t* base_item)
{
    if(base_item->allow && base_item->pie == 0) return true;
    
    if(!base_item->allow && base_item->pie == DRIVE_PROT_PIE_MAX) return true;
    
    return false;
}


/*
 * Функции элементов защиты питания.
 */

ALWAYS_INLINE static drive_protection_power_item_t* drive_protection_power_get_item(drive_prot_index_t index)
{
    return &drive_prot.prot_pwr_items[index];
}

ALWAYS_INLINE static const drive_protection_power_descr_t* drive_protection_power_get_item_descr(drive_prot_index_t index)
{
    return &drive_prot_power_items_descrs[index];
}

ALWAYS_INLINE static fixed32_t drive_protection_get_part(fixed32_t value, uint32_t percents)
{
    return (int64_t)value * percents / 100;
}

ALWAYS_INLINE static fixed32_t drive_protection_get_ovf_level(fixed32_t value, uint32_t percents)
{
    return value + drive_protection_get_part(value, percents);
}

ALWAYS_INLINE static fixed32_t drive_protection_get_udf_level(fixed32_t value, uint32_t percents)
{
    return value - drive_protection_get_part(value, percents);
}

/**
 * Обновляет настройки элемента защиты питания.
 * @param index Индекс элемента защиты питания.
 */
static void drive_prot_power_update_item_settings(drive_prot_index_t index)
{
    // Если превышен индекс - возврат.
    if(index >= DRIVE_PROT_PWR_ITEMS_COUNT) return;
    
    // Элемент защиты.
    drive_protection_power_item_t* item = drive_protection_power_get_item(index);
    // Дескриптор элемента защиты.
    const drive_protection_power_descr_t* descr = drive_protection_power_get_item_descr(index);
    
    drive_prot_update_base_item_settings(&item->base_item,
            descr->param_ena, descr->param_time, descr->param_latch_ena, descr->param_action, DRIVE_PROT_ACTION_EMERGENCY_STOP);
    
    // Значение уровня элемента защиты.
    fixed32_t ref_val = 0;
    // Если задано опорное значение.
    if(descr->param_ref != 0){
        ref_val = settings_valuef(descr->param_ref);
    }
    // Если защита по мгновенным значениям (отсечка) и имеем дело с переменным напряжением,
    // то вычислим амплитудное значение номинального действующего значения.
    if(descr->type == DRIVE_PROT_TYPE_CUT && drive_power_channel_type(descr->power_channel) == POWER_CHANNEL_AC){
        ref_val = fixed32_mul((int64_t)ref_val, 0x16a10); // ref_val *= sqrt(2);
    }
    // Вычислим уровень защиты в зависимости от типа (от повышения или от понижения значения).
    switch(descr->type){
        case DRIVE_PROT_TYPE_CUT:
            item->base_item.pie_inc = DRIVE_PROT_CUTOFF_PIE_INC;
        case DRIVE_PROT_TYPE_OVF:
            item->value_level = drive_protection_get_ovf_level(ref_val, settings_valueu(descr->param_level));
            break;
        case DRIVE_PROT_TYPE_UDF:
            item->value_level = drive_protection_get_udf_level(ref_val, settings_valueu(descr->param_level));
            break;
        case DRIVE_PROT_TYPE_ZERO:
            item->value_level = settings_valuef(descr->param_level);
            break;
    }
}

/**
 * Обновляет настройки элементов защиты питания.
 */
static void drive_prot_power_update_items_settings(void)
{
    drive_prot_index_t i;
    for(i = 0; i < DRIVE_PROT_PWR_ITEMS_COUNT; i ++){
        drive_prot_power_update_item_settings(i);
    }
}


/*
 * Функции общих элементов защиты.
 */

ALWAYS_INLINE static drive_protection_item_t* drive_protection_get_item(drive_prot_index_t index)
{
    return &drive_prot.prot_items[index];
}

ALWAYS_INLINE static const drive_protection_descr_t* drive_protection_get_item_descr(drive_prot_index_t index)
{
    return &drive_prot_items_descrs[index];
}

/**
 * Обновляет настройки элемента защиты.
 * @param index Индекс элемента защиты.
 */
static void drive_prot_update_item_settings(drive_prot_index_t index)
{
    // Если превышен индекс - возврат.
    if(index >= DRIVE_PROT_ITEMS_COUNT) return;
    
    // Элемент защиты.
    drive_protection_item_t* item = drive_protection_get_item(index);
    // Дескриптор элемента защиты.
    const drive_protection_descr_t* descr = drive_protection_get_item_descr(index);
    
    drive_prot_update_base_item_settings(&item->base_item,
            descr->param_ena, descr->param_time, descr->param_latch_ena, descr->param_action, descr->default_action);
    
    // Значение уровня элемента защиты.
    if(descr->param_level){
        item->value_level = settings_valuef(descr->param_level);
    }
}

/**
 * Обновляет настройки элементов защиты питания.
 */
static void drive_prot_update_items_settings(void)
{
    drive_prot_index_t i;
    for(i = 0; i < DRIVE_PROT_ITEMS_COUNT; i ++){
        drive_prot_update_item_settings(i);
    }
}

/*
 * Функции обновления настроек элементов защиты.
 */

/**
 * Обновляет настройки тепловой защиты.
 */
static void drive_protection_update_top_settings(void)
{
    drive_prot.top.k_pie_max =
            (DRIVE_TOP_NOMINAL_OVERCURRENT * DRIVE_TOP_NOMINAL_OVERCURRENT - 1)
            * settings_valuef(PARAM_ID_THERMAL_OVERLOAD_PROT_TIME_6I);
    
    drive_prot.top.enabled = settings_valueu(PARAM_ID_THERMAL_OVERLOAD_PROT_ENABLE);
    
    // Если нет разрешения,
    // сбросим защиту.
    if(!drive_prot.top.enabled){
        drive_prot.top.cur_pie = 0;
        drive_prot.top.overheat = false;
    }
    
    drive_prot.top.action = settings_valueu(PARAM_ID_THERMAL_OVERLOAD_PROT_ACTION);
}

/*
 * Функции проверки элементов защиты.
 */

/**
 * Выполняет проверку состояния фаз.
 * @param item Элемент защиты.
 * @return Флаг нормального состояния фаз.
 */
static bool drive_prot_check_phase_state(drive_protection_item_t* item)
{
    return drive_phase_state_errors() == PHASE_NO_ERROR;
}

/**
 * Выполняет проверку уровня ошибки защиты фаз по углу между фазами.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_phases_angles(drive_protection_item_t* item)
{
    fixed32_t angle = 0;
    
    bool valid = true;
    
    angle = drive_phase_sync_diff_delta_angle(PHASE_A); angle = fixed_abs(angle);
    valid &= angle <= item->value_level;
    
    angle = drive_phase_sync_diff_delta_angle(PHASE_B); angle = fixed_abs(angle);
    valid &= angle <= item->value_level;
    
    angle = drive_phase_sync_diff_delta_angle(PHASE_C); angle = fixed_abs(angle);
    valid &= angle <= item->value_level;
    
    return valid;
}

/**
 * Выполняет проверку защиты фаз по синхронизации с фазами.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_phases_sync(drive_protection_item_t* item)
{
    fixed32_t angle = 0;
    
    angle = drive_phase_sync_offset_angle(DRIVE_PHASE_SYNC_PHASE);
    angle = fixed_abs(angle);
    
    return angle <= item->value_level;
}

/**
 * Выполняет проверку обрыва якоря.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_rot_break(drive_protection_item_t* item)
{
    fixed32_t Urot = drive_power_channel_real_value(DRIVE_POWER_Urot);
    fixed32_t Irot = drive_power_channel_real_value(DRIVE_POWER_Irot);
    
    return Urot <= item->value_level || Irot > drive_prot.I_rot_idle;
}

/**
 * Выполняет проверку вентилятора.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_fan(drive_protection_item_t* item)
{
    bool fan_run = drive_temp_fan_running();
    
    fixed32_t i_fan = drive_power_channel_real_value(DRIVE_POWER_Ifan);
    
    bool fan_i_zero = i_fan <= drive_prot.fan.I_fan_zero_noise;
    bool fan_i_ovf = i_fan > drive_prot.fan.I_fan_ovf;
    
    bool fan_idle_fail = !fan_run && !fan_i_zero;
    bool fan_udf_fail = fan_run && fan_i_zero;
    bool fan_ovf_fail = fan_run && fan_i_ovf;
    
    return !(fan_idle_fail || fan_udf_fail || fan_ovf_fail);
}

/**
 * Выполняет проверку температуры радиатора.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_heatsink_temp(drive_protection_item_t* item)
{
    return drive_temp_heatsink_temp() <= item->value_level;
}

/**
 * Выполняет проверку тиристоров.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_triacs(drive_protection_item_t* item)
{
    if(!drive_triacs_pairs_enabled()) return true;
    
    fixed32_t angle = drive_triacs_pairs_open_angle();
    if(angle < drive_prot.triacs.min_angle) return true;
    
    fixed32_t Ia = drive_power_channel_real_value(DRIVE_POWER_Ia);
    fixed32_t Ib = drive_power_channel_real_value(DRIVE_POWER_Ib);
    fixed32_t Ic = drive_power_channel_real_value(DRIVE_POWER_Ic);
    
    if((Ia < drive_prot.triacs.min_current) &&
       (Ib < drive_prot.triacs.min_current) &&
       (Ic < drive_prot.triacs.min_current)){
        return true;
    }
    
    return !drive_power_triacs_fail();
}

/**
 * Выполняет проверку работоспособности датчика уровня предупреждения.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_warn(drive_protection_item_t* item, bool in_range, bool cur_calc, bool emu_ena, bool can_calc)
{
    if(in_range) return true;
    if(cur_calc) return true;
    
    return false;
}

/**
 * Выполняет проверку работоспособности датчика уровня ошибки.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_fault(drive_protection_item_t* item, bool in_range, bool cur_calc, bool emu_ena, bool can_calc)
{
    if(in_range) return true;
    if(cur_calc) return true;
    if(emu_ena && can_calc) return true;
    
    return false;
}

/**
 * Выполняет проверку работоспособности датчика напряжения
 * фазы A уровня предупреждения.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_warn_u_a(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Ua);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_u_in.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_u_in.adc_range_max);
    
    bool cur_calc = drive_power_phase_calc_voltage() == PHASE_A;
    
    bool emu_ena = drive_prot.sensors.sensor_u_in.emulation_enabled;
    
    bool can_calc = drive_power_phase_can_calc_voltage(PHASE_A);
    
    return drive_prot_check_sensor_warn(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика напряжения
 * фазы A уровня ошибки.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_fault_u_a(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Ua);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_u_in.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_u_in.adc_range_max);
    
    bool cur_calc = drive_power_phase_calc_voltage() == PHASE_A;
    
    bool emu_ena = drive_prot.sensors.sensor_u_in.emulation_enabled;
    
    bool can_calc = drive_power_phase_can_calc_voltage(PHASE_A);
    
    return drive_prot_check_sensor_fault(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика напряжения
 * фазы B уровня предупреждения.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_warn_u_b(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Ub);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_u_in.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_u_in.adc_range_max);
    
    bool cur_calc = drive_power_phase_calc_voltage() == PHASE_B;
    
    bool emu_ena = drive_prot.sensors.sensor_u_in.emulation_enabled;
    
    bool can_calc = drive_power_phase_can_calc_voltage(PHASE_B);
    
    return drive_prot_check_sensor_warn(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика напряжения
 * фазы B уровня ошибки.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_fault_u_b(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Ub);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_u_in.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_u_in.adc_range_max);
    
    bool cur_calc = drive_power_phase_calc_voltage() == PHASE_B;
    
    bool emu_ena = drive_prot.sensors.sensor_u_in.emulation_enabled;
    
    bool can_calc = drive_power_phase_can_calc_voltage(PHASE_B);
    
    return drive_prot_check_sensor_fault(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика напряжения
 * фазы C уровня предупреждения.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_warn_u_c(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Uc);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_u_in.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_u_in.adc_range_max);
    
    bool cur_calc = drive_power_phase_calc_voltage() == PHASE_C;
    
    bool emu_ena = drive_prot.sensors.sensor_u_in.emulation_enabled;
    
    bool can_calc = drive_power_phase_can_calc_voltage(PHASE_C);
    
    return drive_prot_check_sensor_warn(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика напряжения
 * фазы C уровня ошибки.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_fault_u_c(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Uc);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_u_in.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_u_in.adc_range_max);
    
    bool cur_calc = drive_power_phase_calc_voltage() == PHASE_C;
    
    bool emu_ena = drive_prot.sensors.sensor_u_in.emulation_enabled;
    
    bool can_calc = drive_power_phase_can_calc_voltage(PHASE_C);
    
    return drive_prot_check_sensor_fault(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика тока
 * фазы A уровня предупреждения.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_warn_i_a(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Ia);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_i_in.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_i_in.adc_range_max);
    
    bool cur_calc = drive_power_phase_calc_current() == PHASE_A;
    
    bool emu_ena = drive_prot.sensors.sensor_i_in.emulation_enabled;
    
    bool can_calc = drive_power_phase_can_calc_current(PHASE_A);
    
    return drive_prot_check_sensor_warn(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика тока
 * фазы A уровня ошибки.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_fault_i_a(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Ia);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_i_in.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_i_in.adc_range_max);
    
    bool cur_calc = drive_power_phase_calc_current() == PHASE_A;
    
    bool emu_ena = drive_prot.sensors.sensor_i_in.emulation_enabled;
    
    bool can_calc = drive_power_phase_can_calc_current(PHASE_A);
    
    return drive_prot_check_sensor_fault(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика тока
 * фазы B уровня предупреждения.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_warn_i_b(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Ib);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_i_in.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_i_in.adc_range_max);
    
    bool cur_calc = drive_power_phase_calc_current() == PHASE_B;
    
    bool emu_ena = drive_prot.sensors.sensor_i_in.emulation_enabled;
    
    bool can_calc = drive_power_phase_can_calc_current(PHASE_B);
    
    // DEBUG!
    //bool res =  drive_prot_check_sensor_warn(item, in_range, cur_calc, emu_ena, can_calc);
    //if(!res) item->value_level = adc_val;
    //return res;
    
    return drive_prot_check_sensor_warn(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика тока
 * фазы B уровня ошибки.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_fault_i_b(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Ib);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_i_in.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_i_in.adc_range_max);
    
    bool cur_calc = drive_power_phase_calc_current() == PHASE_B;
    
    bool emu_ena = drive_prot.sensors.sensor_i_in.emulation_enabled;
    
    bool can_calc = drive_power_phase_can_calc_current(PHASE_B);
    
    // DEBUG!
    //bool res =  drive_prot_check_sensor_fault(item, in_range, cur_calc, emu_ena, can_calc);
    //if(!res) item->value_level = adc_val;
    //return res;
    
    return drive_prot_check_sensor_fault(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика тока
 * фазы C уровня предупреждения.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_warn_i_c(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Ic);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_i_in.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_i_in.adc_range_max);
    
    bool cur_calc = drive_power_phase_calc_current() == PHASE_C;
    
    bool emu_ena = drive_prot.sensors.sensor_i_in.emulation_enabled;
    
    bool can_calc = drive_power_phase_can_calc_current(PHASE_C);
    
    return drive_prot_check_sensor_warn(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика тока
 * фазы C уровня ошибки.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_fault_i_c(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Ic);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_i_in.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_i_in.adc_range_max);
    
    bool cur_calc = drive_power_phase_calc_current() == PHASE_C;
    
    bool emu_ena = drive_prot.sensors.sensor_i_in.emulation_enabled;
    
    bool can_calc = drive_power_phase_can_calc_current(PHASE_C);
    
    return drive_prot_check_sensor_fault(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика напряжения
 * якоря уровня предупреждения.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_warn_u_rot(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Urot);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_u_rot.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_u_rot.adc_range_max);
    
    bool cur_calc = drive_power_rot_calc_voltage();
    
    bool emu_ena = drive_prot.sensors.sensor_u_rot.emulation_enabled;
    
    bool can_calc = drive_power_rot_can_calc_voltage();
    
    return drive_prot_check_sensor_warn(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика напряжения
 * якоря уровня ошибки.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_fault_u_rot(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Urot);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_u_rot.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_u_rot.adc_range_max);
    
    bool cur_calc = drive_power_rot_calc_voltage();
    
    bool emu_ena = drive_prot.sensors.sensor_u_rot.emulation_enabled;
    
    bool can_calc = drive_power_rot_can_calc_voltage();
    
    return drive_prot_check_sensor_fault(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика тока
 * якоря уровня предупреждения.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_warn_i_rot(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Irot);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_i_rot.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_i_rot.adc_range_max);
    
    bool cur_calc = drive_power_rot_calc_current();
    
    bool emu_ena = drive_prot.sensors.sensor_i_rot.emulation_enabled;
    
    bool can_calc = drive_power_rot_can_calc_current();
    
    return drive_prot_check_sensor_warn(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика тока
 * якоря уровня ошибки.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_fault_i_rot(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Irot);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_i_rot.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_i_rot.adc_range_max);
    
    bool cur_calc = drive_power_rot_calc_current();
    
    bool emu_ena = drive_prot.sensors.sensor_i_rot.emulation_enabled;
    
    bool can_calc = drive_power_rot_can_calc_current();
    
    return drive_prot_check_sensor_fault(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика тока
 * возбуждения уровня предупреждения.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_warn_i_exc(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Iexc);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_i_exc.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_i_exc.adc_range_max);
    
    bool cur_calc = drive_power_exc_calc_current();
    
    bool emu_ena = drive_prot.sensors.sensor_i_exc.emulation_enabled;
    
    bool can_calc = drive_power_exc_can_calc_current();
    
    return drive_prot_check_sensor_warn(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Выполняет проверку работоспособности датчика тока
 * возбуждения уровня ошибки.
 * @return Флаг допустимости элемента защиты.
 */
static bool drive_prot_check_sensor_fault_i_exc(drive_protection_item_t* item)
{
    uint16_t adc_val = drive_power_channel_raw_adc_value_inst(DRIVE_POWER_Iexc);
    
    bool in_range = (adc_val >=  drive_prot.sensors.sensor_i_exc.adc_range_min &&
       adc_val <= drive_prot.sensors.sensor_i_exc.adc_range_max);
    
    bool cur_calc = drive_power_exc_calc_current();
    
    bool emu_ena = drive_prot.sensors.sensor_i_exc.emulation_enabled;
    
    bool can_calc = drive_power_exc_can_calc_current();
    
    return drive_prot_check_sensor_fault(item, in_range, cur_calc, emu_ena, can_calc);
}

/**
 * Обновляет настройки защиты вентилятора.
 */
void drive_protection_fan_update_settings(void)
{
    drive_prot.fan.I_fan_nom = settings_valuef(PARAM_ID_FAN_I_NOM);
    drive_prot.fan.I_fan_zero_noise = settings_valuef(PARAM_ID_FAN_I_ZERO_NOISE);
    drive_prot.fan.I_fan_ovf = drive_protection_get_ovf_level(drive_prot.fan.I_fan_nom, settings_valueu(PARAM_ID_FAN_PROT_OVF_LEVEL));
}

/**
 * Обновляет настройки защиты тиристоров.
 */
void drive_protection_triacs_update_settings(void)
{
    drive_prot.triacs.min_angle = settings_valuef(PARAM_ID_PROT_TRIACS_WARN_MIN_ANGLE);
    drive_prot.triacs.min_current = settings_valuef(PARAM_ID_PROT_TRIACS_WARN_MIN_CURRENT);
}

void drive_protection_sensors_update_settings(void)
{
    // Датчики напряжения сети.
    drive_prot.sensors.sensor_u_in.adc_range_min = settings_valueu(PARAM_ID_PROT_SENSORS_U_IN_ADC_RANGE_MIN);
    drive_prot.sensors.sensor_u_in.adc_range_max = settings_valueu(PARAM_ID_PROT_SENSORS_U_IN_ADC_RANGE_MAX);
    drive_prot.sensors.sensor_u_in.emulation_enabled = settings_valueu(PARAM_ID_PROT_SENSORS_U_IN_EMULATION_ENABLED);
    // Датчики тока сети.
    drive_prot.sensors.sensor_i_in.adc_range_min = settings_valueu(PARAM_ID_PROT_SENSORS_I_IN_ADC_RANGE_MIN);
    drive_prot.sensors.sensor_i_in.adc_range_max = settings_valueu(PARAM_ID_PROT_SENSORS_I_IN_ADC_RANGE_MAX);
    drive_prot.sensors.sensor_i_in.emulation_enabled = settings_valueu(PARAM_ID_PROT_SENSORS_I_IN_EMULATION_ENABLED);
    // Датчик напряжения якоря.
    drive_prot.sensors.sensor_u_rot.adc_range_min = settings_valueu(PARAM_ID_PROT_SENSORS_U_ROT_ADC_RANGE_MIN);
    drive_prot.sensors.sensor_u_rot.adc_range_max = settings_valueu(PARAM_ID_PROT_SENSORS_U_ROT_ADC_RANGE_MAX);
    drive_prot.sensors.sensor_u_rot.emulation_enabled = settings_valueu(PARAM_ID_PROT_SENSORS_U_ROT_EMULATION_ENABLED);
    // Датчик тока якоря.
    drive_prot.sensors.sensor_i_rot.adc_range_min = settings_valueu(PARAM_ID_PROT_SENSORS_I_ROT_ADC_RANGE_MIN);
    drive_prot.sensors.sensor_i_rot.adc_range_max = settings_valueu(PARAM_ID_PROT_SENSORS_I_ROT_ADC_RANGE_MAX);
    drive_prot.sensors.sensor_i_rot.emulation_enabled = settings_valueu(PARAM_ID_PROT_SENSORS_I_ROT_EMULATION_ENABLED);
    // Датчик тока возбуждения.
    drive_prot.sensors.sensor_i_exc.adc_range_min = settings_valueu(PARAM_ID_PROT_SENSORS_I_EXC_ADC_RANGE_MIN);
    drive_prot.sensors.sensor_i_exc.adc_range_max = settings_valueu(PARAM_ID_PROT_SENSORS_I_EXC_ADC_RANGE_MAX);
    drive_prot.sensors.sensor_i_exc.emulation_enabled = settings_valueu(PARAM_ID_PROT_SENSORS_I_EXC_EMULATION_ENABLED);
    
    drive_prot_index_t item_index = 0;
    drive_protection_item_t* item = NULL;
    for(item_index = DRIVE_PROT_ITEMS_SENSOR_BEGIN; item_index <= DRIVE_PROT_ITEMS_SENSOR_END; item_index ++){
        item = drive_protection_get_item(item_index);
        item->base_item.pie_inc = DRIVE_PROT_SENSOR_PIE_INC;
    }
}


/*
 * Основные функции защиты.
 */

bool drive_protection_init(void)
{
    memset(&drive_prot, 0x0, sizeof(drive_protection_t));
    
    return true;
}

void drive_protection_update_settings(void)
{
    drive_prot.U_in = settings_valuef(PARAM_ID_U_NOM);
    drive_prot.U_rot = settings_valuef(PARAM_ID_MOTOR_U_ROT_NOM);
    drive_prot.I_rot = settings_valuef(PARAM_ID_MOTOR_I_ROT_NOM);
    drive_prot.I_exc = settings_valuef(PARAM_ID_I_EXC);
    
    drive_prot.U_rot_idle = settings_valuef(PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_VALUE);
    drive_prot.I_rot_idle = settings_valuef(PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_VALUE);
    
    drive_prot_power_update_items_settings();
    
    drive_prot_update_items_settings();
    
    drive_protection_update_top_settings();
    
    drive_protection_fan_update_settings();
    
    drive_protection_triacs_update_settings();
    
    drive_protection_sensors_update_settings();
    
    drive_prot.emergency_stop_action = settings_valueu(PARAM_ID_EMERGENCY_STOP_ACTION);
}

drive_power_errors_t drive_protection_power_errs_mask(void)
{
    return drive_prot.prot_errs_mask;
}

void drive_protection_power_set_errs_mask(drive_power_errors_t errs_mask)
{
    drive_prot.prot_errs_mask = errs_mask;
}

void drive_protection_power_set_errs_mask_flags(drive_power_errors_t errs_flags)
{
    drive_prot.prot_errs_mask |= errs_flags;
}

void drive_protection_power_reset_errs_mask_flags(drive_power_errors_t errs_flags)
{
    drive_prot.prot_errs_mask &= ~errs_flags;
}

drive_power_warnings_t drive_protection_power_warn_mask(void)
{
    return drive_prot.prot_warn_mask;
}

void drive_protection_power_set_warn_mask(drive_power_warnings_t warn_mask)
{
    drive_prot.prot_warn_mask = warn_mask;
}

void drive_protection_power_set_warn_mask_flags(drive_power_warnings_t warn_flags)
{
    drive_prot.prot_warn_mask |= warn_flags;
}

void drive_protection_power_reset_warn_mask_flags(drive_power_warnings_t warn_flags)
{
    drive_prot.prot_warn_mask &= ~warn_flags;
}

drive_power_errors_t drive_protection_power_cutoff_errs_mask(void)
{
    return drive_prot.prot_cutoff_errs_mask;
}

void drive_protection_power_set_cutoff_errs_mask(drive_power_errors_t errs_mask)
{
    drive_prot.prot_cutoff_errs_mask = errs_mask;
}

void drive_protection_power_set_cutoff_errs_mask_flags(drive_power_errors_t errs_flags)
{
    drive_prot.prot_cutoff_errs_mask |= errs_flags;
}

void drive_protection_power_reset_cutoff_errs_mask_flags(drive_power_errors_t errs_flags)
{
    drive_prot.prot_cutoff_errs_mask &= ~errs_flags;
}

drive_power_warnings_t drive_protection_power_cutoff_warn_mask(void)
{
    return drive_prot.prot_cutoff_warn_mask;
}

void drive_protection_power_set_cutoff_warn_mask(drive_power_warnings_t warn_mask)
{
    drive_prot.prot_cutoff_warn_mask = warn_mask;
}

void drive_protection_power_set_cutoff_warn_mask_flags(drive_power_warnings_t warn_flags)
{
    drive_prot.prot_cutoff_warn_mask |= warn_flags;
}

void drive_protection_power_reset_cutoff_warn_mask_flags(drive_power_warnings_t warn_flags)
{
    drive_prot.prot_cutoff_warn_mask &= ~warn_flags;
}

/**
 * Проверяет значение элемента защиты.
 * @param type Тип элемента защиты.
 * @param value Значение элемента защиты.
 * @param level Уровень элемента защиты.
 * @return Флаг нахождения значения элемента защиты в допустимом диапазоне.
 */
ALWAYS_INLINE static bool drive_protection_check_item_value(drive_protection_type_t type, fixed32_t value, fixed32_t level)
{
    switch(type){
        default:
        case DRIVE_PROT_TYPE_CUT:
        case DRIVE_PROT_TYPE_OVF:
        case DRIVE_PROT_TYPE_ZERO:
            break;
        case DRIVE_PROT_TYPE_UDF:
            return value >= level;
    }
    return value <= level;
}

/**
 * Получает наличие элемента защиты в маске ошибок или предупреждений.
 * @param item Элемент защиты.
 * @param descr Дескриптор элемента защиты.
 * @return Флаг наличия элемента защиты в маске ошибок или предупреждений.
 */
static bool drive_protection_item_masked_impl(const drive_protection_power_descr_t* descr)
{
    if(descr->flag_type == DRIVE_PROT_FLAG_ERR){
        if(descr->type != DRIVE_PROT_TYPE_CUT)
            return (descr->flag & drive_prot.prot_errs_mask) != 0;
        else
            return (descr->flag & drive_prot.prot_cutoff_errs_mask) != 0;
    }else{ // DRIVE_PROT_TYPE_WRN
        if(descr->type != DRIVE_PROT_TYPE_CUT)
            return (descr->flag & drive_prot.prot_warn_mask) != 0;
        else
            return (descr->flag & drive_prot.prot_cutoff_warn_mask) != 0;
    }
}

/**
 * Проверяет элемент канала защиты средних(RMS) значений.
 * @param item Элемент.
 * @param descr Дескриптор.
 * @param warnings Предупреждения.
 * @param errors Ошибки.
 * @return Флаг нахождения элемента защиты в допустимом диапазоне.
 */
static bool drive_protection_check_power_item_real(drive_protection_power_item_t* item, const drive_protection_power_descr_t* descr, drive_power_warnings_t* warnings, drive_power_errors_t* errors)
{
    bool masked = drive_protection_item_masked_impl(descr);
    
    fixed32_t value = drive_power_channel_real_value(descr->power_channel);
    
    bool valid = drive_protection_check_item_value(descr->type, value, item->value_level);
    
    drive_prot_check_base_item(&item->base_item, masked, valid);
    
    if(drive_prot_base_item_active(&item->base_item)){
        if(descr->flag_type == DRIVE_PROT_FLAG_WRN){
            if(warnings) (*warnings) |= descr->flag;
        }else{ // DRIVE_PROT_TYPE_ERR
            if(errors) (*errors) |= descr->flag;
        }
    }
    
    return item->base_item.active;
}

/**
 * Проверяет элемент канала защиты мгновенных значений.
 * @param item Элемент.
 * @param descr Дескриптор.
 * @param warnings Предупреждения.
 * @param errors Ошибки.
 * @return Флаг нахождения элемента защиты в допустимом диапазоне.
 */
static bool drive_protection_check_power_item_inst(drive_protection_power_item_t* item, const drive_protection_power_descr_t* descr, drive_power_warnings_t* warnings, drive_power_errors_t* errors)
{
    bool masked = drive_protection_item_masked_impl(descr);
    
    fixed32_t value = drive_power_channel_real_value_inst(descr->power_channel);
    
    if(!drive_protection_check_item_value(descr->type, value, item->value_level)){
        
        item->base_item.allow = false;
        
        if(item->base_item.pie >= DRIVE_PROT_PIE_MAX){
            item->base_item.pie = DRIVE_PROT_PIE_MAX;
            
            if(masked){
                if(descr->flag_type == DRIVE_PROT_FLAG_WRN){
                    if(warnings) (*warnings) |= descr->flag;
                }else{ // DRIVE_PROT_TYPE_ERR
                    if(errors) (*errors) |= descr->flag;
                }
                item->base_item.active = true;
            }else{
                item->base_item.active = false;
            }
        }else{
            item->base_item.pie += item->base_item.pie_inc;
        }
    }else{
        item->base_item.pie -= item->base_item.pie_inc;
        if(item->base_item.pie < 0) item->base_item.pie = 0;
        
        item->base_item.allow = true;
        item->base_item.active = false;
    }
    
    return item->base_item.active;
}

bool drive_protection_power_check_item(drive_prot_index_t index, drive_power_warnings_t* warnings, drive_power_errors_t* errors)
{
    if(index >= DRIVE_PROT_PWR_ITEMS_COUNT) return false;
    
    drive_protection_power_item_t* item = drive_protection_power_get_item(index);
    
    if(!drive_prot_base_item_enabled(&item->base_item)) return false;
    
    const drive_protection_power_descr_t* descr = drive_protection_power_get_item_descr(index);
    
    bool prev_active = drive_prot_base_item_active(&item->base_item);
    bool new_active = false;
    
    if(descr->type != DRIVE_PROT_TYPE_CUT){//fault || warn
        new_active = drive_protection_check_power_item_real(item, descr, warnings, errors);
    }else{//cutoff
        new_active = drive_protection_check_power_item_inst(item, descr, warnings, errors);
    }
    
    return (prev_active == false) && new_active;
}

bool drive_protection_power_check_items(const drive_prot_index_t* items, size_t items_count, drive_power_warnings_t* warnings, drive_power_errors_t* errors)
{
    if(items == NULL || items_count == 0) return true;
    
    bool res = false;
    
    drive_prot_index_t i;
    for(i = 0; i < items_count; i ++){
        res |= drive_protection_power_check_item(items[i], warnings, errors);
    }
    
    return res;
}

bool drive_protection_power_item_stable(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_PWR_ITEMS_COUNT) return false;
    
    drive_protection_power_item_t* item = drive_protection_power_get_item(index);
    
    return drive_prot_base_item_stable(&item->base_item);
}

void drive_protection_power_clear_item_error(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_PWR_ITEMS_COUNT) return;
    
    drive_protection_power_item_t* item = drive_protection_power_get_item(index);
    
    drive_prot_base_item_reset(&item->base_item);
}

void drive_protection_power_clear_errors(void)
{
    drive_prot_index_t i;
    for(i = 0; i < DRIVE_PROT_PWR_ITEMS_COUNT; i ++){
        drive_protection_power_clear_item_error(i);
    }
}

drive_prot_action_t drive_protection_power_item_action(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_PWR_ITEMS_COUNT) return DRIVE_PROT_ACTION_IGNORE;
    
    drive_protection_power_item_t* item = drive_protection_power_get_item(index);
    
    return drive_prot_base_item_action(&item->base_item);
}

bool drive_protection_power_item_masked(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_PWR_ITEMS_COUNT) return false;
    
    const drive_protection_power_descr_t* descr = drive_protection_power_get_item_descr(index);
    
    return drive_protection_item_masked_impl(descr);
}

bool drive_protection_power_item_allow(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_PWR_ITEMS_COUNT) return DRIVE_PROT_ITEM_ALLOW_DEFAULT;
    
    drive_protection_power_item_t* item = drive_protection_power_get_item(index);
    
    if(!drive_prot_base_item_enabled(&item->base_item)) return true;
    
    return drive_prot_base_item_allow(&item->base_item);
}

bool drive_protection_power_item_active(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_PWR_ITEMS_COUNT) return DRIVE_PROT_ITEM_ACTIVE_DEFAULT;
    
    drive_protection_power_item_t* item = drive_protection_power_get_item(index);
    
    return drive_prot_base_item_active(&item->base_item);
}

drive_power_warnings_t drive_protection_power_item_warning(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_PWR_ITEMS_COUNT) return DRIVE_POWER_WARNING_NONE;

    const drive_protection_power_descr_t* descr = drive_protection_power_get_item_descr(index);

    if(descr->flag_type == DRIVE_PROT_FLAG_WRN) return descr->flag;

    return DRIVE_POWER_WARNING_NONE;
}

drive_power_errors_t drive_protection_power_item_error(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_PWR_ITEMS_COUNT) return DRIVE_POWER_ERROR_NONE;

    const drive_protection_power_descr_t* descr = drive_protection_power_get_item_descr(index);

    if(descr->flag_type == DRIVE_PROT_FLAG_ERR) return descr->flag;

    return DRIVE_POWER_ERROR_NONE;
}

 
 /*
  * Тепловая защита.
  */
 
void drive_protection_top_process(fixed32_t I_rot, fixed32_t dt)
{
    if(!drive_prot.top.enabled) return;
    if(I_rot > drive_prot.I_rot){
        // Q_heat = (i^2 - 1) * dt
        fixed32_t i = fixed32_div((int64_t)I_rot, drive_prot.I_rot);
        fixed32_t q = fixed32_mul((int64_t)i, i);
                  q = q - fixed32_make_from_int(1);
                  q = fixed32_mul((int64_t)q, dt);
        // Интегрируем нагрев.
        drive_prot.top.cur_pie += q;
        // Клампим.
        if(drive_prot.top.cur_pie >= drive_prot.top.k_pie_max){
            drive_prot.top.cur_pie = drive_prot.top.k_pie_max;
            drive_prot.top.overheat = true;
        }
    }else if(drive_prot.top.cur_pie > 0){
        // Q_cool = (pie / max_pie + 0.58) * dt
        fixed32_t q = drive_prot.top.cur_pie;
                  q = fixed32_div((int64_t)q, drive_prot.top.k_pie_max);
                  q += fixed32_make_from_fract(58, 100);
                  q = fixed32_mul((int64_t)q, dt);
        // Интегрируем охлаждение.
        drive_prot.top.cur_pie -= q;
        // Клампим.
        if(drive_prot.top.cur_pie <= 0){
            drive_prot.top.cur_pie = 0;
            drive_prot.top.overheat = false;
        }
    }
}

drive_top_check_res_t drive_protection_top_check(void)
{
    if(drive_prot.top.cur_pie == 0) return DRIVE_TOP_CHECK_NORMAL;
    if(drive_prot.top.cur_pie == drive_prot.top.k_pie_max) return DRIVE_TOP_CHECK_OVERHEAT;
    if(drive_prot.top.overheat) return DRIVE_TOP_CHECK_COOLING;
    return DRIVE_TOP_CHECK_HEATING;
}

bool drive_protection_top_ready(void)
{
    return !drive_prot.top.overheat;
}

drive_prot_action_t drive_protection_top_action(void)
{
    return drive_prot.top.action;
}

drive_prot_action_t drive_protection_emergency_stop_action(void)
{
    return drive_prot.emergency_stop_action;
}


/*
 * Общие элементы защиты.
 */

bool drive_protection_check_item(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return false;

    drive_protection_item_t* item = drive_protection_get_item(index);

    if(!drive_prot_base_item_enabled(&item->base_item)) return false;

    const drive_protection_descr_t* descr = drive_protection_get_item_descr(index);
    
    drive_prot_item_check_proc_t check_proc = descr->check_proc;
    
    if(!check_proc) return false;
    
    bool prev_active = drive_prot_base_item_active(&item->base_item);
    bool new_active = false;
    
    bool masked = item->masked;
    
    bool valid = check_proc(item);
    
    new_active = drive_prot_check_base_item(&item->base_item, masked, valid);

    return (prev_active == false) && new_active;
}

bool drive_protection_check_items(const drive_prot_index_t* items, size_t items_count)
{
    if(items == NULL || items_count == 0) return true;

    bool res = false;

    drive_prot_index_t i;
    for(i = 0; i < items_count; i ++){
        res |= drive_protection_check_item(items[i]);
    }

    return res;
}

bool drive_protection_item_stable(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return false;

    drive_protection_item_t* item = drive_protection_get_item(index);

    return drive_prot_base_item_stable(&item->base_item);
}

void drive_protection_clear_item_error(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return;

    drive_protection_item_t* item = drive_protection_get_item(index);

    drive_prot_base_item_reset(&item->base_item);
}

void drive_protection_clear_errors(void)
{
    drive_prot_index_t i;
    for(i = 0; i < DRIVE_PROT_ITEMS_COUNT; i ++){
        drive_protection_clear_item_error(i);
    }
}

drive_prot_action_t drive_protection_item_action(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return DRIVE_PROT_ACTION_IGNORE;

    drive_protection_item_t* item = drive_protection_get_item(index);

    return drive_prot_base_item_action(&item->base_item);
}

bool drive_protection_item_masked(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return false;

    drive_protection_item_t* item = drive_protection_get_item(index);

    return item->masked;
}

void drive_protection_item_set_masked(drive_prot_index_t index, bool masked)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return;

    drive_protection_item_t* item = drive_protection_get_item(index);

    item->masked = masked;
}

bool drive_protection_item_allow(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return DRIVE_PROT_ITEM_ALLOW_DEFAULT;

    drive_protection_item_t* item = drive_protection_get_item(index);

    if(!drive_prot_base_item_enabled(&item->base_item)) return true;

    return drive_prot_base_item_allow(&item->base_item);
}

bool drive_protection_item_active(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return DRIVE_PROT_ITEM_ACTIVE_DEFAULT;

    drive_protection_item_t* item = drive_protection_get_item(index);

    return drive_prot_base_item_active(&item->base_item);
}

bool drive_protection_sensor_u_in_emulation_enabled(void);


/*
 * Данные проверки датчиков.
 */

bool drive_protection_sensor_u_in_emulation_enabled(void)
{
    return drive_prot.sensors.sensor_u_in.emulation_enabled;
}

bool drive_protection_sensor_i_in_emulation_enabled(void)
{
    return drive_prot.sensors.sensor_i_in.emulation_enabled;
}

bool drive_protection_sensor_u_rot_emulation_enabled(void)
{
    return drive_prot.sensors.sensor_u_rot.emulation_enabled;
}

bool drive_protection_sensor_i_rot_emulation_enabled(void)
{
    return drive_prot.sensors.sensor_i_rot.emulation_enabled;
}

bool drive_protection_sensor_i_exc_emulation_enabled(void)
{
    return drive_prot.sensors.sensor_i_exc.emulation_enabled;
}


/*
 * Прочие проверки.
 */

bool drive_protection_is_normal(drive_pwr_check_res_t check_res)
{
    if(check_res == DRIVE_PWR_CHECK_NORMAL) return true;
    return false;
}

bool drive_protection_is_allow(drive_pwr_check_res_t check_res)
{
    if(check_res == DRIVE_PWR_CHECK_FAULT_OVERFLOW ||
       check_res == DRIVE_PWR_CHECK_FAULT_UNDERFLOW) return false;
    return true;
}

/*
static bool drive_protection_check_delta(fixed32_t ref_value, fixed32_t cur_value, fixed32_t delta)
{
    cur_value -= ref_value;
    cur_value = fixed_abs(cur_value);
    
    return cur_value <= delta;
}

static bool drive_protection_check_range(fixed32_t cur_value, fixed32_t min_val, fixed32_t max_val)
{
    if(cur_value < min_val) return false;
    if(cur_value > max_val) return false;
    return true;
}

static bool drive_protection_check_zero(fixed32_t value, fixed32_t delta)
{
    return value <= delta;
}
*/

drive_pwr_check_res_t drive_protection_check_rot_zero_voltage(void)
{
    if(!drive_protection_power_item_allow(DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Urot)) return DRIVE_PWR_CHECK_FAULT_OVERFLOW;
    if(!drive_protection_power_item_allow(DRIVE_PROT_PWR_ITEM_WARN_IDLE_Urot)) return DRIVE_PWR_CHECK_WARN_OVERFLOW;
    return DRIVE_PWR_CHECK_NORMAL;
}

drive_pwr_check_res_t drive_protection_check_rot_zero_current(void)
{
    if(!drive_protection_power_item_allow(DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Irot)) return DRIVE_PWR_CHECK_FAULT_OVERFLOW;
    if(!drive_protection_power_item_allow(DRIVE_PROT_PWR_ITEM_WARN_IDLE_Irot)) return DRIVE_PWR_CHECK_WARN_OVERFLOW;
    return DRIVE_PWR_CHECK_NORMAL;
}

drive_pwr_check_res_t drive_protection_check_exc_zero_current(void)
{
    if(!drive_protection_power_item_allow(DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Iexc)) return DRIVE_PWR_CHECK_FAULT_OVERFLOW;
    if(!drive_protection_power_item_allow(DRIVE_PROT_PWR_ITEM_WARN_IDLE_Iexc)) return DRIVE_PWR_CHECK_WARN_OVERFLOW;
    return DRIVE_PWR_CHECK_NORMAL;
}

drive_pwr_check_res_t drive_protection_check_exc(void)
{
    if(!drive_protection_power_item_allow(DRIVE_PROT_PWR_ITEM_FAULT_OVF_Iexc)) return DRIVE_PWR_CHECK_FAULT_OVERFLOW;
    if(!drive_protection_power_item_allow(DRIVE_PROT_PWR_ITEM_FAULT_UDF_Iexc)) return DRIVE_PWR_CHECK_FAULT_UNDERFLOW;
    if(!drive_protection_power_item_allow(DRIVE_PROT_PWR_ITEM_WARN_OVF_Iexc)) return DRIVE_PWR_CHECK_WARN_OVERFLOW;
    if(!drive_protection_power_item_allow(DRIVE_PROT_PWR_ITEM_WARN_UDF_Iexc)) return DRIVE_PWR_CHECK_WARN_UNDERFLOW;
    return DRIVE_PWR_CHECK_NORMAL;
}
