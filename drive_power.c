#include "drive_power.h"
#include "utils/utils.h"
#include "utils/critical.h"
#include "mid_filter/mid_filter3i.h"
#include "settings.h"
#include "drive_triacs.h"
#include "drive_protection.h"
#include "drive_tasks.h"
#include "drive_motor.h"
#include "channel_filter.h"
#include <string.h>
#include <arm_math.h>



//! Каналы питания для осциллограмм.
static const uint16_t drive_power_osc_channels_nums[DRIVE_POWER_OSC_CHANNELS_COUNT] = {
    DRIVE_POWER_Ua, DRIVE_POWER_Ub, DRIVE_POWER_Uc,
    DRIVE_POWER_Ia, DRIVE_POWER_Ib, DRIVE_POWER_Ic,
    DRIVE_POWER_Urot, DRIVE_POWER_Irot, DRIVE_POWER_Iexc
};

//! Максимальный разрешённый дрифт.
#define DRIVE_POWER_MAX_DRIFT 32

//! Коэффициент для расчёта напряжения по углу открытия.
#define DRIVE_POWER_K_UD0 (0x256CF) // 3 * sqrt(6) / pi == 2.3391.

/**
 * Делитель измерений АЦП.
 * Число точек за период осцилограммы = Число измерений за период ADC / (делитель измерений АЦП + 1).
 */
#define DRIVE_POWER_OSC_ADC_DIVIDER (POWER_ADC_MEASUREMENTS_PER_PERIOD / DRIVE_POWER_OSC_PERIOD_POINTS - 1)

//! Тип буфера для хранения осцилограмм.
typedef struct _Oscillogram_Buf {
    drive_power_osc_channel_t osc_channels[DRIVE_POWER_OSC_CHANNELS_COUNT];
    size_t index; //!< Текущий индекс в осциллограммах.
    size_t count; //!< Количество значений в осциллограммах.
    size_t pause_mark; //!< Метка возникновения ошибки.
    size_t skip_counter; //!< Счётчик числа пропущенных точек АЦП.
} oscillogram_buf_t;


/////////////////////////////
// Диагностика тиристоров. //
/////////////////////////////


//! Количество открытий тиристора за период.
#define TRIAC_NORMAL_OPENS_COUNT 2


//! Структура диагностики пары тиристоров.
typedef struct _Triac_Pair_Phase_Diag {
    mid_filter3i_t filter_zero; //!< Медианный фильтр сырых значений тока.
    mid_filter3i_t filter_val; //!< Медианный фильтр значений тока.
    int32_t sum_zero_raw; //!< Сумма нулевых значений тока (сырая).
    int32_t sum_count_raw; //!< Количество нулевых значний тока (сырых).
    int16_t val_zero_raw; //!< Значение нуля (сырое).
    fixed32_t val_zero; //!< Значение нуля, А.
} triac_pairs_phase_diag_t;

//! Структура диагностики тиристора.
typedef struct _Triac_Diag {
    triac_pair_number_t last_pair; //!< Пара тиристоров последнего детектирования открытия тиристора.
    size_t cur_open_count; //!< Текущее число открытий тиристора в пределах периода.
    bool cur_opened; //!< Флаг открытого тиристора в данный момент.
    bool cur_closed; //!< Флаг закрытого тиристора в данный момент.
    // Данные диагностики за последний период.
    size_t open_count; //!< Число открытий за последний период.
} triac_diag_t;

// Пары тиристоров.
//! Пара тиристоров фазы A.
#define TRIAC_PAIR_PHASE_A 0
//! Пара тиристоров фазы B.
#define TRIAC_PAIR_PHASE_B 1
//! Пара тиристоров фазы C.
#define TRIAC_PAIR_PHASE_C 2
//! Число пар тиристоров.
#define TRIAC_PAIR_PHASES_COUNT 3

//! Тип фазы пары тиристоров.
typedef size_t phase_triac_pair_t;

//! Структура диагностики тиристоров.
typedef struct _Triacs_Diag {
    triac_pairs_phase_diag_t pair_diags[TRIAC_PAIR_PHASES_COUNT];
    triac_diag_t triac_diags[TRIACS_COUNT];
    fixed32_t I_zero_noise; //!< Шум нуля АЦП.
    triac_pair_number_t last_pair; //!< Последняя обработанная пара.
    triac_pair_number_t period_pair; //!< Пара отсчёта периода.
    bool fail; //!< Флаг ошибки тиристоров.
} triacs_diag_t;


//! Ёмкость фильтра каналов питания.
#define CHANNEL_FILTER_CAPACITY 30
//! Буфер данных фильтра каналов питания.
typedef fixed32_t channel_filter_data_t[CHANNEL_FILTER_CAPACITY];

//! Тип питания привода.
typedef struct _Drive_Power {
    power_value_t power_values[DRIVE_POWER_CHANNELS_COUNT]; //!< Значение каналов АЦП.
    channel_filter_data_t channel_filters_data[DRIVE_POWER_CHANNELS_COUNT]; //!< Данные фильтров каналов АЦП.
    channel_filter_t channel_filters[DRIVE_POWER_CHANNELS_COUNT]; //!< Фильтры каналов АЦП.
    power_t power; //!< Питание.
    size_t processing_iters; //!< Число итераций для накопления и обработки данных.
    size_t iters_processed; //!< Число итераций обработки данных.
    size_t period_iters; //!< Счётчик периода измерений АЦП.
    oscillogram_buf_t osc_buf; //!< Буфер осциллограмм.
    phase_t phase_calc_current; //!< Вычислять ток заданной фазы.
    phase_t phase_calc_voltage; //!< Вычислять напряжение заданной фазы.
    bool rot_calc_current; //!< Вычислять ток якоря.
    bool rot_calc_voltage; //!< Вычислять напряжение якоря.
    bool exc_calc_current; //!< Вычислять ток возбуждения.
    // Вычисляемые значения.
    fixed32_t max_rectified_voltage; //!< Максимальное выпрямленное напряжение.
    fixed32_t open_angle_voltage; //!< Напряжение согласно углу открытия тиристоров.
    fixed32_t U_exc_calc; //!< Вычисленное значение напряжения возбуждения.
    fixed32_t I_exc_calc; //!< Вычисленное значение тока возбуждения.
    // Диагностика.
    triacs_diag_t triacs_diag; //!< Диагностика тиристоров.
} drive_power_t;

//! Питание привода.
static drive_power_t drive_power;



static void drive_power_reset_channel_filters(void)
{
    size_t i;
    for(i = 0; i < DRIVE_POWER_CHANNELS_COUNT; i ++){
        channel_filter_reset(&drive_power.channel_filters[i]);
    }
}

static void drive_power_calc_channel_filter(size_t channel)
{
    channel_filter_t* filter = &drive_power.channel_filters[channel];
    
    channel_filter_put(filter, power_channel_real_value(&drive_power.power, channel));
    channel_filter_calculate(filter);
}

static void drive_power_calc_channel_filters(void)
{
    size_t i;
    for(i = 0; i < DRIVE_POWER_CHANNELS_COUNT; i ++){
        drive_power_calc_channel_filter(i);
    }
}


static void drive_power_init_triacs_diag(void)
{
    triacs_diag_t* triacs_diag = &drive_power.triacs_diag;
    
    triacs_diag->last_pair = TRIAC_PAIR_UNKNOWN;
    triacs_diag->period_pair = TRIAC_PAIR_UNKNOWN;
    
    size_t i;
    for(i = 0; i < TRIACS_COUNT; i ++){
        triac_diag_t* triac_diag = &triacs_diag->triac_diags[i];
        triac_diag->last_pair = TRIAC_PAIR_UNKNOWN;
    }
}


err_t drive_power_init(void)
{
    memset(&drive_power, 0x0, sizeof(drive_power_t));
    
    drive_power.osc_buf.pause_mark = DRIVE_POWER_OSC_CHANNEL_LEN;
    
    drive_power.processing_iters = DRIVE_POWER_PROCESSING_ITERS_DEFAULT;
    
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ua],POWER_CHANNEL_AC, DRIVE_POWER_AC_DEFAULT_FILTER_SIZE, 0x10000); // Ua //0x4478
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ia],POWER_CHANNEL_AC, DRIVE_POWER_AC_DEFAULT_FILTER_SIZE, 0x10000); // Ia
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ub],POWER_CHANNEL_AC, DRIVE_POWER_AC_DEFAULT_FILTER_SIZE, 0x10000); // Ub //0x44ac
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ib],POWER_CHANNEL_AC, DRIVE_POWER_AC_DEFAULT_FILTER_SIZE, 0x10000); // Ib
    power_value_init(&drive_power.power_values[DRIVE_POWER_Uc],POWER_CHANNEL_AC, DRIVE_POWER_AC_DEFAULT_FILTER_SIZE, 0x10000); // Uc //0x450b
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ic],POWER_CHANNEL_AC, DRIVE_POWER_AC_DEFAULT_FILTER_SIZE, 0x10000); // Ic
    power_value_init(&drive_power.power_values[DRIVE_POWER_Urot],POWER_CHANNEL_DC, DRIVE_POWER_DC_DEFAULT_FILTER_SIZE, 0x10000); // Urot //0x6861
    power_value_init(&drive_power.power_values[DRIVE_POWER_Irot],POWER_CHANNEL_DC, DRIVE_POWER_DC_DEFAULT_FILTER_SIZE, 0x10000); // Irot //0x2e14
    power_value_init(&drive_power.power_values[DRIVE_POWER_Iexc],POWER_CHANNEL_AC, DRIVE_POWER_AC_DEFAULT_FILTER_SIZE, 0x10000); // Iexc //0x1bd
    power_value_init(&drive_power.power_values[DRIVE_POWER_Iref],POWER_CHANNEL_DC, DRIVE_POWER_DC_DEFAULT_FILTER_SIZE, 0x10000); // Iref
    power_value_init(&drive_power.power_values[DRIVE_POWER_Ifan],POWER_CHANNEL_DC, DRIVE_POWER_DC_DEFAULT_FILTER_SIZE, 0x10000); // Ifan
    power_value_init(&drive_power.power_values[DRIVE_POWER_Erot],POWER_CHANNEL_DC, DRIVE_POWER_DC_DEFAULT_FILTER_SIZE, 0x10000); // Erot
    
    channel_filter_init(&drive_power.channel_filters[DRIVE_POWER_Ua], drive_power.channel_filters_data[DRIVE_POWER_Ua], CHANNEL_FILTER_CAPACITY, CHANNEL_FILTER_DEFAULT_SIZE);
    channel_filter_init(&drive_power.channel_filters[DRIVE_POWER_Ia], drive_power.channel_filters_data[DRIVE_POWER_Ia], CHANNEL_FILTER_CAPACITY, CHANNEL_FILTER_DEFAULT_SIZE);
    channel_filter_init(&drive_power.channel_filters[DRIVE_POWER_Ub], drive_power.channel_filters_data[DRIVE_POWER_Ub], CHANNEL_FILTER_CAPACITY, CHANNEL_FILTER_DEFAULT_SIZE);
    channel_filter_init(&drive_power.channel_filters[DRIVE_POWER_Ib], drive_power.channel_filters_data[DRIVE_POWER_Ib], CHANNEL_FILTER_CAPACITY, CHANNEL_FILTER_DEFAULT_SIZE);
    channel_filter_init(&drive_power.channel_filters[DRIVE_POWER_Uc], drive_power.channel_filters_data[DRIVE_POWER_Uc], CHANNEL_FILTER_CAPACITY, CHANNEL_FILTER_DEFAULT_SIZE);
    channel_filter_init(&drive_power.channel_filters[DRIVE_POWER_Ic], drive_power.channel_filters_data[DRIVE_POWER_Ic], CHANNEL_FILTER_CAPACITY, CHANNEL_FILTER_DEFAULT_SIZE);
    channel_filter_init(&drive_power.channel_filters[DRIVE_POWER_Urot], drive_power.channel_filters_data[DRIVE_POWER_Urot], CHANNEL_FILTER_CAPACITY, CHANNEL_FILTER_DEFAULT_SIZE);
    channel_filter_init(&drive_power.channel_filters[DRIVE_POWER_Irot], drive_power.channel_filters_data[DRIVE_POWER_Irot], CHANNEL_FILTER_CAPACITY, CHANNEL_FILTER_DEFAULT_SIZE);
    channel_filter_init(&drive_power.channel_filters[DRIVE_POWER_Iexc], drive_power.channel_filters_data[DRIVE_POWER_Iexc], CHANNEL_FILTER_CAPACITY, CHANNEL_FILTER_DEFAULT_SIZE);
    channel_filter_init(&drive_power.channel_filters[DRIVE_POWER_Iref], drive_power.channel_filters_data[DRIVE_POWER_Iref], CHANNEL_FILTER_CAPACITY, CHANNEL_FILTER_DEFAULT_SIZE);
    channel_filter_init(&drive_power.channel_filters[DRIVE_POWER_Ifan], drive_power.channel_filters_data[DRIVE_POWER_Ifan], CHANNEL_FILTER_CAPACITY, CHANNEL_FILTER_DEFAULT_SIZE);
    channel_filter_init(&drive_power.channel_filters[DRIVE_POWER_Erot], drive_power.channel_filters_data[DRIVE_POWER_Erot], CHANNEL_FILTER_CAPACITY, CHANNEL_FILTER_DEFAULT_SIZE);
    
    power_init(&drive_power.power, drive_power.power_values, DRIVE_POWER_CHANNELS_COUNT);
    
    power_set_soft_channel(&drive_power.power, DRIVE_POWER_Erot, true);
    
    drive_power_init_triacs_diag();
    
    return E_NO_ERROR;
}

err_t drive_power_update_settings(void)
{
    drive_power.triacs_diag.I_zero_noise = settings_valuef(PARAM_ID_PROT_I_IN_IDLE_FAULT_LEVEL_VALUE);
    
    drive_power_set_adc_value_multiplier(DRIVE_POWER_Ua, settings_valuef(PARAM_ID_ADC_VALUE_MULTIPLIER_Ua));
    drive_power_set_adc_value_multiplier(DRIVE_POWER_Ub, settings_valuef(PARAM_ID_ADC_VALUE_MULTIPLIER_Ub));
    drive_power_set_adc_value_multiplier(DRIVE_POWER_Uc, settings_valuef(PARAM_ID_ADC_VALUE_MULTIPLIER_Uc));
    drive_power_set_adc_value_multiplier(DRIVE_POWER_Urot, settings_valuef(PARAM_ID_ADC_VALUE_MULTIPLIER_Urot));
    drive_power_set_adc_value_multiplier(DRIVE_POWER_Ia, settings_valuef(PARAM_ID_ADC_VALUE_MULTIPLIER_Ia));
    drive_power_set_adc_value_multiplier(DRIVE_POWER_Ib, settings_valuef(PARAM_ID_ADC_VALUE_MULTIPLIER_Ib));
    drive_power_set_adc_value_multiplier(DRIVE_POWER_Ic, settings_valuef(PARAM_ID_ADC_VALUE_MULTIPLIER_Ic));
    drive_power_set_adc_value_multiplier(DRIVE_POWER_Irot, settings_valuef(PARAM_ID_ADC_VALUE_MULTIPLIER_Irot));
    drive_power_set_adc_value_multiplier(DRIVE_POWER_Iexc, settings_valuef(PARAM_ID_ADC_VALUE_MULTIPLIER_Iexc));
    drive_power_set_adc_value_multiplier(DRIVE_POWER_Iref, settings_valuef(PARAM_ID_ADC_VALUE_MULTIPLIER_Iref));
    drive_power_set_adc_value_multiplier(DRIVE_POWER_Ifan, settings_valuef(PARAM_ID_ADC_VALUE_MULTIPLIER_Ifan));
    drive_power_set_adc_value_multiplier(DRIVE_POWER_Erot, 0x10000);
    
    drive_power_set_calibration_data(DRIVE_POWER_Ua, settings_valueu(PARAM_ID_ADC_CALIBRATION_DATA_Ua));
    drive_power_set_calibration_data(DRIVE_POWER_Ub, settings_valueu(PARAM_ID_ADC_CALIBRATION_DATA_Ub));
    drive_power_set_calibration_data(DRIVE_POWER_Uc, settings_valueu(PARAM_ID_ADC_CALIBRATION_DATA_Uc));
    drive_power_set_calibration_data(DRIVE_POWER_Urot, settings_valueu(PARAM_ID_ADC_CALIBRATION_DATA_Urot));
    drive_power_set_calibration_data(DRIVE_POWER_Ia, settings_valueu(PARAM_ID_ADC_CALIBRATION_DATA_Ia));
    drive_power_set_calibration_data(DRIVE_POWER_Ib, settings_valueu(PARAM_ID_ADC_CALIBRATION_DATA_Ib));
    drive_power_set_calibration_data(DRIVE_POWER_Ic, settings_valueu(PARAM_ID_ADC_CALIBRATION_DATA_Ic));
    drive_power_set_calibration_data(DRIVE_POWER_Irot, settings_valueu(PARAM_ID_ADC_CALIBRATION_DATA_Irot));
    drive_power_set_calibration_data(DRIVE_POWER_Iexc, settings_valueu(PARAM_ID_ADC_CALIBRATION_DATA_Iexc));
    drive_power_set_calibration_data(DRIVE_POWER_Iref, settings_valueu(PARAM_ID_ADC_CALIBRATION_DATA_Iref));
    drive_power_set_calibration_data(DRIVE_POWER_Ifan, settings_valueu(PARAM_ID_ADC_CALIBRATION_DATA_Ifan));
    
    drive_power_set_value_multiplier(DRIVE_POWER_Ua, settings_valuef(PARAM_ID_VALUE_MULTIPLIER_Ua));
    drive_power_set_value_multiplier(DRIVE_POWER_Ub, settings_valuef(PARAM_ID_VALUE_MULTIPLIER_Ub));
    drive_power_set_value_multiplier(DRIVE_POWER_Uc, settings_valuef(PARAM_ID_VALUE_MULTIPLIER_Uc));
    drive_power_set_value_multiplier(DRIVE_POWER_Urot, settings_valuef(PARAM_ID_VALUE_MULTIPLIER_Urot));
    drive_power_set_value_multiplier(DRIVE_POWER_Ia, settings_valuef(PARAM_ID_VALUE_MULTIPLIER_Ia));
    drive_power_set_value_multiplier(DRIVE_POWER_Ib, settings_valuef(PARAM_ID_VALUE_MULTIPLIER_Ib));
    drive_power_set_value_multiplier(DRIVE_POWER_Ic, settings_valuef(PARAM_ID_VALUE_MULTIPLIER_Ic));
    drive_power_set_value_multiplier(DRIVE_POWER_Irot, settings_valuef(PARAM_ID_VALUE_MULTIPLIER_Irot));
    drive_power_set_value_multiplier(DRIVE_POWER_Iexc, settings_valuef(PARAM_ID_VALUE_MULTIPLIER_Iexc));
    drive_power_set_value_multiplier(DRIVE_POWER_Iref, settings_valuef(PARAM_ID_VALUE_MULTIPLIER_Iref));
    drive_power_set_value_multiplier(DRIVE_POWER_Ifan, settings_valuef(PARAM_ID_VALUE_MULTIPLIER_Ifan));
    drive_power_set_value_multiplier(DRIVE_POWER_Erot, settings_valuef(PARAM_ID_VALUE_MULTIPLIER_Erot));
    
    channel_filter_resize_ms(&drive_power.channel_filters[DRIVE_POWER_Ua], settings_valueu(PARAM_ID_AVERAGING_TIME_Ua));
    channel_filter_resize_ms(&drive_power.channel_filters[DRIVE_POWER_Ia], settings_valueu(PARAM_ID_AVERAGING_TIME_Ia));
    channel_filter_resize_ms(&drive_power.channel_filters[DRIVE_POWER_Ub], settings_valueu(PARAM_ID_AVERAGING_TIME_Ub));
    channel_filter_resize_ms(&drive_power.channel_filters[DRIVE_POWER_Ib], settings_valueu(PARAM_ID_AVERAGING_TIME_Ib));
    channel_filter_resize_ms(&drive_power.channel_filters[DRIVE_POWER_Uc], settings_valueu(PARAM_ID_AVERAGING_TIME_Uc));
    channel_filter_resize_ms(&drive_power.channel_filters[DRIVE_POWER_Ic], settings_valueu(PARAM_ID_AVERAGING_TIME_Ic));
    channel_filter_resize_ms(&drive_power.channel_filters[DRIVE_POWER_Urot], settings_valueu(PARAM_ID_AVERAGING_TIME_Urot));
    channel_filter_resize_ms(&drive_power.channel_filters[DRIVE_POWER_Irot], settings_valueu(PARAM_ID_AVERAGING_TIME_Irot));
    channel_filter_resize_ms(&drive_power.channel_filters[DRIVE_POWER_Iexc], settings_valueu(PARAM_ID_AVERAGING_TIME_Iexc));
    channel_filter_resize_ms(&drive_power.channel_filters[DRIVE_POWER_Iref], settings_valueu(PARAM_ID_AVERAGING_TIME_Iref));
    channel_filter_resize_ms(&drive_power.channel_filters[DRIVE_POWER_Ifan], settings_valueu(PARAM_ID_AVERAGING_TIME_Ifan));
    channel_filter_resize_ms(&drive_power.channel_filters[DRIVE_POWER_Erot], settings_valueu(PARAM_ID_AVERAGING_TIME_Erot));
    
    return E_NO_ERROR;
}

void drive_power_reset(void)
{
    drive_power.iters_processed = 0;
}

phase_t drive_power_phase_calc_current(void)
{
    return drive_power.phase_calc_current;
}

phase_t drive_power_phase_calc_voltage(void)
{
    return drive_power.phase_calc_voltage;
}

bool drive_power_rot_calc_current(void)
{
    return drive_power.rot_calc_current;
}

bool drive_power_rot_calc_voltage(void)
{
    return drive_power.rot_calc_voltage;
}

bool drive_power_exc_calc_current(void)
{
    return drive_power.exc_calc_current;
}

bool drive_power_phase_can_calc_current(phase_t phase)
{
    return drive_power.phase_calc_current == PHASE_UNK ||
           drive_power.phase_calc_current == phase;
}

bool drive_power_phase_can_calc_voltage(phase_t phase)
{
    return drive_power.phase_calc_voltage == PHASE_UNK ||
           drive_power.phase_calc_voltage == phase;
}

bool drive_power_rot_can_calc_current(void)
{
    return true;
}

bool drive_power_rot_can_calc_voltage(void)
{
    return true;
}

bool drive_power_exc_can_calc_current(void)
{
    return true;
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

ALWAYS_INLINE static bool drive_power_get_voltage_channel_by_phase(phase_t phase, size_t* channel)
{
    switch(phase){
        default:
            break;
        case PHASE_A:
            if(channel) *channel = DRIVE_POWER_Ua;
            return true;
        case PHASE_B:
            if(channel) *channel = DRIVE_POWER_Ub;
            return true;
        case PHASE_C:
            if(channel) *channel = DRIVE_POWER_Uc;
            return true;
    }
    return false;
}

void drive_power_set_phase_calc_voltage(phase_t phase)
{
    size_t channel;
    
    if(drive_power_get_voltage_channel_by_phase(drive_power.phase_calc_voltage, &channel)){
        power_set_soft_channel(&drive_power.power, channel, false);
    }
    
    drive_power.phase_calc_voltage = phase;
    
    if(drive_power_get_voltage_channel_by_phase(phase, &channel)){
        power_set_soft_channel(&drive_power.power, channel, true);
    }
}

void drive_power_set_rot_calc_current(bool calc)
{
    drive_power.rot_calc_current = calc;
    
    power_set_soft_channel(&drive_power.power, DRIVE_POWER_Irot, calc);
}

void drive_power_set_rot_calc_voltage(bool calc)
{
    drive_power.rot_calc_voltage = calc;
    
    power_set_soft_channel(&drive_power.power, DRIVE_POWER_Urot, calc);
}

void drive_power_set_exc_calc_current(bool calc)
{
    drive_power.exc_calc_current = calc;
    
    power_set_soft_channel(&drive_power.power, DRIVE_POWER_Iexc, calc);
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

bool drive_power_oscillogram_is_paused(void)
{
    return drive_power.osc_buf.pause_mark != DRIVE_POWER_OSC_CHANNEL_LEN;
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
    if(drive_power.phase_calc_current == PHASE_UNK) return;
    
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

static void drive_power_calc_phase_voltage(void)
{
    if(drive_power.phase_calc_voltage == PHASE_UNK) return;
    
    fixed32_t U1 = 0, U2 = 0, U = 0;
    switch(drive_power.phase_calc_voltage){
        default:
            break;
        case PHASE_A:
            U1 = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Ub);
            U2 = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Uc);
            U = -(U1 + U2);
            power_process_soft_channel_value(&drive_power.power, DRIVE_POWER_Ua, U);
            break;
        case PHASE_B:
            U1 = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Ua);
            U2 = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Uc);
            U = -(U1 + U2);
            power_process_soft_channel_value(&drive_power.power, DRIVE_POWER_Ub, U);
            break;
        case PHASE_C:
            U1 = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Ua);
            U2 = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Ub);
            U = -(U1 + U2);
            power_process_soft_channel_value(&drive_power.power, DRIVE_POWER_Uc, U);
            break;
    }
}

static void drive_power_calc_rot_voltage(void)
{
    if(!drive_power.rot_calc_voltage) return;
    
    fixed32_t Urot = drive_power.open_angle_voltage;
    
    power_process_soft_channel_value(&drive_power.power, DRIVE_POWER_Urot, Urot);
}

static void drive_power_calc_exc_current_inst(void)
{
    if(!drive_power.exc_calc_current) return;
    
    fixed32_t Iexc = drive_power.I_exc_calc;
    
    power_process_soft_channel_value(&drive_power.power, DRIVE_POWER_Iexc, Iexc);
}

/**
 * Получает номер канала тока фазы.
 * @param phase Фаза.
 * @return Номер канала, -1 при неудаче.
 */
static int drive_power_phase_current_channel_number(phase_t phase)
{
    static const int8_t channel_number_table[PHASES_COUNT] = {
        DRIVE_POWER_Ia, DRIVE_POWER_Ib, DRIVE_POWER_Ic
    };
    
    if(phase == PHASE_UNK) return -1;
    
    return channel_number_table[phase - 1];
}

static void drive_power_calc_rot_current(void)
{
    if(!drive_power.rot_calc_current) return;
    
    triac_pair_number_t pair_number = drive_triacs_opened_pair();
    
    phase_t phase_hi = PHASE_UNK;
    phase_t phase_lo = PHASE_UNK;
    
    int ch_hi = -1;
    int ch_lo = -1;
    
    if(drive_triacs_phases_by_pair(pair_number, &phase_hi, &phase_lo, NULL)){
        ch_hi = drive_power_phase_current_channel_number(phase_hi);
        ch_lo = drive_power_phase_current_channel_number(phase_lo);
    }
    
    // Если оба канала верные.
    // Считаем ток по двум фазам.
    if(ch_hi != -1 && ch_lo != -1){
        
        fixed32_t Ihi = power_channel_real_value_inst(&drive_power.power, (size_t)ch_hi);
        fixed32_t Ilo = power_channel_real_value_inst(&drive_power.power, (size_t)ch_lo);
        
        Ihi = fixed_abs(Ihi);
        Ilo = fixed_abs(Ilo);
        
        fixed32_t Irot = (Ihi + Ilo) / 2;
        
        power_process_soft_channel_value(&drive_power.power, DRIVE_POWER_Irot, Irot);
        
    }else{ // Считаем среднее по трём фазам.
        fixed32_t Ia = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Ia);
        fixed32_t Ib = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Ib);
        fixed32_t Ic = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Ic);
        
        fixed32_t Irot = (Ia + Ib + Ic) / 3;
        
        power_process_soft_channel_value(&drive_power.power, DRIVE_POWER_Irot, Irot);
    }
}

/**
 * Получает номер защиты питания превышения тока нуля.
 * @param phase Фаза.
 * @return Номер защиты питания, -1 при неудаче.
 */
/*static int drive_power_phase_current_zero_prot_number(phase_t phase)
{
    static const int8_t prot_number_table[PHASES_COUNT] = {
        DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Ia,
        DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Ib,
        DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Ic
    };
    
    if(phase == PHASE_UNK) return -1;
    
    return prot_number_table[phase - 1];
}*/

static bool drive_power_triacs_diag_triac_check(triac_pair_number_t pair_number, triac_number_t triac_number, bool open, bool has_current)
{
    //if(!TRIAC_PAIR_VALID(pair_number)) return false;
    if(!TRIAC_VALID(triac_number)) return false;
    
    //fixed32_t abs_current = fixed_abs(current);
    
    triacs_diag_t* triacs_diag = &drive_power.triacs_diag;
    triac_diag_t* triac_diag = &triacs_diag->triac_diags[triac_number];
    
    bool res = true;
    bool opened = false;
    bool closed = false;
    
    //if(abs_current > triacs_diag->I_zero_noise){
    if(has_current){
        
        opened = true;
        closed = false;
        
        if(!open) res = false;
        
        if(triac_diag->last_pair != pair_number){
            triac_diag->cur_open_count ++;

            triac_diag->last_pair = pair_number;
        }
    }else{
        if(open) res = false;
        
        opened = false;
        closed = true;
    }
    
    triac_diag->cur_opened = opened;
    triac_diag->cur_closed = closed;
    
    return res;
}

static void drive_power_triacs_diag_process_phase(triac_pair_number_t pair_number, phase_t phase)
{
    if(phase == PHASE_UNK) return;
    
    int channel_n = drive_power_phase_current_channel_number(phase);
    if(channel_n == -1) return;
    
    //int pwr_prot_n = drive_power_phase_current_zero_prot_number(phase);
    //if(pwr_prot_n == -1) return;
    
    phase_triac_pair_t phase_index = (phase_triac_pair_t)phase - 1;
    triacs_diag_t* triacs_diag = &drive_power.triacs_diag;
    triac_pairs_phase_diag_t* pair_diag = &triacs_diag->pair_diags[phase_index];
    
    fixed32_t I_cur = power_channel_real_value_inst(&drive_power.power, (size_t)channel_n);
    int16_t I_cur_raw = power_channel_raw_value_inst(&drive_power.power, (size_t)channel_n);
    
    mid_filter3i_put(&pair_diag->filter_val, I_cur);
    mid_filter3i_put(&pair_diag->filter_zero, (int32_t)I_cur_raw);
    
    triac_number_t triac_hi_number = drive_triacs_phase_triac_by_pos(phase, TRIAC_POS_HI);
    triac_number_t triac_lo_number = drive_triacs_phase_triac_by_pos(phase, TRIAC_POS_LO);
    
    bool triac_hi_open = drive_triacs_triac_is_open(pair_number, triac_hi_number);
    bool triac_lo_open = drive_triacs_triac_is_open(pair_number, triac_lo_number);
    
    if(mid_filter3i_full(&pair_diag->filter_val)){
        fixed32_t I_filtered = mid_filter3i_value(&pair_diag->filter_val) - pair_diag->val_zero;
        
        bool has_current_hi = I_filtered > triacs_diag->I_zero_noise;
        bool has_current_lo = I_filtered < -triacs_diag->I_zero_noise;
        
        bool triac_hi_check = drive_power_triacs_diag_triac_check(pair_number, triac_hi_number, triac_hi_open, has_current_hi);
        bool triac_lo_check = drive_power_triacs_diag_triac_check(pair_number, triac_lo_number, triac_lo_open, has_current_lo);

        if((triac_hi_check && triac_lo_check) &&
           !(triac_hi_open || triac_lo_open)){
            if(mid_filter3i_full(&pair_diag->filter_zero)){
                pair_diag->sum_zero_raw += mid_filter3i_value(&pair_diag->filter_zero);
                pair_diag->sum_count_raw ++;
            }
        }
    }
}

static void drive_power_triacs_diag_reset_mid_buffers(void)
{
    triacs_diag_t* triacs_diag = &drive_power.triacs_diag;
    
    size_t i;
    for(i = 0; i < TRIAC_PAIR_PHASES_COUNT; i ++){
        triac_pairs_phase_diag_t* pair_diag = &triacs_diag->pair_diags[i];
        mid_filter3i_reset(&pair_diag->filter_val);
        mid_filter3i_reset(&pair_diag->filter_zero);
    }
    for(i = 0; i < TRIACS_COUNT; i ++){
        triac_diag_t* triac_diag = &triacs_diag->triac_diags[i];
        triac_diag->last_pair = TRIAC_PAIR_UNKNOWN;
    }
}

static void drive_power_triacs_diag_latch_opens_count(void)
{
    triacs_diag_t* triacs_diag = &drive_power.triacs_diag;
    
    size_t i;
    for(i = 0; i < TRIACS_COUNT; i ++){
        triac_diag_t* triac_diag = &triacs_diag->triac_diags[i];
        triac_diag->open_count = triac_diag->cur_open_count;
        triac_diag->cur_open_count = 0;
    }
}

static void drive_power_triacs_diag_process(void)
{
    triac_pair_number_t pair_number = drive_triacs_opened_pair();
    
    triacs_diag_t* triacs_diag = &drive_power.triacs_diag;
    
    if(pair_number != triacs_diag->last_pair){
        if(triacs_diag->period_pair == TRIAC_PAIR_UNKNOWN && TRIAC_PAIR_VALID(pair_number)){
            triacs_diag->period_pair = pair_number;
        }else if(pair_number == TRIAC_PAIR_NONE){
            triacs_diag->period_pair = TRIAC_PAIR_UNKNOWN;
        }

        //
        if(triacs_diag->period_pair == pair_number || triacs_diag->period_pair == TRIAC_PAIR_UNKNOWN){
            drive_power_triacs_diag_latch_opens_count();
        }
        
        drive_power_triacs_diag_reset_mid_buffers();
        triacs_diag->last_pair = pair_number;
    }
    
    if(pair_number != TRIAC_PAIR_UNKNOWN){
        drive_power_triacs_diag_process_phase(pair_number, PHASE_A);
        drive_power_triacs_diag_process_phase(pair_number, PHASE_B);
        drive_power_triacs_diag_process_phase(pair_number, PHASE_C);
    }
    
}

static void drive_power_calc_e_rot(void)
{
    static fixed32_t Irot_prev = 0;
    
    fixed32_t Urot = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Urot);
    fixed32_t Irot = power_channel_real_value_inst(&drive_power.power, DRIVE_POWER_Irot);
    fixed32_t Rrot = drive_motor_r_rot();
    fixed32_t Lrot = drive_motor_l_rot();
    fixed32_t dt = POWER_ADC_DT_F;
    
    fixed32_t IR = fixed32_mul((int64_t)Irot, Rrot);
    fixed32_t dI = Irot - Irot_prev;
    int64_t dIdt = fixed32_div((int64_t)dI, dt);
    fixed32_t LdIdt = fixed32_mul((int64_t)dIdt, Lrot);
    
    fixed32_t Erot = Urot - IR - LdIdt;
    
    Irot_prev = Irot;
    
    power_process_soft_channel_value(&drive_power.power, DRIVE_POWER_Erot, Erot);
}

err_t drive_power_process_adc_values(power_channels_t channels, uint16_t* adc_values)
{
    err_t err = power_process_adc_values(&drive_power.power, channels, adc_values);
    
    drive_power_calc_e_rot();
    
    drive_power_calc_phase_current();
    
    drive_power_calc_phase_voltage();
    
    drive_power_calc_rot_current();
    
    drive_power_calc_rot_voltage();
    
    drive_power_calc_exc_current_inst();
    
    drive_power_append_osc_data();
    
    drive_power_triacs_diag_process();
    
    return err;
}

err_t drive_power_process_accumulated_data(power_channels_t channels)
{
    err_t err = power_process_accumulated_data(&drive_power.power, channels);
    
    return err;
}

static void drive_power_calc_angle_voltage(void)
{
    fixed32_t Uin = (power_channel_real_value(&drive_power.power, DRIVE_POWER_Ua) +
                     power_channel_real_value(&drive_power.power, DRIVE_POWER_Ub) +
                     power_channel_real_value(&drive_power.power, DRIVE_POWER_Uc)) / 3;
    
    fixed32_t Ud0 = fixed32_mul((int64_t)Uin, DRIVE_POWER_K_UD0);
    
    fixed32_t open_angle = drive_triacs_pairs_start_open_angle();
    
    q15_t open_angle_scaled = 0;
    fixed32_t open_angle_cos = 0;
    
    if(open_angle < fixed32_make_from_int(60)){
        // 0...360 --[/360]-> 0...1(0...65536) --[>>1]-> 0...32768
        open_angle_scaled = (q15_t)((open_angle / 360) >> 1);
        open_angle_cos = (fixed32_t)arm_cos_q15(open_angle_scaled) * 2;
    }else{
        // 0...360 --[/360]-> 0...1(0...65536) --[>>1]-> 0...32768
        open_angle_scaled = (q15_t)(((open_angle + fixed32_make_from_int(60)) / 360) >> 1);
        open_angle_cos = fixed32_make_from_int(1) + (fixed32_t)arm_cos_q15(open_angle_scaled) * 2;
    }
    
    drive_power.open_angle_voltage = fixed32_mul((int64_t)Ud0, open_angle_cos);
    drive_power.max_rectified_voltage = Ud0;
}

static void drive_power_calc_exc_current_rms(void)
{
    if(!drive_power.exc_calc_current) return;
    
    drive_triacs_exc_mode_t exc_mode = drive_triacs_exc_mode();
    
    // Внешнее возбуждение или не открытый симистор - измеряемый ток равен нулю.
    if(exc_mode == DRIVE_TRIACS_EXC_EXTERNAL || !drive_triacs_exc_enabled()){
        drive_power.I_exc_calc = 0;
        drive_power.U_exc_calc = 0;
        return;
    }
    
    phase_t exc_phase = drive_triacs_exc_phase();
    
    fixed32_t Uin = 0;
    
    switch(exc_phase){
        default:
            Uin = (power_channel_real_value(&drive_power.power, DRIVE_POWER_Ua) +
                   power_channel_real_value(&drive_power.power, DRIVE_POWER_Ub) +
                   power_channel_real_value(&drive_power.power, DRIVE_POWER_Uc)) / 3;
            break;
        case PHASE_A:
            Uin = power_channel_real_value(&drive_power.power, DRIVE_POWER_Ua);
            break;
        case PHASE_B:
            Uin = power_channel_real_value(&drive_power.power, DRIVE_POWER_Ub);
            break;
        case PHASE_C:
            Uin = power_channel_real_value(&drive_power.power, DRIVE_POWER_Uc);
            break;
    }
    
    fixed32_t U_exc = 0;
    
    // Регулируемое возбуждение - вычислим напряжение по углу открытия.
    if(exc_mode == DRIVE_TRIACS_EXC_REGULATED){
        fixed32_t open_angle = drive_triacs_exc_start_open_angle();
        
        q15_t open_angle_scaled = 0;
        fixed32_t open_angle_cos = 0;

        // 0...360 --[/360]-> 0...1(0...65536) --[>>1]-> 0...32768
        open_angle_scaled = (q15_t)((open_angle / 360) >> 1);
        open_angle_cos = fixed32_make_from_fract(1,2) + (fixed32_t)arm_cos_q15(open_angle_scaled);

        //Uin = Uin / 2;
        U_exc = fixed32_mul((int64_t)Uin, open_angle_cos);
    }else{
        U_exc = Uin;
    }
    
    fixed32_t R_exc = drive_motor_r_exc();
    
    fixed32_t I_exc = fixed32_div((int64_t)U_exc, R_exc);
    
    drive_power.U_exc_calc = U_exc;
    drive_power.I_exc_calc = I_exc;
}

static void drive_power_triacs_diag_calc_zeros(void)
{
    triacs_diag_t* triacs_diag = &drive_power.triacs_diag;
    
    size_t i;
    for(i = 0; i < TRIAC_PAIR_PHASES_COUNT; i ++){
        triac_pairs_phase_diag_t* pair_diag = &triacs_diag->pair_diags[i];
        
        int32_t sum = 0;
        int32_t count = 0;
        
        CRITICAL_ENTER();
        
        count = pair_diag->sum_count_raw;
        sum = pair_diag->sum_zero_raw;
        
        pair_diag->sum_count_raw = 0;
        pair_diag->sum_zero_raw = 0;
        
        CRITICAL_EXIT();
        
        if(count){
            phase_t pair_phase = (phase_t)i + 1;
            
            int pwr_ch = drive_power_phase_current_channel_number(pair_phase);
            if(pwr_ch == -1) continue;
            
            fixed32_t adc_mult = power_adc_value_multiplier(&drive_power.power, pair_phase);
            fixed32_t value_mult = power_value_multiplier(&drive_power.power, pair_phase);
            
            int64_t sum_real = (int64_t)sum * adc_mult;
            sum_real = fixed32_mul(sum_real, value_mult);
            
            pair_diag->val_zero_raw = sum / count;
            pair_diag->val_zero = sum_real / count;
        }
    }
}

static void drive_power_diag_triacs(void)
{
    triacs_diag_t* triacs_diag = &drive_power.triacs_diag;
    
    bool fail = false;
    
    bool triacs_enabled = drive_triacs_pairs_enabled();
    
    size_t i;
    for(i = 0; i < TRIACS_COUNT; i ++){
        triac_diag_t* triac_diag = &triacs_diag->triac_diags[i];
        if(triacs_enabled && triac_diag->open_count != TRIAC_NORMAL_OPENS_COUNT){
            fail = true;
        }
        if(!triacs_enabled && triac_diag->open_count != 0){
            fail = true;
        }
    }
    
    triacs_diag->fail = fail;
}

static void drive_power_diag_triacs_period_reset(void)
{
    triacs_diag_t* triacs_diag = &drive_power.triacs_diag;
    
    if(drive_triacs_opened_pair() == TRIAC_PAIR_NONE){
        
        CRITICAL_ENTER();
        
        size_t i;
        for(i = 0; i < TRIACS_COUNT; i ++){
            triac_diag_t* triac_diag = &triacs_diag->triac_diags[i];
            triac_diag->last_pair = TRIAC_PAIR_UNKNOWN;
            triac_diag->open_count = triac_diag->cur_open_count;
            triac_diag->cur_open_count = 0;
        }
        
        CRITICAL_EXIT();
    }
}

static void drive_power_diag_triac_pairs(void)
{
    drive_power_triacs_diag_calc_zeros();
    
    drive_power_diag_triacs();
    
    drive_power_diag_triacs_period_reset();
}

bool drive_power_triacs_fail(void)
{
    return drive_power.triacs_diag.fail;
}

size_t drive_power_triac_open_count(triac_number_t triac_number)
{
    if(!TRIAC_VALID(triac_number)) return 0;
    
    triacs_diag_t* triacs_diag = &drive_power.triacs_diag;
    
    return triacs_diag->triac_diags[triac_number].open_count;
}

static void drive_power_calc_values_impl(power_channels_t channels, err_t* err)
{
    err_t e = power_calc_values(&drive_power.power, channels);
    if(err) *err = e;
    
    drive_power_calc_channel_filters();
    
    drive_power_calc_angle_voltage();
    
    drive_power_calc_exc_current_rms();
}

bool drive_power_calc_values(power_channels_t channels, err_t* err)
{
    bool res = false;
    
    if(drive_power.processing_iters == 0){
        drive_power_calc_values_impl(channels, err);
        res = power_data_filter_filled(&drive_power.power, channels);
    }else if(++ drive_power.iters_processed >= drive_power.processing_iters){
        drive_power.iters_processed = 0;
        drive_power_calc_values_impl(channels, err);
        res = power_data_filter_filled(&drive_power.power, channels);
    }
    
    // Если прошёл очередной период.
    // И питание вычислено.
    if(++ drive_power.period_iters >= POWER_PERIOD_ITERS){
        drive_power.period_iters = 0;
        drive_power_diag_triac_pairs();
    }
    
    return res;
}

err_t drive_power_calibrate(power_channels_t channels)
{
    return power_calibrate(&drive_power.power, channels);
}

bool drive_power_channel_adc_filter_enabled(size_t channel)
{
    return power_channel_adc_filter_enabled(&drive_power.power, channel);
}

void drive_power_channel_set_adc_filter_enabled(size_t channel, bool enabled)
{
    power_channel_set_adc_filter_enabled(&drive_power.power, channel, enabled);
}

fixed32_t drive_power_adc_value_multiplier(size_t channel)
{
    return power_adc_value_multiplier(&drive_power.power, channel);
}

void drive_power_set_adc_value_multiplier(size_t channel, fixed32_t mult)
{
    power_set_adc_value_multiplier(&drive_power.power, channel, mult);
}

uint16_t drive_power_calibration_data(size_t channel)
{
    return power_adc_calibration_data(&drive_power.power, channel);
}

void drive_power_set_calibration_data(size_t channel, uint16_t data)
{
    power_set_adc_calibration_data(&drive_power.power, channel, data);
}

fixed32_t drive_power_value_multiplier(size_t channel)
{
    return power_value_multiplier(&drive_power.power, channel);
}

void drive_power_set_value_multiplier(size_t channel, fixed32_t mult)
{
    power_set_value_multiplier(&drive_power.power, channel, mult);
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
    drive_power_reset_channel_filters();
    return power_reset_channels(&drive_power.power, channels);
}

uint16_t drive_power_channel_raw_adc_value_inst(size_t channel)
{
    return power_channel_raw_adc_value_inst(&drive_power.power, channel);
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
    if(channel >= DRIVE_POWER_CHANNELS_COUNT) return 0;
    return channel_filter_value(&drive_power.channel_filters[channel]);
    //return power_channel_real_value(&drive_power.power, channel);
}

fixed32_t drive_power_open_angle_voltage(void)
{
    return drive_power.open_angle_voltage;
}

fixed32_t drive_power_max_rectified_voltage(void)
{
    return drive_power.max_rectified_voltage;
}
