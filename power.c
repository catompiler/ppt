#include "power.h"
#include "utils/utils.h"
#include "utils/critical.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "bsqrt.h"



static void power_filter_reset(power_filter_t* filter)
{
    memset(filter->buffer, 0x0, sizeof(power_filter_value_t) * POWER_FILTER_SIZE_MAX);
    filter->index = 0;
    filter->count = 0;
}

static void power_filter_init(power_filter_t* filter, size_t max_size)
{
    if(max_size > POWER_FILTER_SIZE_MAX) max_size = POWER_FILTER_SIZE_MAX;
    if(max_size == 0) max_size = 1;
    
    memset(filter->buffer, 0x0, sizeof(power_filter_value_t) * POWER_FILTER_SIZE_MAX);
    filter->index = 0;
    filter->count = 0;
    filter->size = (uint16_t)max_size;
}

static void power_filter_put(power_filter_t* filter, int32_t sum, uint16_t count)
{
    filter->buffer[filter->index].sum = sum;
    filter->buffer[filter->index].count = count;
    
    if(filter->count < filter->size) filter->count ++;
    if(++ filter->index >= filter->size) filter->index = 0;
}

static int32_t power_filter_calculate(power_filter_t* filter, int32_t* ret_count)
{
    if(filter->count == 0) return 0;
    
    size_t count = filter->count;
    
    int32_t val = 0;
    int32_t count_val = 0;
    
    size_t i;
    for(i = 0; i < count; i ++){
        val       += filter->buffer[i].sum;
        count_val += filter->buffer[i].count;
    }
    
    if(!ret_count){
        val /= count_val;
    }else{
        *ret_count = count_val;
    }
    
    return val;
}

err_t power_value_init(power_value_t* value, power_channel_type_t type, size_t filter_size, fixed32_t k)
{
    memset(value, 0x0, sizeof(power_value_t));
    
    power_filter_init(&value->filter_value, filter_size);
    power_filter_init(&value->filter_zero, filter_size);
    
    value->type = type;
    value->adc_mult = k;
    
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

err_t power_set_soft_channel(power_t* power, size_t channel, bool is_soft)
{
    if(channel >= power->channels_count) return E_OUT_OF_RANGE;
    
    power->channels[channel].is_soft = is_soft;
    
    return E_NO_ERROR;
}

static void power_channel_set_soft_value(power_value_t* channel, fixed32_t value)
{
    // Отфильтруем значение.
    //mid_filter3i_put(&channel->filter_mid_adc, value);
    //value = mid_filter3i_value(&channel->filter_mid_adc);
    
    fixed32_t raw_valuef = fixed32_div((int64_t)value, channel->adc_mult);
    raw_valuef = fixed32_round(raw_valuef);
    int32_t raw_value = fixed32_get_int(raw_valuef);
    
    #if POWER_IGNORE_BITS != 0
    if(raw_value >= 0){
        raw_value &= POWER_IGNORE_BITS_MASK;
    }else{
        raw_value = -((-raw_value) & POWER_IGNORE_BITS_MASK);
    }
    #endif
    
    // Мгновенное сырое значение канала АЦП.
    channel->raw_value_inst = raw_value;
    // Мгновенное реальное значение канала АЦП.
    channel->real_value_inst = value;
    
    // Увеличим число значений в сумме.
    channel->count ++;
    
    // Если канал питания - AC.
    if(channel->type == POWER_CHANNEL_AC){
        // Квадрат значения для RMS.
        channel->sum += raw_value * raw_value;
    }else{
        // Значение для среднего.
        channel->sum += raw_value;
    }
}

err_t power_process_soft_channel_value(power_t* power, size_t channel, fixed32_t value)
{
    if(channel >= power->channels_count) return E_OUT_OF_RANGE;
    if(!power->channels[channel].is_soft) return E_INVALID_VALUE;
    
    power_channel_set_soft_value(&power->channels[channel], value);
    
    return E_NO_ERROR;
}

static void power_channel_process_adc_value(power_value_t* channel, uint16_t adc_value)
{
    // Отфильтруем значение АЦП.
    mid_filter3i_put(&channel->filter_mid_adc, adc_value);
    adc_value = mid_filter3i_value(&channel->filter_mid_adc);
    
    channel->raw_adc_value_inst = adc_value;
    
    // Значение нуля АЦП.
    channel->sum_zero += adc_value;
    // Увеличим число измерений значения нуля.
    channel->count_zero ++;
    
    // Если канал вычисляется программно.
    if(channel->is_soft) return;
    
    // Вычтем значение АЦП при нуле.
    int32_t value = (int32_t)adc_value - channel->raw_zero_cal;

    #if POWER_IGNORE_BITS != 0
    if(value >= 0){
        value &= POWER_IGNORE_BITS_MASK;
    }else{
        value = -((-value) & POWER_IGNORE_BITS_MASK);
    }
    #endif

    // Мгновенное сырое значение канала АЦП.
    channel->raw_value_inst = value;
    // Мгновенное реальное значение канала АЦП.
    channel->real_value_inst = (fixed32_t)channel->raw_value_inst * channel->adc_mult;

    // Увеличим число измерений значений.
    channel->count ++;
    // Если канал питания - AC.
    if(channel->type == POWER_CHANNEL_AC){
        // Квадрат значения для RMS.
        channel->sum += value * value;
    }else{
        // Значение для среднего.
        channel->sum += value;
    }
}

/**
 * Обрабатывает очередное значение АЦП.
 * @param power Питание.
 * @param channel Номер канала АЦП.
 * @param adc_value Значение АЦП.
 * @return Код ошибки.
 */
ALWAYS_INLINE static err_t power_process_adc_value(power_t* power, size_t channel, uint16_t adc_value)
{
    if(power->channels_count <= channel) return E_OUT_OF_RANGE;
    
    power_channel_process_adc_value(&power->channels[channel], adc_value);
    
    return E_NO_ERROR;
}

err_t power_process_adc_values(power_t* power, power_channels_t channels, uint16_t* adc_values)
{
    if(adc_values == NULL) return E_NULL_POINTER;
    if(channels == POWER_CHANNEL_NONE) return E_INVALID_VALUE;
    
    size_t adc_i = 0;
    
    size_t i = 0;
    for(; i < power->channels_count && channels != 0x0; i ++, channels >>= 1){
        if(channels & 0x1){
            RETURN_ERR_IF_FAIL(power_process_adc_value(power, i, adc_values[adc_i ++]));
        }
    }
    
    return E_NO_ERROR;
}

ALWAYS_INLINE static void power_channel_reset_sums(power_value_t* channel)
{
    channel->sum = 0;
    channel->sum_zero = 0;
    channel->count = 0;
    channel->count_zero = 0;
}

/**
 * Обрабатывает накопленные значения заданного канала АЦП.
 * @param channel Канал АЦП.
 */
static void power_channel_process_acc_data(power_value_t* channel)
{
    if(channel->count != 0)
        power_filter_put(&channel->filter_value, channel->sum, channel->count);
    
    if(channel->count_zero != 0)
        power_filter_put(&channel->filter_zero, channel->sum_zero, channel->count_zero);
    
    power_channel_reset_sums(channel);
}

err_t power_process_accumulated_data(power_t* power, power_channels_t channels)
{
    if(channels == POWER_CHANNEL_NONE) return E_INVALID_VALUE;
    
    size_t i = 0;
    for(; i < power->channels_count && channels != 0x0; i ++, channels >>= 1){
        if(channels & 0x1){
            power_channel_process_acc_data(&power->channels[i]);
        }
    }
    
    return E_NO_ERROR;
}

/**
 * Вычисляет значение заданного канала АЦП.
 * @param channel Канал АЦП.
 */
static void power_channel_calc(power_value_t* channel)
{
    int32_t count_val = 0;
    int32_t sum_val = power_filter_calculate(&channel->filter_value, &count_val);
    
    channel->raw_zero_cur = power_filter_calculate(&channel->filter_zero, NULL);
    
    if(count_val == 0) return;
    
    int32_t raw_val = sum_val / count_val;
    
    if(channel->type == POWER_CHANNEL_AC){
        channel->raw_value = bsqrti(raw_val);
    }else{
        channel->raw_value = raw_val;
    }
    
    fixed32_t real_val = 0;
    
    //if(!channel->is_soft){
        if(channel->type == POWER_CHANNEL_AC){
            //real_val = (fixed32_t)channel->raw_value * channel->adc_mult;
            int32_t sq_sum = bsqrti(sum_val);
            int64_t sq_sumf = fixed32_make_from_int((int64_t)sq_sum);
            
            fixed32_t count_valf = fixed32_make_from_int(count_val);
            fixed32_t sq_countf = bsqrtf(count_valf);
            
            real_val = (int32_t)fixed32_div(sq_sumf, sq_countf);
            real_val = fixed32_mul((int64_t)real_val, channel->adc_mult);
        }else{
            int64_t real_sum = (int64_t)sum_val * channel->adc_mult;
            real_val = real_sum / count_val;
        }
    /*}else{
        real_val = power_value_raw_to_soft(channel->raw_value);
    }*/
    
    channel->real_value = fixed32_mul((int64_t)real_val, channel->value_mult);
    
    channel->data_avail = true;
}

ALWAYS_INLINE static void power_calc_channel_value(power_t* power, size_t channel_n)
{
    power_channel_calc(&power->channels[channel_n]);
}

err_t power_calc_values(power_t* power, power_channels_t channels)
{
    if(channels == POWER_CHANNEL_NONE) return E_INVALID_VALUE;
    
    size_t i = 0;
    for(; i < power->channels_count && channels != 0x0; i ++, channels >>= 1){
        if(channels & 0x1){
            power_calc_channel_value(power, i);
            //power_channel_calc(&power->channels[i]);
        }
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
    //channel->data_avail = false;
    //power_channel_reset_sums(channel);
}

err_t power_calibrate(power_t* power, power_channels_t channels)
{
    if(channels == POWER_CHANNEL_NONE) return E_INVALID_VALUE;
    
    size_t i = 0;
    for(; i < power->channels_count && channels != 0x0; i ++, channels >>= 1){
        if(channels & 0x1){
            power_channel_calibrate(&power->channels[i]);
            //power_filter_reset(&power->channels[i].filter);
        }
    }
    
    return E_NO_ERROR;
}

bool power_data_avail(const power_t* power, power_channels_t channels)
{
    if(channels == POWER_CHANNEL_NONE) return false;
    
    bool res = true;
    
    size_t i = 0;
    for(; i < power->channels_count && channels != 0x0; i ++, channels >>= 1){
        if(channels & 0x1){
            res &= power_channel_data_avail(power, i);
            if(!res) break;
        }
    }
    
    return res;
}

bool power_data_filter_filled(const power_t* power, power_channels_t channels)
{
    if(channels == POWER_CHANNEL_NONE) return false;
    
    bool res = true;
    
    size_t i = 0;
    for(; i < power->channels_count && channels != 0x0; i ++, channels >>= 1){
        if(channels & 0x1){
            res &= power_channel_data_filter_filled(power, i);
            if(!res) break;
        }
    }
    
    return res;
}

err_t power_reset_channels(power_t* power, power_channels_t channels)
{
    if(channels == POWER_CHANNEL_NONE) return E_INVALID_VALUE;
    
    size_t i = 0;
    for(; i < power->channels_count && channels != 0x0; i ++, channels >>= 1){
        if(channels & 0x1){
            power_channel_reset_sums(&power->channels[i]);
            power_filter_reset(&power->channels[i].filter_value);
            power_filter_reset(&power->channels[i].filter_zero);
            mid_filter3i_reset(&power->channels[i].filter_mid_adc);
        }
    }
    
    return E_NO_ERROR;
}
