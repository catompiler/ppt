#include <stddef.h>

#include "drive_ui.h"
#include "drive.h"
#include "utils/utils.h"
#include "counter/counter.h"
#include "timers/timers.h"
#include <sys/time.h>


//! Тип структуры интерфейса привода.
typedef struct _Drive_Ui {
    counter_t update_period;
    counter_t last_update_time;
    bool need_update;
    timer_id_t upd_buz_tid;
} drive_ui_t;

//! Интерфейс привода.
static drive_ui_t ui;


static err_t drive_ui_init_keypad(drive_ui_init_t* ui_is)
{
    drive_keypad_init_t keypad_is;
    
    keypad_is.ioport = ui_is->ioport;
    keypad_is.reset_i2c_bus_proc = ui_is->reset_i2c_bus_proc;
    
    return drive_keypad_init(&keypad_is);
}

static err_t drive_ui_init_gui(drive_ui_init_t* ui_is)
{
    drive_gui_init_t gui_is;
    
    gui_is.tft = ui_is->tft;
    
    return drive_gui_init(&gui_is);
}

static void* drive_ui_timer_buzz_off_proc(void* arg)
{
    while(drive_keypad_pins_off(DRIVE_KPD_PIN_BUZZ) != E_NO_ERROR);
    
    return NULL;
}

static void* drive_ui_timer_buzz_on_proc(void* arg)
{
    while(drive_keypad_pins_on(DRIVE_KPD_PIN_BUZZ) != E_NO_ERROR);

    struct timeval tv_start;
    
    gettimeofday(&tv_start, NULL);
    tv_start.tv_sec += 0;
    tv_start.tv_usec += 100000;
    
    timers_add_timer(drive_ui_timer_buzz_off_proc, &tv_start, NULL, NULL, NULL);/**/
    
    return NULL;
}

static void drive_ui_on_key_pressed(keycode_t key)
{
    drive_ui_buzzer_beep(NULL);
    
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

void* drive_ui_buzzer_beep(void* arg) {
    if (drive_ui_sound_enabled()) {
        struct timeval tv_start;

        gettimeofday(&tv_start, NULL);
        tv_start.tv_sec += 0;
        tv_start.tv_usec += 100000;

        timers_add_timer(drive_ui_timer_buzz_on_proc, &tv_start, NULL, NULL, NULL);/**/
    }
    return NULL;
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
    
    ui.upd_buz_tid = timers_add_timer(drive_ui_update_buzzer, &tv, &timeout, NULL, NULL);/**/
    
    ui.need_update = true;
    ui.last_update_time = 0;
    ui.update_period = system_counter_ticks_per_sec() / 4;
    
    return E_NO_ERROR;
}

void drive_ui_deinit(void)
{
    if(ui.upd_buz_tid != INVALID_TIMER_ID){
        timers_remove_timer(ui.upd_buz_tid);
        ui.upd_buz_tid = INVALID_TIMER_ID;
    }
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
void* drive_ui_update_buzzer(void* arg)
{
    static int8_t buzcnt;
    buzcnt++;
    bool buzon = (((buzcnt % DRIVE_UI_BUZZER_SEQUECE_ALARM == 0) && (drive_errors() != DRIVE_ERROR_NONE)) \
                || ((buzcnt % DRIVE_UI_BUZZER_SEQUECE_WARNING == 0) && (drive_warnings() != DRIVE_WARNING_NONE)));
    if (buzon) {
        counter_t cur = system_counter_ticks();
        counter_t ticks_per_sec = system_counter_ticks_per_sec();
        counter_t mute_time = drive_gui_get_touch_menu_explorer() + ticks_per_sec * DRIVE_UI_BUZZER_MUTE_AFTER_TOUCH_SEC;
        if (cur >= mute_time) {
            drive_ui_buzzer_beep(NULL);
        }
    }
    
    return NULL;
}

void drive_ui_process(void)
{
    if(drive_keypad_update()) drive_keypad_process();
    else drive_keypad_repeat();
    
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
