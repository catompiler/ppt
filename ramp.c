#include "ramp.h"
#include "utils/utils.h"


err_t ramp_init(ramp_t* ramp)
{
    ramp->current_ref = 0;
    ramp->target_ref = 0;
    ramp->step_ref = RAMP_TIME_MAX / RAMP_TIME_DEFAULT;
    
    return E_NO_ERROR;
}

err_t ramp_set_time(ramp_t* ramp, ramp_time_t time, fixed32_t step_dt)
{
    if(time < 0) return E_INVALID_VALUE;
    if(time > RAMP_TIME_MAX) return E_OUT_OF_RANGE;
    
    ramp->step_ref = step_dt * 100 / time;
    
    return E_NO_ERROR;
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
    
    fixed32_t new_ref = ramp->target_ref + ramp->step_ref;
    if(new_ref > RAMP_REFERENCE_MAX) new_ref = RAMP_REFERENCE_MAX;
    
    ramp->target_ref = new_ref;
    
    return true;
}

bool ramp_dec_reference(ramp_t* ramp)
{
    if(ramp->target_ref == RAMP_REFERENCE_MIN) return false;
    
    fixed32_t new_ref = ramp->target_ref - ramp->step_ref;
    if(new_ref < RAMP_REFERENCE_MIN) new_ref = RAMP_REFERENCE_MIN;
    
    ramp->target_ref = new_ref;
    
    return true;
}

bool ramp_calc_step(ramp_t* ramp)
{
    if(ramp->current_ref == ramp->target_ref) return true;
    
    fixed32_t dr = ramp->current_ref - ramp->target_ref;
    fixed32_t abs_dr = fixed_abs(dr);
    
    if(abs_dr < ramp->step_ref){
        ramp->current_ref = ramp->target_ref;
        return true;
    }else if(dr < 0){
        ramp->current_ref += ramp->step_ref;
    }else {
        ramp->current_ref -= ramp->step_ref;
    }
    return false;
}
