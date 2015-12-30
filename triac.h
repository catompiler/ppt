/**
 * @file triac.h Библиотека работы с тиристором.
 */
#ifndef TRIAC_H
#define	TRIAC_H

#include <stm32f10x.h>
#include "errors/errors.h"
#include "defs/defs.h"

//! Тип тиристора.
typedef struct _Triac {
    GPIO_TypeDef* gpio; //!< Порт.
    uint16_t pin; //!< Маска пина.
} triac_t;

//! Инициализирует структуру тиристора по месту объявления.
#define MAKE_TRIAC(arg_gpio, arg_pin) { .gpio = arg_gpio, .pin = arg_pin }

/**
 * Инициализирует тиристор.
 * Не инициализирует порт.
 * @param triac Тиристор.
 * @param GPIO Порт.
 * @param pin Маска пина, только один пин.
 * @return Код ошибки.
 */
extern err_t triac_init(triac_t* triac, GPIO_TypeDef* GPIO, uint16_t pin);

/**
 * Инициализирует пин тиристора.
 * @param triac Тиристор.
 * @param mode Режим пина.
 * @param speed Скорость пина.
 * @return Код ошибки.
 */
extern err_t triac_init_gpio(triac_t* triac, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);

/**
 * Открывает тиристор.
 * @param triac Тиристор.
 */
ALWAYS_INLINE static void triac_open(triac_t* triac)
{
    triac->gpio->BSRR |= triac->pin;
}

/**
 * Закрывает тиристор.
 * @param triac Тиристор.
 */
ALWAYS_INLINE static void triac_close(triac_t* triac)
{
    triac->gpio->BRR |= triac->pin;
}

#endif	/* TRIAC_H */

