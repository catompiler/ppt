#include "drive_modbus.h"
#include "drive.h"
#include "drive_events.h"
#include "drive_tasks.h"
#include "settings.h"
#include "future/future.h"
#include "utils/utils.h"
#include "utils/net.h"
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
    drive_event_t event_buf;
    future_t event_future;
    future_t osc_future;
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
#define DRIVE_MODBUS_COILS_START 2000

// Константы адресов.
// Регистры ввода.
//! Полуслова ошибок.
#define DRIVE_MODBUS_INPUT_REG_ERRORS0 (DRIVE_MODBUS_INPUT_REGS_START + 0)
#define DRIVE_MODBUS_INPUT_REG_ERRORS1 (DRIVE_MODBUS_INPUT_REGS_START + 1)
//! Полуслова предупреждений
#define DRIVE_MODBUS_INPUT_REG_WARNINGS0 (DRIVE_MODBUS_INPUT_REGS_START + 2)
#define DRIVE_MODBUS_INPUT_REG_WARNINGS1 (DRIVE_MODBUS_INPUT_REGS_START + 3)
//! Полуслова ошибок питания.
#define DRIVE_MODBUS_INPUT_REG_PWR_ERRORS0 (DRIVE_MODBUS_INPUT_REGS_START + 4)
#define DRIVE_MODBUS_INPUT_REG_PWR_ERRORS1 (DRIVE_MODBUS_INPUT_REGS_START + 5)
//! Полуслова предупреждений питания.
#define DRIVE_MODBUS_INPUT_REG_PWR_WARNINGS0 (DRIVE_MODBUS_INPUT_REGS_START + 6)
#define DRIVE_MODBUS_INPUT_REG_PWR_WARNINGS1 (DRIVE_MODBUS_INPUT_REGS_START + 7)
//! Полуслова ошибок фаз.
#define DRIVE_MODBUS_INPUT_REG_PHASE_ERRORS0 (DRIVE_MODBUS_INPUT_REGS_START + 8)
#define DRIVE_MODBUS_INPUT_REG_PHASE_ERRORS1 (DRIVE_MODBUS_INPUT_REGS_START + 9)
// Регистры хранения.
//! Задание.
#define DRIVE_MODBUS_HOLD_REG_REFERENCE (DRIVE_MODBUS_HOLD_REGS_START + 0)
// Цифровые входа.
// Регистры флагов.
//! Запуск/останов.
#define DRIVE_MODBUS_COIL_RUN (DRIVE_MODBUS_COILS_START + 0)
//! Сброс ошибок.
#define DRIVE_MODBUS_COIL_CLEAR_ERRORS (DRIVE_MODBUS_COILS_START + 1)
//! Применение настроек.
#define DRIVE_MODBUS_COIL_APPLY_PARAMS (DRIVE_MODBUS_COILS_START + 2)
//! Сохранение настроек.
#define DRIVE_MODBUS_COIL_SAVE_PARAMS (DRIVE_MODBUS_COILS_START + 3)
//! Калибровка питания.
#define DRIVE_MODBUS_COIL_CALIBRATE_POWER (DRIVE_MODBUS_COILS_START + 4)
//! Очистка событий.
#define DRIVE_MODBUS_COIL_CLEAR_EVENTS (DRIVE_MODBUS_COILS_START + 5)


/** Пользовательские функции и коды.
 * Запрос: | func | code | [args] |
 * Ответ:  | func | code | [data] |
 * func - номер функции, 1 байт.
 * code - код операции, 1 байт.
 */

// Константы статуса асинхронных операций.
//! Операция не начата.
#define DRIVE_MODBUS_ASYNC_OP_IDLE 0
//! Операция завершена.
#define DRIVE_MODBUS_ASYNC_OP_DONE 1
//! Операция в процессе.
#define DRIVE_MODBUS_ASYNC_OP_RUNNING 2
//! Ошибка при выполнении операции.
#define DRIVE_MODBUS_ASYNC_OP_ERROR 3

//! Функция доступа к списку событий.
#define DRIVE_MODBUS_FUNC_EVENTS_ACCESS 65
/**
 * Код получения числа событий.
 * Запрос: | 65 | 0 |
 * Ответ:  | 65 | 0 | N |
 * N - число событий, 1 байт.
 */
#define DRIVE_MODBUS_CODE_EVENTS_COUNT 0
/**
 * Код запуска чтения события
 * с заданным номером.
 * Запрос: | 65 | 1 | N |
 * Ответ:  | 65 | 1 | N |
 * N - номер события, 1 байт.
 */
#define DRIVE_MODBUS_CODE_READ_EVENT 1
/**
 * Код получения статуса
 * чтения события.
 * Запрос: | 65 | 2 |
 * Ответ:  | 65 | 2 | N |
 * N - состояние чтения события, 1 байт:
 *     0 - событие не прочитано;
 *     1 - событие прочитано;
 *     2 - чтение в процессе.
 *     3 - ошибка чтения события.
 */
#define DRIVE_MODBUS_CODE_READ_EVENT_STATUS 2
/**
 * Код чтения прочитанного
 * события.
 * Запрос: | 65 | 3 |
 * Ответ:  | 65 | 3 | N | data |
 * N - размер данных события, 1 байт;
 * data - данные события, N байт.
 */
#define DRIVE_MODBUS_CODE_GET_READED_EVENT 3

//! Функция доступа к осциллограммам.
#define DRIVE_MODBUS_FUNC_OSC_ACCESS 66
/**
 * Код получения числа осциллограмм.
 * Запрос: | 66 | 0 |
 * Ответ:  | 66 | 0 | N |
 * N - число осциллограмм, 1 байт.
 */
#define DRIVE_MODBUS_CODE_OSC_COUNT 0
/**
 * Код получения идентификатора
 * события осциллограммы с
 * заданным номером.
 * Запрос: | 66 | 1 | N |
 * Ответ:  | 66 | 1 | ID |
 * N - номер осциллограммы, 1 байт.
 * ID - идентификатор события, 1 байт.
 */
#define DRIVE_MODBUS_CODE_OSC_EVENT_ID 1
/**
 * Код запуска чтения заданного
 * канала осциллограммы
 * с заданным номером.
 * Запрос: | 66 | 2 | N | CH |
 * Ответ:  | 66 | 2 | N | CH |
 * N - номер осциллограммы, 1 байт.
 * CH - канал осциллограммы, 1 байт.
 */
#define DRIVE_MODBUS_CODE_READ_OSC 2
/**
 * Код получения статуса
 * чтения осциллограммы.
 * Запрос: | 66 | 3 |
 * Ответ:  | 66 | 3 | N |
 * N - состояние чтения осциллограммы, 1 байт:
 *     0 - осциллограмма не прочитана;
 *     1 - осциллограмма прочитана;
 *     2 - чтение в процессе.
 *     3 - ошибка чтения осциллограммы.
 */
#define DRIVE_MODBUS_CODE_READ_OSC_STATUS 3
/**
 * Код чтения прочитанной
 * осциллограммы.
 * Запрос: | 66 | 4 | A | N |
 * Ответ:  | 66 | 4 | N | data |
 * A - смещение в данных канала осциллограммы, 2 байта, старшим вперёд.
 * N - размер данных осциллограммы, 1 байт;
 * data - данные осциллограммы, N байт.
 */
#define DRIVE_MODBUS_CODE_GET_READED_OSC 4



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
        case DRIVE_MODBUS_INPUT_REG_ERRORS0:
            *value = drive_errors() & 0xffff;
            break;
        case DRIVE_MODBUS_INPUT_REG_ERRORS1:
            *value = (drive_errors() >> 16) & 0xffff;
            break;
        case DRIVE_MODBUS_INPUT_REG_WARNINGS0:
            *value = drive_warnings() & 0xffff;
            break;
        case DRIVE_MODBUS_INPUT_REG_WARNINGS1:
            *value = (drive_warnings() >> 16) & 0xffff;
            break;
        case DRIVE_MODBUS_INPUT_REG_PWR_ERRORS0:
            *value = drive_power_errors() & 0xffff;
            break;
        case DRIVE_MODBUS_INPUT_REG_PWR_ERRORS1:
            *value = (drive_power_errors() >> 16) & 0xffff;
            break;
        case DRIVE_MODBUS_INPUT_REG_PWR_WARNINGS0:
            *value = drive_power_warnings() & 0xffff;
            break;
        case DRIVE_MODBUS_INPUT_REG_PWR_WARNINGS1:
            *value = (drive_power_warnings() >> 16) & 0xffff;
            break;
        case DRIVE_MODBUS_INPUT_REG_PHASE_ERRORS0:
            *value = drive_phase_errors() & 0xffff;
            break;
        case DRIVE_MODBUS_INPUT_REG_PHASE_ERRORS1:
            *value = (drive_phase_errors() >> 16) & 0xffff;
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
        case DRIVE_MODBUS_COIL_CALIBRATE_POWER:
            drive_calibrate_power();
            break;
        case DRIVE_MODBUS_COIL_CLEAR_EVENTS:
            drive_tasks_clear_events();
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

modbus_rtu_error_t drive_modbus_events_access_count(void* tx_data, size_t* tx_size)
{
    ((uint8_t*)tx_data)[0] = DRIVE_MODBUS_CODE_EVENTS_COUNT;
    ((uint8_t*)tx_data)[1] = (uint8_t)drive_events_count();
    *tx_size = 2;
    return MODBUS_RTU_ERROR_NONE;
}

modbus_rtu_error_t drive_modbus_events_access_read_event(const void* rx_data, size_t rx_size, void* tx_data, size_t* tx_size)
{
    if(rx_size != 2) return MODBUS_RTU_ERROR_INVALID_DATA;
    
    size_t index = (size_t)((uint8_t*)rx_data)[1];
    
    drive_event_index_t event_index = drive_events_index_by_number(index);
    
    future_init(&drive_modbus.event_future);
    if(drive_tasks_read_event(&drive_modbus.event_future, event_index, &drive_modbus.event_buf) != E_NO_ERROR){
        return MODBUS_RTU_ERROR_INVALID_DATA;
    }
    
    ((uint8_t*)tx_data)[0] = DRIVE_MODBUS_CODE_READ_EVENT;
    ((uint8_t*)tx_data)[1] = (uint8_t)index;
    *tx_size = 2;
    
    return MODBUS_RTU_ERROR_NONE;
}

modbus_rtu_error_t drive_modbus_events_access_status(void* tx_data, size_t* tx_size)
{
    uint8_t res = DRIVE_MODBUS_ASYNC_OP_IDLE;
    
    if(future_done(&drive_modbus.event_future)){
        if(pvoid_to_int(err_t, future_result(&drive_modbus.event_future)) == E_NO_ERROR){
            res = DRIVE_MODBUS_ASYNC_OP_DONE;
        }else{
            res = DRIVE_MODBUS_ASYNC_OP_ERROR;
        }
    }else if(future_running(&drive_modbus.event_future)){
        res = DRIVE_MODBUS_ASYNC_OP_RUNNING;
    }
    
    ((uint8_t*)tx_data)[0] = DRIVE_MODBUS_CODE_READ_EVENT_STATUS;
    ((uint8_t*)tx_data)[1] = (res != DRIVE_MODBUS_ASYNC_OP_ERROR) ? res
                              : res + pvoid_to_int(err_t, future_result(&drive_modbus.event_future));
    *tx_size = 2;
    
    return MODBUS_RTU_ERROR_NONE;
}

modbus_rtu_error_t drive_modbus_events_access_get_event(void* tx_data, size_t* tx_size)
{
    ((uint8_t*)tx_data)[0] = DRIVE_MODBUS_CODE_GET_READED_EVENT;
    ((uint8_t*)tx_data)[1] = sizeof(drive_event_t);
    memcpy(&((uint8_t*)tx_data)[2], &drive_modbus.event_buf, sizeof(drive_event_t));
    *tx_size = sizeof(drive_event_t) + 2;
    
    return MODBUS_RTU_ERROR_NONE;
}

modbus_rtu_error_t drive_modbus_events_access(const void* rx_data, size_t rx_size, void* tx_data, size_t* tx_size)
{
    if(rx_size == 0) return MODBUS_RTU_ERROR_INVALID_DATA;
    
    uint8_t cmd = *(uint8_t*)rx_data;
    
    switch(cmd){
        default:
            return MODBUS_RTU_ERROR_INVALID_DATA;
        case DRIVE_MODBUS_CODE_EVENTS_COUNT:
            return drive_modbus_events_access_count(tx_data, tx_size);
        case DRIVE_MODBUS_CODE_READ_EVENT:
            return drive_modbus_events_access_read_event(rx_data, rx_size, tx_data, tx_size);
        case DRIVE_MODBUS_CODE_READ_EVENT_STATUS:
            return drive_modbus_events_access_status(tx_data, tx_size);
        case DRIVE_MODBUS_CODE_GET_READED_EVENT:
            return drive_modbus_events_access_get_event(tx_data, tx_size);
    }
    
    return MODBUS_RTU_ERROR_NONE;
}

modbus_rtu_error_t drive_modbus_osc_access_count(void* tx_data, size_t* tx_size)
{
    ((uint8_t*)tx_data)[0] = DRIVE_MODBUS_CODE_OSC_COUNT;
    ((uint8_t*)tx_data)[1] = (uint8_t)drive_events_oscillograms_count();
    *tx_size = 2;
    return MODBUS_RTU_ERROR_NONE;
}

modbus_rtu_error_t drive_modbus_osc_access_event_id(const void* rx_data, size_t rx_size, void* tx_data, size_t* tx_size)
{
    if(rx_size != 2) return MODBUS_RTU_ERROR_INVALID_DATA;
    
    size_t index = (size_t)((uint8_t*)rx_data)[1];
    
    drive_osc_index_t osc_index = drive_events_osc_index_by_number(index);
    drive_event_id_t event_id = drive_events_osc_event_id(osc_index);
    
    ((uint8_t*)tx_data)[0] = DRIVE_MODBUS_CODE_READ_OSC;
    ((uint8_t*)tx_data)[1] = (uint8_t)event_id;
    *tx_size = 2;
    
    return MODBUS_RTU_ERROR_NONE;
}

modbus_rtu_error_t drive_modbus_osc_access_read_osc(const void* rx_data, size_t rx_size, void* tx_data, size_t* tx_size)
{
    if(rx_size != 3) return MODBUS_RTU_ERROR_INVALID_DATA;
    
    size_t index = (size_t)((uint8_t*)rx_data)[1];
    size_t channel = (size_t)((uint8_t*)rx_data)[2];
    
    drive_osc_index_t osc_index = drive_events_osc_index_by_number(index);
    
    future_init(&drive_modbus.osc_future);
    if(drive_tasks_read_osc_channel(&drive_modbus.osc_future, osc_index, channel) != E_NO_ERROR){
        return MODBUS_RTU_ERROR_INVALID_DATA;
    }
    
    ((uint8_t*)tx_data)[0] = DRIVE_MODBUS_CODE_READ_OSC;
    ((uint8_t*)tx_data)[1] = (uint8_t)index;
    ((uint8_t*)tx_data)[2] = (uint8_t)channel;
    *tx_size = 3;
    
    return MODBUS_RTU_ERROR_NONE;
}

modbus_rtu_error_t drive_modbus_osc_access_status(void* tx_data, size_t* tx_size)
{
    uint8_t res = DRIVE_MODBUS_ASYNC_OP_IDLE;
    
    if(future_done(&drive_modbus.osc_future)){
        if(pvoid_to_int(err_t, future_result(&drive_modbus.osc_future)) == E_NO_ERROR){
            res = DRIVE_MODBUS_ASYNC_OP_DONE;
        }else{
            res = DRIVE_MODBUS_ASYNC_OP_ERROR;
        }
    }else if(future_running(&drive_modbus.osc_future)){
        res = DRIVE_MODBUS_ASYNC_OP_RUNNING;
    }
    
    ((uint8_t*)tx_data)[0] = DRIVE_MODBUS_CODE_READ_OSC_STATUS;
    ((uint8_t*)tx_data)[1] = (res != DRIVE_MODBUS_ASYNC_OP_ERROR) ? res
                              : res + pvoid_to_int(err_t, future_result(&drive_modbus.osc_future));
    *tx_size = 2;
    
    return MODBUS_RTU_ERROR_NONE;
}

modbus_rtu_error_t drive_modbus_osc_access_get_osc(const void* rx_data, size_t rx_size, void* tx_data, size_t* tx_size)
{
    if(rx_size != 4) return MODBUS_RTU_ERROR_INVALID_DATA;
    
    uint8_t* paddr = (uint8_t*)rx_data + 1;
    size_t address = ((size_t)paddr[0] << 8) | paddr[1];
    //size_t address = (size_t)ntohs(*(uint16_t*)((uint8_t*)rx_data + 1));
    size_t size = (size_t)((uint8_t*)rx_data)[3];
    
    if(address >= (DRIVE_POWER_OSC_CHANNEL_SIZE)) return MODBUS_RTU_ERROR_INVALID_ADDRESS;
    if(size > (MODBUS_RTU_DATA_SIZE_MAX - 2)) return MODBUS_RTU_ERROR_INVALID_DATA;
    
    ((uint8_t*)tx_data)[0] = DRIVE_MODBUS_CODE_GET_READED_OSC;
    ((uint8_t*)tx_data)[1] = size;
    memcpy(&((uint8_t*)tx_data)[2], (const uint8_t*)drive_events_readed_osc_data() + address, size);
    *tx_size = size + 2;
    
    return MODBUS_RTU_ERROR_NONE;
}

modbus_rtu_error_t drive_modbus_osc_access(const void* rx_data, size_t rx_size, void* tx_data, size_t* tx_size)
{
    if(rx_size == 0) return MODBUS_RTU_ERROR_INVALID_DATA;
    
    uint8_t cmd = *(uint8_t*)rx_data;
    
    switch(cmd){
        default:
            return MODBUS_RTU_ERROR_INVALID_DATA;
        case DRIVE_MODBUS_CODE_OSC_COUNT:
            return drive_modbus_osc_access_count(tx_data, tx_size);
        case DRIVE_MODBUS_CODE_OSC_EVENT_ID:
            return drive_modbus_osc_access_event_id(rx_data, rx_size, tx_data, tx_size);
        case DRIVE_MODBUS_CODE_READ_OSC:
            return drive_modbus_osc_access_read_osc(rx_data, rx_size, tx_data, tx_size);
        case DRIVE_MODBUS_CODE_READ_OSC_STATUS:
            return drive_modbus_osc_access_status(tx_data, tx_size);
        case DRIVE_MODBUS_CODE_GET_READED_OSC:
            return drive_modbus_osc_access_get_osc(rx_data, rx_size, tx_data, tx_size);
    }
    
    return MODBUS_RTU_ERROR_NONE;
}

modbus_rtu_error_t drive_modbus_on_custom_func(modbus_rtu_func_t func, const void* rx_data, size_t rx_size, void* tx_data, size_t* tx_size)
{
    switch(func){
        case DRIVE_MODBUS_FUNC_EVENTS_ACCESS:
            return drive_modbus_events_access(rx_data, rx_size, tx_data, tx_size);
        case DRIVE_MODBUS_FUNC_OSC_ACCESS:
            return drive_modbus_osc_access(rx_data, rx_size, tx_data, tx_size);
        default:
            return MODBUS_RTU_ERROR_FUNC;
    }
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
    modbus_rtu_set_custom_function_callback(drive_modbus.modbus, drive_modbus_on_custom_func);
    
    return E_NO_ERROR;
}
