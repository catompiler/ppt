/* 
 * @file commands.h Команды меню.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>
#include <stdbool.h>
#include "commands_ids.h"

//! Тип функции
typedef bool (*commands_callback_t)();

//! Тип идентификатора команды меню.
typedef uint16_t command_id_t;

//! Тип команды меню.
typedef struct _Command_Menu {
    command_id_t id;
    commands_callback_t callback;
} command_menu_t;

//! Начинает список команд меню.
#define MENU_COMMANDS(arg_name, arg_count)\
        static const command_menu_t arg_name[arg_count] = 

//! Описывает команду меню.
#define MENU_COMMAND(arg_id, arg_function)\
        { .id = arg_id, .callback = (commands_callback_t)arg_function }

extern command_menu_t* command_get_by_id(command_id_t id);

extern bool command_start_stop(void);

extern bool command_enable_drive(void);

extern bool command_settings_save(void);

extern bool command_state_save(void);

extern bool command_settings_default(void);

extern bool command_test_leds(void);

// Количество команд меню.
#define COMMANDS_COUNT 6

// Объявление команд меню.
MENU_COMMANDS(menu_commands, COMMANDS_COUNT) {
    MENU_COMMAND(CMD_ID_START_STOP, &command_start_stop),
    MENU_COMMAND(CMD_ID_ENABLE_DRIVE, &command_enable_drive),
    MENU_COMMAND(CMD_ID_SETTINGS_SAVE, &command_settings_save),
    MENU_COMMAND(CMD_ID_STATE_SAVE, &command_state_save),
    MENU_COMMAND(CMD_ID_SETTINGS_DEFAULT, &command_settings_default),
    MENU_COMMAND(CMD_ID_TEST_LEDS, &command_test_leds),
};

#endif /* COMMANDS_H */

