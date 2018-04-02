#include "ramp.h"
#include "utils/utils.h"


err_t ramp_init(ramp_t* ramp)
{
    ramp->current_ref = 0;
    ramp->target_ref = 0;
    // (0.02 * 0x10000 << 16) / (time / 100).
    ramp->reference_step = 0x51E0000 /* 20 ms << 16 */ / (RAMP_REFERENCE_TIME_DEFAULT / 100);
    ramp->start_step = 0x51E0000 /* 20 ms << 16 */ / (RAMP_START_TIME_DEFAULT / 100);
    ramp->stop_step = 0x51E0000 /* 20 ms << 16 */ / (RAMP_STOP_TIME_DEFAULT / 100);
    ramp->fast_stop_step = 0x51E0000 /* 20 ms << 16 */ / (RAMP_FAST_STOP_TIME_DEFAULT / 100);
    ramp->stop_mode = RAMP_STOP_MODE_NORMAL;
    
    return E_NO_ERROR;
}

err_t ramp_set_reference_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt)
{
    if(time < 0) return E_INVALID_VALUE;
    if(time > RAMP_TIME_MAX) return E_OUT_OF_RANGE;
    
    fixed32_t step = step_dt * 100;
    step = fixed32_div((int64_t)step, time);
    
    ramp->reference_step = step;
    
    return E_NO_ERROR;
}

err_t ramp_set_start_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt)
{
    if(time < 0) return E_INVALID_VALUE;
    if(time > RAMP_TIME_MAX) return E_OUT_OF_RANGE;
    
    fixed32_t step = step_dt * 100;
    step = fixed32_div((int64_t)step, time);
    
    ramp->start_step = step;
    
    return E_NO_ERROR;
}

err_t ramp_set_stop_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt)
{
    if(time < 0) return E_INVALID_VALUE;
    if(time > RAMP_TIME_MAX) return E_OUT_OF_RANGE;
    
    fixed32_t step = step_dt * 100;
    step = fixed32_div((int64_t)step, time);
    
    ramp->stop_step = step;
    
    return E_NO_ERROR;
}

err_t ramp_set_fast_stop_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt)
{
    if(time < 0) return E_INVALID_VALUE;
    if(time > RAMP_TIME_MAX) return E_OUT_OF_RANGE;
    
    fixed32_t step = step_dt * 100;
    step = fixed32_div((int64_t)step, time);
    
    ramp->fast_stop_step = step;
    
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
    if(reference > RAMP_REFERENCE_MAX_F) return E_OUT_OF_RANGE;
    if(reference < RAMP_REFERENCE_MIN_F) return E_OUT_OF_RANGE;
    
    ramp->target_ref = reference;
    
    return E_NO_ERROR;
}

bool ramp_inc_reference(ramp_t* ramp)
{
    if(ramp->target_ref == RAMP_REFERENCE_MAX_F) return false;
    
    fixed32_t new_ref = ramp->target_ref + ramp->reference_step;
    if(new_ref > RAMP_REFERENCE_MAX_F) new_ref = RAMP_REFERENCE_MAX_F;
    
    ramp->target_ref = new_ref;
    
    return true;
}

bool ramp_dec_reference(ramp_t* ramp)
{
    if(ramp->target_ref == RAMP_REFERENCE_MIN_F) return false;
    
    fixed32_t new_ref = ramp->target_ref - ramp->reference_step;
    if(new_ref < RAMP_REFERENCE_MIN_F) new_ref = RAMP_REFERENCE_MIN_F;
    
    ramp->target_ref = new_ref;
    
    return true;
}

void ramp_adjust_current_reference(ramp_t* ramp, fixed32_t cur_val, fixed32_t max_val)
{
    if(cur_val <= RAMP_REFERENCE_MIN_F){
        ramp->current_ref = 0;
        return;
    }
    if(cur_val >= max_val){
        ramp->current_ref = RAMP_REFERENCE_MAX_F;
        return;
    }
    
    fixed32_t cur_calc_ref = fixed32_div((int64_t)cur_val, max_val);
    cur_calc_ref = cur_calc_ref * 100;
    
    cur_calc_ref = CLAMP(cur_calc_ref, RAMP_REFERENCE_MIN_F, RAMP_REFERENCE_MAX_F);
    
    ramp->current_ref = cur_calc_ref;
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
