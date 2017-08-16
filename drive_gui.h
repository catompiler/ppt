/**
 * @file Реализация графического интерфейса привода.
 */

#ifndef DRIVE_GUI_H
#define DRIVE_GUI_H

#include "errors/errors.h"
#include "tft9341/tft9341.h"
#include "input/key_input.h"
#include "gui/widgets/gui_home.h"
#include "gui/widgets/gui_menu.h"

//! Список доступных языков интерфейса
#define GUI_TILE_LANGUAGES_COUNT 2
#define GUI_TILE_LANGUAGES_MIN 0
#define GUI_TILE_LANGUAGES_MAX (GUI_TILE_LANGUAGES_COUNT - 1)
#define GUI_TILE_LANGUAGES(arg_name, arg_count)\
        static const lang_id_t arg_name[arg_count] = 
#define GUI_TILE_LANGUAGE(lang_id) lang_id

//! Структура инициализации графического интерфейса привода.
typedef struct _Drive_Gui_Init {
    tft9341_t* tft; //!< TFT.
} drive_gui_init_t;


/**
 * Инициализирует графический интерфейс привода.
 * @param gui_is Структура инициализации.
 * @return Код ошибки.
 */
extern err_t drive_gui_init(drive_gui_init_t* gui_is);

/**
 * Проверка параметра и изменение языка интерфейса
 */
extern bool drive_gui_check_language(void);

/**
 * Перерисовывает графический интерфейс привода.
 */
extern void drive_gui_repaint(void);

/**
 * Обновляет графический интерфейс привода.
 */
extern void drive_gui_update(void);

/**
 * Обрабатывает нажатие клавиши.
 * @param key Код клавиши.
 */
extern void drive_gui_on_key_pressed(keycode_t key);

/**
 * Обрабатывает отпускание клавиши.
 * @param key Код клавиши.
 */
extern void drive_gui_on_key_released(keycode_t key);

/**
 * Обработка перехода от главного экрана к меню по нажатию Enter
 * @param home
 */
extern void drive_gui_home_on_enter(gui_home_t* home);

/**
 * Обработка перехода от меню к главному экрану по нажатию Home (Esc)
 * @param menu
 */
extern void drive_gui_menu_on_home(gui_menu_t* menu);

/**
 * Обновление состояния плиток
 */
extern void drive_gui_update_tiles();

/**
 * Устанавливает последнее время обработки callback для отображения статуса в GUI
 */
extern void drive_gui_modbus_set_last_time();

/**
 * Статус обмена сообщениями по Modbus для GUI
 */
extern bool drive_gui_modbus_status_work(void);

/**
 * Статус простоя (ожидания) по Modbus для GUI
 */
extern bool drive_gui_modbus_status_idle(void);

extern bool drive_gui_menu_user_is(menu_user_t user);

extern counter_t drive_gui_get_touch_menu_explorer();

void drive_gui_tiles_init();

#endif /* DRIVE_GUI_H */
