#include "gui_menu.h"
#include "utils/utils.h"
#include "graphics/painter.h"
#include "graphics/graphics_inlines.h"
#include "tft9341/tft9341.h"

#include "../resources/resources_menu.h"
#include "parameters_menu.h"
#include "parameters_ids.h"
#include "settings.h"
#include "commands.h"
#include "gui/resources/resources_colors.h"
#include "drive_keypad.h"
#include "drive_events.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

//MENU_ITEMS(menu_items, menu_descrs);

err_t gui_menu_init(gui_menu_t* menu, gui_metro_t* gui)
{
    return gui_menu_init_parent(menu, gui, NULL);
}

err_t gui_menu_init_parent(gui_menu_t* menu, gui_metro_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_widget_init_parent(GUI_WIDGET(menu), gui, parent));
    
    //gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(menu)));
    
    GUI_WIDGET(menu)->type_id = GUI_MENU_TYPE_ID;
    GUI_WIDGET(menu)->on_repaint = GUI_WIDGET_ON_REPAINT_PROC(gui_menu_on_repaint);
    GUI_WIDGET(menu)->on_key_press = GUI_WIDGET_ON_KEY_PRESS_PROC(gui_menu_on_key_press);
    GUI_WIDGET(menu)->on_key_release = GUI_WIDGET_ON_KEY_RELEASE_PROC(gui_menu_on_key_release);
    
    gui_menu_init_counters(menu, gui);
    
    //menu_make_from_descrs(&(menu->menu), menu_items, MENU_ITEMS_COUNT(menu_items), menu_descrs, MENU_DESCRS_COUNT(menu_descrs), NULL);
    menu->menu.root = (menu_item_t*)&m_item1;
    menu->menu.current = (menu_item_t*)&m_item1;
    
    menu_explorer_init(&(menu->explorer), &(menu->menu));
    
    return E_NO_ERROR;
}

void gui_menu_init_counters(gui_menu_t* menu, gui_metro_t* gui)
{
    menu->long_esc_press_cnt = 0;
    menu->key_down_press = false;
    menu->key_up_press = false;
    menu->explorer.help = false;
}

void gui_menu_on_repaint(gui_menu_t* menu, const rect_t* rect)
{
    menu->explorer.autoupdate = system_counter_ticks();
    //gui_widget_on_repaint(GUI_WIDGET(menu), rect);
    if (gui_widget_visible(GUI_WIDGET(menu))) {

        gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(menu)));
        painter_t painter;
        
        gui_widget_t* widget = GUI_WIDGET(menu);
        graphics_pos_t width = widget->rect.right - widget->rect.left;
        graphics_pos_t height = widget->rect.bottom - widget->rect.top;

        gui_widget_begin_paint(widget, &painter, rect);

        painter_set_pen(&painter, PAINTER_PEN_SOLID);
        painter_set_brush(&painter, PAINTER_BRUSH_SOLID);
        painter_set_brush_color(&painter, theme->color_menu);
        painter_set_font(&painter, theme->middle_font);
        painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMASK);

        graphics_pos_t text_x, text_y;
         
        if (menu_explorer_state_command_result(&menu->explorer)) { 
            // результат выполнения программы
            gui_menu_draw_command_result(menu, &painter, theme, width);
        }
        else if (menu_explorer_state_password_request(&menu->explorer)) {
            // запрос на редактирование пароля
            gui_menu_draw_password_request(menu, &painter, theme, width);
        }
        else if (menu_explorer_state_events(&menu->explorer)) {
            // история событий
            gui_menu_draw_events(menu, &painter, theme, width);
        }
        else if (menu_explorer_state_event(&menu->explorer)) {
             // событие
            gui_menu_draw_event_page(menu, &painter, theme, width);
        }
        else { // параметры меню
        
            // заголовок меню
            if (menu->explorer.draw_mode & GUI_MENU_DRAW_MODE_TITLE) 
                gui_menu_draw_title(menu, &painter, theme, width);

            // элементы меню
            text_y = MENU_ITEM_HEIGHT;
            int i;
            for (i = 0; i < menu_explorer_displayed_cnt(&menu->explorer); i++) {
                text_x = GUI_MENU_TEXT_LEFT_PAD;

                gui_menu_draw_item(menu, menu->explorer.displayed[i], &painter, theme, width, height, text_x, text_y);

                text_y += MENU_ITEM_HEIGHT;
            }

            // скроллбар
            if (menu->explorer.draw_mode & GUI_MENU_DRAW_MODE_SCROLL) 
                gui_menu_draw_scrollbar(menu, &painter, theme, width, height);

            // справка
            if (menu->explorer.draw_mode & GUI_MENU_DRAW_MODE_HELP)
                gui_menu_draw_help(menu, &painter, theme, width, height);

        }
        
        gui_widget_end_paint(GUI_WIDGET(menu), &painter);
    }
}

void gui_menu_set_graphics(gui_menu_t* menu, graphics_t* graphics, uint8_t count)
{
    menu->icon_graphics = graphics;
    menu->icon_count = count;
    gui_widget_repaint(GUI_WIDGET(menu), NULL);
}

void gui_menu_on_home_action(gui_menu_t* menu, keycode_t key)
{
    menu->on_home(menu);
    menu->long_esc_press_cnt = 0;
    menu->home_on_timer_cnt = 0;
    //menu_explorer_init(&menu->explorer, &menu->menu);
}

void gui_menu_on_timer_home_action(gui_menu_t* menu)
{
    //if (GUI_WIDGET(menu)->focusable) {
        counter_t cur = system_counter_ticks();
        counter_t ticks_per_sec = system_counter_ticks_per_sec();
        // проверка перехода на главный экран
        counter_t reset_time = menu->explorer.touch + ticks_per_sec * MENU_HOME_ON_TIMER_SEC;
        if (cur >= reset_time) {
            gui_menu_on_home_action(menu, 0);
        }
        // проверка сброса предыдущих прав пользователя по таймауту
        reset_time = menu->explorer.touch + ticks_per_sec * MENU_EXPLORER_USER_RESET_SEC;
        if (cur >= reset_time) {
            menu->explorer.user = MENU_USER_NONE;
        }
        // проверка обновления значений пунктов меню
        reset_time = menu->explorer.autoupdate + ticks_per_sec * MENU_EXPLORER_AUTO_UPDATE_VALUES_SEC;
        if (cur >= reset_time) {
            // перерисовываются только автообновляемые значения
            menu->explorer.draw_mode = GUI_MENU_DRAW_MODE_NONE;
            gui_menu_on_repaint(menu, NULL);
        }
    //}
}

bool gui_menu_event_error(drive_event_t* event, drive_error_t error) {
    return (event->errors & error) != 0;
}

bool gui_menu_event_warning(drive_event_t* event, drive_warning_t warning) {
    return (event->warnings & warning) != 0;
}

bool gui_menu_event_power_error (drive_event_t* event, drive_power_error_t error) {
    return (event->power_errors & error) != 0;
}

bool gui_menu_event_power_warning(drive_event_t* event, drive_power_warning_t warning) {
    return (event->power_warnings & warning) != 0;
}

void gui_menu_draw_event(uint8_t index, gui_menu_t* menu, drive_event_t* event, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width, graphics_pos_t height, graphics_pos_t x, graphics_pos_t y)
{
    gui_menu_draw_mode_t mode = menu->explorer.draw_mode;
    
    bool item_is_selected = (index == menu->explorer.sel_pos);
    bool mode_is_lines = ((mode & GUI_MENU_DRAW_MODE_LINES) > 0);
    bool item_is_null = false;
    bool item_is_submenu = true;
    
    graphics_color_t color_menu_font = THEME_COLOR_GRAY;
    graphics_color_t color_menu_back = THEME_COLOR_WHITE;
    graphics_pos_t text_x = x;
    graphics_pos_t text_y = y;
    
    //bool item_is_autoupdate = (menu->explorer.sel_object->flags & MENU_FLAG_VALUE);
    //bool mode_is_values = ((mode & GUI_MENU_DRAW_MODE_VALUES) > 0) || item_is_autoupdate;
    
    
    if (event->type == DRIVE_EVENT_TYPE_ERROR) {
        if (item_is_selected) {
            color_menu_back = THEME_COLOR_RED_D;
            color_menu_font = THEME_COLOR_WHITE;
        }
        else {
            color_menu_font = THEME_COLOR_RED_D;
            color_menu_back = THEME_COLOR_WHITE;
        }
    }
    else if (event->type == DRIVE_EVENT_TYPE_WARNING) {
         if (item_is_selected) {
            color_menu_back = THEME_COLOR_ORANGE;
            color_menu_font = THEME_COLOR_WHITE;
        }
        else {
            color_menu_font = THEME_COLOR_ORANGE;
            color_menu_back = THEME_COLOR_WHITE;
        }
    }
    else {
        if (item_is_selected) {
            color_menu_font = THEME_COLOR_WHITE;
            color_menu_back = THEME_COLOR_GRAY;
        }
    }
    
    painter_set_pen_color(painter, color_menu_font);
    painter_set_brush_color(painter, color_menu_back);
    // фон элемента меню
    if (mode_is_lines) {
        painter_draw_fillrect(painter, 0, text_y, width, text_y + MENU_ITEM_HEIGHT);
    }
    // пустой элемент меню
    if (item_is_null) { 
        if (mode_is_lines)
            painter_draw_string(painter, text_x, text_y, " ");
    }
    // префикс подменю
    if (item_is_submenu) { 
        if (mode_is_lines) 
            painter_draw_string(painter, text_x, text_y, GUI_MENU_SUBMENU_PREFIX);
        text_x = GUI_MENU_ICON_LEFT_PAD;
    }
    
    // наименование пункта (события)
    if (mode_is_lines) {
        char time_str[GUI_MENU_EVENT_TITLE_SIZE];
        gui_menu_event_title(time_str, GUI_MENU_EVENT_TITLE_SIZE, GUI_MENU_EVENT_TITLE_FORMAT, event);

        graphics_size_t w;
        painter_string_size(painter, time_str, &w, NULL);
        painter_draw_string(painter, text_x, text_y, time_str);
        
        text_x += w;
    }
    
    // иконки
    if (mode_is_lines) {
        text_y += 2;
        size_t icon_width = (menu->icon_graphics->width / menu->icon_count);
        size_t icon_height = menu->icon_graphics->height;
        uint8_t k = GUI_MENU_EVENT_ICONS_COUNT;
        int i;
        for (i = 0; i < GUI_EVENT_ICON_CONDITIONS_COUNT; i++) {
            if (gui_event_icon_conditions[i].callback(event, gui_event_icon_conditions[i].param)) {
                if (item_is_selected) {
                    color_menu_back = gui_event_icon_conditions[i].color;
                } 
                else {
                    color_menu_font = gui_event_icon_conditions[i].color;
                }
                painter_set_pen_color(painter, color_menu_font);
                painter_set_brush_color(painter, color_menu_back);
                uint8_t val = gui_event_icon_conditions[i].icon;
                if (val != ICONS_STATUSBAR_VAL_NOTHING) {
                    painter_bitblt(painter, text_x, text_y, menu->icon_graphics, icon_width * val, 0, icon_width, icon_height);
                }
                else {
                    painter_draw_fillrect(painter, text_x, text_y, text_x + icon_width, text_y + icon_height);
                }
                
                if (--k <= 0) break;
                text_x += icon_width;
                if (text_x > TFT_WIDTH) break;
            }
        }
    }
        
}



void gui_menu_event_title(char* title, size_t size, const char* format, drive_event_t* event)
{
    int epoch_time = (int)event->time;
    time_t epoch_time_as_time_t = epoch_time;
    struct tm * timeinfo;
    timeinfo = localtime(&epoch_time_as_time_t);
    
    snprintf(title, size, format, timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year % 100, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, (int)event->id);
}

drive_event_type_t gui_menu_events_max_level(void)
{
    drive_event_type_t type = DRIVE_EVENT_TYPE_STATUS;
    size_t cnt = drive_events_count();
    if (cnt > 0) {
        int i;
        for (i = 0; i < cnt; i++) {
            drive_event_t event;
            drive_event_index_t id = drive_events_index_by_number((size_t)i);
            
            err_t err = E_NO_ERROR;
            do {
                err = drive_events_read_event((drive_event_t*)&event, id);
            } while(err == E_BUSY);
            
            if (err == E_NO_ERROR) {
                if (event.type == DRIVE_EVENT_TYPE_ERROR) {
                    type = DRIVE_EVENT_TYPE_ERROR;
                    break;
                } else if (event.type == DRIVE_EVENT_TYPE_WARNING) {
                    type = DRIVE_EVENT_TYPE_WARNING;
                }
            }
        }
    }
    return type;
}

void gui_menu_draw_event_page(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width)
{
    gui_widget_t* widget = GUI_WIDGET(menu);
    graphics_pos_t height = widget->rect.bottom - widget->rect.top;
    
    graphics_pos_t start_x, text_x, text_y;
    start_x = 10;
    const char* title = TR(TR_ID_MENU_EVENTS);
    
    // количество событий в памяти
    size_t cnt = menu->explorer.count;
    
    painter_set_pen_color(painter, theme->color_menu_font);
    painter_set_brush_color(painter, theme->color_menu);
    
    menu_explorer_t* explorer = &menu->explorer;
    
    gui_menu_draw_mode_t mode = explorer->draw_mode;
    bool mode_is_lines = ((mode & GUI_MENU_DRAW_MODE_LINES) > 0);
    
    text_y = MENU_ITEM_HEIGHT;
    if (cnt > 0) { // есть события
        drive_event_t event;       
        int8_t number = cnt - 1 - explorer->item_pos;
        if (number >= 0) {
            drive_event_index_t id = drive_events_index_by_number((size_t)number);

            err_t err = E_NO_ERROR;
            do {
                err = drive_events_read_event((drive_event_t*)&event, id);
            } while(err == E_BUSY);

            if (err == E_NO_ERROR) {
                graphics_color_t color_title_back = theme->color_menu_title;
                graphics_color_t color_title_font = theme->color_menu_title_font;
                graphics_color_t color_menu_font = THEME_COLOR_WHITE;
                graphics_color_t color_menu_back = THEME_COLOR_GRAY;
                
                const char* text_status = TR(TR_ID_MENU_EVENT_STATUS);
                const char* text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_INIT);
                
                if (event.type == DRIVE_EVENT_TYPE_ERROR) {
                    color_title_font = THEME_COLOR_WHITE;
                    color_title_back = THEME_COLOR_RED_L;
                    color_menu_back = THEME_COLOR_RED_D;
                    color_menu_font = THEME_COLOR_WHITE;
                    text_status = TR(TR_ID_MENU_EVENT_ERROR);
                }
                else if (event.type == DRIVE_EVENT_TYPE_WARNING) {
                    color_title_font = THEME_COLOR_BLACK;
                    color_title_back = THEME_COLOR_YELLOW;
                    color_menu_back = THEME_COLOR_ORANGE;
                    color_menu_font = THEME_COLOR_WHITE;
                    text_status = TR(TR_ID_MENU_EVENT_WARNING);
                }

                // заголовок меню
                if (menu->explorer.draw_mode & GUI_MENU_DRAW_MODE_TITLE) {
                    char title[GUI_MENU_EVENT_TITLE_SIZE];
                    gui_menu_event_title(title, GUI_MENU_EVENT_TITLE_SIZE, GUI_MENU_EVENT_TITLE_FORMAT, &event);

                    painter_string_size(painter, title, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);

                    painter_set_pen_color(painter, color_title_font);
                    painter_set_brush_color(painter, color_title_back);
                    // наименование пункта меню
                    painter_draw_fillrect(painter, 0, 0, width, MENU_ITEM_HEIGHT);
                    text_x = (width - text_x) / 2;
                    text_y = 0;//((graphics_pos_t)gui_widget_height(GUI_WIDGET(label)) - text_y) / 2;
                    painter_draw_string(painter, text_x, text_y, title);
                }
                
                text_x = start_x;
                text_y = MENU_ITEM_HEIGHT;
                
                painter_set_font(painter, theme->small_font);
                painter_set_pen_color(painter, color_menu_font);
                painter_set_brush_color(painter, color_menu_back);
                // фон элемента
                if (mode_is_lines) {
                    painter_draw_fillrect(painter, 0, text_y, width, height);
                }

                text_y += 5;
                
                switch(event.state) {
                    case DRIVE_STATE_INIT: //!< Инициализация.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_INIT);
                        break;
                    case DRIVE_STATE_CALIBRATION: //!< Калибровка питания.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_CALIBRATION);
                        break;
                    case DRIVE_STATE_IDLE: //!< Простой (готовность).
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_IDLE);
                        break;
                    case DRIVE_STATE_STOP: //!< Останов.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_STOP);
                        break;
                    case DRIVE_STATE_START: //!< Запуск.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_START);
                        break;
                    case DRIVE_STATE_RUN: //!< Работа.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_RUN);
                        break;
                    case DRIVE_STATE_STOP_ERROR: //!< Останов при ошибке.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_STOP_ERROR);
                        break;
                    case DRIVE_STATE_ERROR:  //!< Ошибка.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_ERROR);
                        break;
                }

                /*
                // иконки
                text_x = start_x + GUI_MENU_EVENT_HELP_ICONS_SEP;
                text_y = start_y + GUI_MENU_EVENT_HELP_ICONS_SEP;
                size_t icon_width = (menu->icon_graphics->width / menu->icon_count);
                size_t icon_height = menu->icon_graphics->height;
                uint8_t j = 1;
                int i;
                for (i = 0; i < GUI_EVENT_ICON_CONDITIONS_COUNT; i++) {
                    if (gui_event_icon_conditions[i].callback(&event, gui_event_icon_conditions[i].param)) {
                        painter_set_brush_color(painter, gui_event_icon_conditions[i].color);
                        uint8_t val = gui_event_icon_conditions[i].icon;
                        if (val != ICONS_STATUSBAR_VAL_NOTHING) {
                            painter_bitblt(painter, text_x, text_y, menu->icon_graphics, icon_width * val, 0, icon_width, icon_height);
                        }
                        else {
                            painter_draw_fillrect(painter, text_x, text_y, text_x + icon_width, text_y + icon_height);
                        }

                        text_x += icon_width + GUI_MENU_EVENT_HELP_ICONS_SEP;
                        if (j == GUI_MENU_EVENT_HELP_ICONS_W) {
                            text_y += icon_height + GUI_MENU_EVENT_HELP_ICONS_SEP;
                            text_x = start_x + GUI_MENU_EVENT_HELP_ICONS_SEP;
                        }
                        if (j >= GUI_MENU_EVENT_HELP_ICONS_W * 2) break;
                        j++;
                    }
                }

                */
                
                painter_set_pen_color(painter, color_menu_font);
                painter_set_brush_color(painter, color_menu_back);

                // text_x = start_x + (icon_width + GUI_MENU_EVENT_HELP_ICONS_SEP) * GUI_MENU_EVENT_HELP_ICONS_W + 5;
                // text_y = start_y + GUI_MENU_EVENT_HELP_ICONS_SEP;

                graphics_size_t item_h = theme->small_font->char_height;
                
                // Тип события
                painter_draw_string(painter, text_x, text_y, text_status);
                text_y += item_h;
                
                // Состояние привода
                painter_draw_string(painter, text_x, text_y, text_state);
                text_y += item_h;
                
                // Состояние привода
                const char* ref_str = TR(TR_ID_MENU_EVENT_DRIVE_REFERENCE_FORMAT);
                painter_draw_string(painter, text_x, text_y, ref_str);
                graphics_size_t w;
                painter_string_size(painter, ref_str, &w, NULL);
                text_x += w;
                char str[5];
                snprintf(str, 5, "%d%%", (int)event.reference);
                painter_draw_string(painter, text_x, text_y, str); 
                text_y += item_h;
                
                // Список ошибок и предупреждений
                // Ошибки event.errors
                // Ошибки питания event.power_errors
                // Предупреждения event.warnings  
                // Предупреждения питания event.power_warnings
                text_x = start_x;
                int i;
                for (i = 0; i < GUI_EVENT_TEXT_CONDITIONS_COUNT; i++) {
                    if (gui_event_text_conditions[i].callback(&event, gui_event_text_conditions[i].param)) {
                        painter_draw_string(painter, text_x, text_y, TR(gui_event_text_conditions[i].text)); 
                        text_y += item_h;
                    }
                }
            }
        }
    }
    else { // нет событий
        if (mode_is_lines) {
            title = TR(TR_ID_MENU_EVENTS_NONE);
            painter_draw_fillrect(painter, 0, MENU_ITEM_HEIGHT, width, TFT_HEIGHT);

            painter_string_size(painter, title, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);
            text_x = (width - text_x) / 2;
            text_y = 4 * MENU_ITEM_HEIGHT;
            painter_draw_string(painter, text_x, text_y, title);
        }
    }
}

void gui_menu_draw_events(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width)
{
    gui_widget_t* widget = GUI_WIDGET(menu);
    graphics_pos_t height = widget->rect.bottom - widget->rect.top;
    
    graphics_pos_t text_x, text_y;
    const char* title = TR(TR_ID_MENU_EVENTS);
       
    graphics_color_t color_title_back = theme->color_menu_title;
    graphics_color_t color_title_font = theme->color_menu_title_font;
    
    if (menu->explorer.sel_object != NULL) {
        title = TR(menu->explorer.sel_object->text);
    }
    // заголовок меню
    if (menu->explorer.draw_mode & GUI_MENU_DRAW_MODE_TITLE) {
        
        drive_event_type_t events_max_level = gui_menu_events_max_level();
        if (events_max_level == DRIVE_EVENT_TYPE_ERROR) {
            color_title_font = THEME_COLOR_WHITE;
            color_title_back = THEME_COLOR_RED_L;
        }
        else if (events_max_level == DRIVE_EVENT_TYPE_WARNING) {
            color_title_font = THEME_COLOR_BLACK;
            color_title_back = THEME_COLOR_YELLOW;
        }
        
        painter_string_size(painter, title, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);

        painter_set_pen_color(painter, color_title_font);
        painter_set_brush_color(painter, color_title_back);
        // наименование пункта меню
        painter_draw_fillrect(painter, 0, 0, width, MENU_ITEM_HEIGHT);
        text_x = (width - text_x) / 2;
        text_y = 0;//((graphics_pos_t)gui_widget_height(GUI_WIDGET(label)) - text_y) / 2;
        painter_draw_string(painter, text_x, text_y, title);
    }
    
    // количество событий в памяти
    size_t cnt = menu->explorer.count;
    
    painter_set_pen_color(painter, theme->color_menu_font);
    painter_set_brush_color(painter, theme->color_menu);
    
    menu_explorer_t* explorer = &menu->explorer;
    
    gui_menu_draw_mode_t mode = explorer->draw_mode;
    bool mode_is_lines = ((mode & GUI_MENU_DRAW_MODE_LINES) > 0);
    
    if (cnt > 0) { // есть события
        text_y = MENU_ITEM_HEIGHT;
        int i;
        for (i = 0; i < menu_explorer_displayed_cnt(explorer); i++) {
            text_x = GUI_MENU_TEXT_LEFT_PAD;
            
            drive_event_t event;
            
            int8_t number = cnt - i - 1 - explorer->item_pos + explorer->sel_pos;
            if (number >= 0) {
                drive_event_index_t id = drive_events_index_by_number((size_t)number);

                err_t err = E_NO_ERROR;
                do {
                    err = drive_events_read_event((drive_event_t*)&event, id);
                } while(err == E_BUSY);
                
                if (err == E_NO_ERROR) {
                    gui_menu_draw_event(i, menu, &event, painter, theme, width, height, text_x, text_y);
                }
            }
            else {
                if (mode_is_lines) {
                    painter_set_pen_color(painter, theme->color_menu_active);
                    painter_set_brush_color(painter, theme->color_menu);
                    painter_draw_fillrect(painter, 0, text_y, width, text_y + MENU_ITEM_HEIGHT);
                }
            }
            
            text_y += MENU_ITEM_HEIGHT;
        }
    }
    else { // нет событий
        if (mode_is_lines) {
            title = TR(TR_ID_MENU_EVENTS_NONE);
            painter_draw_fillrect(painter, 0, MENU_ITEM_HEIGHT, width, TFT_HEIGHT);

            painter_string_size(painter, title, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);
            text_x = (width - text_x) / 2;
            text_y = 4 * MENU_ITEM_HEIGHT;
            painter_draw_string(painter, text_x, text_y, title);
        }
    }
    
    // скроллбар
    if (menu->explorer.draw_mode & GUI_MENU_DRAW_MODE_SCROLL) 
        gui_menu_draw_scrollbar(menu, painter, theme, width, height);
    
    // краткое описание события
    if (menu->explorer.draw_mode & GUI_MENU_DRAW_MODE_HELP)
        gui_menu_draw_events_help(menu, painter, theme, width, height);
}

void gui_menu_draw_command_result(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width)
{
    graphics_pos_t text_x, text_y;
    const char* title = TR(TR_ID_MENU_HEADER_TITLE);
    if (menu->explorer.sel_object != NULL) {
        title = TR(menu->explorer.sel_object->text);
    }
    painter_string_size(painter, title, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);

    if (menu->explorer.state == MENU_EXPLORER_STATE_CMD_FAIL) {
        painter_set_pen_color(painter, THEME_COLOR_WHITE);
        painter_set_brush_color(painter, THEME_COLOR_RED_L);
    }
    else {
        painter_set_pen_color(painter, THEME_COLOR_BLACK);
        painter_set_brush_color(painter, THEME_COLOR_GREEN_L);
    }

    // наименование пункта меню с запросом доступа
    painter_draw_fillrect(painter, 0, 0, width, MENU_ITEM_HEIGHT);
    text_x = (width - text_x) / 2;
    text_y = 0;//((graphics_pos_t)gui_widget_height(GUI_WIDGET(label)) - text_y) / 2;
    painter_draw_string(painter, text_x, text_y, title);

    // строка сообщение запроса доступа (пароля)
    if (menu->explorer.state == MENU_EXPLORER_STATE_CMD_FAIL) {
        painter_set_pen_color(painter, THEME_COLOR_WHITE);
        painter_set_brush_color(painter, THEME_COLOR_RED_D);
        title = TR(TR_ID_MENU_COMMAND_RESULT_FAIL);
    }
    else {
        painter_set_pen_color(painter, THEME_COLOR_WHITE);
        painter_set_brush_color(painter, THEME_COLOR_GREEN_D);
        title = TR(TR_ID_MENU_COMMAND_RESULT_SUCCESS);
    }
    painter_draw_fillrect(painter, 0, MENU_ITEM_HEIGHT, width, TFT_HEIGHT);
  
    painter_string_size(painter, title, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);
    text_x = (width - text_x) / 2;
    text_y = 3 * MENU_ITEM_HEIGHT;
    painter_draw_string(painter, text_x, text_y, title);

    // строка "Нажмите Esc чтобы выйти"
    painter_set_font(painter, theme->middle_font);
    painter_set_pen_color(painter, THEME_COLOR_GRAY_L);
    title = TR(TR_ID_MENU_PASSWORD_REQUEST_ESC);
    painter_string_size(painter, title, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);
    text_x = (width - text_x) / 2;
    text_y = 5 * MENU_ITEM_HEIGHT;
    painter_draw_string(painter, text_x, text_y, title);
}

void gui_menu_draw_password_request(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width)
{
    graphics_pos_t text_x, text_y;
    const char* title = TR(TR_ID_MENU_HEADER_TITLE);
    if (menu->explorer.draw_mode & GUI_MENU_DRAW_MODE_PASSWORD) {
        if (menu->explorer.sel_object != NULL) {
            title = TR(menu->explorer.sel_object->text);
        }
        painter_string_size(painter, title, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);

        painter_set_pen_color(painter, THEME_COLOR_WHITE);
        painter_set_brush_color(painter, THEME_COLOR_RED_L);

        // наименование пункта меню с запросом доступа
        painter_draw_fillrect(painter, 0, 0, width, MENU_ITEM_HEIGHT);
        text_x = (width - text_x) / 2;
        text_y = 0;//((graphics_pos_t)gui_widget_height(GUI_WIDGET(label)) - text_y) / 2;
        painter_draw_string(painter, text_x, text_y, title);
    
        // строка сообщение запроса доступа (пароля)
        painter_set_brush_color(painter, THEME_COLOR_RED_D);
        painter_draw_fillrect(painter, 0, MENU_ITEM_HEIGHT, width, TFT_HEIGHT);
        title = TR(TR_ID_MENU_PASSWORD_REQUEST);
        painter_string_size(painter, title, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);
        text_x = (width - text_x) / 2;
        text_y = 2 * MENU_ITEM_HEIGHT;
        painter_draw_string(painter, text_x, text_y, title);
    }
    
    if (menu->explorer.draw_mode & (GUI_MENU_DRAW_MODE_PASSWORD | GUI_MENU_DRAW_MODE_PASSWORD_VALUE)) {
        // Отображение пароля
        painter_set_font(painter, theme->big_font);
        
        graphics_pos_t dx, dy, delta;
        delta = 10;
        painter_string_size(painter, GUI_MENU_PASSWORD_ASTERISK, (graphics_size_t*)&dx, (graphics_size_t*)&dy);
        text_y = 3.5 * MENU_ITEM_HEIGHT;
        text_x = width / (menu->explorer.edit_param_decim + 1) - dx / 2;
        int i;
        for (i = 0; i < menu->explorer.edit_param_decim; i++) {
            if (menu->explorer.edit_param_curdecim == i) {
                char str[2];
                snprintf(str, 2, "%d", (int)menu->explorer.edit_param_curdecim_val);
                painter_set_brush_color(painter, THEME_COLOR_WHITE);
                painter_set_pen_color(painter, THEME_COLOR_RED_D);
                painter_draw_fillrect(painter, text_x - delta / 2, text_y - delta / 2, text_x + dx + delta, text_y + dy + delta / 2);
                painter_draw_string(painter, text_x, text_y, str);
            }
            else {
                painter_set_brush_color(painter, THEME_COLOR_RED_D);
                painter_set_pen_color(painter, THEME_COLOR_WHITE);
                painter_draw_fillrect(painter, text_x - delta / 2, text_y - delta / 2, text_x + dx + delta, text_y + dy + delta / 2);
                painter_draw_string(painter, text_x, text_y, GUI_MENU_PASSWORD_ASTERISK);
            }
            text_x += width / (menu->explorer.edit_param_decim + 1);
        }
    }

    if (menu->explorer.draw_mode & GUI_MENU_DRAW_MODE_PASSWORD) {
        // строка "Нажмите Esc чтобы выйти"
        painter_set_font(painter, theme->middle_font);
        painter_set_brush_color(painter, THEME_COLOR_RED_D);
        painter_set_pen_color(painter, THEME_COLOR_GRAY_L);
        title = TR(TR_ID_MENU_PASSWORD_REQUEST_ESC);
        painter_string_size(painter, title, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);
        text_x = (width - text_x) / 2;
        text_y = 6 * MENU_ITEM_HEIGHT;
        painter_draw_string(painter, text_x, text_y, title);
    }
}

void gui_menu_draw_title(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width)
{
    graphics_pos_t text_x, text_y;
    const char* title = TR(TR_ID_MENU_HEADER_TITLE);
    if (menu->explorer.sel_object->parent != NULL) {
        title = TR(menu->explorer.sel_object->parent->text);
    }
    painter_string_size(painter, title, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);

    painter_set_pen_color(painter, theme->color_menu_title_font);
    painter_set_brush_color(painter, theme->color_menu_title);

    painter_draw_fillrect(painter, 0, 0, width, MENU_ITEM_HEIGHT);
    text_x = (width - text_x) / 2;
    text_y = 0;//((graphics_pos_t)gui_widget_height(GUI_WIDGET(label)) - text_y) / 2;
    painter_draw_string(painter, text_x, text_y, title);
}

void gui_menu_draw_scrollbar(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width, graphics_pos_t height)
{
    if (menu->explorer.count > menu_explorer_displayed_cnt(&menu->explorer)) {
        painter_set_pen_color(painter, theme->color_scroll);
        graphics_pos_t x = width - GUI_MENU_TEXT_RIGHT_PAD;
        graphics_pos_t y0 = GUI_MENU_TEXT_TOP_PAD + MENU_ITEM_HEIGHT;
        graphics_pos_t y1 = height - GUI_MENU_TEXT_BOTTOM_PAD;
        if (menu_explorer_help_visible(&menu->explorer)) {
            y1 -= (MENU_ITEM_HEIGHT * 2);
        }
        graphics_pos_t scroll_max_height = y1 - y0;
        painter_draw_vline(painter, x, y0, y1);
        painter_draw_vline(painter, ++x, y0, y1);
        //
        graphics_pos_t scroll_item_height = scroll_max_height / menu->explorer.count; 
        graphics_pos_t scroll_height = menu_explorer_displayed_cnt(&menu->explorer) * scroll_item_height;
        if (scroll_height < GUI_MENU_SCROLL_WIDTH) scroll_height = GUI_MENU_SCROLL_WIDTH;
        graphics_pos_t scroll_start = scroll_item_height * (menu->explorer.item_pos - menu->explorer.sel_pos);
        if (scroll_start < 0) scroll_start = 0;
        if (scroll_start > (scroll_max_height - GUI_MENU_SCROLL_WIDTH)) scroll_start = (scroll_max_height - GUI_MENU_SCROLL_WIDTH);
        painter_draw_vline(painter, ++x, y0 + scroll_start, y0 + scroll_start + scroll_height);
        painter_draw_vline(painter, ++x, y0 + scroll_start + 1, y0 + scroll_start + scroll_height - 1);
        x -= 4;
        painter_draw_vline(painter, x, y0 + scroll_start, y0 + scroll_start + scroll_height);
        painter_draw_vline(painter, --x, y0 + scroll_start + 1, y0 + scroll_start + scroll_height - 1);
    }
}

void gui_menu_draw_item(gui_menu_t* menu, menu_item_t* menu_item, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width, graphics_pos_t height, graphics_pos_t x, graphics_pos_t y)
{
    gui_menu_draw_mode_t mode = menu->explorer.draw_mode;
    param_t* param = NULL;
    bool item_not_null = (menu_item != NULL);
    bool item_is_null = !item_not_null;
    bool item_is_selected = false;
    bool item_is_submenu = false;
    bool item_is_param = false;
    bool item_is_command = false;
    bool item_is_enum = false;
    bool item_is_string = false;
    bool mode_is_lines = ((mode & GUI_MENU_DRAW_MODE_LINES) > 0);
    bool mode_is_values = false;
    bool mode_is_edit = false;
    bool item_is_autoupdate = false;
    bool item_is_events = false;
    if (item_not_null) {
        item_is_selected = menu_explorer_is_selected(&menu->explorer, menu_item);
        item_is_events = (menu_item->flags & MENU_FLAG_EVENTS);
        item_is_submenu = menu_item_childs_count(menu_item) > 0 || item_is_events;
        item_is_param = !item_is_submenu;
        if (item_is_param) {
            param = settings_param_by_id(menu_item->id);
            item_is_command = (menu_item->flags & MENU_FLAG_CMD);
            item_is_autoupdate = (menu_item->flags & MENU_FLAG_VALUE);
            item_is_param = (param != NULL);
            mode_is_values = ((mode & GUI_MENU_DRAW_MODE_VALUES) > 0) || item_is_autoupdate;
            if (item_is_selected) {
                if (menu_explorer_state_edit(&menu->explorer)) 
                    mode_is_edit = ((mode & GUI_MENU_DRAW_MODE_EDIT) > 0);
            }
        }
        if (!item_is_submenu && !item_is_command && (menu_item->value != NULL)) {
            item_is_enum = item_is_param && (menu_item->value->type == MENU_VALUE_TYPE_ENUM);
            item_is_string = (menu_item->value->type == MENU_VALUE_TYPE_STRING);
        }
    }
    
    bool is_param_fix = false;
    if (item_is_param) {
        param_type_t param_type = settings_param_type(param);
        is_param_fix = (param_type == PARAM_TYPE_FRACT_10) \
                || (param_type == PARAM_TYPE_FRACT_100) \
                || (param_type == PARAM_TYPE_FRACT_1000) \
                || (param_type == PARAM_TYPE_FRACT_10000);
    }
    
    graphics_color_t color_menu_font = theme->color_menu_font;
    graphics_color_t color_menu_back = theme->color_menu;
    graphics_color_t color_value_font = theme->color_menu_font;
    graphics_color_t color_value_back = theme->color_menu;
    graphics_pos_t text_x = x;
    graphics_pos_t text_y = y;
    graphics_pos_t text_unit;
    text_unit = 0;
    
    if (item_is_selected) {
        color_menu_font = theme->color_menu_active_font;
        color_menu_back = theme->color_menu_active;
        color_value_font = theme->color_menu_active_font;
        color_value_back = theme->color_menu_active;
        if (menu_explorer_state_edit(&menu->explorer)) {
            color_value_font = theme->color_menu_edit_active;
            color_value_back = theme->color_menu_edit;
        }
    }
    // фон элемента меню
    if (mode_is_lines) {
        painter_set_pen_color(painter, color_menu_font);
        painter_set_brush_color(painter, color_menu_back);
        painter_draw_fillrect(painter, 0, text_y, width, text_y + MENU_ITEM_HEIGHT);
    }
    // пустой элемент меню
    if (item_is_null) { 
        if (mode_is_lines)
            painter_draw_string(painter, text_x, text_y, " ");
    }
    // префикс подменю
    if (item_is_submenu) { 
        if (mode_is_lines) 
            painter_draw_string(painter, text_x, text_y, GUI_MENU_SUBMENU_PREFIX);
        text_x = GUI_MENU_ICON_LEFT_PAD;
    }
    // текст меню
    if (item_not_null) {
        if (mode_is_lines) {
            painter_draw_string(painter, text_x, text_y, TR(menu_item->text));
            if (item_is_events) { 
                // для пункта Список событий кол-во событий
                char str[9];
                size_t cnt = drive_events_count();
                snprintf(str, 9, "(%d)", (int)(cnt));

                graphics_size_t wid;
                painter_string_size(painter, str, &wid, NULL);
                text_x = width - wid - 10;

                painter_draw_string(painter, text_x, text_y, str);
            }
        }
    }
    if (item_is_enum || item_is_string) { // перечисление
        const char* str = "";
        graphics_size_t wid = 0;
        text_x = width - 25;
        if (item_is_enum) {
            size_t val_cnt = menu_value_enum_count(menu_item->value);
            menu_value_t* values = menu_value_enum_values(menu_item->value);

            uint32_t val = settings_param_valueu(param);
            if (item_is_selected && menu_explorer_state_edit(&menu->explorer)) {
                // редактируемое значение
                val = menu->explorer.edit_param;
            }
            if (val >= val_cnt) val = val_cnt - 1;

            int j;
            for (j = 0; j < val_cnt; j++) {
                graphics_size_t w;
                str = TR(menu_value_string(&values[j]));
                painter_string_size(painter, str, &w, NULL);
                if (w > wid) wid = w;
            }

            str = TR(menu_value_string(&values[val]));
        }
        else if (item_is_string) {
            str = menu_value_string(menu_item->value);
            painter_string_size(painter, str, &wid, NULL);
        }

        graphics_size_t w;
        painter_string_size(painter, str, &w, NULL);

        text_x = text_x - wid;

        painter_set_pen_color(painter, color_value_font);
        painter_set_brush_color(painter, color_value_back);
        if (mode_is_values || mode_is_edit)
            painter_draw_fillrect(painter, text_x - 12, text_y, text_x + wid + 18, text_y + MENU_ITEM_HEIGHT);

        text_x = width - 22;

        if (item_is_selected && !item_is_autoupdate) {
            painter_set_pen_color(painter, THEME_COLOR_GRAY_L);
            if (mode_is_values || mode_is_edit)
                painter_draw_string(painter, text_x, text_y, GUI_MENU_ENUM_VALUE_POSTFIX);
            painter_set_pen_color(painter, color_value_font);
        }
        text_x = width - 25 - w;
        if (mode_is_values || mode_is_edit)
            painter_draw_string(painter, text_x, text_y, str);
    }
    else if (item_is_param)  { // параметр
        param_units_t unit = settings_param_units(param);
        text_x = width - 8;
        if (unit != NULL) {
            text_x = text_x - 40;
        }
        text_unit = text_x;

        char str[9];
        if (item_is_selected && menu_explorer_state_edit(&menu->explorer)) {
            // редактируемое значение
            gui_menu_param_value_to_string(param, str, &menu->explorer.edit_param);
        }
        else {
            // отображение для чтения
            gui_menu_param_value_to_string(param, str, NULL);
        }


        if (unit != NULL) {
            const char* unit_text = TR(unit);
            if (mode_is_lines) {
                painter_draw_fillrect(painter, text_unit - 3, text_y, width , text_y + MENU_ITEM_HEIGHT);
                painter_draw_string(painter, text_unit, text_y, unit_text);
            }
        }

        graphics_size_t wid;
        painter_string_size(painter, str, &wid, NULL);
        text_x = text_x - wid - 10;

        painter_set_pen_color(painter, color_value_font);
        painter_set_brush_color(painter, color_value_back);
        if (mode_is_values || mode_is_edit) {
            painter_draw_fillrect(painter, text_x - 6, text_y, text_unit - 3, text_y + MENU_ITEM_HEIGHT);
            if (item_is_selected && menu_explorer_state_edit(&menu->explorer)) {
                gui_menu_edit_draw_string(painter, text_x, text_y, str, menu->explorer.edit_param_curdecim, is_param_fix);
            }
            else {
                painter_draw_string(painter, text_x, text_y, str);
            }
        }
    }
}

void gui_menu_draw_help(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width, graphics_pos_t height)
{
    if (menu_explorer_help_visible(&menu->explorer)) {
        //param_t* param = NULL;
        const char* note = NULL;
        if (menu->explorer.sel_object != NULL) {
            //param = settings_param_by_id(menu->explorer.sel_object->id);
            note = menu_item_help(menu->explorer.sel_object);
        }
        graphics_pos_t y1 = height - (MENU_ITEM_HEIGHT * 2);
        painter_set_pen_color(painter, THEME_COLOR_WHITE);
        painter_set_brush_color(painter, THEME_COLOR_GREEN_D);
        
        painter_draw_fillrect(painter, 0, y1, width, height);
        
        if (note) {
            painter_set_font(painter, theme->small_font);
            // painter_draw_string_wrap(painter, 5, y1 + 2, TR(note), width - 10);
            // построчный вывод с разделением по словам
            graphics_size_t sx = 5;
            graphics_size_t sy = y1 + 2;
            gui_menu_string_line_wrapping(painter, TR(note), &sx, &sy, width - 10, height);
            painter_set_font(painter, theme->middle_font);
        }
    }
}

void gui_menu_draw_events_help(gui_menu_t* menu, painter_t* painter, gui_metro_theme_t* theme, graphics_pos_t width, graphics_pos_t height)
{
    graphics_pos_t start_x = 0;
    graphics_pos_t start_y = height - (MENU_ITEM_HEIGHT * 2);
    graphics_pos_t text_x = start_x;
    graphics_pos_t text_y = start_y;
    menu_explorer_t* explorer = &menu->explorer;
    if (menu_explorer_help_visible(explorer)) {
        drive_event_t event;
        
        int8_t number = drive_events_count() - menu->explorer.item_pos - 1;
        if (number >= 0) {
            drive_event_index_t id = drive_events_index_by_number((size_t)number);

            err_t err = E_NO_ERROR;
            do {
                err = drive_events_read_event((drive_event_t*)&event, id);
            } while(err == E_BUSY);
            
            if (err == E_NO_ERROR) {                   
                graphics_color_t color_menu_font = THEME_COLOR_WHITE;
                graphics_color_t color_menu_back = THEME_COLOR_GRAY;
                const char* title = TR(TR_ID_MENU_EVENT_STATUS);
                const char* text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_INIT);

                if (event.type == DRIVE_EVENT_TYPE_ERROR) {
                    color_menu_back = THEME_COLOR_RED_D;
                    color_menu_font = THEME_COLOR_WHITE;
                    title = TR(TR_ID_MENU_EVENT_ERROR);
                }
                else if (event.type == DRIVE_EVENT_TYPE_WARNING) {
                    color_menu_back = THEME_COLOR_ORANGE;
                    color_menu_font = THEME_COLOR_WHITE;
                    title = TR(TR_ID_MENU_EVENT_WARNING);
                }

                switch(event.state) {
                    case DRIVE_STATE_INIT: //!< Инициализация.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_INIT);
                        break;
                    case DRIVE_STATE_CALIBRATION: //!< Калибровка питания.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_CALIBRATION);
                        break;
                    case DRIVE_STATE_IDLE: //!< Простой (готовность).
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_IDLE);
                        break;
                    case DRIVE_STATE_STOP: //!< Останов.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_STOP);
                        break;
                    case DRIVE_STATE_START: //!< Запуск.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_START);
                        break;
                    case DRIVE_STATE_RUN: //!< Работа.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_RUN);
                        break;
                    case DRIVE_STATE_STOP_ERROR: //!< Останов при ошибке.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_STOP_ERROR);
                        break;
                    case DRIVE_STATE_ERROR:  //!< Ошибка.
                        text_state = TR(TR_ID_MENU_EVENT_DRIVE_STATE_ERROR);
                        break;
                }

                painter_set_pen_color(painter, color_menu_font);
                painter_set_brush_color(painter, color_menu_back);

                // фон элемента меню
                painter_draw_fillrect(painter, text_x, text_y, width, height);

                // иконки
                text_x = start_x + GUI_MENU_EVENT_HELP_ICONS_SEP;
                text_y = start_y + GUI_MENU_EVENT_HELP_ICONS_SEP;
                size_t icon_width = (menu->icon_graphics->width / menu->icon_count);
                size_t icon_height = menu->icon_graphics->height;
                uint8_t j = 1;
                int i;
                for (i = 0; i < GUI_EVENT_ICON_CONDITIONS_COUNT; i++) {
                    if (gui_event_icon_conditions[i].callback(&event, gui_event_icon_conditions[i].param)) {
                        painter_set_brush_color(painter, gui_event_icon_conditions[i].color);
                        uint8_t val = gui_event_icon_conditions[i].icon;
                        if (val != ICONS_STATUSBAR_VAL_NOTHING) {
                            painter_bitblt(painter, text_x, text_y, menu->icon_graphics, icon_width * val, 0, icon_width, icon_height);
                        }
                        else {
                            painter_draw_fillrect(painter, text_x, text_y, text_x + icon_width, text_y + icon_height);
                        }

                        text_x += icon_width + GUI_MENU_EVENT_HELP_ICONS_SEP;
                        if (j == GUI_MENU_EVENT_HELP_ICONS_W) {
                            text_y += icon_height + GUI_MENU_EVENT_HELP_ICONS_SEP;
                            text_x = start_x + GUI_MENU_EVENT_HELP_ICONS_SEP;
                        }
                        if (j >= GUI_MENU_EVENT_HELP_ICONS_W * 2) break;
                        j++;
                    }
                }

                painter_set_pen_color(painter, color_menu_font);
                painter_set_brush_color(painter, color_menu_back);
                painter_set_font(painter, theme->small_font);

                text_x = start_x + (icon_width + GUI_MENU_EVENT_HELP_ICONS_SEP) * GUI_MENU_EVENT_HELP_ICONS_W + 5;
                text_y = start_y + GUI_MENU_EVENT_HELP_ICONS_SEP;

                // Тип события
                painter_draw_string(painter, text_x, text_y, title);

                text_y += theme->small_font->char_height + GUI_MENU_EVENT_HELP_ICONS_SEP;
                // Состояние привода
                painter_draw_string(painter, text_x, text_y, text_state);

                text_y += theme->small_font->char_height + GUI_MENU_EVENT_HELP_ICONS_SEP;
                // Состояние привода
                const char* ref_str = TR(TR_ID_MENU_EVENT_DRIVE_REFERENCE_FORMAT);
                painter_draw_string(painter, text_x, text_y, ref_str);
                graphics_size_t w;
                painter_string_size(painter, ref_str, &w, NULL);
                text_x += w;
                char str[5];
                snprintf(str, 5, "%d%%", (int)event.reference);
                painter_draw_string(painter, text_x, text_y, str); 


                painter_set_font(painter, theme->middle_font);
            }
        }
    }
}

void gui_menu_on_key_press(gui_menu_t* menu, keycode_t key)
{
    menu->home_on_timer_cnt = 0;
    switch (key) {
        //case KEY_LEFT:
        //case KEY_MINUS:
        case KEY_DOWN:
            menu->key_down_press = true;
            if (menu->key_up_press) { // одновременное нажатие UP + DOWN
                menu_explorer_help_toggle(&menu->explorer);
            }
            else { // нажатие DOWN
                menu_explorer_down(&(menu->explorer));
            }
            gui_menu_on_repaint(menu, NULL);
            return;
        //case KEY_RIGHT:
        //case KEY_PLUS:
        case KEY_UP:
            menu->key_up_press = true;
            if (menu->key_down_press) { // одновременное нажатие UP + DOWN
                menu_explorer_help_toggle(&menu->explorer);
            }
            else { // нажатие UP
                menu_explorer_up(&(menu->explorer));
            }
            gui_menu_on_repaint(menu, NULL);
            return;
        case KEY_ENTER:
            menu_explorer_in(&(menu->explorer));
            gui_menu_on_repaint(menu, NULL);
            return;
        case KEY_ESC:
            menu->long_esc_press_cnt++;
            // длительное нажание на кнопку Esc (переход на Home)
            if (menu->long_esc_press_cnt > MENU_HOME_ON_LONG_ESC_CNT) {
                if (menu_explorer_long_esc(&(menu->explorer))) {
                    if (menu_explorer_state(&(menu->explorer)) == MENU_EXPLORER_STATE_HOME) {
                        // переход к главному экрану
                        gui_menu_on_home_action(menu, key);
                        gui_menu_on_repaint(menu, NULL);
                    }
                    else if (menu_explorer_state(&(menu->explorer)) == MENU_EXPLORER_STATE_NAVI) {
                        // переход в режим навигации
                    }
                }
            }
            return;
        default:
            break;
    }
}

void gui_menu_on_key_release(gui_menu_t* menu, keycode_t key)
{
    switch (key) {
        //case KEY_LEFT:
        case KEY_MINUS:
            //
            break;
        case KEY_DOWN:
            menu->key_down_press = false;
            break;
        //case KEY_RIGHT:
        case KEY_PLUS:
            //
            break;
        case KEY_UP:
            menu->key_up_press = false;
            break;
        case KEY_ENTER:
            //
            break;
        case KEY_ESC:
            if (menu->long_esc_press_cnt > 0) menu->long_esc_press_cnt--;
            if (!menu_explorer_out(&(menu->explorer))) {
                gui_menu_on_home_action(menu, key);
            }
            gui_menu_on_repaint(menu, NULL);
            break;
        default:
            break;
    }
}

bool gui_menu_string_line_wrapping(painter_t* painter, const char* source, graphics_size_t* sx, graphics_size_t* sy, graphics_size_t width, graphics_size_t height)
{
    uint8_t count = 70; // размер буфера строки
    char buf[count]; // буфер строки
    char* dest= &buf[0];
    if (painter->font == NULL || source == NULL) return false;
    
    char* s = (char*)source;
    graphics_size_t cur_x = *sx;
    graphics_size_t y = *sy;
    font_char_t c = 0;
    graphics_size_t orig_x = cur_x;
    size_t c_size = 0;
    char* last_space_pos = s;
    char* last_space_pos_change = s;
    char* start_dest = dest;
    
    rect_t char_rect;
    point_t char_offset;

    while(*s){
        switch(*s){
            case '\r':
                last_space_pos_change = s;
                cur_x = orig_x;
                c_size = 1;
                break;
            case '\n':
                last_space_pos_change = s;
                if(font_get_char_position(painter->font, 0x20, &char_rect, &char_offset)){ 
                    cur_x = orig_x;
                    y += rect_height(&char_rect) + point_y(&char_offset) + font_vspace(painter->font);
                }
                c_size = 1;
                break;
            case '\t':
                last_space_pos = s;
                if(font_get_char_position(painter->font, 0x20, &char_rect, &char_offset)){
                    cur_x += (rect_width(&char_rect) + point_x(&char_offset) + font_hspace(painter->font)) * FONT_TAB_SIZE;
                }
                c_size = 1;
                break;
            default:
                c = font_utf8_decode(s, &c_size);
                if (c == 0x20) {
                    last_space_pos = s;
                }
                if(font_get_char_position(painter->font, c, &char_rect, &char_offset)){
                    cur_x += rect_width(&char_rect) + point_x(&char_offset) + font_hspace(painter->font);
                }
                break;
        }
        if (cur_x > width || (dest - start_dest) > count) {
            if (last_space_pos_change != last_space_pos) {
                dest -= (s - last_space_pos);
                s = last_space_pos + 1;
                last_space_pos_change = last_space_pos;
            }
            *dest = '\0'; 
            dest = start_dest;
            painter_draw_string(painter, orig_x, y, dest);
            if (font_get_char_position(painter->font, 0x20, &char_rect, &char_offset)) { 
                cur_x = orig_x;
                y += rect_height(&char_rect) + point_y(&char_offset) + font_vspace(painter->font);
            }
        }
        else {
            int i;
            for (i = 0; i < c_size; i++) {
                *dest = *s;
                s++;
                dest++;
            }
        }
        *sx = orig_x;
        *sy = y + rect_height(&char_rect) + point_y(&char_offset) + font_vspace(painter->font);
        if(font_get_char_position(painter->font, 0x20, &char_rect, &char_offset)){ 
            if ((cur_x > 0) && ((y + rect_height(&char_rect) + point_y(&char_offset)) > height))  return false;
        }
    }
    *dest = '\0';
    dest = start_dest;
    painter_draw_string(painter, orig_x, y, dest);
    return true;
}

void gui_menu_param_value_to_string(param_t* param, char* str, int32_t* edit_data) {
    int32_t value_int = 0;
    uint32_t value_uint = 0;
    fixed32_t value_fix = 0;
    size_t masklen = 15;
    char mask[masklen];
    int32_t max;
    int32_t min;
        
    param_type_t param_type = settings_param_type(param);
    param_value_t param_max = settings_param_max(param);
    param_value_t param_min = settings_param_min(param);
    switch(param_type) {
        default:
        case PARAM_TYPE_INT:
            max = param_max.int_value;
            min = param_min.int_value;
            break;
        case PARAM_TYPE_UINT:
            max = (int32_t)param_max.uint_value;
            min = (int32_t)param_min.uint_value;
            break;
        case PARAM_TYPE_FRACT_10:
        case PARAM_TYPE_FRACT_100:
        case PARAM_TYPE_FRACT_1000:
        case PARAM_TYPE_FRACT_10000:
            max = (int32_t)settings_param_data_from_fixed32(param, param_max.fixed_value);
            min = (int32_t)settings_param_data_from_fixed32(param, param_min.fixed_value);
            break;
    }
    
    if (min < 0) min = -min;
    if (min > max) max = min;
        
    int32_t den = 1;
    int decim = 0;
    while (den <= max) {
        decim++;
        den *= 10;
    }
    decim--;
    
    if (edit_data != NULL) {
        switch (settings_param_type(param)) {
            case PARAM_TYPE_INT:
            case PARAM_TYPE_UINT:
                value_int = *edit_data;
                snprintf(mask, masklen, "%%0%dd", decim + 1);
                snprintf(str, 9, mask, (int)value_int);
                break;
            case PARAM_TYPE_FRACT_10:
                value_int = *edit_data;
                snprintf(mask, masklen, "%%0%dd.%%01d", decim);
                snprintf(str, 9, mask, (int)(value_int / 10), (int)(value_int % 10));
                break;
            case PARAM_TYPE_FRACT_100:
                value_int = *edit_data;
                snprintf(mask, masklen, "%%0%dd.%%02d", decim - 1);
                snprintf(str, 9, mask, (int)(value_int / 100), (int)(value_int % 100));
                break;
            case PARAM_TYPE_FRACT_1000:
                value_int = *edit_data;
                snprintf(mask, masklen, "%%0%dd.%%03d", decim - 2);
                snprintf(str, 9, mask, (int)(value_int / 1000), (int)(value_int % 1000));
                break;
            case PARAM_TYPE_FRACT_10000:
                value_int = *edit_data;
                snprintf(mask, masklen, "%%0%dd.%%04d", decim - 3);
                snprintf(str, 9, mask, (int)(value_int / 10000), (int)(value_int % 10000));
                break;
            default:
                value_int = *edit_data;
                snprintf(mask, 9, "%%0%dd", decim + 1);
                snprintf(str, 9, mask, (int)value_int);
                break;
        }
    }
    else {
        switch (settings_param_type(param)) {
            case PARAM_TYPE_INT:
                value_int = settings_param_valuei(param);
                snprintf(str, 9, "%d", (int)value_int);
                break;
            case PARAM_TYPE_UINT:
                value_uint = settings_param_valueu(param);
                snprintf(str, 9, "%d", (int)value_uint);
                break;
             case PARAM_TYPE_FRACT_10:
                value_fix = settings_param_valuef(param);
                if(value_fix >= 0)
                    value_fix += fixed32_make_from_fract(5, 100);
                else
                    value_fix -= fixed32_make_from_fract(5, 100);
                value_int = fixed32_get_int(value_fix) * 10 + fixed32_get_fract_by_denom(value_fix, 10);
                snprintf(str, 9, "%d.%01d", (int)(value_int / 10), (int)(value_int % 10));
                break;
            case PARAM_TYPE_FRACT_100:
                value_fix = settings_param_valuef(param);
                if(value_fix >= 0)
                    value_fix += fixed32_make_from_fract(5, 1000);
                else
                    value_fix -= fixed32_make_from_fract(5, 1000);
                value_int = fixed32_get_int(value_fix) * 100 + fixed32_get_fract_by_denom(value_fix, 100);
                snprintf(str, 9, "%d.%02d", (int)(value_int / 100), (int)(value_int % 100));
                break;
            case PARAM_TYPE_FRACT_1000:
                value_fix = settings_param_valuef(param);
                if(value_fix >= 0)
                    value_fix += fixed32_make_from_fract(5, 10000);
                else
                    value_fix -= fixed32_make_from_fract(5, 10000);
                value_int = fixed32_get_int(value_fix) * 1000 + fixed32_get_fract_by_denom(value_fix, 1000);
                snprintf(str, 9, "%d.%03d", (int)(value_int / 1000), (int)(value_int % 1000));
                break;
            case PARAM_TYPE_FRACT_10000:
                value_fix = settings_param_valuef(param);
                if(value_fix >= 0)
                    value_fix += fixed32_make_from_fract(5, 100000);
                else
                    value_fix -= fixed32_make_from_fract(5, 100000);
                value_int = fixed32_get_int(value_fix) * 10000 + fixed32_get_fract_by_denom(value_fix, 10000);
                snprintf(str, 9, "%d.%04d", (int)(value_int / 10000), (int)(value_int % 10000));
                break;
            default:
                value_fix = settings_param_valuef(param);
                snprintf(str, 9, "%d", (int)value_fix);
                break;
        }
    }
}

size_t gui_menu_edit_draw_string(painter_t* painter, graphics_pos_t x, graphics_pos_t y, const char* s, uint8_t decim, bool type_fix)
{
    if(painter->font == NULL || s == NULL) return 0;
    
    if(x >= (graphics_pos_t)graphics_width(painter->graphics) ||
       y >= (graphics_pos_t)graphics_height(painter->graphics)) return 0;

    size_t slen = strlen(s);
    
    font_char_t c = 0;
    size_t c_size = 0;
    size_t count = 0;
    rect_t char_rect;
    point_t char_offset;
    graphics_pos_t dy;
    
    size_t dlt = 0;
    if (type_fix) dlt = 1;
    while(*s){
        count ++;
        
        bool isdigit_s = false;
        if (isdigit((int)*s)) {
            isdigit_s = true;
        }
        else {
            dlt = 0;
        }
        
        c = font_utf8_decode(s, &c_size);
        s += c_size;
        
        if(painter->font == NULL) return false;
    
        const font_bitmap_t* font_bitmap = NULL;
        
        rect_t rect;
        point_t offset;

        if(font_get_char_bitmap_position(painter->font, c, &font_bitmap, &rect, &offset)) {

            x += point_x(&offset);
            dy = y + point_y(&offset);
            
            graphics_color_t brush_color = painter->brush_color;
            graphics_color_t pen_color = painter->pen_color;
            bool revcolor = ((slen - count - dlt) == decim) && isdigit_s;
            if (revcolor) {
                painter_set_pen_color(painter, brush_color);
                painter_set_brush_color(painter, pen_color);
            }
            
            painter_draw_fillrect(painter, x - font_hspace(painter->font), dy - point_y(&offset) + 2, x + rect_width(&rect) + point_x(&offset) + font_hspace(painter->font), dy - point_y(&offset) + MENU_ITEM_HEIGHT - 2);
             
            painter_bitblt(painter, x, dy, font_bitmap_graphics(font_bitmap),
                    rect_left(&rect), rect_top(&rect), rect_width(&rect), rect_height(&rect));

            
            if (revcolor) {
                painter_set_pen_color(painter, pen_color);
                painter_set_brush_color(painter, brush_color);
            }
            
            rect_copy(&char_rect, &rect);
            point_copy(&char_offset, &offset);

            x += rect_width(&char_rect) + point_x(&char_offset) + font_hspace(painter->font);
        }
    }
    
    return count;
}

