/**
 * @file drive_keypad.h Библиотека работы с кейпадом привода.
 */

#ifndef DRIVE_KEYPAD_H
#define DRIVE_KEYPAD_H


#include "errors/errors.h"
#include "pca9555/pca9555.h"
#include "input/keys.h"


//! Синоним для обозначения кнопки старт.
#define KEY_START KEY_F1
//! Синоним для обозначения кнопки стоп.
#define KEY_STOP KEY_F2
//! Синоним для обозначения кнопки = (+).
#define KEY_PLUS KEY_EQUAL


//! Перечисление светодиодов.
typedef enum _Drive_Keypad_Led {
    DRIVE_KPD_LED_0 = PCA9555_PIN_3,
    DRIVE_KPD_LED_1 = PCA9555_PIN_4,
    DRIVE_KPD_LED_2 = PCA9555_PIN_5,
    DRIVE_KPD_LED_3 = PCA9555_PIN_6,
    DRIVE_KPD_LED_4 = PCA9555_PIN_7,
    DRIVE_KPD_LED_ALL = 0xf8
} drive_kpd_led_t;

//! Тип светодиодов.
typedef pca9555_pins_t drive_kpd_leds_t;

//! Перечисление пинов.
typedef enum _Drive_Keypad_Pin {
    DRIVE_KPD_PIN_TFT_RST = PCA9555_PIN_0,
    DRIVE_KPD_PIN_BUZZ    = PCA9555_PIN_1,
    DRIVE_KPD_PIN_ALL     = 0x3
} drive_kpd_pin_t;

//! Тип пинов.
typedef pca9555_pins_t drive_kpd_pins_t;

//! Тип кнопок.
typedef enum _Drive_Keypad_Key {
    DRIVE_KPD_KEY_MINUS = PCA9555_PIN_15,
    //DRIVE_KPD_KEY_LEFT  = PCA9555_PIN_15,
    DRIVE_KPD_KEY_UP    = PCA9555_PIN_14,
    DRIVE_KPD_KEY_DOWN  = PCA9555_PIN_13,
    DRIVE_KPD_KEY_ESC   = PCA9555_PIN_11,
    DRIVE_KPD_KEY_ENTER = PCA9555_PIN_12,
    //DRIVE_KPD_KEY_OK    = PCA9555_PIN_11,
    DRIVE_KPD_KEY_PLUS  = PCA9555_PIN_10,
    //DRIVE_KPD_KEY_RIGHT = PCA9555_PIN_10,
    DRIVE_KPD_KEY_STOP  = PCA9555_PIN_9,
    DRIVE_KPD_KEY_START = PCA9555_PIN_8,
    DRIVE_KPD_KEY_ALL   = 0xff00
} drive_kpd_key_t;

//! Тип кнопок.
typedef pca9555_pins_t drive_kpd_keys_t;

//! Структура инициализации кейпада привода.
typedef struct _Drive_Keypad_Init {
    pca9555_t* ioport; //!< Порт ввода-вывода.
} drive_keypad_init_t;

/**
 * Инициализирует кейпад привода.
 * @param keypad_is Структура инициализации.
 * @return Код ошибки.
 */
extern err_t drive_keypad_init(drive_keypad_init_t* keypad_is);

/**
 * Ждёт завершения текущей операции.
 * @return Код ошибки.
 */
extern err_t drive_keypad_wait(void);

/**
 * Получает состояние светодиодов.
 * @return Состояние светодиодов.
 */
extern drive_kpd_leds_t drive_keypad_leds(void);

/**
 * Устанавливает состояние светодиодов.
 * @param leds Состояние светодиодов.
 * @return Код ошибки.
 */
extern err_t drive_keypad_set_leds(drive_kpd_leds_t leds);

/**
 * Зажигает заданные светодиоды.
 * @param leds Заданные светодиоды.
 * @return Код ошибки.
 */
extern err_t drive_keypad_leds_on(drive_kpd_leds_t leds);

/**
 * Гасит заданные светодиоды.
 * @param leds Заданные светодиоды.
 * @return Код ошибки.
 */
extern err_t drive_keypad_leds_off(drive_kpd_leds_t leds);

/**
 * Включает заданные пины.
 * @param pins Заданные пины.
 * @return Код ошибки.
 */
extern err_t drive_keypad_pins_on(drive_kpd_pins_t pins);

/**
 * Выключает заданные пины.
 * @param pins Заданные пины.
 * @return Код ошибки.
 */
extern err_t drive_keypad_pins_off(drive_kpd_pins_t pins);

/**
 * Получает состояние клавишь.
 * @return Состояние клавишь.
 */
extern drive_kpd_keys_t drive_keypad_state(void);

/**
 * Обработчик события нажатия на кнопку.
 * Помечает необходимость обновления состояния клавишь.
 */
extern void drive_keypad_pressed(void);

/**
 * Обрабатывает нажатые клавиши.
 */
extern void drive_keypad_process(void);

/**
 * Обрабатывает повторное нажатие клавишь при удерживании.
 */
extern void drive_keypad_repeat(void);

/**
 * Обновляет состояние клавишь.
 * @return true, если клавиши обновлены, иначе false.
 */
extern bool drive_keypad_update(void);

extern void drive_keypad_buzzer_toggle(void);
extern void drive_keypad_buzzer_on(void);
extern void drive_keypad_buzzer_off(void);

#endif /* DRIVE_KEYPAD_H */
