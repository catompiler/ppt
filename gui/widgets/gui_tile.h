/**
 * @file gui_tile.h Реализация виджета плитки графического интерфейса.
 */

#ifndef GUI_TILE_H
#define	GUI_TILE_H

#include "gui/gui_metro.h"
#include "gui/gui_widget.h"
#include "errors/errors.h"
#include "defs/defs.h"

#define GUI_TILE_VALUE_FONT_WIDTH 24
#define GUI_TILE_VALUE_FONT_SPACE 2

//! Статус плитки.
typedef enum _Gui_Tile_Status {
    GUI_TILE_STATUS_OK = 0, //!< Нормальный режим.
    GUI_TILE_STATUS_WARNING, //!< Предупреждение.
    GUI_TILE_STATUS_ALARM //!< Авария.
} gui_tile_status_t;

typedef struct _Gui_Tile gui_tile_t;

//! Зеначение идентификатора типа плитки.
#define GUI_TILE_TYPE_ID 102

struct _Gui_Tile {
    gui_widget_t super; //!< Суперкласс.
    const char* caption; //!< Подпись.
    const char* value; //!< Отображаемое значение.
    const char* unit; //!< Единица измерения.
    const char* errors; //!< Сообщения об ошибках.
    gui_tile_status_t status; //!< Статус плитки.
    graphics_color_t status_color; //!< Цвет фона плитки
};

//! Приводит указатель tile к типу плитки.
#define GUI_TILE(tile) ((gui_tile_t*)(tile))

/**
 * Инициализирует плитку.
 * @param tile Плитка.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
EXTERN err_t gui_tile_init(gui_tile_t* tile, gui_metro_t* gui);

/**
 * Инициализирует плитку как потомок заданного родителя.
 * @param tile Плитка.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
EXTERN err_t gui_tile_init_parent(gui_tile_t* tile, gui_metro_t* gui, gui_widget_t* parent);

/**
 * Получает текст подписи плитки.
 * @param tile Плитка.
 * @return Текст подписи плитки.
 */
ALWAYS_INLINE static const char*  gui_tile_caption(gui_tile_t* tile)
{
    return tile->caption;
}

/**
 * Устанавливает текст подписи плитки.
 * @param tile Плитка.
 * @param caption Текст подписи плитки.
 */
EXTERN void gui_tile_set_caption(gui_tile_t* tile, const char*  caption);

/**
 * Получает статус плитки.
 * @param tile Плитка.
 * @return Статус плитки.
 */
ALWAYS_INLINE static const gui_tile_status_t gui_tile_status(gui_tile_t* tile)
{
    return tile->status;
}

/**
 * Устанавливает cтатус плитки.
 * @param tile Плитка.
 * @param status Статус плитки.
 */
EXTERN void gui_tile_set_status(gui_tile_t* tile, const gui_tile_status_t status);


/**
 * Получает единицу измерения плитки.
 * @param tile Плитка.
 * @return Единица измерения плитки.
 */
ALWAYS_INLINE static const char* gui_tile_unit(gui_tile_t* tile)
{
    return tile->unit;
}

/**
 * Устанавливает единицу измерения плитки.
 * @param tile Плитка.
 * @param unit Единица измерения плитки.
 */
EXTERN void gui_tile_set_unit(gui_tile_t* tile, const char* unit);

/**
 * Получает значение плитки.
 * @param tile Плитка.
 * @return Значение плитки.
 */
ALWAYS_INLINE static const char* gui_tile_value(gui_tile_t* tile)
{
    return tile->value;
}

/**
 * Устанавливает цвет фона плитки.
 * @param tile Плитка.
 * @param color Цвет фона плитки.
 */
EXTERN void gui_tile_set_value(gui_tile_t* tile, const char* value);

EXTERN void gui_tile_set_errors(gui_tile_t* tile, const char* errors);

/**
 * Обработчик перерисовки.
 * @param tile Плитка.
 * @param rect Область перерисовки.
 */
EXTERN void gui_tile_on_repaint(gui_tile_t* tile, const rect_t* rect);

/**
 * Обработчик перерисовки значения.
 * @param tile Плитка.
 */
EXTERN void gui_tile_repaint_value(gui_tile_t* tile, const rect_t* rect);

size_t gui_tile_draw_value_string(painter_t* painter, graphics_pos_t x, graphics_pos_t y, const char* s);

#endif	/* GUI_TILE_H */
