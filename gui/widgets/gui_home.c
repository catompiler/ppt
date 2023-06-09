#include "gui_home.h"
#include "utils/utils.h"
#include "graphics/painter.h"
#include "graphics/graphics_inlines.h"
#include "tft9341/tft9341.h"
#include "../resources/resources_colors.h"
#include "../resources/resources_params.h"
#include "drive_keypad.h"
#include "drive.h"


// Инкремент задания при нажатии на кнопку.
#define GUI_HOME_DRIVE_REFERENCE_DELTA (fixed32_make_from_fract(1, 2))


err_t gui_home_init(gui_home_t* home, gui_metro_t* gui)
{
    return gui_home_init_parent(home, gui, NULL);
}

err_t gui_home_init_parent(gui_home_t* home, gui_metro_t* gui, gui_widget_t* parent)
{
    RETURN_ERR_IF_FAIL(gui_widget_init_parent(GUI_WIDGET(home), gui, parent));
    
    //gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(home)));
        
    GUI_WIDGET(home)->type_id = GUI_HOME_TYPE_ID;
    GUI_WIDGET(home)->on_repaint = GUI_WIDGET_ON_REPAINT_PROC(gui_home_on_repaint);
    GUI_WIDGET(home)->on_key_press = GUI_WIDGET_ON_KEY_PRESS_PROC(gui_home_on_key_press);
    GUI_WIDGET(home)->on_key_release = GUI_WIDGET_ON_KEY_RELEASE_PROC(gui_home_on_key_release);
    home->on_enter = NULL;
    
    return E_NO_ERROR;
}

void gui_home_on_repaint(gui_home_t* home, const rect_t* rect)
{
    //gui_widget_on_repaint(GUI_WIDGET(home), rect);
    /*
    gui_metro_theme_t* theme = gui_metro_theme(gui_object_gui(GUI_OBJECT(home)));
    painter_t painter;
    gui_widget_begin_paint(GUI_WIDGET(home), &painter, rect);
    
    gui_widget_end_paint(GUI_WIDGET(home), &painter);
    */
}

static void gui_home_on_inc_reference(void)
{
    drive_set_reference(drive_reference() + GUI_HOME_DRIVE_REFERENCE_DELTA);
}

static void gui_home_on_dec_reference(void)
{
    drive_set_reference(drive_reference() - GUI_HOME_DRIVE_REFERENCE_DELTA);
}

void gui_home_on_key_press(gui_home_t* home, keycode_t key)
{
    switch (key) {
        case KEY_MINUS:
            gui_home_on_dec_reference();
            break;
        case KEY_PLUS:
            gui_home_on_inc_reference();
            break;
        case KEY_ENTER:
            home->on_enter(home);
            break;
        case KEY_ESC:
            //
            break;
        default:
            break;
    }
}

void gui_home_on_key_release(gui_home_t* home, keycode_t key)
{
    
}

void gui_home_tiles_set_visible(gui_tile_t* tiles, bool visible)
{
    int i;
    for (i = 0; i < GUI_HOME_TILES_COUNT; i++) {
        gui_widget_set_visible(GUI_WIDGET(&tiles[i]), visible);
    }
}