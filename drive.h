/**
 * @file drive.h Реализация логики привода.
 */

#ifndef DRIVE_H
#define	DRIVE_H

#include "stm32f10x.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "errors/errors.h"
#include "defs/defs.h"
#include "drive_phase_state.h"
#include "drive_power.h"
#include "drive_triacs.h"
#include "drive_regulator.h"
#include "drive_dio.h"




/*
 * Конфигурация таймера для искусственных датчиков нуля..
 */
//! Число тиков.
#define DRIVE_NULL_TIMER_CNT_TICKS (40000)
//! Период в микросекундах.
#define DRIVE_NULL_TIMER_CNT_PERIOD_US (6667)
//! Период.
#define DRIVE_NULL_TIMER_CNT_PERIOD (DRIVE_NULL_TIMER_CNT_TICKS - 1)
//! Предделитель.
#define DRIVE_NULL_TIMER_CNT_PRESCALER (12 - 1)
//! Частота срабатывания таймера нуля.
#define DRIVE_NULL_TIMER_FREQ (DRIVE_POWER_FREQ * 3)
//! Угол таймера нуля.
#define DRIVE_NULL_TIMER_ANGLE (120)
//! Отклонение таймера нуля для синхронизации.
#define DRIVE_NULL_TIMER_OFFSET_TICKS_MAX (750) // 125 мкс
//! Коэффициент тик/градус отклонения.
#define DRIVE_NULL_TIMER_TICKS_PER_DEG 333//400



//! Флаги привода - тип флага.
typedef enum _Drive_Flag {
    DRIVE_FLAG_NONE             = 0x0,
    DRIVE_FLAG_POWER_CALIBRATED = 0x1,
    DRIVE_FLAG_POWER_DATA_AVAIL = 0x2
} drive_flag_t;

//! Тип флагов привода.
typedef uint32_t drive_flags_t;

//! Тип статуса привода.
typedef enum _Drive_Status {
    DRIVE_STATUS_INIT       = 0, //!< Инициализация.
    DRIVE_STATUS_IDLE       = 1, //!< Простой (готовность).
    DRIVE_STATUS_RUN        = 2, //!< Работа.
    DRIVE_STATUS_ERROR      = 3  //!< Ошибка.
} drive_status_t;

//! Тип состояния машины состояний привода.
typedef enum _Drive_State {
    DRIVE_STATE_INIT        = 0, //!< Инициализация.
    DRIVE_STATE_CALIBRATION = 1, //!< Калибровка питания.
    DRIVE_STATE_IDLE        = 2, //!< Простой (готовность).
    DRIVE_STATE_START       = 3, //!< Запуск.
    DRIVE_STATE_RUN         = 4, //!< Работа.
    DRIVE_STATE_STOP        = 5, //!< Останов.
    DRIVE_STATE_STOP_ERROR  = 6, //!< Останов при ошибке.
    DRIVE_STATE_ERROR       = 7  //!< Ошибка.
} drive_state_t;

//! Тип ошибки привода.
typedef enum _Drive_Error {
    DRIVE_ERROR_NONE                 = 0x0, //!< Нет ошибки.
    DRIVE_ERROR_POWER_DATA_NOT_AVAIL = 0x1, //!< Данные питания не поступают с АЦП.
    DRIVE_ERROR_POWER_INVALID        = 0x2, //!< Неправильные значения питания, см. drive_power_error_t.
    DRIVE_ERROR_EMERGENCY_STOP       = 0x4, //!< Аварийный останов (Грибок).
    DRIVE_ERROR_PHASE                = 0x8, //!< Ошибка фаз.
    DRIVE_ERROR_THERMAL_OVERLOAD     = 0x10, //!< Тепловая защита.
    DRIVE_ERROR_ROT_BREAK            = 0x20 //!< Обрыв якоря.
} drive_error_t;

//! Тип ошибок привода.
typedef uint32_t drive_errors_t;

//! Тип ошибки питания привода.
typedef enum _Drive_Power_Error {
    DRIVE_POWER_ERROR_NONE           = 0x0,
    DRIVE_POWER_ERROR_UNDERFLOW_Ua   = 0x1,
    DRIVE_POWER_ERROR_OVERFLOW_Ua    = 0x2,
    DRIVE_POWER_ERROR_UNDERFLOW_Ub   = 0x4,
    DRIVE_POWER_ERROR_OVERFLOW_Ub    = 0x8,
    DRIVE_POWER_ERROR_UNDERFLOW_Uc   = 0x10,
    DRIVE_POWER_ERROR_OVERFLOW_Uc    = 0x20,
    DRIVE_POWER_ERROR_UNDERFLOW_Urot = 0x40,
    DRIVE_POWER_ERROR_OVERFLOW_Urot  = 0x80,
    DRIVE_POWER_ERROR_UNDERFLOW_Ia   = 0x100,
    DRIVE_POWER_ERROR_OVERFLOW_Ia    = 0x200,
    DRIVE_POWER_ERROR_UNDERFLOW_Ib   = 0x400,
    DRIVE_POWER_ERROR_OVERFLOW_Ib    = 0x800,
    DRIVE_POWER_ERROR_UNDERFLOW_Ic   = 0x1000,
    DRIVE_POWER_ERROR_OVERFLOW_Ic    = 0x2000,
    DRIVE_POWER_ERROR_UNDERFLOW_Irot = 0x4000,
    DRIVE_POWER_ERROR_OVERFLOW_Irot  = 0x8000,
    DRIVE_POWER_ERROR_UNDERFLOW_Iexc = 0x10000,
    DRIVE_POWER_ERROR_OVERFLOW_Iexc  = 0x20000,
    DRIVE_POWER_ERROR_UNDERFLOW_Ifan = 0x40000,
    DRIVE_POWER_ERROR_OVERFLOW_Ifan  = 0x80000,
    DRIVE_POWER_ERROR_UNDERFLOW_Iref = 0x100000,
    DRIVE_POWER_ERROR_OVERFLOW_Iref  = 0x200000,
    DRIVE_POWER_ERROR_IDLE_Ia        = 0x400000,
    DRIVE_POWER_ERROR_IDLE_Ib        = 0x800000,
    DRIVE_POWER_ERROR_IDLE_Ic        = 0x1000000,
    DRIVE_POWER_ERROR_IDLE_Urot      = 0x2000000,
    DRIVE_POWER_ERROR_IDLE_Irot      = 0x4000000,
    DRIVE_POWER_ERROR_IDLE_Iexc      = 0x8000000,
    DRIVE_POWER_ERROR_ALL            = 0xfffffff
} drive_power_error_t;

//! Тип ошибок питания привода.
typedef uint32_t drive_power_errors_t;

typedef enum _Drive_Warning {
    DRIVE_WARNING_NONE  = 0x0, //!< Нет предупреждений.
    DRIVE_WARNING_POWER = 0x1,  //!< Предупреждение по питанию.
    DRIVE_WARNING_THERMAL_OVERLOAD = 0x400000 //!< Перегрев.
} drive_warning_t;

//! Тип предупреждений привода.
typedef uint32_t drive_warnings_t;

typedef enum _Drive_Power_Warning {
    DRIVE_POWER_WARNING_NONE           = 0x0,
    DRIVE_POWER_WARNING_UNDERFLOW_Ua   = 0x1,
    DRIVE_POWER_WARNING_OVERFLOW_Ua    = 0x2,
    DRIVE_POWER_WARNING_UNDERFLOW_Ub   = 0x4,
    DRIVE_POWER_WARNING_OVERFLOW_Ub    = 0x8,
    DRIVE_POWER_WARNING_UNDERFLOW_Uc   = 0x10,
    DRIVE_POWER_WARNING_OVERFLOW_Uc    = 0x20,
    DRIVE_POWER_WARNING_UNDERFLOW_Urot = 0x40,
    DRIVE_POWER_WARNING_OVERFLOW_Urot  = 0x80,
    DRIVE_POWER_WARNING_UNDERFLOW_Ia   = 0x100,
    DRIVE_POWER_WARNING_OVERFLOW_Ia    = 0x200,
    DRIVE_POWER_WARNING_UNDERFLOW_Ib   = 0x400,
    DRIVE_POWER_WARNING_OVERFLOW_Ib    = 0x800,
    DRIVE_POWER_WARNING_UNDERFLOW_Ic   = 0x1000,
    DRIVE_POWER_WARNING_OVERFLOW_Ic    = 0x2000,
    DRIVE_POWER_WARNING_UNDERFLOW_Irot = 0x4000,
    DRIVE_POWER_WARNING_OVERFLOW_Irot  = 0x8000,
    DRIVE_POWER_WARNING_UNDERFLOW_Iexc = 0x10000,
    DRIVE_POWER_WARNING_OVERFLOW_Iexc  = 0x20000,
    DRIVE_POWER_WARNING_UNDERFLOW_Ifan = 0x40000,
    DRIVE_POWER_WARNING_OVERFLOW_Ifan  = 0x80000,
    DRIVE_POWER_WARNING_UNDERFLOW_Iref = 0x100000,
    DRIVE_POWER_WARNING_OVERFLOW_Iref  = 0x200000,
    DRIVE_POWER_WARNING_IDLE_Ia        = 0x400000,
    DRIVE_POWER_WARNING_IDLE_Ib        = 0x800000,
    DRIVE_POWER_WARNING_IDLE_Ic        = 0x1000000,
    DRIVE_POWER_WARNING_IDLE_Urot      = 0x2000000,
    DRIVE_POWER_WARNING_IDLE_Irot      = 0x4000000,
    DRIVE_POWER_WARNING_IDLE_Iexc      = 0x8000000,
    DRIVE_POWER_WARNING_ALL            = 0xfffffff
} drive_power_warning_t;

//! Тип предупреждений питания привода.
typedef uint32_t drive_power_warnings_t;

//! Перечисление состояний инициализации привода.
typedef enum _Drive_Init_State {
    DRIVE_INIT_NONE        = 0,
    DRIVE_INIT_BEGIN       = 1,
    DRIVE_INIT_WAIT_PHASES = 2,
    DRIVE_INIT_WAIT_POWER  = 3,
    DRIVE_INIT_DONE        = 4
} drive_init_state_t;

//! Перечисление состояний калибровки питания.
typedef enum _Drive_Power_Calibration {
    DRIVE_PWR_CALIBRATION_NONE    = 0, //!< Нет калибровки.
    DRIVE_PWR_CALIBRATION_BEGIN   = 1, //!< Нужно выполнить калибровку.
    DRIVE_PWR_CALIBRATION_RUNNING = 2, //!< Калибровка выполняется.
    DRIVE_PWR_CALIBRATION_DONE    = 3  //!< Калибровка выполнена.
} drive_power_calibration_t;

//! Перечисление состояний запуска привода.
typedef enum _Drive_Starting {
    DRIVE_STARTING_NONE     = 0, //!< Не запускается.
    DRIVE_STARTING_BEGIN    = 1, //!< Нужно запустить.
    DRIVE_STARTING_WAIT_EXC = 2, //!< Ожидание возбуждения.
    DRIVE_STARTING_RAMP     = 3, //!< Разгон.
    DRIVE_STARTING_DONE     = 4  //!< Запущен.
} drive_starting_t;

//! Перечисление состояний останова привода.
typedef enum _Drive_Stopping {
    DRIVE_STOPPING_NONE     = 0, //!< Не останавливается.
    DRIVE_STOPPING_BEGIN    = 1, //!< Нужно остановить.
    DRIVE_STOPPING_RAMP     = 2, //!< Торможение.
    DRIVE_STOPPING_WAIT_ROT = 3, //!< Ожидане остановки якоря.
    DRIVE_STOPPING_WAIT_EXC = 4, //!< Ожидане возвращения к нулю возбуждения.
    DRIVE_STOPPING_DONE     = 5  //!< Остановлен.
} drive_stopping_t;

//! Перечисление состояний останова привода при ошибке.
typedef enum _Drive_Err_Stopping {
    DRIVE_ERR_STOPPING_NONE     = 0, //!< Не останавливается.
    DRIVE_ERR_STOPPING_BEGIN    = 1, //!< Нужно остановить.
    DRIVE_ERR_STOPPING_WAIT_ROT = 2, //!< Ожидане остановки якоря.
    DRIVE_ERR_STOPPING_WAIT_EXC = 3, //!< Ожидане возвращения к нулю возбуждения.
    DRIVE_ERR_STOPPING_DONE     = 4//!< Остановлен.
} drive_err_stopping_t;

//! Перечисление типа останова привода.
typedef enum _Drive_Stop_Mode {
    DRIVE_STOP_MODE_NORMAL = 0, //!< Нормальный останов.
    DRIVE_STOP_MODE_FAST   = 1, //!< Быстрый останов.
    DRIVE_STOP_MODE_COAST  = 2, //!< Останов выбегом.
} drive_stop_mode_t;

//! Фронт сигнала датчика нуля.
typedef enum _Null_Sensor_Edge {
    NULL_SENSOR_EDGE_LEADING = 0, //!< Передний фронт.
    NULL_SENSOR_EDGE_TRAILING = 1 //!< Задний фронт.
} null_sensor_edge_t;



//! Каллбэк возникновения ошибки привода.
typedef void (*drive_error_callback_t)(void);

//! Каллбэк возникновения предупреждения привода.
typedef void (*drive_warning_callback_t)(void);


/**
 * Инициализирует привод.
 * @return Код ошибки.
 */
extern err_t drive_init(void);

/**
 * Обновляет настройки привода.
 * @return Код ошибки.
 */
extern err_t drive_update_settings(void);

/**
 * Получает состояние флага привода.
 * @param flag Флаг.
 * @return Состояние флага.
 */
extern bool drive_flag(drive_flag_t flag);

/**
 * Получает состояние флагов привода.
 * @return Состояние флагов привода.
 */
extern drive_flags_t drive_flags(void);

/**
 * Получает статус привода.
 * @return Статус привода.
 */
extern drive_status_t drive_status(void);

/**
 * Получает состояние привода.
 * @return Состояние привода.
 */
extern drive_state_t drive_state(void);

/**
 * Получает наличие ошибки привода.
 * @param error Ошибка привода.
 * @return Наличие ошибки привода.
 */
extern bool drive_error(drive_error_t error);

/**
 * Получает ошибки привода.
 * @return Ошибки привода.
 */
extern drive_errors_t drive_errors(void);

/**
 * Получает наличие предупреждения привода.
 * @param warning Предупреждение привода.
 * @return Наличие предупреждения привода.
 */
extern bool drive_warning(drive_warning_t warning);

/**
 * Получает предупреждения привода.
 * @return Предупреждения привода.
 */
extern drive_warnings_t drive_warnings(void);

/**
 * Получает наличие ошибки питания привода.
 * @param error Ошибка питания привода.
 * @return Наличие ошибки питания привода.
 */
extern bool drive_power_error(drive_power_error_t error);

/**
 * Получает ошибки питания привода.
 * @return Ошибки питания привода.
 */
extern drive_power_errors_t drive_power_errors(void);

/**
 * Получает наличие предупреждения питания привода.
 * @param warning Предупреждение питания привода.
 * @return Наличие предупреждения питания привода.
 */
extern bool drive_power_warning(drive_power_warning_t warning);

/**
 * Получает предупреждения питания привода.
 * @return Предупреждения питания привода.
 */
extern drive_power_warnings_t drive_power_warnings(void);

/**
 * Получает состояние инициализации привода.
 * @return Состояние инициализации привода.
 */
extern drive_init_state_t drive_init_state(void);

/**
 * Получает состояние калибровки питания привода.
 * @return Состояние калибровки питания привода.
 */
extern drive_power_calibration_t drive_power_calibration(void);

/**
 * Получает состояние запуска привода.
 * @return Состояние запуска привода.
 */
extern drive_starting_t drive_starting(void);

/**
 * Получает состояние останова привода.
 * @return Состояние останова привода.
 */
extern drive_stopping_t drive_stopping(void);

/**
 * Получает состояние останова привода при ошибке.
 * @return Состояние останова привода при ошибке.
 */
extern drive_err_stopping_t drive_err_stopping(void);

/**
 * Получает направление вращения.
 * @return Направление вращения.
 */
ALWAYS_INLINE static drive_dir_t drive_direction(void)
{
    return drive_phase_state_direction();
}

/**
 * Получает ошибки фаз.
 * @return Ошибка фаз.
 */
ALWAYS_INLINE static drive_phase_errors_t drive_phase_errors(void)
{
    return drive_phase_state_errors();
}

/**
 * Получает значение задания.
 */
ALWAYS_INLINE static reference_t drive_reference(void)
{
    return drive_regulator_reference();
}

/**
 * Устанавливает значение задания.
 * @param reference Задание.
 * @return Код ошибки.
 */
ALWAYS_INLINE static err_t drive_set_reference(reference_t reference)
{
    return drive_regulator_set_reference(reference);
}

/**
 * Получает флаг готовности привода.
 * @return Флаг готовности привода.
 */
extern bool drive_ready(void);

/**
 * Запускает привод.
 * @return Флаг запуска привода.
 */
extern bool drive_start(void);

/**
 * Останавливает привод.
 * @return Флаг останова привода.
 */
extern bool drive_stop(void);

/**
 * Аварийно останавливает привод.
 * @return Флаг останова привода.
 */
extern bool drive_emergency_stop(void);

/**
 * Выполняет калибровку питания.
 * @return Флаг калибровки питания.
 */
extern bool drive_calibrate_power(void);

/**
 * Получает флаг работы привода.
 * @return Флаг работы привода.
 */
extern bool drive_running(void);

/**
 * Очищает ошибки привода.
 */
extern void drive_clear_errors(void);

/**
 * Получает каллбэк при возникновении ошибки.
 * @return Каллбэк при возникновении ошибки.
 */
extern drive_error_callback_t drive_error_callback(void);

/**
 * Устанавливает каллбэк при возникновении ошибки.
 * @param callback Каллбэк при возникновении ошибки.
 */
extern void drive_set_error_callback(drive_error_callback_t callback);

/**
 * Получает каллбэк при возникновении предупреждения.
 * @return Каллбэк при возникновении предупреждения.
 */
extern drive_warning_callback_t drive_warning_callback(void);

/**
 * Устанавливает каллбэк при возникновении предупреждения.
 * @param callback Каллбэк при возникновении предупреждения.
 */
extern void drive_set_warning_callback(drive_warning_callback_t callback);

/**
 * Устанавливает порт ввода-вывода для цифрового входа.
 * @param input Цифровой вход.
 * @param GPIO Порт ввода-вывода.
 * @param pin Пин.
 * @return Код ошибки.
 */
ALWAYS_INLINE static err_t drive_set_dio_input_gpio(drive_dio_input_t input, GPIO_TypeDef* GPIO, uint16_t pin)
{
    return drive_dio_input_set_gpio(input, GPIO, pin);
}

/**
 * Устанавливает порт ввода-вывода для цифрового выхода.
 * @param output Цифровой выход.
 * @param GPIO Порт ввода-вывода.
 * @param pin Пин.
 * @return Код ошибки.
 */
ALWAYS_INLINE static err_t drive_set_dio_output_gpio(drive_dio_output_t output, GPIO_TypeDef* GPIO, uint16_t pin)
{
    return drive_dio_output_set_gpio(output, GPIO, pin);
}

/**
 * Устанавливает порт вывода для заданной пары тиристоров.
 * @param triac_pair Номер пары тиристоров.
 * @param GPIO_a Порт тиристора А.
 * @param pin_a Пин тиристора А.
 * @param GPIO_b Порт тиристора Б.
 * @param pin_b Пин тиристора Б.
 * @return Код ошибки.
 */
ALWAYS_INLINE static  err_t drive_set_triac_pair_gpio(triac_pair_number_t triac_pair, GPIO_TypeDef* GPIO_a, uint16_t pin_a, GPIO_TypeDef* GPIO_b, uint16_t pin_b)
{
    return drive_triacs_set_pair_gpio(triac_pair, GPIO_a, pin_a, GPIO_b, pin_b);
}

/**
 * Устанавливает порт тиристора возбуждения.
 * @param GPIO Порт тиристора возбуждения.
 * @param pin Пин тиристора возбуждения.
 * @return Код ошибки.
 */
ALWAYS_INLINE static  err_t drive_set_triac_exc_gpio(GPIO_TypeDef* GPIO, uint16_t pin)
{
    return drive_triacs_set_exc_gpio(GPIO, pin);
}

/**
 * Устанавливает таймер для открытия тиристоров.
 * @param index Индекс таймера.
 * @param TIM Таймер.
 * @return Код ошибки.
 */
ALWAYS_INLINE static  err_t drive_set_triacs_pairs_timer(size_t index, TIM_TypeDef* TIM)
{
    return drive_triacs_set_pairs_timer(index, TIM);
}

/**
 * Устанавливает таймер для открытия симистора возбуждения.
 * @param TIM Таймер.
 * @return Код ошибки.
 */
ALWAYS_INLINE static err_t drive_set_triac_exc_timer(TIM_TypeDef* TIM)
{
    return drive_triacs_set_exc_timer(TIM);
}

/**
 * Устанавливает таймер для отсчёта интервалов между датчиками нуля.
 * @param TIM Таймер.
 * @return Код ошибки.
 */
ALWAYS_INLINE static err_t drive_set_phase_state_timer(TIM_TypeDef* TIM)
{
    return drive_phase_state_set_timer(TIM);
}

/**
 * Устанавливает таймер искусственных датчиков нуля.
 * @param TIM Таймер.
 * @return Код ошибки.
 */
extern err_t drive_set_null_timer(TIM_TypeDef* TIM);

/**
 * Обработчик прерывания таймера 0 открытия тиристоров.
 */
extern void drive_triac_pairs_timer0_irq_handler(void);

/**
 * Обработчик прерывания таймера 1 открытия тиристоров.
 */
extern void drive_triac_pairs_timer1_irq_handler(void);

/**
 * Обработчик прерывания таймера открытия тиристора возбуждения.
 */
extern void drive_triac_exc_timer_irq_handler(void);

/**
 * Обрабатывает событие датчика нуля по заданной фазе.
 * Устаревшая функция, датчики нуля теперь
 * не используются для определения нуля.
 * @param phase Фаза.
 * @param edge Фронт сигнала датчика нуля.
 * @return Код ошибки.
 */
extern __attribute__ ((deprecated)) err_t drive_process_null_sensor(phase_t phase, null_sensor_edge_t edge);

/**
 * Обрабатывает прерывание переполнения таймера искусственных датчиков нуля.
 */
extern void drive_null_timer_irq_handler(void);

/**
 * Обрабатывает прерывание переполнения таймера отсчёта времени между фазами.
 */
ALWAYS_INLINE static void drive_phases_timer_irq_handler(void)
{
    drive_phase_state_process_phase_timeout();
}

/**
 * Обрабатывает очередные значения АЦП.
 * @param channels Маска каналов АЦП.
 * @param adc_values Значения АЦП.
 * @return Код ошибки.
 */
extern err_t drive_process_power_adc_values(power_channels_t channels, uint16_t* adc_values);

/**
 * Обрабатывает накопленные данные АЦП.
 * @param channels Маска каналов АЦП.
 * @return Код ошибки.
 */
ALWAYS_INLINE static err_t drive_process_power_accumulated_data(power_channels_t channels)
{
    return drive_power_process_accumulated_data(channels);
}

/**
 * Обработка данных с АЦП.
 * Вызывается из прерывания АЦП. 
 * @return Флаг обработки данных питания.
 */
bool drive_calculate_power(void);

/**
 * Инкрементирует значение задания.
 * @return Код ошибки.
 */
ALWAYS_INLINE static err_t drive_inc_reference()
{
    reference_t reference = drive_reference();
    if(reference < REFERENCE_MAX) reference ++;
    return drive_set_reference(reference);
}

/**
 * Декрементирует значение задания.
 * @return Код ошибки.
 */
ALWAYS_INLINE static err_t drive_dec_reference()
{
    reference_t reference = drive_reference();
    if(reference > 0) reference --;
    return drive_set_reference(reference);
}

#endif	/* DRIVE_H */
