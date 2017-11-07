/**
 * @file asin.h Библиотека функций asin и acos.
 */
#ifndef ASIN_H
#define ASIN_H

#include <stdint.h>
#include "fixed/fixed32.h"


/**
 * Вычисляет значение арксинуса.
 * @param value Значение синуса.
 * @return Угол.
 */
fixed32_t fixed32_asin(fixed32_t value);

/**
 * Вычисляет значение арккосинус.
 * @param value Значение косинуса.
 * @return Угол.
 */
fixed32_t fixed32_acos(fixed32_t value);

/**
 * Линейная интерполяция.
 * @param a Число А.
 * @param b Число Б.
 * @param t Параметр интерполяции.
 * @return Интерполированное значение.
 */
fixed32_t fixed32_lerp(fixed32_t a, fixed32_t b, fixed32_t t);

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

#endif /* ASIN_H */
