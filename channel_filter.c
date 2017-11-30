#include "channel_filter.h"



void channel_filter_init(channel_filter_t* filter, fixed32_t* buffer, size_t capacity, size_t size, channel_filter_type_t type)
{
    filter->buffer = buffer;
    filter->capacity = capacity;
    filter->index = 0;
    filter->count = 0;
    filter->type = type;
    
    channel_filter_resize(filter, size);
}

void channel_filter_reset(channel_filter_t* filter)
{
    filter->index = 0;
    filter->count = 0;
}

void channel_filter_resize(channel_filter_t* filter, size_t size)
{
    if(size > filter->capacity) size = filter->capacity;
    
    filter->size = (uint16_t)size;
    if(filter->count > filter->size) filter->count = filter->size;
    if(filter->index >= filter->size) filter->index = 0;
}

void channel_filter_resize_ms(channel_filter_t* filter, uint32_t time_ms)
{
    fixed32_t item_time = (filter->type == CHANNEL_FILTER_NORMAL) ?
                            CHANNEL_FILTER_TIME_PER_ITEM_MS_F :
                            CHANNEL_FILTER_TIME_PER_FAST_ITEM_MS_F;
    
    //fixed32_t max_time = filter->capacity * CHANNEL_FILTER_TIME_PER_ITEM_MS_F;
    fixed32_t time = fixed32_make_from_int(time_ms);
    fixed32_t items_count = fixed32_div((int64_t)time, item_time);
    items_count = fixed32_round(items_count);
    
    size_t size = fixed32_get_int(items_count);
    if(size == 0) size = 1;
    if(size > filter->capacity) size = filter->capacity;
    
    channel_filter_resize(filter, size);
}

void channel_filter_put(channel_filter_t* filter, fixed32_t value)
{
    filter->buffer[filter->index] = value;
    
    if(filter->count < filter->size) filter->count ++;
    if(++ filter->index >= filter->size) filter->index = 0;
}

void channel_filter_calculate(channel_filter_t* filter)
{
    int32_t count = (int32_t)filter->count;
    
    int64_t sum_val = 0;
    
    size_t i;
    for(i = 0; i < count; i ++){
        sum_val += filter->buffer[i];
    }
    
    if(count > 1) sum_val /= count;
    
    filter->value = sum_val;
}
