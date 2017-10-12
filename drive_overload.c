#include "drive_overload.h"
#include "drive_power.h"
#include "settings.h"
#include <string.h>

//! Структура перегруза привода.
typedef struct _Drive_Overload {
    bool enabled; //!< Разрешение перегруза.
    drive_overload_mode_t mode; //!< Режим перегруза.
    fixed32_t I_base; //!< Базовый ток.
    fixed32_t I_max; //!< Максимальный ток.
    fixed32_t I_allow; //!< Допустимый ток с учётом зоны нечувствительности.
    
    bool overloaded; //!< Флаг достижения максимального перегруза.
    
    fixed32_t q_max; //!< Максимальное выделенное тепло.
    fixed32_t dq_heat; //!< Шаг при нормальном режиме перегруза.
    fixed32_t dq_cool; //!< Шаг охлаждения.
    fixed32_t q_pie; //!< Накопленное тепло.
} drive_overload_t;

//! Перегруз привода.
static drive_overload_t overload;



static fixed32_t drive_overload_calc_q(fixed32_t I, fixed32_t t)
{
    fixed32_t i = fixed32_div((int64_t)I, overload.I_base);
    fixed32_t q = fixed32_mul((int64_t)i, i);
              q = q - fixed32_make_from_int(1);
              q = fixed32_mul((int64_t)q, t);
    
    return q;
}

void drive_overload_init(void)
{
    memset(&overload, 0x0, sizeof(drive_overload_t));
}

err_t drive_overload_update_settings(void)
{
    err_t err = E_NO_ERROR;
    
    overload.mode = settings_valueu(PARAM_ID_REGULATOR_OVERLOAD_MODE);
    overload.I_base = settings_valuef(PARAM_ID_REGULATOR_OVERLOAD_BASE_CURRENT);
    overload.I_max = settings_valuef(PARAM_ID_REGULATOR_OVERLOAD_MAX_CURRENT);
    
    // Если токи не нулевые и максимальный ток больше базового.
    if(overload.I_base > 0 && overload.I_max > overload.I_base){
    
        uint32_t time_heat = settings_valueu(PARAM_ID_REGULATOR_OVERLOAD_TIME);
        uint32_t period = settings_valueu(PARAM_ID_REGULATOR_OVERLOAD_PERIOD);
        
        uint32_t dead_zone = settings_valueu(PARAM_ID_REGULATOR_OVERLOAD_DEAD_ZONE);
        
        overload.I_allow = (int64_t)overload.I_base * (100 + dead_zone) / 100;

        // Если времена не нулевые и период перегруза больше времени перегруза.
        if(period > time_heat && time_heat > 0){
            
            uint32_t time_cool = period - time_heat;
            
            overload.q_max = drive_overload_calc_q(overload.I_max, fixed32_make_from_int(time_heat));
            overload.dq_heat = overload.q_max / time_heat;
            overload.dq_cool = overload.q_max / time_cool;
        }else{
            err = E_INVALID_VALUE;
        }
    }else{
        err = E_INVALID_VALUE;
    }
    
    // Если инициализация прошла без ошибок.
    if(err == E_NO_ERROR){
        // Получим разрешение из настроек.
        overload.enabled = settings_valueu(PARAM_ID_REGULATOR_OVERLOAD_ENABLED);
    }else{
        // Иначе запретим перегруз.
        overload.enabled = false;
    }
    
    // Если перегруз запрещён.
    if(!overload.enabled){
        // Сбросим накопленные данные.
        overload.q_pie = 0;
        overload.overloaded = false;
    }
    
    return err;
}

bool drive_overload_enabled(void)
{
    return overload.enabled;
}

bool drive_overload_overloaded(void)
{
    return overload.overloaded;
}

fixed32_t drive_overload_avail_current(void)
{
    if(!overload.overloaded) return overload.I_max;
    return overload.I_base;
}

void drive_overload_process(fixed32_t dt)
{
    if(!overload.enabled) return;
    
    fixed32_t I = drive_power_channel_real_value(DRIVE_POWER_Irot);
    fixed32_t q_step = 0;
    
    // Перегруз - нагрев.
    if(I > overload.I_allow){
        if(overload.mode == DRIVE_OVERLOAD_MODE_NORMAL){
            q_step = fixed32_mul((int64_t)overload.dq_heat, dt);
        }else{ //DRIVE_OVERLOAD_MODE_THERMAL
            q_step = drive_overload_calc_q(I, dt);
        }
        
        overload.q_pie += q_step;
        
        if(overload.q_pie >= overload.q_max){
            overload.q_pie = overload.q_max;
            overload.overloaded = true;
        }
    }else{ // Остывание.
        q_step = fixed32_mul((int64_t)overload.dq_cool, dt);
        
        overload.q_pie -= q_step;
        
        if(overload.q_pie <= 0){
            overload.q_pie = 0;
            overload.overloaded = false;
        }
    }
}
