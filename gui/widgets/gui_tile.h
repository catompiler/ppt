/**
 * @file gui_tile.h Реализация виджета плитки графического интерфейса.
 */

#ifndef GUI_TILE_H
#define	GUI_TILE_H

#include "translations_ids.h"
#include "parameters_ids.h"
#include "settings.h"
#include "localization/localization.h"
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

typedef struct _Gui_Tile_Type gui_tile_type_t;

struct _Gui_Tile_Type {
    param_id_t param_id;
    param_id_t alarm_min;
    param_id_t warn_min;
    param_id_t warn_max;
    param_id_t alarm_max;
    const char* title;
};

//! Тип плитки (отображаемое значение)
#define GUI_TILE_TYPE(id, arg_title, arg_alarm_min, arg_warn_min, arg_warn_max, arg_alarm_max)\
     { .param_id = id, .title = arg_title, \
        .alarm_min = arg_alarm_min, .warn_min = arg_warn_min,\
        .warn_max = arg_warn_max, .alarm_max = arg_alarm_max }

//! Список типов плитки (отображаемых значений)
#define GUI_TILE_TYPES_COUNT 10
#define GUI_TILE_TYPES_MIN 0
#define GUI_TILE_TYPES_MAX (GUI_TILE_TYPES_COUNT - 1)
#define GUI_TILE_TYPES(arg_name, arg_count)\
        static const gui_tile_type_t arg_name[arg_count] = 

struct _Gui_Tile {
    gui_widget_t super; //!< Суперкласс.
    gui_tile_type_t type; //!< Тип плитки (отображаемое значение)
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
 * Получает статус плитки.
 * @param tile Плитка.
 * @return Статус плитки.
 */
ALWAYS_INLINE static const gui_tile_status_t gui_tile_status(gui_tile_t* tile)
{
    return tile->status;
}

/**
 * Получает тип плитки.
 * @param tile Плитка.
 * @return Тип плитки.
 */
ALWAYS_INLINE static const gui_tile_type_t gui_tile_type(gui_tile_t* tile)
{
    return tile->type;
}

/**
 * Устанавливает cтатус плитки.
 * @param tile Плитка.
 * @param status Статус плитки.
 */
EXTERN void gui_tile_set_status(gui_tile_t* tile, const gui_tile_status_t status);

/**
 * Устанавливает тип плитки.
 * @param tile Плитка.
 * @param type Статус плитки.
 */
EXTERN void gui_tile_set_type(gui_tile_t* tile, const gui_tile_type_t type);

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
