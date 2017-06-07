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
#include "drive_ui.h"
#include "drive_gui.h"
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
    uint8_t count;              // кол-во иконок в анимации
    uint8_t icon;             // идентификатор статической иконки
    graphics_color_t color;   // цвет иконки
    uint8_t* list;               // список идентификаторов иконок в анимации
} gui_icon_condition_t;

//! Начинает список условий отображения иконок.
#define GUI_ICON_CONDITIONS(arg_name, arg_count)\
        static const gui_icon_condition_t arg_name[arg_count] = 

//! Описывает дескриптор условия отображения иконки.
#define GUI_ICON_CONDITION(arg_callback, arg_param, arg_icon, arg_color, arg_list)\
        { .callback = (gui_icon_condition_callback_t)arg_callback, .param = (uint32_t)arg_param, .icon = arg_icon, .color = arg_color, .list = (uint8_t*)(arg_list), .count = sizeof(*arg_list)/sizeof(uint8_t)}

// другие ошибки
#define DRIVE_ERROR_ETC     DRIVE_ERROR_ROT_BREAK |\
                            DRIVE_ERROR_POWER_DATA_NOT_AVAIL |\
                            DRIVE_ERROR_PHASE

// ошибки по перегреву
#define DRIVE_ERROR_OVERHEAT    DRIVE_ERROR_THERMAL_OVERLOAD |\
                                DRIVE_ERROR_HEATSINK_TEMP

//другие предупреждения
#define DRIVE_WARNING_ETC 0xffffff

// предупреждения по перегреву
#define DRIVE_WARNING_OVERHEAT      DRIVE_WARNING_THERMAL_OVERLOAD |\
                                    DRIVE_WARNING_HEATSINK_TEMP

// ошибка по току возбуждения
#define DRIVE_POWER_ERROR_Iexc  DRIVE_POWER_ERROR_UNDERFLOW_Iexc |\
                                DRIVE_POWER_ERROR_OVERFLOW_Iexc |\
                                DRIVE_POWER_ERROR_IDLE_Iexc

// предупреждение по току возбуждения
#define DRIVE_POWER_WARNING_Iexc  DRIVE_POWER_WARNING_UNDERFLOW_Iexc |\
                                DRIVE_POWER_WARNING_OVERFLOW_Iexc |\
                                DRIVE_POWER_WARNING_IDLE_Iexc

// ошибка по напряжению
#define DRIVE_POWER_ERROR_U DRIVE_POWER_ERROR_UNDERFLOW_Ua |\
                            DRIVE_POWER_ERROR_UNDERFLOW_Ub |\
                            DRIVE_POWER_ERROR_UNDERFLOW_Uc |\
                            DRIVE_POWER_ERROR_UNDERFLOW_Urot |\
                            DRIVE_POWER_ERROR_OVERFLOW_Ua |\
                            DRIVE_POWER_ERROR_OVERFLOW_Ub |\
                            DRIVE_POWER_ERROR_OVERFLOW_Uc |\
                            DRIVE_POWER_ERROR_OVERFLOW_Urot |\
                            DRIVE_POWER_ERROR_IDLE_Urot

// предупреждение по напряжению
#define DRIVE_POWER_WARNING_U   DRIVE_POWER_WARNING_UNDERFLOW_Ua |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Ub |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Uc |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Urot |\
                                DRIVE_POWER_WARNING_OVERFLOW_Ua |\
                                DRIVE_POWER_WARNING_OVERFLOW_Ub |\
                                DRIVE_POWER_WARNING_OVERFLOW_Uc |\
                                DRIVE_POWER_WARNING_OVERFLOW_Urot |\
                                DRIVE_POWER_WARNING_IDLE_Urot
// ошибка по току
#define DRIVE_POWER_ERROR_I     DRIVE_POWER_ERROR_IDLE_Ia |\
                                DRIVE_POWER_ERROR_IDLE_Ib |\
                                DRIVE_POWER_ERROR_IDLE_Ic |\
                                DRIVE_POWER_ERROR_IDLE_Irot |\
                                DRIVE_POWER_ERROR_UNDERFLOW_Ia |\
                                DRIVE_POWER_ERROR_UNDERFLOW_Ib |\
                                DRIVE_POWER_ERROR_UNDERFLOW_Ic |\
                                DRIVE_POWER_ERROR_UNDERFLOW_Irot |\
                                DRIVE_POWER_ERROR_OVERFLOW_Ia |\
                                DRIVE_POWER_ERROR_OVERFLOW_Ib |\
                                DRIVE_POWER_ERROR_OVERFLOW_Ic |\
                                DRIVE_POWER_ERROR_OVERFLOW_Irot

// предупреждение по току
#define DRIVE_POWER_WARNING_I   DRIVE_POWER_WARNING_IDLE_Ia |\
                                DRIVE_POWER_WARNING_IDLE_Ib |\
                                DRIVE_POWER_WARNING_IDLE_Ic |\
                                DRIVE_POWER_WARNING_IDLE_Irot |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Ia |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Ib |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Ic |\
                                DRIVE_POWER_WARNING_UNDERFLOW_Irot |\
                                DRIVE_POWER_WARNING_OVERFLOW_Ia |\
                                DRIVE_POWER_WARNING_OVERFLOW_Ib |\
                                DRIVE_POWER_WARNING_OVERFLOW_Ic |\
                                DRIVE_POWER_WARNING_OVERFLOW_Irot

// ошибка по току вентилятора
#define DRIVE_POWER_ERROR_Ifan  DRIVE_POWER_ERROR_UNDERFLOW_Ifan |\
                                DRIVE_POWER_ERROR_OVERFLOW_Ifan

// предупреждение по току вентилятора
#define DRIVE_POWER_WARNING_Ifan  DRIVE_POWER_WARNING_UNDERFLOW_Ifan |\
                                  DRIVE_POWER_WARNING_OVERFLOW_Ifan

// ошибка по току 4-20
#define DRIVE_POWER_ERROR_Iref    DRIVE_POWER_ERROR_UNDERFLOW_Iref |\
                                    DRIVE_POWER_ERROR_OVERFLOW_Iref
// предупреждение по току 4-20
#define DRIVE_POWER_WARNING_Iref    DRIVE_POWER_WARNING_UNDERFLOW_Iref |\
                                    DRIVE_POWER_WARNING_OVERFLOW_Iref

/**
 * Возвращает статус готовности привода (до его запуска)
 * @return 
 */
EXTERN bool gui_statusbar_drive_ready();

//! Таблица отображения иконок в зависимости от условий
#define GUI_ICON_CONDITIONS_COUNT 23
GUI_ICON_CONDITIONS(gui_icon_conditions, GUI_ICON_CONDITIONS_COUNT) {
    // состояние
    GUI_ICON_CONDITION(gui_statusbar_drive_ready,     NULL,   0,  THEME_COLOR_BLUE_L,    &icons_statusbar_anim_ready),
    GUI_ICON_CONDITION(drive_running,   NULL,   0,  THEME_COLOR_GREEN_L,    &icons_statusbar_anim_work),
    // ошибки
    GUI_ICON_CONDITION(drive_error,         DRIVE_ERROR_ETC,            ICONS_STATUSBAR_VAL_WARNING,    THEME_COLOR_RED_L, NULL),
    GUI_ICON_CONDITION(drive_error,         DRIVE_ERROR_EMERGENCY_STOP, ICONS_STATUSBAR_VAL_EMERGENCY,  THEME_COLOR_RED_L, NULL),
    GUI_ICON_CONDITION(drive_power_error,   DRIVE_POWER_ERROR_Iexc,     ICONS_STATUSBAR_VAL_FAULT_F,    THEME_COLOR_RED_L, NULL),
    GUI_ICON_CONDITION(drive_power_error,   DRIVE_POWER_ERROR_U,        ICONS_STATUSBAR_VAL_FAULT_U,    THEME_COLOR_RED_L, NULL),
    GUI_ICON_CONDITION(drive_power_error,   DRIVE_POWER_ERROR_I,        ICONS_STATUSBAR_VAL_FAULT_I,    THEME_COLOR_RED_L, NULL),
    GUI_ICON_CONDITION(drive_power_error,   DRIVE_POWER_ERROR_Ifan,     ICONS_STATUSBAR_VAL_FAN,        THEME_COLOR_RED_L, NULL),
    GUI_ICON_CONDITION(drive_error,         DRIVE_ERROR_OVERHEAT,       ICONS_STATUSBAR_VAL_OVERHEAT, THEME_COLOR_RED_L, NULL),
    GUI_ICON_CONDITION(drive_power_error,   DRIVE_POWER_ERROR_Iref,     ICONS_STATUSBAR_VAL_LOOP_BREAK, THEME_COLOR_RED_L, NULL),
          
    // предупреждения
    GUI_ICON_CONDITION(drive_warning,       DRIVE_WARNING_ETC,              ICONS_STATUSBAR_VAL_WARNING,    THEME_COLOR_YELLOW, NULL),
    GUI_ICON_CONDITION(drive_power_warning, DRIVE_POWER_WARNING_Iexc,       ICONS_STATUSBAR_VAL_FAULT_F,    THEME_COLOR_YELLOW, NULL),
    GUI_ICON_CONDITION(drive_power_warning, DRIVE_POWER_WARNING_U,          ICONS_STATUSBAR_VAL_FAULT_U,    THEME_COLOR_YELLOW, NULL),
    GUI_ICON_CONDITION(drive_power_warning, DRIVE_POWER_WARNING_I,          ICONS_STATUSBAR_VAL_FAULT_I,    THEME_COLOR_YELLOW, NULL),
    GUI_ICON_CONDITION(drive_power_warning, DRIVE_POWER_WARNING_Ifan,       ICONS_STATUSBAR_VAL_FAN,        THEME_COLOR_YELLOW, NULL),
    GUI_ICON_CONDITION(drive_warning,       DRIVE_WARNING_OVERHEAT,         ICONS_STATUSBAR_VAL_OVERHEAT,   THEME_COLOR_YELLOW, NULL),
    GUI_ICON_CONDITION(drive_warning,       DRIVE_WARNING_FAN_FAIL,         ICONS_STATUSBAR_VAL_FAN,        THEME_COLOR_YELLOW, NULL),
    GUI_ICON_CONDITION(drive_power_warning, DRIVE_POWER_WARNING_Iref,       ICONS_STATUSBAR_VAL_LOOP_BREAK, THEME_COLOR_YELLOW, NULL),
 
    // информация
    GUI_ICON_CONDITION(drive_gui_modbus_status_work,    NULL,   0,                               THEME_COLOR_WHITE, &icons_statusbar_anim_modbus_work),
    GUI_ICON_CONDITION(drive_gui_modbus_status_idle,    NULL,   ICONS_STATUSBAR_VAL_MODBUS_IDLE,    THEME_COLOR_WHITE, NULL),
    GUI_ICON_CONDITION(drive_ui_sound_disabled,         NULL,   ICONS_STATUSBAR_VAL_BEEP_OFF,    THEME_COLOR_WHITE, NULL),
    
    GUI_ICON_CONDITION(drive_gui_menu_user_is,  MENU_USER_ROOT,     ICONS_STATUSBAR_VAL_ROOT,    THEME_COLOR_WHITE, NULL),
    GUI_ICON_CONDITION(drive_gui_menu_user_is,  MENU_USER_ADMIN,    ICONS_STATUSBAR_VAL_KEY,    THEME_COLOR_WHITE, NULL),
    
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

/**
 * Сброс значений иконки
 */
EXTERN void gui_statusbar_update_icons_unset_icon(gui_statusbar_t* statusbar, uint8_t index);

#endif	/* GUI_STATUSBAR_H */
