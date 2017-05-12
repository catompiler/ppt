#include "drive_phase_sync.h"
#include "stm32f10x.h"
#include <arm_math.h>
#include <arm_const_structs.h>
#include <stdlib.h>
#include <string.h>
#include "defs/defs.h"
#include "utils/critical.h"
#include "utils/utils.h"
#include "cordic/cordic32.h"
#include "pid_controller/pid_controller.h"
#include "drive_power.h"



//! Размер FFT.
#define FFT_SIZE 32
#define FFT_SIZE2 (FFT_SIZE * 2)

//! Флаг реверса бит FFT.
#define FFT_BIT_REVERSE 1

//! Число буферов FFT.
#define FFT_BUFFERS_COUNT 2

//! Индекс действительной части первой гармоники.
#define FFT_BUFFER_FIRST_VALUE_REAL 2

//! Индекс мнимой части первой гармоники.
#define FFT_BUFFER_FIRST_VALUE_IMAG 3

//! Максимальное значение счётчика пропусков значений АЦП.
#define FFT_ADC_PRESCALER (DRIVE_POWER_ADC_FREQ / DRIVE_POWER_FREQ / FFT_SIZE)

//! Размер фильтра FFT.
#define FFT_FILTER_SIZE 7

//! Отбрасывать минимальных значений фильтра.
#define FFT_FILTER_SKIP_MIN 2

//! Отбрасывать максимальных значений фильтра.
#define FFT_FILTER_SKIP_MAX 2

//! Время регулирования ФАПЧ.
#define PLL_PID_DT 0x1b5//0x51f

//! Максимальный угол регулирования ФАПЧ в обе стороны.
#define PLL_PID_MAX_ANGLE 0xAA0000 // 170

//! Число микросекунд в электрическом градусе.
#define US_PER_DEGREE (0x378E38) // 55.(5)

//! Преобразование агла в микросекунды.
#define ANGLE_TO_US(A) (fixed32_mul((int64_t)(A), US_PER_DEGREE))

//! Угол в 120 градусов.
#define ANGLE_120 (0x780000)

//! Угол в 240 градусов.
#define ANGLE_240 (0xF00000)

//! Тип числа FFT.
typedef int16_t fft_int_t;

//! Тип индекса в буфере данных FFT.
typedef uint8_t fft_buffer_index_t;


//! Структура буфера исходных FFT.
typedef struct _Phase_Sync_Fft_Buffer {
    fft_int_t data[FFT_SIZE2]; //!< Данные буфера исходных данных FFT.
} phase_sync_fft_buffer_t;

//! Структура буферов FFT.
typedef struct _Phase_Sync_Fft_Buffers {
    phase_sync_fft_buffer_t buffer_a; //!< Буфер фазы A.
    phase_sync_fft_buffer_t buffer_b; //!< Буфер фазы B.
    phase_sync_fft_buffer_t buffer_c; //!< Буфер фазы C.
    fft_buffer_index_t index; //!< Индекс в данных буфера FFT.
    fixed32_t first_angle; //!< Угол первой точки буфера.
} phase_sync_fft_buffers_t;

/*
//! Структура значения фильтра FFT.
typedef struct _Phase_Sync_Fft_Filter_Value {
    int16_t x;
    int16_t y;
} phase_sync_fft_filter_value_t;

//! Структура фильтра значений FFT.
typedef struct _Phase_Sync_Fft_Filter {
    phase_sync_fft_filter_value_t values[FFT_FILTER_SIZE];
    fft_buffer_index_t index;
    fft_buffer_index_t count;
} phase_sync_fft_filter_t;
*/

//! Структура значения FFT.
typedef struct _Phase_Sync_Fft_Value {
    fixed32_t angle;
    fixed32_t offset_angle;
    fixed32_t delta_angle;
    int16_t offset_time;
    fixed32_t filter[FFT_FILTER_SIZE];
    fft_buffer_index_t filter_index;
    fft_buffer_index_t filter_count;
} phase_sync_fft_value_t;

//! Структура значений FFT.
typedef struct _Phase_Sync_Fft_Values {
    phase_sync_fft_value_t value_a;
    phase_sync_fft_value_t value_b;
    phase_sync_fft_value_t value_c;
} phase_sync_fft_values_t;

//! Перечисление состояний синхронизации фаз.
typedef enum _Drive_Phase_Sync_Calc_State {
    DRIVE_PHASE_SYNC_CALC_INIT = 0,
    DRIVE_PHASE_SYNC_CALC_DATA_WAIT = 1,
    DRIVE_PHASE_SYNC_CALC_PHASE_A = 2,
    DRIVE_PHASE_SYNC_CALC_PHASE_B = 3,
    DRIVE_PHASE_SYNC_CALC_PHASE_C = 4
} drive_phase_sync_calc_state_t;

typedef struct _Drive_Phase_Sync_Pll {
    pid_controller_t pid;
} drive_phase_sync_pll_t;

//! Структура синхронизации с фазами.
typedef struct _Drive_Phase_Sync {
    uint8_t adc_counter; //!< Счётчик пропусков значений АЦП.
    drive_phase_sync_angle_callback_t angle_callback; //!< Каллбэк получения угла.
    phase_sync_fft_buffers_t buffers[FFT_BUFFERS_COUNT]; //!< Буферы FFT.
    fft_buffer_index_t put_buffers; //!< Индекс текущих буферов FFT для записи.
    fft_buffer_index_t get_buffers; //!< Индекс текущих буферов FFT для чтения.
    phase_sync_fft_values_t values; //!< Значения FFT.
    drive_phase_sync_calc_state_t calc_state; //!< Состояние синхронизации фаз.
    uint8_t calc_phases_counter; //!< Счётчик пропусков вычисления фаз.
    drive_phase_sync_pll_t pll; //!< ФАПЧ.
    fixed32_t accuracy_angle; //!< Точность синхронизации.
} drive_phase_sync_t;

//! Синхронизация с фазами.
static drive_phase_sync_t phase_sync;


err_t drive_phase_sync_init(void)
{
    memset(&phase_sync, 0x0, sizeof(drive_phase_sync_t));
    
    pid_controller_init(&phase_sync.pll.pid, 0, 0, 0);
    
    phase_sync.put_buffers = 0;
    phase_sync.get_buffers = 1;
    
    return E_NO_ERROR;
}

void drive_phase_sync_reset(void)
{
    CRITICAL_ENTER();
    
    memset(&phase_sync.buffers, 0x0, sizeof(phase_sync_fft_buffers_t) * FFT_BUFFERS_COUNT);
    memset(&phase_sync.values, 0x0, sizeof(phase_sync_fft_values_t));
    
    phase_sync.adc_counter = 0;
    phase_sync.calc_phases_counter = 0;
    
    phase_sync.put_buffers = 0;
    phase_sync.get_buffers = 1;
    
    phase_sync.calc_state = DRIVE_PHASE_SYNC_CALC_INIT;
    pid_controller_reset(&phase_sync.pll.pid);
    
    CRITICAL_EXIT();
}

void drive_phase_sync_set_angle_callback(drive_phase_sync_angle_callback_t callback)
{
    phase_sync.angle_callback = callback;
}

/**
 * Получает буферы FFT для записи.
 * @return Буферы FFT.
 */
ALWAYS_INLINE static phase_sync_fft_buffers_t* phase_sync_cur_put_fft_buffers(void)
{
    return &phase_sync.buffers[phase_sync.put_buffers];
}

/**
 * Получает буферы FFT для чтения.
 * @return Буферы FFT.
 */
ALWAYS_INLINE static phase_sync_fft_buffers_t* phase_sync_cur_get_fft_buffers(void)
{
    return &phase_sync.buffers[phase_sync.get_buffers];
}

/**
 * Получает следующий индекс буферов данных FFT.
 * @param index Текущий индекс.
 * @return Следующий индекс.
 */
ALWAYS_INLINE static fft_buffer_index_t phase_sync_fft_buffer_next_index(fft_buffer_index_t index)
{
    if(index < FFT_BUFFERS_COUNT - 1) return index + 1;
    return 0;
}

/**
 * Сбрасывает буферы записи FFT.
 */
ALWAYS_INLINE static void phase_sync_reset_fft_put_buffers(void)
{
    phase_sync_fft_buffers_t* fft_buffers = phase_sync_cur_put_fft_buffers();
    
    fft_buffers->index = 0;
    fft_buffers->first_angle = 0;
}

/**
 * Переключает буферы данных FFT.
 */
ALWAYS_INLINE static void phase_sync_swap_fft_buffers(void)
{
    CRITICAL_ENTER();
    
    phase_sync.put_buffers = phase_sync_fft_buffer_next_index(phase_sync.put_buffers);
    phase_sync.get_buffers = phase_sync_fft_buffer_next_index(phase_sync.get_buffers);
    
    phase_sync_reset_fft_put_buffers();
    
    phase_sync.adc_counter = 0;
    
    CRITICAL_EXIT();
}

// fixed32_t -> fft_int_t ==> val >> 11;
/**
 * Преобразует значение напряжения к типу FFT.
 * @param val Значение напряжения.
 * @return Значение для FFT.
 */
ALWAYS_INLINE static fft_int_t fixed32_to_fft_int(fixed32_t val)
{
    return val >> 12;
}

/**
 * Преобразует выходное значение RFFT в число с фиксированной запятой fixed32.
 * @param val Значение RFFT.
 * @return Число с фиксированной запятой.
 */
ALWAYS_INLINE static fixed32_t fft_value_to_fixed32(fft_int_t val)
{
    return (fixed32_t)val << 5;
}

static void phase_sync_fft_buffer_put(phase_sync_fft_buffer_t* fft_buffer, fft_buffer_index_t index, fixed32_t val)
{
    fft_buffer_index_t index2 = index * 2;
    fft_buffer->data[index2] = fixed32_to_fft_int(val);
    fft_buffer->data[index2 + 1] = 0;//index * 41;
}

/*
static void phase_sync_fft_buffer_put_raw(phase_sync_fft_buffer_t* fft_buffer, fft_buffer_index_t index, int16_t val)
{
    fft_buffer_index_t index2 = index * 2;
    fft_buffer->data[index2] = val;
    fft_buffer->data[index2 + 1] = 0;//index * 41;
}
*/

void drive_phase_sync_append_data(void)
{
    if(phase_sync.adc_counter == 0){
        
        phase_sync_fft_buffers_t* fft_buffers = phase_sync_cur_put_fft_buffers();

        if(fft_buffers->index < FFT_SIZE){

            if(fft_buffers->index == 0){
                if(phase_sync.angle_callback){
                    fft_buffers->first_angle = phase_sync.angle_callback();
                }
            }
            
            fixed32_t Ua = drive_power_channel_real_value_inst(DRIVE_POWER_Ua);
            fixed32_t Ub = drive_power_channel_real_value_inst(DRIVE_POWER_Ub);
            fixed32_t Uc = drive_power_channel_real_value_inst(DRIVE_POWER_Uc);

            phase_sync_fft_buffer_put(&fft_buffers->buffer_a, fft_buffers->index,
                    Ub - Ua);

            phase_sync_fft_buffer_put(&fft_buffers->buffer_b, fft_buffers->index,
                    Uc - Ub);

            phase_sync_fft_buffer_put(&fft_buffers->buffer_c, fft_buffers->index,
                    Ua - Uc);

            fft_buffers->index ++;
        }
    }
    phase_sync.adc_counter ++;
    if(phase_sync.adc_counter >= FFT_ADC_PRESCALER){
        phase_sync.adc_counter = 0;
    }
}

void drive_phase_sync_swap_buffers(void)
{
    phase_sync_swap_fft_buffers();
}

/**
 * Получает буфер FFT для заданной фазы.
 * @param fft_buffers Буферы FFT.
 * @param phase Фаза.
 * @return Буфер FFT.
 */
static phase_sync_fft_buffer_t* phase_sync_get_buffer(phase_sync_fft_buffers_t* fft_buffers, phase_t phase)
{
    switch(phase){
        default:
            break;
        case PHASE_A:
            return &fft_buffers->buffer_a;
        case PHASE_B:
            return &fft_buffers->buffer_b;
        case PHASE_C:
            return &fft_buffers->buffer_c;
    }
    return NULL;
}

/**
 * Получает значение FFT для заданной фазы.
 * @param phase Фаза.
 * @return Значение FFT.
 */
static phase_sync_fft_value_t* phase_sync_get_value(phase_t phase)
{
    switch(phase){
        default:
            break;
        case PHASE_A:
            return &phase_sync.values.value_a;
        case PHASE_B:
            return &phase_sync.values.value_b;
        case PHASE_C:
            return &phase_sync.values.value_c;
    }
    return NULL;
}

/**
 * Получает флаг пустоты буфера.
 * @param fft_buffer Буфер.
 * @return Флаг пустоты буфера.
 */
ALWAYS_INLINE static bool phase_sync_buffers_empty(phase_sync_fft_buffers_t* fft_buffers)
{
    return fft_buffers->index == 0;
}

/**
 * Получает флаг полноты буфера.
 * @param fft_buffer Буфер.
 * @return Флаг полноты буфера.
 */
ALWAYS_INLINE static bool phase_sync_buffers_full(phase_sync_fft_buffers_t* fft_buffers)
{
    return fft_buffers->index >= FFT_SIZE;
}

/**
 * Очищает незаполненную часть буфера.
 * @param fft_buffers Буферы FFT.
 * @param fft_buffer Буфер для очистки.
 */
ALWAYS_INLINE static void phase_sync_buffer_clear_tail(phase_sync_fft_buffers_t* fft_buffers, phase_sync_fft_buffer_t* fft_buffer)
{
    fft_buffer_index_t index2 = fft_buffers->index * 2;
    memset(&fft_buffer->data[index2], 0x0, (FFT_SIZE2 - index2) * sizeof(fft_int_t));
}

/**
 * Добавляет данные в буфер фильтра.
 * @param fft_value Значение FFT.
 * @param angle Вычисленный угол.
 */
static void phase_sync_filter_put(phase_sync_fft_value_t* fft_value, fixed32_t angle)
{
    fft_value->filter[fft_value->filter_index] = angle;
    
    if(++ fft_value->filter_index >= FFT_FILTER_SIZE) fft_value->filter_index = 0;
    if(fft_value->filter_count < FFT_FILTER_SIZE) fft_value->filter_count ++;
}

/**
 * Функция сравнения двух значений.
 * @param a Значение А.
 * @param b Значение Б.
 * @return Результат сравнения.
 */
static int fft_filter_cmp(const void* a, const void* b)
{
    return (*(fixed32_t*)a - *(fixed32_t*)b);
}

/**
 * Вычисляет значение фильтра.
 * @param fft_value Значение FFT.
 * @return Вычисленное значение фильтра.
 */
static fixed32_t phase_sync_filter_calc(phase_sync_fft_value_t* fft_value)
{
    if(fft_value->filter_count == 0) return 0;
    
    size_t count = fft_value->filter_count;
    
    fixed32_t buffer[count];
    
    memcpy(buffer, fft_value->filter, sizeof(fixed32_t) * count);
    
    qsort(buffer, count, sizeof(fixed32_t), fft_filter_cmp);
    
    fixed32_t val = 0;
    size_t from = 0;
    size_t to = count;
    
    if(count > (FFT_FILTER_SKIP_MIN + FFT_FILTER_SKIP_MAX)){
        from += FFT_FILTER_SKIP_MIN;
        to -= FFT_FILTER_SKIP_MAX;
        count -= (FFT_FILTER_SKIP_MIN + FFT_FILTER_SKIP_MAX);
    }
    
    while(from < to){
        val  += buffer[from];
        from ++;
    }
    
    val /= (fixed32_t)count;
    
    return val;
}

/**
 * Получает флаг заполнения фильтра значения FFT.
 * @param fft_value Значение FFT.
 * @return Флаг заполнения фильтра.
 */
static bool phase_sync_filter_full(phase_sync_fft_value_t* fft_value)
{
    return fft_value->filter_count == FFT_FILTER_SIZE;
}

/**
 * Вычисляет значение угла фазы для буфера FFT.
 * @param fft_buffers Буферы данных FFT.
 * @param fft_buffer Буфер для вычисления.
 * @param fft_value Значение FFT.
 * @return Код ошибки.
 */
static err_t drive_phase_sync_calc_buffer(phase_sync_fft_buffers_t* fft_buffers, phase_sync_fft_buffer_t* fft_buffer, phase_sync_fft_value_t* fft_value)
{
    if(phase_sync_buffers_empty(fft_buffers)) return E_OUT_OF_RANGE;
    
    if(!phase_sync_buffers_full(fft_buffers)) phase_sync_buffer_clear_tail(fft_buffers, fft_buffer);
    
    arm_cfft_q15(&arm_cfft_sR_q15_len32, fft_buffer->data, 0, FFT_BIT_REVERSE);
    
    fixed32_t x = (fixed32_t)fft_value_to_fixed32(fft_buffer->data[FFT_BUFFER_FIRST_VALUE_REAL]);
    fixed32_t y = (fixed32_t)fft_value_to_fixed32(fft_buffer->data[FFT_BUFFER_FIRST_VALUE_IMAG]);
    fixed32_t angle = 0;
    
    cordic32_atan2_hyp(x, y, &angle, NULL);

    /**/
    // Учтём начальный угол.
    angle -= fft_buffers->first_angle;
    // Прибавим 90 градусов чтобы получить фазу синусоиды.
    angle += CORDIC32_ANGLE_90;
    // Нормализуем значение.
    if(angle >= CORDIC32_ANGLE_360) angle -= CORDIC32_ANGLE_360;
    
    // Приведём к интервалу -180 ... +180.
    if(angle > CORDIC32_ANGLE_180) angle = -(CORDIC32_ANGLE_360 - angle);
    /**/
    
    // Отфильтруем.
    phase_sync_filter_put(fft_value, angle);
    angle = phase_sync_filter_calc(fft_value);
    
    fft_value->angle = angle;
    
    return E_NO_ERROR;
}

err_t drive_phase_sync_calc(phase_t phase)
{
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    
    phase_sync_fft_buffers_t* fft_buffers = phase_sync_cur_get_fft_buffers();
    
    phase_sync_fft_buffer_t* fft_buffer = phase_sync_get_buffer(fft_buffers, phase);
    phase_sync_fft_value_t* fft_value = phase_sync_get_value(phase);
    
    if(!fft_buffer || !fft_value) return E_NULL_POINTER;
    
    return drive_phase_sync_calc_buffer(fft_buffers, fft_buffer, fft_value);
}

err_t drive_phase_sync_angle(phase_t phase, fixed32_t* angle)
{
    if(phase == PHASE_UNK) return E_INVALID_VALUE;
    if(angle == NULL) return E_NULL_POINTER;
    
    phase_sync_fft_value_t* fft_value = phase_sync_get_value(phase);
    
    if(!fft_value) return E_NULL_POINTER;
    
    *angle = fft_value->angle;
    
    return E_NO_ERROR;
}

int16_t drive_phase_sync_offset(phase_t phase)
{
    if(phase == PHASE_UNK) return 0;
    
    phase_sync_fft_value_t* fft_value = phase_sync_get_value(phase);
    
    if(!fft_value) return 0;
    
    return fft_value->offset_time;
}

fixed32_t drive_phase_sync_offset_angle(phase_t phase)
{
    if(phase == PHASE_UNK) return 0;
    
    phase_sync_fft_value_t* fft_value = phase_sync_get_value(phase);
    
    if(!fft_value) return 0;
    
    return fft_value->offset_angle;
}

fixed32_t drive_phase_sync_delta_angle(phase_t phase)
{
    if(phase == PHASE_UNK) return 0;
    
    phase_sync_fft_value_t* fft_value = phase_sync_get_value(phase);
    
    if(!fft_value) return 0;
    
    return fft_value->delta_angle;
}

fixed32_t drive_phase_sync_diff_delta_angle(phase_t phase)
{
    return drive_phase_sync_delta_angle(phase) - ANGLE_120;
}

err_t drive_phase_sync_process_calc(void)
{
    err_t err = E_NO_ERROR;
    
    switch(phase_sync.calc_state){
        case DRIVE_PHASE_SYNC_CALC_INIT:
            drive_phase_sync_swap_buffers();
            phase_sync.calc_phases_counter = 0;
            phase_sync.calc_state = DRIVE_PHASE_SYNC_CALC_DATA_WAIT;
        case DRIVE_PHASE_SYNC_CALC_DATA_WAIT:
            if(++ phase_sync.calc_phases_counter == PHASES_COUNT){
                phase_sync.calc_state = DRIVE_PHASE_SYNC_CALC_PHASE_A;
                phase_sync.calc_phases_counter = 0;
            }
            break;
        case DRIVE_PHASE_SYNC_CALC_PHASE_A:
            drive_phase_sync_swap_buffers();
            err = drive_phase_sync_calc(PHASE_A);
            phase_sync.calc_state = DRIVE_PHASE_SYNC_CALC_PHASE_B;
            break;
        case DRIVE_PHASE_SYNC_CALC_PHASE_B:
            err = drive_phase_sync_calc(PHASE_B);
            phase_sync.calc_state = DRIVE_PHASE_SYNC_CALC_PHASE_C;
            break;
        case DRIVE_PHASE_SYNC_CALC_PHASE_C:
            err = drive_phase_sync_calc(PHASE_C);
            phase_sync.calc_state = DRIVE_PHASE_SYNC_CALC_PHASE_A;
            break;
    }
    
    return err;
}

bool drive_phase_sync_data_avail(void)
{
    return phase_sync_filter_full(&phase_sync.values.value_a) &&
           phase_sync_filter_full(&phase_sync.values.value_b) &&
           phase_sync_filter_full(&phase_sync.values.value_c);
}

void drive_phase_sync_set_pll_pid(fixed32_t kp, fixed32_t ki, fixed32_t kd)
{
    pid_controller_set_kp(&phase_sync.pll.pid, kp);
    pid_controller_set_ki(&phase_sync.pll.pid, ki);
    pid_controller_set_kd(&phase_sync.pll.pid, kd);
}

void drive_phase_sync_pll_pid_clamp(fixed32_t pid_min, fixed32_t pid_max)
{
    pid_controller_clamp(&phase_sync.pll.pid, pid_min, pid_max);
}

fixed32_t drive_phase_sync_pll_pid_value(void)
{
    fixed32_t res = pid_controller_value(&phase_sync.pll.pid);
    
    /*
    fixed32_t abs_angle = fixed_abs(phase_sync.values.value_a.angle);
    
    if(abs_angle > PLL_PID_MAX_ANGLE){
        res = MAX(0, res);
    }
    */
    
    return res;
}

bool drive_phase_sync_pll_regulate(void)
{
    if(!drive_phase_sync_data_avail()) return false;
    
    if(phase_sync.calc_state != DRIVE_PHASE_SYNC_CALC_PHASE_B) return false;
    
    fixed32_t angle_err = phase_sync.values.value_a.angle;
    
    if(angle_err < -PLL_PID_MAX_ANGLE){
        angle_err = fixed_abs(angle_err);
    }
    
    return pid_controller_calculate(&phase_sync.pll.pid, angle_err, PLL_PID_DT);
}

 void drive_phase_sync_set_accuracy(fixed32_t angle)
 {
     phase_sync.accuracy_angle = angle;
 }
 
 bool drive_phase_sync_synchronized(void)
 {
     if(!drive_phase_sync_data_avail()) return false;
     
     fixed32_t delta_angle = fixed_abs(phase_sync.values.value_a.angle);
     
     if(delta_angle > phase_sync.accuracy_angle) return false;
     
     return true;
 }
 
 drive_dir_t drive_phase_sync_current_dir(void)
 {
     if(!drive_phase_sync_data_avail()) return DRIVE_DIR_UNK;
     
     fixed32_t ab = phase_sync.values.value_a.angle - phase_sync.values.value_b.angle;
     fixed32_t ac = phase_sync.values.value_a.angle - phase_sync.values.value_c.angle;
     
     if(ab < 0) ab += CORDIC32_ANGLE_360;
     if(ac < 0) ac += CORDIC32_ANGLE_360;
     
     if(ac > ab) return DRIVE_DIR_FORW;
     if(ac < ab) return DRIVE_DIR_BACKW;
     
     return DRIVE_DIR_UNK;
 }

phase_t drive_phase_sync_current_phase(void)
{
    drive_dir_t dir = drive_phase_sync_current_dir();

    if(dir == DRIVE_DIR_UNK) return PHASE_UNK;

    phase_t phase = PHASE_UNK;

    switch(phase_sync.calc_state){
        default:
            break;
        case DRIVE_PHASE_SYNC_CALC_PHASE_B: // last caclulated: A
            phase = PHASE_A;
            break;
        case DRIVE_PHASE_SYNC_CALC_PHASE_C: // last caclulated: B
            phase = (dir == DRIVE_DIR_FORW) ? PHASE_B : PHASE_C;
            break;
        case DRIVE_PHASE_SYNC_CALC_PHASE_A: // last caclulated: C
            phase = (dir == DRIVE_DIR_FORW) ? PHASE_C : PHASE_B;
            break;
    }
    
    return phase;
}

phase_t drive_phase_sync_next_phase(void)
{
    drive_dir_t dir = drive_phase_sync_current_dir();

    if(dir == DRIVE_DIR_UNK) return PHASE_UNK;

    phase_t phase = PHASE_UNK;

    switch(phase_sync.calc_state){
        default:
            break;
        case DRIVE_PHASE_SYNC_CALC_PHASE_A: // last caclulated: C
            phase = PHASE_A;
            break;
        case DRIVE_PHASE_SYNC_CALC_PHASE_B: // last caclulated: A
            phase = (dir == DRIVE_DIR_FORW) ? PHASE_B : PHASE_C;
            break;
        case DRIVE_PHASE_SYNC_CALC_PHASE_C: // last caclulated: B
            phase = (dir == DRIVE_DIR_FORW) ? PHASE_C : PHASE_B;
            break;
    }
    
    return phase;
}

bool drive_phase_sync_calc_offsets(void)
{
    if(!drive_phase_sync_data_avail()) return false;

    fixed32_t angle_a = phase_sync.values.value_a.angle;
    fixed32_t angle_b = phase_sync.values.value_b.angle;
    fixed32_t angle_c = phase_sync.values.value_c.angle;
    
    fixed32_t ref_angle_a = 0;
    fixed32_t ref_angle_b = 0;
    fixed32_t ref_angle_c = 0;
    
    fixed32_t offsetf = 0;
    
    drive_dir_t dir = drive_phase_sync_current_dir();
    
    if(dir == DRIVE_DIR_FORW){
        ref_angle_b = -ANGLE_120;
        ref_angle_c = ANGLE_120;
    }
    else if(dir == DRIVE_DIR_BACKW){
        ref_angle_b = ANGLE_120;
        ref_angle_c = -ANGLE_120;
    }
    // DRIVE_DIR_UNK
    else{
        return false;
    }
    
    angle_a -= ref_angle_a;
    offsetf = ANGLE_TO_US(angle_a);
    phase_sync.values.value_a.offset_angle = angle_a;
    phase_sync.values.value_a.offset_time = fixed32_get_int(offsetf);
    
    angle_b -= ref_angle_b;
    offsetf = ANGLE_TO_US(angle_b);
    phase_sync.values.value_b.offset_angle = angle_b;
    phase_sync.values.value_b.offset_time = fixed32_get_int(offsetf);
    
    angle_c -= ref_angle_c;
    offsetf = ANGLE_TO_US(angle_c);
    phase_sync.values.value_c.offset_angle = angle_c;
    phase_sync.values.value_c.offset_time = fixed32_get_int(offsetf);

    return true;
}

static fixed32_t drive_phase_sync_calc_delta_angle(fixed32_t angle_a, fixed32_t angle_b)
{
    if(angle_b >= angle_a) return angle_b - angle_a;
    return (angle_b + CORDIC32_ANGLE_360) - angle_a;
}

bool drive_phase_sync_calc_delta_angles(void)
{
    if(!drive_phase_sync_data_avail()) return false;

    fixed32_t angle_a = phase_sync.values.value_a.angle;
    fixed32_t angle_b = phase_sync.values.value_b.angle;
    fixed32_t angle_c = phase_sync.values.value_c.angle;
    
    fixed32_t ref_angle_a = 0;
    fixed32_t ref_angle_b = 0;
    fixed32_t ref_angle_c = 0;
    
    drive_dir_t dir = drive_phase_sync_current_dir();
    
    if(dir == DRIVE_DIR_FORW){
        ref_angle_a = angle_c;
        ref_angle_b = angle_a;
        ref_angle_c = angle_b;
    }
    else if(dir == DRIVE_DIR_BACKW){
        ref_angle_a = angle_b;
        ref_angle_b = angle_c;
        ref_angle_c = angle_a;
    }
    // DRIVE_DIR_UNK
    else{
        return false;
    }
    angle_a = drive_phase_sync_calc_delta_angle(angle_a, ref_angle_a);
    angle_b = drive_phase_sync_calc_delta_angle(angle_b, ref_angle_b);
    angle_c = drive_phase_sync_calc_delta_angle(angle_c, ref_angle_c);
    
    phase_sync.values.value_a.delta_angle = angle_a;
    phase_sync.values.value_b.delta_angle = angle_b;
    phase_sync.values.value_c.delta_angle = angle_c;

    return true;
}

bool drive_phase_sync_process(void)
{
    err_t err = drive_phase_sync_process_calc();
    if(err != E_NO_ERROR) return false;
    
    bool res = drive_phase_sync_pll_regulate();
    if(!res) return false;
    
    res = drive_phase_sync_calc_offsets();
    if(!res) return false;
    
    res = drive_phase_sync_calc_delta_angles();
    if(!res) return false;
    
    return true;
}
