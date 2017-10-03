/**
 * @file channel_filter.h Библиотека фильтра канала измерения.
 */

#ifndef CHANNEL_FILTER_H
#define CHANNEL_FILTER_H

#include <stdint.h>
#include <stddef.h>
#include "fixed/fixed32.h"
#include "defs/defs.h"


//! Размер фильтра канала измерения по-умолчанию.
#define CHANNEL_FILTER_DEFAULT_SIZE 1 // 6.67 мс.

//! Время одного элемента фильтра.
#define CHANNEL_FILTER_TIME_PER_ITEM_US 6667

//! Фильтр значений каналов АЦП.
typedef struct _Channel_Filter {
    fixed32_t* buffer; //!< Буфер значений канала измерения.
    uint16_t capacity; //!< Ёмкость буфера.
    uint16_t size;  //!< Размер фильтра.
    uint16_t count; //!< Количество значений канала измерения.
    uint16_t index; //!< Индекс текущего элемента в фильтре.
    fixed32_t value; //!< Вычисленное значение фильтра.
} channel_filter_t;


/**
 * Инициализирует фильтр.
 * @param filter Фильтр.
 * @param buffer Буфер фильтра.
 * @param capacity Размер буфера.
 * @param size Начальный размер фильтра.
 */
EXTERN void channel_filter_init(channel_filter_t* filter, fixed32_t* buffer, size_t capacity, size_t size);

/**
 * Сбрасывает фильтр.
 * @param filter Фильтр.
 */
EXTERN void channel_filter_reset(channel_filter_t* filter);

/**
 * Изменяет размер фильтра.
 * @param filter Фильтр.
 * @param size Размер фильтра.
 */
EXTERN void channel_filter_resize(channel_filter_t* filter, size_t size);

/**
 * Изменяет размер фильтра по времени.
 * Устанавливает ближайшее округлённое значение длины фильтра в элементах.
 * Дискретизация - 6.67 мс.
 * @param filter Фильтр.
 * @param size Размер фильтра в миллисекундах.
 */
EXTERN void channel_filter_resize_ms(channel_filter_t* filter, uint32_t time_ms);

/**
 * Добавляет значение в буфер фильтра.
 * @param filter Фильтр.
 * @param value Значение.
 */
EXTERN void channel_filter_put(channel_filter_t* filter, fixed32_t value);

/**
 * Вычисляет значение фильтра.
 * @param filter Фильтр.
 */
EXTERN void channel_filter_calculate(channel_filter_t* filter);

/**
 * Получает вычисленное значение фильтра.
 * @param filter Фильтр.
 * @return Отфильтрованное значение.
 */
ALWAYS_INLINE static fixed32_t channel_filter_value(channel_filter_t* filter)
{
    return filter->value;
}

#endif /* CHANNEL_FILTER_H */
