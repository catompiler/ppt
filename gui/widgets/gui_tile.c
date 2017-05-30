#include "gui_tile.h"
#include "utils/utils.h"
#include "graphics/painter.h"
#include "graphics/graphics_inlines.h"
#include "tft9341/tft9341.h"
#include "localization/localization.h"
#include "graphics/graphics.h"
#include "../fonts/arialbold42.h"
#include "drive_power.h"
#include <stdio.h>

err_t gui_tile_init(gui_tile_t* tile, gui_metro_t* gui)
{
    gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(tile)));

    tile->status = GUI_TILE_STATUS_OK;
    tile->status_color = theme->color_tile;
    return gui_tile_init_parent(tile, gui, NULL);
}

err_t gui_tile_init_parent(gui_tile_t* tile, gui_metro_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_widget_init_parent(GUI_WIDGET(tile), gui, parent));
    
    gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(tile)));    
    tile->status = GUI_TILE_STATUS_OK;
    tile->status_color = theme->color_tile;
    GUI_WIDGET(tile)->type_id = GUI_TILE_TYPE_ID;
    GUI_WIDGET(tile)->on_repaint = GUI_WIDGET_ON_REPAINT_PROC(gui_tile_on_repaint);
    
    return E_NO_ERROR;
}

void gui_tile_set_status(gui_tile_t* tile, const gui_tile_status_t status)
{
    if (tile->status != status) {
        tile->status = status;
        gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(tile)));
        switch (tile->status) {
            case GUI_TILE_STATUS_OK:
                tile->status_color = theme->color_tile;
                break;
            case GUI_TILE_STATUS_WARNING:
                tile->status_color = theme->color_tile_warning;
                break;
            case GUI_TILE_STATUS_ALARM:
                tile->status_color = theme->color_tile_alarm;
                break;
        }
        gui_widget_set_back_color(GUI_WIDGET(tile), tile->status_color);
        gui_widget_repaint(GUI_WIDGET(tile), NULL);
    }
}

void gui_tile_set_type(gui_tile_t* tile, const gui_tile_type_t type)
{
    if (tile->type.param_id != type.param_id) {
        tile->type = type;
        gui_widget_repaint(GUI_WIDGET(tile), NULL);
    }
}

void gui_tile_on_repaint(gui_tile_t* tile, const rect_t* rect)
{
    gui_widget_on_repaint(GUI_WIDGET(tile), rect);
    if (gui_widget_visible(GUI_WIDGET(tile))) {
        gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(tile)));

        painter_t painter;
        gui_widget_begin_paint(GUI_WIDGET(tile), &painter, rect);

        //if (tile->caption != NULL) {
            painter_set_pen(&painter, PAINTER_PEN_SOLID);
            painter_set_pen_color(&painter, theme->color_tile_font);
            painter_set_brush_color(&painter, tile->status_color);
            painter_set_font(&painter, theme->middle_font);
            painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMASK);

            graphics_pos_t text_x, text_y;
            text_x = 3;//((graphics_pos_t)gui_widget_width(GUI_WIDGET(label)) - text_x) / 2;
            text_y = 1;//((graphics_pos_t)gui_widget_height(GUI_WIDGET(label)) - text_y) / 2;

            painter_draw_string(&painter, text_x, text_y, TR(tile->type.title));
        //}
        //if (tile->unit != NULL) {
            painter_set_pen(&painter, PAINTER_PEN_SOLID);
            painter_set_pen_color(&painter, theme->color_tile_font);
            painter_set_brush_color(&painter, tile->status_color);
            painter_set_font(&painter, theme->big_font);
            painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMASK);

            param_units_t unit = settings_param_units(settings_param_by_id(tile->type.param_id));
            //graphics_pos_t text_x, text_y;
            painter_string_size(&painter, TR(unit), (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);
            text_x = ((graphics_pos_t)gui_widget_width(GUI_WIDGET(tile)) - text_x - 3);
            text_y = ((graphics_pos_t)gui_widget_height(GUI_WIDGET(tile)) - text_y + 2);

            painter_draw_string(&painter, text_x, text_y, TR(unit));
        //}
        gui_widget_end_paint(GUI_WIDGET(tile), &painter);
        
        //gui_tile_repaint_value(tile, rect);
    }
}

void gui_tile_repaint_value(gui_tile_t* tile, const rect_t* rect)
{
    if (gui_widget_visible(GUI_WIDGET(tile))) {
        gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(tile)));
        painter_t painter;
        gui_widget_begin_paint(GUI_WIDGET(tile), &painter, rect);

        if(drive_power_data_avail(DRIVE_POWER_CHANNELS)) {
            char str[9];
            
            param_t* param = settings_param_by_id(tile->type.param_id);
            int32_t int_part = fixed32_get_int(settings_param_valuef(param));
            int32_t fract_part = fixed32_get_fract_by_denom((int64_t)fixed_abs(settings_param_valuef(param)), 10);
            param_units_t unit = settings_param_units(param);
            
            if ((int_part < 0 && int_part > -10) || (int_part > 0 && int_part < 100)) {
                snprintf(str, 9, "% 3d.%d", (int)int_part, (int)fract_part);
            }
            else {
                snprintf(str, 9, "% 4d", (int)int_part);
            }
            
            painter_set_pen(&painter, PAINTER_PEN_SOLID);
            painter_set_brush(&painter, PAINTER_BRUSH_SOLID);
            painter_set_pen_color(&painter, theme->color_tile_font);
            painter_set_brush_color(&painter, tile->status_color);
            painter_set_font(&painter, theme->big_font);
            painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMASK);

            graphics_pos_t text_x, text_y, dx, dy;
            painter_string_size(&painter, str, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);
            text_y = ((graphics_pos_t)gui_widget_height(GUI_WIDGET(tile)) - text_y + 2);
            painter_string_size(&painter, TR(unit), (graphics_size_t*)&dx, (graphics_size_t*)&dy);
            text_x = ((graphics_pos_t)gui_widget_width(GUI_WIDGET(tile)) - dx - text_x - 5);
            //painter_draw_fillrect(&painter, text_x, text_y, text_x + dx, text_y + dy);
            gui_tile_draw_value_string(&painter, text_x, text_y, str);
        }
        
        gui_widget_end_paint(GUI_WIDGET(tile), &painter);
    }
}

size_t gui_tile_draw_value_string(painter_t* painter, graphics_pos_t x, graphics_pos_t y, const char* s)
{
    if(painter->font == NULL || s == NULL) return 0;
    
    if(x >= (graphics_pos_t)graphics_width(painter->graphics) ||
       y >= (graphics_pos_t)graphics_height(painter->graphics)) return 0;

    font_char_t c = 0;
    size_t c_size = 0;
    size_t count = 0;

    while(*s){
        c = font_utf8_decode(s, &c_size);
        if (c < 44 && c > 57) return count;
        s += c_size;
        if (c == 46) painter_draw_fillrect(painter, x, y, x + 10, y + ARIALBOLD42_CHAR_HEIGHT);
        if(painter_draw_char(painter, x, y, c)){
            if (c == 46) {
                x += 10;
            }
            else {
                x += GUI_TILE_VALUE_FONT_WIDTH;
            }
            count ++;
        }
        painter_draw_fillrect(painter, x, y, x + GUI_TILE_VALUE_FONT_SPACE, y + ARIALBOLD42_CHAR_HEIGHT);
        x += GUI_TILE_VALUE_FONT_SPACE;
        if(y >= (graphics_pos_t)graphics_width(painter->graphics)) break;
    }
    
    return count;
}