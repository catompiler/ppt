#include "drive_keypad.h"
#include "input/key_input.h"
#include "utils/utils.h"
#include "timers/timers.h"
#include "i2c/i2c.h"
#include <stddef.h>
#include <string.h>
#include <sys/time.h>


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
    struct timeval ioport_timeout; //!< Таймаут обмена данными с портом ввода-вывода.
    reset_i2c_bus_proc_t reset_i2c_bus_proc; //!< Функция сброса i2c.
    struct timeval last_update_time; //!< Последнее время обновления клавиатуры.
    volatile bool kbd_need_update; //!< Флаг необходимости обновления клавиатуры.
    drive_kpd_kbd_upd_state_t kbd_upd_state; //!< Состояние обновления клавиатуры.
    struct timeval last_key_pressed_time; //!< Последнее время нажатия клавиши.
    struct timeval last_key_repeat_time; //!< Последнее время повтора нажатия клавиши.
} drive_keypad_t;

//! Кейпад привода.
static drive_keypad_t keypad;


//! Таймаут обновления, с.
#define DRIVE_KEYPAD_UPDATE_TIMEOUT_S 0
//! Таймаут обновления, мкс.
#define DRIVE_KEYPAD_UPDATE_TIMEOUT_US 15000


//! Таймаут нажатия, с.
#define DRIVE_KEYPAD_KEY_PRESSED_TIMEOUT_S 1
//! Таймаут нажатия, мкс.
#define DRIVE_KEYPAD_KEY_PRESSED_TIMEOUT_US 0

//! Таймаут повторения нажатия, с.
#define DRIVE_KEYPAD_KEY_REPEAT_TIMEOUT_S 0
//! Таймаут повторения нажатия, мкс.
#define DRIVE_KEYPAD_KEY_REPEAT_TIMEOUT_US 250000

//! Число попыток обмена данными с PCA9555.
#define DRIVE_KEYPAD_IO_RETRIES_COUNT 10

//! Таймаут обмена данными с PCA9555.
#define DRIVE_KEYPAD_IO_TIMEOUT_DEFAULT_US 100000




ALWAYS_INLINE static void drive_keypad_update_timeout(struct timeval* timeout)
{
    timeout->tv_sec = DRIVE_KEYPAD_UPDATE_TIMEOUT_S;
    timeout->tv_usec = DRIVE_KEYPAD_UPDATE_TIMEOUT_US;
}

ALWAYS_INLINE static void drive_keypad_update_last_update_time(void)
{
    gettimeofday(&keypad.last_update_time, NULL);
}

static bool drive_keypad_kbd_update_timeout(void)
{
    struct timeval tv = {0, 0};
    struct timeval timeout = {0, 0};
    
    drive_keypad_update_timeout(&timeout);
    gettimeofday(&tv, NULL);
    
    timersub(&tv, &keypad.last_update_time, &tv);
    
    return timercmp(&tv, &timeout, >);
}

ALWAYS_INLINE static void drive_keypad_io_reset(void)
{
    if(keypad.reset_i2c_bus_proc){
        keypad.reset_i2c_bus_proc();
    }else{
        pca9555_timeout(keypad.ioport);
        i2c_bus_reset(pca9555_i2c_bus(keypad.ioport));
    }
}

static void* drive_keypad_reset_ioport_task(void* arg)
{
    drive_keypad_io_reset();
    
    return NULL;
}

static err_t drive_keypad_try_safe(err_t (*pca9555_io_proc)(pca9555_t*))
{
    // Текущее время.
    struct timeval cur_time = {0};
    // Идентификатор таймера.
    timer_id_t tid = INVALID_TIMER_ID;
    // Время истечения тайм-аута.
    struct timeval timeout = {0, 0};
    
    err_t err = E_IO_ERROR;
    size_t i;
    for(i = 0; i < DRIVE_KEYPAD_IO_RETRIES_COUNT; i ++){
        
        // Получим время истечения тайм-аута.
        gettimeofday(&cur_time, NULL);
        timeradd(&cur_time, &keypad.ioport_timeout, &timeout);
        
        tid = timers_add_timer(drive_keypad_reset_ioport_task, &timeout, NULL, NULL, NULL);
        if(tid != INVALID_TIMER_ID){
            
            err = pca9555_io_proc(keypad.ioport);
            if(err == E_NO_ERROR){
                err = pca9555_wait(keypad.ioport);
            }
            
            timers_remove_timer(tid);
        }
        
        if(err == E_NO_ERROR) break;
    }
    return err;
}

static err_t drive_keypad_ioport_init(void)
{
    pca9555_set_pins_direction(keypad.ioport, PCA9555_PIN_ALL, PCA9555_PIN_INPUT);
    pca9555_set_pins_direction(keypad.ioport, DRIVE_KPD_LED_ALL, PCA9555_PIN_OUTPUT);
    pca9555_set_pins_direction(keypad.ioport, DRIVE_KPD_PIN_ALL, PCA9555_PIN_OUTPUT);
    
    RETURN_ERR_IF_FAIL(drive_keypad_try_safe(pca9555_write_pins_direction));
    
    pca9555_set_pins_state(keypad.ioport, PCA9555_PIN_ALL, PCA9555_PIN_ON);
    pca9555_set_pins_state(keypad.ioport, DRIVE_KPD_PIN_BUZZ, PCA9555_PIN_OFF);
    
    RETURN_ERR_IF_FAIL(drive_keypad_try_safe(pca9555_write_pins_state));
    
    return E_NO_ERROR;
}

err_t drive_keypad_init(drive_keypad_init_t* keypad_is)
{
    if(keypad_is == NULL) return E_NULL_POINTER;
    if(keypad_is->ioport == NULL) return E_NULL_POINTER;
    
    memset(&keypad, 0x0, sizeof(drive_keypad_t));
    
    keypad.ioport = keypad_is->ioport;
    keypad.reset_i2c_bus_proc = keypad_is->reset_i2c_bus_proc;
    keypad.kbd_need_update = true;
    keypad.kbd_upd_state = DRIVE_KPD_KBD_UPD_NONE;
    
    if(keypad_is->ioport_timeout){
        memcpy(&keypad.ioport_timeout, keypad_is->ioport_timeout, sizeof(struct timeval));
    }else{
        keypad.ioport_timeout.tv_sec = 0;
        keypad.ioport_timeout.tv_usec = DRIVE_KEYPAD_IO_TIMEOUT_DEFAULT_US;
    }
    
    RETURN_ERR_IF_FAIL(drive_keypad_ioport_init());
    RETURN_ERR_IF_FAIL(key_input_init());
    
    return E_NO_ERROR;
}

err_t drive_keypad_wait(void)
{
    return drive_keypad_try_safe(pca9555_wait);
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

ALWAYS_INLINE static void drive_keypad_repeat_timeout(struct timeval* timeout)
{
    timeout->tv_sec = DRIVE_KEYPAD_KEY_REPEAT_TIMEOUT_S;
    timeout->tv_usec = DRIVE_KEYPAD_KEY_REPEAT_TIMEOUT_US;
}

ALWAYS_INLINE static void drive_keypad_pressed_timeout(struct timeval* timeout)
{
    timeout->tv_sec = DRIVE_KEYPAD_KEY_PRESSED_TIMEOUT_S;
    timeout->tv_usec = DRIVE_KEYPAD_KEY_PRESSED_TIMEOUT_US;
}

ALWAYS_INLINE static void drive_keypad_update_repeat_time(void)
{
    gettimeofday(&keypad.last_key_repeat_time, NULL);
}

ALWAYS_INLINE static void drive_keypad_update_pressed_time(void)
{
    gettimeofday(&keypad.last_key_pressed_time, NULL);
}

static bool drive_keypad_repeat_is_timeout(void)
{
    struct timeval tv = {0, 0};
    struct timeval timeout = {0, 0};
    
    drive_keypad_repeat_timeout(&timeout);
    gettimeofday(&tv, NULL);
    
    timersub(&tv, &keypad.last_key_repeat_time, &tv);
    
    return timercmp(&tv, &timeout, >);
}

static bool drive_keypad_pressed_is_timeout(void)
{
    struct timeval tv = {0, 0};
    struct timeval timeout = {0, 0};
    
    drive_keypad_pressed_timeout(&timeout);
    gettimeofday(&tv, NULL);
    
    timersub(&tv, &keypad.last_key_pressed_time, &tv);
    
    return timercmp(&tv, &timeout, >);
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

static bool drive_keypad_update_sync(void)
{
    switch(keypad.kbd_upd_state){
        case DRIVE_KPD_KBD_UPD_NONE:
        case DRIVE_KPD_KBD_UPD_UPDATED:
            if(drive_keypad_kbd_update_timeout()){
                keypad.kbd_need_update = true;
            }
            if(keypad.kbd_need_update == false) break;
            keypad.kbd_upd_state = DRIVE_KPD_KBD_UPD_NEED_UPDATE;
            //keypad.kbd_need_update = false;
        case DRIVE_KPD_KBD_UPD_NEED_UPDATE:
            if(drive_keypad_try_safe(pca9555_read_pins_state) == E_NO_ERROR){
                keypad.kbd_upd_state = DRIVE_KPD_KBD_UPD_UPDATING;
            }else{
                break;
            }
        case DRIVE_KPD_KBD_UPD_UPDATING:
            keypad.kbd_upd_state = DRIVE_KPD_KBD_UPD_UPDATED;
            keypad.kbd_need_update = false;
            drive_keypad_update_last_update_time();
            return true;
    }
    
    return false;
}

bool drive_keypad_update(void)
{
    return drive_keypad_update_sync();
}

void drive_keypad_buzzer_toggle(void)
{
    pca9555_pin_state_t cur = pca9555_pin_output_state(keypad.ioport, DRIVE_KPD_PIN_BUZZ);
    pca9555_set_pins_state(keypad.ioport, DRIVE_KPD_PIN_BUZZ, (cur == PCA9555_PIN_ON) ? PCA9555_PIN_OFF : PCA9555_PIN_ON);
}

void drive_keypad_buzzer_off(void)
{
    pca9555_set_pins_state(keypad.ioport, DRIVE_KPD_PIN_BUZZ, PCA9555_PIN_OFF);
}

void drive_keypad_buzzer_on(void)
{
    pca9555_set_pins_state(keypad.ioport, DRIVE_KPD_PIN_BUZZ, PCA9555_PIN_ON);
}
