#include "drive_selftuning.h"
#include "drive_power.h"
#include "defs/defs.h"
#include <string.h>
#include "tree/rbtree.h"
#include "drive_hires_timer.h"


//! Объединение данных точки самонастройки.
typedef union _SelftuningPointData{
    uint16_t adc_data; //!< Данные с АЦП.
    fixed32_t value; //!< Данные питания.
} tuning_point_data_t;

//! Структура элемента дерева самонастройки.
typedef struct _Selftuning_Tree_Item {
    rbtree_node_t node; //!< Элемент дерева.
    //fixed32_t time_ms; //!< Отметка времени, мс.
    tuning_point_data_t U_rot; //!< Данные напряжения якоря.
    tuning_point_data_t I_rot; //!< Данные тока якоря.
} tuning_tree_item_t;

//! Число элементов пула дерева (количесвто элементов для самонастройки).
#define TUNING_POOL_SIZE DRIVE_POWER_OSC_CHANNEL_LEN //100

//! Пул элементов дерева самонастройки.
typedef struct _Tuning_Tree_Pool {
    tuning_tree_item_t items[TUNING_POOL_SIZE];
    uint8_t index;
} tuning_tree_pool_t;

//! Структура самонастройки привода.
typedef struct _Drive_Selftuning {
    rbtree_t tree; //!< Дерево точек.
    tuning_tree_pool_t pool; //!< Пул элементов дерева.
    bool data_collecting; //!< Флаг сбора данных АЦП.
    struct timeval adc_base_tv; //!< Время начала открытия тиристоров.
    fixed32_t adc_t; //!< Время последних данных АЦП.
    fixed32_t adc_dt; //!< Время между данными АЦП.
} drive_selftuning_t;

//! Самонастройка привода.
static drive_selftuning_t tuning;



static int tuning_compare(rbtree_key_t lkey, rbtree_key_t rkey)
{
    return (int)lkey - (int)rkey;
}

ALWAYS_INLINE static void tuning_pool_reset(void)
{
    tuning.pool.index = 0;
}

ALWAYS_INLINE static tuning_tree_item_t* tuning_pool_get_item(size_t index)
{
    return &tuning.pool.items[index];
}

ALWAYS_INLINE static tuning_tree_item_t* tuning_pool_alloc(void)
{
    if(tuning.pool.index >= TUNING_POOL_SIZE) return NULL;
    
    return &tuning.pool.items[tuning.pool.index ++];
}

ALWAYS_INLINE static bool tuning_pool_free_last(tuning_tree_item_t* item)
{
    if(tuning.pool.index == 0) return false;
    
    if(&tuning.pool.items[tuning.pool.index - 1] != item) return false;
    
    tuning.pool.index --;
    
    return true;
}

ALWAYS_INLINE static tuning_tree_item_t* tuning_pool_node_item(rbtree_node_t* node)
{
    return (tuning_tree_item_t*)rbtree_node_value(node);
}

ALWAYS_INLINE static void tuning_pool_init_item(tuning_tree_item_t* item, uint16_t adc_u, uint16_t adc_i, fixed32_t time)
{
    item->U_rot.adc_data = adc_u;
    item->I_rot.adc_data = adc_i;
    
    rbtree_node_init(&item->node, (rbtree_key_t)time, (rbtree_value_t)item);
}

ALWAYS_INLINE static bool tuning_pool_insert_item(tuning_tree_item_t* item)
{
    return rbtree_insert_node(&tuning.tree, &item->node);
}

static void tuning_pool_init(void)
{
    rbtree_init(&tuning.tree, tuning_compare, NULL, NULL);
    
    tuning_tree_item_t* item;
    size_t i;
    for(i = 0; i < TUNING_POOL_SIZE; i ++){
        item = tuning_pool_get_item(i);
        
        rbtree_node_init(&item->node, (rbtree_key_t)NULL, (rbtree_value_t)item);
    }
}

err_t drive_selftuning_init(void)
{
    memset(&tuning, 0x0, sizeof(drive_selftuning_t));
    
    tuning_pool_init();
    
    tuning.adc_dt = fixed32_make_from_fract(1000, POWER_ADC_FREQ);
    
    return E_NO_ERROR;
}

void drive_selftuning_reset(void)
{
    tuning.data_collecting = false;
    tuning_pool_reset();
    rbtree_init(&tuning.tree, tuning_compare, NULL, NULL);
    drive_selftuning_reset_adc_time();
    drive_selftuning_set_adc_rate(1);
}

void drive_selftuning_set_data_collecting(bool collecting)
{
    tuning.data_collecting = collecting;
}

bool drive_selftuning_data_collecting(void)
{
    return tuning.data_collecting;
}

bool drive_selftuning_data_collected(void)
{
    return tuning.pool.index == TUNING_POOL_SIZE;
}

void drive_selftuning_reset_adc_time(void)
{
    drive_hires_timer_value(&tuning.adc_base_tv);
    tuning.adc_t = 0;
}

void drive_selftuning_set_adc_rate(uint32_t adc_rate)
{
    tuning.adc_dt = fixed32_make_from_fract(1000, (POWER_ADC_FREQ * (int32_t)adc_rate));
}

bool drive_selftuning_put(uint16_t U_rot_adc, uint16_t I_rot_adc)
{
    if(!tuning.data_collecting) return false;
    
    tuning_tree_item_t* item = tuning_pool_alloc();
    
    if(!item) return false;
    
    if(tuning.adc_t == 0){
        struct timeval tv;
        drive_hires_timer_value(&tv);
        
        timersub(&tv, &tuning.adc_base_tv, &tv);
        
        fixed32_t time_ms = fixed32_make_from_fract((int64_t)tv.tv_usec, 1000);
        fixed32_t offset = time_ms;
        
        tuning.adc_t = offset;
    }
    
    tuning_pool_init_item(item, U_rot_adc, I_rot_adc, tuning.adc_t);
    
    if(!tuning_pool_insert_item(item)){
        tuning_pool_free_last(item);
    }
    
    tuning.adc_t += tuning.adc_dt;
    
    return true;
}

void drive_selftuning_calculate_adc_data(void)
{
    drive_selftuning_data_iter_t iter = drive_selftuning_data_iter_begin();
    
    tuning_tree_item_t* item;
    
    while(!drive_selftuning_data_iter_at_end(iter)){
        
        item = tuning_pool_node_item((rbtree_node_t*)iter);
        
        item->U_rot.value = drive_power_channel_calc_inst_value(DRIVE_POWER_Urot, item->U_rot.adc_data);
        item->I_rot.value = drive_power_channel_calc_inst_value(DRIVE_POWER_Irot, item->I_rot.adc_data);
        
        iter = drive_selftuning_data_iter_next(iter);
    }
}

drive_selftuning_data_iter_t drive_selftuning_data_iter_begin(void)
{
    return (drive_selftuning_data_iter_t)rbtree_first(&tuning.tree);
}

drive_selftuning_data_iter_t drive_selftuning_data_iter_next(drive_selftuning_data_iter_t iter)
{
    return (drive_selftuning_data_iter_t)rbtree_next(&tuning.tree, (rbtree_node_t*)iter);
}

bool drive_selftuning_data_iter_at_end(drive_selftuning_data_iter_t iter)
{
    return iter == NULL;
}

fixed32_t drive_selftuning_data_iter_urot(drive_selftuning_data_iter_t iter)
{
    tuning_tree_item_t* item = tuning_pool_node_item((rbtree_node_t*)iter);
    
    return item->U_rot.value;
}

fixed32_t drive_selftuning_data_iter_irot(drive_selftuning_data_iter_t iter)
{
    tuning_tree_item_t* item = tuning_pool_node_item((rbtree_node_t*)iter);
    
    return item->I_rot.value;
}

fixed32_t drive_selftuning_data_iter_time(drive_selftuning_data_iter_t iter)
{
    tuning_tree_item_t* item = tuning_pool_node_item((rbtree_node_t*)iter);
    
    return (fixed32_t)rbtree_node_key(&item->node);
}

