#include "power.h"
#include "utils/utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


//! Число отбрасываемых минимальных значений.
#define POWER_FILTER_SKIP_MIN 1
//! Число отбрасываемых максимальных значений.
#define POWER_FILTER_SKIP_MAX 1


static void power_filter_reset(power_filter_t* filter)
{
    memset(filter, 0x0, sizeof(power_filter_t));
}

static void power_filter_put(power_filter_t* filter, int16_t value)
{
    filter->buffer[filter->index] = value;
    if(filter->count < POWER_FILTER_SIZE) filter->count ++;
    if(++ filter->index >= POWER_FILTER_SIZE) filter->index = 0;
}

static int power_filter_cmp(const void* a, const void* b)
{
    return (*(const int16_t*)a - *(const int16_t*)b);
}

static int16_t power_filter_calculate(power_filter_t* filter)
{
    if(filter->count == 0) return 0;
    
    int16_t buffer[filter->count];
    
    memcpy(buffer, filter->buffer, sizeof(int16_t) * filter->count);
    
    qsort(buffer, filter->count, sizeof(int16_t), power_filter_cmp);
    
    int32_t val = 0;
    size_t from = 0;
    size_t to = filter->count;
    size_t avg_count = filter->count;
    
    if(filter->count > (POWER_FILTER_SKIP_MIN + POWER_FILTER_SKIP_MAX)){
        from += POWER_FILTER_SKIP_MIN;
        to -= POWER_FILTER_SKIP_MAX;
        avg_count -= (POWER_FILTER_SKIP_MIN + POWER_FILTER_SKIP_MAX);
    }
    
    while(from < to){
        val += buffer[from];
        from ++;
    }
    
    val /= (int32_t)avg_count;
    
    return val;
}

err_t power_value_init(power_value_t* value, power_channel_type_t type, fixed32_t k)
{
    memset(value, 0x0, sizeof(power_value_t));
    
    value->type = type;
    value->k = k;
    
    return E_NO_ERROR;
}

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
    int32_t value = (int16_t)adc_value - channel->raw_zero_cal;
    int32_t abs_value = value;
    
    if(channel->type == POWER_CHANNEL_AC){
        abs_value = ABS(abs_value);
    }
    
    channel->raw_value_inst = value;
    channel->real_value_inst = (fixed32_t)channel->raw_value_inst * channel->k;
    
    channel->sum_zero += adc_value;
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
    
    /*if(channel == 6 && adc_value > 1024){
        printf("ADC[%d] == %u\n", (int)channel, (unsigned int)adc_value);
    }*/
    
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
    if(channel->count == 0){
        channel->data_avail = false;
        return;
    }
    
    int16_t value = 0;
    
    value = channel->sum_avg / channel->count;
    power_filter_put(&channel->filter_avg, value);
    channel->raw_value_avg = power_filter_calculate(&channel->filter_avg);//value;//
    //! @todo Вычислени квадратного корня для RMS.
    channel->raw_value_rms = channel->raw_value_avg;
    
    channel->raw_zero_cur = channel->sum_zero / channel->count;
    
    channel->real_value_avg = (fixed32_t)channel->raw_value_avg * channel->k;
    channel->real_value_rms = (fixed32_t)channel->raw_value_rms * channel->k;
    
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
        power_filter_reset(&power->channels[i].filter_avg);
        power_filter_reset(&power->channels[i].filter_rms);
    }
    
    return E_NO_ERROR;
}

bool power_data_avail(const power_t* power, power_channels_t channels)
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

err_t power_reset_channels(power_t* power, power_channels_t channels)
{
    if(channels == POWER_CHANNEL_NONE) return E_INVALID_VALUE;
    
    size_t i = 0;
    
    for(; channels != 0; channels >>= 1, i ++){
        if(!(channels & 0x1)) continue;
        
        if(i >= power->channels_count) return E_OUT_OF_RANGE;
        
        power_channel_reset_sums(&power->channels[i]);
        power_filter_reset(&power->channels[i].filter_avg);
        power_filter_reset(&power->channels[i].filter_rms);
    }
    
    return E_NO_ERROR;
}
