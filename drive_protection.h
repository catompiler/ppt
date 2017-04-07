/**
 * @file drive_protection.h Библиотека функций защиты привода.
 */

#ifndef DRIVE_PROTECTION_H
#define DRIVE_PROTECTION_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "fixed/fixed32.h"
#include "drive.h"


/*
 * Защита питания.
 */

//! Количество элементов защиты питания.
#define DRIVE_PROT_PWR_ITEMS_COUNT 47

/*
 * Индексы элементов защиты питания.
 */

#define DRIVE_PROT_PWR_ITEM_FAULT_OVF_Ua    0
#define DRIVE_PROT_PWR_ITEM_FAULT_UDF_Ua    1
#define DRIVE_PROT_PWR_ITEM_WARN_OVF_Ua     2
#define DRIVE_PROT_PWR_ITEM_WARN_UDF_Ua     3

#define DRIVE_PROT_PWR_ITEM_FAULT_OVF_Ub    4
#define DRIVE_PROT_PWR_ITEM_FAULT_UDF_Ub    5
#define DRIVE_PROT_PWR_ITEM_WARN_OVF_Ub     6
#define DRIVE_PROT_PWR_ITEM_WARN_UDF_Ub     7

#define DRIVE_PROT_PWR_ITEM_FAULT_OVF_Uc    8
#define DRIVE_PROT_PWR_ITEM_FAULT_UDF_Uc    9
#define DRIVE_PROT_PWR_ITEM_WARN_OVF_Uc     10
#define DRIVE_PROT_PWR_ITEM_WARN_UDF_Uc     11

#define DRIVE_PROT_PWR_ITEM_FAULT_OVF_Ia    12
#define DRIVE_PROT_PWR_ITEM_WARN_OVF_Ia     13

#define DRIVE_PROT_PWR_ITEM_FAULT_OVF_Ib    14
#define DRIVE_PROT_PWR_ITEM_WARN_OVF_Ib     15

#define DRIVE_PROT_PWR_ITEM_FAULT_OVF_Ic    16
#define DRIVE_PROT_PWR_ITEM_WARN_OVF_Ic     17

#define DRIVE_PROT_PWR_ITEM_FAULT_OVF_Urot  18
#define DRIVE_PROT_PWR_ITEM_WARN_OVF_Urot   19

#define DRIVE_PROT_PWR_ITEM_FAULT_OVF_Irot  20
#define DRIVE_PROT_PWR_ITEM_WARN_OVF_Irot   21

#define DRIVE_PROT_PWR_ITEM_FAULT_OVF_Iexc  22
#define DRIVE_PROT_PWR_ITEM_FAULT_UDF_Iexc  23
#define DRIVE_PROT_PWR_ITEM_WARN_OVF_Iexc   24
#define DRIVE_PROT_PWR_ITEM_WARN_UDF_Iexc   25

#define DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Ia   26
#define DRIVE_PROT_PWR_ITEM_WARN_IDLE_Ia    27

#define DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Ib   28
#define DRIVE_PROT_PWR_ITEM_WARN_IDLE_Ib    29

#define DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Ic   30
#define DRIVE_PROT_PWR_ITEM_WARN_IDLE_Ic    31

#define DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Urot 32
#define DRIVE_PROT_PWR_ITEM_WARN_IDLE_Urot  33

#define DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Irot 34
#define DRIVE_PROT_PWR_ITEM_WARN_IDLE_Irot  35

#define DRIVE_PROT_PWR_ITEM_FAULT_IDLE_Iexc 36
#define DRIVE_PROT_PWR_ITEM_WARN_IDLE_Iexc  37

#define DRIVE_PROT_PWR_ITEM_CUTOFF_Ua       38

#define DRIVE_PROT_PWR_ITEM_CUTOFF_Ub       39

#define DRIVE_PROT_PWR_ITEM_CUTOFF_Uc       40

#define DRIVE_PROT_PWR_ITEM_CUTOFF_Ia       41

#define DRIVE_PROT_PWR_ITEM_CUTOFF_Ib       42

#define DRIVE_PROT_PWR_ITEM_CUTOFF_Ic       43

#define DRIVE_PROT_PWR_ITEM_CUTOFF_Urot     44

#define DRIVE_PROT_PWR_ITEM_CUTOFF_Irot     45

#define DRIVE_PROT_PWR_ITEM_CUTOFF_Iexc     46


/*
 * Общая защита.
 */

//! Количество элементов защиты питания.
#define DRIVE_PROT_ITEMS_COUNT 5

/*
 * Индексы элементов защиты питания.
 */

#define DRIVE_PROT_ITEM_FAULT_PHASE_STATE    0

#define DRIVE_PROT_ITEM_FAULT_PHASES_ANGLES  1
#define DRIVE_PROT_ITEM_WARN_PHASES_ANGLES   2

#define DRIVE_PROT_ITEM_FAULT_PHASES_SYNC    3
#define DRIVE_PROT_ITEM_WARN_PHASES_SYNC     4


//! Тип индекса элемента защиты.
typedef size_t drive_prot_index_t;


//! Тип результата проверки питания.
typedef enum _Drive_Pwr_Check_Res {
    DRIVE_PWR_CHECK_NORMAL = 0,
    DRIVE_PWR_CHECK_WARN_UNDERFLOW,
    DRIVE_PWR_CHECK_WARN_OVERFLOW,
    DRIVE_PWR_CHECK_FAULT_UNDERFLOW,
    DRIVE_PWR_CHECK_FAULT_OVERFLOW,
} drive_pwr_check_res_t;

//! Тип результата проверки тепловой защиты.
typedef enum _Drive_Top_Check_Res {
    DRIVE_TOP_CHECK_NORMAL = 0,
    DRIVE_TOP_CHECK_HEATING,
    DRIVE_TOP_CHECK_COOLING,
    DRIVE_TOP_CHECK_OVERHEAT
} drive_top_check_res_t;

//! Тип результата проверки обрыва якоря.
typedef enum _Drive_Break_Check_Res {
    DRIVE_BREAK_CHECK_NORMAL = 0,
    DRIVE_BREAK_CHECK_FAIL
} drive_break_check_res_t;

//! Тип действия при активации элемента защиты.
typedef enum _Drive_Prot_Action {
    DRIVE_PROT_ACTION_IGNORE         = 0,
    DRIVE_PROT_ACTION_WARNING        = 1,
    DRIVE_PROT_ACTION_FAST_STOP      = 2,
    DRIVE_PROT_ACTION_COAST_STOP     = 3,
    DRIVE_PROT_ACTION_EMERGENCY_STOP = 4
} drive_prot_action_t;



/*
 * Инициализация.
 */

/**
 * Инициализирует защиту привода.
 * @return Флаг успешной инициализации.
 */
extern bool drive_protection_init(void);

/**
 * Обновляет параметры защиты из настроек.
 */
extern void drive_protection_update_settings(void);

/*
 * Маска ошибок для проверки средних(rms) значений.
 */

/**
 * Получает маску ошибок питания.
 * @return Маска ошибок питания.
 */
extern drive_power_errors_t drive_protection_power_errs_mask(void);

/**
 * Устанавливает маску ошибок питания.
 * @param errs_mask Маска ошибок питания.
 */
extern void drive_protection_power_set_errs_mask(drive_power_errors_t errs_mask);

/**
 * Устанавливает флаги в маске ошибок питания.
 * @param errs_flags Флаги в маске ошибок питания.
 */
extern void drive_protection_power_set_errs_mask_flags(drive_power_errors_t errs_flags);

/**
 * Сбрасывает флаги в маске ошибок питания.
 * @param errs_flags Флаги в маске ошибок питания.
 */
extern void drive_protection_power_reset_errs_mask_flags(drive_power_errors_t errs_flags);

/*
 * Маска предупреждений для проверки средних(rms) значений питания.
 */

/**
 * Получает маску предупреждений питания.
 * @return Маска предупреждений питания.
 */
extern drive_power_warnings_t drive_protection_power_warn_mask(void);

/**
 * Устанавливает маску предупреждений питания.
 * @param warn_mask Маска предупреждений питания.
 */
extern void drive_protection_power_set_warn_mask(drive_power_warnings_t warn_mask);

/**
 * Устанавливает флаги в маске предупреждений питания.
 * @param warn_flags Флаги в маске предупреждений питания.
 */
extern void drive_protection_power_set_warn_mask_flags(drive_power_warnings_t warn_flags);

/**
 * Сбрасывает флаги в маске предупреждений питания.
 * @param warn_flags Флаги в маске предупреждений питания.
 */
extern void drive_protection_power_reset_warn_mask_flags(drive_power_warnings_t warn_flags);

/*
 * Маска ошибок для проверки значений отсечки (мгновенных) питания.
 */

/**
 * Получает маску ошибок отсечки питания.
 * @return Маска ошибок отсечки питания.
 */
extern drive_power_errors_t drive_protection_power_cutoff_errs_mask(void);

/**
 * Устанавливает маску ошибок отсечки питания.
 * @param errs_mask Маска ошибок отсечки питания.
 */
extern void drive_protection_power_set_cutoff_errs_mask(drive_power_errors_t errs_mask);

/**
 * Устанавливает флаги в маске ошибок отсечки питания.
 * @param errs_flags Флаги в маске ошибок отсечки питания.
 */
extern void drive_protection_power_set_cutoff_errs_mask_flags(drive_power_errors_t errs_flags);

/**
 * Сбрасывает флаги в маске ошибок отсечки питания.
 * @param errs_flags Флаги в маске ошибок отсечки питания.
 */
extern void drive_protection_power_reset_cutoff_errs_mask_flags(drive_power_errors_t errs_flags);

/*
 * Маска предупреждений для проверки значений отсечки (мгновенных) питания.
 */

/**
 * Получает маску предупреждений отсечки питания.
 * @return Маска предупреждений отсечки питания.
 */
extern drive_power_warnings_t drive_protection_power_cutoff_warn_mask(void);

/**
 * Устанавливает маску предупреждений отсечки питания.
 * @param warn_mask Маска предупреждений отсечки питания.
 */
extern void drive_protection_power_set_cutoff_warn_mask(drive_power_warnings_t warn_mask);

/**
 * Устанавливает флаги в маске предупреждений отсечки питания.
 * @param warn_flags Флаги в маске предупреждений отсечки питания.
 */
extern void drive_protection_power_set_cutoff_warn_mask_flags(drive_power_warnings_t warn_flags);

/**
 * Сбрасывает флаги в маске предупреждений отсечки питания.
 * @param warn_flags Флаги в маске предупреждений отсечки питания.
 */
extern void drive_protection_power_reset_cutoff_warn_mask_flags(drive_power_warnings_t warn_flags);

/*
 * Проверка питания.
 */

/**
 * Проверяет элемент канала защиты питания.
 * @param index Индекс элемента защиты.
 * @param warnings Указатель на переменную для предупреждений. Может быть NULL.
 * @param errors Указатель на переменную для ошибок. Может быть NULL.
 * @return Флаг активации элемента защиты.
 */
extern bool drive_protection_power_check_item(drive_prot_index_t index, drive_power_warnings_t* warnings, drive_power_errors_t* errors);

/**
 * Выполняет проверку по элементам защиты питания.
 * Не обнуляет переменные с ошибками и предупреждениями.
 * @param items Указатель на массив с индексами элементов защиты.
 * @param items_count Число индексов элементов защиты в массиве.
 * @param warnings Указатель на переменную для предупреждений. Может быть NULL.
 * @param errors Указатель на переменную для ошибок. Может быть NULL.
 * @return Флаг активации минимум одного элемента защиты.
 */
extern bool drive_protection_power_check_items(const drive_prot_index_t* items, size_t items_count, drive_power_warnings_t* warnings, drive_power_errors_t* errors);

/**
 * Получает флаг стабильности состояния
 * элемента защиты питания за время изменения состояния
 * при выходе из допустимого диапазона.
 * @param index Индекс элемента защиты.
 * @return Флаг стабильности элемента защиты.
 */
extern bool drive_protection_power_item_stable(drive_prot_index_t index);

/**
 * Очищает ошибки элементов защиты питания.
 * @param index Индекс элемента защиты питания.
 */
extern void drive_protection_power_clear_item_error(drive_prot_index_t index);

/**
 * Очищает ошибки элементов защиты питания.
 */
extern void drive_protection_power_clear_errors(void);

/**
 * Получает действие элемента защиты питания.
 * @param index Индекс элемента защиты.
 * @return Действие элемента защиты.
 */
extern drive_prot_action_t drive_protection_power_item_action(drive_prot_index_t index);

/**
 * Получает наличие элемента защиты в маске ошибок или предупреждений питания.
 * @param index Индекс элемента защиты.
 * @return Флаг наличия элемента защиты в маске ошибок или предупреждений.
 */
extern bool drive_protection_power_item_masked(drive_prot_index_t index);

/**
 * Получает флаг допустимости значения элемента защиты питания.
 * @param index Индекс элемента защиты.
 * @return Флаг допустимости значения элемента защиты.
 */
extern bool drive_protection_power_item_allow(drive_prot_index_t index);

/**
 * Получает флаг активности(срабатывания) элемента защиты питания.
 * @param index Индекс элемента защиты.
 * @return Флаг активности(срабатывания) элемента защиты.
 */
extern bool drive_protection_power_item_active(drive_prot_index_t index);

/**
 * Получает флаг предупреждения элемента защиты питания.
 * @param index Индекс элемента защиты.
 * @return Флаг предупреждения элемента защиты.
 */
extern drive_power_warnings_t drive_protection_power_item_warning(drive_prot_index_t index);

/**
 * Получает флаг ошибки элемента защиты питания.
 * @param index Индекс элемента защиты.
 * @return Флаг ошибки элемента защиты.
 */
extern drive_power_errors_t drive_protection_power_item_error(drive_prot_index_t index);


/*
 * Тепловая защита.
 * (Thermal Overload Protection).
 */

/**
 * Обрабатывает нагрев/охлаждение под действием тока за заданный интервал времени.
 * @param I_rot Текущий ток ротора.
 * @param dt Интервал времени действия тока.
 */
extern void drive_protection_top_process(fixed32_t I_rot, fixed32_t dt);

/**
 * Производит проверку текущего состояния тепловой защиты.
 * @return Результат проверки тепловой защиты.
 */
extern drive_top_check_res_t drive_protection_top_check(void);

/**
 * Получает флаг готовности привода по тепловой защите.
 * @return Флаг готовности привода по тепловой защите.
 */
extern bool drive_protection_top_ready(void);

/**
 * Получает действие при срабатывании тепловой защиты.
 * @return Действие при срабатывании тепловой защиты.
 */
extern drive_prot_action_t drive_protection_top_action(void);


/*
 * Грибок.
 */
/**
 * Получает действие при нажатии на грибок.
 * @return Действие при нажатии на грибок.
 */
extern drive_prot_action_t drive_protection_emergency_stop_action(void);


/*
 * Общие элементы защиты.
 */

/**
 * Проверяет элемент канала защиты.
 * @param index Индекс элемента защиты.
 * @return Флаг активации элемента защиты.
 */
extern bool drive_protection_check_item(drive_prot_index_t index);

/**
 * Выполняет проверку по элементам защиты питания.
 * Не обнуляет переменные с ошибками и предупреждениями.
 * @param items Указатель на массив с индексами элементов защиты.
 * @param items_count Число индексов элементов защиты в массиве.
 * @return Флаг активации минимум одного элемента защиты.
 */
extern bool drive_protection_check_items(const drive_prot_index_t* items, size_t items_count);

/**
 * Получает флаг стабильности состояния
 * элемента защиты за время изменения состояния
 * при выходе из допустимого диапазона.
 * @param index Индекс элемента защиты.
 * @return Флаг стабильности элемента защиты.
 */
extern bool drive_protection_item_stable(drive_prot_index_t index);

/**
 * Очищает ошибки элементов защиты.
 * @param index Индекс элемента защиты.
 */
extern void drive_protection_clear_item_error(drive_prot_index_t index);

/**
 * Очищает ошибки элементов защиты.
 */
extern void drive_protection_clear_errors(void);

/**
 * Получает действие элемента защиты.
 * @param index Индекс элемента защиты.
 * @return Действие элемента защиты.
 */
extern drive_prot_action_t drive_protection_item_action(drive_prot_index_t index);

/**
 * Получает маску (разрешение) элемента защиты.
 * @param index Индекс элемента защиты.
 * @return Маска элемента защиты.
 */
extern bool drive_protection_item_masked(drive_prot_index_t index);

/**
 * Устанавливает маску (разрешение) элемента защиты.
 * @param index Индекс элемента защиты.
 * @param masked Маска элемента защиты.
 */
extern void drive_protection_item_set_masked(drive_prot_index_t index, bool masked);

/**
 * Получает флаг допустимости значения элемента защиты.
 * @param index Индекс элемента защиты.
 * @return Флаг допустимости значения элемента защиты.
 */
extern bool drive_protection_item_allow(drive_prot_index_t index);

/**
 * Получает флаг активности(срабатывания) элемента защиты.
 * @param index Индекс элемента защиты.
 * @return Флаг активности(срабатывания) элемента защиты.
 */
extern bool drive_protection_item_active(drive_prot_index_t index);


/*
 * Прочие проверки.
 */
/**
 * Получает флаг отсутствия отклонения (предупреждения и ошибки).
 * @param check_res Результат проверки,
 * @return Флаг отсутствия отклонения (предупреждения и ошибки).
 */
extern bool drive_protection_is_normal(drive_pwr_check_res_t check_res);

/**
 * Получает флаг отсутствия критического отклонения (ошибки).
 * @param check_res Результат проверки,
 * @return Флаг отсутствия критического отклонения (ошибки).
 */
extern bool drive_protection_is_allow(drive_pwr_check_res_t check_res);

/**
 * Выполняет проверку отсутствия напряжения якоря.
 * @return Результат проверки.
 */
extern drive_pwr_check_res_t drive_protection_check_rot_zero_voltage(void);

/**
 * Выполняет проверку отсутствия тока возбуждения.
 * @return Результат проверки.
 */
extern drive_pwr_check_res_t drive_protection_check_exc_zero_current(void);

/**
 * Выполняет проверку тока возбуждения.
 * @return Результат проверки.
 */
extern drive_pwr_check_res_t drive_protection_check_exc(void);

#endif /* DRIVE_PROTECTION_H */
