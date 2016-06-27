#include "drive_modbus.h"
#include "drive.h"
#include "settings.h"
#include <string.h>


#define DRIVE_ID_NAME_MAX 20

#pragma pack(push, 1)
typedef struct _DriveModbusId {
    uint16_t magic;
    uint8_t major;
    uint8_t minor;
    char name[DRIVE_ID_NAME_MAX];
} drive_modbus_id_t;
#pragma pack(pop)


#define DRIVE_ID_MAGIC 0x4702
#define DRIVE_ID_MAJOR 0x1
#define DRIVE_ID_MINOR 0x2
#define DRIVE_ID_NAME  "Drive"


//! Тип интерфейса Modbus привода.
typedef struct _Drive_modbus {
    modbus_rtu_t* modbus;
    drive_modbus_id_t id;
    apply_settings_callback_t apply_settings_callback;
    save_settings_callback_t save_settings_callback;
} drive_modbus_t;

//! Интерфейс привода.
static drive_modbus_t drive_modbus;


#define DRIVE_MODBUS_HOLD_REG_
#define DRIVE_MODBUS_INPUT_REG_
#define DRIVE_MODBUS_DIN_
#define DRIVE_MODBUS_COIL_

//! Начало адресов регистров хранения.
#define DRIVE_MODBUS_HOLD_REGS_START 2000
//! Начало адресов регистров ввода.
#define DRIVE_MODBUS_INPUT_REGS_START 2000
//! Начало адресов цифровых входов.
#define DRIVE_MODBUS_DINS_START 2000
//! Начало адресов регистров флагов.
#define DRIVE_MODBUS_COIS_START 2000

// Константы адресов.
// Регистры ввода.
// Регистры хранения.
//! Задание.
#define DRIVE_MODBUS_HOLD_REG_REFERENCE (DRIVE_MODBUS_HOLD_REGS_START + 0)
// Цифровые входа.
// Регистры флагов.
//! Запуск/останов.
#define DRIVE_MODBUS_COIL_RUN (DRIVE_MODBUS_COIS_START + 0)
//! Сброс ошибок.
#define DRIVE_MODBUS_COIL_CLEAR_ERRORS (DRIVE_MODBUS_COIS_START + 1)
//! Применение настроек.
#define DRIVE_MODBUS_COIL_APPLY_PARAMS (DRIVE_MODBUS_COIS_START + 2)
//! Сохранение настроек.
#define DRIVE_MODBUS_COIL_SAVE_PARAMS (DRIVE_MODBUS_COIS_START + 3)


ALWAYS_INLINE static int16_t pack_f32_f10_6(fixed32_t value)
{
    return value >> 10;
}

static modbus_rtu_error_t drive_modbus_on_read_din(uint16_t address, modbus_rtu_din_value_t* value)
{
    switch(address){
        default:
            return MODBUS_RTU_ERROR_INVALID_ADDRESS;
    }
    return MODBUS_RTU_ERROR_NONE;
}

static modbus_rtu_error_t drive_modbus_on_read_inp_reg(uint16_t address, uint16_t* value)
{
    param_t* param = NULL;
    switch(address){
        default:
            param = settings_param_by_id((param_id_t)address);
            if(param == NULL){
                return MODBUS_RTU_ERROR_INVALID_ADDRESS;
            }
            *value = settings_param_value_raw(param);
            break;
    }
    return MODBUS_RTU_ERROR_NONE;
}

static modbus_rtu_error_t drive_modbus_on_read_hold_reg(uint16_t address, uint16_t* value)
{
    param_t* param = NULL;
    switch(address){
        default:
            param = settings_param_by_id((param_id_t)address);
            if(param == NULL){
                return MODBUS_RTU_ERROR_INVALID_ADDRESS;
            }
            *value = settings_param_value_raw(param);
            break;
        case DRIVE_MODBUS_HOLD_REG_REFERENCE:
            *value = drive_reference();
            break;
    }
    return MODBUS_RTU_ERROR_NONE;
}

static modbus_rtu_error_t drive_modbus_on_write_reg(uint16_t address, uint16_t value)
{
    param_t* param = NULL;
    switch(address){
        default:
            param = settings_param_by_id((param_id_t)address);
            if(param == NULL || settings_param_is_virtual(param)){
                return MODBUS_RTU_ERROR_INVALID_ADDRESS;
            }
            settings_param_set_value_raw(param, (param_data_t)value);
            break;
        case DRIVE_MODBUS_HOLD_REG_REFERENCE:
            if(drive_set_reference(value) != E_NO_ERROR)
                return MODBUS_RTU_ERROR_INVALID_DATA;
            break;
    }
    return MODBUS_RTU_ERROR_NONE;
}

static modbus_rtu_error_t drive_modbus_on_read_coil(uint16_t address, modbus_rtu_coil_value_t* value)
{
    switch(address){
        default:
            return MODBUS_RTU_ERROR_INVALID_ADDRESS;
        case DRIVE_MODBUS_COIL_RUN:
            *value = drive_running();
            break;
        case DRIVE_MODBUS_COIL_CLEAR_ERRORS:
            *value = drive_errors() != DRIVE_ERROR_NONE;
            break;
    }
    return MODBUS_RTU_ERROR_NONE;
}

static modbus_rtu_error_t drive_modbus_on_write_coil(uint16_t address, modbus_rtu_coil_value_t value)
{
    switch(address){
        default:
            return MODBUS_RTU_ERROR_INVALID_ADDRESS;
        case DRIVE_MODBUS_COIL_RUN:
            value ? drive_start() : drive_stop();
            break;
        case DRIVE_MODBUS_COIL_CLEAR_ERRORS:
            if(value) drive_clear_errors();
            break;
        case DRIVE_MODBUS_COIL_APPLY_PARAMS:
            if(!drive_modbus.apply_settings_callback){
                return MODBUS_RTU_ERROR_INVALID_FUNC;
            }
            drive_modbus.apply_settings_callback();
            break;
        case DRIVE_MODBUS_COIL_SAVE_PARAMS:
            if(!drive_modbus.save_settings_callback){
                return MODBUS_RTU_ERROR_INVALID_FUNC;
            }
            drive_modbus.save_settings_callback();
            break;
    }
    return MODBUS_RTU_ERROR_NONE;
}

static modbus_rtu_error_t drive_modbus_on_report_slave_id(modbus_rtu_slave_id_t* slave_id)
{
    slave_id->status = MODBUS_RTU_RUN_STATUS_ON;
    slave_id->id = &drive_modbus.id;
    slave_id->id_size = sizeof(drive_modbus_id_t);
    slave_id->data = NULL;
    slave_id->data_size = 0;
    
    return MODBUS_RTU_ERROR_NONE;
}


err_t drive_modbus_init(modbus_rtu_t* modbus, drive_modbus_init_t* drive_modbus_is)
{
    if(modbus == NULL) return E_NULL_POINTER;
    
    drive_modbus.id.magic = DRIVE_ID_MAGIC;
    drive_modbus.id.major = DRIVE_ID_MAJOR;
    drive_modbus.id.minor = DRIVE_ID_MINOR;
    memcpy(drive_modbus.id.name, DRIVE_ID_NAME, strlen(DRIVE_ID_NAME) + 1);
    
    drive_modbus.modbus = modbus;
    drive_modbus.apply_settings_callback = drive_modbus_is->apply_settings_callback;
    drive_modbus.save_settings_callback = drive_modbus_is->save_settings_callback;
    
    modbus_rtu_set_read_coil_callback(drive_modbus.modbus, drive_modbus_on_read_coil);
    modbus_rtu_set_read_din_callback(drive_modbus.modbus, drive_modbus_on_read_din);
    modbus_rtu_set_read_holding_reg_callback(drive_modbus.modbus, drive_modbus_on_read_hold_reg);
    modbus_rtu_set_read_input_reg_callback(drive_modbus.modbus, drive_modbus_on_read_inp_reg);
    modbus_rtu_set_write_coil_callback(drive_modbus.modbus, drive_modbus_on_write_coil);
    modbus_rtu_set_write_holding_reg_callback(drive_modbus.modbus, drive_modbus_on_write_reg);
    modbus_rtu_set_report_slave_id_callback(drive_modbus.modbus, drive_modbus_on_report_slave_id);
    
    return E_NO_ERROR;
}
