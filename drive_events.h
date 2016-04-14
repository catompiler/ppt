/**
 * @file drive_events.h Библиотека событий привода.
 */

#ifndef DRIVE_EVENTS_H
#define DRIVE_EVENTS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <time.h>
#include "errors/errors.h"
#include "drive.h"


//! Максимальное число осциллограмм.
#define DRIVE_OSCILLOGRAMS_COUNT_MAX 3

//! Тип идентификатора события.
typedef uint8_t drive_event_id_t;

//! Тип индекса события.
typedef uint8_t drive_event_index_t;

//! Тип события.
typedef enum _Drive_Event_Type {
    DRIVE_EVENT_TYPE_ERROR = 0 //!< Ошибка.
} drive_event_type_t;

#pragma pack(push, 1)
//! Тип события.
typedef struct _Drive_Event {
    drive_event_id_t id; //!< Идентификатор события.
    uint8_t type; //!< Тип события.
    uint8_t state; //!< Состояние привода.
    uint8_t direction; //!< Направление вращения привода.
    uint8_t calibration_state; //!< Состояние калибровки привода.
    uint8_t starting_state; //!< Состояние запуска привода.
    uint8_t stopping_state; //!< Состояние останова привода.
    uint8_t err_stopping_state; //!< Состояние останова по ошибке привода.
    reference_t reference; //!< Задание привода.
    drive_flags_t flags; //!< Флаги привода.
    drive_warnings_t warnings; //!< Предупреждения привода.
    drive_errors_t errors; //!< Ошибки привода.
    drive_power_warnings_t power_warnings; //!< Предупреждения питания привода.
    drive_power_errors_t power_errors; //!< Ошибки питания привода.
    drive_phase_errors_t phase_errors; //!< Ошибки фаз привода.
    uint32_t time; //!< Время возникновения события. 
    uint16_t crc; //!< Контрольная сумма.
} drive_event_t;
#pragma pack(pop)

/**
 * Инициализирует события привода.
 * @return Код ошибки.
 */
extern err_t drive_events_init(void);

/**
 * Сбрасывает события привода.
 */
extern void drive_events_reset(void);

/**
 * Считывает информацию о событиях из хранилища.
 * @return Код ошибки.
 */
extern err_t drive_events_read(void);

/**
 * Записывает информацию о событиях в хранилищие.
 * @return 
 */
extern err_t drive_events_write(void);

/**
 * Заполняет событие привода согласно текущему состоянию привода.
 * @param event Событие привода.
 * @param type Тип события привода.
 */
extern void drive_events_make_event(drive_event_t* event, drive_event_type_t type);

/**
 * Получает число событий.
 * @return Число событий.
 */
extern size_t drive_events_count(void);

/**
 * Получает следующий индекс события.
 * @param index Индекс события.
 * @return Следующий индекс события.
 */
extern drive_event_index_t drive_events_next_index(drive_event_index_t index);

/**
 * Получает индекс первого события.
 * @return Индекс первого события.
 */
extern drive_event_index_t drive_events_first_index(void);

/**
 * Получает индекс последнего события.
 * @return Индекс последнего события.
 */
extern drive_event_index_t drive_events_last_index(void);

/**
 * Записывает новое событие в хранилище данных.
 * Также записывает информацию о событиях.
 * @param event Новое событие.
 * @return Код ошибки.
 */
extern err_t drive_events_write_event(drive_event_t* event);

/**
 * Считывает событие из хранилища данных.
 * @param event событие.
 * @param Индекс события.
 * @return Код ошибки.
 */
extern err_t drive_events_read_event(drive_event_t* event, drive_event_index_t index);

#endif /* DRIVE_EVENTS_H */
