#include "drive_selftuning.h"
#include "drive_power.h"
#include "defs/defs.h"
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "tree/rbtree.h"
#include "drive_hires_timer.h"
#include "settings.h"
#include "drive.h"
#include "utils/utils.h"


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
    fixed32_t dI; //!< Дифференциал тока.
} tuning_tree_item_t;

//! Число элементов пула дерева (количесвто элементов для самонастройки).
#define TUNING_POOL_SIZE DRIVE_POWER_OSC_CHANNEL_LEN //100

//! Пул элементов дерева самонастройки.
typedef struct _Tuning_Tree_Pool {
    tuning_tree_item_t items[TUNING_POOL_SIZE];
    uint8_t index;
} tuning_tree_pool_t;

//! Структура самонастройки привода.
typedef struct _Selftuning {
    // Данные самонастройки.
    rbtree_t tree; //!< Дерево точек.
    tuning_tree_pool_t pool; //!< Пул элементов дерева.
    // Состояние самонастройки.
    bool data_collecting; //!< Флаг сбора данных АЦП.
    size_t data_iter; //!< Счётчик итераций сбора данных.
    // Промежуточный результат (сумма).
    fixed32_t r_sum; //!< Сумма измерений сопротивления.
    fixed32_t l_sum; //!< Сумма измерений индуктивности.
    // Вычисленные значения.
    fixed32_t R; //!< Значение сопротивления.
    fixed32_t L; //!< Значение индуктивности.
    // Время точек АЦП.
    struct timeval adc_base_tv; //!< Время начала открытия тиристоров.
    fixed32_t adc_t; //!< Время последних данных АЦП.
    fixed32_t adc_dt; //!< Время между данными АЦП.
    // Настройки самонастройки.
    fixed32_t open_angle; //!< Угол открытия для самонастройки.
    size_t data_iters_count; //!< Число итераций сбора данных.
    //size_t data_filter_avg; //!< Число элементов для усреднения.
    size_t data_delta_avg; //!< Число элементов для разности.
    bool use_mid_filter; //!< Флаг использования медианного фильтра.
} selftuning_t;

//! Самонастройка привода.
static selftuning_t tuning;

//! Количество итераций сбора данных по-умолчанию.
#define TUNING_DATA_ITERS_DEFAULT 10
//! Число элементов усреднения для фильтра по-умолчанию.
//#define TUNING_DATA_FILTER_AVG_DEFAULT 1
//! Число элементов для дифференцирования по-умолчанию.
#define TUNING_DATA_DELTA_AVG_DEFAULT 1
//! Флаг использования медианного фильтра по-умолчанию.
#define TUNING_USE_MID_FILTER_DEFAULT true;

//! Множитель АЦП по-умолчанию.
#define TUNING_ADC_RATE_DEFAULT DRIVE_ADC_RATE_NORMAL



// Коэффициент минимальной разницы во времени между двумя точками АЦП.
#define TUNING_CMP_DELTA_RATE 8

static int tuning_compare(rbtree_key_t lkey, rbtree_key_t rkey)
{
    fixed32_t lt = (fixed32_t)lkey;
    fixed32_t rt = (fixed32_t)rkey;
    
    fixed32_t dt = lt - rt;
    
    fixed32_t min_dt = tuning.adc_dt / TUNING_CMP_DELTA_RATE;
    
    if(dt > min_dt) return 1;
    if(dt < -min_dt) return -1;
    
    return 0;
    
    //return (int)lkey - (int)rkey;
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
    
    drive_selftuning_reset();
    
    return E_NO_ERROR;
}

void drive_selftuning_update_settings(void)
{
    tuning.open_angle = settings_valuef(PARAM_ID_SELFTUNE_OPEN_ANGLE);
    tuning.use_mid_filter = settings_valueu(PARAM_ID_SELFTUNE_USE_MID_FILTER);
    //tuning.data_filter_avg = TUNING_DATA_FILTER_AVG_DEFAULT;
    tuning.data_delta_avg = settings_valueu(PARAM_ID_SELFTUNE_DIDT_AVG_COUNT);
    tuning.data_iters_count = settings_valueu(PARAM_ID_SELFTUNE_ITERS_COUNT);
}

fixed32_t drive_selftuning_open_angle(void)
{
    return tuning.open_angle;
}

void drive_selftuning_reset(void)
{
    tuning.data_collecting = false;
    tuning.data_iter = 0;
    tuning.r_sum = 0;
    tuning.l_sum = 0;
    
    drive_selftuning_clear_data();
    
    drive_selftuning_reset_adc_time();
    drive_selftuning_set_adc_rate(TUNING_ADC_RATE_DEFAULT);
}

void drive_selftuning_clear_data(void)
{
    tuning_pool_reset();
    rbtree_init(&tuning.tree, tuning_compare, NULL, NULL);
}

bool drive_selftuning_data_collecting_done(void)
{
    return tuning.data_iter >= tuning.data_iters_count;
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

/**
 * Выполняет сокращение 64 бит числа до 32 бит с насыщением.
 * @param f Число 64 бит.
 * @return Число 32 бит.
 */
static fixed32_t tuning_satf(int64_t f)
{
    if(f > INT32_MAX) return INT32_MAX;
    if(f < INT32_MIN) return INT32_MIN;
    return (fixed32_t)f;
}

/**
 * Получает элемент дерева данных относительно данного.
 * @param node Элемент дерева.
 * @param index Чилос шагов. Отрицательное - назад, положительное - вперёд.
 * @return Элемент дерева.
 */
/*static rbtree_node_t* tuning_tree_node_at_rel(rbtree_node_t* node, int index)
{
    //if(index == 0) return node;
    
    int step = (index >= 0) ? (-1) : (1);
    
    rbtree_node_t* (*tree_pos_proc)(rbtree_t* tree, rbtree_node_t* node) = 
            (index >= 0) ? rbtree_next : rbtree_prev;
    
    rbtree_node_t* node_next;
    
    for(;;){
        if(index == 0) break;
        
        node_next = tree_pos_proc(&tuning.tree, node);
        
        if(!node_next) break;
        
        node = node_next;
        index += step;
    }
    
    return node;
}*/

/**
 * Получает среднее значение данных эелементов дерева.
 * @param node Элемент дерева.
 * @param index Чилос шагов. Отрицательное - назад, положительное - вперёд.
 */
static void tuning_tree_node_calc_avg(rbtree_node_t* node, int steps,
        fixed32_t* avg_u_rot, fixed32_t* avg_i_rot, fixed32_t* avg_di, fixed32_t* avg_t)
{
    if(steps == 0) return;
    
    size_t count = (size_t)ABS(steps);
    
    int64_t sum_u = 0;
    int64_t sum_i = 0;
    int64_t sum_di = 0;
    int64_t sum_t = 0;
    
    int step = (steps >= 0) ? (-1) : (1);
    
    rbtree_node_t* (*tree_pos_proc)(rbtree_t* tree, rbtree_node_t* node) = 
            (steps >= 0) ? rbtree_next : rbtree_prev;
    
    rbtree_node_t* node_next;
    
    tuning_tree_item_t* item;
    
    while(node && steps){
        item = tuning_pool_node_item(node);
        
        if(avg_u_rot) sum_u += item->U_rot.value;
        if(avg_i_rot) sum_i += item->I_rot.value;
        if(avg_di) sum_di += item->dI;
        if(avg_t) sum_t += (fixed32_t)rbtree_node_key(node);
        
        node_next = tree_pos_proc(&tuning.tree, node);
        
        if(node_next){
            node = node_next;
        }
        
        steps += step;
    }
    
    if(avg_u_rot) *avg_u_rot = tuning_satf(sum_u / count);
    if(avg_i_rot) *avg_i_rot = tuning_satf(sum_i / count);
    if(avg_di) *avg_di = tuning_satf(sum_di / count);
    if(avg_t) *avg_t = tuning_satf(sum_t / count);
}

/**
 * Вычисляет dI/dt.
 */
static void tuning_calc_didt(void)
{
    rbtree_node_t* node = rbtree_first(&tuning.tree);
    rbtree_node_t* node_prev = node;
    
    if(!node) return;
    
    int count = (int)tuning.data_delta_avg;
    
    tuning_tree_item_t* item;
    
    fixed32_t Icur, Iprev;
    fixed32_t Tcur, Tprev;
    fixed32_t di, dt;
    int64_t didt;
    
    mid_filter3i_t filter_didt;
    
    mid_filter3i_init(&filter_didt);
    
    while(node){
        
        Iprev = 0;
        Tprev = 0;
        
        Icur = 0;
        Tcur = 0;
        
        tuning_tree_node_calc_avg(node_prev, -count, NULL, &Iprev, NULL, &Tprev);
        tuning_tree_node_calc_avg(node,       count, NULL, &Icur,  NULL, &Tcur);
        
        item = tuning_pool_node_item(node);
        
        dt = Tcur - Tprev;
        di = Icur - Iprev;
        
        if(dt > 0){
            didt = fixed32_div((int64_t)di, dt);
        }else{
            didt = 0;
        }
        
        if(tuning.use_mid_filter){
            mid_filter3i_put(&filter_didt, tuning_satf(didt));
            item->dI = mid_filter3i_value(&filter_didt);
        }else{
            item->dI = tuning_satf(didt);
        }
        
        node_prev = node;
        node = rbtree_next(&tuning.tree, node);
    }
}

/**
 * Вычисляет коэффициенты для системы линейных уравнений
 * по методу наименьших квадратов.
 * Из массива выборок, связанных уравнением:
 * (I*R)n + (dI/dt)n - (U)n = 0;
 * получаем систему уравнений:
 * {{
 * { a*R + b*L + c = 0,
 * { d*R + e*L + f = 0.
 * {{
 */
static void tuning_calc_coefs(int64_t* pa, int64_t* pb, int64_t* pc, int64_t* pd, int64_t* pe, int64_t* pf)
{
    int64_t a = 0;
    int64_t b = 0;
    int64_t c = 0;
    int64_t d = 0;
    int64_t e = 0;
    int64_t f = 0;
    
    rbtree_node_t* node = rbtree_first(&tuning.tree);
    
    tuning_tree_item_t* item;
    
    // ir + didt - u = 0;
    
    while(node){
        item = tuning_pool_node_item(node);
        
        a = a + fixed32_mul((int64_t)item->I_rot.value, item->I_rot.value);
        b = b + fixed32_mul((int64_t)item->dI,          item->I_rot.value);
        c = c - fixed32_mul((int64_t)item->U_rot.value, item->I_rot.value);
        
        d = d + fixed32_mul((int64_t)item->I_rot.value, item->dI);
        e = e + fixed32_mul((int64_t)item->dI,          item->dI);
        f = f - fixed32_mul((int64_t)item->U_rot.value, item->dI);
        
        node = rbtree_next(&tuning.tree, node);
    }
    
    if(pa) *pa = a;
    if(pb) *pb = b;
    if(pc) *pc = c;
    if(pd) *pd = d;
    if(pe) *pe = e;
    if(pf) *pf = f;
}

static bool tuning_calc_rl(fixed32_t* pr, fixed32_t* pl)
{
    int64_t a = 0;
    int64_t b = 0;
    int64_t c = 0;
    int64_t d = 0;
    int64_t e = 0;
    int64_t f = 0;
    
    tuning_calc_coefs(&a, &b, &c, &d, &e, &f);
    
    if(a == 0) return false;
    
    int64_t dc = fixed32_mul(d, c);
    int64_t fa = fixed32_mul(f, a);
    int64_t ae = fixed32_mul(a, e);
    int64_t db = fixed32_mul(d, b);
    
    int64_t ae_db = ae - db;
    
    if(ae_db == 0) return false;
    
    int64_t dc_fa = dc - fa;
    
    int64_t y = fixed32_div(dc_fa, ae_db);
    
    int64_t by = fixed32_mul(b, y);
    int64_t by_c = -by - c;
    
    int64_t x = fixed32_div(by_c, a);
    
    if(pr) *pr = tuning_satf(x);
    if(pl) *pl = tuning_satf(y);
    
    return true;
}

void drive_selftuning_calculate_adc_data(void)
{
    rbtree_node_t* node = rbtree_first(&tuning.tree);
    
    tuning_tree_item_t* item;
    
    fixed32_t U_rot, I_rot;
    
    mid_filter3i_t filter_u_rot, filter_i_rot;
    
    mid_filter3i_init(&filter_u_rot);
    mid_filter3i_init(&filter_i_rot);
    
    while(node){
        
        item = tuning_pool_node_item(node);
        
        U_rot = drive_power_channel_calc_inst_value(DRIVE_POWER_Urot, item->U_rot.adc_data);
        I_rot = drive_power_channel_calc_inst_value(DRIVE_POWER_Irot, item->I_rot.adc_data);
        
        if(tuning.use_mid_filter){
            mid_filter3i_put(&filter_u_rot, U_rot);
            mid_filter3i_put(&filter_i_rot, I_rot);

            item->U_rot.value = mid_filter3i_value(&filter_u_rot);
            item->I_rot.value = mid_filter3i_value(&filter_i_rot);
        }else{
            item->U_rot.value = U_rot;
            item->I_rot.value = I_rot;
        }
        
        node = rbtree_next(&tuning.tree, node);
    }
}

bool drive_selftuning_calculate_current_data(void)
{
    fixed32_t r, l;
    
    tuning_calc_didt();
    
    if(!tuning_calc_rl(&r, &l)) return false;
    
    tuning.l_sum += l;
    tuning.r_sum += r;
    tuning.data_iter ++;
    
    return true;
}

void drive_selftuning_calculate(void)
{
    if(tuning.data_iter != 0){
        tuning.R = tuning.r_sum / tuning.data_iter;
        tuning.L = tuning.l_sum / tuning.data_iter;
    
        param_t* pr = settings_param_by_id(PARAM_ID_MOTOR_R_ROT_NOM);
        param_t* pl = settings_param_by_id(PARAM_ID_MOTOR_L_ROT_NOM);

        if(pr) settings_param_set_valuef(pr, tuning.R);
        if(pl) settings_param_set_valuef(pl, tuning.L);
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

fixed32_t drive_selftuning_data_iter_di(drive_selftuning_data_iter_t iter)
{
    tuning_tree_item_t* item = tuning_pool_node_item((rbtree_node_t*)iter);
    
    return item->dI;
}

