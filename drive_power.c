#include "drive_power.h"
#include <string.h>

typedef struct _Drive_Power {
    phase_t power_phase; //!< Фаза отсчёта периодов измерения питания.
    power_value_t power_values[DRIVE_POWER_CHANNELS_COUNT]; //!< Значение каналов АЦП.
    power_t power; //!< Питание.
    size_t processing_periods; //!< Число периодов для накопления и обработки данных.
    size_t periods_processed; //!< Число периодов данных.
} drive_power_t;


static drive_power_t drive_power;


err_t drive_power_init(void)
{
    memset(&drive_power, 0x0, sizeof(drive_power_t));
    
    drive_power.processing_periods = DRIVE_POWER_PROCESSING_PERIODS_DEFAULT;
    
    drive_power.power_phase = PHASE_UNK;
    
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ua],POWER_CHANNEL_AC, 0x4478); // Ua
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ia],POWER_CHANNEL_AC, 0x5f11); // Ia
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ub],POWER_CHANNEL_AC, 0x44ac); // Ub
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ib],POWER_CHANNEL_AC, 0x5f11); // Ib
    power_value_init(&drive_power.power_values[DRIVE_POWER_Uc],POWER_CHANNEL_AC, 0x450b); // Uc
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ic],POWER_CHANNEL_AC, 0x5f11); // Ic
    power_value_init(&drive_power.power_values[DRIVE_POWER_Urot],POWER_CHANNEL_DC, 0x6861); // Urot
    power_value_init(&drive_power.power_values[DRIVE_POWER_Irot],POWER_CHANNEL_DC, 0x5f11); // Irot //0x2e14
    power_value_init(&drive_power.power_values[DRIVE_POWER_Iexc],POWER_CHANNEL_AC, 0x321); // Iexc //0x1bd
    power_value_init(&drive_power.power_values[DRIVE_POWER_Iref],POWER_CHANNEL_DC, 0x10000); // Iref
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ifan],POWER_CHANNEL_DC, 0x10000); // Ifan
    
    power_init(&drive_power.power, drive_power.power_values, DRIVE_POWER_CHANNELS);
    
    return E_NO_ERROR;
}

phase_t drive_power_phase(void)
{
    return drive_power.power_phase;
}

err_t drive_power_set_phase(phase_t phase)
{
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    
    drive_power.power_phase = phase;
    
    return E_NO_ERROR;
}

size_t drive_power_processing_periods(void)
{
    return drive_power.processing_periods;
}

err_t drive_power_set_processing_periods(size_t periods)
{
    //if(periods == 0) return E_INVALID_VALUE;
    
    drive_power.processing_periods = periods;
    
    return E_NO_ERROR;
}

err_t drive_power_process_adc_values(power_channels_t channels, uint16_t* adc_values)
{
    return power_process_adc_values(&drive_power.power, channels, adc_values);
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

bool drive_power_calc_values(power_channels_t channels, phase_t phase, err_t* err)
{
    if(drive_power.processing_periods == 0){
        drive_power_calc_values_impl(channels, err);
        return power_data_filter_filled(&drive_power.power, channels);
    }
    if(phase != drive_power.power_phase) return false;
    if(++ drive_power.periods_processed >= drive_power.processing_periods){
        drive_power.periods_processed = 0;
        drive_power_calc_values_impl(channels, err);
        return power_data_filter_filled(&drive_power.power, channels);
    }
    return false;
}

err_t drive_power_calibrate(power_channels_t channels)
{
    return power_calibrate(&drive_power.power, channels);
}

bool drive_power_data_avail(power_channels_t channels)
{
    return power_data_avail(&drive_power.power, channels);
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
