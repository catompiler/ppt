#include <stddef.h>

#include "drive_ui.h"
#include "drive.h"
#include "drive_tasks.h"
#include "utils/utils.h"
#include <sys/time.h>


#define DRIVE_UI_UPDATE_PERIOD_DEFAULT_US 250000


//! Тип структуры интерфейса привода.
typedef struct _Drive_Ui {
    struct timeval update_period;
    struct timeval last_update_time;
    bool need_update;
} drive_ui_t;

//! Интерфейс привода.
static drive_ui_t ui;


static err_t drive_ui_init_keypad(drive_ui_init_t* ui_is)
{
    drive_keypad_init_t keypad_is;
    
    keypad_is.ioport = ui_is->ioport;
    keypad_is.ioport_timeout = ui_is->ioport_timeout;
    keypad_is.reset_i2c_bus_proc = ui_is->reset_i2c_bus_proc;
    keypad_is.ioport_i2c_watchdog = ui_is->ioport_i2c_watchdog;
    
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
    drive_ui_buzzer_beep();
    
    switch(key){
        case KEY_START:
            drive_start();
            ui.need_update = true;
            break;
        case KEY_STOP:
            drive_stop();
            ui.need_update = true;
            break;
        default:
            drive_gui_on_key_pressed(key);
            break;
    }
}

void drive_ui_buzzer_beep(void) {
    if (drive_ui_sound_enabled()) {
        
        drive_task_buzz_beep();
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
    
    // обновление звукового оповещения по таймеру
    struct timeval timeout = {1, 0};
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    timeradd(&tv, &timeout, &tv);
    
    ui.need_update = true;
    
    ui.last_update_time.tv_sec = 0;
    ui.last_update_time.tv_usec = 0;
    
    ui.update_period.tv_sec = 0;
    ui.update_period.tv_usec = DRIVE_UI_UPDATE_PERIOD_DEFAULT_US;
    
    return E_NO_ERROR;
}

err_t drive_ui_setup(void)
{
    RETURN_ERR_IF_FAIL(drive_keypad_setup());
    RETURN_ERR_IF_FAIL(drive_gui_setup());
    
    return E_NO_ERROR;
}

void drive_ui_deinit(void)
{
}

ALWAYS_INLINE static void drive_ui_update_update_time(void)
{
    gettimeofday(&ui.last_update_time, NULL);
}

static bool drive_ui_update_timeout(void)
{
    struct timeval cur_tv;
    gettimeofday(&cur_tv, NULL);
    
    struct timeval timeout;
    timeradd(&ui.update_period, &ui.last_update_time, &timeout);
    
    //return system_counter_diff(&ui.last_update_time) >= ui.update_period;
    return timercmp(&cur_tv, &timeout, >=);
}

static void drive_ui_update_leds(void)
{
    drive_kpd_leds_t leds = 0, leds_cur = drive_keypad_leds();
    drive_kpd_leds_t leds_to_on = DRIVE_KPD_LED_4;
    drive_kpd_leds_t leds_to_off = 0;

    if(drive_ready()) leds_to_on |= DRIVE_KPD_LED_3;
    else leds_to_off |= DRIVE_KPD_LED_3;

    if(drive_running()) {
        leds_to_on |= DRIVE_KPD_LED_2;
        leds_to_off |= DRIVE_KPD_LED_3;
    }
    else leds_to_off |= DRIVE_KPD_LED_2;

    if(drive_errors() != DRIVE_ERROR_NONE) leds_to_on |= DRIVE_KPD_LED_0;
    else leds_to_off |= DRIVE_KPD_LED_0;

    if(drive_warnings() != DRIVE_WARNING_NONE) leds_to_on |= DRIVE_KPD_LED_1;
    else leds_to_off |= DRIVE_KPD_LED_1;

    leds = (leds_cur | leds_to_on) & ~leds_to_off;
    
    /*if(leds != leds_cur)*/ drive_keypad_set_leds(leds);
}

/**
 * Обновление звукового оповещения
 */
void drive_ui_update_buzzer(void)
{
    static int8_t buzcnt;
    buzcnt++;
    bool buzon = (((buzcnt % DRIVE_UI_BUZZER_SEQUECE_ALARM == 0) && (drive_errors() != DRIVE_ERROR_NONE)) \
                || ((buzcnt % DRIVE_UI_BUZZER_SEQUECE_WARNING == 0) && (drive_warnings() != DRIVE_WARNING_NONE)));
    if (buzon) {
        
        struct timeval cur_tv;
        gettimeofday(&cur_tv, NULL);
        
        struct timeval touch_tv;
        drive_gui_get_touch_menu_explorer(&touch_tv);
        
        touch_tv.tv_sec += DRIVE_UI_BUZZER_MUTE_AFTER_TOUCH_SEC;
        
        if(timercmp(&cur_tv, &touch_tv, >=)){
            drive_ui_buzzer_beep();
        }
        
        // System counter removed.
        //counter_t cur = system_counter_ticks();
        //counter_t ticks_per_sec = system_counter_ticks_per_sec();
        //counter_t mute_time = drive_gui_get_touch_menu_explorer() + ticks_per_sec * DRIVE_UI_BUZZER_MUTE_AFTER_TOUCH_SEC;
        //if (cur >= mute_time) {
        //    drive_ui_buzzer_beep(NULL);
        //}
    }
}

void drive_ui_process(void)
{
    if(drive_keypad_update()) drive_keypad_process();
    else drive_keypad_repeat();
    
    drive_ui_update_buzzer();
    
    if(drive_ui_update_timeout()) ui.need_update = true;
    
    if(ui.need_update){
        // приоритет обновления светодиодов
        drive_ui_update_leds();
        // обновление графического интерфейса
        drive_gui_update();
        ui.need_update = false;
        drive_ui_update_update_time();
    }
}

bool drive_ui_sound_enabled(void) {
    return settings_valueu(PARAM_ID_GUI_BUZZER) == GUI_BUZZER_ON;
}

bool drive_ui_sound_disabled(void) {
    return !drive_ui_sound_enabled();
}
