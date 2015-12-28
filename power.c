#include "power.h"
#include "utils/utils.h"



err_t power_init(power_t* power, power_value_t* channels, size_t channels_count)
{
    if(channels == NULL) return E_NULL_POINTER;
    if(channels_count == 0) return E_INVALID_VALUE;
    
    power->channels = channels;
    power->channels_count = channels_count;
    
    return E_NO_ERROR;
}

ALWAYS_INLINE static void power_channel_process_adc_value(power_value_t* channel, uint16_t adc_value)
{
    int32_t value = (int32_t)adc_value;
    
    int32_t abs_value = value - channel->raw_zero_cal;
    
    if(channel->type == POWER_CHANNEL_AC){
        abs_value = ABS(abs_value);
    }
    
    channel->sum_zero += value;
    channel->sum_avg += abs_value;
    channel->sum_rms += abs_value * abs_value;
    channel->count ++;
}

/**
 * Обрабатывает очередное значение АЦП.
 * @param power Питание.
 * @param channel Номер канала АЦП.
 * @param adc_value Значение АЦП.
 * @return Код ошибки.
 */
ALWAYS_INLINE err_t power_process_adc_value(power_t* power, size_t channel, uint16_t adc_value)
{
    if(power->channels_count <= channel) return E_OUT_OF_RANGE;
    
    power_channel_process_adc_value(&power->channels[channel], adc_value);
    
    return E_NO_ERROR;
}

err_t power_process_adc_values(power_t* power, power_channels_t channels, uint16_t* adc_values)
{
    if(adc_values == NULL) return E_NULL_POINTER;
    if(channels == POWER_CHANNEL_NONE) return E_INVALID_VALUE;
    
    size_t i = 0;
    size_t adc_i = 0;
    
    for(; channels != 0; channels >>= 1, i ++){
        if(!(channels & 0x1)) continue;
        
        if(i >= power->channels_count) return E_OUT_OF_RANGE;
        
        RETURN_ERR_IF_FAIL(power_process_adc_value(power, i, adc_values[adc_i ++]));
    }
    
    return E_NO_ERROR;
}

ALWAYS_INLINE static void power_channel_reset_sums(power_value_t* channel)
{
    channel->count = 0;
    channel->sum_zero = 0;
    channel->sum_avg = 0;
    channel->sum_rms = 0;
}

/**
 * Вычисляет значение заданного канала АЦП.
 * @param channel Канал АЦП.
 */
ALWAYS_INLINE static void power_channel_calc(power_value_t* channel)
{
    if(channel->count == 0) return;
    
    channel->raw_zero_cur = channel->sum_zero / channel->count;
    channel->raw_value_avg = channel->sum_avg / channel->count;
    //! @todo Вычислени квадратного корня для RMS.
    channel->raw_value_rms = channel->raw_value_avg;
    
    channel->real_value_avg = fixed32_mul((int64_t)channel->real_value_avg, channel->k);
    channel->real_value_rms = fixed32_mul((int64_t)channel->real_value_rms, channel->k);
    
    power_channel_reset_sums(channel);
    
    channel->data_avail = true;
}

err_t power_calc_values(power_t* power, power_channels_t channels)
{
    if(channels == POWER_CHANNEL_NONE) return E_INVALID_VALUE;
    
    size_t i = 0;
    
    for(; channels != 0; channels >>= 1, i ++){
        if(!(channels & 0x1)) continue;
        
        if(i >= power->channels_count) return E_OUT_OF_RANGE;
        
        power_channel_calc(&power->channels[i]);
    }
    
    return E_NO_ERROR;
}

/**
 * Калибрует заданный канал АЦП.
 * @param channel Канал АЦП.
 */
ALWAYS_INLINE static void power_channel_calibrate(power_value_t* channel)
{
    channel->raw_zero_cal = channel->raw_zero_cur;
    channel->calibrated = true;
    channel->data_avail = false;
    power_channel_reset_sums(channel);
}

err_t power_calibrate(power_t* power, power_channels_t channels)
{
    if(channels == POWER_CHANNEL_NONE) return E_INVALID_VALUE;
    
    size_t i = 0;
    
    for(; channels != 0; channels >>= 1, i ++){
        if(!(channels & 0x1)) continue;
        
        if(i >= power->channels_count) return E_OUT_OF_RANGE;
        
        power_channel_calibrate(&power->channels[i]);
    }
    
    return E_NO_ERROR;
}

bool power_data_avail(power_t* power, power_channels_t channels)
{
    if(channels == POWER_CHANNEL_NONE) return false;
    
    size_t i = 0;
    bool res = true;
    
    for(; channels != 0; channels >>= 1, i ++){
        if(!(channels & 0x1)) continue;
        
        if(i >= power->channels_count) return false;
        
        res &= power_channel_data_avail(power, i);
    }
    
    return res;
}
