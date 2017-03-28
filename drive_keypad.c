#include "drive_keypad.h"
#include "input/key_input.h"
#include "utils/utils.h"
#include "counter/counter.h"
#include "i2c/i2c.h"
#include <stddef.h>


//! Тип состояния обновления клавиатуры.
typedef enum _Drive_Kpd_Upd_KbdState {
    DRIVE_KPD_KBD_UPD_NONE = 0, //!< Нет обновления.
    DRIVE_KPD_KBD_UPD_UPDATED, //!< Обновлено.
    DRIVE_KPD_KBD_UPD_NEED_UPDATE, //!< Необходимо обновление.
    DRIVE_KPD_KBD_UPD_UPDATING //!< Обновление.
} drive_kpd_kbd_upd_state_t;

//! Тип кейпада привода.
typedef struct _Drive_Keypad {
    pca9555_t* ioport; //!< Порт ввода-вывода.
    counter_t last_async_io_time; //!< Последнее время обмена данными с PCA9555.
    volatile bool kbd_need_update; //!< Флаг необходимости обновления клавиатуры.
    drive_kpd_kbd_upd_state_t kbd_upd_state; //!< Состояние обновления клавиатуры.
    counter_t last_key_pressed_time; //!< Последнее время нажатия клавиши.
    counter_t last_key_repeat_time; //!< Последнее время повтора нажатия клавиши.
} drive_keypad_t;

//! Кейпад привода.
static drive_keypad_t keypad;


//! Число попыток обмена данными с PCA9555.
#define DRIVE_KEYPAD_IO_RETRIES_COUNT 10


static counter_t drive_keypad_io_timeout(void)
{
    static counter_t timeout = 0;
    if(timeout == 0) timeout = system_counter_ticks_per_sec() >> 6; // 15 мс.
    return timeout;
}

ALWAYS_INLINE static bool drive_keypad_async_io_done(void)
{
    return pca9555_done(keypad.ioport);
}

ALWAYS_INLINE static err_t drive_keypad_async_io_error(void)
{
    return pca9555_error(keypad.ioport);
}

ALWAYS_INLINE static bool drive_keypad_async_io_timeout(void)
{
    return system_counter_diff(&keypad.last_async_io_time) >= drive_keypad_io_timeout();
}

ALWAYS_INLINE static void drive_keypad_async_io_reset(void)
{
    pca9555_reset(keypad.ioport);
    i2c_bus_reset(pca9555_i2c_bus(keypad.ioport));
}

static err_t drive_keypad_wait_safe(void)
{
    counter_t counter = system_counter_ticks();
    
    while(i2c_bus_busy(pca9555_i2c_bus(keypad.ioport)) || pca9555_busy(keypad.ioport)){
        if(system_counter_diff(&counter) >= drive_keypad_io_timeout()){
            drive_keypad_async_io_reset();
            return E_BUSY;
        }
    }
    
    return pca9555_error(keypad.ioport);
}

static err_t drive_keypad_try_safe_async(err_t (*pca9555_io_proc)(pca9555_t*))
{
    drive_keypad_wait_safe();
    
    err_t err;
    size_t i;
    for(i = 0; i < DRIVE_KEYPAD_IO_RETRIES_COUNT; i ++){
        err = pca9555_io_proc(keypad.ioport);
        
        if(err == E_NO_ERROR){
            keypad.last_async_io_time = system_counter_ticks();
            return E_NO_ERROR;
        }
    }
    return err;
}

static err_t drive_keypad_try_safe_sync(err_t (*pca9555_io_proc)(pca9555_t*))
{
    drive_keypad_wait_safe();
    
    err_t err;
    size_t i;
    for(i = 0; i < DRIVE_KEYPAD_IO_RETRIES_COUNT; i ++){
        
        err = pca9555_io_proc(keypad.ioport);
        if(err != E_NO_ERROR) continue;
        
        err = drive_keypad_wait_safe();
        if(err == E_NO_ERROR) return E_NO_ERROR;
    }
    return err;
}

#define drive_keypad_try_safe(proc) drive_keypad_try_safe_sync(proc)


static err_t drive_keypad_ioport_init(void)
{
    pca9555_set_pins_direction(keypad.ioport, PCA9555_PIN_ALL, PCA9555_PIN_INPUT);
    pca9555_set_pins_direction(keypad.ioport, DRIVE_KPD_LED_ALL, PCA9555_PIN_OUTPUT);
    pca9555_set_pins_direction(keypad.ioport, DRIVE_KPD_PIN_ALL, PCA9555_PIN_OUTPUT);
    
    RETURN_ERR_IF_FAIL(drive_keypad_try_safe_sync(pca9555_write_pins_direction));
    
    pca9555_set_pins_state(keypad.ioport, PCA9555_PIN_ALL, PCA9555_PIN_ON);
    pca9555_set_pins_state(keypad.ioport, DRIVE_KPD_PIN_BUZZ, PCA9555_PIN_OFF);
    
    RETURN_ERR_IF_FAIL(drive_keypad_try_safe_sync(pca9555_write_pins_state));
    
    return E_NO_ERROR;//drive_keypad_try_safe_sync(pca9555_read_pins_state);
}

err_t drive_keypad_init(drive_keypad_init_t* keypad_is)
{
    if(keypad_is == NULL) return E_NULL_POINTER;
    if(keypad_is->ioport == NULL) return E_NULL_POINTER;
    
    keypad.ioport = keypad_is->ioport;
    keypad.last_async_io_time = 0;
    keypad.kbd_need_update = true;
    keypad.kbd_upd_state = DRIVE_KPD_KBD_UPD_NONE;
    keypad.last_key_pressed_time = 0;
    keypad.last_key_repeat_time = 0;
    
    RETURN_ERR_IF_FAIL(drive_keypad_ioport_init());
    RETURN_ERR_IF_FAIL(key_input_init());
    
    return E_NO_ERROR;
}

err_t drive_keypad_wait(void)
{
    return drive_keypad_wait_safe();
}

drive_kpd_leds_t drive_keypad_leds(void)
{
    return pca9555_pins_state(keypad.ioport, PCA9555_PIN_OFF) & DRIVE_KPD_LED_ALL;
}

err_t drive_keypad_set_leds(drive_kpd_leds_t leds)
{
    drive_kpd_leds_t old_leds = pca9555_pins_state(keypad.ioport, PCA9555_PIN_OFF);
    
    pca9555_set_pins_state(keypad.ioport, DRIVE_KPD_LED_ALL, PCA9555_PIN_ON);
    pca9555_set_pins_state(keypad.ioport, leds, PCA9555_PIN_OFF);
    
    err_t err = drive_keypad_try_safe(pca9555_write_pins_state);
    
    if(err != E_NO_ERROR){
        pca9555_set_pins_state(keypad.ioport, DRIVE_KPD_LED_ALL, PCA9555_PIN_ON);
        pca9555_set_pins_state(keypad.ioport, old_leds, PCA9555_PIN_OFF);
    }
    
    return err;
}

err_t drive_keypad_leds_on(drive_kpd_leds_t leds)
{
    drive_kpd_leds_t old_leds = pca9555_pins_state(keypad.ioport, PCA9555_PIN_OFF);
    
    pca9555_set_pins_state(keypad.ioport, leds, PCA9555_PIN_OFF);
    
    err_t err = drive_keypad_try_safe(pca9555_write_pins_state);
    
    if(err != E_NO_ERROR){
        pca9555_set_pins_state(keypad.ioport, DRIVE_KPD_LED_ALL, PCA9555_PIN_ON);
        pca9555_set_pins_state(keypad.ioport, old_leds, PCA9555_PIN_OFF);
    }
    
    return err;
}

err_t drive_keypad_leds_off(drive_kpd_leds_t leds)
{
    drive_kpd_leds_t old_leds = pca9555_pins_state(keypad.ioport, PCA9555_PIN_OFF);
    
    pca9555_set_pins_state(keypad.ioport, leds, PCA9555_PIN_ON);
    
    err_t err = drive_keypad_try_safe(pca9555_write_pins_state);
    
    if(err != E_NO_ERROR){
        pca9555_set_pins_state(keypad.ioport, old_leds, PCA9555_PIN_OFF);
    }
    
    return err;
}

err_t drive_keypad_pins_on(drive_kpd_pins_t pins)
{
    drive_kpd_pins_t old_pins = pca9555_pins_state(keypad.ioport, PCA9555_PIN_ON);
    
    pca9555_set_pins_state(keypad.ioport, pins, PCA9555_PIN_ON);
    
    err_t err = drive_keypad_try_safe(pca9555_write_pins_state);
    
    if(err != E_NO_ERROR){
        pca9555_set_pins_state(keypad.ioport, DRIVE_KPD_PIN_ALL, PCA9555_PIN_OFF);
        pca9555_set_pins_state(keypad.ioport, old_pins, PCA9555_PIN_ON);
    }
    
    return err;
}

err_t drive_keypad_pins_off(drive_kpd_pins_t pins)
{
    drive_kpd_pins_t old_pins = pca9555_pins_state(keypad.ioport, PCA9555_PIN_ON);
    
    pca9555_set_pins_state(keypad.ioport, pins, PCA9555_PIN_OFF);
    
    err_t err = drive_keypad_try_safe(pca9555_write_pins_state);
    
    if(err != E_NO_ERROR){
        pca9555_set_pins_state(keypad.ioport, DRIVE_KPD_PIN_ALL, PCA9555_PIN_OFF);
        pca9555_set_pins_state(keypad.ioport, old_pins, PCA9555_PIN_ON);
    }
    
    return err;
}

drive_kpd_keys_t drive_keypad_state(void)
{
    return pca9555_pins_state(keypad.ioport, PCA9555_PIN_OFF) & DRIVE_KPD_KEY_ALL;
}

void drive_keypad_pressed(void)
{
    keypad.kbd_need_update = true;
}

static counter_t drive_keypad_repeat_timeout(void)
{
    static counter_t timeout = 0;
    if(timeout == 0) timeout = system_counter_ticks_per_sec() >> 2; // 250 мс.
    return timeout;
}

static counter_t drive_keypad_pressed_timeout(void)
{
    static counter_t timeout = 0;
    if(timeout == 0) timeout = system_counter_ticks_per_sec(); // 1000 мс.
    return timeout;
}

ALWAYS_INLINE static void drive_keypad_update_repeat_time(void)
{
    keypad.last_key_repeat_time = system_counter_ticks();
}

ALWAYS_INLINE static void drive_keypad_update_pressed_time(void)
{
    keypad.last_key_pressed_time = system_counter_ticks();
}

ALWAYS_INLINE static bool drive_keypad_repeat_is_timeout(void)
{
    return system_counter_diff(&keypad.last_key_repeat_time) >= drive_keypad_repeat_timeout();
}

ALWAYS_INLINE static bool drive_keypad_pressed_is_timeout(void)
{
    return system_counter_diff(&keypad.last_key_pressed_time) >= drive_keypad_pressed_timeout();
}

void drive_keypad_process(void)
{
    static drive_kpd_keys_t ioport_old_pins = 0;
    drive_kpd_keys_t cur_pins = drive_keypad_state();
    drive_kpd_keys_t changed_pins = cur_pins ^ ioport_old_pins;
    
    if(changed_pins != 0){
        if(cur_pins & changed_pins & DRIVE_KPD_KEY_ESC   ) key_input_pressed(KEY_ESC);
        if(cur_pins & changed_pins & DRIVE_KPD_KEY_ENTER ) key_input_pressed(KEY_ENTER);
        if(cur_pins & changed_pins & DRIVE_KPD_KEY_MINUS ) key_input_pressed(KEY_MINUS);
        if(cur_pins & changed_pins & DRIVE_KPD_KEY_PLUS  ) key_input_pressed(KEY_PLUS);
        if(cur_pins & changed_pins & DRIVE_KPD_KEY_UP    ) key_input_pressed(KEY_UP);
        if(cur_pins & changed_pins & DRIVE_KPD_KEY_DOWN  ) key_input_pressed(KEY_DOWN);
        if(cur_pins & changed_pins & DRIVE_KPD_KEY_START ) key_input_pressed(KEY_START);
        if(cur_pins & changed_pins & DRIVE_KPD_KEY_STOP  ) key_input_pressed(KEY_STOP);

        if(~cur_pins & changed_pins & DRIVE_KPD_KEY_ESC   ) key_input_released(KEY_ESC);
        if(~cur_pins & changed_pins & DRIVE_KPD_KEY_ENTER ) key_input_released(KEY_ENTER);
        if(~cur_pins & changed_pins & DRIVE_KPD_KEY_MINUS ) key_input_released(KEY_MINUS);
        if(~cur_pins & changed_pins & DRIVE_KPD_KEY_PLUS  ) key_input_released(KEY_PLUS);
        if(~cur_pins & changed_pins & DRIVE_KPD_KEY_UP    ) key_input_released(KEY_UP);
        if(~cur_pins & changed_pins & DRIVE_KPD_KEY_DOWN  ) key_input_released(KEY_DOWN);
        if(~cur_pins & changed_pins & DRIVE_KPD_KEY_START ) key_input_released(KEY_START);
        if(~cur_pins & changed_pins & DRIVE_KPD_KEY_STOP  ) key_input_released(KEY_STOP);

        drive_keypad_update_pressed_time();
        drive_keypad_update_repeat_time();

        ioport_old_pins = cur_pins;
    }
}

void drive_keypad_repeat(void)
{
    // Если неактуальные данные - возврат.
    if(keypad.kbd_upd_state != DRIVE_KPD_KBD_UPD_UPDATED){
        drive_keypad_update_pressed_time();
        drive_keypad_update_repeat_time();
        return;
    }
    
    if(!drive_keypad_pressed_is_timeout()) return;
    if(!drive_keypad_repeat_is_timeout()) return;
    
    drive_kpd_keys_t cur_pins = drive_keypad_state();
    
    if(cur_pins & DRIVE_KPD_KEY_ESC   ) key_input_pressed(KEY_ESC);
    if(cur_pins & DRIVE_KPD_KEY_ENTER ) key_input_pressed(KEY_ENTER);
    if(cur_pins & DRIVE_KPD_KEY_MINUS ) key_input_pressed(KEY_MINUS);
    if(cur_pins & DRIVE_KPD_KEY_PLUS  ) key_input_pressed(KEY_PLUS);
    if(cur_pins & DRIVE_KPD_KEY_UP    ) key_input_pressed(KEY_UP);
    if(cur_pins & DRIVE_KPD_KEY_DOWN  ) key_input_pressed(KEY_DOWN);
    if(cur_pins & DRIVE_KPD_KEY_START ) key_input_pressed(KEY_START);
    if(cur_pins & DRIVE_KPD_KEY_STOP  ) key_input_pressed(KEY_STOP);
    
    drive_keypad_update_repeat_time();
}

bool drive_keypad_update(void)
{
    switch(keypad.kbd_upd_state){
        case DRIVE_KPD_KBD_UPD_NONE:
        case DRIVE_KPD_KBD_UPD_UPDATED:
            if(keypad.kbd_need_update == false) break;
            keypad.kbd_upd_state = DRIVE_KPD_KBD_UPD_NEED_UPDATE;
            keypad.kbd_need_update = false;
        case DRIVE_KPD_KBD_UPD_NEED_UPDATE:
            if(drive_keypad_try_safe_async(pca9555_read_pins_state) == E_NO_ERROR){
                keypad.kbd_upd_state = DRIVE_KPD_KBD_UPD_UPDATING;
            }
            break;
        case DRIVE_KPD_KBD_UPD_UPDATING:
            if(drive_keypad_async_io_done()){
                if(drive_keypad_async_io_error() == E_NO_ERROR){
                    keypad.kbd_upd_state = DRIVE_KPD_KBD_UPD_UPDATED;
                    return true;
                }else{
                    keypad.kbd_upd_state = DRIVE_KPD_KBD_UPD_NEED_UPDATE;
                }
            }else if(drive_keypad_async_io_timeout()){
                drive_keypad_async_io_reset();
                keypad.kbd_upd_state = DRIVE_KPD_KBD_UPD_NEED_UPDATE;
            }
            break;
    }
    
    return false;
}
