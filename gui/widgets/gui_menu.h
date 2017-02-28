/**
 * @file gui_menu.h Реализация меню графического интерфейса.
 */

#ifndef GUI_MENU_H
#define	GUI_MENU_H

#define MENU_HOME_ON_LONG_ESC_CNT 4
#define MENU_HOME_ON_TIMER_CNT 30
#define MENU_MACRO
#define MENU_DESCR_MACRO
#define MENU_DESCRS_ATTRIBS static const
#define MENU_ITEMS_ATTRIBS static

#include "menu/menu.h"

#include "gui/gui_metro.h"
#include "gui/gui_widget.h"
#include "errors/errors.h"
#include "defs/defs.h"
#include "../resources/resources_params.h"
#include "../menu/menu_explorer.h"

typedef struct _Gui_Menu gui_menu_t;

//! Значение идентификатора меню.
#define GUI_MENU_TYPE_ID 120

#define MENU_ITEM_HEIGHT (TFT_HEIGHT - GUI_STATUSBAR_HEIGHT) / (MENU_EXPLORER_DISPLAYED_COUNT + 1)
#define GUI_MENU_TEXT_LEFT_PAD 3
#define GUI_MENU_TEXT_RIGHT_PAD 4
#define GUI_MENU_ICON_LEFT_PAD 25
#define GUI_MENU_TEXT_TOP_PAD 3
#define GUI_MENU_TEXT_BOTTOM_PAD 3
#define GUI_MENU_SCROLL_WIDTH 6

struct _Gui_Menu {
    gui_widget_t super; //!< Суперкласс.
    menu_t menu; //!< Объект меню
    menu_explorer_t explorer; //!< Объект навигации по меню
    uint8_t long_esc_press_cnt; //!< Счетчик длительного нажатия на кнопку Esc
    uint8_t home_on_timer_cnt; //!< Счетчик выхода на главный экран по таймеру
    bool key_down_press; //!< Сотояние кнопки DOWN
    bool key_up_press; //!< Сотояние кнопки UP
    void (*on_home)(gui_menu_t* menu); //!< Каллбэк: выход из меню
};

#define GUI_MENU_ON_HOME_PROC(proc) ((void (*)(gui_menu_t*))proc)

typedef void (*gui_menu_on_home_t)(gui_menu_t* menu);

//! Приводит указатель menu к типу меню.
#define GUI_MENU(menu) ((gui_menu_t*)(menu))

/**
 * Инициализирует меню.
 * @param menu Меню.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
EXTERN err_t gui_menu_init(gui_menu_t* menu, gui_metro_t* gui);

/**
 * Инициализирует menu как потомок заданного родителя.
 * @param menu Меню.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
EXTERN err_t gui_menu_init_parent(gui_menu_t* menu, gui_metro_t* gui, gui_widget_t* parent);

/**
 * Обработчик перерисовки.
 * @param menu Меню.
 * @param rect Область перерисовки.
 */
EXTERN void gui_menu_on_repaint(gui_menu_t* menu, const rect_t* rect);

/**
 * Обработчик нажатия клавиши.
 * @param menu Меню.
 * @param key Код клавиши.
 */
EXTERN void gui_menu_on_key_press(gui_menu_t* menu, keycode_t key);

/**
 * Обработчик отпускания клавиши.
 * @param menu Меню.
 * @param key Код клавиши.
 */
EXTERN void gui_menu_on_key_release(gui_menu_t* menu, keycode_t key);

ALWAYS_INLINE static gui_menu_on_home_t gui_menu_on_home(gui_menu_t* menu)
{
    return menu->on_home;
}

ALWAYS_INLINE static void gui_menu_set_on_home(gui_menu_t* menu, gui_menu_on_home_t on_home)
{
    menu->on_home = on_home;
}

/**
 * Выполняет выход на главный экран
 * @param menu
 * @param key
 */
EXTERN void gui_menu_on_home_action(gui_menu_t* menu, keycode_t key);

/**
 * Выполняет выход на главный экран по таймеру
 * @param menu
 */
EXTERN void gui_menu_on_timer_home_action(gui_menu_t* menu);

EXTERN void gui_menu_init_counters(gui_menu_t* menu, gui_metro_t* gui);

int gui_menu_get_f32_fract(int number, int decimals);

void gui_menu_draw_title(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width);

void gui_menu_draw_scrollbar(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width, graphics_pos_t height);

void gui_menu_draw_item(gui_menu_t* menu, menu_item_t* menu_item, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width, graphics_pos_t height, graphics_pos_t x, graphics_pos_t y);

void gui_menu_draw_help(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width, graphics_pos_t height);

void gui_menu_string_line_wrapping(painter_t* painter, const char* source, graphics_size_t sx, graphics_size_t sy, graphics_size_t width, graphics_size_t height);

#endif	/* GUI_MENU_H */
