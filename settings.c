#include "settings.h"
#include "defs/defs.h"
#include "utils/utils.h"
#include "crc/crc16_ccitt.h"
#include "storage.h"
#include <stddef.h>
#include <stdlib.h>



//! Объявляет массив данных параметров.
#define PARAMETERS_DATA(arg_name, arg_count)\
        static struct _Parameters_Data {\
            param_data_t data[arg_count];\
            uint16_t crc;\
        } arg_name;

//! Объявляет массив параметров.
#define PARAMETERS(arg_array_name, arg_count)\
        static param_t arg_array_name[arg_count]


#include "drive_phase_state.h"
#include "drive_triacs.h"
#include "ramp.h"
#include "fixed/fixed32.h"
#include "parameters_list.h"

// Данные параметров.
#pragma pack(push, 1)
PARAMETERS_DATA(parameters_data, PARAMETERS_REAL_COUNT);
#pragma pack(pop)

// Массив параметров.
PARAMETERS(parameters, PARAMETERS_COUNT);


//! Флаг только для чтения.
static bool _settings_readonly = false;

//! Получает флаг только для чтения.
ALWAYS_INLINE static bool ro(void)
{
    return _settings_readonly;
}

/**
 * Получает параметр по индексу.
 * @param index Индекс.
 * @return Указатель на параметр с заданным индексом.
 */
ALWAYS_INLINE static param_t* settings_parameter_by_index(size_t index)
{
    return &parameters[index];
}

/**
 * Получает дескриптор параметра по индексу.
 * @param index Индекс.
 * @return Указатель на дескриптор параметра с заданным индексом.
 */
ALWAYS_INLINE static const param_descr_t* settings_param_descr_by_index(size_t index)
{
    return &parameters_descrs[index];
}

/**
 * Получает данные параметра по индексу.
 * @param index Индекс.
 * @return Указатель на данные параметра с заданным индексом.
 */
ALWAYS_INLINE static param_data_t* settings_param_data_by_index(size_t index)
{
    return &parameters_data.data[index];
}

/**
 * Получает значение параметра как знаковое целое.
 * @param param Параметр.
 * @return Значение параметра.
 */
static int32_t settings_param_get_int32(param_t* param)
{
    const param_descr_t* descr = settings_param_descr_by_index(param->descr_index);
    param_data_t data = (descr->flags & PARAM_FLAG_VIRTUAL) ? (param->virt_data):
                        (*settings_param_data_by_index(param->data_index));
    
    switch(descr->type){
        default:
        case PARAM_TYPE_INT:
        case PARAM_TYPE_UINT:
            break;
        case PARAM_TYPE_FRACT_10:
            return data / 10;
        case PARAM_TYPE_FRACT_100:
            return data / 100;
        case PARAM_TYPE_FRACT_1000:
            return data / 1000;
        case PARAM_TYPE_FRACT_10000:
            return data / 10000;
    }
    
    return data;
}

/**
 * Устанавливает значение параметра как знаковое целое.
 * @param param Параметр.
 * @return Значение параметра.
 */
static void settings_param_set_int32(param_t* param, int32_t value)
{
    const param_descr_t* descr = settings_param_descr_by_index(param->descr_index);
    param_data_t* data = (descr->flags & PARAM_FLAG_VIRTUAL) ? (&param->virt_data):
                         (settings_param_data_by_index(param->data_index));
    
    switch(descr->type){
        default:
        case PARAM_TYPE_INT:
        case PARAM_TYPE_UINT:
            *data = value;
            break;
        case PARAM_TYPE_FRACT_10:
            *data = value * 10;
            break;
        case PARAM_TYPE_FRACT_100:
            *data = value * 100;
            break;
        case PARAM_TYPE_FRACT_1000:
            *data = value * 1000;
            break;
        case PARAM_TYPE_FRACT_10000:
            *data = value * 10000;
            break;
    }
}

/**
 * Получает значение параметра как беззнаковое целое.
 * @param param Параметр.
 * @return Значение параметра.
 */
static uint32_t settings_param_get_uint32(param_t* param)
{
    const param_descr_t* descr = settings_param_descr_by_index(param->descr_index);
    param_data_t data = (descr->flags & PARAM_FLAG_VIRTUAL) ? (param->virt_data):
                        (*settings_param_data_by_index(param->data_index));
    
    switch(descr->type){
        default:
        case PARAM_TYPE_INT:
        case PARAM_TYPE_UINT:
            break;
        case PARAM_TYPE_FRACT_10:
            return data / 10;
        case PARAM_TYPE_FRACT_100:
            return data / 100;
        case PARAM_TYPE_FRACT_1000:
            return data / 1000;
        case PARAM_TYPE_FRACT_10000:
            return data / 10000;
    }
    
    return data;
}

/**
 * Устанавливает значение параметра как беззнаковое целое.
 * @param param Параметр.
 * @return Значение параметра.
 */
static void settings_param_set_uint32(param_t* param, uint32_t value)
{
    const param_descr_t* descr = settings_param_descr_by_index(param->descr_index);
    param_data_t* data = (descr->flags & PARAM_FLAG_VIRTUAL) ? (&param->virt_data):
                         (settings_param_data_by_index(param->data_index));
    
    switch(descr->type){
        default:
        case PARAM_TYPE_INT:
        case PARAM_TYPE_UINT:
            *data = value;
            break;
        case PARAM_TYPE_FRACT_10:
            *data = value * 10;
            break;
        case PARAM_TYPE_FRACT_100:
            *data = value * 100;
            break;
        case PARAM_TYPE_FRACT_1000:
            *data = value * 1000;
            break;
        case PARAM_TYPE_FRACT_10000:
            *data = value * 10000;
            break;
    }
}

/**
 * Получает значение параметра как число с фиксированной запятой.
 * @param param Параметр.
 * @return Значение параметра.
 */
static fixed32_t settings_param_get_fixed32(param_t* param)
{
    const param_descr_t* descr = settings_param_descr_by_index(param->descr_index);
    param_data_t data = (descr->flags & PARAM_FLAG_VIRTUAL) ? (param->virt_data):
                        (*settings_param_data_by_index(param->data_index));
    
    switch(descr->type){
        default:
        case PARAM_TYPE_INT:
        case PARAM_TYPE_UINT:
            break;
        case PARAM_TYPE_FRACT_10:
            return fixed32_make_from_fract((fixed32_t)data, 10);
        case PARAM_TYPE_FRACT_100:
            return fixed32_make_from_fract((fixed32_t)data, 100);
        case PARAM_TYPE_FRACT_1000:
            return fixed32_make_from_fract((fixed32_t)data, 1000);
        case PARAM_TYPE_FRACT_10000:
            return fixed32_make_from_fract((fixed32_t)data, 10000);
    }
    return fixed32_make_from_int((fixed32_t)data);
}

/**
 * Устанавливает значение параметра как число с фиксированной запятой.
 * @param param Параметр.
 * @return Значение параметра.
 */
static void settings_param_set_fixed32(param_t* param, fixed32_t value)
{
    const param_descr_t* descr = settings_param_descr_by_index(param->descr_index);
    param_data_t* data = (descr->flags & PARAM_FLAG_VIRTUAL) ? (&param->virt_data):
                         (settings_param_data_by_index(param->data_index));
    
    int32_t int_part = 0;
    int32_t fract_part = 0;
    
    /*
     * Магия.
     * Лучше не трогать.
     */
    switch(descr->type){
        default:
        case PARAM_TYPE_INT:
        case PARAM_TYPE_UINT:
            *data = fixed32_get_int(value);
            break;
        case PARAM_TYPE_FRACT_10:
            if(value >= 0)
                value += fixed32_make_from_fract(5, 100);
            else
                value -= fixed32_make_from_fract(5, 100);
            int_part = fixed32_get_int(value);
            fract_part = fixed32_get_fract_by_denom(value, 10);
            *data = int_part * 10 + fract_part;
            break;
        case PARAM_TYPE_FRACT_100:
            if(value >= 0)
                value += fixed32_make_from_fract(5, 1000);
            else
                value -= fixed32_make_from_fract(5, 1000);
            int_part = fixed32_get_int(value);
            fract_part = fixed32_get_fract_by_denom(value, 100);
            *data = int_part * 100 + fract_part;
            break;
        case PARAM_TYPE_FRACT_1000:
            if(value >= 0)
                value += fixed32_make_from_fract(5, 10000);
            else
                value -= fixed32_make_from_fract(5, 10000);
            int_part = fixed32_get_int(value);
            fract_part = fixed32_get_fract_by_denom(value, 1000);
            *data = int_part * 1000 + fract_part;
            break;
        case PARAM_TYPE_FRACT_10000:
            if(value >= 0)
                value += fixed32_make_from_fract(5, 100000);
            else
                value -= fixed32_make_from_fract(5, 100000);
            int_part = fixed32_get_int(value);
            fract_part = fixed32_get_fract_by_denom(value, 10000);
            *data = int_part * 10000 + fract_part;
            break;
    }
}

err_t settings_init(void)
{
    size_t i = 0;
    size_t index = 0;
    
    const param_descr_t* descr;
    param_t* param;
    
    _settings_readonly = false;
    
    for(; i < PARAMETERS_COUNT; i ++){
        
        descr = settings_param_descr_by_index(i);
        param = settings_parameter_by_index(i);
        
        if(!(descr->flags & PARAM_FLAG_VIRTUAL)){
            if(index >= PARAMETERS_REAL_COUNT) return E_OUT_OF_RANGE;
            parameters_data.data[index] = 0;
            param->data_index = index ++;
        }else{
            param->virt_data = 0;
        }
        
        param->descr_index = i;
    }
    
    parameters_data.crc = 0;
    
    return E_NO_ERROR;
}

err_t settings_default(void)
{
    if(ro()) return E_STATE;
    
    size_t i = 0;
    
    const param_descr_t* descr;
    param_t* param;
    
    for(; i < PARAMETERS_COUNT; i ++){
        
        param = settings_parameter_by_index(i);
        descr = settings_param_descr_by_index(param->descr_index);
        
        switch(descr->type){
        default:
        case PARAM_TYPE_INT:
            settings_param_set_valuei(param, descr->def.int_value);
            break;
        case PARAM_TYPE_UINT:
            settings_param_set_valueu(param, descr->def.uint_value);
            break;
        case PARAM_TYPE_FRACT_10:
        case PARAM_TYPE_FRACT_100:
        case PARAM_TYPE_FRACT_1000:
        case PARAM_TYPE_FRACT_10000:
            settings_param_set_valuef(param, descr->def.fixed_value);
            break;
    }
    }
    
    return E_NO_ERROR;
}

err_t settings_read(void)
{
    if(ro()) return E_STATE;
    
    RETURN_ERR_IF_FAIL(
            storage_read(STORAGE_RGN_SETTINGS_ADDRESS, &parameters_data, sizeof(parameters_data))
        );
    
    uint16_t crc = crc16_ccitt(parameters_data.data, sizeof(parameters_data) - sizeof(uint16_t));
    
    if(crc != parameters_data.crc) return E_CRC;
    
    return E_NO_ERROR;
}

err_t settings_write(void)
{
    parameters_data.crc = crc16_ccitt(parameters_data.data, sizeof(parameters_data) - sizeof(uint16_t));
    
    return storage_write(STORAGE_RGN_SETTINGS_ADDRESS, &parameters_data, sizeof(parameters_data));
}

bool settings_readonly(void)
{
    return _settings_readonly;
}

void settings_set_readonly(bool readonly)
{
    _settings_readonly = readonly;
}

/**
 * Функция сравнения двух параметров по идентификатору.
 * @param a Параметр А.
 * @param b Параметр Б.
 * @return Результат сравнения.
 */
static int settings_compare_params_ids (const void * key, const void * item)
{
    const param_descr_t* item_descr = settings_param_descr_by_index(((param_t*)item)->descr_index);
    param_id_t item_id = item_descr->id;
    param_id_t id = (param_id_t)(unsigned long)key;
    
    if(id > item_id) return 1;
    if(id < item_id) return -1;
    
    return 0;
}

param_t* settings_param_by_id(param_id_t id)
{
    param_t* param = bsearch((void*)(unsigned long)id, parameters, PARAMETERS_COUNT, sizeof(param_t), settings_compare_params_ids);
    return param;
}

bool settings_param_is_virtual(param_t* param)
{
    return (settings_param_descr_by_index(param->descr_index)->flags & PARAM_FLAG_VIRTUAL) != 0;
}

param_type_t settings_param_type(param_t* param)
{
    return settings_param_descr_by_index(param->descr_index)->type;
}

bool settings_param_flags(param_t* param)
{
    return settings_param_descr_by_index(param->descr_index)->flags;
}

param_units_t settings_param_units(param_t* param)
{
    return settings_param_descr_by_index(param->descr_index)->units;
}

 param_data_t settings_param_value_raw(param_t* param)
 {
    const param_descr_t* descr = settings_param_descr_by_index(param->descr_index);
    return (descr->flags & PARAM_FLAG_VIRTUAL) ? (param->virt_data):
           (*settings_param_data_by_index(param->data_index));
 }
 
 bool settings_param_set_value_raw(param_t* param, param_data_t value)
 {
    if(ro()) return false;
    const param_descr_t* descr = settings_param_descr_by_index(param->descr_index);
    param_data_t* data = (descr->flags & PARAM_FLAG_VIRTUAL) ? (&param->virt_data):
                         (settings_param_data_by_index(param->data_index));
    *data = value;
    return true;
 }

int32_t settings_param_valuei(param_t* param)
{
    return settings_param_get_int32(param);
}

bool settings_param_set_valuei(param_t* param, int32_t value)
{
    if(ro()) return false;
    
    settings_param_set_int32(param, value);
    
    return true;
}

uint32_t settings_param_valueu(param_t* param)
{
    return settings_param_get_uint32(param);
}

bool settings_param_set_valueu(param_t* param, uint32_t value)
{
    if(ro()) return false;
    
    settings_param_set_uint32(param, value);
    
    return true;
}

fixed32_t settings_param_valuef(param_t* param)
{
    return settings_param_get_fixed32(param);
}

bool settings_param_set_valuef(param_t* param, fixed32_t value)
{
    if(ro()) return false;
    
    settings_param_set_fixed32(param, value);
    
    return true;
}

int32_t settings_valuei(param_id_t id)
{
    param_t* param = settings_param_by_id(id);
    if(!param) return 0;
    return settings_param_valuei(param);
}

uint32_t settings_valueu(param_id_t id)
{
    param_t* param = settings_param_by_id(id);
    if(!param) return 0;
    return settings_param_valueu(param);
}

fixed32_t settings_valuef(param_id_t id)
{
    param_t* param = settings_param_by_id(id);
    if(!param) return 0;
    return settings_param_valuef(param);
}
