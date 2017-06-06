#include "commands.h"
#include "drive.h"
#include "settings.h"
#include "drive_events.h"
#include "drive_keypad.h"
#include "drive_tasks.h"
#include <stdlib.h>

/**
 * Функция сравнения двух параметров по идентификатору.
 * @param a Параметр А.
 * @param b Параметр Б.
 * @return Результат сравнения.
 */
static int commands_compare_ids(const void * key, const void * item)
{
    command_id_t item_id = ((command_menu_t*)item)->id;
    command_id_t id = (command_id_t)(unsigned long)key;
    
    if(id > item_id) return 1;
    if(id < item_id) return -1;
    
    return 0;
}

command_menu_t* command_get_by_id(command_id_t id) 
{
    command_menu_t* command = bsearch((void*)(unsigned long)id, menu_commands, COMMANDS_COUNT, sizeof(command_menu_t), commands_compare_ids);
    return command;
}

bool command_start_stop(void) {
    switch(drive_state()) {
        case DRIVE_STATE_INIT: //!< Инициализация.
        case DRIVE_STATE_CALIBRATION: //!< Калибровка питания.
            break;
        case DRIVE_STATE_IDLE: //!< Простой (готовность).
        case DRIVE_STATE_STOP: //!< Останов.
            return drive_start();
        case DRIVE_STATE_START: //!< Запуск.
        case DRIVE_STATE_RUN: //!< Работа.
            return drive_stop();
            break;
        case DRIVE_STATE_STOP_ERROR: //!< Останов при ошибке.
        case DRIVE_STATE_ERROR:  //!< Ошибка.
            break;
    }
    return false;
}

bool command_enable_drive(void) {
    // TODO
    return false;
}

bool command_settings_save(void) {
    return (drive_tasks_save_settings() == E_NO_ERROR);
}

bool command_state_save(void) {
    return (drive_tasks_write_status_event() == E_NO_ERROR);
}

bool command_clear_events(void) {
    return (drive_tasks_clear_events() == E_NO_ERROR);
}

bool command_settings_default(void) {
    return (settings_default() == E_NO_ERROR);
}

bool command_test_leds(void) {
    drive_keypad_leds_on(DRIVE_KPD_LED_ALL);
    return true;
}

