/**
 * @file drive_ui.h Реализация интерфейса привода.
 */

#ifndef DRIVE_UI_H
#define DRIVE_UI_H

#include "errors/errors.h"
#include "pca9555/pca9555.h"
#include "tft9341/tft9341.h"
#include "drive_keypad.h"
#include "drive_gui.h"


//! Структура инициализации интерфейса привода.
typedef struct _Drive_Ui_Init {
    pca9555_t* ioport; //!< Порт ввода-вывода.
    tft9341_t* tft; //!< TFT.
    reset_i2c_bus_proc_t reset_i2c_bus_proc; //!< Функция сброса i2c.
} drive_ui_init_t;


/**
 * Инициализирует интерфейс привода.
 * @param ui_is Структура инициализации.
 * @return Код ошибки.
 */
extern err_t drive_ui_init(drive_ui_init_t* ui_is);

/**
 * Управляет обновлением интерфейса пользователя.
 */
extern void drive_ui_process(void);

#endif /* DRIVE_UI_H */
