/**
 * @file drive_modbus.h Реализация взаимодействия с приводом по протоколу Modbus RTU.
 */

#ifndef DRIVE_MODBUS_H
#define DRIVE_MODBUS_H

#include "errors/errors.h"
#include "modbus/modbus_rtu.h"


/**
 * Инициализирует интерфейс Modbus привода.
 * @param modbus Протокол Modbus.
 * @return Код ошибки.
 */
extern err_t drive_modbus_init(modbus_rtu_t* modbus);

#endif /* DRIVE_MODBUS_H */

