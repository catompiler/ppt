#include "drive_power.h"
#include "utils/utils.h"
#include "settings.h"
#include <string.h>



//! Каналы питания для осциллограмм.
static const uint16_t drive_power_osc_channels_nums[DRIVE_POWER_OSC_CHANNELS_COUNT] = {
    DRIVE_POWER_Ua, DRIVE_POWER_Ub, DRIVE_POWER_Uc,
    DRIVE_POWER_Ia, DRIVE_POWER_Ib, DRIVE_POWER_Ic,
    DRIVE_POWER_Urot, DRIVE_POWER_Irot, DRIVE_POWER_Iexc
};

//! Максимальный разрешённый дрифт.
#define DRIVE_POWER_MAX_DRIFT 32

/**
 * Делитель измерений АЦП.
 * Число точек за период осцилограммы = Число измерений за период ADC / (делитель измерений АЦП + 1).
 */
#define DRIVE_POWER_OSC_ADC_DIVIDER 1

//! Тип буфера для хранения осцилограмм.
typedef struct _Oscillogram_Buf {
    drive_power_osc_channel_t osc_channels[DRIVE_POWER_OSC_CHANNELS_COUNT];
    size_t index; //!< Текущий индекс в осциллограммах.
    size_t count; //!< Количество значений в осциллограммах.
    size_t pause_mark; //!< Метка возникновения ошибки.
    size_t skip_counter; //!< Счётчик числа пропущенных точек АЦП.
} oscillogram_buf_t;

//! Тип питания привода.
typedef struct _Drive_Power {
    power_value_t power_values[DRIVE_POWER_CHANNELS_COUNT]; //!< Значение каналов АЦП.
    power_t power; //!< Питание.
    size_t processing_iters; //!< Число итераций для накопления и обработки данных.
    size_t iters_processed; //!< Число итераций обработки данных.
    oscillogram_buf_t osc_buf; //!< Буфер осциллограмм.
    phase_t phase_calc_current; //!< Вычислять ток заданной фазы.
} drive_power_t;

//! Питание привода.
static drive_power_t drive_power;


err_t drive_power_init(void)
{
    memset(&drive_power, 0x0, sizeof(drive_power_t));
    
    drive_power.osc_buf.pause_mark = DRIVE_POWER_OSC_CHANNEL_LEN;
    
    drive_power.processing_iters = DRIVE_POWER_PROCESSING_ITERS_DEFAULT;
    
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ua],POWER_CHANNEL_AC, 0x10000); // Ua //0x4478
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ia],POWER_CHANNEL_AC, 0x10000); // Ia
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ub],POWER_CHANNEL_AC, 0x10000); // Ub //0x44ac
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ib],POWER_CHANNEL_AC, 0x10000); // Ib
    power_value_init(&drive_power.power_values[DRIVE_POWER_Uc],POWER_CHANNEL_AC, 0x10000); // Uc //0x450b
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ic],POWER_CHANNEL_AC, 0x10000); // Ic
    power_value_init(&drive_power.power_values[DRIVE_POWER_Urot],POWER_CHANNEL_DC, 0x10000); // Urot //0x6861
    power_value_init(&drive_power.power_values[DRIVE_POWER_Irot],POWER_CHANNEL_DC, 0x10000); // Irot //0x2e14
    power_value_init(&drive_power.power_values[DRIVE_POWER_Iexc],POWER_CHANNEL_AC, 0x10000); // Iexc //0x1bd
    power_value_init(&drive_power.power_values[DRIVE_POWER_Iref],POWER_CHANNEL_DC, 0x10000); // Iref
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ifan],POWER_CHANNEL_DC, 0x10000); // Ifan
    
    /*
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ua],POWER_CHANNEL_AC, 0x6FE1); // Ua //0x4478
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ia],POWER_CHANNEL_AC, 0x5f11); // Ia
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ub],POWER_CHANNEL_AC, 0x7037); // Ub //0x44ac
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ib],POWER_CHANNEL_AC, 0x5f11); // Ib
    power_value_init(&drive_power.power_values[DRIVE_POWER_Uc],POWER_CHANNEL_AC, 0x70D2); // Uc //0x450b
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ic],POWER_CHANNEL_AC, 0x5f11); // Ic
    power_value_init(&drive_power.power_values[DRIVE_POWER_Urot],POWER_CHANNEL_DC, 0xA5A0); // Urot //0x6861
    power_value_init(&drive_power.power_values[DRIVE_POWER_Irot],POWER_CHANNEL_DC, 0x5f11); // Irot //0x2e14
    power_value_init(&drive_power.power_values[DRIVE_POWER_Iexc],POWER_CHANNEL_AC, 0x321); // Iexc //0x1bd
    power_value_init(&drive_power.power_values[DRIVE_POWER_Iref],POWER_CHANNEL_DC, 0x10000); // Iref
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ifan],POWER_CHANNEL_DC, 0x10000); // Ifan
    */
    
    power_init(&drive_power.power, drive_power.power_values, DRIVE_POWER_CHANNELS);
    
    return E_NO_ERROR;
}

void drive_power_reset(void)
{
    drive_power.processing_iters = DRIVE_POWER_PROCESSING_ITERS_DEFAULT;
    drive_power.iters_processed = 0;
}

phase_t drive_power_phase_calc_current(void)
{
    return drive_power.phase_calc_current;
}

ALWAYS_INLINE static bool drive_power_get_current_channel_by_phase(phase_t phase, size_t* channel)
{
    switch(phase){
        default:
            break;
        case PHASE_A:
            if(channel) *channel = DRIVE_POWER_Ia;
            return true;
        case PHASE_B:
            if(channel) *channel = DRIVE_POWER_Ib;
            return true;
        case PHASE_C:
            if(channel) *channel = DRIVE_POWER_Ic;
            return true;
    }
    return false;
}

void drive_power_set_phase_calc_current(phase_t phase)
{
    size_t channel;
    
    if(drive_power_get_current_channel_by_phase(drive_power.phase_calc_current, &channel)){
        power_set_soft_channel(&drive_power.power, channel, false);
    }
    
    drive_power.phase_calc_current = phase;
    
    if(drive_power_get_current_channel_by_phase(phase, &channel)){
        power_set_soft_channel(&drive_power.power, channel, true);
    }
}

size_t drive_power_processing_iters(void)
{
    return drive_power.processing_iters;
}

err_t drive_power_set_processing_iters(size_t iters)
{
    drive_power.processing_iters = iters;
    
    return E_NO_ERROR;
}

size_t drive_power_osc_channels_count(void)
{
    return DRIVE_POWER_OSC_CHANNELS_COUNT;
}

size_t drive_power_oscillogram_length(void)
{
    return drive_power.osc_buf.count;
}

bool drive_power_oscillogram_full(void)
{
    return drive_power.osc_buf.count == DRIVE_POWER_OSC_CHANNEL_LEN;
}

void drive_power_oscillogram_pause(void)
{
    drive_power.osc_buf.pause_mark = drive_power.osc_buf.index;
}

void drive_power_oscillogram_half_pause(void)
{
    drive_power.osc_buf.pause_mark = drive_power.osc_buf.index + (DRIVE_POWER_OSC_CHANNEL_LEN / 2);
    if(drive_power.osc_buf.pause_mark >= DRIVE_POWER_OSC_CHANNEL_LEN){
        drive_power.osc_buf.pause_mark -= DRIVE_POWER_OSC_CHANNEL_LEN;
    }
}

bool drive_power_oscillogram_paused(void)
{
    return drive_power.osc_buf.index == drive_power.osc_buf.pause_mark;
}

void drive_power_oscillogram_resume(void)
{
    drive_power.osc_buf.pause_mark = DRIVE_POWER_OSC_CHANNEL_LEN;
}

ALWAYS_INLINE static bool drive_power_osc_pause_mark_is_valid(void)
{
    return drive_power.osc_buf.pause_mark < DRIVE_POWER_OSC_CHANNEL_LEN;
}

size_t drive_power_oscillogram_start_index(void)
{
    if(drive_power_osc_pause_mark_is_valid()){
        return drive_power.osc_buf.pause_mark;
    }
    return drive_power.osc_buf.index;
}

size_t drive_power_oscillogram_half_index(void)
{
    size_t res = 0;
    if(drive_power_osc_pause_mark_is_valid()){
        res = drive_power.osc_buf.pause_mark + (DRIVE_POWER_OSC_CHANNEL_LEN / 2);
    }else{
        res = drive_power.osc_buf.index + (DRIVE_POWER_OSC_CHANNEL_LEN / 2);
    }
    if(res >= DRIVE_POWER_OSC_CHANNEL_LEN) res -= DRIVE_POWER_OSC_CHANNEL_LEN;
    
    return res;
}

size_t drive_power_oscillogram_end_index(void)
{
    if(drive_power_osc_pause_mark_is_valid()){
        if(drive_power.osc_buf.pause_mark == 0){
            return (DRIVE_POWER_OSC_CHANNEL_LEN - 1);
        }
        return drive_power.osc_buf.pause_mark - 1;
    }
    if(drive_power.osc_buf.index == 0){
        return (DRIVE_POWER_OSC_CHANNEL_LEN - 1);
    }
    return drive_power.osc_buf.index - 1;
}

size_t drive_power_oscillogram_next_index(size_t index)
{
    if(++ index >= DRIVE_POWER_OSC_CHANNEL_LEN) index = 0;
    
    return index;
}

osc_value_t drive_power_osc_channel_data(size_t osc_channel, size_t index)
{
    if(osc_channel >= DRIVE_POWER_OSC_CHANNELS_COUNT) return 0;
    if(index >= DRIVE_POWER_OSC_CHANNEL_LEN) return 0;
    return drive_power.osc_buf.osc_channels[osc_channel].data[index];
}

err_t drive_power_osc_channel_get(size_t osc_channel, drive_power_osc_channel_t* osc)
{
    if(osc_channel >= DRIVE_POWER_OSC_CHANNELS_COUNT) return E_OUT_OF_RANGE;
    
    size_t start_index = drive_power_oscillogram_start_index();
    size_t end_index = drive_power_oscillogram_end_index();
    
    if(end_index < start_index){
        memcpy(osc->data, &drive_power.osc_buf.osc_channels[osc_channel].data[start_index],
                (DRIVE_POWER_OSC_CHANNEL_LEN - start_index) * sizeof(osc_value_t));
        memcpy(osc->data + (DRIVE_POWER_OSC_CHANNEL_LEN - start_index),
                &drive_power.osc_buf.osc_channels[osc_channel].data[0],
                (end_index + 1) * sizeof(osc_value_t));
    }else{
        memcpy(osc->data, &drive_power.osc_buf.osc_channels[osc_channel].data[start_index],
                (end_index - start_index + 1) * sizeof(osc_value_t));
    }
    
    return E_NO_ERROR;
}

static void drive_power_osc_buffer_put_data(void)
{
    size_t i = 0;
    for(; i < DRIVE_POWER_OSC_CHANNELS_COUNT; i ++){
        osc_value_t value = drive_power_osc_value_from_fixed32(drive_power_channel_real_value_inst(
                    drive_power_osc_channels_nums[i]
                ));
        drive_power.osc_buf.osc_channels[i].data[drive_power.osc_buf.index] = value;
    }

/*
    size_t i = 0;
    osc_value_t value = 0;
    osc_value_t new_value = 0;
    static osc_value_t prev_value[DRIVE_POWER_OSC_CHANNELS_COUNT] = {0};
    for(; i < DRIVE_POWER_OSC_CHANNELS_COUNT; i ++){
        new_value = drive_power_osc_value_from_fixed32(drive_power_channel_real_value_inst(
                    drive_power_osc_channels_nums[i]
                ));
        
        value = (prev_value[i] + new_value) >> 1;
        prev_value[i] = new_value;
        drive_power.osc_buf.osc_channels[i].data[drive_power.osc_buf.index] = value;
    }
*/
    
    if(++ drive_power.osc_buf.index >= DRIVE_POWER_OSC_CHANNEL_LEN){
        drive_power.osc_buf.index = 0;
    }
    
    if(drive_power.osc_buf.count < DRIVE_POWER_OSC_CHANNEL_LEN){
        drive_power.osc_buf.count ++;
    }
}

static void drive_power_append_osc_data(void)
{
    if(drive_power.osc_buf.skip_counter ++ >= DRIVE_POWER_OSC_ADC_DIVIDER){
        drive_power.osc_buf.skip_counter = 0;
        
        if(drive_power.osc_buf.index != drive_power.osc_buf.pause_mark){
            drive_power_osc_buffer_put_data();
        }
    }
}

static void drive_power_calc_phase_current(void)
{
    fixed32_t I1 = 0, I2 = 0, I = 0;
    switch(drive_power.phase_calc_current){
        default:
            break;
        case PHASE_A:
            I1 = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Ib);
            I2 = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Ic);
            I = -(I1 + I2);
            power_process_soft_channel_value(&drive_power.power, DRIVE_POWER_Ia, I);
            break;
        case PHASE_B:
            I1 = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Ia);
            I2 = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Ic);
            I = -(I1 + I2);
            power_process_soft_channel_value(&drive_power.power, DRIVE_POWER_Ib, I);
            break;
        case PHASE_C:
            I1 = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Ia);
            I2 = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Ib);
            I = -(I1 + I2);
            power_process_soft_channel_value(&drive_power.power, DRIVE_POWER_Ic, I);
            break;
    }
}

err_t drive_power_process_adc_values(power_channels_t channels, uint16_t* adc_values)
{
    err_t err = power_process_adc_values(&drive_power.power, channels, adc_values);
    
    drive_power_calc_phase_current();
    
    drive_power_append_osc_data();
    
    return err;
}

err_t drive_power_process_accumulated_data(power_channels_t channels)
{
    err_t err = power_process_accumulated_data(&drive_power.power, channels);
    
    return err;
}

/*bool drive_power_calc_values(power_channels_t channels, phase_t phase, err_t* err)
{
    if(phase != drive_power.power_phase) return false;
    if(++ drive_power.periods_processed >= drive_power.processing_periods){
        drive_power.periods_processed = 0;
        err_t e = power_calc_values(&drive_power.power, channels);
        if(err) *err = e;
        return true;
    }
    return false;
}*/

static void drive_power_calc_values_impl(power_channels_t channels, err_t* err)
{
    err_t e = power_calc_values(&drive_power.power, channels);
    if(err) *err = e;
}

bool drive_power_calc_values(power_channels_t channels, err_t* err)
{
    if(drive_power.processing_iters == 0){
        drive_power_calc_values_impl(channels, err);
        return power_data_filter_filled(&drive_power.power, channels);
    }
    if(++ drive_power.iters_processed >= drive_power.processing_iters){
        drive_power.iters_processed = 0;
        drive_power_calc_values_impl(channels, err);
        return power_data_filter_filled(&drive_power.power, channels);
    }
    return false;
}

err_t drive_power_calibrate(power_channels_t channels)
{
    return power_calibrate(&drive_power.power, channels);
}

fixed32_t drive_power_value_multiplier(size_t channel)
{
    return power_value_multiplier(&drive_power.power, channel);
}

void drive_power_set_value_multiplier(size_t channel, fixed32_t mult)
{
    power_set_value_multiplier(&drive_power.power, channel, mult);
}

uint16_t drive_power_calibration_data(size_t channel)
{
    return power_calibration_data(&drive_power.power, channel);
}

void drive_power_set_calibration_data(size_t channel, uint16_t data)
{
    power_set_calibration_data(&drive_power.power, channel, data);
}

power_channel_type_t drive_power_channel_type(size_t channel)
{
    return power_channel_type(&drive_power.power, channel);
}

bool drive_power_data_avail(power_channels_t channels)
{
    return power_data_filter_filled(&drive_power.power, channels);
}

bool drive_power_new_data_avail(power_channels_t channels)
{
    return drive_power.iters_processed == 0 && drive_power_data_avail(channels);
}

err_t drive_power_reset_channels(power_channels_t channels)
{
    return power_reset_channels(&drive_power.power, channels);
}

int16_t drive_power_channel_raw_value_inst(size_t channel)
{
    return power_channel_raw_value_inst(&drive_power.power, channel);
}

int16_t drive_power_channel_raw_value(size_t channel)
{
    return power_channel_raw_value(&drive_power.power, channel);
}

fixed32_t drive_power_channel_real_value_inst(size_t channel)
{
    return power_channel_real_value_inst(&drive_power.power, channel);
}

fixed32_t drive_power_channel_real_value(size_t channel)
{
    return power_channel_real_value(&drive_power.power, channel);
}
