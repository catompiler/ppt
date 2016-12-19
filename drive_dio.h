/**
 * @file drive_dio.h Библиотека цифровых входов-выходов привода.
 */

#ifndef DRIVE_DIO_H
#define DRIVE_DIO_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f10x.h"
#include "errors/errors.h"

//! Количество цифровых входов.
#define DRIVE_DIO_INPUTS_COUNT 5
//! Количество цифровых выходов.
#define DRIVE_DIO_OUTPUTS_COUNT 4

//! Перечисление инвертированности логического уровня сигнала.
typedef enum _Drive_Dio_Inversion {
    DRIVE_DIO_INVERSION_NONE = 0,
    DRIVE_DIO_INVERSION_INVERTED = 1,
} drive_dio_inversion_t;

//! Перечисление типов цифровых входов.
typedef enum _Drive_Dio_Input_Type {
    DRIVE_DIO_IN_NONE = 0,
    DRIVE_DIO_IN_STOP_START = 1,
    DRIVE_DIO_IN_EMERGENCY_STOP = 2,
    DRIVE_DIO_IN_REFERENCE_INC = 3,
    DRIVE_DIO_IN_REFERENCE_DEC = 4,
    DRIVE_DIO_IN_CLEAR_ERRORS = 5,
    DRIVE_DIO_IN_USER = 6,
} drive_dio_input_type_t;

//! Перечисление типов цифровых выходов.
typedef enum _Drive_Dio_Output_Type {
    DRIVE_DIO_OUT_NONE = 0,
    DRIVE_DIO_OUT_READY = 1,
    DRIVE_DIO_OUT_RUNNING = 2,
    DRIVE_DIO_OUT_ERROR = 3,
    DRIVE_DIO_OUT_WARNING = 4,
    DRIVE_DIO_OUT_USER = 5,
} drive_dio_output_type_t;

//! Перечисление номеров цифровых входов.
typedef enum _Drive_Dio_Input {
    DRIVE_DIO_INPUT_1 = 0,
    DRIVE_DIO_INPUT_2 = 1,
    DRIVE_DIO_INPUT_3 = 2,
    DRIVE_DIO_INPUT_4 = 3,
    DRIVE_DIO_INPUT_5 = 4,
} drive_dio_input_t;

//! Перечисление номеров цифровых выходов.
typedef enum _Drive_Dio_Output {
    DRIVE_DIO_OUTPUT_1 = 0,
    DRIVE_DIO_OUTPUT_2 = 1,
    DRIVE_DIO_OUTPUT_3 = 2,
    DRIVE_DIO_OUTPUT_4 = 3,
} drive_dio_output_t;

//! Структура инициализации цифрового входа.
typedef struct _Drive_Dio_Input_Init {
    drive_dio_input_t input;
    drive_dio_input_type_t type;
    drive_dio_inversion_t inversion;
    GPIO_TypeDef* GPIO;
    uint16_t pin;
} drive_dio_input_init_t;

//! Структура инициализации цифрового выхода.
typedef struct _Drive_Dio_Output_Init {
    drive_dio_output_t output;
    drive_dio_output_type_t type;
    drive_dio_inversion_t inversion;
    GPIO_TypeDef* GPIO;
    uint16_t pin;
} drive_dio_output_init_t;

//! Перечисление состояния цифровых входов / выходов.
typedef enum _Drive_Dio_State {
    DRIVE_DIO_OFF = 0,
    DRIVE_DIO_ON = 1
} drive_dio_state_t;

//! Каллбэк при изменении состояния цифрового входа.
typedef void (*drive_dio_on_input_changed_callback_t)(drive_dio_input_type_t type, drive_dio_state_t state);


/**
 * Инициализирует цифровые входа/выхода привода.
 * @return Код ошибки.
 */
extern err_t drive_dio_init(void);

/**
 * Инициализирует цифровой вход.
 * @param input_init Структура инициализации цифрового входа.
 * @return Код ошибки.
 */
extern err_t drive_dio_input_init(drive_dio_input_init_t* input_init);

/**
 * Инициализирует цифровой выход.
 * @param output_init Структура инициализации цифрового выхода.
 * @return Код ошибки.
 */
extern err_t drive_dio_output_init(drive_dio_output_init_t* output_init);

/**
 * Устанавливает пин для цифрового входа.
 * @param input Цифровой вход.
 * @param GPIO Порт.
 * @param pin Пин.
 * @return Код ошибки.
 */
extern err_t drive_dio_input_set_gpio(drive_dio_input_t input, GPIO_TypeDef* GPIO, uint16_t pin);

/**
 * Настраивает цифровой вход.
 * @param input Цифровой вход.
 * @param type Тип цифрового входа.
 * @param inversion Инверсия значения.
 * @return Код ошибки.
 */
extern err_t drive_dio_input_setup(drive_dio_input_t input,
        drive_dio_input_type_t type, drive_dio_inversion_t inversion);

/**
 * Устанавливает пин для цифрового выхода.
 * @param output Цифровой выход.
 * @param GPIO Порт.
 * @param pin Пин.
 * @return Код ошибки.
 */
extern err_t drive_dio_output_set_gpio(drive_dio_output_t output, GPIO_TypeDef* GPIO, uint16_t pin);

/**
 * Настраивает цифровой выход.
 * @param output Цифровой выход.
 * @param type Тип цифрового выхода.
 * @param inversion Инверсия значения.
 * @return Код ошибки.
 */
extern err_t drive_dio_output_setup(drive_dio_output_t output,
             drive_dio_output_type_t type, drive_dio_inversion_t inversion);

/**
 * Устанавливает каллбэк при изменении состояния цифрового входа.
 * @param callback Каллбэк.
 */
extern void drive_dio_set_on_input_changed_callback(drive_dio_on_input_changed_callback_t callback);

/**
 * Получает состояние заданного цифрового входа.
 * @param input Цифровой вход.
 * @return Состояние цифрового входа.
 */
extern drive_dio_state_t drive_dio_input_state(drive_dio_input_t input);

/**
 * Получает состояние типа цифрового входа.
 * @param type Тип цифрового входа.
 * @return Состояние типа цифрового входа.
 */
extern drive_dio_state_t drive_dio_input_type_state(drive_dio_input_type_t type);

/**
 * Получает состояние типа цифрового входа.
 * @param type Тип цифрового входа.
 * @param state Состояние типа цифрового входа.
 * @return Наличие цифрового входа с заданным типом.
 */
extern bool drive_dio_input_get_type_state(drive_dio_input_type_t type, drive_dio_state_t* state);

/**
 * Получает состояние заданного цифрового выхода.
 * @param output Цифровой выход.
 * @return Состояние цифрового выхода.
 */
extern drive_dio_state_t drive_dio_output_state(drive_dio_output_t output);

/**
 * Получает состояние типа цифрового выхода.
 * @param type Тип цифрового выхода.
 * @return Состояние типа цифрового выхода.
 */
extern drive_dio_state_t drive_dio_output_type_state(drive_dio_output_type_t type);

/**
 * Получает состояние типа цифрового выхода.
 * @param type Тип цифрового выхода.
 * @param state Состояние типа цифрового выхода.
 * @return Наличие цифрового выхода с заданным типом.
 */
extern bool drive_dio_output_get_type_state(drive_dio_output_type_t type, drive_dio_state_t* state);

/**
 * Устанавливает состояние заданного цифрового выхода.
 * @param output Цифровой выход.
 * @param state Состояние цифрового выхода.
 * @return Код ошибки.
 */
extern err_t drive_dio_set_output_state(drive_dio_output_t output, drive_dio_state_t state);

/**
 * Устанавливает состояние типа цифрового выхода.
 * @param type Тип цифрового выхода.
 * @param state Состояние типа цифрового выхода.
 */
extern void drive_dio_set_output_type_state(drive_dio_output_type_t type, drive_dio_state_t state);

/**
 * Устанавливает состояние заданного цифрового выхода.
 * @param output Цифровой выход.
 * @return Код ошибки.
 */
extern err_t drive_dio_toggle_output_state(drive_dio_output_t output);

/**
 * Устанавливает состояние типа цифрового выхода.
 * @param type Тип цифрового выхода.
 */
extern void drive_dio_toggle_output_type_state(drive_dio_output_type_t type);

/**
 * Обработчик изменения состояния цифрового входа.
 * @param input Цифровой вход.
 * @return Код ошибки.
 */
extern err_t drive_dio_input_changed(drive_dio_input_t input);

#endif /* DRIVE_DIO_H */
