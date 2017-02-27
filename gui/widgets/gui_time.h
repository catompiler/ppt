/**
 * @file gui_time.h Реализация виджета отображения текущего времени
 */

#ifndef GUI_TIME_H
#define	GUI_TIME_H

#include "../gui_metro.h"
#include "../gui_widget.h"
#include "errors/errors.h"
#include "defs/defs.h"

#define GUI_TIME_STR_MAX_LEN 6
#define GUI_TIME_STR_FORMAT "%02d:%02d"

typedef struct _Gui_Time gui_time_t;

//! Зеначение идентификатора типа плитки.
#define GUI_TIME_TYPE_ID 103

struct _Gui_Time {
    gui_widget_t super; //!< Суперкласс.
    graphics_color_t back_color; //!< Цвет фона плитки
    graphics_color_t font_color; //!< Цвет шрифта плитки
};

//! Приводит указатель time к типу плитки.
#define GUI_TIME(time) ((gui_time_t*)(time))

/**
 * Инициализирует плитку.
 * @param time Плитка.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
EXTERN err_t gui_time_init(gui_time_t* time, gui_metro_t* gui);

/**
 * Инициализирует плитку как потомок заданного родителя.
 * @param time Плитка.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
EXTERN err_t gui_time_init_parent(gui_time_t* time, gui_metro_t* gui, gui_widget_t* parent);

/**
 * Обработчик перерисовки.
 * @param time Плитка.
 * @param rect Область перерисовки.
 */
EXTERN void gui_time_on_repaint(gui_time_t* time, const rect_t* rect);

/**
 * Обработчик перерисовки значения.
 * @param time Плитка.
 */
EXTERN void gui_time_repaint_value(gui_time_t* time, const rect_t* rect);

#endif	/* GUI_TIME_H */
