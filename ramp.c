#include "ramp.h"
#include "utils/utils.h"


err_t ramp_init(ramp_t* ramp)
{
    ramp->current_ref = 0;
    ramp->target_ref = 0;
    ramp->reference_step = 0x20000 /* 20 ms * 100 */ / RAMP_REFERENCE_TIME_DEFAULT;
    ramp->start_step = 0x20000 /* 20 ms * 100 */ / RAMP_START_TIME_DEFAULT;
    ramp->stop_step = 0x20000 /* 20 ms * 100 */ / RAMP_STOP_TIME_DEFAULT;
    ramp->fast_stop_step = 0x20000 /* 20 ms * 100 */ / RAMP_FAST_STOP_TIME_DEFAULT;
    ramp->stop_mode = RAMP_STOP_MODE_NORMAL;
    
    return E_NO_ERROR;
}

err_t ramp_set_reference_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt)
{
    if(time < 0) return E_INVALID_VALUE;
    if(time > RAMP_TIME_MAX) return E_OUT_OF_RANGE;
    
    ramp->reference_step = step_dt * 100 / time;
    
    return E_NO_ERROR;
}

err_t ramp_set_start_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt)
{
    if(time < 0) return E_INVALID_VALUE;
    if(time > RAMP_TIME_MAX) return E_OUT_OF_RANGE;
    
    ramp->start_step = step_dt * 100 / time;
    
    return E_NO_ERROR;
}

err_t ramp_set_stop_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt)
{
    if(time < 0) return E_INVALID_VALUE;
    if(time > RAMP_TIME_MAX) return E_OUT_OF_RANGE;
    
    ramp->stop_step = step_dt * 100 / time;
    
    return E_NO_ERROR;
}

err_t ramp_set_fast_stop_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt)
{
    if(time < 0) return E_INVALID_VALUE;
    if(time > RAMP_TIME_MAX) return E_OUT_OF_RANGE;
    
    ramp->fast_stop_step = step_dt * 100 / time;
    
    return E_NO_ERROR;
}

static fixed32_t ramp_get_step(ramp_t* ramp)
{
    if(ramp->current_ref < ramp->target_ref){
        return ramp->start_step;
    }else if(ramp->current_ref > ramp->target_ref){
        if(ramp->stop_mode == RAMP_STOP_MODE_NORMAL){
            return ramp->stop_step;
        }else{
            return ramp->fast_stop_step;
        }
    }
    return 0;
}

err_t ramp_set_target_reference(ramp_t* ramp, ramp_reference_t reference)
{
    if(reference > RAMP_REFERENCE_MAX) return E_OUT_OF_RANGE;
    
    ramp->target_ref = fixed32_make_from_int((fixed32_t)reference);
    
    return E_NO_ERROR;
}

bool ramp_inc_reference(ramp_t* ramp)
{
    if(ramp->target_ref == RAMP_REFERENCE_MAX) return false;
    
    fixed32_t new_ref = ramp->target_ref + ramp->reference_step;
    if(new_ref > RAMP_REFERENCE_MAX) new_ref = RAMP_REFERENCE_MAX;
    
    ramp->target_ref = new_ref;
    
    return true;
}

bool ramp_dec_reference(ramp_t* ramp)
{
    if(ramp->target_ref == RAMP_REFERENCE_MIN) return false;
    
    fixed32_t new_ref = ramp->target_ref - ramp->reference_step;
    if(new_ref < RAMP_REFERENCE_MIN) new_ref = RAMP_REFERENCE_MIN;
    
    ramp->target_ref = new_ref;
    
    return true;
}

bool ramp_calc_step(ramp_t* ramp)
{
    if(ramp->current_ref == ramp->target_ref) return true;
    
    fixed32_t dr = ramp->current_ref - ramp->target_ref;
    fixed32_t abs_dr = fixed_abs(dr);
    fixed32_t step = ramp_get_step(ramp);
    
    if(abs_dr < step){
        ramp->current_ref = ramp->target_ref;
        return true;
    }else if(dr < 0){
        ramp->current_ref += step;
    }else{
        ramp->current_ref -= step;
    }
    return false;
}
