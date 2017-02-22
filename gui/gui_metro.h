/**
 * @file gui_metro.h Библиотека работы с GUI METRO.
 */

#ifndef GUI_METRO_H
#define	GUI_METRO_H

#include "defs/defs.h"
#include "graphics/graphics.h"
#include "graphics/font.h"
#include "graphics/rect.h"
#include "input/key_input.h"

//! Тип границы виджета.
typedef enum _Gui_Border {
    GUI_BORDER_NONE = 0, //!< Нет границы.
    GUI_BORDER_SOLID //!< Сплошная граница.
} gui_border_t;

//! Структура темы оформления GUI.
typedef struct _Gui_Metro_Theme {
    graphics_color_t color_back; //!< Цвет фона.
    graphics_color_t color_statusbar; //!< Цвет строки состояния.
    graphics_color_t color_statusbar_font; //!< Цвет шрифта строки состояния.
    graphics_color_t color_tile; //!< Цвет плитки.
    graphics_color_t color_tile_warning; //!< Цвет плитки (предупреждение).
    graphics_color_t color_tile_alarm; //!< Цвет плитки (авария).
    graphics_color_t color_tile_font; //!< Цвет шрифта плитки.
    graphics_color_t color_menu; //!< Цвет фона меню.
    graphics_color_t color_menu_title; //!< Цвет фона заголовка меню.
    graphics_color_t color_menu_title_font; //!< Цвет шрифта заголовка меню.
    graphics_color_t color_menu_font; //!< Цвет шрифта меню.
    graphics_color_t color_menu_active; //!< Цвет фона выбранного элемента меню.
    graphics_color_t color_menu_active_font; //!< Цвет шрифта выбранного элемента меню.
    graphics_color_t color_menu_edit; //!< Цвет фона редактируемого элемента меню.
    graphics_color_t color_menu_edit_active; //!< Цвет символа редактируемого элемента меню.
    graphics_color_t color_scroll; //!< Цвет полосы прокрутки.
    const font_t* big_font; //!< Крупный шрифт виджета.
    const font_t* middle_font; //!< Средний шрифт меню.
    const font_t* small_font; //!< Мелкий шрифт меню.
} gui_metro_theme_t;

#define MAKE_GUI_METRO_THEME(arg_color_back, arg_color_statusbar, arg_color_statusbar_font,\
                       arg_color_tile, arg_color_tile_warning, arg_color_tile_alarm,\
                       arg_color_tile_font, arg_color_menu, \
                       arg_color_menu_title, arg_color_menu_title_font, arg_color_menu_font,\
                       arg_color_menu_active, arg_color_menu_active_font, arg_color_menu_edit,\
                       arg_color_menu_edit_active, arg_color_scroll,\
                        arg_big_font, arg_middle_font, arg_small_font)\
        { .color_back = arg_color_back, .color_statusbar = arg_color_statusbar, .color_statusbar_font = arg_color_statusbar_font,\
                       .color_tile = arg_color_tile, .color_tile_warning = arg_color_tile_warning, .color_tile_alarm = arg_color_tile_alarm,\
                       .color_tile_font = arg_color_tile_font, .color_menu = arg_color_menu, .color_menu_title = arg_color_menu_title,\
                       .color_menu_title_font = arg_color_menu_title_font, .color_menu_font = arg_color_menu_font,\
                       .color_menu_active = arg_color_menu_active, .color_menu_active_font = arg_color_menu_active_font, .color_menu_edit = arg_color_menu_edit,\
                       .color_menu_edit_active = arg_color_menu_edit_active, .color_scroll = arg_color_scroll,\
                        .big_font = arg_big_font, .middle_font = arg_middle_font, .small_font = arg_small_font }

#ifndef GUI_WIDGET_TYPE_DEFINED
#define GUI_WIDGET_TYPE_DEFINED
//! Тип структуры виджета.
typedef struct _Gui_Widget gui_widget_t;
#endif //GUI_WIDGET_TYPE_DEFINED

//! Структура графического интерфейса.
typedef struct _Gui_Metro {
    graphics_t* graphics;           //!< Графический буфер.
    gui_metro_theme_t* theme;       //!< Тема оформления.
    gui_widget_t* root_widget;      //!< Корневой виджет.
    gui_widget_t* focus_widget;     //!< Виджет в фокусе.
} gui_metro_t;

#define MAKE_METRO_GUI(arg_graphics, arg_theme)\
        { .graphics = arg_graphics, .theme = arg_theme,\
          .root_widget = NULL, .focus_widget = NULL, .locale = RES_LOCALE_RU }

/**
 * Инициализирует графический интерфейс.
 * @param gui Графический интерфейс.
 * @param graphics Графический буфер.
 * @param theme Тема графического интерфейса.
 * @return Код ошибки.
 */
EXTERN err_t gui_metro_init(gui_metro_t* gui, graphics_t* graphics, gui_metro_theme_t* theme);

/**
 * Получает графический буфер.
 * @param gui Графический интерфейс.
 * @return Графический буфер.
 */
ALWAYS_INLINE static graphics_t* gui_metro_graphics(gui_metro_t* gui)
{
    return gui->graphics;
}

/**
 * Устанавливает графический буфер.
 * @param gui
 * @param graphics
 * @return Код ошибки.
 */
EXTERN err_t gui_metro_set_graphics(gui_metro_t* gui, graphics_t* graphics);

/**
 * Получает тему оформления.
 * @param gui Графический интерфейс.
 * @return Тема оформления.
 */
ALWAYS_INLINE static gui_metro_theme_t* gui_metro_theme(gui_metro_t* gui)
{
    return gui->theme;
}

/**
 * Устанавливает тему оформления.
 * @param gui
 * @param theme Тема оформления.
 * @return Код ошибки.
 */
EXTERN err_t gui_metro_set_theme(gui_metro_t* gui, gui_metro_theme_t* theme);

/**
 * Получает корневой виджет.
 * @param gui Графический интерфейс.
 * @return Корневой виджет.
 */
ALWAYS_INLINE static gui_widget_t* gui_metro_root_widget(gui_metro_t* gui)
{
    return gui->root_widget;
}

/**
 * Устанавливает корневой виджет.
 * @param gui Графический интерфейс.
 * @param widget Корневой виджет.
 * @return Код ошибки.
 */
EXTERN err_t gui_metro_set_root_widget(gui_metro_t* gui, gui_widget_t* widget);

/**
 * Устанавливает виджет в фокусе
 */
EXTERN bool gui_metro_set_focus_widget(gui_metro_t* gui, gui_widget_t* widget);

/**
 * Перерисовывает графический интерфейс.
 * @param gui Графический интерфейс.
 * @param rect Область перерисовки, может быть NULL.
 */
EXTERN void gui_metro_repaint(gui_metro_t* gui, rect_t* rect);

/**
 * Обрабатывает нажатие клавиши.
 * @param gui Графический интерфейс.
 * @param key Код клавиши.
 */
EXTERN void gui_metro_key_pressed(gui_metro_t* gui, keycode_t key);

/**
 * Обрабатывает отпускание клавиши.
 * @param gui Графический интерфейс.
 * @param key Код клавиши.
 */
EXTERN void gui_metro_key_released(gui_metro_t* gui, keycode_t key);

#endif	/* GUI_METRO_H */

