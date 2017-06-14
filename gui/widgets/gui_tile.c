#include "gui_tile.h"
#include "utils/utils.h"
#include "graphics/painter.h"
#include "graphics/graphics_inlines.h"
#include "tft9341/tft9341.h"
#include "localization/localization.h"
#include "graphics/graphics.h"
#include "../fonts/arialbold42.h"
#include "drive_power.h"
#include "drive.h"
#include <stdio.h>
#include "gui_menu.h"

//! Таблица отображения ошибок в зависимости от условий
#define GUI_TILE_CONDITIONS_COUNT 15
GUI_TILE_CONDITIONS(gui_tile_conditions, GUI_TILE_CONDITIONS_COUNT) {
    // ошибки
    GUI_TILE_CONDITION(drive_error, DRIVE_ERROR_POWER_DATA_NOT_AVAIL,  TEXT(TR_ID_DRIVE_ERROR_POWER_DATA_NOT_AVAIL)),
    GUI_TILE_CONDITION(drive_error, DRIVE_ERROR_POWER_INVALID,  TEXT(TR_ID_DRIVE_ERROR_POWER_INVALID)),
    GUI_TILE_CONDITION(drive_error, DRIVE_ERROR_EMERGENCY_STOP, TEXT(TR_ID_DRIVE_ERROR_EMERGENCY_STOP)),
    GUI_TILE_CONDITION(drive_error, DRIVE_ERROR_PHASE,  TEXT(TR_ID_DRIVE_ERROR_PHASE)),
    GUI_TILE_CONDITION(drive_error, DRIVE_ERROR_PHASE_ANGLE,  TEXT(TR_ID_DRIVE_ERROR_PHASE_ANGLE)),
    GUI_TILE_CONDITION(drive_error, DRIVE_ERROR_PHASE_SYNC,  TEXT(TR_ID_DRIVE_ERROR_PHASE_SYNC)),
    GUI_TILE_CONDITION(drive_error, DRIVE_ERROR_THERMAL_OVERLOAD,  TEXT(TR_ID_DRIVE_ERROR_THERMAL_OVERLOAD)),
    GUI_TILE_CONDITION(drive_error, DRIVE_ERROR_ROT_BREAK,  TEXT(TR_ID_DRIVE_ERROR_ROT_BREAK)),
    GUI_TILE_CONDITION(drive_error, DRIVE_ERROR_HEATSINK_TEMP,  TEXT(TR_ID_DRIVE_ERROR_HEATSINK_TEMP)),
    // предупреждения
    GUI_TILE_CONDITION(drive_warning, DRIVE_WARNING_POWER,  TEXT(TR_ID_DRIVE_WARNING_POWER)),
    GUI_TILE_CONDITION(drive_warning, DRIVE_WARNING_PHASE_ANGLE,  TEXT(TR_ID_DRIVE_WARNING_PHASE_ANGLE)),
    GUI_TILE_CONDITION(drive_warning, DRIVE_WARNING_PHASE_SYNC,  TEXT(TR_ID_DRIVE_WARNING_PHASE_SYNC)),
    GUI_TILE_CONDITION(drive_warning, DRIVE_WARNING_THERMAL_OVERLOAD,  TEXT(TR_ID_DRIVE_WARNING_THERMAL_OVERLOAD)),
    GUI_TILE_CONDITION(drive_warning, DRIVE_WARNING_FAN_FAIL,  TEXT(TR_ID_DRIVE_WARNING_FAN_FAIL)),
    GUI_TILE_CONDITION(drive_warning, DRIVE_WARNING_HEATSINK_TEMP,  TEXT(TR_ID_DRIVE_WARNING_HEATSINK_TEMP))
};


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
        gui_widget_t* widget = GUI_WIDGET(tile);
        graphics_pos_t width = widget->rect.right - widget->rect.left;
        graphics_pos_t height = widget->rect.bottom - widget->rect.top;
        
        if (tile->warnings != DRIVE_WARNING_NONE || tile->errors != DRIVE_ERROR_NONE) {
            // отображение ошибок
            painter_set_pen(&painter, PAINTER_PEN_SOLID);
            painter_set_pen_color(&painter, theme->color_tile_font);
            painter_set_brush_color(&painter, tile->status_color);
            painter_set_font(&painter, theme->small_font);
            painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMASK);

            graphics_size_t start_x = 3;
            graphics_size_t text_x, text_y;
            text_x = start_x;//((graphics_pos_t)gui_widget_width(GUI_WIDGET(label)) - text_x) / 2;
            text_y = 1;//((graphics_pos_t)gui_widget_height(GUI_WIDGET(label)) - text_y) / 2;

            //painter_draw_string(&painter, text_x, text_y, TR(TR_ID_DRIVE_ERROR_EMERGENCY_STOP));
            uint8_t start_i = tile->show_error_break;
            int i;
            for (i = start_i; i < GUI_TILE_CONDITIONS_COUNT; i++) {
                if (gui_tile_conditions[i].callback(gui_tile_conditions[i].param)) {
                    tile->show_error_break = i;
                    if (gui_menu_string_line_wrapping(&painter, TR(gui_tile_conditions[i].text), &text_x, &text_y, width - 10, height)) {
                        text_x = start_x;
                        text_y += 2;
                        tile->update_errors = false;
                    }
                    else {
                        tile->update_errors = true;
                        break;
                    }
                }
            }
            if (start_i != 0 && i >= GUI_TILE_CONDITIONS_COUNT) {
                tile->show_error_break = 0;
                tile->update_errors = true;
            }
        }
        else {
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

            param_units_t unit = gui_tile_units(tile);
            //graphics_pos_t text_x, text_y;
            painter_string_size(&painter, TR(unit), (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);
            text_x = ((graphics_pos_t)gui_widget_width(GUI_WIDGET(tile)) - text_x - 3);
            text_y = ((graphics_pos_t)gui_widget_height(GUI_WIDGET(tile)) - text_y + 2);

            painter_draw_string(&painter, text_x, text_y, TR(unit));
        //}
        }
            
        gui_widget_end_paint(GUI_WIDGET(tile), &painter);
        
        //gui_tile_repaint_value(tile, rect);
    }
}

void gui_tile_repaint_value(gui_tile_t* tile, const rect_t* rect)
{
    if (gui_widget_visible(GUI_WIDGET(tile))) {
        // TODO: добавить получение индекса из параметра
        uint8_t for_errors_id = 3; // индекс плитки для отоброжения ошибок
        // отображать ошибки и предупреждения на этой плитке?
        bool for_show_errors = (tile->id == for_errors_id);
        drive_errors_t drive_errorsw = drive_errors();
        drive_warnings_t drive_warningsw = drive_warnings();
        bool errors_n_warnings = (drive_errorsw != DRIVE_ERROR_NONE) ||\
                                    (drive_warningsw != DRIVE_WARNING_NONE);
        if (for_show_errors && errors_n_warnings) {
            if (tile->warnings != drive_warningsw || tile->errors != drive_errorsw) {
                // отображение ошибок и предупреждений
                // измениение статуса (цвета плитки)
                tile->warnings = drive_warningsw;
                tile->errors = drive_errorsw;
                tile->show_error_break = 0;
                
                gui_tile_status_t ch_status = GUI_TILE_STATUS_OK;
                if (tile->errors != DRIVE_ERROR_NONE) {
                    ch_status = GUI_TILE_STATUS_ALARM;
                }
                else if (tile->warnings != DRIVE_WARNING_NONE) {
                    ch_status = GUI_TILE_STATUS_WARNING;
                }
                
                // перерисовывается только при изменении статуса
                gui_tile_set_status(tile, ch_status);
            } else if (tile->update_errors) {
                // циклическое отображение ошибок и предупреждений
                counter_t cur = system_counter_ticks();
                counter_t ticks_per_sec = system_counter_ticks_per_sec();
                counter_t update_time = tile->last_error_update + ticks_per_sec * TILE_TIMEOUT_ERROR_UPDATE;
                if (cur > update_time) {
                    // в предыдущий раз все ошибки не отобразились, перерисовываем для циклического отображения
                    gui_widget_repaint(GUI_WIDGET(tile), NULL);
                    tile->last_error_update = cur;
                }
            }
        }
        else {  // нет ошибок и предупреждений
            tile->warnings = DRIVE_ERROR_NONE;
            tile->errors = DRIVE_WARNING_NONE;
            if(drive_power_data_avail(DRIVE_POWER_CHANNELS)) {
                char str[9];

                param_t* param = settings_param_by_id(tile->type.param_id);
                param_t* param_alarm_min = settings_param_by_id(tile->type.alarm_min);
                param_t* param_warn_min = settings_param_by_id(tile->type.warn_min);
                param_t* param_warn_max = settings_param_by_id(tile->type.warn_max);
                param_t* param_alarm_max = settings_param_by_id(tile->type.alarm_max);
                fixed32_t valf = settings_param_valuef(param);
                fixed32_t alarm_min = settings_param_valuef(param_alarm_min);
                fixed32_t warn_min = settings_param_valuef(param_warn_min);
                fixed32_t warn_max = settings_param_valuef(param_warn_max);
                fixed32_t alarm_max = settings_param_valuef(param_alarm_max);

                int32_t int_part = fixed32_get_int(valf);
                int32_t fract_part = fixed32_get_fract_by_denom((int64_t)fixed_abs(valf), 10);
                param_units_t unit = gui_tile_units(tile);

                if ((int_part < 0 && int_part > -10) || (int_part > 0 && int_part < 100)) {
                    // отображение с дробной частью
                    snprintf(str, 9, "% 3d.%d", (int)int_part, (int)fract_part);
                }
                else {
                    // отображение только целой части
                    snprintf(str, 9, "% 4d", (int)int_part);
                }

                // измениение статуса (цвета плитки)
                gui_tile_status_t ch_status = GUI_TILE_STATUS_OK;
                if (valf < alarm_min || valf > alarm_max) {
                    ch_status = GUI_TILE_STATUS_ALARM;
                }
                else if (valf < warn_min || valf > warn_max) {
                    ch_status = GUI_TILE_STATUS_WARNING;
                }
                // перерисовывается только при изменении статуса
                gui_tile_set_status(tile, ch_status);

                // перерисовка значения показания
                gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(tile)));
                painter_t painter;
                gui_widget_begin_paint(GUI_WIDGET(tile), &painter, rect);

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

                gui_widget_end_paint(GUI_WIDGET(tile), &painter);
            }
        }
    }
}

param_units_t gui_tile_units(gui_tile_t* tile)
{
    // приоритет единицы измерения специально для плитки
    param_units_t unit = tile->type.unit;
    if (unit == NULL) {
        // единица измерения из параметра
        unit = settings_param_units(settings_param_by_id(tile->type.param_id));
    }
    return unit;
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