/**
 * @file triac_pair.h Библиотека работы с парой пара тиристоровов.
 */

#ifndef TRIAC_PAIR_H
#define	TRIAC_PAIR_H

#include "triac.h"
#include "errors/errors.h"
#include "defs/defs.h"

//! Тип пары тиристоров.
typedef struct _Triac_Pair {
    triac_t triac_a; //!< Один тиристор.
    triac_t triac_b; //!< Второй тиристор.
} triac_pair_t;

//! Инициализирует структуру пары тиристоров по месту объявления.
#define MAKE_TRIAC_PAIR(arg_gpio_a, arg_pin_a, arg_gpio_b, arg_pin_b)\
        { .triac_a = MAKE_TRIAC(arg_gpio_a, arg_pin_a), .triac_b = MAKE_TRIAC(arg_gpio_b, arg_pin_b) }

/**
 * Инициализирует пару тиристоров.
 * Не инициализирует порт.
 * @param triac_pair Пара тиристоров.
 * @param GPIO_a Порт тиристора A.
 * @param pin_a Маска пина тиристора A, только один пин.
 * @param GPIO_b Порт тиристора B.
 * @param pin_b Маска пина тиристора B, только один пин.
 * @return Код ошибки.
 */
extern err_t triac_pair_init(triac_pair_t* triac_pair, GPIO_TypeDef* GPIO_a, uint16_t pin_a,
                                                         GPIO_TypeDef* GPIO_b, uint16_t pin_b);

/**
 * Инициализирует пин пары тиристоров.
 * @param triac_pair Пара тиристоров.
 * @param mode Режим пина.
 * @param speed Скорость пина.
 * @return Код ошибки.
 */
extern err_t triac_pair_init_gpio(triac_pair_t* triac_pair, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed);

/**
 * Открывает пару тиристоров.
 * @param triac_pair Пара тиристоров.
 */
ALWAYS_INLINE static void triac_pair_open(triac_pair_t* triac_pair)
{
    triac_open(&triac_pair->triac_a);
    triac_open(&triac_pair->triac_b);
}

/**
 * Закрывает пару тиристоров.
 * @param triac_pair Пара тиристоров.
 */
ALWAYS_INLINE static void triac_pair_close(triac_pair_t* triac_pair)
{
    triac_close(&triac_pair->triac_a);
    triac_close(&triac_pair->triac_b);
}

#endif	/* TRIAC_PAIR_H */

