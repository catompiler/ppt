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

// период обновления зуммера
#define DRIVE_UI_BUZZER_PERIOD 10
#define DRIVE_UI_BUZZER_SEQUECE_WARNING 10 
#define DRIVE_UI_BUZZER_SEQUECE_ALARM 5

//! Перечисление светодиодов.
typedef enum _Drive_Ui_Buzzer_State {
    GUI_BUZZER_ON = 0, // Звуковое оповещение есть
    GUI_BUZZER_OFF = 1 // Звуковое оповещение отключено
} drive_ui_buzzer_state_t;


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

/**
 * Возвращает статус разрешено звуковое оповещение
 * @return 
 */
extern bool drive_ui_sound_enabled(void);

/**
 * Возвращает статус запрещено звуковое оповещение
 * @return 
 */
extern bool drive_ui_sound_disabled(void);

#endif /* DRIVE_UI_H */
