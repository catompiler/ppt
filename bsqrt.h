/**
 * @file bsqrt.h Реализация двоичного квадратного корня.
 */

#ifndef BSQRT_H
#define BSQRT_H

#include <stdint.h>
#include "fixed/fixed32.h"

/**
 * Вычисляет квадратный корень
 * от целочисленного числа.
 * @param value Значение.
 * @return Корень.
 */
uint32_t bsqrti(uint32_t value);

/**
 * Вычисляет квадратный корень
 * от числа с фиксированной запятой.
 * @param value Значение.
 * @return Корень.
 */
fixed32_t bsqrtf(fixed32_t value);


#endif /* BSQRT_H */

