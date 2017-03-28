#ifndef STORAGE_H
#define STORAGE_H

#include <stddef.h>
#include "errors/errors.h"
#include "m95x/m95x.h"



// Адреса и размеры регионов.
//! Адрес региона настроек.
#define STORAGE_RGN_SETTINGS_ADDRESS 0x0
//! Размер региона настроек.
#define STORAGE_RGN_SETTINGS_SIZE 0x400

//! Адрес региона.
#define STORAGE_RGN_EVMAP_ADDRESS 0x400
//! Размер региона.
#define STORAGE_RGN_EVMAP_SIZE 0x40

//! Адрес региона.
#define STORAGE_RGN_EVENTS_ADDRESS 0x440
//! Размер региона.
#define STORAGE_RGN_EVENTS_SIZE 0xbc0

//! Адрес региона.
#define STORAGE_RGN_OSC_ADDRESS 0x1000
//! Размер региона.
#define STORAGE_RGN_OSC_SIZE 0xf000//0x3000

/*
//! Адрес региона.
#define STORAGE_RGN__ADDRESS 0x
//! Размер региона.
#define STORAGE_RGN__SIZE 0x
*/


//! Тип адреса хранилища данных.
typedef m95x_address_t storage_address_t;


/**
 * Инициализирует хранилище данных.
 * @param eeprom Память EEPROM для хранения данных.
 * @return Код ошибки.
 */
extern err_t storage_init(m95x_t* eeprom);

/**
 * Читает данные из хранилища данных.
 * @param address Адрес.
 * @param data Буфер для чтения.
 * @param size Размер.
 * @return Код ошибки.
 */
extern err_t storage_read(storage_address_t address, void* data, size_t size);

/**
 * Записывает данные в хранилище данных.
 * @param address Адрес.
 * @param data Данные.
 * @param size Размер.
 * @return Код ошибки.
 */
extern err_t storage_write(storage_address_t address, const void* data, size_t size);


#endif /* STORAGE_H */
