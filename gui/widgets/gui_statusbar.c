#include "gui_statusbar.h"
#include "utils/utils.h"
#include "graphics/painter.h"
#include "graphics/graphics_inlines.h"
#include "tft9341/tft9341.h"
#include "gui/bitmaps/icons_statusbar.h"
#include "gui/resources/resources_colors.h"
#include <stdio.h>

err_t gui_statusbar_init(gui_statusbar_t* statusbar, gui_metro_t* gui)
{
    return gui_statusbar_init_parent(statusbar, gui, NULL);
}

err_t gui_statusbar_init_parent(gui_statusbar_t* statusbar, gui_metro_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_widget_init_parent(GUI_WIDGET(statusbar), gui, parent));
    
    //gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(statusbar)));    

    gui_time_init_parent(&(statusbar->time), gui, GUI_WIDGET(statusbar));
    gui_widget_move(GUI_WIDGET(&(statusbar->time)), 260, 0);
    gui_widget_resize(GUI_WIDGET(&(statusbar->time)), 60, 24);
    //gui_widget_set_back_color(GUI_WIDGET(&gui.icon), THEME_COLOR_BLACK);
    gui_widget_set_visible(GUI_WIDGET(&(statusbar->time)), true);
        
    GUI_WIDGET(statusbar)->type_id = GUI_STATUSBAR_TYPE_ID;
    GUI_WIDGET(statusbar)->on_repaint = GUI_WIDGET_ON_REPAINT_PROC(gui_statusbar_on_repaint);
    
    return E_NO_ERROR;
}

void gui_statusbar_on_repaint(gui_statusbar_t* statusbar, const rect_t* rect)
{
    statusbar->reference = -1; // сброс отображаемого значения задания, для обновления
    // необходимость перерисовки иконок статусбара
    gui_statusbar_update_icons(statusbar, true);
    gui_widget_on_repaint(GUI_WIDGET(statusbar), rect);
}

void gui_statusbar_repaint_reference_n_icons(gui_statusbar_t* statusbar, const rect_t* rect)
{
    if (gui_widget_visible(GUI_WIDGET(statusbar))) {
        graphics_size_t width, height;
        gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(statusbar)));
        graphics_pos_t text_x = -6;
        
        painter_t painter;
        gui_widget_begin_paint(GUI_WIDGET(statusbar), &painter, rect);
        painter_set_pen(&painter, PAINTER_PEN_SOLID);
        painter_set_pen_color(&painter, theme->color_statusbar_font);
        painter_set_brush_color(&painter, theme->color_statusbar);
        painter_set_font(&painter, theme->middle_font);
        painter_set_source_image_mode(&painter, PAINTER_SOURCE_IMAGE_MODE_BITMASK);
        
        char ref_str[6];  
        // отрисовка задания привода
        uint32_t reference_i = fixed32_get_int(drive_reference());
        if (statusbar->reference != reference_i) {
            statusbar->reference = reference_i;
            
            snprintf(ref_str, 6, GUI_STATUSBAR_REF_STR_FORMAT, (int)statusbar->reference);
            graphics_pos_t text_y;
            text_y = -1;

            painter_string_size(&painter, ref_str, &width, &height);
            
            painter_draw_string(&painter, text_x, text_y, ref_str);
        }
        else {
            snprintf(ref_str, 6, GUI_STATUSBAR_REF_STR_FORMAT, (int)statusbar->reference);
            painter_string_size(&painter, ref_str, &width, &height);
        }
        text_x += width + 3;
        // прорисовка иконок
        snprintf(ref_str, 6, GUI_TIME_STR_FORMAT, 0, 0);
        painter_string_size(&painter, ref_str, (graphics_size_t*)&width, (graphics_size_t*)&height);
        width = gui_widget_width(GUI_WIDGET(statusbar)) - width - 3;
        int i;
        for (i = 0; i < GUI_STATUSBAR_ICONS_COUNT; i++) {
            gui_statusbar_icon_repaint(statusbar, &painter, theme, text_x, width, i, &(statusbar->icons[i]));
        }
        gui_widget_end_paint(GUI_WIDGET(statusbar), &painter);
    }
}

void gui_statusbar_update(gui_statusbar_t* statusbar, const rect_t* rect)
{
    if (gui_widget_visible(GUI_WIDGET(statusbar))) {
        gui_time_repaint_value(&(statusbar->time), rect);
        gui_statusbar_repaint_reference_n_icons(statusbar, rect);
    }
}

void gui_statusbar_set_graphics(gui_statusbar_t* statusbar, graphics_t* graphics, uint8_t count)
{
    statusbar->icon_graphics = graphics;
    statusbar->icon_count = count;
    gui_widget_repaint(GUI_WIDGET(statusbar), NULL);
}

// Нет смысла делать константным параметр, передаваемый по значению.
void gui_statusbar_icon_repaint(gui_statusbar_t* statusbar, painter_t* painter, const gui_metro_theme_t* theme, graphics_pos_t left, graphics_pos_t right, uint8_t pos, gui_icon_t* icon)
{
    if (icon->count > 0) {
        // Если оставить знаковый тип, то
        // при value == 127 будет переполнение знакового типа в -1.
        icon->value++;
        if (icon->value >= icon->count) icon->value = 0;
    }
    if (icon->value != icon->current) {
        icon->current = icon->value;
        const uint8_t* item = &(icon->current);
        if (icon->count > 0) {
            if (icon->current >= icon->count) icon->current = 0;
            item = &icon->list[icon->current];
        }
        size_t icon_width = (statusbar->icon_graphics->width / statusbar->icon_count);
        size_t icon_height = statusbar->icon_graphics->height;
        graphics_pos_t y = 0;
        graphics_pos_t x = left + icon_width * pos;

        if ((x + icon_width) > right) return;

        painter_set_pen(painter, PAINTER_PEN_SOLID);
        painter_set_brush(painter, PAINTER_BRUSH_SOLID);
        
        painter_set_brush_color(painter, theme->color_statusbar);
        painter_set_source_image_mode(painter, PAINTER_SOURCE_IMAGE_MODE_BITMASK);
        
        uint8_t val = (*item);
        if (val != ICONS_STATUSBAR_VAL_NOTHING) {
            painter_set_pen_color(painter, icon->color);
            painter_bitblt(painter, x, y, statusbar->icon_graphics, icon_width * val, 0, icon_width, icon_height);
        }
        else {
            painter_draw_fillrect(painter, x, y, x+icon_width, x+icon_height);
        }
    }
}

void gui_statusbar_set_icons(gui_statusbar_t* statusbar, gui_icon_t* icons)
{
    statusbar->icons = icons;
}

void gui_statusbar_update_icons(gui_statusbar_t* statusbar, bool repaint)
{
    uint8_t j = 0;
    int i;
    for (i = 0; i < GUI_ICON_CONDITIONS_COUNT; i++) {
        gui_statusbar_update_icons_set_icon(statusbar, &j, &gui_icon_conditions[i], repaint);
    }
    for (i = j; i < GUI_STATUSBAR_ICONS_COUNT; i++) {
        gui_statusbar_update_icons_unset_icon(statusbar, i);
    }
}

void gui_statusbar_update_icons_set_icon(gui_statusbar_t* statusbar, uint8_t* index, const gui_icon_condition_t* condition, bool repaint)
{
    if (*index >= GUI_STATUSBAR_ICONS_COUNT) return;
    if (condition->callback(condition->param)) {
        gui_icon_t* icon = &statusbar->icons[*index];
        if (condition->list != NULL && condition->count > 0) {
            if (icon->list != condition->list || icon->count != condition->count)
            {
                icon->list = condition->list;
                icon->count = condition->count;
                icon->value = condition->icon;
                icon->current = ICONS_STATUSBAR_VAL_NOTHING;
            }
        }
        else {
            icon->list = NULL;
            icon->count = 0;
            if (icon->value != condition->icon) {
                icon->value = condition->icon;
                icon->current = ICONS_STATUSBAR_VAL_NOTHING;
            }
        }
        icon->color = condition->color;
        if (repaint) {
            icon->current = ICONS_STATUSBAR_VAL_NOTHING;
        }
        (*index)++;
    }
}

void gui_statusbar_update_icons_unset_icon(gui_statusbar_t* statusbar, uint8_t index)
{
    gui_icon_t* icon = &statusbar->icons[index];
    if (icon->current != ICONS_STATUSBAR_VAL_NOTHING) {
        icon->list = NULL;
        icon->count = 0;
        icon->value = ICONS_STATUSBAR_VAL_NOTHING;
        icon->current = 0;
    }
}

bool gui_statusbar_drive_ready()
{
    return drive_ready() && !drive_running();
}