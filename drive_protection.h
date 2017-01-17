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


#define DRIVE_PROT_ITEMS_COUNT 47

#define DRIVE_PROT_ITEM_FAULT_OVF_Ua    0
#define DRIVE_PROT_ITEM_FAULT_UDF_Ua    1
#define DRIVE_PROT_ITEM_WARN_OVF_Ua     2
#define DRIVE_PROT_ITEM_WARN_UDF_Ua     3

#define DRIVE_PROT_ITEM_FAULT_OVF_Ub    4
#define DRIVE_PROT_ITEM_FAULT_UDF_Ub    5
#define DRIVE_PROT_ITEM_WARN_OVF_Ub     6
#define DRIVE_PROT_ITEM_WARN_UDF_Ub     7

#define DRIVE_PROT_ITEM_FAULT_OVF_Uc    8
#define DRIVE_PROT_ITEM_FAULT_UDF_Uc    9
#define DRIVE_PROT_ITEM_WARN_OVF_Uc     10
#define DRIVE_PROT_ITEM_WARN_UDF_Uc     11

#define DRIVE_PROT_ITEM_FAULT_OVF_Ia    12
#define DRIVE_PROT_ITEM_WARN_OVF_Ia     13

#define DRIVE_PROT_ITEM_FAULT_OVF_Ib    14
#define DRIVE_PROT_ITEM_WARN_OVF_Ib     15

#define DRIVE_PROT_ITEM_FAULT_OVF_Ic    16
#define DRIVE_PROT_ITEM_WARN_OVF_Ic     17

#define DRIVE_PROT_ITEM_FAULT_OVF_Urot  18
#define DRIVE_PROT_ITEM_WARN_OVF_Urot   19

#define DRIVE_PROT_ITEM_FAULT_OVF_Irot  20
#define DRIVE_PROT_ITEM_WARN_OVF_Irot   21

#define DRIVE_PROT_ITEM_FAULT_OVF_Iexc  22
#define DRIVE_PROT_ITEM_FAULT_UDF_Iexc  23
#define DRIVE_PROT_ITEM_WARN_OVF_Iexc   24
#define DRIVE_PROT_ITEM_WARN_UDF_Iexc   25

#define DRIVE_PROT_ITEM_FAULT_IDLE_Ia   26
#define DRIVE_PROT_ITEM_WARN_IDLE_Ia    27

#define DRIVE_PROT_ITEM_FAULT_IDLE_Ib   28
#define DRIVE_PROT_ITEM_WARN_IDLE_Ib    29

#define DRIVE_PROT_ITEM_FAULT_IDLE_Ic   30
#define DRIVE_PROT_ITEM_WARN_IDLE_Ic    31

#define DRIVE_PROT_ITEM_FAULT_IDLE_Urot 32
#define DRIVE_PROT_ITEM_WARN_IDLE_Urot  33

#define DRIVE_PROT_ITEM_FAULT_IDLE_Irot 34
#define DRIVE_PROT_ITEM_WARN_IDLE_Irot  35

#define DRIVE_PROT_ITEM_FAULT_IDLE_Iexc 36
#define DRIVE_PROT_ITEM_WARN_IDLE_Iexc  37

#define DRIVE_PROT_ITEM_CUTOFF_Ua       38

#define DRIVE_PROT_ITEM_CUTOFF_Ub       39

#define DRIVE_PROT_ITEM_CUTOFF_Uc       40

#define DRIVE_PROT_ITEM_CUTOFF_Ia       41

#define DRIVE_PROT_ITEM_CUTOFF_Ib       42

#define DRIVE_PROT_ITEM_CUTOFF_Ic       43

#define DRIVE_PROT_ITEM_CUTOFF_Urot     44

#define DRIVE_PROT_ITEM_CUTOFF_Irot     45

#define DRIVE_PROT_ITEM_CUTOFF_Iexc     46


typedef enum _Drive_Pwr_Check_Res {
    DRIVE_PWR_CHECK_NORMAL = 0,
    DRIVE_PWR_CHECK_WARN_UNDERFLOW,
    DRIVE_PWR_CHECK_WARN_OVERFLOW,
    DRIVE_PWR_CHECK_FAULT_UNDERFLOW,
    DRIVE_PWR_CHECK_FAULT_OVERFLOW,
} drive_pwr_check_res_t;

typedef enum _Drive_Top_Check_Res {
    DRIVE_TOP_CHECK_NORMAL = 0,
    DRIVE_TOP_CHECK_HEATING,
    DRIVE_TOP_CHECK_COOLING,
    DRIVE_TOP_CHECK_OVERHEAT
} drive_top_check_res_t;

typedef enum _Drive_Break_Check_Res {
    DRIVE_BREAK_CHECK_NORMAL = 0,
    DRIVE_BREAK_CHECK_FAIL
} drive_break_check_res_t;


typedef enum _Drive_Prot_Action {
    DRIVE_PROT_IGNORE         = 0,
    DRIVE_PROT_WARNING        = 1,
    DRIVE_PROT_STOP           = 2,
    DRIVE_PROT_FAST_STOP      = 3,
    DRIVE_PROT_EMERGENCY_STOP = 4
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
 * Получает маску ошибок.
 * @return Маска ошибок.
 */
extern drive_power_errors_t drive_protection_errs_mask(void);

/**
 * Устанавливает маску ошибок.
 * @param errs_mask Маска ошибок.
 */
extern void drive_protection_set_errs_mask(drive_power_errors_t errs_mask);

/**
 * Устанавливает флаги в маске ошибок.
 * @param errs_flags Флаги в маске ошибок.
 */
extern void drive_protection_set_errs_mask_flags(drive_power_errors_t errs_flags);

/**
 * Сбрасывает флаги в маске ошибок.
 * @param errs_flags Флаги в маске ошибок.
 */
extern void drive_protection_reset_errs_mask_flags(drive_power_errors_t errs_flags);

/*
 * Маска предупреждений для проверки средних(rms) значений.
 */

/**
 * Получает маску предупреждений.
 * @return Маска предупреждений.
 */
extern drive_power_warnings_t drive_protection_warn_mask(void);

/**
 * Устанавливает маску предупреждений.
 * @param warn_mask Маска предупреждений.
 */
extern void drive_protection_set_warn_mask(drive_power_warnings_t warn_mask);

/**
 * Устанавливает флаги в маске предупреждений.
 * @param warn_flags Флаги в маске предупреждений.
 */
extern void drive_protection_set_warn_mask_flags(drive_power_warnings_t warn_flags);

/**
 * Сбрасывает флаги в маске предупреждений.
 * @param warn_flags Флаги в маске предупреждений.
 */
extern void drive_protection_reset_warn_mask_flags(drive_power_warnings_t warn_flags);

/*
 * Маска ошибок для проверки значений отсечки (мгновенных).
 */

/**
 * Получает маску ошибок отсечки.
 * @return Маска ошибок отсечки.
 */
extern drive_power_errors_t drive_protection_cutoff_errs_mask(void);

/**
 * Устанавливает маску ошибок отсечки.
 * @param errs_mask Маска ошибок отсечки.
 */
extern void drive_protection_set_cutoff_errs_mask(drive_power_errors_t errs_mask);

/**
 * Устанавливает флаги в маске ошибок отсечки.
 * @param errs_flags Флаги в маске ошибок отсечки.
 */
extern void drive_protection_set_cutoff_errs_mask_flags(drive_power_errors_t errs_flags);

/**
 * Сбрасывает флаги в маске ошибок отсечки.
 * @param errs_flags Флаги в маске ошибок отсечки.
 */
extern void drive_protection_reset_cutoff_errs_mask_flags(drive_power_errors_t errs_flags);

/*
 * Маска предупреждений для проверки значений отсечки (мгновенных).
 */

/**
 * Получает маску предупреждений отсечки.
 * @return Маска предупреждений отсечки.
 */
extern drive_power_warnings_t drive_protection_cutoff_warn_mask(void);

/**
 * Устанавливает маску предупреждений отсечки.
 * @param warn_mask Маска предупреждений отсечки.
 */
extern void drive_protection_set_cutoff_warn_mask(drive_power_warnings_t warn_mask);

/**
 * Устанавливает флаги в маске предупреждений отсечки.
 * @param warn_flags Флаги в маске предупреждений отсечки.
 */
extern void drive_protection_set_cutoff_warn_mask_flags(drive_power_warnings_t warn_flags);

/**
 * Сбрасывает флаги в маске предупреждений отсечки.
 * @param warn_flags Флаги в маске предупреждений отсечки.
 */
extern void drive_protection_reset_cutoff_warn_mask_flags(drive_power_warnings_t warn_flags);

/*
 * Проверка питания.
 */

/**
 * Проверяет элемент канала защиты.
 * @param index Индекс элемента защиты.
 * @param warnings Указатель на переменную для предупреждений. Может быть NULL.
 * @param errors Указатель на переменную для ошибок. Может быть NULL.
 * @return Флаг нахождения элемента защиты в допустимом диапазоне.
 */
extern bool drive_protection_check_item(size_t index, drive_power_warnings_t* warnings, drive_power_errors_t* errors);

/**
 * Выполняет проверку по элементам защиты.
 * Не обнуляет переменные с ошибками и предупреждениями.
 * @param items Указатель на массив с индексами элементов защиты.
 * @param items_count Число индексов элементов защиты в массиве.
 * @param warnings Указатель на переменную для предупреждений. Может быть NULL.
 * @param errors Указатель на переменную для ошибок. Может быть NULL.
 * @return Флаг нахождения всех элементов в допустимых диапазонах питания.
 */
extern bool drive_protection_check_power_items(const size_t* items, size_t items_count, drive_power_warnings_t* warnings, drive_power_errors_t* errors);

/**
 * Очищает ошибки элементов защиты.
 * @param index Индекс элемента защиты.
 */
extern void drive_protection_clear_power_item_error(size_t index);

/**
 * Очищает ошибки элементов защиты.
 */
extern void drive_protection_clear_power_errors(void);

/**
 * Получает наличие элемента защиты в маске ошибок или предупреждений.
 * @param index Индекс элемента защиты.
 * @return Флаг наличия элемента защиты в маске ошибок или предупреждений.
 */
extern bool drive_protection_item_masked(size_t index);

/**
 * Получает флаг допустимости значения элемента защиты.
 * @param index Индекс элемента защиты.
 * @return Флаг допустимости значения элемента защиты.
 */
extern bool drive_protection_item_allow(size_t index);

/**
 * Получает флаг активности(срабатывания) элемента защиты.
 * @param index Индекс элемента защиты.
 * @return Флаг активности(срабатывания) элемента защиты.
 */
extern bool drive_protection_item_active(size_t index);

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
