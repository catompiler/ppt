/**
 * @file gui_tile.h Реализация виджета статусбара (строки состояния) графического интерфейса.
 */

#ifndef GUI_STATUSBAR_H
#define	GUI_STATUSBAR_H

#include "gui/gui_metro.h"
#include "gui/gui_widget.h"
#include "errors/errors.h"
#include "defs/defs.h"
#include "gui_time.h"
#include "drive.h"
#include "gui/resources/resources_colors.h"
#include "gui/bitmaps/icons_statusbar.h"
#include <stdint.h>

typedef struct _Gui_Statusbar gui_statusbar_t;
typedef struct _Gui_Icon gui_icon_t;

//! Зеначение идентификатора типа статусбара.
#define GUI_STATUSBAR_TYPE_ID 201

#define GUI_STATUSBAR_ICONS_COUNT 10
#define GUI_STATUSBAR_REF_STR_FORMAT "% 4d%%"

typedef bool (*gui_icon_condition_callback_t)();

//! Тип дескриптора иконок (условия отображения).
typedef struct _Gui_Icon_Condition {
    gui_icon_condition_callback_t callback; // функция-условие отображения иконки
    uint32_t param; // параметр функции условия
    // char всё же обычно используется для символов.
    // лучше использовать в таких случаях
    // либо typedef (что лучше, чтобы не менять по коду в случае необходимости),
    // либо что-то стандартное, например uint8_t
    // (вроде нет нигде явно требующегося знакового типа).
    uint8_t icon;             // идентификатор статической иконки
    graphics_color_t color;   // цвет иконки
    uint8_t* list;               // список идентификаторов иконок в анимации
} gui_icon_condition_t;

//! Начинает список условий отображения иконок.
#define GUI_ICON_CONDITIONS(arg_name, arg_count)\
        static const gui_icon_condition_t arg_name[arg_count] = 

//! Описывает дескриптор условия отображения иконки.
#define GUI_ICON_CONDITION(arg_callback, arg_param, arg_icon, arg_color, arg_list)\
        { .callback = (gui_icon_condition_callback_t)arg_callback, .param = (uint32_t)arg_param, .icon = arg_icon, .color = arg_color, .list = (uint8_t*)arg_list}

#define GUI_ICON_CONDITIONS_COUNT 7

//! Таблица отображения иконок в зависимости от условий
GUI_ICON_CONDITIONS(gui_icon_conditions, GUI_ICON_CONDITIONS_COUNT) {
    GUI_ICON_CONDITION(drive_ready, NULL, 0, THEME_COLOR_GREEN_L, &icons_statusbar_anim_ready),
    GUI_ICON_CONDITION(drive_running, NULL, 0, THEME_COLOR_GREEN_L, &icons_statusbar_anim_work),
    GUI_ICON_CONDITION(drive_error, DRIVE_ERROR_ROT_BREAK | DRIVE_ERROR_POWER_DATA_NOT_AVAIL | DRIVE_ERROR_PHASE, ICONS_STATUSBAR_VAL_WARNING, THEME_COLOR_RED_L, NULL),
    GUI_ICON_CONDITION(drive_error, DRIVE_ERROR_EMERGENCY_STOP, ICONS_STATUSBAR_VAL_EMERGENCY, THEME_COLOR_RED_L, NULL),
    GUI_ICON_CONDITION(drive_error, DRIVE_ERROR_THERMAL_OVERLOAD, ICONS_STATUSBAR_VAL_OVERHEAT, THEME_COLOR_RED_L, NULL),
    GUI_ICON_CONDITION(drive_warning, 0xffffff, ICONS_STATUSBAR_VAL_WARNING, THEME_COLOR_YELLOW, NULL),
    GUI_ICON_CONDITION(drive_warning, DRIVE_WARNING_THERMAL_OVERLOAD, ICONS_STATUSBAR_VAL_OVERHEAT, THEME_COLOR_YELLOW, NULL),
};

struct _Gui_Icon {
    const uint8_t* list; //!< Выбранный значок или последовательность значков
    uint8_t count; //!< Кол-во значков в последовательности
    uint8_t current; //!< Позиция текущего значка последовательности
    uint8_t value; //!< Новая позиция значка последовательности
    graphics_color_t color; //!< Цвет значка
};

struct _Gui_Statusbar {
    gui_widget_t super; //!< Суперкласс.
    gui_time_t time; //!< Отображение времени.
    gui_icon_t* icons; //!< Иконки статуса.
    graphics_t* icon_graphics; //!< Изображение значков
    char icon_count; //!< Количество значков в изображении
    reference_t reference; //!< Отображаемое значение задания привода.
};

//! Приводит указатель statusbar к типу статусбара.
#define GUI_STATUSBAR(statusbar) ((gui_statusbar_t*)(statusbar))

/**
 * Инициализирует плитку.
 * @param statusbar Статусбар.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
EXTERN err_t gui_statusbar_init(gui_statusbar_t* statusbar, gui_metro_t* gui);

/**
 * Получает изображение значков.
 * @param statusbar Статусбар.
 * @return Структура графических данных изображения.
 */
ALWAYS_INLINE static const graphics_t* gui_statusbar_graphics(gui_statusbar_t* statusbar)
{
    return statusbar->icon_graphics;
}

/**
 * Устанавливает изображение значков.
 * @param statusbar Статусбар.
 * @param graphics Структура графических данныех изображения.
 */
EXTERN void gui_statusbar_set_graphics(gui_statusbar_t* statusbar, graphics_t* graphics, uint8_t count);

/**
 * Устанавливает иконки.
 * @param statusbar Статусбар.
 * @param icons Набор иконок статусбара.
 */
EXTERN void gui_statusbar_set_icons(gui_statusbar_t* statusbar, gui_icon_t* icons);

/**
 * Инициализирует плитку как потомок заданного родителя.
 * @param tile Плитка.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
EXTERN err_t gui_statusbar_init_parent(gui_statusbar_t* statusbar, gui_metro_t* gui, gui_widget_t* parent);

/**
 * Обработчик перерисовки.
 * @param statusbar Статусбар.
 * @param rect Область перерисовки.
 */
EXTERN void gui_statusbar_on_repaint(gui_statusbar_t* statusbar, const rect_t* rect);

/**
 * Перерисовка значения задания привода и иконок статусбара
 * @param statusbar Статусбар
 * @param rect Область рисования
 */
EXTERN void gui_statusbar_repaint_reference_n_icons(gui_statusbar_t* statusbar, const rect_t* rect);

/**
 * Обновление статусбара
 * @param statusbar Статусбар
 * @param rect Область рисования
 */
EXTERN void gui_statusbar_update(gui_statusbar_t* statusbar, const rect_t* rect);

/**
 * Перерисовка иконки
 * @param statusbar Статусбар
 * @param painter 
 * @param theme Тема
 * @param left Начальное смещение иконок в статусбаре
 * @param right Правая граница иконок в статусбаре
 * @param pos Позиция иконки
 * @param icon Иконка
 */
EXTERN void gui_statusbar_icon_repaint(gui_statusbar_t* statusbar, painter_t* painter, const gui_metro_theme_t* theme, graphics_pos_t left, graphics_pos_t right, uint8_t pos, gui_icon_t* icon);

/**
 * Обновление состояния иконок статусбара
 * @param statusbar Статусбар
 * @param repaint Необходимость перерисовки иконки
 */
EXTERN void gui_statusbar_update_icons(gui_statusbar_t* statusbar, bool repaint);

/**
 * Проверка условий отображения иконки в соответствии с таблицей условий GUI_ICON_CONDITIONS
 */
EXTERN void gui_statusbar_update_icons_set_icon(gui_statusbar_t* statusbar, uint8_t* index, const gui_icon_condition_t* condition, bool repaint);

#endif	/* GUI_STATUSBAR_H */
