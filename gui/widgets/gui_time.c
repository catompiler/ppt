#include <time.h>
#include "gui_time.h"
#include "utils/utils.h"
#include "graphics/painter.h"
#include "graphics/graphics_inlines.h"
#include "tft9341/tft9341.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

err_t gui_time_init(gui_time_t* time, gui_metro_t* gui)
{
    gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(time)));
    (GUI_WIDGET(time))->back_color = theme->color_statusbar;
    return gui_time_init_parent(time, gui, NULL);
}

err_t gui_time_init_parent(gui_time_t* time, gui_metro_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_widget_init_parent(GUI_WIDGET(time), gui, parent));
    
    gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(time)));
    (GUI_WIDGET(time))->back_color = theme->color_statusbar;
    
    GUI_WIDGET(time)->type_id = GUI_TIME_TYPE_ID;
    GUI_WIDGET(time)->on_repaint = GUI_WIDGET_ON_REPAINT_PROC(gui_time_on_repaint);
    
    return E_NO_ERROR;
}

void gui_time_on_repaint(gui_time_t* time, const rect_t* rect)
{
    gui_widget_on_repaint(GUI_WIDGET(time), rect);
    
    gui_time_repaint_value(time, rect);
}

void gui_time_repaint_value(gui_time_t* time_w, const rect_t* rect)
{
    gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(time_w)));
    painter_t painter;
    gui_widget_begin_paint(GUI_WIDGET(time_w), &painter, rect);
    
    painter_set_pen(&painter, PAINTER_PEN_SOLID);
    painter_set_pen_color(&painter, theme->color_statusbar_font);
    painter_set_brush_color(&painter, theme->color_statusbar);
    painter_set_font(&painter, theme->middle_font);
    painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMASK);
    
    static char time_str[6];    
    time_t t = time(NULL);   
    struct tm* ts = localtime(&t);
    snprintf(time_str, 6, GUI_TIME_STR_FORMAT, ts->tm_hour, ts->tm_min);
    graphics_pos_t text_x, text_y;
    painter_string_size(&painter, time_str, (graphics_size_t*)&text_x, (graphics_size_t*)&text_y);
    text_x = ((graphics_pos_t)gui_widget_width(GUI_WIDGET(time_w)) - text_x) / 2;
    text_y = ((graphics_pos_t)gui_widget_height(GUI_WIDGET(time_w)) - text_y) / 2 - 2;

    painter_draw_string(&painter, text_x, text_y, time_str);
    
    gui_widget_end_paint(GUI_WIDGET(time_w), &painter);
}