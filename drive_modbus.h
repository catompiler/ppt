/**
 * @file drive_modbus.h Реализация взаимодействия с приводом по протоколу Modbus RTU.
 */

#ifndef DRIVE_MODBUS_H
#define DRIVE_MODBUS_H

#include "errors/errors.h"
#include "modbus/modbus_rtu.h"

//! Каллбэк применения настроек.
typedef void (*apply_settings_callback_t)(void);

//! Каллбэк сохранения настроек.
typedef void (*save_settings_callback_t)(void);

typedef struct _Drive_Modbus_Init {
    apply_settings_callback_t apply_settings_callback;
    save_settings_callback_t save_settings_callback;
} drive_modbus_init_t;

/**
 * Инициализирует интерфейс Modbus привода.
 * @param modbus Протокол Modbus.
 * @return Код ошибки.
 */
extern err_t drive_modbus_init(modbus_rtu_t* modbus, drive_modbus_init_t* drive_modbus_is);

#endif /* DRIVE_MODBUS_H */

