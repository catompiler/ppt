/**
 * @file menu_explorer.h Реализация проводника меню.
 */

#ifndef MENU_EXPLORER_H
#define	MENU_EXPLORER_H

#include "menu/menu.h"
#include "settings.h"
#include "counter/counter.h"

// Максимальный уровень меню
#define MENU_EXPLORER_MAX_LEVEL 7
// Количество отображаемых на экране элементов меню
#define MENU_EXPLORER_DISPLAYED_COUNT 7

// Время повторного запроса пароля для предоставления прав пользователя в сек.
#define MENU_EXPLORER_USER_RESET_SEC 10

// Тип состояния меню
typedef enum _Menu_Explorer_State {
    MENU_EXPLORER_STATE_HOME = 0,           // начальная страница
    MENU_EXPLORER_STATE_NAVI,               // навигация
    MENU_EXPLORER_STATE_PASSWORD_REQUEST,   // запрос доступа
    MENU_EXPLORER_STATE_EDIT,               // редактирование
} menu_explorer_state_t;

// Тип режима перерисовки меню
typedef enum _Gui_Menu_Draw_Mode {
    GUI_MENU_DRAW_MODE_NONE = 0x0, // без перерисовки
    GUI_MENU_DRAW_MODE_LINES = 0x01, // перерисовка строки
    GUI_MENU_DRAW_MODE_TITLE = 0x02, // перерисовка заголовка
    GUI_MENU_DRAW_MODE_HELP = 0x04, // перерисовка строки справочной информации
    GUI_MENU_DRAW_MODE_VALUES = 0x08, // перерисовка значений
    GUI_MENU_DRAW_MODE_EDIT = 0x10, // перерисовка редактируемого значения
    GUI_MENU_DRAW_MODE_SCROLL = 0x20, // перерисовка строки справочной информации
    GUI_MENU_DRAW_MODE_PASSWORD = 0x40, // перерисовка запроса доступа   
    GUI_MENU_DRAW_MODE_PASSWORD_VALUE = 0x80, // перерисовка запроса доступа        
    GUI_MENU_DRAW_MODE_ALL = 0xFF, // полная перерисовка
} gui_menu_draw_mode_t;

typedef enum _Gui_Menu_User {
    MENU_USER_NONE = 0,
    MENU_USER_ADMIN,
    MENU_USER_ROOT
} menu_user_t;

//! Тип Проводник меню.
typedef struct _Menu_Explorer menu_explorer_t;

//! Проводник меню.
struct _Menu_Explorer {
    menu_item_t* displayed[MENU_EXPLORER_DISPLAYED_COUNT]; //!< Указатели на отображаемые элементы меню
    menu_t* menu; //!< Стурктура меню
    menu_item_t* sel_object; //!< Указатель на выбранный элемент меню
    int32_t edit_param; //!< Значение редактируемого параметра выбранного элемента меню
    uint8_t edit_param_decim; //!< Кол-во разрядов редактируемого параметра выбранного элемента меню
    uint8_t edit_param_curdecim; //!< Текущий редактируемый разряд редактируемого параметра выбранного элемента меню
    uint8_t edit_param_curdecim_val; //!< Значение текущего редактируемого разряда редактируемого параметра выбранного элемента меню
    menu_item_t* history[MENU_EXPLORER_MAX_LEVEL]; //!< Список ранее выбранных элементов меню (индекс - уровень меню)
    uint8_t history_pos[MENU_EXPLORER_MAX_LEVEL]; //!< Список позиций ранее выбранных элементов меню (индекс - уровень меню)
    uint8_t history_item_pos[MENU_EXPLORER_MAX_LEVEL]; //!< Список абсолютных позиций ранее выбранных элементов меню (индекс - уровень меню)
    uint8_t level; //!< Текущий уровень меню
    uint8_t sel_pos; //!< Позиция текущего элемента в отображаемых элементах
    uint8_t item_pos; //!< Позиция текущего элемента в списке
    uint8_t count; //!< Количество элементов меню текущего уровня
    bool help; //!< Показывать справку по элементу меню
    menu_explorer_state_t state; //!< Состояние меню (режим)
    gui_menu_draw_mode_t draw_mode; //!< Режим перерисовки после обновления состояния
    counter_t touch; //!< Последнее время обращения пользователя
    menu_user_t user; //!< Права доступа текущего пользователя
};

/**
 * Инициализирует объект меню.
 * @param explorer Проводник меню
 * @param menu Структура меню
 * @return Код ошибки
 */
EXTERN err_t menu_explorer_init(menu_explorer_t* explorer, menu_t* menu);

/**
 * Обновляет последнее время обращения пользователя
 */
EXTERN void menu_explorer_touch(menu_explorer_t* explorer);

/**
 * Последнее время обращения пользователя
 */
EXTERN counter_t menu_explorer_get_touch(menu_explorer_t* explorer);

/**
 * Устанавливает права доступа (пользователя) меню
 * @param explorer
 * @param user
 */
EXTERN void menu_explorer_set_user(menu_explorer_t* explorer, menu_user_t user);

/**
 * Проверяет права доступа текущего пользователя меню
 * @param explorer
 * @param user
 * @return 
 */
EXTERN bool menu_explorer_user_is(menu_explorer_t* explorer, menu_user_t user);

/**
 * Проврека доступа к элементу меню 
 */
EXTERN bool menu_explorer_check_user(menu_explorer_t* explorer);

/**
 * Получает выбранный элемент меню
 * @param explorer Проводник меню
 * @return Выбранный элемент меню
 */
EXTERN menu_item_t* menu_explorer_sel(menu_explorer_t* explorer);

/**
 * Вход в подменю или выбор объекта для редактирования
 * @param explorer Проводник меню
 * @return Выбранный элемент меню
 */
EXTERN menu_item_t* menu_explorer_in(menu_explorer_t* explorer);

/**
 * Вход в подменю
 * @param explorer Проводник меню
 * @return Выбранный элемент меню
 */
menu_item_t* menu_explorer_navi_in(menu_explorer_t* explorer);

/**
 * Обработка нажатия Enter в режиме редактирования
 * @param explorer Проводник меню
 * @return Статус
 */
bool menu_explorer_edit_enter(menu_explorer_t* explorer);

/**
 * Обработка нажатия Esc в режиме редактирования
 * @param explorer Проводник меню
 * @return Статус
 */
bool menu_explorer_edit_esc(menu_explorer_t* explorer);

/**
 * Обработка длительного нажатия Esc в режиме редактирования
 * @param explorer Проводник меню
 * @return Статус
 */
bool menu_explorer_long_esc(menu_explorer_t* explorer);

/**
 * Выход на уровень выше меню или выход из редактирования
 * @param explorer Проводник меню
 * @return Статус
 */
EXTERN bool menu_explorer_out(menu_explorer_t* explorer);

/**
 * Выход на уровень выше меню
 * @param explorer Проводник меню
 * @return Статус
 */
bool menu_explorer_navi_out(menu_explorer_t* explorer);

/**
 * Обработка нажатия UP
 * @param explorer Проводник меню
 * @return Выбранный элемент меню
 */
EXTERN menu_item_t* menu_explorer_up(menu_explorer_t* explorer);
/**
 * Обработка нажатия DOWN
 * @param explorer Проводник меню
 * @return Выбранный элемент меню
 */
EXTERN menu_item_t* menu_explorer_down(menu_explorer_t* explorer);

/**
 * Выбрать следующий элемент меню
 * @param explorer Проводник меню
 * @return Выбранный элемент меню
 */
EXTERN menu_item_t* menu_explorer_next(menu_explorer_t* explorer);
/**
 * Выбрать предыдущий элемент меню
 * @param explorer Проводник меню
 * @return Выбранный элемент меню
 */
EXTERN menu_item_t* menu_explorer_prev(menu_explorer_t* explorer);

/**
 * Получает видимость справочной информации по элементу меню
 * @param explorer Проводник меню
 * @return Видимость справочной информации
 */
EXTERN bool menu_explorer_help_visible(menu_explorer_t* explorer);

/**
 * Устанавливает видимость справочной информации по элементу меню
 * @param explorer Проводник меню
 * @param visible Видимость элемента меню
 */
EXTERN void menu_explorer_set_help_visible(menu_explorer_t* explorer, bool visible);

/**
 * Переключает видимость справочной информации по элементу меню
 * @param explorer Проводник меню
 */
EXTERN void menu_explorer_help_toggle(menu_explorer_t* explorer);

/**
 * Получает количество отображаемых элементов меню с учетом вывода справки
 * @param explorer Проводник меню
 * @return Количество отображаемых элементов меню
 */
EXTERN uint8_t menu_explorer_displayed_cnt(menu_explorer_t* explorer);

/**
 * Получает позицию центрального отображаемого элемента меню с учетом вывода справки
 * @param explorer Проводник меню
 * @return Позиция центрального элемента меню
 */
EXTERN uint8_t menu_explorer_center_pos(menu_explorer_t* explorer);

/**
 * Получает статус элемента меню (выбран - не выбран)
 * @param explorer Проводник меню
 * @param item Элемент меню
 * @return Статус элемента меню (выбран - не выбран)
 */
EXTERN bool menu_explorer_is_selected(menu_explorer_t* explorer, menu_item_t* item);

/**
 * Возвращает статус состояния меню
 * @param explorer Проводник меню
 * @return Состояние меню
 */
EXTERN menu_explorer_state_t menu_explorer_state(menu_explorer_t* explorer);

/**
 * Возвращает флаг состояния меню Навигация
 * @param explorer Проводник меню
 * @return Состояние меню в режиме навигации
 */
EXTERN bool menu_explorer_state_navi(menu_explorer_t* explorer);

/**
 * Возвращает флаг состояния меню Редактирование
 * @param explorer Проводник меню
 * @return Состояние меню в режиме редактирования элемента
 */
EXTERN bool menu_explorer_state_edit(menu_explorer_t* explorer);

/**
 * Возвращает флаг состояния меню Главный экран
 * @param explorer Проводник меню
 * @return Состояние меню в режиме главного экрана
 */
EXTERN bool menu_explorer_state_home(menu_explorer_t* explorer);

/**
 * Возвращает флаг состояния меню Запрос доступа
 * @param explorer
 * @return 
 */
EXTERN bool menu_explorer_state_password_request(menu_explorer_t* explorer);

/**
 * Возвращает параметр текущего элемента меню
 * @param explorer Проводник меню
 * @return Параметр текущего элемента меню
 */
EXTERN param_t* menu_explorer_selelected_param(menu_explorer_t* explorer);

/**
 * 
 * @param explorer
 * @param password
 * @return 
 */
EXTERN bool menu_explorer_user_change(menu_explorer_t* explorer, int32_t password);

#endif	/* MENU_EXPLORER */

