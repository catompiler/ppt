/**
 * @file menu_explorer.h Реализация проводника меню.
 */

#ifndef MENU_EXPLORER_H
#define	MENU_EXPLORER_H

#include "menu/menu.h"

// Максимальный уровень меню
#define MENU_EXPLORER_MAX_LEVEL 7
// Количество отображаемых на экране элементов меню
#define MENU_EXPLORER_DISPLAYED_COUNT 7

// Тип состояния меню
typedef enum _Menu_Explorer_State {
    MENU_EXPLORER_STATE_HOME = 0,
    MENU_EXPLORER_STATE_NAVI,
    MENU_EXPLORER_STATE_EDIT,
} menu_explorer_state_t;

//! Тип Проводник меню.
typedef struct _Menu_Explorer menu_explorer_t;

//! Проводник меню.
struct _Menu_Explorer {
    menu_item_t* displayed[MENU_EXPLORER_DISPLAYED_COUNT]; //!< Указатели на отображаемые элементы меню
    menu_t* menu; //!< Стурктура меню
    menu_item_t* sel_object; //!< Указатель на выбранный элемент меню
    menu_item_t* history[MENU_EXPLORER_MAX_LEVEL]; //!< Список ранее выбранных элементов меню (индекс - уровень меню)
    uint8_t history_pos[MENU_EXPLORER_MAX_LEVEL]; //!< Список позиций ранее выбранных элементов меню (индекс - уровень меню)
    uint8_t history_item_pos[MENU_EXPLORER_MAX_LEVEL]; //!< Список абсолютных позиций ранее выбранных элементов меню (индекс - уровень меню)
    uint8_t level; //!< Текущий уровень меню
    uint8_t sel_pos; //!< Позиция текущего элемента в отображаемых элементах
    uint8_t item_pos; //!< Позиция текущего элемента в списке
    uint8_t count; //!< Количество элементов меню текущего уровня
    bool help; //!< Показывать справку по элементу меню
    menu_explorer_state_t state; //!< Состояние меню (режим)
};

/**
 * Инициализирует объект меню.
 * @param explorer Проводник меню
 * @param menu Структура меню
 * @return Код ошибки
 */
EXTERN err_t menu_explorer_init(menu_explorer_t* explorer, menu_t* menu);

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

#endif	/* MENU_EXPLORER */

