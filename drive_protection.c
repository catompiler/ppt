#include "drive_protection.h"
#include <string.h>
#include "utils/utils.h"
#include "settings.h"
#include "drive_power.h"
#include "drive_phase_state.h"
#include "drive.h"
#include "drive_phase_sync.h"
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


//! Структура защиты фаз по времени между датчиками нуля.
typedef struct _Drive_Protection_Phases_Time {
    drive_prot_base_item_t base_item; //!< Базовый элемент.
    bool masked; //!< Замаскированность.
} drive_protection_phases_time_t;


//! Структура защиты фаз по углу между фазами.
typedef struct _Drive_Protection_Phases_Angle {
    drive_prot_base_item_t fault_base_item; //!< Базовый элемент ошибки.
    drive_prot_base_item_t warn_base_item; //!< Базовый элемент предупреждения.
    fixed32_t fault_value; //!< Уровень срабатывания ошибки.
    fixed32_t warn_value; //!< Уровень срабатывания предупреждения.
    bool masked; //!< Замаскированность.
} drive_protection_phases_angle_t;


//! Структура защиты фаз по синхронизации с фазами.
typedef struct _Drive_Protection_Phases_Sync {
    drive_prot_base_item_t fault_base_item; //!< Базовый элемент ошибки.
    drive_prot_base_item_t warn_base_item; //!< Базовый элемент предупреждения.
    bool masked; //!< Замаскированность.
} drive_protection_phases_sync_t;


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

//! Тип элемента защиты.
typedef struct _Drive_Prot_Descr {
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
} drive_protection_descr_t;


#define PROT_DESCR(arg_pwr_ch, arg_type, arg_flag_type, arg_flag,\
                   arg_par_ref, arg_par_ena, arg_par_lvl, arg_par_time,\
                   arg_par_lch_ena, arg_par_act)\
    {\
        .power_channel = arg_pwr_ch, .type = arg_type, .flag_type = arg_flag_type, .flag = arg_flag,\
        .param_ref = arg_par_ref, .param_ena = arg_par_ena, .param_level = arg_par_lvl, .param_time = arg_par_time,\
        .param_latch_ena = arg_par_lch_ena, .param_action = arg_par_act\
    }


#define DRIVE_PROT_PIE_MAX 0x10000
#define DRIVE_PROT_CALC_PIE_INC(time) (0x6aac1 / time) //6.667 * f32(1.0) / t_avail

//! Тип элемента защиты питания.
typedef struct _Drive_Prot_Power_Item {
    drive_prot_base_item_t base_item; //!< Базовый элемент защиты.
    fixed32_t value_level; //!< Значение уровня срабатывания защиты.
} drive_protection_power_item_t;


#define DRIVE_PROT_ITEM_ALLOW_DEFAULT false
#define DRIVE_PROT_ITEM_ACTIVE_DEFAULT false


static const drive_protection_descr_t drive_prot_items_descrs[DRIVE_PROT_ITEMS_COUNT] = {
    PROT_DESCR(DRIVE_POWER_Ua, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ua, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_OVF_FAULT_ENABLE, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_IN_OVF_FAULT_ACTION),
    PROT_DESCR(DRIVE_POWER_Ua, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_UNDERFLOW_Ua, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_UDF_FAULT_ENABLE, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_UDF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_IN_UDF_FAULT_ACTION),
    
    PROT_DESCR(DRIVE_POWER_Ua, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Ua, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_OVF_WARN_ENABLE, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_IN_OVF_WARN_ACTION),
    PROT_DESCR(DRIVE_POWER_Ua, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_UNDERFLOW_Ua, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_UDF_WARN_ENABLE, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_UDF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_IN_UDF_WARN_ACTION),
    
    
    PROT_DESCR(DRIVE_POWER_Ub, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ub, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_OVF_FAULT_ENABLE, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_IN_OVF_FAULT_ACTION),
    PROT_DESCR(DRIVE_POWER_Ub, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_UNDERFLOW_Ub, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_UDF_FAULT_ENABLE, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_UDF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_IN_UDF_FAULT_ACTION),
    
    PROT_DESCR(DRIVE_POWER_Ub, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Ub, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_OVF_WARN_ENABLE, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_IN_OVF_WARN_ACTION),
    PROT_DESCR(DRIVE_POWER_Ub, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_UNDERFLOW_Ub, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_UDF_WARN_ENABLE, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_UDF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_IN_UDF_WARN_ACTION),
    
    
    PROT_DESCR(DRIVE_POWER_Uc, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Uc, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_OVF_FAULT_ENABLE, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_IN_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_IN_OVF_FAULT_ACTION),
    PROT_DESCR(DRIVE_POWER_Uc, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_UNDERFLOW_Uc, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_UDF_FAULT_ENABLE, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_IN_UDF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_UDF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_IN_UDF_FAULT_ACTION),
    
    PROT_DESCR(DRIVE_POWER_Uc, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Uc, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_OVF_WARN_ENABLE, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_IN_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_IN_OVF_WARN_ACTION),
    PROT_DESCR(DRIVE_POWER_Uc, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_UNDERFLOW_Uc, PARAM_ID_U_NOM,\
        PARAM_ID_PROT_U_IN_UDF_WARN_ENABLE, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_IN_UDF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_IN_UDF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_IN_UDF_WARN_ACTION),
    
     
    PROT_DESCR(DRIVE_POWER_Ia, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ia, PARAM_ID_I_NOM,\
        PARAM_ID_PROT_I_IN_OVF_FAULT_ENABLE, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_IN_OVF_FAULT_ACTION),
    
    PROT_DESCR(DRIVE_POWER_Ia, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Ia, PARAM_ID_I_NOM,\
        PARAM_ID_PROT_I_IN_OVF_WARN_ENABLE, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_IN_OVF_WARN_ACTION),
    
    
    PROT_DESCR(DRIVE_POWER_Ib, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ib, PARAM_ID_I_NOM,\
        PARAM_ID_PROT_I_IN_OVF_FAULT_ENABLE, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_IN_OVF_FAULT_ACTION),
    
    PROT_DESCR(DRIVE_POWER_Ib, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Ib, PARAM_ID_I_NOM,\
        PARAM_ID_PROT_I_IN_OVF_WARN_ENABLE, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_IN_OVF_WARN_ACTION),
    
    
    PROT_DESCR(DRIVE_POWER_Ic, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ic, PARAM_ID_I_NOM,\
        PARAM_ID_PROT_I_IN_OVF_FAULT_ENABLE, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_IN_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_IN_OVF_FAULT_ACTION),
    
    PROT_DESCR(DRIVE_POWER_Ic, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Ic, PARAM_ID_I_NOM,\
        PARAM_ID_PROT_I_IN_OVF_WARN_ENABLE, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_IN_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_IN_OVF_WARN_ACTION),
    
    
    PROT_DESCR(DRIVE_POWER_Urot, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Urot, PARAM_ID_U_ROT_NOM,\
        PARAM_ID_PROT_U_ROT_OVF_FAULT_ENABLE, PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_ROT_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_ROT_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_ROT_OVF_FAULT_ACTION),
    PROT_DESCR(DRIVE_POWER_Urot, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Urot, PARAM_ID_U_ROT_NOM,\
        PARAM_ID_PROT_U_ROT_OVF_WARN_ENABLE, PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_ROT_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_ROT_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_ROT_OVF_WARN_ACTION),
    
    
    PROT_DESCR(DRIVE_POWER_Irot, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Irot, PARAM_ID_I_ROT_NOM,\
        PARAM_ID_PROT_I_ROT_OVF_FAULT_ENABLE, PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_ROT_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_ROT_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_ROT_OVF_FAULT_ACTION),
    PROT_DESCR(DRIVE_POWER_Irot, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Irot, PARAM_ID_I_ROT_NOM,\
        PARAM_ID_PROT_I_ROT_OVF_WARN_ENABLE, PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_ROT_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_ROT_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_ROT_OVF_WARN_ACTION),
    
    
    PROT_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Iexc, PARAM_ID_I_EXC,\
        PARAM_ID_PROT_I_EXC_OVF_FAULT_ENABLE, PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_EXC_OVF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_EXC_OVF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_EXC_OVF_FAULT_ACTION),
    PROT_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_UNDERFLOW_Iexc, PARAM_ID_I_EXC,\
        PARAM_ID_PROT_I_EXC_UDF_FAULT_ENABLE, PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_EXC_UDF_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_EXC_UDF_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_EXC_UDF_FAULT_ACTION),
    PROT_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_OVF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_OVERFLOW_Iexc, PARAM_ID_I_EXC,\
        PARAM_ID_PROT_I_EXC_OVF_WARN_ENABLE, PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_EXC_OVF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_EXC_OVF_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_EXC_OVF_WARN_ACTION),
    PROT_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_UDF, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_UNDERFLOW_Iexc, PARAM_ID_I_EXC,\
        PARAM_ID_PROT_I_EXC_UDF_WARN_ENABLE, PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_EXC_UDF_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_EXC_UDF_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_EXC_UDF_WARN_ACTION),
    
     
    PROT_DESCR(DRIVE_POWER_Ia, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_IDLE_Ia, 0,\
        PARAM_ID_PROT_I_IN_IDLE_FAULT_ENABLE, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_IDLE_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_IN_IDLE_FAULT_ACTION),
    
    PROT_DESCR(DRIVE_POWER_Ia, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_IDLE_Ia, 0,\
        PARAM_ID_PROT_I_IN_IDLE_WARN_ENABLE, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_IDLE_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_IN_IDLE_WARN_ACTION),
    
     
    PROT_DESCR(DRIVE_POWER_Ib, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_IDLE_Ib, 0,\
        PARAM_ID_PROT_I_IN_IDLE_FAULT_ENABLE, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_IDLE_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_IN_IDLE_FAULT_ACTION),
    
    PROT_DESCR(DRIVE_POWER_Ib, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_IDLE_Ib, 0,\
        PARAM_ID_PROT_I_IN_IDLE_WARN_ENABLE, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_IDLE_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_IN_IDLE_WARN_ACTION),
    
     
    PROT_DESCR(DRIVE_POWER_Ic, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_IDLE_Ic, 0,\
        PARAM_ID_PROT_I_IN_IDLE_FAULT_ENABLE, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_IDLE_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_IN_IDLE_FAULT_ACTION),
    
    PROT_DESCR(DRIVE_POWER_Ic, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_IDLE_Ic, 0,\
        PARAM_ID_PROT_I_IN_IDLE_WARN_ENABLE, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_IN_IDLE_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_IN_IDLE_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_IN_IDLE_WARN_ACTION),
    
     
    PROT_DESCR(DRIVE_POWER_Urot, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_IDLE_Urot, 0,\
        PARAM_ID_PROT_U_ROT_IDLE_FAULT_ENABLE, PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_VALUE, PARAM_ID_PROT_U_ROT_IDLE_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_ROT_IDLE_FAULT_LATCH_ENABLE, PARAM_ID_PROT_U_ROT_IDLE_FAULT_ACTION),
    
    PROT_DESCR(DRIVE_POWER_Urot, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_IDLE_Urot, 0,\
        PARAM_ID_PROT_U_ROT_IDLE_WARN_ENABLE, PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_VALUE, PARAM_ID_PROT_U_ROT_IDLE_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_U_ROT_IDLE_WARN_LATCH_ENABLE, PARAM_ID_PROT_U_ROT_IDLE_WARN_ACTION),
    
     
    PROT_DESCR(DRIVE_POWER_Irot, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_IDLE_Irot, 0,\
        PARAM_ID_PROT_I_ROT_IDLE_FAULT_ENABLE, PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_ROT_IDLE_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_ROT_IDLE_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_ROT_IDLE_FAULT_ACTION),
    
    PROT_DESCR(DRIVE_POWER_Irot, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_IDLE_Irot, 0,\
        PARAM_ID_PROT_I_ROT_IDLE_WARN_ENABLE, PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_ROT_IDLE_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_ROT_IDLE_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_ROT_IDLE_WARN_ACTION),
    
     
    PROT_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_IDLE_Iexc, 0,\
        PARAM_ID_PROT_I_EXC_IDLE_FAULT_ENABLE, PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_VALUE, PARAM_ID_PROT_I_EXC_IDLE_FAULT_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_EXC_IDLE_FAULT_LATCH_ENABLE, PARAM_ID_PROT_I_EXC_IDLE_FAULT_ACTION),
    
    PROT_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_ZERO, DRIVE_PROT_FLAG_WRN, DRIVE_POWER_WARNING_IDLE_Iexc, 0,\
        PARAM_ID_PROT_I_EXC_IDLE_WARN_ENABLE, PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_VALUE, PARAM_ID_PROT_I_EXC_IDLE_WARN_LEVEL_TIME_MS,\
        PARAM_ID_PROT_I_EXC_IDLE_WARN_LATCH_ENABLE, PARAM_ID_PROT_I_EXC_IDLE_WARN_ACTION),

    
    PROT_DESCR(DRIVE_POWER_Ua, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ua, PARAM_ID_U_NOM,\
        0, PARAM_ID_PROT_U_IN_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_DESCR(DRIVE_POWER_Ub, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ub, PARAM_ID_U_NOM,\
        0, PARAM_ID_PROT_U_IN_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_DESCR(DRIVE_POWER_Uc, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Uc, PARAM_ID_U_NOM,\
        0, PARAM_ID_PROT_U_IN_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
     
    PROT_DESCR(DRIVE_POWER_Ia, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ia, PARAM_ID_I_NOM,\
        0, PARAM_ID_PROT_I_IN_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_DESCR(DRIVE_POWER_Ib, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ib, PARAM_ID_I_NOM,\
        0, PARAM_ID_PROT_I_IN_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_DESCR(DRIVE_POWER_Ic, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Ic, PARAM_ID_I_NOM,\
        0, PARAM_ID_PROT_I_IN_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_DESCR(DRIVE_POWER_Urot, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Urot, PARAM_ID_U_ROT_NOM,\
        0, PARAM_ID_PROT_U_ROT_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_DESCR(DRIVE_POWER_Irot, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Irot, PARAM_ID_I_ROT_NOM,\
        0, PARAM_ID_PROT_I_ROT_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
    
    PROT_DESCR(DRIVE_POWER_Iexc, DRIVE_PROT_TYPE_CUT, DRIVE_PROT_FLAG_ERR, DRIVE_POWER_ERROR_OVERFLOW_Iexc, PARAM_ID_I_EXC,\
        0, PARAM_ID_PROT_I_EXC_CUTOFF_LEVEL_VALUE, 0,\
        0, 0),
};

//#define DRIVE_PROT_ITEMS_COUNT (sizeof(drive_prot_items_descrs) / sizeof(drive_protection_descr_t))


//! Тип структуры защиты привода.
typedef struct _Drive_Protection {
    fixed32_t U_in; //!< Номинальное напряжение, В.
    fixed32_t U_rot; //!< Номинальное напряжение якоря.
    fixed32_t I_rot; //!< Номинальный ток якоря.
    fixed32_t I_exc; //!< Номинальный ток возбуждения.
    drive_protection_power_item_t prot_items[DRIVE_PROT_ITEMS_COUNT];
    drive_power_errors_t prot_errs_mask; //!< Маска ошибок защиты.
    drive_power_warnings_t prot_warn_mask; //!< Маска предупреждений защиты.
    drive_power_errors_t prot_cutoff_errs_mask; //!< Маска ошибок защиты отсечки.
    drive_power_warnings_t prot_cutoff_warn_mask; //!< Маска предупреждений защиты отсечки.
    drive_top_t top; //!< Тепловая защита.
    drive_protection_phases_time_t prot_phases_time; //!< Защита фаз по времени между датчиками нуля.
    drive_protection_phases_angle_t prot_phases_angles; //!< Защита фаз по углу между фазами.
    drive_protection_phases_sync_t prot_phases_sync; //!< Защита фаз по синхронизации с фазами.
    drive_prot_action_t emergency_stop_action; //!< Действие при нажатии грибка.
} drive_protection_t;

//! Структура защиты привода.
static drive_protection_t drive_prot;


/*
 * Функционал базовых элементов защиты.
 */

static void drive_prot_update_base_item_settings(drive_prot_base_item_t* item,
                        param_id_t param_ena, param_id_t param_time, param_id_t param_latch_ena, param_id_t param_action)
{
    // Если отсутствует элемент - возврат.
    if(item == NULL) return;
    
    // Разрешение элемента защиты.
    if(param_ena != 0){
        item->enabled = (bool)settings_valueu(param_ena);
    }else{
        item->enabled = false;
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
        item->action = DRIVE_PROT_ACTION_IGNORE;
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


bool drive_protection_init(void)
{
    memset(&drive_prot, 0x0, sizeof(drive_protection_t));
    
    return true;
}

ALWAYS_INLINE static drive_protection_power_item_t* drive_protection_get_prot_item(drive_prot_index_t index)
{
    return &drive_prot.prot_items[index];
}

ALWAYS_INLINE static const drive_protection_descr_t* drive_protection_get_prot_item_descr(drive_prot_index_t index)
{
    return &drive_prot_items_descrs[index];
}

ALWAYS_INLINE static fixed32_t drive_protection_get_part(fixed32_t value, uint32_t percents)
{
    return value * percents / 100;
}

ALWAYS_INLINE static fixed32_t drive_protection_get_ovf_level(fixed32_t value, uint32_t percents)
{
    return value + drive_protection_get_part(value, percents);
}

ALWAYS_INLINE static fixed32_t drive_protection_get_udf_level(fixed32_t value, uint32_t percents)
{
    return value - drive_protection_get_part(value, percents);
}

static void drive_prot_update_prot_item_settings(drive_prot_index_t index)
{
    // Если превышен индекс - возврат.
    if(index >= DRIVE_PROT_ITEMS_COUNT) return;
    
    // Элемент защиты.
    drive_protection_power_item_t* item = drive_protection_get_prot_item(index);
    // Дескриптор элемента защиты.
    const drive_protection_descr_t* descr = drive_protection_get_prot_item_descr(index);
    
    drive_prot_update_base_item_settings(&item->base_item,
            descr->param_ena, descr->param_time, descr->param_latch_ena, descr->param_action);
    
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
 * Обновляет настройки элементов защиты.
 */
static void drive_prot_update_prot_items_settings(void)
{
    drive_prot_index_t i;
    for(i = 0; i < DRIVE_PROT_ITEMS_COUNT; i ++){
        drive_prot_update_prot_item_settings(i);
    }
}

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

/**
 * Обновляет настройки защиты фаз по времени между фазами.
 */
static void drive_protection_update_phases_time_prot_settings(void)
{
    drive_prot_update_base_item_settings(&drive_prot.prot_phases_time.base_item,
            PARAM_ID_PROT_PHASES_TIME_ENABLED, PARAM_ID_PROT_PHASES_TIME_TIME_MS, 0, PARAM_ID_PROT_PHASES_TIME_ACTION);
}

/**
 * Обновляет настройки защиты фаз по углу между фазами.
 */
static void drive_protection_update_phases_angles_prot_settings(void)
{
    drive_prot_update_base_item_settings(&drive_prot.prot_phases_angles.fault_base_item,
            PARAM_ID_PROT_PHASES_ANGLES_FAULT_ENABLED, PARAM_ID_PROT_PHASES_ANGLES_FAULT_TIME_MS,
            PARAM_ID_PROT_PHASES_ANGLES_FAULT_LATCH_ENABLE, PARAM_ID_PROT_PHASES_ANGLES_FAULT_ACTION);
    drive_prot_update_base_item_settings(&drive_prot.prot_phases_angles.warn_base_item,
            PARAM_ID_PROT_PHASES_ANGLES_WARN_ENABLED, PARAM_ID_PROT_PHASES_ANGLES_WARN_TIME_MS,
            PARAM_ID_PROT_PHASES_ANGLES_WARN_LATCH_ENABLE, PARAM_ID_PROT_PHASES_ANGLES_WARN_ACTION);
    
    drive_prot.prot_phases_angles.fault_value = settings_valuef(PARAM_ID_PROT_PHASES_ANGLES_FAULT_VALUE);
    drive_prot.prot_phases_angles.warn_value = settings_valuef(PARAM_ID_PROT_PHASES_ANGLES_WARN_VALUE);
}

/**
 * Обновляет настройки защиты фаз по синхронизации с фазами.
 */
static void drive_protection_update_phases_sync_prot_settings(void)
{
    drive_prot_update_base_item_settings(&drive_prot.prot_phases_sync.fault_base_item,
            PARAM_ID_PROT_PHASES_SYNC_FAULT_ENABLED, PARAM_ID_PROT_PHASES_SYNC_FAULT_TIME_MS,
            PARAM_ID_PROT_PHASES_SYNC_FAULT_LATCH_ENABLE, PARAM_ID_PROT_PHASES_SYNC_FAULT_ACTION);
    drive_prot_update_base_item_settings(&drive_prot.prot_phases_sync.warn_base_item,
            PARAM_ID_PROT_PHASES_SYNC_WARN_ENABLED, PARAM_ID_PROT_PHASES_SYNC_WARN_TIME_MS,
            PARAM_ID_PROT_PHASES_SYNC_WARN_LATCH_ENABLE, PARAM_ID_PROT_PHASES_SYNC_WARN_ACTION);
}

void drive_protection_update_settings(void)
{
    drive_prot.U_in = settings_valuef(PARAM_ID_U_NOM);
    drive_prot.U_rot = settings_valuef(PARAM_ID_U_ROT_NOM);
    drive_prot.I_rot = settings_valuef(PARAM_ID_I_ROT_NOM);
    drive_prot.I_exc = settings_valuef(PARAM_ID_I_EXC);
    
    drive_protection_update_top_settings();
    
    drive_protection_update_phases_time_prot_settings();
    
    drive_protection_update_phases_angles_prot_settings();
    
    drive_protection_update_phases_sync_prot_settings();
    
    drive_prot.emergency_stop_action = settings_valueu(PARAM_ID_EMERGENCY_STOP_ACTION);
    
    drive_prot_update_prot_items_settings();
}

drive_power_errors_t drive_protection_errs_mask(void)
{
    return drive_prot.prot_errs_mask;
}

void drive_protection_set_errs_mask(drive_power_errors_t errs_mask)
{
    drive_prot.prot_errs_mask = errs_mask;
}

void drive_protection_set_errs_mask_flags(drive_power_errors_t errs_flags)
{
    drive_prot.prot_errs_mask |= errs_flags;
}

void drive_protection_reset_errs_mask_flags(drive_power_errors_t errs_flags)
{
    drive_prot.prot_errs_mask &= ~errs_flags;
}

drive_power_warnings_t drive_protection_warn_mask(void)
{
    return drive_prot.prot_warn_mask;
}

void drive_protection_set_warn_mask(drive_power_warnings_t warn_mask)
{
    drive_prot.prot_warn_mask = warn_mask;
}

void drive_protection_set_warn_mask_flags(drive_power_warnings_t warn_flags)
{
    drive_prot.prot_warn_mask |= warn_flags;
}

void drive_protection_reset_warn_mask_flags(drive_power_warnings_t warn_flags)
{
    drive_prot.prot_warn_mask &= ~warn_flags;
}

drive_power_errors_t drive_protection_cutoff_errs_mask(void)
{
    return drive_prot.prot_cutoff_errs_mask;
}

void drive_protection_set_cutoff_errs_mask(drive_power_errors_t errs_mask)
{
    drive_prot.prot_cutoff_errs_mask = errs_mask;
}

void drive_protection_set_cutoff_errs_mask_flags(drive_power_errors_t errs_flags)
{
    drive_prot.prot_cutoff_errs_mask |= errs_flags;
}

void drive_protection_reset_cutoff_errs_mask_flags(drive_power_errors_t errs_flags)
{
    drive_prot.prot_cutoff_errs_mask &= ~errs_flags;
}

drive_power_warnings_t drive_protection_cutoff_warn_mask(void)
{
    return drive_prot.prot_cutoff_warn_mask;
}

void drive_protection_set_cutoff_warn_mask(drive_power_warnings_t warn_mask)
{
    drive_prot.prot_cutoff_warn_mask = warn_mask;
}

void drive_protection_set_cutoff_warn_mask_flags(drive_power_warnings_t warn_flags)
{
    drive_prot.prot_cutoff_warn_mask |= warn_flags;
}

void drive_protection_reset_cutoff_warn_mask_flags(drive_power_warnings_t warn_flags)
{
    drive_prot.prot_cutoff_warn_mask &= ~warn_flags;
}

/**
 * Выполняет проверку защиты фаз по времени между фазами.
 * @return Флаг срабатывания элемента защиты.
 */
static bool drive_protection_check_phases_errors(void)
{
    if(!drive_prot.prot_phases_time.base_item.enabled) return false;
    
    drive_prot_check_base_item(&drive_prot.prot_phases_time.base_item,
                drive_prot.prot_phases_time.masked, drive_phase_state_errors() == PHASE_NO_ERROR);
    
    return drive_prot.prot_phases_time.base_item.active;
}

/**
 * Выполняет проверку уровня ошибки защиты фаз по углу между фазами.
 * @return Флаг срабатывания элемента защиты.
 */
static bool drive_protection_check_phases_angles_fault(void)
{
    if(!drive_prot.prot_phases_angles.fault_base_item.enabled) return false;
    
    fixed32_t angle = 0;
    
    bool valid = true;
    
    angle = drive_phase_sync_diff_delta_angle(PHASE_A); angle = fixed_abs(angle);
    valid &= angle <= drive_prot.prot_phases_angles.fault_value;
    
    angle = drive_phase_sync_diff_delta_angle(PHASE_B); angle = fixed_abs(angle);
    valid &= angle <= drive_prot.prot_phases_angles.fault_value;
    
    angle = drive_phase_sync_diff_delta_angle(PHASE_C); angle = fixed_abs(angle);
    valid &= angle <= drive_prot.prot_phases_angles.fault_value;
    
    drive_prot_check_base_item(&drive_prot.prot_phases_angles.fault_base_item,
                drive_prot.prot_phases_angles.masked, valid);
    
    return drive_prot.prot_phases_angles.fault_base_item.active;
}

/**
 * Выполняет проверку уровня предупреждения защиты фаз по углу между фазами.
 * @return Флаг срабатывания элемента защиты.
 */
static bool drive_protection_check_phases_angles_warning(void)
{
    if(!drive_prot.prot_phases_angles.warn_base_item.enabled) return false;
    
    fixed32_t angle = 0;
    
    bool valid = true;
    
    angle = drive_phase_sync_diff_delta_angle(PHASE_A); angle = fixed_abs(angle);
    valid &= angle <= drive_prot.prot_phases_angles.warn_value;
    
    angle = drive_phase_sync_diff_delta_angle(PHASE_B); angle = fixed_abs(angle);
    valid &= angle <= drive_prot.prot_phases_angles.warn_value;
    
    angle = drive_phase_sync_diff_delta_angle(PHASE_C); angle = fixed_abs(angle);
    valid &= angle <= drive_prot.prot_phases_angles.warn_value;
    
    drive_prot_check_base_item(&drive_prot.prot_phases_angles.warn_base_item,
                drive_prot.prot_phases_angles.masked, valid);
    
    return drive_prot.prot_phases_angles.warn_base_item.active;
}

/**
 * Выполняет проверку уровня ошибки защиты фаз по синхронизации с фазами.
 * @return Флаг срабатывания элемента защиты.
 */
static bool drive_protection_check_phases_sync_fault(void)
{
    if(!drive_prot.prot_phases_sync.fault_base_item.enabled) return false;
    
    drive_prot_check_base_item(&drive_prot.prot_phases_sync.fault_base_item,
                drive_prot.prot_phases_sync.masked, drive_phase_sync_synchronized());
    
    return drive_prot.prot_phases_sync.fault_base_item.active;
}

/**
 * Выполняет проверку уровня предупреждения защиты фаз по синхронизации с фазами.
 * @return Флаг срабатывания элемента защиты.
 */
static bool drive_protection_check_phases_sync_warning(void)
{
    if(!drive_prot.prot_phases_sync.warn_base_item.enabled) return false;
    
    drive_prot_check_base_item(&drive_prot.prot_phases_sync.warn_base_item,
                drive_prot.prot_phases_sync.masked, drive_phase_sync_synchronized());
    
    return drive_prot.prot_phases_sync.warn_base_item.active;
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
    if(type != DRIVE_PROT_TYPE_UDF){
        return value <= level;
    }else{
        return value >= level;
    }
}

/**
 * Получает наличие элемента защиты в маске ошибок или предупреждений.
 * @param item Элемент защиты.
 * @param descr Дескриптор элемента защиты.
 * @return Флаг наличия элемента защиты в маске ошибок или предупреждений.
 */
static bool drive_protection_item_masked_impl(const drive_protection_descr_t* descr)
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
static bool drive_protection_check_power_item_real(drive_protection_power_item_t* item, const drive_protection_descr_t* descr, drive_power_warnings_t* warnings, drive_power_errors_t* errors)
{
    bool masked = drive_protection_item_masked_impl(descr);
    
    fixed32_t value = drive_power_channel_real_value(descr->power_channel);
    
    bool valid = drive_protection_check_item_value(descr->type, value, item->value_level);
    
    drive_prot_check_base_item(&item->base_item, masked, valid);
    
    if(item->base_item.active){
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
static bool drive_protection_check_power_item_inst(drive_protection_power_item_t* item, const drive_protection_descr_t* descr, drive_power_warnings_t* warnings, drive_power_errors_t* errors)
{
    bool masked = drive_protection_item_masked_impl(descr);
    
    fixed32_t value = drive_power_channel_real_value_inst(descr->power_channel);
    
    if(!drive_protection_check_item_value(descr->type, value, item->value_level)){
        
        item->base_item.allow = false;
        
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
        item->base_item.allow = true;
        item->base_item.active = false;
    }
    
    return item->base_item.active;
}

bool drive_protection_check_item(drive_prot_index_t index, drive_power_warnings_t* warnings, drive_power_errors_t* errors)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return false;
    
    drive_protection_power_item_t* item = drive_protection_get_prot_item(index);
    
    if(!item->base_item.enabled) return false;
    
    const drive_protection_descr_t* descr = drive_protection_get_prot_item_descr(index);
    
    bool prev_active = item->base_item.active;
    bool new_active = false;
    
    if(descr->type != DRIVE_PROT_TYPE_CUT){//fault || warn
        new_active = drive_protection_check_power_item_real(item, descr, warnings, errors);
    }else{//cutoff
        new_active = drive_protection_check_power_item_inst(item, descr, warnings, errors);
    }
    
    return (prev_active == false) && new_active;
}

bool drive_protection_check_power_items(const drive_prot_index_t* items, size_t items_count, drive_power_warnings_t* warnings, drive_power_errors_t* errors)
{
    if(items == NULL || items_count == 0) return true;
    
    bool res = false;
    
    drive_prot_index_t i;
    for(i = 0; i < items_count; i ++){
        res |= drive_protection_check_item(items[i], warnings, errors);
    }
    
    return res;
}

bool drive_protection_power_item_stable(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return false;
    
    drive_protection_power_item_t* item = drive_protection_get_prot_item(index);
    
    if(item->base_item.allow && item->base_item.pie == 0) return true;
    
    if(!item->base_item.allow && item->base_item.pie == DRIVE_PROT_PIE_MAX) return true;
    
    return false;
}

void drive_protection_clear_power_item_error(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return;
    
    drive_protection_power_item_t* item = drive_protection_get_prot_item(index);
    
    drive_prot_base_item_reset(&item->base_item);
}

void drive_protection_clear_power_errors(void)
{
    drive_prot_index_t i;
    for(i = 0; i < DRIVE_PROT_ITEMS_COUNT; i ++){
        drive_protection_clear_power_item_error(i);
    }
}

drive_prot_action_t drive_protection_item_action(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return DRIVE_PROT_ACTION_IGNORE;
    
    drive_protection_power_item_t* item = drive_protection_get_prot_item(index);
    
    return item->base_item.action;
}

bool drive_protection_item_masked(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return false;
    
    const drive_protection_descr_t* descr = drive_protection_get_prot_item_descr(index);
    
    return drive_protection_item_masked_impl(descr);
}

bool drive_protection_item_allow(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return DRIVE_PROT_ITEM_ALLOW_DEFAULT;
    
    drive_protection_power_item_t* item = drive_protection_get_prot_item(index);
    
    if(!item->base_item.enabled) return true;
    
    return item->base_item.allow;
}

bool drive_protection_item_active(drive_prot_index_t index)
{
    if(index >= DRIVE_PROT_ITEMS_COUNT) return DRIVE_PROT_ITEM_ACTIVE_DEFAULT;
    
    drive_protection_power_item_t* item = drive_protection_get_prot_item(index);
    
    return item->base_item.active;
}

 drive_power_warnings_t drive_protection_item_warning(drive_prot_index_t index)
 {
     if(index >= DRIVE_PROT_ITEMS_COUNT) return DRIVE_POWER_WARNING_NONE;
    
    const drive_protection_descr_t* descr = drive_protection_get_prot_item_descr(index);
    
    if(descr->flag_type == DRIVE_PROT_FLAG_WRN) return descr->flag;
    
    return DRIVE_POWER_WARNING_NONE;
 }

 drive_power_errors_t drive_protection_item_error(drive_prot_index_t index)
 {
     if(index >= DRIVE_PROT_ITEMS_COUNT) return DRIVE_POWER_ERROR_NONE;
    
    const drive_protection_descr_t* descr = drive_protection_get_prot_item_descr(index);
    
    if(descr->flag_type == DRIVE_PROT_FLAG_ERR) return descr->flag;
    
    return DRIVE_POWER_ERROR_NONE;
 }

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


bool drive_protection_phases_time_check(void)
{
    bool prev_active = drive_prot.prot_phases_time.base_item.active;
    bool new_active = drive_protection_check_phases_errors();
    
    return (prev_active == false) && new_active;
}

bool drive_protection_phases_time_allow(void)
{
    return drive_prot.prot_phases_time.base_item.allow;
}

bool drive_protection_phases_time_active(void)
{
    return drive_prot.prot_phases_time.base_item.active;
}

drive_prot_action_t drive_protection_phases_time_action(void)
{
    return drive_prot.prot_phases_time.base_item.action;
}

void drive_protection_clear_phases_time_errors(void)
{
    drive_prot_base_item_reset(&drive_prot.prot_phases_time.base_item);
}

bool drive_protection_phases_time_masked(void)
{
    return drive_prot.prot_phases_time.masked;
}

void drive_protection_phases_time_set_masked(bool masked)
{
    drive_prot.prot_phases_time.masked = masked;
}


bool drive_protection_phases_angles_fault_check(void)
{
    bool prev_active = drive_prot.prot_phases_angles.fault_base_item.active;
    bool new_active = drive_protection_check_phases_angles_fault();
    
    return (prev_active == false) && new_active;
}

bool drive_protection_phases_angles_fault_allow(void)
{
    return drive_prot.prot_phases_angles.fault_base_item.allow;
}

bool drive_protection_phases_angles_fault_active(void)
{
    return drive_prot.prot_phases_angles.fault_base_item.active;
}

drive_prot_action_t drive_protection_phases_angles_fault_action(void)
{
    return drive_prot.prot_phases_angles.fault_base_item.action;
}

bool drive_protection_phases_angles_warn_check(void)
{
    bool prev_active = drive_prot.prot_phases_angles.warn_base_item.active;
    bool new_active = drive_protection_check_phases_angles_warning();
    
    return (prev_active == false) && new_active;
}

bool drive_protection_phases_angles_warn_allow(void)
{
    return drive_prot.prot_phases_angles.warn_base_item.allow;
}

bool drive_protection_phases_angles_warn_active(void)
{
    return drive_prot.prot_phases_angles.warn_base_item.active;
}

drive_prot_action_t drive_protection_phases_angles_warn_action(void)
{
    return drive_prot.prot_phases_angles.warn_base_item.action;
}

void drive_protection_clear_phases_angles_errors(void)
{
    drive_prot_base_item_reset(&drive_prot.prot_phases_angles.fault_base_item);
    drive_prot_base_item_reset(&drive_prot.prot_phases_angles.warn_base_item);
}

bool drive_protection_phases_angles_masked(void)
{
    return drive_prot.prot_phases_angles.masked;
}

void drive_protection_phases_angles_set_masked(bool masked)
{
    drive_prot.prot_phases_angles.masked = masked;
}


bool drive_protection_phases_sync_fault_check(void)
{
    bool prev_active = drive_prot.prot_phases_sync.fault_base_item.active;
    bool new_active = drive_protection_check_phases_sync_fault();
    
    return (prev_active == false) && new_active;
}

bool drive_protection_phases_sync_fault_allow(void)
{
    return drive_prot.prot_phases_sync.fault_base_item.allow;
}

bool drive_protection_phases_sync_fault_active(void)
{
    return drive_prot.prot_phases_sync.fault_base_item.active;
}

drive_prot_action_t drive_protection_phases_sync_fault_action(void)
{
    return drive_prot.prot_phases_sync.fault_base_item.action;
}

bool drive_protection_phases_sync_warn_check(void)
{
    bool prev_active = drive_prot.prot_phases_sync.warn_base_item.active;
    bool new_active = drive_protection_check_phases_sync_warning();
    
    return (prev_active == false) && new_active;
}

bool drive_protection_phases_sync_warn_allow(void)
{
    return drive_prot.prot_phases_sync.warn_base_item.allow;
}

bool drive_protection_phases_sync_warn_active(void)
{
    return drive_prot.prot_phases_sync.warn_base_item.active;
}

drive_prot_action_t drive_protection_phases_sync_warn_action(void)
{
    return drive_prot.prot_phases_sync.warn_base_item.action;
}

void drive_protection_clear_phases_sync_errors(void)
{
    drive_prot_base_item_reset(&drive_prot.prot_phases_sync.fault_base_item);
    drive_prot_base_item_reset(&drive_prot.prot_phases_sync.warn_base_item);
}

bool drive_protection_phases_sync_masked(void)
{
    return drive_prot.prot_phases_sync.masked;
}

void drive_protection_phases_sync_set_masked(bool masked)
{
    drive_prot.prot_phases_sync.masked = masked;
}


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
    if(!drive_protection_item_allow(DRIVE_PROT_ITEM_FAULT_IDLE_Urot)) return DRIVE_PWR_CHECK_FAULT_OVERFLOW;
    if(!drive_protection_item_allow(DRIVE_PROT_ITEM_WARN_IDLE_Urot)) return DRIVE_PWR_CHECK_WARN_OVERFLOW;
    return DRIVE_PWR_CHECK_NORMAL;
}

drive_pwr_check_res_t drive_protection_check_exc_zero_current(void)
{
    if(!drive_protection_item_allow(DRIVE_PROT_ITEM_FAULT_IDLE_Iexc)) return DRIVE_PWR_CHECK_FAULT_OVERFLOW;
    if(!drive_protection_item_allow(DRIVE_PROT_ITEM_WARN_IDLE_Iexc)) return DRIVE_PWR_CHECK_WARN_OVERFLOW;
    return DRIVE_PWR_CHECK_NORMAL;
}

drive_pwr_check_res_t drive_protection_check_exc(void)
{
    if(!drive_protection_item_allow(DRIVE_PROT_ITEM_FAULT_OVF_Iexc)) return DRIVE_PWR_CHECK_FAULT_OVERFLOW;
    if(!drive_protection_item_allow(DRIVE_PROT_ITEM_FAULT_UDF_Iexc)) return DRIVE_PWR_CHECK_FAULT_UNDERFLOW;
    if(!drive_protection_item_allow(DRIVE_PROT_ITEM_WARN_OVF_Iexc)) return DRIVE_PWR_CHECK_WARN_OVERFLOW;
    if(!drive_protection_item_allow(DRIVE_PROT_ITEM_WARN_UDF_Iexc)) return DRIVE_PWR_CHECK_WARN_UNDERFLOW;
    return DRIVE_PWR_CHECK_NORMAL;
}
