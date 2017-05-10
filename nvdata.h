/**
 * @file nvdata.h Библиотека доступа к бэкапным регистрам.
 */

#ifndef NVDATA_H
#define NVDATA_H

#include <stddef.h>
#include <stdint.h>
#include "errors/errors.h"

#if defined(STM32F10X_LD) || defined(STM32F10X_LD_VL) ||\
    defined(STM32F10X_MD) || defined(STM32F10X_MD_VL)

#define NVDATA_SIZE 20

#elif defined(STM32F10X_HD) || defined(STM32F10X_HD_VL) ||\
      defined(STM32F10X_XL) || defined(STM32F10X_CL)

#define NVDATA_SIZE 84

#else

#error "Target STM32F10x device not defined!"

#endif


/**
 * Сбрасывает энергонезависимый блок данных.
 */
extern void nvdata_clear(void);

/**
 * Читает байт из энергонезависимого блока данных.
 * @param address Адрес.
 * @param data Данные.
 * @return Код ошибки.
 */
extern err_t nvdata_get_byte(size_t address, uint8_t* data);

/**
 * Записывает байт в энергонезависимый блок данных.
 * @param address Адрес.
 * @param data Данные.
 * @return Код ошибки.
 */
extern err_t nvdata_put_byte(size_t address, uint8_t data);

/**
 * Читает полуслово из энергонезависимого блока данных.
 * @param address Адрес.
 * @param data Данные.
 * @return Код ошибки.
 */
extern err_t nvdata_get_half_word(size_t address, uint16_t* data);

/**
 * Записывает полуслово в энергонезависимый блок данных.
 * @param address Адрес.
 * @param data Данные.
 * @return Код ошибки.
 */
extern err_t nvdata_put_half_word(size_t address, uint16_t data);

/**
 * Читает слово из энергонезависимого блока данных.
 * @param address Адрес.
 * @param data Данные.
 * @return Код ошибки.
 */
extern err_t nvdata_get_word(size_t address, uint32_t* data);

/**
 * Записывает слово в энергонезависимый блок данных.
 * @param address Адрес.
 * @param data Данные.
 * @return Код ошибки.
 */
extern err_t nvdata_put_word(size_t address, uint32_t data);

#endif /* NVDATA_H */

