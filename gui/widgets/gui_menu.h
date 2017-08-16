/**
 * @file gui_menu.h Реализация меню графического интерфейса.
 */

#ifndef GUI_MENU_H
#define	GUI_MENU_H

#define MENU_HOME_ON_LONG_ESC_CNT 4
#define MENU_HOME_ON_TIMER_SEC 60
#define MENU_MACRO
#define MENU_DESCR_MACRO
#define MENU_DESCRS_ATTRIBS static const
#define MENU_ITEMS_ATTRIBS static
#define MENU_ITEM_ATTRIBS static const

#include "menu/menu.h"
#include "gui/gui_metro.h"
#include "gui/gui_widget.h"
#include "errors/errors.h"
#include "defs/defs.h"
#include "gui/resources/resources_params.h"
#include "gui/menu/menu_explorer.h"
#include "settings.h"
#include "drive_events.h"
#include "gui_icon_conditions.h"
#include "gui/bitmaps/icons_statusbar.h"
#include "gui/resources/resources_colors.h"
#include "localization/localization.h"
#include "translations_ids.h"

typedef struct _Gui_Menu gui_menu_t;

//! Значение идентификатора меню.
#define GUI_MENU_TYPE_ID 120

#define MENU_ITEM_HEIGHT (TFT_HEIGHT - GUI_STATUSBAR_HEIGHT) / (MENU_EXPLORER_DISPLAYED_COUNT + 1)
#define GUI_MENU_TEXT_LEFT_PAD 3
#define GUI_MENU_TEXT_RIGHT_PAD 4
#define GUI_MENU_ICON_LEFT_PAD 25
#define GUI_MENU_TEXT_TOP_PAD 3
#define GUI_MENU_TEXT_BOTTOM_PAD 3
#define GUI_MENU_SCROLL_WIDTH 6

#define GUI_MENU_SUBMENU_PREFIX "►"
#define GUI_MENU_ENUM_VALUE_POSTFIX "↕"
#define GUI_MENU_PASSWORD_ASTERISK "*"
#define GUI_MENU_EVENT_TITLE_SIZE 25
#define GUI_MENU_EVENT_TITLE_FORMAT "%02d.%02d.%02d|%02d:%02d:%02d|#%02d|"
#define GUI_MENU_EVENT_ICONS_COUNT 3
#define GUI_MENU_EVENT_HELP_ICONS_W 3
#define GUI_MENU_EVENT_HELP_ICONS_SEP 2


typedef bool (*gui_event_text_condition_callback_t)();

//! Тип дескриптора текста ошибки (условия отображения).
typedef struct _Gui_Event_Text_Condition {
    gui_event_text_condition_callback_t callback; // функция-условие отображения иконки
    uint32_t param;             // параметр функции условия
    const char* text;               // текст ошибки
} gui_event_text_condition_t;

//! Начинает список условий отображения текста ошибки.
#define GUI_EVENT_TEXT_CONDITIONS(arg_name, arg_count)\
        static const gui_event_text_condition_t arg_name[arg_count] = 

//! Описывает дескриптор условия отображения текста ошибки.
#define GUI_EVENT_TEXT_CONDITION(arg_callback, arg_param, arg_text)\
        { .callback = (gui_event_text_condition_callback_t)arg_callback, .param = (uint32_t)arg_param, .text = arg_text}


typedef bool (*gui_event_icon_condition_callback_t)();

//! Тип дескриптора иконок (условия отображения).
typedef struct _Gui_Event_Icon_Condition {
    gui_event_icon_condition_callback_t callback; // функция-условие отображения иконки
    uint32_t param;             // параметр функции условия
    uint8_t icon;               // идентификатор статической иконки
    graphics_color_t color;     // цвет иконки
} gui_event_icon_condition_t;

//! Начинает список условий отображения иконок.
#define GUI_EVENT_ICON_CONDITIONS(arg_name, arg_count)\
        static const gui_event_icon_condition_t arg_name[arg_count] = 

//! Описывает дескриптор условия отображения иконки.
#define GUI_EVENT_ICON_CONDITION(arg_callback, arg_param, arg_icon, arg_color)\
        { .callback = (gui_event_icon_condition_callback_t)arg_callback, .param = (uint32_t)arg_param, .icon = arg_icon, .color = arg_color}

bool gui_menu_event_error(drive_event_t* event, drive_error_t error);

bool gui_menu_event_warning(drive_event_t* event, drive_warning_t warning);

bool gui_menu_event_power_error (drive_event_t* event, drive_power_error_t error);

bool gui_menu_event_power_warning(drive_event_t* event, drive_power_warning_t warning);

//! Таблица отображения иконок в зависимости от условий
#define GUI_EVENT_ICON_CONDITIONS_COUNT 17
GUI_EVENT_ICON_CONDITIONS(gui_event_icon_conditions, GUI_EVENT_ICON_CONDITIONS_COUNT) {
    // ошибки
    GUI_EVENT_ICON_CONDITION(gui_menu_event_error,         DRIVE_ERROR_ETC,            ICONS_STATUSBAR_VAL_WARNING,    THEME_COLOR_RED_L),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_error,         DRIVE_ERROR_EMERGENCY_STOP, ICONS_STATUSBAR_VAL_EMERGENCY,  THEME_COLOR_RED_L),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_power_error,   DRIVE_POWER_ERROR_Iexc,     ICONS_STATUSBAR_VAL_FAULT_F,    THEME_COLOR_RED_L),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_power_error,   DRIVE_POWER_ERROR_U,        ICONS_STATUSBAR_VAL_FAULT_U,    THEME_COLOR_RED_L),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_power_error,   DRIVE_POWER_ERROR_I,        ICONS_STATUSBAR_VAL_FAULT_I,    THEME_COLOR_RED_L),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_power_error,   DRIVE_POWER_ERROR_Ifan,     ICONS_STATUSBAR_VAL_FAN,        THEME_COLOR_RED_L),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_error,         DRIVE_ERROR_OVERHEAT,       ICONS_STATUSBAR_VAL_OVERHEAT,   THEME_COLOR_RED_L),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_power_error,   DRIVE_POWER_ERROR_Iref,     ICONS_STATUSBAR_VAL_LOOP_BREAK, THEME_COLOR_RED_L),
          
    // предупреждения
    GUI_EVENT_ICON_CONDITION(gui_menu_event_warning,       DRIVE_WARNING_ETC,              ICONS_STATUSBAR_VAL_WARNING,    THEME_COLOR_ORANGE),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_Iexc,       ICONS_STATUSBAR_VAL_FAULT_F,    THEME_COLOR_ORANGE),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_U,          ICONS_STATUSBAR_VAL_FAULT_U,    THEME_COLOR_ORANGE),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_I,          ICONS_STATUSBAR_VAL_FAULT_I,    THEME_COLOR_ORANGE),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_Ifan,       ICONS_STATUSBAR_VAL_FAN,        THEME_COLOR_ORANGE),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_warning,       DRIVE_WARNING_OVERHEAT,         ICONS_STATUSBAR_VAL_OVERHEAT,   THEME_COLOR_ORANGE),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_warning,       DRIVE_WARNING_TRIAC,            ICONS_STATUSBAR_VAL_TRIAC,      THEME_COLOR_ORANGE),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_warning,       DRIVE_WARNING_FAN_FAIL,         ICONS_STATUSBAR_VAL_FAN,        THEME_COLOR_ORANGE),
    GUI_EVENT_ICON_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_Iref,       ICONS_STATUSBAR_VAL_LOOP_BREAK, THEME_COLOR_ORANGE),
};

//! Таблица отображения текста ошибок и предупреждений события в зависимости от условий
#define GUI_EVENT_TEXT_CONDITIONS_COUNT 75
GUI_EVENT_TEXT_CONDITIONS(gui_event_text_conditions, GUI_EVENT_TEXT_CONDITIONS_COUNT) {
    // ошибки
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_error, DRIVE_ERROR_POWER_DATA_NOT_AVAIL, TEXT(TR_ID_DRIVE_ERROR_POWER_DATA_NOT_AVAIL)), //!< Данные питания не поступают с АЦП.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_error, DRIVE_ERROR_POWER_INVALID, TEXT(TR_ID_DRIVE_ERROR_POWER_INVALID)), //!< Неправильные значения питания, см. drive_power_error_t.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_error, DRIVE_ERROR_EMERGENCY_STOP, TEXT(TR_ID_DRIVE_ERROR_EMERGENCY_STOP)), //!< Аварийный останов (Грибок).
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_error, DRIVE_ERROR_PHASE, TEXT(TR_ID_DRIVE_ERROR_PHASE)), //!< Ошибка состояния фаз.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_error, DRIVE_ERROR_PHASE_ANGLE, TEXT(TR_ID_DRIVE_ERROR_PHASE_ANGLE)), //!< Ошибка угла между фазами.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_error, DRIVE_ERROR_PHASE_SYNC, TEXT(TR_ID_DRIVE_ERROR_PHASE_SYNC)), //!< Ошибка синхронизации фаз.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_error, DRIVE_ERROR_THERMAL_OVERLOAD, TEXT(TR_ID_DRIVE_ERROR_THERMAL_OVERLOAD)), //!< Тепловая защита.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_error, DRIVE_ERROR_ROT_BREAK, TEXT(TR_ID_DRIVE_ERROR_ROT_BREAK)), //!< Обрыв якоря.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_error, DRIVE_ERROR_HEATSINK_TEMP, TEXT(TR_ID_DRIVE_ERROR_HEATSINK_TEMP)), //!< Перегрев радиатора.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_error, DRIVE_ERROR_ROT_MEASURE_BREAK, TEXT(TR_ID_DRIVE_ERROR_ROT_MEASURE_BREAK)), //
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_error, DRIVE_ERROR_SENSOR, TEXT(TR_ID_DRIVE_ERROR_SENSOR)), //
    // ошибки питания
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_UNDERFLOW_Ua, TEXT(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Ua)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_OVERFLOW_Ua, TEXT(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Ua)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_UNDERFLOW_Ub, TEXT(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Ub)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_OVERFLOW_Ub, TEXT(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Ub)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_UNDERFLOW_Uc, TEXT(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Uc)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_OVERFLOW_Uc, TEXT(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Uc)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_UNDERFLOW_Urot, TEXT(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Urot)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_OVERFLOW_Urot, TEXT(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Urot)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_UNDERFLOW_Ia, TEXT(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Ia)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_OVERFLOW_Ia, TEXT(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Ia)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_UNDERFLOW_Ib, TEXT(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Ib)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_OVERFLOW_Ib, TEXT(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Ib)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_UNDERFLOW_Ic, TEXT(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Ic)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_OVERFLOW_Ic, TEXT(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Ic)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_UNDERFLOW_Irot, TEXT(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Irot)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_OVERFLOW_Irot, TEXT(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Irot)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_UNDERFLOW_Iexc, TEXT(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Iexc)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_OVERFLOW_Iexc, TEXT(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Iexc)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_UNDERFLOW_Ifan, TEXT(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Ifan)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_OVERFLOW_Ifan, TEXT(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Ifan)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_UNDERFLOW_Iref, TEXT(TR_ID_DRIVE_POWER_ERROR_UNDERFLOW_Iref)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_OVERFLOW_Iref, TEXT(TR_ID_DRIVE_POWER_ERROR_OVERFLOW_Iref)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_IDLE_Ia, TEXT(TR_ID_DRIVE_POWER_ERROR_IDLE_Ia)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_IDLE_Ib, TEXT(TR_ID_DRIVE_POWER_ERROR_IDLE_Ib)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_IDLE_Ic, TEXT(TR_ID_DRIVE_POWER_ERROR_IDLE_Ic)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_IDLE_Urot, TEXT(TR_ID_DRIVE_POWER_ERROR_IDLE_Urot)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_IDLE_Irot, TEXT(TR_ID_DRIVE_POWER_ERROR_IDLE_Irot)),
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_error, DRIVE_POWER_ERROR_IDLE_Iexc, TEXT(TR_ID_DRIVE_POWER_ERROR_IDLE_Iexc)),
    // предупреждения
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_warning, DRIVE_WARNING_POWER, TEXT(TR_ID_DRIVE_WARNING_POWER)),//!< Предупреждение по питанию.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_warning, DRIVE_WARNING_PHASE_ANGLE, TEXT(TR_ID_DRIVE_WARNING_PHASE_ANGLE)),//!< Ошибка угла между фазами.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_warning, DRIVE_WARNING_PHASE_SYNC, TEXT(TR_ID_DRIVE_WARNING_PHASE_SYNC)),//!< Ошибка синхронизации фаз.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_warning, DRIVE_WARNING_THERMAL_OVERLOAD, TEXT(TR_ID_DRIVE_WARNING_THERMAL_OVERLOAD)),//!< Перегрев.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_warning, DRIVE_WARNING_FAN_FAIL, TEXT(TR_ID_DRIVE_WARNING_FAN_FAIL)),//!< Ошибка вентилятора.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_warning, DRIVE_WARNING_HEATSINK_TEMP, TEXT(TR_ID_DRIVE_WARNING_HEATSINK_TEMP)),//!< Перегрев радиатора.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_warning, DRIVE_WARNING_SENSOR, TEXT(TR_ID_DRIVE_WARNING_SENSOR)),//!< Ошибка датчиков.
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_warning, DRIVE_WARNING_TRIAC, TEXT(TR_ID_DRIVE_WARNING_TRIAC)),//!< Ошибка тиристоров.
    // предупреждения питания
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_UNDERFLOW_Ua, TEXT(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Ua)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_OVERFLOW_Ua, TEXT(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Ua)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_UNDERFLOW_Ub, TEXT(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Ub)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_OVERFLOW_Ub, TEXT(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Ub)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_UNDERFLOW_Uc, TEXT(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Uc)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_OVERFLOW_Uc, TEXT(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Uc)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_UNDERFLOW_Urot, TEXT(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Urot)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_OVERFLOW_Urot, TEXT(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Urot)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_UNDERFLOW_Ia, TEXT(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Ia)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_OVERFLOW_Ia, TEXT(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Ia)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_UNDERFLOW_Ib, TEXT(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Ib)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_OVERFLOW_Ib, TEXT(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Ib)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_UNDERFLOW_Ic, TEXT(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Ic)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_OVERFLOW_Ic, TEXT(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Ic)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_UNDERFLOW_Irot, TEXT(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Irot)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_OVERFLOW_Irot, TEXT(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Irot)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_UNDERFLOW_Iexc, TEXT(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Iexc)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_OVERFLOW_Iexc, TEXT(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Iexc)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_UNDERFLOW_Ifan, TEXT(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Ifan)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_OVERFLOW_Ifan, TEXT(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Ifan)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_UNDERFLOW_Iref, TEXT(TR_ID_DRIVE_POWER_WARNING_UNDERFLOW_Iref)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_OVERFLOW_Iref, TEXT(TR_ID_DRIVE_POWER_WARNING_OVERFLOW_Iref)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_IDLE_Ia, TEXT(TR_ID_DRIVE_POWER_WARNING_IDLE_Ia)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_IDLE_Ib, TEXT(TR_ID_DRIVE_POWER_WARNING_IDLE_Ib)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_IDLE_Ic, TEXT(TR_ID_DRIVE_POWER_WARNING_IDLE_Ic)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_IDLE_Urot, TEXT(TR_ID_DRIVE_POWER_WARNING_IDLE_Urot)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_IDLE_Irot, TEXT(TR_ID_DRIVE_POWER_WARNING_IDLE_Irot)),//!< 
    GUI_EVENT_TEXT_CONDITION(gui_menu_event_power_warning, DRIVE_POWER_WARNING_IDLE_Iexc, TEXT(TR_ID_DRIVE_POWER_WARNING_IDLE_Iexc)),//!<        
};

struct _Gui_Menu {
    gui_widget_t super; //!< Суперкласс.
    menu_t menu; //!< Объект меню
    menu_explorer_t explorer; //!< Объект навигации по меню
    uint8_t long_esc_press_cnt; //!< Счетчик длительного нажатия на кнопку Esc
    uint8_t home_on_timer_cnt; //!< Счетчик выхода на главный экран по таймеру
    bool key_down_press; //!< Сотояние кнопки DOWN
    bool key_up_press; //!< Сотояние кнопки UP
    graphics_t* icon_graphics; //!< Ссылка на набор  иконок
    uint8_t icon_count; //!< Количество значков в изображении иконок
    void (*on_home)(gui_menu_t* menu); //!< Каллбэк: выход из меню
};

#define GUI_MENU_ON_HOME_PROC(proc) ((void (*)(gui_menu_t*))proc)

typedef void (*gui_menu_on_home_t)(gui_menu_t* menu);

//! Приводит указатель menu к типу меню.
#define GUI_MENU(menu) ((gui_menu_t*)(menu))

/**
 * Инициализирует меню.
 * @param menu Меню.
 * @param gui Графический интерфейс.
 * @return Код ошибки.
 */
EXTERN err_t gui_menu_init(gui_menu_t* menu, gui_metro_t* gui);

/**
 * Инициализирует menu как потомок заданного родителя.
 * @param menu Меню.
 * @param gui Графический интерфейс.
 * @param parent Родитель.
 * @return Код ошибки.
 */
EXTERN err_t gui_menu_init_parent(gui_menu_t* menu, gui_metro_t* gui, gui_widget_t* parent);

/**
 * Обработчик перерисовки.
 * @param menu Меню.
 * @param rect Область перерисовки.
 */
EXTERN void gui_menu_on_repaint(gui_menu_t* menu, const rect_t* rect);

/**
 * Устанавливает изображение значков.
 */
EXTERN void gui_menu_set_graphics(gui_menu_t* menu, graphics_t* graphics, uint8_t count);

/**
 * Обработчик нажатия клавиши.
 * @param menu Меню.
 * @param key Код клавиши.
 */
EXTERN void gui_menu_on_key_press(gui_menu_t* menu, keycode_t key);

/**
 * Обработчик отпускания клавиши.
 * @param menu Меню.
 * @param key Код клавиши.
 */
EXTERN void gui_menu_on_key_release(gui_menu_t* menu, keycode_t key);

ALWAYS_INLINE static gui_menu_on_home_t gui_menu_on_home(gui_menu_t* menu)
{
    return menu->on_home;
}

ALWAYS_INLINE static void gui_menu_set_on_home(gui_menu_t* menu, gui_menu_on_home_t on_home)
{
    menu->on_home = on_home;
}

/**
 * Выполняет выход на главный экран
 * @param menu
 * @param key
 */
EXTERN void gui_menu_on_home_action(gui_menu_t* menu, keycode_t key);

/**
 * Выполняет выход на главный экран по таймеру
 * @param menu
 */
EXTERN void gui_menu_on_timer_home_action(gui_menu_t* menu);

EXTERN void gui_menu_init_counters(gui_menu_t* menu, gui_metro_t* gui);

int gui_menu_get_f32_fract(int number, int decimals);

void gui_menu_draw_events(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width);

void gui_menu_draw_event_page(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width);

void gui_menu_draw_command_result(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width);

void gui_menu_draw_password_request(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width);

void gui_menu_draw_title(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width);

void gui_menu_draw_scrollbar(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width, graphics_pos_t height);

void gui_menu_draw_item(gui_menu_t* menu, menu_item_t* menu_item, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width, graphics_pos_t height, graphics_pos_t x, graphics_pos_t y);

void gui_menu_draw_event(uint8_t index, gui_menu_t* menu, drive_event_t* event, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width, graphics_pos_t height, graphics_pos_t x, graphics_pos_t y);

void gui_menu_event_title(char* title, size_t size, const char* format, drive_event_t* event);

void gui_menu_draw_help(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width, graphics_pos_t height);

void gui_menu_draw_events_help(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width, graphics_pos_t height);

EXTERN bool gui_menu_string_line_wrapping(painter_t* painter, const char* source, graphics_size_t* sx, graphics_size_t* sy, graphics_size_t width, graphics_size_t height);

void gui_menu_value_to_str(param_t* param, char* str, size_t len);

void gui_menu_param_value_to_string(param_t* param, char* str, int32_t* edit_data);

size_t gui_menu_edit_draw_string(painter_t* painter, graphics_pos_t x, graphics_pos_t y, const char* s, uint8_t decim, bool type_fix);

#endif	/* GUI_MENU_H */
