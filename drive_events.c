#include "drive_events.h"
#include <string.h>
#include "crc/crc16_ccitt.h"
#include "utils/utils.h"
#include "storage.h"


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
    drive_event_id_t osc_event_ids[DRIVE_OSCILLOGRAMS_COUNT_MAX];
    //! Контрольная сумма.
    uint16_t crc;
} drive_events_map_t;
#pragma pack(pop)


//! Тип событий привода.
typedef struct _Drive_Events {
    drive_events_map_t events_map; //!< Карта событий.
    drive_power_oscillogram_t osc; //!< Буфер для чтения/записи осциллограмм.
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
    event->calibration_state = (uint8_t)drive_power_calibration();
    event->starting_state = (uint8_t)drive_stopping();
    event->stopping_state = (uint8_t)drive_starting();
    event->err_stopping_state = (uint8_t)drive_err_stopping();
    event->reference = drive_reference();
    event->flags = drive_flags();
    event->warnings = drive_warnings();
    event->errors = drive_errors();
    event->power_warnings = drive_power_warnings();
    event->power_errors = drive_power_errors();
    event->phase_errors = drive_phase_errors();
    event->time = 0;
    event->crc = 0;
}

size_t drive_events_count(void)
{
    return (size_t)events.events_map.events_count;
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

drive_event_index_t drive_events_oscillograms_next_index(drive_event_index_t index)
{
    if(++ index >= DRIVE_OSCILLOGRAMS_COUNT_MAX) return 0;
    return index;
}

drive_event_index_t drive_events_oscillograms_first_index(void)
{
    if(events.events_map.osc_count < DRIVE_EVENTS_COUNT_MAX)
        return 0;
    
    return drive_events_oscillograms_next_index(events.events_map.osc_index);
}

drive_event_index_t drive_events_oscillograms_last_index(void)
{
    return events.events_map.osc_index;
}

static storage_address_t drive_events_get_osc_address(drive_event_index_t osc_index, size_t osc_ch_index)
{
    return STORAGE_RGN_OSC_ADDRESS + osc_index * DRIVE_OSC_SIZE  + (uint32_t)osc_ch_index * sizeof(drive_power_oscillogram_t);
}

err_t drive_events_write_osc_impl(size_t osc_ch_index, drive_power_oscillogram_t* osc)
{
    storage_address_t address = drive_events_get_osc_address(events.events_map.osc_index, osc_ch_index);
    return storage_write(address, osc, sizeof(drive_power_oscillogram_t));
}

err_t drive_events_write_current_oscillograms(drive_event_id_t event_id)
{
    size_t osc_ch_index = 0;
    
    for(; osc_ch_index < DRIVE_POWER_OSC_COUNT; osc_ch_index ++){
        RETURN_ERR_IF_FAIL(drive_power_oscillogram_get(osc_ch_index, &events.osc));
        RETURN_ERR_IF_FAIL(drive_events_write_osc_impl(osc_ch_index, &events.osc));
    }
    
    drive_event_index_t osc_index = 0;
    if(events.events_map.osc_count != 0){
        osc_index = drive_events_oscillograms_next_index(events.events_map.osc_index);
    }
    
    events.events_map.osc_event_ids[osc_index] = event_id;
    events.events_map.osc_index = osc_index;
    
    return E_NO_ERROR;
}
