#include "drive_ui.h"
#include "drive.h"
#include "utils/utils.h"
#include "counter/counter.h"


//! Тип структуры интерфейса привода.
typedef struct _Drive_Ui {
    counter_t update_period;
    counter_t last_update_time;
    bool need_update;
} drive_ui_t;

//! Интерфейс привода.
static drive_ui_t ui;


static err_t drive_ui_init_keypad(drive_ui_init_t* ui_is)
{
    drive_keypad_init_t keypad_is;
    
    keypad_is.ioport = ui_is->ioport;
    
    return drive_keypad_init(&keypad_is);
}

static err_t drive_ui_init_gui(drive_ui_init_t* ui_is)
{
    drive_gui_init_t gui_is;
    
    gui_is.tft = ui_is->tft;
    
    return drive_gui_init(&gui_is);
}

static void drive_ui_on_key_pressed(keycode_t key)
{
    reference_t reference;
    switch(key){
        case KEY_START:
            drive_start();
            ui.need_update = true;
            break;
        case KEY_STOP:
            drive_stop();
            ui.need_update = true;
            break;
        case KEY_PLUS:
            reference = drive_reference();
            if(reference < REFERENCE_MAX) reference ++;
            drive_set_reference(reference);
            ui.need_update = true;
            break;
        case KEY_MINUS:
            reference = drive_reference();
            if(reference > 0) reference --;
            drive_set_reference(reference);
            ui.need_update = true;
            break;
        default:
            drive_gui_on_key_pressed(key);
            break;
    }
}

static void drive_ui_on_key_released(keycode_t key)
{
    drive_gui_on_key_released(key);
}


err_t drive_ui_init(drive_ui_init_t* ui_is)
{
    if(ui_is == NULL) return E_NULL_POINTER;
    
    RETURN_ERR_IF_FAIL(drive_ui_init_keypad(ui_is));
    RETURN_ERR_IF_FAIL(drive_ui_init_gui(ui_is));
    
    key_input_set_on_pressed_callback(drive_ui_on_key_pressed);
    key_input_set_on_released_callback(drive_ui_on_key_released);
    
    ui.need_update = true;
    ui.last_update_time = 0;
    ui.update_period = system_counter_ticks_per_sec() / 10;
    
    return E_NO_ERROR;
}


ALWAYS_INLINE static void drive_ui_update_update_time(void)
{
    ui.last_update_time = system_counter_ticks();
}

ALWAYS_INLINE static bool drive_ui_update_timeout(void)
{
    return system_counter_diff(&ui.last_update_time) >= ui.update_period;
}

static void drive_ui_update_leds(void)
{
    drive_kpd_leds_t leds = 0, leds_cur = drive_keypad_leds();
    drive_kpd_leds_t leds_to_on = DRIVE_KPD_LED_4;
    drive_kpd_leds_t leds_to_off = 0;

    if(drive_ready()) leds_to_on |= DRIVE_KPD_LED_2;
    else leds_to_off |= DRIVE_KPD_LED_2;

    if(drive_running()) {
        leds_to_on |= DRIVE_KPD_LED_3;
        leds_to_off |= DRIVE_KPD_LED_2;
    }
    else leds_to_off |= DRIVE_KPD_LED_3;

    if(drive_errors() != DRIVE_ERROR_NONE) leds_to_on |= DRIVE_KPD_LED_0;
    else leds_to_off |= DRIVE_KPD_LED_0;

    if(drive_warnings() != DRIVE_WARNING_NONE) leds_to_on |= DRIVE_KPD_LED_1;
    else leds_to_off |= DRIVE_KPD_LED_1;

    leds = (leds_cur | leds_to_on) & ~leds_to_off;
    
    if(leds != leds_cur) drive_keypad_set_leds(leds);
}

void drive_ui_process(void)
{
    if(drive_keypad_update()) drive_keypad_process();
    else drive_keypad_repeat();
    
    if(drive_ui_update_timeout()) ui.need_update = true;
    
    if(ui.need_update){
        drive_gui_update();
        
        drive_ui_update_leds();
        
        ui.need_update = false;
        drive_ui_update_update_time();
    }
}
