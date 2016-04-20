/**
 * @file Реализация графического интерфейса привода.
 */

#ifndef DRIVE_GUI_H
#define DRIVE_GUI_H

#include "errors/errors.h"
#include "tft9341/tft9341.h"
#include "input/key_input.h"


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

#endif /* DRIVE_GUI_H */
