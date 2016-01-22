#include "settings.h"
#include "defs/defs.h"

//! Тип структуры настроек.
typedef struct _Settings {
    uint8_t nominal_voltage;
    uint8_t allowed_nominal_voltage_variation;
    uint8_t critical_nominal_voltage_variation;
    uint8_t zero_voltage_noise;
    uint16_t zero_current_noise;
    uint8_t excitation_phase;
} settings_t;


#define SETTINGS_DEFAULT_NOMINAL_VOLTAGE 220
#define SETTINGS_DEFAULT_ALLOWED_NOMINAL_VOLTAGE_VARIATION 5
#define SETTINGS_DEFAULT_CRITICAL_NOMINAL_VOLTAGE_VARIATION 10
#define SETTINGS_DEFAULT_ZERO_VOLTAGE_NOISE 1
#define SETTINGS_DEFAULT_ZERO_CURRENT_NOISE 100
#define SETTINGS_DEFAULT_EXCITATION_PHASE PHASE_A


static settings_t settings;
static bool _settings_readonly = false;


ALWAYS_INLINE static bool ro(void)
{
    return _settings_readonly;
}

void settings_init(void)
{
    settings.nominal_voltage = SETTINGS_DEFAULT_NOMINAL_VOLTAGE;
    settings.allowed_nominal_voltage_variation = SETTINGS_DEFAULT_ALLOWED_NOMINAL_VOLTAGE_VARIATION;
    settings.critical_nominal_voltage_variation = SETTINGS_DEFAULT_CRITICAL_NOMINAL_VOLTAGE_VARIATION;
    settings.zero_voltage_noise = SETTINGS_DEFAULT_ZERO_VOLTAGE_NOISE;
    settings.zero_current_noise = SETTINGS_DEFAULT_ZERO_CURRENT_NOISE;
    settings.excitation_phase = SETTINGS_DEFAULT_EXCITATION_PHASE;
}

err_t settings_read(void)
{
    return E_NOT_IMPLEMENTED;
}

err_t settings_write(void)
{
    return E_NOT_IMPLEMENTED;
}

bool settings_readonly(void)
{
    return _settings_readonly;
}

void settings_set_readonly(bool readonly)
{
    _settings_readonly = readonly;
}

uint8_t settings_nominal_voltage(void)
{
    return settings.nominal_voltage;
}

bool settings_set_nominal_voltage(uint8_t voltage)
{
    if(ro()) return false;
    settings.nominal_voltage = voltage;
    return true;
}

uint8_t settings_allowed_nominal_voltage_variation(void)
{
    return settings.allowed_nominal_voltage_variation;
}

bool settings_set_allowed_nominal_voltage_variation(uint8_t variation)
{
    if(ro()) return false;
    settings.allowed_nominal_voltage_variation = variation;
    return true;
}

uint8_t settings_critical_nominal_voltage_variation(void)
{
    return settings.critical_nominal_voltage_variation;
}

bool settings_set_critical_nominal_voltage_variation(uint8_t variation)
{
    if(ro()) return false;
    settings.critical_nominal_voltage_variation = variation;
    return true;
}

uint8_t settings_zero_voltage_noise(void)
{
    return settings.zero_voltage_noise;
}

bool settings_set_zero_voltage_noise(uint8_t noise)
{
    if(ro()) return false;
    settings.zero_voltage_noise = noise;
    return true;
}

uint16_t settings_zero_current_noise(void)
{
    return settings.zero_current_noise;
}

bool settings_set_zero_current_noise(uint16_t noise)
{
    if(ro()) return false;
    settings.zero_current_noise = noise;
    return true;
}

phase_t settings_excitation_phase(void)
{
    return settings.excitation_phase;
}

bool settings_set_excitation_phase(phase_t phase)
{
    if(ro()) return false;
    settings.excitation_phase = phase;
    return true;
}
