/**
 * @file gui_home.h Реализация главного экрана с плитками.
 */

#ifndef GUI_HOME_H
#define	GUI_HOME_H

#include "../gui_metro.h"
#include "../gui_widget.h"
#include "gui_tile.h"
#include "errors/errors.h"
#include "defs/defs.h"
#include "translations_ids.h"

//! Зеначение идентификатора типа главного экрана.
#define GUI_HOME_TYPE_ID 301
#define GUI_HOME_TILES_WIDTH 2
#define GUI_HOME_TILES_HEIGHT 2
#define GUI_HOME_TILES_COUNT (GUI_HOME_TILES_WIDTH * GUI_HOME_TILES_HEIGHT)

#define GUI_HOME_ON_ENTER_PROC(proc) ((void (*)(gui_home_t*))proc)

typedef struct _Gui_Home gui_home_t;

struct _Gui_Home {
    gui_widget_t super; //!< Суперкласс.
   // gui_tile_t* tiles[GUI_HOME_TILES_COUNT];
    void (*on_enter)(gui_home_t* home); //!< Каллбэк по нажатию Enter (вход в меню)
};

typedef void (*gui_home_on_enter_t)(gui_home_t*);

typedef enum _Home_Tile_Param {
    HOME_TILE_CAPTION = 0,
    HOME_TILE_UNIT,
} home_tile_param_t;

#define HOME_TILES_VALUES(home_tiles_values)\
static const param_id_t home_tiles_values[GUI_HOME_TILES_COUNT] = {\
    PARAM_ID_GUI_TILE_1,\
    PARAM_ID_GUI_TILE_2,\
    PARAM_ID_GUI_TILE_3,\
    PARAM_ID_GUI_TILE_4,\
};

//! Приводит указатель home к типу главного экрана.
#define GUI_HOME(home) ((gui_home_t*)(home))

/**
 * Инициализирует главный экран.
 * @param home Главный экран.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
EXTERN err_t gui_home_init(gui_home_t* home, gui_metro_t* gui);

/**
 * Инициализирует главный экран как потомок заданного родителя.
 * @param home Главный экран.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
EXTERN err_t gui_home_init_parent(gui_home_t* home, gui_metro_t* gui, gui_widget_t* parent);

/**
 * Обработчик перерисовки.
 * @param home Главный экран.
 * @param rect Область перерисовки.
 */
EXTERN void gui_home_on_repaint(gui_home_t* home, const rect_t* rect);

/**
 * Обработчик перерисовки значения.
 * @param home Главный экран.
 */
EXTERN void gui_home_repaint_value(gui_home_t* home, const rect_t* rect);

/**
 * Обработчик нажатия клавиши.
 * @param menu Меню.
 * @param key Код клавиши.
 */
EXTERN void gui_home_on_key_press(gui_home_t* home, keycode_t key);

/**
 * Обработчик отпускания клавиши.
 * @param menu Меню.
 * @param key Код клавиши.
 */
EXTERN void gui_home_on_key_release(gui_home_t* home, keycode_t key);

/**
 * Получает каллбэк нажатия Enter.
 * @param home Главный экран.
 * @return Каллбэк нажатия Enter.
 */
ALWAYS_INLINE static gui_home_on_enter_t gui_home_on_enter(gui_home_t* home)
{
    return home->on_enter;
}

/**
 * Устанавливает каллбэк нажатия Enter.
 * @param home Главный экран.
 * @param on_enter Каллбэк нажатия Enter.
 */
ALWAYS_INLINE static void gui_home_set_on_enter(gui_home_t* home, gui_home_on_enter_t on_enter)
{
    home->on_enter = on_enter;
}

/**
 * Устанавливает видимость плиток главного экрана
 * @param tiles Плитки главного экрана
 * @param visible Статус видимости
 */
EXTERN void gui_home_tiles_set_visible(gui_tile_t* tiles, bool visible);

#endif	/* GUI_HOME_H */
