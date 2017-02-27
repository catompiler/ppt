#include "gui_menu.h"
#include "utils/utils.h"
#include "graphics/painter.h"
#include "graphics/graphics_inlines.h"
#include "tft9341/tft9341.h"

#include "../resources/resources_menu.h"
#include "../../parameters_menu.h"
#include "../../parameters_ids.h"
#include "settings.h"
#include "gui/resources/resources_colors.h"

#include <stdio.h>

MENU_ITEMS(menu_items, menu_descrs);

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
    
    menu_make_from_descrs(&(menu->menu), menu_items, MENU_ITEMS_COUNT(menu_items), menu_descrs, MENU_DESCRS_COUNT(menu_descrs), NULL);

    menu_explorer_init(&(menu->explorer), &(menu->menu));
    
    return E_NO_ERROR;
}

void gui_menu_init_counters(gui_menu_t* menu, gui_metro_t* gui)
{
    menu->long_esc_press_cnt = 0;
    menu->home_on_timer_cnt = 0;
    menu->key_down_press = false;
    menu->key_up_press = false;
    menu->explorer.help = false;
}

void gui_menu_on_repaint(gui_menu_t* menu, const rect_t* rect)
{
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
        
        // Заголовок меню
        gui_menu_draw_title(menu, &painter, theme, width);
        
        
        text_y = MENU_ITEM_HEIGHT;
        int i;
        for (i = 0; i < menu_explorer_displayed_cnt(&menu->explorer); i++) {
            text_x = GUI_MENU_TEXT_LEFT_PAD;
            
            gui_menu_draw_item(menu, menu->explorer.displayed[i], &painter, theme, width, height, text_x, text_y);

            text_y += MENU_ITEM_HEIGHT;
        }
        
        // скроллбар
        gui_menu_draw_scrollbar(menu, &painter, theme, width, height);

        gui_menu_draw_help(menu, &painter, theme, width, height);
        
        gui_widget_end_paint(GUI_WIDGET(menu), &painter);
    }
}

void gui_menu_on_home_action(gui_menu_t* menu, keycode_t key)
{
    menu->on_home(menu);
    menu->long_esc_press_cnt = 0;
    menu->home_on_timer_cnt = 0;
    menu_explorer_init(&menu->explorer, &menu->menu);
}

void gui_menu_on_timer_home_action(gui_menu_t* menu)
{
    //if (GUI_WIDGET(menu)->focusable) {
        menu->home_on_timer_cnt++;
        if (menu->home_on_timer_cnt > MENU_HOME_ON_TIMER_CNT) {
            gui_menu_on_home_action(menu, 0);
        }
    //}
}

int gui_menu_get_f32_fract(int number, int decimals)
{
    int denom = 0;
    size_t i;
    for(i = 0; i < decimals; i ++){
        if(denom == 0) denom = 10;
        else denom *= 10;
    }
    return fixed32_get_fract_by_denom((int64_t)fixed_abs(number), denom);
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
    graphics_pos_t text_x = x;
    graphics_pos_t text_y = y;
    graphics_pos_t text_unit;
    text_unit = 0;
    if (menu_item == NULL) {
        painter_set_pen_color(painter, theme->color_menu_font);
        painter_set_brush_color(painter, theme->color_menu);
        painter_draw_fillrect(painter, 0, text_y, width, text_y + MENU_ITEM_HEIGHT);
        painter_draw_string(painter, text_x, text_y, " ");
    }
    else {
        if (menu_explorer_is_selected(&menu->explorer, menu_item)) {
            painter_set_pen_color(painter, theme->color_menu_active_font);
            painter_set_brush_color(painter, theme->color_menu_active);
            painter_draw_fillrect(painter, 0, text_y, width, text_y + MENU_ITEM_HEIGHT);
        }
        else {
            painter_set_pen_color(painter, theme->color_menu_font);
            painter_set_brush_color(painter, theme->color_menu);
            painter_draw_fillrect(painter, 0, text_y, width, text_y + MENU_ITEM_HEIGHT);
        }
        if (menu_item_childs_count(menu_item) > 0) {
            // подменю
            painter_draw_string(painter, text_x, text_y, "►");
            text_x = GUI_MENU_ICON_LEFT_PAD;
        }
        painter_draw_string(painter, text_x, text_y, TR(menu_item->text));
        if (menu_item_childs_count(menu_item) == 0) {
            // параметр
            param_t* param = settings_param_by_id(menu_item->id);
            if (menu_item->value == NULL) {
                param_units_t unit = settings_param_units(param);
                text_x = width - 8;
                if (unit != NULL) {
                    text_x = text_x - 40;
                    text_unit = text_x;
                }
                int32_t value_int = 0;
                uint32_t value_uint = 0;
                fixed32_t value_fix = 0;
                char str[9];
                char mask[9];
                int max = (int)settings_param_max(param).int_value;
                int32_t den = 1;
                int decim = 0;
                while (den < max) {
                    decim++;
                    den *= 10;
                }
                switch (settings_param_type(param)) {
                    case PARAM_TYPE_INT:
                        value_int = settings_param_valuei(param);
                        snprintf(mask, 9, "%%0%dd", decim);
                        snprintf(str, 9, mask, (int)value_int);
                        break;
                    case PARAM_TYPE_UINT:
                        value_uint = settings_param_valueu(param);
                        snprintf(mask, 9, "%%0%dd", decim);
                        snprintf(str, 9, mask, (int)value_uint);
                        break;
                    case PARAM_TYPE_FRACT_10:
                        value_fix = settings_param_valuef(param);
                        snprintf(str, 9, "%d.%01d", (int)fixed32_get_int(value_fix), gui_menu_get_f32_fract(value_fix, 1));
                        break;
                    case PARAM_TYPE_FRACT_100:
                        value_fix = settings_param_valuef(param);
                        snprintf(str, 9, "%d.%02d", (int)fixed32_get_int(value_fix), gui_menu_get_f32_fract(value_fix, 2));
                        break;
                    case PARAM_TYPE_FRACT_1000:
                        value_fix = settings_param_valuef(param);
                        snprintf(str, 9, "%d.%03d", (int)fixed32_get_int(value_fix), gui_menu_get_f32_fract(value_fix, 3));
                        break;
                    case PARAM_TYPE_FRACT_10000:
                        value_fix = settings_param_valuef(param);
                        snprintf(str, 9, "%d.%04d", (int)fixed32_get_int(value_fix), gui_menu_get_f32_fract(value_fix, 4));
                        break;
                    default:
                        value_fix = settings_param_valuef(param);
                        snprintf(str, 9, "%d", (int)value_fix);
                        break;
                }

                if (unit != NULL) {
                    const char* unit_text = TR(unit);
                    painter_draw_fillrect(painter, text_unit - 3, text_y, width , text_y + MENU_ITEM_HEIGHT);
                    painter_draw_string(painter, text_unit, text_y, unit_text);
                }

                graphics_size_t wid;
                painter_string_size(painter, str, &wid, NULL);
                text_x = text_x - wid - 6;

                //
                if (menu_explorer_is_selected(&menu->explorer, menu_item) && menu->explorer.state == MENU_EXPLORER_STATE_EDIT) {
                    painter_set_pen_color(painter, theme->color_menu_edit_active);
                    painter_set_brush_color(painter, theme->color_menu_edit);
                }
                //
                painter_draw_fillrect(painter, text_x - 12, text_y, text_unit - 3, text_y + MENU_ITEM_HEIGHT);
                painter_draw_string(painter, text_x, text_y, str);

            }
            else {
                size_t val_cnt = menu_value_enum_count(menu_item->value);
                menu_value_t* values = menu_value_enum_values(menu_item->value);

                menu_value_t* val = menu_value_enum_current_value(menu_item->value);

                text_x = width - 25;

                graphics_size_t wid = 0;
                int j;
                for (j = 0; j < val_cnt; j++) {
                    graphics_size_t w;
                    const char* str = TR(values[j].value_string);
                    painter_string_size(painter, str, &w, NULL);
                    if (w > wid) wid = w;
                }

                const char* str = TR(val->value_string);
                graphics_size_t w;
                painter_string_size(painter, str, &w, NULL);

                text_x = text_x - wid;

                //painter_set_brush_color(&painter, THEME_COLOR_WHITE);
                painter_draw_fillrect(painter, text_x - 12, text_y, text_x + wid + 6, text_y + MENU_ITEM_HEIGHT);

                text_x = width - 22;
                if (menu_explorer_is_selected(&menu->explorer, menu_item)) {
                    painter_set_pen_color(painter, THEME_COLOR_GRAY_L);
                }
                else {
                    painter_set_pen_color(painter, THEME_COLOR_GRAY);
                }
                painter_draw_string(painter, text_x, text_y, "↕");
                text_x = width - 25 - w;
                if (menu_explorer_is_selected(&menu->explorer, menu_item)) {
                    painter_set_pen_color(painter, theme->color_menu_active_font);
                }
                else {
                    painter_set_pen_color(painter, theme->color_menu_font);
                }
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
            char dest[200];
            painter_set_font(painter, theme->small_font);
            gui_menu_string_line_wrapping(painter, TR(note), &dest[0], 200, width - 10, height);
            painter_draw_string(painter, 5, y1 + 2, dest);
            painter_set_font(painter, theme->middle_font);
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
            if (menu->key_up_press) {
                menu_explorer_help_toggle(&menu->explorer);
            }
            else {
                menu_explorer_next(&(menu->explorer));
            }
            gui_menu_on_repaint(menu, NULL);
            return;
        //case KEY_RIGHT:
        //case KEY_PLUS:
        case KEY_UP:
            menu->key_up_press = true;
            if (menu->key_down_press) {
                menu_explorer_help_toggle(&menu->explorer);
            }
            else {
                menu_explorer_prev(&(menu->explorer));
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
                gui_menu_on_home_action(menu, key);
                gui_menu_on_repaint(menu, NULL);
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
        //case KEY_MINUS:
        case KEY_DOWN:
            menu->key_down_press = false;
            break;
        //case KEY_RIGHT:
        //case KEY_PLUS:
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

void gui_menu_string_line_wrapping(painter_t* painter, const char* source, char* dest, uint16_t count, graphics_size_t width, graphics_size_t height)
{
    if (painter->font == NULL || source == NULL) return;
    
    char* s = (char*)source;
    graphics_size_t cur_x = 0;
    graphics_size_t y = 0;
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
        if (cur_x > width) {
            if (last_space_pos_change != last_space_pos) {
                dest -= (s - last_space_pos);
                s = last_space_pos + 1;;
                last_space_pos_change = last_space_pos;
            }
            if (font_get_char_position(painter->font, 0x20, &char_rect, &char_offset)) { 
                cur_x = orig_x;
                y += rect_height(&char_rect) + point_y(&char_offset) + font_vspace(painter->font);
            }
            *dest = '\n';
            dest++;
            if ((dest - start_dest) > count) return;
        }
        else {
            int i;
            for (i = 0; i < c_size; i++) {
                *dest = *s;
                s++;
                dest++;
            }
        }
        *dest = 0x00;
        if(font_get_char_position(painter->font, 0x20, &char_rect, &char_offset)){ 
            if ((cur_x > 0) && ((y + rect_height(&char_rect) + point_y(&char_offset)) > height))  return;
        }
    }
}