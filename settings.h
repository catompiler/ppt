/* 
 * @file settings.h Библиотека настроек.
 */

#ifndef SETTINGS_H
#define	SETTINGS_H

#include <stdint.h>
#include <stdbool.h>
#include "fixed/fixed32.h"
#include "errors/errors.h"


// Список идентификаторов параметров.
#include "parameters_ids.h"


//! Тип значения параметра.
typedef enum _Parameter_Type {
    PARAM_TYPE_INT = 0,
    PARAM_TYPE_UINT = 1,
    PARAM_TYPE_FRACT_10 = 2,
    PARAM_TYPE_FRACT_100 = 3,
    PARAM_TYPE_FRACT_1000 = 4
} param_type_t;

//! Тип данных в массиве значений параметров.
typedef uint16_t param_data_t;

//! Тип индекса параметра в массиве.
typedef uint16_t param_index_t;

//! Тип идентификатора параметра.
typedef uint16_t param_id_t;

//! Тип значения параметра.
typedef union _Param_Value {
    fixed32_t fixed_value;
    uint32_t uint_value;
    int32_t int_value;
} param_value_t;

//! Тип вида значения параметра.
typedef enum _Param_Value_Type {
    PARAM_VALUE_TYPE_FIXED = 0,
    PARAM_VALUE_TYPE_INT = 1,
    PARAM_VALUE_TYPE_UINT = 2
} param_value_type_t;

//! Флаги параметров.
typedef enum _Param_Flag {
    PARAM_FLAG_NONE = 0,
    PARAM_FLAG_VIRTUAL = 1
} param_flag_t;

//! Тип флагов параметров.
typedef uint32_t param_flags_t;

//! Тип дескриптора параметра.
typedef struct _Param_Descr {
    param_id_t id;
    param_type_t type;
    param_value_t min;
    param_value_t max;
    param_value_t def;
    param_flags_t flags;
} param_descr_t;

//! Тип параметра.
typedef struct _Param {
    param_index_t descr_index;
    union {
        param_data_t virt_data;
        param_index_t data_index;
    };
} param_t;


/**
 * Инициализирует настройки значениями по-умолчанию.
 * @return Код ошибки.
 */
extern err_t settings_init(void);

/**
 * Применяет настройки по-умолчанию.
 * @return Код ошибки.
 */
extern err_t settings_default(void);

/**
 * Читает настройки.
 * @return Код ошибки.
 */
extern err_t settings_read(void);

/**
 * Записывает настройки.
 * @return Код ошибки.
 */
extern err_t settings_write(void);

/**
 * Получает флаг только для чтения настроек.
 * @return Флаг только для чтения настроек.
 */
extern bool settings_readonly(void);

/**
 * Устанавливает флаг только для чтения настроек.
 * @param readonly Флаг только для чтения настроек.
 */
extern void settings_set_readonly(bool readonly);

/**
 * Получает параметр по идентификатору.
 * @param id Идентификатор параметра.
 * @return Параметр с заданным идентификатором.
 */
extern param_t* settings_param_by_id(param_id_t id);

/**
 * Получает сырое значение параметра.
 * @param param Параметр.
 * @return Сырое значение параметра.
 */
extern param_data_t settings_param_value_raw(param_t* param);

/**
 * Устанавливает сырое значение параметра.
 * @param param Параметр.
 * @param value Сырое значение параметра.
 * @return true в случае успеха, иначе false.
 */
extern bool settings_param_set_value_raw(param_t* param, param_data_t value);

/**
 * Получает знаковое целочисленное значение параметра.
 * @param param Параметр.
 * @return Знаковое целочисленное значение параметра.
 */
extern int32_t settings_param_valuei(param_t* param);

/**
 * Устанавливает знаковое целочисленное значение параметра.
 * @param param Параметр.
 * @param value Знаковое целочисленное значение параметра.
 * @return true в случае успеха, иначе false.
 */
extern bool settings_param_set_valuei(param_t* param, int32_t value);

/**
 * Получает беззнаковое целочисленное значение параметра.
 * @param param Параметр.
 * @return Беззнаковое целочисленное значение параметра.
 */
extern uint32_t settings_param_valueu(param_t* param);

/**
 * Устанавливает беззнаковое целочисленное значение параметра.
 * @param param Параметр.
 * @param value Беззнаковое целочисленное значение параметра.
 * @return true в случае успеха, иначе false.
 */
extern bool settings_param_set_valueu(param_t* param, uint32_t value);

/**
 * Получает значение с фиксированной запятой параметра.
 * @param param Параметр.
 * @return Значение с фиксированной запятой параметра.
 */
extern fixed32_t settings_param_valuef(param_t* param);

/**
 * Получает значение с фиксированной запятой параметра.
 * @param param Параметр.
 * @param value Значение с фиксированной запятой параметра.
 * @return true в случае успеха, иначе false.
 */
extern bool settings_param_set_valuef(param_t* param, fixed32_t value);

/**
 * Получает знаковое целочисленное значение параметра по идентификатору.
 * @param id Идентификатор параметра.
 * @return Знаковое целочисленное значение параметра.
 */
extern int32_t settings_valuei(param_id_t id);

/**
 * Получает беззнаковое целочисленное значение параметра по идентификатору.
 * @param id Идентификатор параметра.
 * @return Беззнаковое целочисленное значение параметра.
 */
extern uint32_t settings_valueu(param_id_t id);

/**
 * Получает значение с фиксированной запятой параметра по идентификатору.
 * @param id Идентификатор параметра.
 * @return Значение с фиксированной запятой параметра.
 */
extern fixed32_t settings_valuef(param_id_t id);

#endif	/* SETTINGS_H */
