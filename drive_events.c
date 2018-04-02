#include "drive_events.h"
#include <string.h>
#include "crc/crc16_ccitt.h"
#include "utils/utils.h"
#include <time.h>
#include "storage.h"
#ifdef USE_ZERO_SENSORS
#include "drive_phase_state.h"
#else
#include "drive_phase_sync.h"
#endif


//! Отладочный флаг записи в осциллограмму данных самонастройки.
//#define DRIVE_EVENTS_OSC_SELFTUNE

#ifdef DRIVE_EVENTS_OSC_SELFTUNE
#include "drive_selftuning.h"
#endif


//! Размер события в хранилище.
#define DRIVE_EVENT_SIZE 64
//! Число событий в хранилище.
#define DRIVE_EVENTS_COUNT_MAX (STORAGE_RGN_EVENTS_SIZE / DRIVE_EVENT_SIZE)

//! Размер осциллограммы в хранилище.
#define DRIVE_OSC_SIZE 4096
//! Число осциллограмм в хранилище.
#define DRIVE_OSCS_COUNT_MAX (STORAGE_RGN_OSC_SIZE / DRIVE_OSC_SIZE)

#pragma pack(push, 1)
//! Тип карты событий.
typedef struct _Drive_Events_Map {
    //! Число событий.
    drive_event_index_t events_count;
    //! Индекс последнего события.
    drive_event_index_t event_index;
    //! Идентификатор последнего события.
    drive_event_id_t event_id;
    //! Число осциллограмм.
    drive_event_index_t osc_count;
    //! Индекс следующей осциллограммы.
    drive_event_index_t osc_index;
    //! Принадлежность осциллограмм событиям.
    drive_event_id_t osc_event_ids[DRIVE_OSCS_COUNT_MAX];
    //! Контрольная сумма.
    uint16_t crc;
} drive_events_map_t;
#pragma pack(pop)


//! Тип событий привода.
typedef struct _Drive_Events {
    drive_events_map_t events_map; //!< Карта событий.
    drive_power_osc_channel_t osc_buf; //!< Буфер для чтения/записи осциллограмм.
} drive_events_t;

static drive_events_t events;


err_t drive_events_init(void)
{
    memset(&events, 0x0, sizeof(drive_events_t));
    
    return E_NO_ERROR;
}

void drive_events_reset(void)
{
    memset(&events.events_map, 0x0, sizeof(drive_events_map_t));
}

err_t drive_events_read(void)
{
     RETURN_ERR_IF_FAIL(
                storage_read(STORAGE_RGN_EVMAP_ADDRESS, &events.events_map,
                             sizeof(drive_events_map_t))
             );
     uint16_t crc = crc16_ccitt(&events.events_map,
             sizeof(drive_events_map_t) - sizeof(uint16_t));
     
     if(crc != events.events_map.crc) return E_CRC;
     
     return E_NO_ERROR;
}

err_t drive_events_write(void)
{
    events.events_map.crc = crc16_ccitt(&events.events_map,
            sizeof(drive_events_map_t) - sizeof(uint16_t));
    
    return storage_write(STORAGE_RGN_EVMAP_ADDRESS, &events.events_map,
                         sizeof(drive_events_map_t));
}

void drive_events_make_event(drive_event_t* event, drive_event_type_t type)
{
    event->id = 0;
    event->type = (uint8_t)type;
    event->state = (uint8_t)drive_state();
    event->direction = (uint8_t)drive_direction();
    event->init_state = (uint8_t)drive_init_state();
    event->calibration_state = (uint8_t)drive_power_calibration();
    event->starting_state = (uint8_t)drive_starting();
    event->stopping_state = (uint8_t)drive_stopping();
    event->err_stopping_state = (uint8_t)drive_err_stopping();
    event->reference = fixed32_get_int(drive_reference());
    event->flags = drive_flags();
    event->warnings = drive_warnings();
    event->errors = drive_errors();
    event->power_warnings = drive_power_warnings();
    event->power_errors = drive_power_errors();
    event->phase_errors = drive_phase_errors();
#ifdef USE_ZERO_SENSORS
    event->phase_a_time = drive_phase_state_phase_time(PHASE_A);
    event->phase_b_time = drive_phase_state_phase_time(PHASE_B);
    event->phase_c_time = drive_phase_state_phase_time(PHASE_C);
#else
    fixed32_t tmp_val = 0;
    drive_phase_sync_angle(PHASE_A, &tmp_val); event->phase_a_angle = fixed32_get_int(tmp_val);//
    drive_phase_sync_angle(PHASE_B, &tmp_val); event->phase_b_angle = fixed32_get_int(tmp_val);//
    drive_phase_sync_angle(PHASE_C, &tmp_val); event->phase_c_angle = fixed32_get_int(tmp_val);//
#endif //USE_ZERO_SENSORS
    event->time = (uint32_t)time(NULL);
    event->crc = 0;
}

size_t drive_events_count(void)
{
    return (size_t)events.events_map.events_count;
}

drive_event_index_t drive_events_index_by_number(size_t number)
{
    size_t count = drive_events_count();
    
    if(number >= count) return drive_events_last_index();
    
    number += drive_events_first_index();
    
    if(number >= count) number -= count;
    
    return (drive_event_index_t)number;
}

drive_event_index_t drive_events_next_index(drive_event_index_t index)
{
    if(++ index >= DRIVE_EVENTS_COUNT_MAX) return 0;
    return index;
}

drive_event_index_t drive_events_first_index(void)
{
    if(events.events_map.events_count < DRIVE_EVENTS_COUNT_MAX)
        return 0;
    
    return drive_events_next_index(events.events_map.event_index);
}

drive_event_index_t drive_events_last_index(void)
{
    return events.events_map.event_index;
}

static storage_address_t drive_events_get_address(drive_event_index_t event_index)
{
    return STORAGE_RGN_EVENTS_ADDRESS + (uint32_t)event_index * DRIVE_EVENT_SIZE;
}

err_t drive_events_write_event(drive_event_t* event)
{
    drive_event_index_t event_index = 0;
    if(events.events_map.events_count != 0){
        event_index = drive_events_next_index(events.events_map.event_index);
    }
    
    event->id = events.events_map.event_id + 1;
    event->crc = crc16_ccitt(event, sizeof(drive_event_t) - sizeof(uint16_t));
    
    storage_address_t event_address = drive_events_get_address(event_index);
    
    err_t err = E_NO_ERROR;
    
    err = storage_write(event_address, event, sizeof(drive_event_t));
    if(err != E_NO_ERROR) return err;
    
    events.events_map.event_id ++;
    events.events_map.event_index = event_index;
    if(events.events_map.events_count < DRIVE_EVENTS_COUNT_MAX){
        events.events_map.events_count ++;
    }
    
    return E_NO_ERROR;
}

err_t drive_events_read_event(drive_event_t* event, drive_event_index_t index)
{
    storage_address_t event_address = drive_events_get_address(index);
    
    RETURN_ERR_IF_FAIL(storage_read(event_address, event, sizeof(drive_event_t)));
    
    uint16_t crc = crc16_ccitt(event, sizeof(drive_event_t) - sizeof(uint16_t));
    
    if(crc != event->crc) return E_CRC;
    
    return E_NO_ERROR;
}

size_t drive_events_oscillograms_count(void)
{
    return (size_t)events.events_map.osc_count;
}

drive_osc_index_t drive_events_osc_index_by_number(size_t number)
{
    size_t count = drive_events_oscillograms_count();
    
    if(number >= count) return drive_events_oscillograms_last_index();
    
    number += drive_events_oscillograms_first_index();
    
    if(number >= count) number -= count;
    
    return (drive_osc_index_t)number;
}

drive_osc_index_t drive_events_oscillograms_next_index(drive_osc_index_t index)
{
    if(++ index >= DRIVE_OSCS_COUNT_MAX) return 0;
    return index;
}

drive_osc_index_t drive_events_oscillograms_first_index(void)
{
    if(events.events_map.osc_count < DRIVE_OSCS_COUNT_MAX)
        return 0;
    
    return drive_events_oscillograms_next_index(events.events_map.osc_index);
}

drive_osc_index_t drive_events_oscillograms_last_index(void)
{
    return events.events_map.osc_index;
}

drive_event_id_t drive_events_osc_event_id(drive_osc_index_t index)
{
    if(index >= DRIVE_OSCS_COUNT_MAX) index = drive_events_oscillograms_last_index();
    return events.events_map.osc_event_ids[index];
}

static storage_address_t drive_events_get_osc_channel_address(drive_osc_index_t osc_index, size_t osc_ch_index)
{
    return STORAGE_RGN_OSC_ADDRESS + osc_index * DRIVE_OSC_SIZE  + (uint32_t)osc_ch_index * sizeof(drive_power_osc_channel_t);
}

static err_t drive_events_write_osc_channel_impl(drive_osc_index_t osc_index, size_t osc_ch_index, drive_power_osc_channel_t* osc)
{
    storage_address_t address = drive_events_get_osc_channel_address(osc_index, osc_ch_index);
    return storage_write(address, osc, sizeof(drive_power_osc_channel_t));
}

#ifdef DRIVE_EVENTS_OSC_SELFTUNE
static void drive_events_put_tuning_urot_data(void)
{
    drive_selftuning_data_iter_t iter = drive_selftuning_data_iter_begin();
    size_t i;
    osc_value_t* value;
    fixed32_t f;
    for(i = 0; i < DRIVE_POWER_OSC_CHANNEL_LEN; i ++){
        value = &events.osc_buf.data[i];
        if(!drive_selftuning_data_iter_at_end(iter)){
            f = drive_selftuning_data_iter_urot(iter);
            *value = drive_power_osc_value_from_fixed32(f);
            iter = drive_selftuning_data_iter_next(iter);
        }else{
            *value = 0;
        }
    }
}

static void drive_events_put_tuning_irot_data(void)
{
    drive_selftuning_data_iter_t iter = drive_selftuning_data_iter_begin();
    size_t i;
    osc_value_t* value;
    fixed32_t f;
    for(i = 0; i < DRIVE_POWER_OSC_CHANNEL_LEN; i ++){
        value = &events.osc_buf.data[i];
        if(!drive_selftuning_data_iter_at_end(iter)){
            f = drive_selftuning_data_iter_irot(iter);
            *value = drive_power_osc_value_from_fixed32(f);
            iter = drive_selftuning_data_iter_next(iter);
        }else{
            *value = 0;
        }
    }
}

static void drive_events_put_tuning_time_data(void)
{
    drive_selftuning_data_iter_t iter = drive_selftuning_data_iter_begin();
    size_t i;
    osc_value_t* value;
    fixed32_t f;
    for(i = 0; i < DRIVE_POWER_OSC_CHANNEL_LEN; i ++){
        value = &events.osc_buf.data[i];
        if(!drive_selftuning_data_iter_at_end(iter)){
            f = drive_selftuning_data_iter_time(iter);
            *value = drive_power_osc_value_from_fixed32(f*100);
            iter = drive_selftuning_data_iter_next(iter);
        }else{
            *value = 0;
        }
    }
}

static void drive_events_put_tuning_di_data(void)
{
    drive_selftuning_data_iter_t iter = drive_selftuning_data_iter_begin();
    size_t i;
    osc_value_t* value;
    fixed32_t f;
    for(i = 0; i < DRIVE_POWER_OSC_CHANNEL_LEN; i ++){
        value = &events.osc_buf.data[i];
        if(!drive_selftuning_data_iter_at_end(iter)){
            f = drive_selftuning_data_iter_di(iter);
            *value = drive_power_osc_value_from_fixed32(f);
            iter = drive_selftuning_data_iter_next(iter);
        }else{
            *value = 0;
        }
    }
}
#endif //DRIVE_EVENTS_OSC_SELFTUNE

err_t drive_events_write_current_oscillogram(drive_event_id_t event_id)
{
    size_t osc_ch_index = 0;
    
    drive_osc_index_t osc_index = 0;
    if(events.events_map.osc_count != 0){
        osc_index = drive_events_oscillograms_next_index(events.events_map.osc_index);
    }
    
    for(; osc_ch_index < DRIVE_POWER_OSC_CHANNELS_COUNT; osc_ch_index ++){
        
        switch(osc_ch_index){
#ifdef DRIVE_EVENTS_OSC_SELFTUNE
            case DRIVE_POWER_OSC_CHANNEL_Urot:
                drive_events_put_tuning_urot_data();
                break;
            case DRIVE_POWER_OSC_CHANNEL_Irot:
                drive_events_put_tuning_irot_data();
                break;
            case DRIVE_POWER_OSC_CHANNEL_Iexc:
                drive_events_put_tuning_time_data();
                break;
            case DRIVE_POWER_OSC_CHANNEL_Ib:
                drive_events_put_tuning_di_data();
                break;
#endif //DRIVE_EVENTS_OSC_SELFTUNE
            default:
                RETURN_ERR_IF_FAIL(drive_power_osc_channel_get(osc_ch_index, &events.osc_buf));
                break;
        }
        RETURN_ERR_IF_FAIL(drive_events_write_osc_channel_impl(osc_index, osc_ch_index, &events.osc_buf));
    }
    
    if(events.events_map.osc_count < DRIVE_OSCS_COUNT_MAX){
        events.events_map.osc_count ++;
    }
    
    events.events_map.osc_event_ids[osc_index] = event_id;
    events.events_map.osc_index = osc_index;
    
    return E_NO_ERROR;
}

err_t drive_events_read_osc_channel(drive_osc_index_t index, size_t osc_channel)
{
    if(index >= events.events_map.osc_count) return E_OUT_OF_RANGE;
    storage_address_t address = drive_events_get_osc_channel_address(index, osc_channel);
    return storage_read(address, &events.osc_buf, sizeof(drive_power_osc_channel_t));
}

const osc_value_t* drive_events_readed_osc_data(void)
{
    return events.osc_buf.data;
}
