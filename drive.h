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
#include "drive_phase_sync.h"




/*
 * Конфигурация таймера для итераций работы привода.
 */
//! Число тиков.
#define DRIVE_MAIN_TIMER_CNT_TICKS (40000)
//! Период в микросекундах.
#define DRIVE_MAIN_TIMER_CNT_PERIOD_US (3333)
//! Период.
#define DRIVE_MAIN_TIMER_CNT_PERIOD (DRIVE_MAIN_TIMER_CNT_TICKS - 1)
//! Предделитель.
#define DRIVE_MAIN_TIMER_CNT_PRESCALER (6 - 1)
//! Частота срабатывания таймера нуля.
#define DRIVE_MAIN_TIMER_FREQ (POWER_FREQ * 6)
//! Угол таймера нуля.
#define DRIVE_MAIN_TIMER_ANGLE (60)
//! Отклонение таймера нуля для синхронизации.
#define DRIVE_MAIN_TIMER_OFFSET_TICKS_MAX (1500) // 125 мкс
//! Коэффициент тик/градус отклонения.
#define DRIVE_MAIN_TIMER_TICKS_PER_DEG 667//400
//! Число срабатываний таймера за период.
#define DRIVE_MAIN_TIMER_PERIOD_ITERS (6)

/*
 * Периодичность основных итераций привода.
 */
//! Делитель частоты для основного таймера для основных итераций привода.
#define DRIVE_MAIN_ITER_PRESCALER (2 - 1)
//! Число основных итераций за период.
#define DRIVE_MAIN_ITER_PERIOD_ITERS (3)
//! Частота основных итераций.
#define DRIVE_MAIN_ITER_FREQ (POWER_FREQ * DRIVE_MAIN_ITER_PERIOD_ITERS)

/*
 * Кратность частоты АЦП.
 */
//! Нормальный режим.
#define DRIVE_ADC_RATE_NORMAL 1
//! Ускоренный режим.
#define DRIVE_ADC_RATE_FAST 10
//! Функция установки частоты АЦП.
typedef void (*set_adc_rate_proc_t)(uint32_t rate);



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
    DRIVE_STATE_ERROR       = 7, //!< Ошибка.
    DRIVE_STATE_ZERO_SPEED  = 8, //!< Нулевое задание.
    DRIVE_STATE_SELFTUNE    = 9  //!< Самонастройка.
} drive_state_t;

//! Тип ошибки привода.
typedef enum _Drive_Error {
    DRIVE_ERROR_NONE                 = 0x0, //!< Нет ошибки.
    DRIVE_ERROR_POWER_DATA_NOT_AVAIL = 0x1, //!< Данные питания не поступают с АЦП.
    DRIVE_ERROR_POWER_INVALID        = 0x2, //!< Неправильные значения питания, см. drive_power_error_t.
    DRIVE_ERROR_EMERGENCY_STOP       = 0x4, //!< Аварийный останов (Грибок).
    DRIVE_ERROR_PHASE                = 0x8, //!< Ошибка состояния фаз.
    DRIVE_ERROR_PHASE_ANGLE          = 0x10, //!< Ошибка угла между фазами.
    DRIVE_ERROR_PHASE_SYNC           = 0x20, //!< Ошибка синхронизации фаз.
    DRIVE_ERROR_THERMAL_OVERLOAD     = 0x40, //!< Тепловая защита.
    DRIVE_ERROR_ROT_BREAK            = 0x80, //!< Обрыв якоря.
    DRIVE_ERROR_ROT_MEASURE_BREAK    = 0x100, //!< Обрыв измерения якоря.
    DRIVE_ERROR_HEATSINK_TEMP        = 0x200, //!< Перегрев радиатора.
    DRIVE_ERROR_SENSOR               = 0x400, //!< Ошибка датчиков.
    DRIVE_ERROR_WATCHDOG             = 0x4000 //!< Срабатывание сторожевого таймера.
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

//! Тип ошибки датчиков привода.
typedef enum _Drive_Sensor_Error {
    DRIVE_SENSOR_ERROR_NONE  = 0x0,
    DRIVE_SENSOR_ERROR_Ua    = 0x1,
    DRIVE_SENSOR_ERROR_Ub    = 0x2,
    DRIVE_SENSOR_ERROR_Uc    = 0x4,
    DRIVE_SENSOR_ERROR_Ia    = 0x8,
    DRIVE_SENSOR_ERROR_Ib    = 0x10,
    DRIVE_SENSOR_ERROR_Ic    = 0x20,
    DRIVE_SENSOR_ERROR_Urot  = 0x40,
    DRIVE_SENSOR_ERROR_Irot  = 0x80,
    DRIVE_SENSOR_ERROR_Iexc  = 0x100
} drive_sensor_error_t;

//! Тип ошибок датчиков привода.
typedef uint32_t drive_sensor_errors_t;

typedef enum _Drive_Warning {
    DRIVE_WARNING_NONE             = 0x0, //!< Нет предупреждений.
    DRIVE_WARNING_POWER            = 0x2,  //!< Предупреждение по питанию.
    DRIVE_WARNING_PHASE_ANGLE      = 0x10, //!< Ошибка угла между фазами.
    DRIVE_WARNING_PHASE_SYNC       = 0x20, //!< Ошибка синхронизации фаз.
    DRIVE_WARNING_THERMAL_OVERLOAD = 0x40, //!< Перегрев.
    DRIVE_WARNING_HEATSINK_TEMP    = 0x200, //!< Перегрев радиатора.
    DRIVE_WARNING_SENSOR           = 0x400, //!< Ошибка датчиков.
    DRIVE_WARNING_FAN_FAIL         = 0x800, //!< Ошибка вентилятора.
    DRIVE_WARNING_TRIAC            = 0x1000, //!< Ошибка тиристоров.
    DRIVE_WARNING_MOTOR_PARAMS     = 0x2000  //!< Ошибка параметров двигателя.
} drive_warning_t;

//! Тип предупреждений привода.
typedef uint32_t drive_warnings_t;

//! Тип предупреждения питания привода.
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

//! Тип предупреждения датчиков привода.
typedef enum _Drive_Sensor_Warning {
    DRIVE_SENSOR_WARNING_NONE  = 0x0,
    DRIVE_SENSOR_WARNING_Ua    = 0x1,
    DRIVE_SENSOR_WARNING_Ub    = 0x2,
    DRIVE_SENSOR_WARNING_Uc    = 0x4,
    DRIVE_SENSOR_WARNING_Ia    = 0x8,
    DRIVE_SENSOR_WARNING_Ib    = 0x10,
    DRIVE_SENSOR_WARNING_Ic    = 0x20,
    DRIVE_SENSOR_WARNING_Urot  = 0x40,
    DRIVE_SENSOR_WARNING_Irot  = 0x80,
    DRIVE_SENSOR_WARNING_Iexc  = 0x100
} drive_sensor_warning_t;

//! Тип предупреждений датчиков привода.
typedef uint32_t drive_sensor_warnings_t;

/*
//! Тип ошибки углов фаз.
typedef enum _Drive_Phase_Angle_Error {
    DRIVE_PHASE_ANGLE_NO_ERROR  = 0,
    DRIVE_PHASE_ANGLE_A_ERROR   = 0x1,
    DRIVE_PHASE_ANGLE_B_ERROR   = 0x2,
    DRIVE_PHASE_ANGLE_C_ERROR   = 0x4,
    DRIVE_PHASE_ANGLE_AB_ERROR  = 0x3,
    DRIVE_PHASE_ANGLE_BC_ERROR  = 0x6,
    DRIVE_PHASE_ANGLE_AC_ERROR  = 0x5,
    DRIVE_PHASE_ANGLE_ABC_ERROR = 0x7
} drive_phase_angle_error_t;

//! Тип ошибок углов фаз.
typedef uint32_t drive_phase_angle_errors_t;

//! Тип предупреждения углов фаз.
typedef enum _Drive_Phase_Angle_Warning {
    DRIVE_PHASE_ANGLE_NO_WARNING  = 0,
    DRIVE_PHASE_ANGLE_A_WARNING   = 0x1,
    DRIVE_PHASE_ANGLE_B_WARNING   = 0x2,
    DRIVE_PHASE_ANGLE_C_WARNING   = 0x4,
    DRIVE_PHASE_ANGLE_AB_WARNING  = 0x3,
    DRIVE_PHASE_ANGLE_BC_WARNING  = 0x6,
    DRIVE_PHASE_ANGLE_AC_WARNING  = 0x5,
    DRIVE_PHASE_ANGLE_ABC_WARNING = 0x7
} drive_phase_angle_warning_t;

//! Тип предупреждений углов фаз.
typedef uint32_t drive_phase_angle_warnings_t;
*/


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

//! Перечисление состояний самонастройки.
typedef enum _Drive_Selftuning {
    DRIVE_SELFTUNING_NONE = 0,
    DRIVE_SELFTUNING_BEGIN = 1,
    DRIVE_SELFTUNING_DATA = 2,
    DRIVE_SELFTUNING_CALC_DATA = 3,
    DRIVE_SELFTUNING_CALC = 4,
    DRIVE_SELFTUNING_DONE = 5
} drive_selftuning_t;

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

//! Каллбэк сброса ошибок привода.
typedef void (*drive_reset_callback_t)(void);


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
 * Получает наличие ошибки датчиков привода.
 * @param error Ошибка датчиков привода.
 * @return Наличие ошибки датчиков привода.
 */
extern bool drive_sensor_error(drive_sensor_error_t error);

/**
 * Получает ошибки датчиков привода.
 * @return Ошибки датчиков привода.
 */
extern drive_sensor_errors_t drive_sensor_errors(void);

/**
 * Получает наличие предупреждения датчиков привода.
 * @param warning Предупреждение датчиков привода.
 * @return Наличие предупреждения датчиков привода.
 */
extern bool drive_sensor_warning(drive_sensor_warning_t warning);

/**
 * Получает предупреждения датчиков привода.
 * @return Предупреждения датчиков привода.
 */
extern drive_sensor_warnings_t drive_sensor_warnings(void);

/**
 * Получает ошибки углов между фазами.
 * @return Ошибки углов между фазами.
 */
//extern drive_phase_angle_errors_t drive_phase_angle_errors(void);

/**
 * Получает предупреждения углов между фазами.
 * @return Предупреждения углов между фазами.
 */
//extern drive_phase_angle_warnings_t drive_phase_angle_warnings(void);

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
 * Получает состояние самонастройки привода.
 * @return Состояние самонастройки привода.
 */
extern drive_selftuning_t drive_selftuning(void);

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
 * @return Ошибки фаз.
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
 * Получает флаг отсутствия проблем привода.
 * @return Флаг отсутствия проблем привода.
 */
extern bool drive_ok(void);

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
 * Обработчик внутренней ошибки.
 * Отключает тиристоры,
 * устанавливает цифровой выход ошибки.
 */
extern void drive_mcu_fault(void);

/**
 * Обработчик перезагрузки по сторожевому таймеру.
 * Записывает событие, устанавливает флаг ошибки.
 */
extern void drive_watchdog_timeout(void);

/**
 * Выполняет калибровку питания.
 * @return Флаг калибровки питания.
 */
extern bool drive_calibrate_power(void);

/**
 * Выполняет самонастройку привода.
 * @return Флаг начала самонастройки привода.
 */
extern bool drive_selftune(void);

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
 * Получает каллбэк при сбросе.
 * @return Каллбэк при сбросе.
 */
extern drive_reset_callback_t drive_reset_callback(void);

/**
 * Устанавливает каллбэк при сбросе.
 * @param callback Каллбэк при сбросе.
 */
extern void drive_set_reset_callback(drive_reset_callback_t callback);

/**
 * Получает функцию установки частоты АЦП.
 * @return Функция установки частоты АЦП.
 */
extern set_adc_rate_proc_t drive_adc_rate_proc(void);

/**
 * Устанавливает функцию установки частоты АЦП.
 * @param proc Функция установки частоты АЦП.
 */
extern void drive_set_adc_rate_proc(set_adc_rate_proc_t callback);

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
 * Обрабатывает итерацию синхронизации с фазами.
 */
extern void drive_process_sync_iter(void);

/**
 * Обрабатывает итерацию открытия тиристоров и регулирования тока.
 */
extern void drive_process_triacs_iter(void);

/**
 * Обрабатывает очередную итерацию действий и вычислений
 * при срабатывании виртуального датчика нуля.
 */
extern void drive_process_iter(void);

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
 * Вычисление значений питания.
 * @return Флаг обработки данных питания.
 */
bool drive_calculate_power(void);

/**
 * Вычисление значений тока.
 * @return Флаг обработки данных питания.
 */
bool drive_calculate_currrent(void);

/**
 * Инкрементирует значение задания.
 * @return Код ошибки.
 */
ALWAYS_INLINE static err_t drive_inc_reference()
{
    drive_regulator_inc_reference();
    return E_NO_ERROR;
}

/**
 * Декрементирует значение задания.
 * @return Код ошибки.
 */
ALWAYS_INLINE static err_t drive_dec_reference()
{
    drive_regulator_dec_reference();
    return E_NO_ERROR;
}

#endif	/* DRIVE_H */
