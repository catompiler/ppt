/**
 * @file main.c Главный файл проекта ППТ.
 */
#include "stm32f10x.h"
#include "usart/usart_bus.h"
#include "modbus/modbus_rtu.h"
#include <stdio.h>
#include "rtc/rtc.h"
#include "spi/spi.h"
#include "dma/dma.h"
#include "utils/delay.h"
#include "utils/utils.h"
#include "tft9341/tft9341.h"
#include "fixed/fixed32.h"
#include "pca9555/pca9555.h"
#include "m95x/m95x.h"
#include "lm75/lm75.h"
#include "storage.h"
#include "drive.h"
#include "settings.h"
#include "drive_events.h"
#include "drive_ui.h"
#include "drive_keypad.h"
#include "drive_gui.h"
#include "drive_modbus.h"
#include "drive_tasks.h"
#include "drive_dio.h"
#include "drive_dip.h"
#include "drive_nvdata.h"
#include "drive_temp.h"
#include "drive_hires_timer.h"
#include "utils/critical.h"
#include "drive_selftuning.h"
#include "drive_task_selftune.h"
#include <string.h>
#include <stdlib.h>
#undef LIST_H
#include <FreeRTOS.h>
#include <list.h>
#include <task.h>
#include <errno.h>


//! GPIO контроля интерфейса RS485.
#define RS485_IFACE_CTL_GPIO GPIOC
//! Пин контроля интерфейса RS485.
#define RS485_IFACE_CTL_PIN GPIO_Pin_9

//! Шина USART для Modbus 485.
static usart_bus_t usart_bus_485;
//! Modbus 485.
static modbus_rtu_t modbus_485;
//! Сообщения Modbus 485.
static modbus_rtu_message_t modbus_485_rx_msg, modbus_485_tx_msg;

//! Шина USART для Modbus BT.
static usart_bus_t usart_bus_bt;
//! Modbus BT.
static modbus_rtu_t modbus_bt;
//! Сообщения Modbus BT.
static modbus_rtu_message_t modbus_bt_rx_msg, modbus_bt_tx_msg;

//! Счётчик миллисекунд для функции gettimeofday.
static uint32_t counter_ms_gtod = 0;
//static struct timeval counter_gtod = {0, 0};


//! Шина spi.
static spi_bus_t spi;

//! Шина spi2.
static spi_bus_t spi2;

//! Микросхема EEPROM.
static m95x_t eeprom;

//! Порт EEPROM.
#define M95X_CE_GPIO GPIOD
//! Пин EEPROM.
#define M95X_CE_PIN GPIO_Pin_8


// Пины i2c1.
//! Пин SDA.
#define I2C1_PIN_SDA (GPIO_Pin_7)
//! Пин SCL.
#define I2C1_PIN_SCL (GPIO_Pin_6)
//! Порт i2c1.
#define I2C1_GPIO (GPIOB)
//! Шина i2c.
static i2c_bus_t i2c1;

// Пины i2c2.
//! Пин SDA.
#define I2C2_PIN_SDA (GPIO_Pin_11)
//! Пин SCL.
#define I2C2_PIN_SCL (GPIO_Pin_10)
//! Порт i2c2.
#define I2C2_GPIO (GPIOB)
//! Шина i2c2.
static i2c_bus_t i2c2;

#define I2C_RESET_CLK_HALF_PERIOD_US (100)

//! Датчик температуры радиатора.
static lm75_t heatsink_sensor;
//! Адрес датчика температуры радиатора.
#define HEATSINK_SENSOR_ADDRESS (LM75_I2C_DEFAULT_ADDRESS | 0x1)
//! Таймаут обмена данными с датчиком температуры радиатора, мкс.
#define HEATSINK_SENSOR_IO_TIMEOUT_US (200000)
//! Таймаут попыток обмена данными с датчиком температуры радиатора, с.
#define HEATSINK_SENSOR_TIMEOUT_S (35)
//! Период обновления температуры, сек.
#define DRIVE_TEMP_UPDATE_PERIOD_S (10)

// Шим вентилятора.
//! Таймер для ШИМ.
#define HEATSINK_FAN_PWM_TIM TIM8
//! Минимальное значение ШИМ.
#define HEATSINK_FAN_PWM_MIN 0
// Максимальное значение ШИМ.
#define HEATSINK_FAN_PWM_MAX 999

//! Расширитель ввода-вывода.
static pca9555_t ioport;
//! Таймаут обмена данными с расширителем ввода-вывода, мкс.
#define IOPORT_IO_TIMEOUT_US (200000)


// Watchdogs.
// Работа с шиной i2c1.
#define WATCHDOG_I2C1 0
// Работа с шиной i2c2.
#define WATCHDOG_I2C2 1


//#define DEBUG_RESET_I2C 1


//! TFT9341.
static tft9341_t tft;
// Порты и пины GPIO для TFT.
#define TFT_DC_GPIO        GPIOB
#define TFT_DC_PIN         GPIO_Pin_4
#define TFT_CE_GPIO        GPIOA
#define TFT_CE_PIN         GPIO_Pin_15
#define TFT_RST_GPIO       GPIOC
#define TFT_RST_PIN        GPIO_Pin_12
/******************************************************************************/

// EXTI.
//! GPIO PCA9555.
#define EXTI_PCA9555_GPIO GPIOD
//! Пин PCA95555.
#define EXTI_PCA9555_Pin GPIO_Pin_7
//! Линия PCA9555.
#define EXTI_PCA9555_LINE EXTI_Line7
//! Линия RTC Alarm.
#define EXTI_RTC_ALARM_LINE EXTI_Line17


//! Таймер открытия тиристоров и итераций привода.
#define DRIVE_MAIN_TIM TIM7
//! Делитель (счётчик) частоты основного таймера привода.
static uint8_t drive_main_iter_prescaler = 0;

// ADC.
//! Таймер АЦП.
#define ADC_TIM (TIM1)
//! Минимальный множитель дисктретизации АЦП.
#define ADC_FREQ_MULT_MIN 1
//! Максимальный множитель дисктретизации АЦП.
#define ADC_FREQ_MULT_MAX 10
//! Флаг необходимости изменения частоты DMA АЦП.
static bool adc_change_rate = false;
//! Текущая частота АЦП.
static uint8_t adc_rate = ADC_FREQ_MULT_MIN;
//! Необходимая частота АЦП.
static uint8_t adc_new_rate = ADC_FREQ_MULT_MIN;

//! Количество каналов ADC 1 и 2.
#define ADC12_CHANNELS_COUNT 4
//! Длина буфера ADC 1 и 2 в трансферах 32 бит.
#define ADC12_RAW_BUFFER_DMA_TRANSFERS (ADC12_CHANNELS_COUNT)
#define ADC12_RAW_BUFFER_DMA_TRANSFERS_MAX (ADC12_CHANNELS_COUNT * ADC_FREQ_MULT_MAX)
//! Размер буфера ADC 1 и 2.
#define ADC12_RAW_BUFFER_SIZE (ADC12_RAW_BUFFER_DMA_TRANSFERS_MAX * 2)
//! Буфер ADC12.
static volatile uint16_t adc12_raw_buffer[ADC12_RAW_BUFFER_SIZE] = {0};

//! Количество каналов ADC 3.
#define ADC3_CHANNELS_COUNT 3
//! Длина буфера ADC 3 в трансферах 16 бит.
#define ADC3_RAW_BUFFER_DMA_TRANSFERS (ADC3_CHANNELS_COUNT)
#define ADC3_RAW_BUFFER_DMA_TRANSFERS_MAX (ADC3_CHANNELS_COUNT * ADC_FREQ_MULT_MAX)
//! Размер буфера ADC 3.
#define ADC3_RAW_BUFFER_SIZE (ADC3_RAW_BUFFER_DMA_TRANSFERS_MAX)
//! Буфер ADC3.
static volatile uint16_t adc3_raw_buffer[ADC3_RAW_BUFFER_SIZE] = {0};

//! Количество каналов трёх АЦП суммарно.
#define ADC_CHANNELS_COUNT (ADC12_CHANNELS_COUNT + ADC12_CHANNELS_COUNT + ADC3_CHANNELS_COUNT)
//! Размер данных буфера ADC12
#define ADC12_DATA_SIZE ((ADC12_CHANNELS_COUNT + ADC12_CHANNELS_COUNT) * 2)
//! Размер данных буфера ADC3
#define ADC3_DATA_SIZE (ADC3_CHANNELS_COUNT * 2)
//! Буфер ADC однократного измерения каналов.
static uint16_t adc_raw_buffer[ADC_CHANNELS_COUNT] = {0};
//! Буфер ADC.
//static volatile uint16_t adc_raw_buffer[ADC12_RAW_BUFFER_SIZE + ADC3_RAW_BUFFER_SIZE] = {0};
// Количество каналов самонастройки.
#define ADC_TUNING_CHANNELS_COUNT 2
// Максимальная длина буфера.
#define ADC_TUNING_BUFFER_LEN (ADC_TUNING_CHANNELS_COUNT * ADC_FREQ_MULT_MAX)
// Буфер для значенией самонастройки.
static uint16_t adc_raw_tuning_buffer[ADC_TUNING_BUFFER_LEN];


// Константы для цифровых входов-выходов.
#define DIGITAL_IO_SPEED GPIO_Speed_2MHz
#define DIGITAL_IO_IN_MODE GPIO_Mode_IN_FLOATING
#define DIGITAL_IO_OUT_MODE GPIO_Mode_Out_PP
// Цифровые выхода.
#define DIGITAL_IO_OUT_1_GPIO GPIOE
#define DIGITAL_IO_OUT_1_PIN GPIO_Pin_6
#define DIGITAL_IO_OUT_2_GPIO GPIOE
#define DIGITAL_IO_OUT_2_PIN GPIO_Pin_5
#define DIGITAL_IO_OUT_3_GPIO GPIOE
#define DIGITAL_IO_OUT_3_PIN GPIO_Pin_4
#define DIGITAL_IO_OUT_4_GPIO GPIOC
#define DIGITAL_IO_OUT_4_PIN GPIO_Pin_13
// Цифровые входа.
#define DIGITAL_IO_IN_1_GPIO GPIOB
#define DIGITAL_IO_IN_1_PIN GPIO_Pin_9
#define DIGITAL_IO_IN_2_GPIO GPIOE
#define DIGITAL_IO_IN_2_PIN GPIO_Pin_0
#define DIGITAL_IO_IN_3_GPIO GPIOE
#define DIGITAL_IO_IN_3_PIN GPIO_Pin_1
#define DIGITAL_IO_IN_4_GPIO GPIOE
#define DIGITAL_IO_IN_4_PIN GPIO_Pin_2
#define DIGITAL_IO_IN_5_GPIO GPIOE
#define DIGITAL_IO_IN_5_PIN GPIO_Pin_3


/*
 * Приоритеты прерываний.
 */
#define IRQ_PRIOR_TRIACS_TIMER 1
#define IRQ_PRIOR_TRIAC_EXC_TIMER 2

#define IRQ_PRIOR_HIRES_TIMER 5

#define IRQ_PRIOR_RTOS_MAX 8

//#define IRQ_PRIOR_ADC_TIM (IRQ_PRIOR_RTOS_MAX + 0)
#define IRQ_PRIOR_ADC_DMA (IRQ_PRIOR_RTOS_MAX + 1)
#define IRQ_PRIOR_NULL_TIMER (IRQ_PRIOR_RTOS_MAX + 2)
#define IRQ_PRIOR_RTC (IRQ_PRIOR_RTOS_MAX + 3)

#define IRQ_PRIOR_MODBUS_USART (IRQ_PRIOR_RTOS_MAX + 4)
#define IRQ_PRIOR_I2C1 (IRQ_PRIOR_RTOS_MAX + 5)
#define IRQ_PRIOR_I2C2 (IRQ_PRIOR_RTOS_MAX + 5)
#define IRQ_PRIOR_SPI1 (IRQ_PRIOR_RTOS_MAX + 5)
#define IRQ_PRIOR_SPI2 (IRQ_PRIOR_RTOS_MAX + 5)
#define IRQ_PRIOR_USART (IRQ_PRIOR_RTOS_MAX + 5)
#define IRQ_PRIOR_DMA_CH1 (IRQ_PRIOR_RTOS_MAX + 5)
#define IRQ_PRIOR_DMA_CH2 (IRQ_PRIOR_RTOS_MAX + 5) // spi1
#define IRQ_PRIOR_DMA_CH3 (IRQ_PRIOR_RTOS_MAX + 5) // spi1
#define IRQ_PRIOR_DMA_CH4 (IRQ_PRIOR_RTOS_MAX + 5) // spi2 usart1 (modbus)
#define IRQ_PRIOR_DMA_CH5 (IRQ_PRIOR_RTOS_MAX + 5) // spi2 usart1 (modbus) i2c2
#define IRQ_PRIOR_DMA_CH6 (IRQ_PRIOR_RTOS_MAX + 5) // i2c1 usart2 i2c2
#define IRQ_PRIOR_DMA_CH7 (IRQ_PRIOR_RTOS_MAX + 5) // i2c1 usart2
#define IRQ_PRIOR_KEYPAD (IRQ_PRIOR_RTOS_MAX + 6)
#define IRQ_PRIOR_RTC_ALARM (IRQ_PRIOR_RTOS_MAX + 6)


/*
 * Приоритеты задач.
 */
#define DRIVE_TASK_PRIORITY_ADC 15
#define DRIVE_TASK_PRIORITY_SYNC 14
#define DRIVE_TASK_PRIORITY_TRIACS 13
#define DRIVE_TASK_PRIORITY_MAIN 12
#define DRIVE_TASK_PRIORITY_MODBUS 10
#define DRIVE_TASK_PRIORITY_STORAGE 8
#define DRIVE_TASK_PRIORITY_UTILS 7
#define DRIVE_TASK_PRIORITY_SELFTUNE 6
//#define DRIVE_TASK_PRIORITY_TIMER 5
#define DRIVE_TASK_PRIORITY_I2C_WDT 4
#define DRIVE_TASK_PRIORITY_TEMP 3
#define DRIVE_TASK_PRIORITY_BUZZ 2
#define DRIVE_TASK_PRIORITY_UI 1
//#define DRIVE_TASK_PRIORITY_ 0
//#define DRIVE_TASK_PRIORITY_ 0


//! Атрибуты обработчиков прерываний.
#define IRQ_ATTRIBS __attribute__ ((interrupt ("IRQ")))


/*
 * Выкидывать жалко.
 */

/*
// Время моргания для аппаратных ошибок.
#define DRIVE_MCU_FAULT_T_ON 4
#define DRIVE_MCU_FAULT_T_OFF 1

// Время моргания для ошибки переполнения стека.
#define DRIVE_STACK_OVERFLOW_T_ON 2
#define DRIVE_STACK_OVERFLOW_T_OFF 2



//! Задержка на заданное число секунд.
static void delay_s(uint32_t t)
{
    while(t --){
        delay_ms(1000);
    }
}


//! Моргание выходом ERROR.
//! Индикация фатальной ошибки.
static void mcu_fault_blink_loop(uint32_t t_on_s, uint32_t t_off_s)
{
    for(;;){
        drive_dio_set_output_type_state(DRIVE_DIO_OUT_ERROR, DRIVE_DIO_ON);
        delay_s(t_on_s);
        drive_dio_set_output_type_state(DRIVE_DIO_OUT_ERROR, DRIVE_DIO_OFF);
        delay_s(t_off_s);
    }
}
*/

/*
 * Обработчики исключений.
 */

IRQ_ATTRIBS void HardFault_Handler(void)
{
    drive_mcu_fault();
    
    //mcu_fault_blink_loop(DRIVE_MCU_FAULT_T_ON, DRIVE_MCU_FAULT_T_OFF);
    
    for(;;);
}

IRQ_ATTRIBS void MemManage_Handler(void)
{
    drive_mcu_fault();
    
    //mcu_fault_blink_loop(DRIVE_MCU_FAULT_T_ON, DRIVE_MCU_FAULT_T_OFF);
    
    for(;;);
}

IRQ_ATTRIBS void BusFault_Handler(void)
{
    drive_mcu_fault();
    
    //mcu_fault_blink_loop(DRIVE_MCU_FAULT_T_ON, DRIVE_MCU_FAULT_T_OFF);
    
    for(;;);
}

IRQ_ATTRIBS void UsageFault_Handler(void)
{
    drive_mcu_fault();
    
    //mcu_fault_blink_loop(DRIVE_MCU_FAULT_T_ON, DRIVE_MCU_FAULT_T_OFF);
    
    for(;;);
}

/*
 * Обработчики прерываний.
 */

IRQ_ATTRIBS void USART1_IRQHandler(void)
{
    usart_bus_irq_handler(&usart_bus_485);
}

IRQ_ATTRIBS void USART3_IRQHandler(void)
{
    usart_bus_irq_handler(&usart_bus_bt);
}

IRQ_ATTRIBS void SPI1_IRQHandler(void)
{
    spi_bus_irq_handler(&spi);
}

IRQ_ATTRIBS void SPI2_IRQHandler(void)
{
    spi_bus_irq_handler(&spi2);
}

IRQ_ATTRIBS void DMA1_Channel2_IRQHandler(void)
{
    if(usart_bus_dma_tx_channel_irq_handler(&usart_bus_bt)) return;
    if(spi_bus_dma_rx_channel_irq_handler(&spi)) return;
}

IRQ_ATTRIBS void DMA1_Channel3_IRQHandler(void)
{
    if(usart_bus_dma_rx_channel_irq_handler(&usart_bus_bt)) return;
    if(spi_bus_dma_tx_channel_irq_handler(&spi)) return;
}

IRQ_ATTRIBS void DMA1_Channel4_IRQHandler(void)
{
    if(usart_bus_dma_tx_channel_irq_handler(&usart_bus_485)) return;
    if(spi_bus_dma_rx_channel_irq_handler(&spi2)) return;
    if(i2c_bus_dma_tx_channel_irq_handler(&i2c2)) return;
}

IRQ_ATTRIBS void DMA1_Channel5_IRQHandler(void)
{
    if(usart_bus_dma_rx_channel_irq_handler(&usart_bus_485)) return;
    if(spi_bus_dma_tx_channel_irq_handler(&spi2)) return;
    if(i2c_bus_dma_rx_channel_irq_handler(&i2c2)) return;
}

IRQ_ATTRIBS void DMA1_Channel6_IRQHandler(void)
{
    i2c_bus_dma_tx_channel_irq_handler(&i2c1);
}

IRQ_ATTRIBS void DMA1_Channel7_IRQHandler(void)
{
    i2c_bus_dma_rx_channel_irq_handler(&i2c1);
}

IRQ_ATTRIBS void I2C1_EV_IRQHandler(void)
{
    i2c_bus_event_irq_handler(&i2c1);
}

IRQ_ATTRIBS void I2C1_ER_IRQHandler(void)
{
    i2c_bus_error_irq_handler(&i2c1);
}

IRQ_ATTRIBS void I2C2_EV_IRQHandler(void)
{
    i2c_bus_event_irq_handler(&i2c2);
}

IRQ_ATTRIBS void I2C2_ER_IRQHandler(void)
{
    i2c_bus_error_irq_handler(&i2c2);
}

IRQ_ATTRIBS void RTC_IRQHandler(void)
{
    rtc_interrupt_handler();
}

IRQ_ATTRIBS void RTCAlarm_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_RTC_ALARM_LINE) != RESET){
        EXTI_ClearITPendingBit(EXTI_RTC_ALARM_LINE);
    }
    rtc_alarm_interrupt_handler();
}

/*
 * RTC second callback.
 */
static void rtc_on_second(void)
{
    counter_ms_gtod = xTaskGetTickCountFromISR();
    
    //drive_hires_timer_value(&counter_gtod);
    
    drive_nvdata_inc_lifetime();
    
    if(drive_running()){
        drive_nvdata_inc_runtime();
        drive_nvdata_inc_last_runtime();
    }
    
    if(drive_temp_fan_running()){
        drive_nvdata_inc_fan_runtime(drive_temp_fan_rpm());
    }
    
    drive_nvdata_update_time_params();
}

/*
 * RTC alarm callback.
 */
/*
static void rtc_on_alarm(void)
{
}
*/

/*
 * get/set time of day functions.
 */
int _gettimeofday(struct timeval *tv, struct timezone *tz)
{
    if(tv == NULL){
        errno = EFAULT;
        return -1;
    }
    
    tv->tv_sec = rtc_time(NULL);
    
    uint32_t ticks_old = counter_ms_gtod;
    uint32_t ticks = xTaskGetTickCount();
    
    if(ticks >= ticks_old){
        ticks -= ticks_old;
    }else{
        ticks = UINT32_MAX - ticks_old + ticks + 1;
    }
    tv->tv_usec = (ticks * 1000 / configTICK_RATE_HZ) * 1000;
    if(tv->tv_usec >= 1000000){
        tv->tv_usec = 1000000 - 1;
        //tv->tv_sec ++;
    }
    
    if(tz != NULL){
        tz->tz_minuteswest = 0;
        tz->tz_dsttime = DST_NONE;
    }
    
    return 0;
}

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    if(tv == NULL){
        errno = EFAULT;
        return -1;
    }
    
    if(tz != NULL){
        if(tz->tz_dsttime != DST_NONE){
            errno = EINVAL;
            return -1;
        }
    }
    
    rtc_set_time(&tv->tv_sec);
    
    return 0;
}

/**
 * Обработчик прерывания таймера АЦП.
 */
IRQ_ATTRIBS void TIM1_UP_IRQHandler(void)
{
    if(ADC_TIM->SR & TIM_SR_UIF){
        ADC_TIM->SR = ~TIM_SR_UIF;
        
        ADC_TIM->DIER &= ~TIM_DIER_UIE;
    }
}

static void adc_set_rate_impl(uint32_t rate);

/**
 * Обработчик окончания передачи данных от ADC.
 */
IRQ_ATTRIBS void DMA1_Channel1_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_TCIF1){
        DMA1->IFCR = DMA_IFCR_CTCIF1;
        
        if(adc_change_rate){
            adc_set_rate_impl(adc_new_rate);
        }
        
        memcpy(
                (void*)&adc_raw_buffer[0],
                (void*)&adc12_raw_buffer[(ADC12_CHANNELS_COUNT + ADC12_CHANNELS_COUNT) * (adc_rate - 1)],
                ADC12_DATA_SIZE
            );
        memcpy(
                (void*)&adc_raw_buffer[ADC12_CHANNELS_COUNT + ADC12_CHANNELS_COUNT],
                (void*)&adc3_raw_buffer[(ADC3_CHANNELS_COUNT) * (adc_rate - 1)],
                ADC3_DATA_SIZE
            );
        
        BaseType_t pxHigherPriorityTaskWoken = 0;
        
        drive_task_adc_process_data_isr((uint16_t*)adc_raw_buffer, &pxHigherPriorityTaskWoken);
        
        // Самонастройка.
        size_t i, rindex, tindex;
        if(drive_selftuning_data_collecting() && !drive_selftuning_data_collected()){
            // Первый цикл - сохранение данных в буфер.
            for(i = 0; i < adc_rate; i ++){
                tindex = i * (ADC_TUNING_CHANNELS_COUNT);
                rindex = i * (ADC12_CHANNELS_COUNT + ADC12_CHANNELS_COUNT);
                adc_raw_tuning_buffer[tindex + 0] = adc12_raw_buffer[DRIVE_POWER_Urot + rindex];
                adc_raw_tuning_buffer[tindex + 1] = adc12_raw_buffer[DRIVE_POWER_Irot + rindex];
            }
            // Второй цикл - обработка.
            for(i = 0; i < adc_rate; i ++){
                tindex = i * (ADC_TUNING_CHANNELS_COUNT);
                if(!drive_selftuning_put(adc_raw_tuning_buffer[tindex + 0],
                                         adc_raw_tuning_buffer[tindex + 1])){
                    break;
                }
            }
        }
        
        if(adc_change_rate){
            adc_rate = adc_new_rate;
            adc_change_rate = false;
        }
        
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    }
}

/**
 * Прерывание от регистра PCA9555 по изменению состояния кнопок.
 */
IRQ_ATTRIBS void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line7);
        drive_keypad_pressed();
    }
}

IRQ_ATTRIBS void TIM2_IRQHandler(void)
{
    drive_triac_pairs_timer0_irq_handler();
}

IRQ_ATTRIBS void TIM3_IRQHandler(void)
{
    drive_triac_pairs_timer1_irq_handler();
}

IRQ_ATTRIBS void TIM4_IRQHandler(void)
{
    drive_triac_exc_timer_irq_handler();
}

// Функция сброса сторожевого таймера.
static void reset_watchdog(void);
// Прерывание таймера виртуального нуля.
IRQ_ATTRIBS void TIM7_IRQHandler(void)
{
    if(TIM_GetITStatus(DRIVE_MAIN_TIM, TIM_IT_Update) != RESET){
        TIM_ClearITPendingBit(DRIVE_MAIN_TIM, TIM_IT_Update);
        
        reset_watchdog();
    
        if(drive_state() == DRIVE_STATE_SELFTUNE && drive_selftuning_data_collecting()){
            drive_selftuning_set_data_collecting(false);
        }
        
        bool need_yield = false;
        
        need_yield |= drive_task_triacs_process_isr();
            
        if(drive_main_iter_prescaler == 0){
            need_yield |= drive_task_main_process_isr();
            need_yield |= drive_task_sync_process_isr();
        }
        
        if(drive_main_iter_prescaler >= DRIVE_MAIN_ITER_PRESCALER){
            drive_main_iter_prescaler = 0;
        }else{
            drive_main_iter_prescaler ++;
        }
        
        if(need_yield) portYIELD();
    }
}

/*
 * Функции обратного вызова (каллбэки).
 */

static bool i2c_callback(void)
{
    return pca9555_i2c_callback(&ioport);
}

static bool i2c2_callback(void)
{
    return lm75_i2c_callback(&heatsink_sensor);
}

static bool spi_callback(void)
{
    return tft9341_spi_callback(&tft);
}

static bool spi2_callback(void)
{
    return m95x_spi_callback(&eeprom);
}

/*
 * Modbus rs485
 */

static void modbus_rs485_set_input(void)
{
    RS485_IFACE_CTL_GPIO->BRR = RS485_IFACE_CTL_PIN;
}

static void modbus_rs485_set_output(void)
{
    RS485_IFACE_CTL_GPIO->BSRR = RS485_IFACE_CTL_PIN;
}

static bool usart_485_rx_byte_callback(uint8_t byte)
{
    return modbus_rtu_usart_rx_byte_callback(&modbus_485, byte);
}

static bool usart_485_rx_callback(void)
{
    return modbus_rtu_usart_rx_callback(&modbus_485);
}

static bool usart_485_tx_callback(void)
{
    return modbus_rtu_usart_tx_callback(&modbus_485);
}

static bool usart_485_tc_callback(void)
{
    modbus_rs485_set_input();
    
    return true;
}

static void modbus_485_on_msg_recv(void)
{
    drive_gui_modbus_set_last_time();
    
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

    if(drive_task_modbus_process_isr(&modbus_485, true, &pxHigherPriorityTaskWoken)){
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    }
}

static void modbus_485_on_msg_sent(void)
{
    drive_gui_modbus_set_last_time();
}

/*
 * Modbus Bluetooth.
 */

static bool usart_bt_rx_byte_callback(uint8_t byte)
{
    return modbus_rtu_usart_rx_byte_callback(&modbus_bt, byte);
}

static bool usart_bt_rx_callback(void)
{
    return modbus_rtu_usart_rx_callback(&modbus_bt);
}

static bool usart_bt_tx_callback(void)
{
    return modbus_rtu_usart_tx_callback(&modbus_bt);
}

/*static bool usart_bt_tc_callback(void)
{
    return true;
}*/

static void modbus_bt_on_msg_recv(void)
{
    drive_gui_modbus_bt_set_last_time();

    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;

    if(drive_task_modbus_process_isr(&modbus_bt, false, &pxHigherPriorityTaskWoken)){
        portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    }
}

static void modbus_bt_on_msg_sent(void)
{
    drive_gui_modbus_bt_set_last_time();
}

/*
 * Инициализация.
 */

static void init_dip(void)
{
    drive_dip_init();
    drive_dip_update();
}

static void remap_config(void)
{
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    if(drive_dip_pin_state(DRIVE_DIP_UI_PAD)){
        GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
    }
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);
    //GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
}

static void init_periph_clock(void)
{
    // GPIO.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    // AFIO.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    // ADC.
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
    // USART.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    // DMA.
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    // SPI1.
    if(drive_dip_pin_state(DRIVE_DIP_UI_PAD)){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    }
    // SPI2.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    // I2C1.
    if(drive_dip_pin_state(DRIVE_DIP_UI_PAD)){
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    }
    // I2C2.
    if(drive_dip_pin_state(DRIVE_DIP_HS_TEMP)){
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    }
    // TIM1.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);    // Включаем тактирование General-purpose TIM2
    // TIM2.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);    // Включаем тактирование General-purpose TIM2
    // TIM3.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    // Включаем тактирование General-purpose TIM3
    // TIM4.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);    // Включаем тактирование General-purpose TIM4
    // TIM6.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);    // Включаем тактирование Basic TIM6
    // TIM7 - DRIVE_MAIN_TIM.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);    // Включаем тактирование Basic TIM7
    // TIM8.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);    // Включаем тактирование Motor-control TIM8
    // Backup domain + RTC.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
}

//static void init_usart(void)
//{
//    GPIO_InitTypeDef gpio_tx =
//        {.GPIO_Pin = GPIO_Pin_10, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_AF_PP};
//    GPIO_InitTypeDef gpio_rx =
//        {.GPIO_Pin = GPIO_Pin_11, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_IN_FLOATING};
//    GPIO_Init(GPIOC, &gpio_tx);
//    GPIO_Init(GPIOC, &gpio_rx);
//
//    USART_InitTypeDef usart_is =
//        {.USART_BaudRate = 115200, .USART_WordLength = USART_WordLength_8b, .USART_StopBits = USART_StopBits_1,
//         .USART_Parity = USART_Parity_No, .USART_Mode = USART_Mode_Rx | USART_Mode_Tx, .USART_HardwareFlowControl = USART_HardwareFlowControl_None};
//    USART_Init(USART3, &usart_is);
//    USART_Cmd(USART3, ENABLE);
//
//    usart_buf_init_t usartb_is = {.usart = USART3,
//         .write_buffer = usart_write_buffer, .write_buffer_size = USART_WRITE_BUFFER_SIZE,
//         .read_buffer = usart_read_buffer, .read_buffer_size = USART_READ_BUFFER_SIZE};
//    usart_buf_init(&usart_buf, &usartb_is);
//    usart_setup_stdio(&usart_buf);
//
//    NVIC_SetPriority(USART3_IRQn, IRQ_PRIOR_USART);
//    NVIC_EnableIRQ(USART3_IRQn);
//}

static void init_PDR(void)
{
    PWR->CR |= PWR_CR_PLS_2V9;
    PWR->CR |= PWR_CR_PVDE;
}

// Ключи сторожевого таймера.
// Ключ снятия защиты на запись.
#define WDT_UNPROTECT_KEY 0x5555
// Ключ установки зашиты на запись - любое значение.
#define WDT_PROTECT_KEY 0xFFFF
// Ключ перезапуска таймера.
#define WDT_RELOAD_KEY 0xAAAA
// Ключ запуска таймера.
#define WDT_START_KEY 0xCCCC

static void init_watchdog(void)
{
    // Включим внутренний осциллятор 40 кГц.
    RCC->CSR |= RCC_CSR_LSION;
    // Подождём запуска.
    while(!(RCC->CSR & RCC_CSR_LSIRDY));
    
    // Разблокируем защиту на запись.
    IWDG->KR = WDT_UNPROTECT_KEY;
    
    // Подождём сброса бита обновления предделителя.
    while((IWDG->SR & IWDG_SR_PVU));
    // Запишем значение предделителя.
    IWDG->PR = 0x0;
    
    // Подождём сброса бита обновления загружаемого значения.
    while((IWDG->SR & IWDG_SR_RVU));
    // Запишем загружаемое значение.
    IWDG->RLR = 0x64;
    
    // Заблокируем защиту на запись.
    IWDG->KR = WDT_PROTECT_KEY;
    
    // Получаем время счёта таймера 10 мс.
    
    // Запустим таймер.
    IWDG->KR = WDT_START_KEY;
}

static bool check_watchdog_reset(void)
{
    return (RCC->CSR & RCC_CSR_IWDGRSTF) != 0;
}

static void clear_watchdog_reset_flag(void)
{
    RCC->CSR |= RCC_CSR_RMVF;
}

static void reset_watchdog(void)
{
    IWDG->KR = WDT_RELOAD_KEY;
}

/*
static void init_rtc_alarm_exti_line(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_RTC_ALARM_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    
    EXTI_Init(&EXTI_InitStructure);
}
*/

static void init_rtc(void)
{
    rtc_init();
    
    if(rtc_state() == DISABLE){
        PWR_BackupAccessCmd(ENABLE);
        
        RCC_BackupResetCmd(ENABLE);
        __NOP();
        RCC_BackupResetCmd(DISABLE);
        
        PWR_BackupAccessCmd(ENABLE);
        
        RCC_LSEConfig(RCC_LSE_ON);
        while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        RCC_RTCCLKCmd(ENABLE);
        
        rtc_synchronize();
        rtc_setup(0x7fff, NULL);
        
        PWR_BackupAccessCmd(DISABLE);
    }
    
    rtc_set_second_callback(rtc_on_second);
    
    /*init_rtc_alarm_exti_line();
    rtc_set_alarm_callback(rtc_on_alarm);
    rtc_set_alarm_interrupt_enabled(false);*/
    
    NVIC_SetPriority(RTC_IRQn, IRQ_PRIOR_RTC);
    //NVIC_SetPriority(RTCAlarm_IRQn, IRQ_PRIOR_RTC_ALARM);
    NVIC_EnableIRQ(RTC_IRQn);
    //NVIC_EnableIRQ(RTCAlarm_IRQn);
}

static void init_nvdata(void)
{
    drive_nvdata_init();
    
    if(!drive_nvdata_valid()){
        if(drive_nvdata_read() != E_NO_ERROR){
            drive_nvdata_clear();
        }
    }
}

static void init_modbus_usart_bt(void)
{
    GPIO_InitTypeDef gpio_tx =
        {.GPIO_Pin = GPIO_Pin_10, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_AF_PP};
    GPIO_InitTypeDef gpio_rx =
        {.GPIO_Pin = GPIO_Pin_11, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_IPU};
    
    USART_InitTypeDef usart_is =
        {.USART_BaudRate = 38400, .USART_WordLength = USART_WordLength_8b, .USART_StopBits = USART_StopBits_1,
         .USART_Parity = USART_Parity_No, .USART_Mode = USART_Mode_Rx | USART_Mode_Tx, .USART_HardwareFlowControl = USART_HardwareFlowControl_None};
    
    usart_bus_init_t usartb_is = {
        .usart_device = USART3,
        .dma_tx_channel = DMA1_Channel2,
        .dma_rx_channel = DMA1_Channel3
    };
    
    /*param_t* baud_param = settings_param_by_id(PARAM_ID_MODBUS_BAUD);
    if(baud_param){
        usart_is.USART_BaudRate = settings_param_valueu(baud_param);
    }*/
    
    GPIO_Init(GPIOC, &gpio_tx);
    GPIO_Init(GPIOC, &gpio_rx);
    
    USART_Init(USART3, &usart_is);
    USART_Cmd(USART3, ENABLE);
    usart_bus_init(&usart_bus_bt, &usartb_is);
    
    usart_bus_set_rx_callback(&usart_bus_bt, usart_bt_rx_callback);
    usart_bus_set_tx_callback(&usart_bus_bt, usart_bt_tx_callback);
    //usart_bus_set_tc_callback(&usart_bus_bt, usart_bt_tc_callback);
    usart_bus_set_rx_byte_callback(&usart_bus_bt, usart_bt_rx_byte_callback);
    
    usart_bus_set_idle_mode(&usart_bus_bt, USART_IDLE_MODE_END_RX);
    
    NVIC_SetPriority(USART3_IRQn, IRQ_PRIOR_MODBUS_USART);
    NVIC_EnableIRQ(USART3_IRQn);
    
    //NVIC_SetPriority(DMA1_Channel2_IRQn, IRQ_PRIOR_DMA_CH2);
    //NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    //NVIC_SetPriority(DMA1_Channel3_IRQn, IRQ_PRIOR_DMA_CH3);
    //NVIC_EnableIRQ(DMA1_Channel3_IRQn);
}

static void init_modbus_usart_485(void)
{
    GPIO_InitTypeDef gpio_tx =
        {.GPIO_Pin = GPIO_Pin_9, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_AF_PP};
    GPIO_InitTypeDef gpio_rx =
        {.GPIO_Pin = GPIO_Pin_10, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_IN_FLOATING};
    
    USART_InitTypeDef usart_is =
        {.USART_BaudRate = 9600, .USART_WordLength = USART_WordLength_8b, .USART_StopBits = USART_StopBits_1,
         .USART_Parity = USART_Parity_No, .USART_Mode = USART_Mode_Rx | USART_Mode_Tx, .USART_HardwareFlowControl = USART_HardwareFlowControl_None};
    
    usart_bus_init_t usartb_is = {
        .usart_device = USART1,
        .dma_tx_channel = DMA1_Channel4,
        .dma_rx_channel = DMA1_Channel5
    };
    
    param_t* baud_param = settings_param_by_id(PARAM_ID_MODBUS_BAUD);
    if(baud_param){
        usart_is.USART_BaudRate = settings_param_valueu(baud_param);
    }
    
    GPIO_Init(GPIOA, &gpio_tx);
    GPIO_Init(GPIOA, &gpio_rx);
    
    USART_Init(USART1, &usart_is);
    USART_Cmd(USART1, ENABLE);
    usart_bus_init(&usart_bus_485, &usartb_is);
    
    usart_bus_set_rx_callback(&usart_bus_485, usart_485_rx_callback);
    usart_bus_set_tx_callback(&usart_bus_485, usart_485_tx_callback);
    usart_bus_set_tc_callback(&usart_bus_485, usart_485_tc_callback);
    usart_bus_set_rx_byte_callback(&usart_bus_485, usart_485_rx_byte_callback);
    
    usart_bus_set_idle_mode(&usart_bus_485, USART_IDLE_MODE_END_RX);
    
    NVIC_SetPriority(USART1_IRQn, IRQ_PRIOR_MODBUS_USART);
    NVIC_EnableIRQ(USART1_IRQn);
    
    /*NVIC_SetPriority(DMA1_Channel4_IRQn, IRQ_PRIOR_DMA_CH4);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    NVIC_SetPriority(DMA1_Channel5_IRQn, IRQ_PRIOR_DMA_CH5);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);*/
}

static void init_modbus_bt(void)
{
    modbus_rtu_init_t modbus_is;

    modbus_is.usart = &usart_bus_bt;
    modbus_is.mode = MODBUS_RTU_MODE_SLAVE;
    modbus_is.address = 0x1;
    modbus_is.rx_message = &modbus_bt_rx_msg;
    modbus_is.tx_message = &modbus_bt_tx_msg;

    modbus_rtu_init(&modbus_bt, &modbus_is);
    modbus_rtu_set_msg_recv_callback(&modbus_bt, modbus_bt_on_msg_recv);
    modbus_rtu_set_msg_sent_callback(&modbus_bt, modbus_bt_on_msg_sent);
}

static void init_modbus_485(void)
{
    modbus_rtu_init_t modbus_is;
    
    modbus_is.usart = &usart_bus_485;
    modbus_is.mode = MODBUS_RTU_MODE_SLAVE;
    param_t* address_param = settings_param_by_id(PARAM_ID_MODBUS_ADDRESS);
    if(address_param){
        modbus_is.address = settings_param_valueu(address_param);
    }else{
        modbus_is.address = 0xaa;//0x10;
    }
    modbus_is.rx_message = &modbus_485_rx_msg;
    modbus_is.tx_message = &modbus_485_tx_msg;
    
    modbus_rtu_init(&modbus_485, &modbus_is);
    modbus_rtu_set_msg_recv_callback(&modbus_485, modbus_485_on_msg_recv);
    modbus_rtu_set_msg_sent_callback(&modbus_485, modbus_485_on_msg_sent);
    
    modbus_rs485_set_input();
}

static void init_drive_modbus(void)
{
    drive_modbus_init_t drive_modbus_is;
    
    drive_modbus_is.apply_settings_callback = (apply_settings_callback_t)drive_tasks_apply_settings;
    drive_modbus_is.save_settings_callback = (save_settings_callback_t)drive_tasks_save_settings;
    
    drive_modbus_init(&drive_modbus_is);

    drive_modbus_setup(&modbus_485);
    drive_modbus_setup(&modbus_bt);
}

static void init_spi(void)
{
    GPIO_InitTypeDef gpio_sck_mosi =
        {.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_3, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_AF_PP};
    //GPIO_InitTypeDef gpio_miso =
    //    {.GPIO_Pin = GPIO_Pin_4, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_IN_FLOATING};/**/
    
    GPIO_Init(GPIOB, &gpio_sck_mosi);
    //GPIO_Init(GPIOB, &gpio_miso);
    
    SPI_InitTypeDef spi_is;
    SPI_StructInit(&spi_is);
    
    spi_is.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
    spi_is.SPI_CPHA = SPI_CPHA_1Edge;
    spi_is.SPI_CPOL = SPI_CPOL_Low;
    spi_is.SPI_CRCPolynomial = 0;
    spi_is.SPI_DataSize = SPI_DataSize_8b;
    spi_is.SPI_Direction = SPI_Direction_1Line_Tx;
    spi_is.SPI_FirstBit = SPI_FirstBit_MSB;
    spi_is.SPI_Mode = SPI_Mode_Master;
    spi_is.SPI_NSS = SPI_NSS_Soft;
    
    SPI_Init(SPI1, &spi_is);
    SPI_Cmd(SPI1, ENABLE);
    
    spi_bus_init_t spi_bus_is;
    spi_bus_is.spi_device = SPI1;
    spi_bus_is.dma_rx_channel = DMA1_Channel2;
    spi_bus_is.dma_tx_channel = DMA1_Channel3;
    spi_bus_init(&spi, &spi_bus_is);
    
    spi_bus_set_callback(&spi, spi_callback);
    
    NVIC_SetPriority(SPI1_IRQn, IRQ_PRIOR_SPI1);
    NVIC_EnableIRQ(SPI1_IRQn);
    
    NVIC_SetPriority(DMA1_Channel2_IRQn, IRQ_PRIOR_DMA_CH2);
    NVIC_SetPriority(DMA1_Channel3_IRQn, IRQ_PRIOR_DMA_CH3);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
}

static void init_spi2(void)
{
    GPIO_InitTypeDef gpio_sck_mosi =
        {.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_AF_PP};
    GPIO_InitTypeDef gpio_miso =
        {.GPIO_Pin = GPIO_Pin_14, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_IN_FLOATING};
    
    GPIO_Init(GPIOB, &gpio_sck_mosi);
    GPIO_Init(GPIOB, &gpio_miso);
    
    SPI_InitTypeDef spi_is;
    SPI_StructInit(&spi_is);
    
    spi_is.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    spi_is.SPI_CPHA = SPI_CPHA_1Edge;
    spi_is.SPI_CPOL = SPI_CPOL_Low;
    spi_is.SPI_CRCPolynomial = 0;
    spi_is.SPI_DataSize = SPI_DataSize_8b;
    spi_is.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi_is.SPI_FirstBit = SPI_FirstBit_MSB;
    spi_is.SPI_Mode = SPI_Mode_Master;
    spi_is.SPI_NSS = SPI_NSS_Soft;
    
    SPI_Init(SPI2, &spi_is);
    SPI_Cmd(SPI2, ENABLE);
    
    spi_bus_init_t spi_bus_is;
    spi_bus_is.spi_device = SPI2;
    spi_bus_is.dma_rx_channel = DMA1_Channel4;
    spi_bus_is.dma_tx_channel = DMA1_Channel5;
    spi_bus_init(&spi2, &spi_bus_is);
    
    spi_bus_set_callback(&spi2, spi2_callback);
    
    NVIC_SetPriority(SPI2_IRQn, IRQ_PRIOR_SPI2);
    NVIC_EnableIRQ(SPI2_IRQn);
    
    NVIC_SetPriority(DMA1_Channel4_IRQn, IRQ_PRIOR_DMA_CH4);
    NVIC_SetPriority(DMA1_Channel5_IRQn, IRQ_PRIOR_DMA_CH5);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

static void init_eeprom(void)
{
    GPIO_InitTypeDef gpio_ce = 
    {.GPIO_Pin = M95X_CE_PIN, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_Out_PP};
    
    GPIO_Init(M95X_CE_GPIO, &gpio_ce);
    
    m95x_init_t m95x_is;
    m95x_is.page = M95X_PAGE_128; //M95X_PAGE_64
    m95x_is.spi = &spi2;
    m95x_is.transfer_id = M95X_DEFAULT_TRANSFER_ID;
    m95x_is.ce_gpio = M95X_CE_GPIO;
    m95x_is.ce_pin = M95X_CE_PIN;
    
    m95x_init(&eeprom, &m95x_is);
}

static void init_storage(void)
{
    storage_init(&eeprom);
}

static void reset_i2c_slave(GPIO_TypeDef* gpio, uint16_t pin_sda, uint16_t pin_scl)
{
    gpio->BSRR = pin_sda;
    gpio->BRR = pin_scl;
    delay_us(I2C_RESET_CLK_HALF_PERIOD_US);
    
    size_t clocks = 0;
#define I2C_RESET_SLAVE_CLOKS_MAX 9
    
    for(; clocks < I2C_RESET_SLAVE_CLOKS_MAX; clocks ++){
        if(gpio->IDR & pin_sda) break;
        
        gpio->BSRR = pin_scl;
        delay_us(I2C_RESET_CLK_HALF_PERIOD_US);
        gpio->BRR = pin_scl;
        delay_us(I2C_RESET_CLK_HALF_PERIOD_US);
    }
    
#undef I2C_RESET_SLAVE_CLOKS_MAX
    
    gpio->BRR = pin_sda;
    delay_us(I2C_RESET_CLK_HALF_PERIOD_US);
    gpio->BSRR = pin_scl;
    delay_us(I2C_RESET_CLK_HALF_PERIOD_US);
    gpio->BSRR = pin_sda;
}

static void init_i2c1_periph(void)
{
    I2C_InitTypeDef i2c_is;
    i2c_is.I2C_Ack = I2C_Ack_Disable;
    i2c_is.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c_is.I2C_OwnAddress1 = 0x11;
    i2c_is.I2C_ClockSpeed = 10000;
    i2c_is.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c_is.I2C_Mode = I2C_Mode_I2C;
    
    I2C_DeInit(I2C1);
    I2C_Init(I2C1, &i2c_is);
    I2C_StretchClockCmd(I2C1, ENABLE);
    I2C_Cmd(I2C1, ENABLE);
}

static void init_i2c1_gpio(void)
{
    GPIO_InitTypeDef gpio_i2c =
        {.GPIO_Pin = I2C1_PIN_SDA | I2C1_PIN_SCL, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_AF_OD};
    GPIO_Init(I2C1_GPIO, &gpio_i2c);
}

static void reset_i2c1_slave(void)
{
    GPIO_InitTypeDef gpio_i2c =
        {.GPIO_Pin = I2C1_PIN_SDA | I2C1_PIN_SCL, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_Out_OD};
    GPIO_Init(I2C1_GPIO, &gpio_i2c);
    
    reset_i2c_slave(I2C1_GPIO, I2C1_PIN_SDA, I2C1_PIN_SCL);
    
    init_i2c1_gpio();
}

static void reset_i2c1(void)
{
    i2c_device_reset(I2C1);
    i2c_bus_reset(&i2c1);
    
    I2C_Cmd(I2C1, DISABLE);
    
    if(!(I2C1_GPIO->IDR & I2C1_PIN_SDA)){
        reset_i2c1_slave();
    }
    
    init_i2c1_periph();
}

static void init_i2c1(void)
{
    init_i2c1_gpio();
    
    init_i2c1_periph();
    
    i2c_bus_init_t i2c_bus_is;
    i2c_bus_is.i2c_device = I2C1;
    i2c_bus_is.dma_rx_channel = DMA1_Channel7;
    i2c_bus_is.dma_tx_channel = DMA1_Channel6;
    
    i2c_bus_init(&i2c1, &i2c_bus_is);
    
    i2c_bus_set_callback(&i2c1, i2c_callback);
    
    NVIC_SetPriority(I2C1_EV_IRQn, IRQ_PRIOR_I2C1);
    NVIC_SetPriority(I2C1_ER_IRQn, IRQ_PRIOR_I2C1);
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    NVIC_EnableIRQ(I2C1_ER_IRQn);
    
    NVIC_SetPriority(DMA1_Channel6_IRQn, IRQ_PRIOR_DMA_CH6);
    NVIC_SetPriority(DMA1_Channel7_IRQn, IRQ_PRIOR_DMA_CH7);
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

static void init_i2c2_gpio(void)
{
    GPIO_InitTypeDef gpio_i2c =
        {.GPIO_Pin = I2C2_PIN_SDA | I2C2_PIN_SCL, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_AF_OD};
    GPIO_Init(I2C2_GPIO, &gpio_i2c);
}

static void init_i2c2_periph(void)
{
    I2C_InitTypeDef i2c_is;
    i2c_is.I2C_Ack = I2C_Ack_Disable;
    i2c_is.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c_is.I2C_OwnAddress1 = 0x11;
    i2c_is.I2C_ClockSpeed = 10000;
    i2c_is.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c_is.I2C_Mode = I2C_Mode_I2C;
    
    I2C_DeInit(I2C2);
    I2C_Init(I2C2, &i2c_is);
    I2C_StretchClockCmd(I2C2, ENABLE);
    I2C_Cmd(I2C2, ENABLE);
}

static void reset_i2c2_slave(void)
{
    GPIO_InitTypeDef gpio_i2c =
        {.GPIO_Pin = I2C2_PIN_SDA | I2C2_PIN_SCL, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_Out_OD};
    GPIO_Init(I2C2_GPIO, &gpio_i2c);
    
    reset_i2c_slave(I2C2_GPIO, I2C2_PIN_SDA, I2C2_PIN_SCL);
    
    init_i2c2_gpio();
}

static void reset_i2c2(void)
{
    i2c_device_reset(I2C2);
    i2c_bus_reset(&i2c2);
    
    I2C_Cmd(I2C2, DISABLE);
    
    if(!(I2C2_GPIO->IDR & I2C2_PIN_SDA)){
        reset_i2c2_slave();
    }
    
    init_i2c2_periph();
}

static void init_i2c2(void)
{
    init_i2c2_gpio();
    
    init_i2c2_periph();
    
    i2c_bus_init_t i2c_bus_is;
    i2c_bus_is.i2c_device = I2C2;
    i2c_bus_is.dma_rx_channel = DMA1_Channel5;
    i2c_bus_is.dma_tx_channel = DMA1_Channel4;
    
    i2c_bus_init(&i2c2, &i2c_bus_is);
    
    i2c_bus_set_callback(&i2c2, i2c2_callback);
    
    NVIC_SetPriority(I2C2_EV_IRQn, IRQ_PRIOR_I2C2);
    NVIC_SetPriority(I2C2_ER_IRQn, IRQ_PRIOR_I2C2);
    NVIC_EnableIRQ(I2C2_EV_IRQn);
    NVIC_EnableIRQ(I2C2_ER_IRQn);
    
    NVIC_SetPriority(DMA1_Channel4_IRQn, IRQ_PRIOR_DMA_CH4);
    NVIC_SetPriority(DMA1_Channel5_IRQn, IRQ_PRIOR_DMA_CH5);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

static void init_adc(void)
{
    GPIO_InitTypeDef gpio_adcA =
        {.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2, .GPIO_Speed = GPIO_Speed_2MHz, .GPIO_Mode = GPIO_Mode_AIN};
    GPIO_InitTypeDef gpio_adcB =
        {.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1, .GPIO_Speed = GPIO_Speed_2MHz, .GPIO_Mode = GPIO_Mode_AIN};
    GPIO_InitTypeDef gpio_adcC =
        {.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5, .GPIO_Speed = GPIO_Speed_2MHz, .GPIO_Mode = GPIO_Mode_AIN};
    GPIO_Init(GPIOA, &gpio_adcA);
    GPIO_Init(GPIOB, &gpio_adcB);
    GPIO_Init(GPIOC, &gpio_adcC);
    
    dma_channel_lock(DMA1_Channel1);
    dma_channel_lock(DMA2_Channel5);
    
    DMA_DeInit(DMA1_Channel1);
    DMA_DeInit(DMA2_Channel5);

    DMA_InitTypeDef dma_is;
    DMA_StructInit(&dma_is);
    dma_is.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
    dma_is.DMA_MemoryBaseAddr = (uint32_t)adc12_raw_buffer;
    dma_is.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma_is.DMA_BufferSize = ADC12_RAW_BUFFER_DMA_TRANSFERS;
    dma_is.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_is.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_is.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
    dma_is.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
    dma_is.DMA_Mode = DMA_Mode_Circular;
    dma_is.DMA_Priority = DMA_Priority_Medium;
    dma_is.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA1_Channel1, &dma_is);

    DMA_StructInit(&dma_is);
    dma_is.DMA_PeripheralBaseAddr = (uint32_t)&ADC3->DR;
    dma_is.DMA_MemoryBaseAddr = (uint32_t)adc3_raw_buffer;
    dma_is.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma_is.DMA_BufferSize = ADC3_RAW_BUFFER_DMA_TRANSFERS;
    dma_is.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_is.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_is.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma_is.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma_is.DMA_Mode = DMA_Mode_Circular;
    dma_is.DMA_Priority = DMA_Priority_Medium;
    dma_is.DMA_M2M = DMA_M2M_Disable;

    DMA_Init(DMA2_Channel5, &dma_is);
        
    ADC_DeInit(ADC1);
    ADC_DeInit(ADC2);
    ADC_DeInit(ADC3);
    
    ADC_InitTypeDef adc_is;
    ADC_StructInit(&adc_is);
    //Настройка параметров  ADC1
    adc_is.ADC_Mode = ADC_Mode_RegSimult;
    adc_is.ADC_ScanConvMode = ENABLE;
    adc_is.ADC_ContinuousConvMode = DISABLE;
    adc_is.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;
    adc_is.ADC_DataAlign = ADC_DataAlign_Right;
    adc_is.ADC_NbrOfChannel = ADC12_CHANNELS_COUNT;
    
    ADC_Init(ADC1, &adc_is);
    
    ADC_StructInit(&adc_is);
    //Настройка параметров  ADC2
    adc_is.ADC_Mode = ADC_Mode_RegSimult;
    adc_is.ADC_ScanConvMode = ENABLE;
    adc_is.ADC_ContinuousConvMode = DISABLE;
    adc_is.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    adc_is.ADC_DataAlign = ADC_DataAlign_Right;
    adc_is.ADC_NbrOfChannel = ADC12_CHANNELS_COUNT;
    
    ADC_Init(ADC2, &adc_is);
    
    ADC_StructInit(&adc_is);
    //Настройка параметров  ADC3
    adc_is.ADC_Mode = ADC_Mode_Independent;
    adc_is.ADC_ScanConvMode = ENABLE;
    adc_is.ADC_ContinuousConvMode = DISABLE;
    adc_is.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;
    adc_is.ADC_DataAlign = ADC_DataAlign_Right;
    adc_is.ADC_NbrOfChannel = ADC3_CHANNELS_COUNT;
    
    ADC_Init(ADC3, &adc_is);
    
    
#define ADC_SAMPLE_TIME ADC_SampleTime_1Cycles5
    
    //Порядок оцифровки ADC1.
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 1, ADC_SAMPLE_TIME);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 2, ADC_SAMPLE_TIME);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 3, ADC_SAMPLE_TIME);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 4, ADC_SAMPLE_TIME);
    
    //Порядок оцифровки ADC2.
    ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 1, ADC_SAMPLE_TIME);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_13, 2, ADC_SAMPLE_TIME);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 3, ADC_SAMPLE_TIME);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 4, ADC_SAMPLE_TIME);
    
    //Порядок оцифровки ADC3.
    ADC_RegularChannelConfig(ADC3, ADC_Channel_1, 1, ADC_SAMPLE_TIME);
    ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 2, ADC_SAMPLE_TIME);
    ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 3, ADC_SAMPLE_TIME);
    
#undef ADC_SAMPLE_TIME
    
    ADC_DMACmd(ADC1, ENABLE); //Enable ADC1 DMA.
    DMA_Cmd(DMA1_Channel1, ENABLE);
    
    ADC_DMACmd(ADC3, ENABLE); //Enable ADC3 DMA.
    DMA_Cmd(DMA2_Channel5, ENABLE);
    
    ADC_DiscModeChannelCountConfig(ADC1, ADC12_RAW_BUFFER_DMA_TRANSFERS);
    ADC_DiscModeCmd(ADC1, ENABLE);
    
    ADC_DiscModeChannelCountConfig(ADC2, ADC12_RAW_BUFFER_DMA_TRANSFERS);
    ADC_DiscModeCmd(ADC2, ENABLE);
    
    ADC_DiscModeChannelCountConfig(ADC3, ADC3_RAW_BUFFER_DMA_TRANSFERS);
    ADC_DiscModeCmd(ADC3, ENABLE);
    
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);
    ADC_ExternalTrigConvCmd(ADC2, ENABLE);
    ADC_ExternalTrigConvCmd(ADC3, ENABLE);

    ADC_Cmd(ADC1, ENABLE); //Enable ADC1.
    ADC_Cmd(ADC2, ENABLE); //Enable ADC2.
    ADC_Cmd(ADC3, ENABLE); //Enable ADC2.
    
    //Калибровка ADC.
    //Enable reset calibaration register.
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1)); //Check the end of ADC1 reset calibration register.
    ADC_ResetCalibration(ADC2);
    while (ADC_GetResetCalibrationStatus(ADC2)); //Check the end of ADC2 reset calibration register.
    ADC_ResetCalibration(ADC3);
    while (ADC_GetResetCalibrationStatus(ADC3)); //Check the end of ADC3 reset calibration register.
    //Start calibaration.
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1)); //Check the end of ADC1 calibration.
    ADC_StartCalibration(ADC2);
    while (ADC_GetCalibrationStatus(ADC2)); //Check the end of ADC2 calibration.
    ADC_StartCalibration(ADC3);
    while (ADC_GetCalibrationStatus(ADC3)); //Check the end of ADC3 calibration.
    
    DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
    NVIC_SetPriority(DMA1_Channel1_IRQn, IRQ_PRIOR_ADC_DMA);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

#define ADC_TIM_PRESCALER (1)
#define ADC_TIM_PERIOD (72000000UL / ADC_TIM_PRESCALER / POWER_ADC_FREQ)

static void init_adc_timer(void)
{
    TIM_DeInit(ADC_TIM);
    TIM_TimeBaseInitTypeDef tim1_is;
    TIM_TimeBaseStructInit(&tim1_is);
            tim1_is.TIM_Prescaler = ADC_TIM_PRESCALER-1;                    // Делитель (0000...FFFF)
            tim1_is.TIM_CounterMode = TIM_CounterMode_Up;    // Режим счетчика
            tim1_is.TIM_Period = ADC_TIM_PERIOD-1;                     // Значение периода (0000...FFFF)
            tim1_is.TIM_ClockDivision = 0;                   // определяет тактовое деление
    TIM_TimeBaseInit(ADC_TIM, &tim1_is);

    TIM_OCInitTypeDef tim1_oc_is;
    TIM_OCStructInit(&tim1_oc_is);
        tim1_oc_is.TIM_OCMode = TIM_OCMode_PWM1;
        tim1_oc_is.TIM_OutputState = TIM_OutputState_Disable;
        tim1_oc_is.TIM_OutputNState = TIM_OutputNState_Disable;
        tim1_oc_is.TIM_Pulse = 1; // Минимальное значение.
        tim1_oc_is.TIM_OCPolarity = TIM_OCPolarity_Low;
        tim1_oc_is.TIM_OCNPolarity = TIM_OCNPolarity_Low;
        tim1_oc_is.TIM_OCIdleState = TIM_OCIdleState_Reset;
        tim1_oc_is.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    TIM_OC3Init(ADC_TIM, &tim1_oc_is);
    TIM_CCxCmd (ADC_TIM, TIM_Channel_3, TIM_CCx_Enable);
    
    //TIM_ARRPreloadConfig(ADC_TIM, DISABLE);
    
    TIM_Cmd(ADC_TIM, ENABLE);                                                      // Начать отсчёт!
    TIM_CtrlPWMOutputs(ADC_TIM, ENABLE);
    
    //NVIC_SetPriority(TIM1_UP_IRQn, IRQ_PRIOR_ADC_TIM);
    //NVIC_EnableIRQ(TIM1_UP_IRQn);
}

static void adc_timer_set_rate(uint32_t rate)
{
    ADC_TIM->CR1 &= ~TIM_CR1_CEN;
    
    ADC_TIM->ARR = (ADC_TIM_PERIOD / rate) - 1;
    
    if(ADC_TIM->CNT > ADC_TIM->ARR){
        ADC_TIM->CNT = ADC_TIM->ARR - 1;
    }
    
    ADC_TIM->CR1 |= TIM_CR1_CEN;
}

/*static void adc_stop(void)
{
    ADC1->CR2 &= ~ADC_CR2_ADON;
    ADC2->CR2 &= ~ADC_CR2_ADON;
    ADC3->CR2 &= ~ADC_CR2_ADON;
}

static void adc_start(void)
{
    ADC1->CR2 |= ADC_CR2_ADON;
    ADC2->CR2 |= ADC_CR2_ADON;
    ADC3->CR2 |= ADC_CR2_ADON;
}*/

static void adc_dma_set_rate(uint32_t rate)
{
    DMA1_Channel1->CCR &= ~DMA_CCR1_EN;
    DMA2_Channel5->CCR &= ~DMA_CCR1_EN;
    
    DMA1_Channel1->CNDTR = ADC12_RAW_BUFFER_DMA_TRANSFERS * rate;
    DMA2_Channel5->CNDTR = ADC3_RAW_BUFFER_DMA_TRANSFERS * rate;
    
    DMA1_Channel1->CMAR = (uint32_t)adc12_raw_buffer;
    DMA2_Channel5->CMAR = (uint32_t)adc3_raw_buffer;
    
    DMA1_Channel1->CCR |= DMA_CCR1_EN;
    DMA2_Channel5->CCR |= DMA_CCR1_EN;
}

static void adc_set_rate_impl(uint32_t rate)
{
    adc_dma_set_rate(rate);
    adc_timer_set_rate(rate);
}

static void adc_set_rate(uint32_t rate)
{
    if(rate >= ADC_FREQ_MULT_MIN && rate <= ADC_FREQ_MULT_MAX){
        adc_new_rate = rate;
        
        adc_change_rate = true;
        
        //ADC_TIM->SR = ~TIM_SR_UIF;
        //ADC_TIM->DIER |= TIM_DIER_UIE;
    }
}

#undef ADC_TIM_PERIOD
#undef ADC_TIM_PRESCALER

static void init_ioport_simple(void)
{
    pca9555_init(&ioport, &i2c1, PCA9555_I2C_DEFAULT_ADDRESS);
}

static void init_tft_simple(void)
{
    GPIO_InitTypeDef gpio_dc = 
    {.GPIO_Pin = TFT_DC_PIN, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_Out_PP};
    GPIO_InitTypeDef gpio_ce = 
    {.GPIO_Pin = TFT_CE_PIN, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_Out_PP};
    //GPIO_InitTypeDef gpio_rst = 
    //{.GPIO_Pin = TFT_RST_PIN, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_Out_PP};
    
    GPIO_Init(TFT_DC_GPIO, &gpio_dc);
    GPIO_Init(TFT_CE_GPIO, &gpio_ce);
    //GPIO_Init(TFT_RST_GPIO, &gpio_rst);
    
    tft9341_init_t tft_init;
    
    tft_init.ce_gpio = TFT_CE_GPIO; tft_init.ce_pin = TFT_CE_PIN;
    tft_init.dc_gpio = TFT_DC_GPIO; tft_init.dc_pin = TFT_DC_PIN;
    tft_init.reset_gpio = TFT_RST_GPIO; tft_init.reset_pin = TFT_RST_PIN;
    tft_init.spi = &spi;
    tft_init.transfer_id = TFT9341_DEFAULT_TRANSFER_ID;
    
    tft9341_init(&tft, &tft_init);
}

static void on_drive_reset(void);
static void init_drive(void)
{
    drive_init();
    drive_set_error_callback((drive_error_callback_t)drive_tasks_write_error_event);
    drive_set_warning_callback((drive_warning_callback_t)drive_tasks_write_warning_event);
    drive_set_reset_callback(on_drive_reset);
    drive_set_adc_rate_proc(adc_set_rate);
}

static void reset_ioport(void)
{
    pca9555_timeout(&ioport);
    reset_i2c1();
    
    // pca9555 resets counter.
#ifdef DEBUG_RESET_I2C
    param_t* p = settings_param_by_id(PARAM_ID_DEBUG_2);
    if(p){
        unsigned int rst_cnt = settings_param_valueu(p);
        settings_param_set_valueu(p, rst_cnt + 1);
    }
#endif
}

/******************************************************************************/

static void triacs_pairs_timer_init(TIM_TypeDef* TIM)
{
    TIM_DeInit(TIM);
    TIM_TimeBaseInitTypeDef tim_is;
    TIM_TimeBaseStructInit(&tim_is);
            tim_is.TIM_Prescaler = TRIACS_TIM_PRESCALER;   // Делитель (0000...FFFF)
            tim_is.TIM_CounterMode = TIM_CounterMode_Up;   // Режим счетчика
            tim_is.TIM_Period = TRIACS_TIM_PERIOD;         // Значение периода (0000...FFFF)
            tim_is.TIM_ClockDivision = TIM_CKD_DIV1;       // определяет тактовое деление
    TIM_TimeBaseInit(TIM, &tim_is);
    TIM_SetCounter(TIM, 0);
    TIM_SelectOnePulseMode(TIM, TIM_OPMode_Single);        // Однопульсный режим таймера
    
    TIM_OCInitTypeDef tim_oc_is;
    TIM_OCStructInit(&tim_oc_is);
        tim_oc_is.TIM_OCMode = TIM_OCMode_Timing;
        tim_oc_is.TIM_OutputState = TIM_OutputState_Disable;
        tim_oc_is.TIM_OutputNState = TIM_OutputNState_Disable;
        tim_oc_is.TIM_Pulse = 0;
        tim_oc_is.TIM_OCPolarity = TIM_OCPolarity_Low;
        tim_oc_is.TIM_OCNPolarity = TIM_OCNPolarity_Low;
        tim_oc_is.TIM_OCIdleState = TIM_OCIdleState_Reset;
        tim_oc_is.TIM_OCNIdleState = TIM_OCNIdleState_Reset ;
    TIM_OC1Init(TIM, &tim_oc_is);
    TIM_OC2Init(TIM, &tim_oc_is);
    //TIM_OC3Init(TIM, &tim_oc_is);
    //TIM_OC4Init(TIM, &tim_oc_is);
    
    TIM_CCxCmd (TIM, TRIACS_A_OPEN_CHANNEL,  TIM_CCx_Enable);
    TIM_CCxCmd (TIM, TRIACS_A_CLOSE_CHANNEL, TIM_CCx_Enable);
    //TIM_CCxCmd (TIM, TRIACS_B_OPEN_CHANNEL,  TIM_CCx_Enable);
    //TIM_CCxCmd (TIM, TRIACS_B_CLOSE_CHANNEL, TIM_CCx_Enable);
}

static void triac_exc_timer_init(TIM_TypeDef* TIM)
{
    TIM_DeInit(TIM);
    TIM_TimeBaseInitTypeDef tim_is;
    TIM_TimeBaseStructInit(&tim_is);
            tim_is.TIM_Prescaler = TRIAC_EXC_TIM_PRESCALER; // Делитель (0000...FFFF)
            tim_is.TIM_CounterMode = TIM_CounterMode_Up;    // Режим счетчика
            tim_is.TIM_Period = TRIAC_EXC_TIM_PERIOD;       // Значение периода (0000...FFFF)
            tim_is.TIM_ClockDivision = TIM_CKD_DIV1;        // определяет тактовое деление
    TIM_TimeBaseInit(TIM, &tim_is);
    TIM_SetCounter(TIM, 0);
    TIM_SelectOnePulseMode(TIM, TIM_OPMode_Single);        // Однопульсный режим таймера
    
    TIM_OCInitTypeDef tim_oc_is;
    TIM_OCStructInit(&tim_oc_is);
        tim_oc_is.TIM_OCMode = TIM_OCMode_Timing;
        tim_oc_is.TIM_OutputState = TIM_OutputState_Disable;
        tim_oc_is.TIM_OutputNState = TIM_OutputNState_Disable;
        tim_oc_is.TIM_Pulse = 0;
        tim_oc_is.TIM_OCPolarity = TIM_OCPolarity_Low;
        tim_oc_is.TIM_OCNPolarity = TIM_OCNPolarity_Low;
        tim_oc_is.TIM_OCIdleState = TIM_OCIdleState_Reset;
        tim_oc_is.TIM_OCNIdleState = TIM_OCNIdleState_Reset ;
    TIM_OC1Init(TIM, &tim_oc_is);
    TIM_OC2Init(TIM, &tim_oc_is);
    TIM_OC3Init(TIM, &tim_oc_is);
    TIM_OC4Init(TIM, &tim_oc_is);
    
    TIM_CCxCmd (TIM, TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL,   TIM_CCx_Enable);
    TIM_CCxCmd (TIM, TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL,  TIM_CCx_Enable);
    TIM_CCxCmd (TIM, TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL,  TIM_CCx_Enable);
    TIM_CCxCmd (TIM, TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL, TIM_CCx_Enable);
    
    //TIM_ITConfig(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL_IT,   ENABLE); // Разрешаем прерывание OC 1 от таймера
    //TIM_ITConfig(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL_IT,  ENABLE); // Разрешаем прерывание OC 2 от таймера
    //TIM_ITConfig(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL_IT,  ENABLE); // Разрешаем прерывание OC 3 от таймера
    //TIM_ITConfig(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL_IT, ENABLE); // Разрешаем прерывание OC 4 от таймера
}

#if configGENERATE_RUN_TIME_STATS == 1

//static uint32_t hiresTimerValue = 0;

#endif //configGENERATE_RUN_TIME_STATS

IRQ_ATTRIBS void TIM6_IRQHandler(void)
{
    drive_hires_timer_irq_handler();
    
#if configGENERATE_RUN_TIME_STATS == 1

    //hiresTimerValue ++;

#endif //configGENERATE_RUN_TIME_STATS
}

static void init_hires_timer(void)
{
    drive_hires_timer_init(TIM6);
    drive_hires_timer_irq_set_enabled(true);
    
    drive_hires_timer_start();

    NVIC_SetPriority(TIM6_IRQn, IRQ_PRIOR_HIRES_TIMER);
    NVIC_EnableIRQ(TIM6_IRQn);
}

static void init_drive_main_timer(void)
{
    TIM_DeInit(DRIVE_MAIN_TIM);
    TIM_TimeBaseInitTypeDef tim_is;
            tim_is.TIM_Prescaler = DRIVE_MAIN_TIMER_CNT_PRESCALER; // Настраиваем делитель чтобы таймер тикал 1 000 000 раз в секунду
            tim_is.TIM_CounterMode = TIM_CounterMode_Up;    // Режим счетчика
            tim_is.TIM_Period = DRIVE_MAIN_TIMER_CNT_PERIOD; // Значение периода (0000...FFFF)
            tim_is.TIM_ClockDivision = TIM_CKD_DIV1;        // определяет тактовое деление
    TIM_TimeBaseInit(DRIVE_MAIN_TIM, &tim_is);
    TIM_SelectOnePulseMode(DRIVE_MAIN_TIM, TIM_OPMode_Repetitive);                // Повторяющийся режим таймера
    TIM_SetCounter(DRIVE_MAIN_TIM, 0);                                            // Сбрасываем счетный регистр в ноль
    
    drive_set_null_timer(DRIVE_MAIN_TIM);
    
    TIM_ITConfig(DRIVE_MAIN_TIM, TIM_IT_Update, ENABLE);                        // Разрешаем прерывание от таймера
    TIM_Cmd(DRIVE_MAIN_TIM, ENABLE);                                            // Начать отсчёт!
    
    NVIC_SetPriority(TIM7_IRQn, IRQ_PRIOR_NULL_TIMER);
    NVIC_EnableIRQ(TIM7_IRQn);
}


static void init_triacs(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    /* GPIOB Configuration: 57 (PD10 - VS_1); 58 (PD11 - VS_2); 59 (PD12 - VS_3); as output open drain
     *                      60 (PD13 - VS_4); 61 (PD14 - VS_5); 62 (PD15 - VS_6); as output open drain */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* GPIOB Configuration: 63 (PC6 - VS_xS) as output open drain */
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    drive_set_triac_pair_gpio(TRIAC_PAIR_3_6, GPIOD, GPIO_Pin_12, GPIOD, GPIO_Pin_15);
    drive_set_triac_pair_gpio(TRIAC_PAIR_3_2, GPIOD, GPIO_Pin_12, GPIOD, GPIO_Pin_11);
    drive_set_triac_pair_gpio(TRIAC_PAIR_5_2, GPIOD, GPIO_Pin_14, GPIOD, GPIO_Pin_11);
    drive_set_triac_pair_gpio(TRIAC_PAIR_5_4, GPIOD, GPIO_Pin_14, GPIOD, GPIO_Pin_13);
    drive_set_triac_pair_gpio(TRIAC_PAIR_1_4, GPIOD, GPIO_Pin_10, GPIOD, GPIO_Pin_13);
    drive_set_triac_pair_gpio(TRIAC_PAIR_1_6, GPIOD, GPIO_Pin_10, GPIOD, GPIO_Pin_15);
    
    drive_set_triac_exc_gpio(GPIOC, GPIO_Pin_6);
}

static void init_triacs_timers(void)
{
    triacs_pairs_timer_init(TIM2);
    triacs_pairs_timer_init(TIM3);
    triac_exc_timer_init(TIM4);
    
    drive_set_triacs_pairs_timer(TRIACS_TIMER_0, TIM2);
    drive_set_triacs_pairs_timer(TRIACS_TIMER_1, TIM3);
    drive_set_triac_exc_timer(TIM4);
    
    NVIC_SetPriority(TIM2_IRQn, IRQ_PRIOR_TRIACS_TIMER);
    NVIC_EnableIRQ (TIM2_IRQn);         // Разрешаем прерывания по Таймеру2
    NVIC_SetPriority(TIM3_IRQn, IRQ_PRIOR_TRIACS_TIMER);
    NVIC_EnableIRQ (TIM3_IRQn);         // Разрешаем прерывания по Таймеру3
    NVIC_SetPriority(TIM4_IRQn, IRQ_PRIOR_TRIAC_EXC_TIMER);
    NVIC_EnableIRQ (TIM4_IRQn);         // Разрешаем прерывания по Таймеру4
}

static void init_exti_pca9555(void)
{
    /*
     * PCA9555 Interrupt.
     */
    GPIO_InitTypeDef GPIO_InitStructure;
    /* GPIOB Configuration: 95 (PD7 - Int_PCA9555) as input pull-down */
        GPIO_InitStructure.GPIO_Pin = EXTI_PCA9555_Pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(EXTI_PCA9555_GPIO, &GPIO_InitStructure);
    
    /*
     * Привязка Прерывания к портам МК.
     */       
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource7);
    
    
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_PCA9555_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_SetPriority(EXTI9_5_IRQn, IRQ_PRIOR_KEYPAD);
 
    NVIC_EnableIRQ (EXTI9_5_IRQn);       // Разрешаем прерывание от 5-9 ног
}

static void init_gpio (void)
{
    GPIO_InitTypeDef gpio_is;
    /*
     * Dip 8 switch
     */
    /* GPIOB Configuration: 37 (PB2 - Dip_1) as input floating */
        gpio_is.GPIO_Pin = GPIO_Pin_2;
        gpio_is.GPIO_Speed = GPIO_Speed_2MHz;
        gpio_is.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &gpio_is);

    /* GPIOE Configuration: 38 (PE7 - Dip_2);  39 (PE8 - Dip_3);  40 (PE9 - Dip_4);
     *                      41 (PE10 - Dip_5); 42 (PE11 - Dip_6); 43 (PE12 - Dip_7)  as input floating */
        gpio_is.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
        gpio_is.GPIO_Speed = GPIO_Speed_2MHz;
        gpio_is.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOE, &gpio_is);

    /* GPIOC Configuration: 65 (PC8 - Dip_8) as input floating */
        gpio_is.GPIO_Pin = GPIO_Pin_8;
        gpio_is.GPIO_Speed = GPIO_Speed_2MHz;
        gpio_is.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &gpio_is);
    
    /* GPIOC Configuration: 66 (PC9 - RS485 iface ctl) as output pp */
        gpio_is.GPIO_Pin = RS485_IFACE_CTL_PIN;
        gpio_is.GPIO_Speed = GPIO_Speed_2MHz;
        gpio_is.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(RS485_IFACE_CTL_GPIO, &gpio_is);
}

static void init_fan_pwm(void)
{
    GPIO_InitTypeDef gpio_is;
    /* GPIOC Configuration: 64 (PC7 - FAN) as output af pp */
        gpio_is.GPIO_Pin = GPIO_Pin_7;
        gpio_is.GPIO_Speed = GPIO_Speed_2MHz;
        gpio_is.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &gpio_is);
    //GPIO_SetBits(GPIOC, GPIO_Pin_7); // PWM: TIM8_CH2.
    
    
    TIM_TimeBaseInitTypeDef tim_is;
    tim_is.TIM_ClockDivision = TIM_CKD_DIV1;
    tim_is.TIM_CounterMode = TIM_CounterMode_Up;
    tim_is.TIM_RepetitionCounter = 0;
    tim_is.TIM_Prescaler = 12 - 1; // 6 kHz.
    tim_is.TIM_Period = HEATSINK_FAN_PWM_MAX;
    
    TIM_TimeBaseInit(HEATSINK_FAN_PWM_TIM, &tim_is);
    
    TIM_OCInitTypeDef pwm_is;
    pwm_is.TIM_OCIdleState = TIM_OCIdleState_Reset;
    pwm_is.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
    pwm_is.TIM_OCPolarity = TIM_OCPolarity_High;
    pwm_is.TIM_OCNPolarity = TIM_OCNPolarity_Low;
    pwm_is.TIM_OutputState = TIM_OutputState_Enable;
    pwm_is.TIM_OutputNState = TIM_OutputNState_Disable;
    pwm_is.TIM_OCMode = TIM_OCMode_PWM1;
    pwm_is.TIM_Pulse = 0;
    
    TIM_OC2Init(HEATSINK_FAN_PWM_TIM, &pwm_is);
    TIM_CCxCmd(HEATSINK_FAN_PWM_TIM, TIM_Channel_2, TIM_CCx_Enable);
    
    TIM_OC2PreloadConfig(HEATSINK_FAN_PWM_TIM, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(HEATSINK_FAN_PWM_TIM, ENABLE);
    
    TIM_Cmd(HEATSINK_FAN_PWM_TIM, ENABLE);
    
    TIM_CtrlPWMOutputs(HEATSINK_FAN_PWM_TIM, ENABLE);
    
    //TIM_SetCompare2(HEATSINK_FAN_PWM_TIM, HEATSINK_FAN_PWM_MAX / 3);
}

static void init_dio_in_pin(GPIO_TypeDef* GPIO, uint16_t pin)
{
    GPIO_InitTypeDef gpio_is;
    
    gpio_is.GPIO_Mode = DIGITAL_IO_IN_MODE;
    gpio_is.GPIO_Speed = DIGITAL_IO_SPEED;
    gpio_is.GPIO_Pin = pin;
    
    GPIO_Init(GPIO, &gpio_is);
}

static void init_dio_out_pin(GPIO_TypeDef* GPIO, uint16_t pin)
{
    GPIO_InitTypeDef gpio_is;
    
    gpio_is.GPIO_Mode = DIGITAL_IO_OUT_MODE;
    gpio_is.GPIO_Speed = DIGITAL_IO_SPEED;
    gpio_is.GPIO_Pin = pin;
    
    GPIO_Init(GPIO, &gpio_is);
}

static void init_dio_in(drive_dio_input_t input, GPIO_TypeDef* GPIO, uint16_t pin)
{
    init_dio_in_pin(GPIO, pin);
    drive_set_dio_input_gpio(input, GPIO, pin);
}

static void init_dio_out(drive_dio_output_t output, GPIO_TypeDef* GPIO, uint16_t pin)
{
    init_dio_out_pin(GPIO, pin);
    drive_set_dio_output_gpio(output, GPIO, pin);
}

static void init_dio(void)
{
    init_dio_in(DRIVE_DIO_INPUT_1, DIGITAL_IO_IN_1_GPIO, DIGITAL_IO_IN_1_PIN);
    init_dio_in(DRIVE_DIO_INPUT_2, DIGITAL_IO_IN_2_GPIO, DIGITAL_IO_IN_2_PIN);
    init_dio_in(DRIVE_DIO_INPUT_3, DIGITAL_IO_IN_3_GPIO, DIGITAL_IO_IN_3_PIN);
    init_dio_in(DRIVE_DIO_INPUT_4, DIGITAL_IO_IN_4_GPIO, DIGITAL_IO_IN_4_PIN);
    init_dio_in(DRIVE_DIO_INPUT_5, DIGITAL_IO_IN_5_GPIO, DIGITAL_IO_IN_5_PIN);
    
    init_dio_out(DRIVE_DIO_OUTPUT_1, DIGITAL_IO_OUT_1_GPIO, DIGITAL_IO_OUT_1_PIN);
    init_dio_out(DRIVE_DIO_OUTPUT_2, DIGITAL_IO_OUT_2_GPIO, DIGITAL_IO_OUT_2_PIN);
    init_dio_out(DRIVE_DIO_OUTPUT_3, DIGITAL_IO_OUT_3_GPIO, DIGITAL_IO_OUT_3_PIN);
    init_dio_out(DRIVE_DIO_OUTPUT_4, DIGITAL_IO_OUT_4_GPIO, DIGITAL_IO_OUT_4_PIN);
}

static void init_heatsink_sensor(void)
{
    lm75_init(&heatsink_sensor, &i2c2, HEATSINK_SENSOR_ADDRESS);
}

static void reset_heatsink_sensor(void)
{
    lm75_timeout(&heatsink_sensor);
    reset_i2c2();
    
    // LM75 resets counter.
#ifdef DEBUG_RESET_I2C
    param_t* p = settings_param_by_id(PARAM_ID_DEBUG_3);
    if(p){
        unsigned int rst_cnt = settings_param_valueu(p);
        settings_param_set_valueu(p, rst_cnt + 1);
    }
#endif
}

static void fan_pwm_set_value(uint32_t rpm_percents)
{
    if(rpm_percents == 0){
        TIM_SetCompare2(HEATSINK_FAN_PWM_TIM, 0);
        return;
    }
    
    rpm_percents = rpm_percents * HEATSINK_FAN_PWM_MAX / 100;
    
    rpm_percents = CLAMP(rpm_percents, 0, HEATSINK_FAN_PWM_MAX);
    
    TIM_SetCompare2(HEATSINK_FAN_PWM_TIM, rpm_percents);
}

static void init_drive_ui(void)
{
    init_ioport_simple();
    init_tft_simple();
    
    drive_ui_init_t ui_is;
    
    struct timeval timeout = {0, IOPORT_IO_TIMEOUT_US};
    
    ui_is.ioport = &ioport;
    ui_is.tft = &tft;
    ui_is.ioport_timeout = &timeout;
    ui_is.reset_i2c_bus_proc = reset_ioport;
    ui_is.ioport_i2c_watchdog = WATCHDOG_I2C1;
    
    drive_ui_init(&ui_is);
}

static void init_drive_temp(void)
{
    drive_temp_init_t temp_is;
    
    struct timeval temp_interval = { DRIVE_TEMP_UPDATE_PERIOD_S, 0 };
    struct timeval io_timeout = {0, HEATSINK_SENSOR_IO_TIMEOUT_US};
    struct timeval timeout = {HEATSINK_SENSOR_TIMEOUT_S, 0};
    
    memset(&temp_is, 0x0, sizeof(drive_temp_init_t));

    if(drive_dip_pin_state(DRIVE_DIP_HS_TEMP)){
        temp_is.heatsink_sensor = &heatsink_sensor;
        temp_is.heatsink_sensor_reset_proc = reset_heatsink_sensor;
        temp_is.heatsink_sensor_io_timeout = &io_timeout;
        temp_is.heatsink_sensor_timeout = &timeout;
        temp_is.heatsink_sensor_i2c_watchdog = WATCHDOG_I2C2;
    }
    temp_is.update_interval = &temp_interval;
    temp_is.set_fan_rpm_proc = fan_pwm_set_value;
    
    drive_temp_init(&temp_is);
}

static void on_i2c_watchdog_timeout(uint32_t i2c_number)
{
    // unlock rs485 dma channels.
    dma_channel_unlock(DMA1_Channel4);
    dma_channel_unlock(DMA1_Channel5);
    
    switch(i2c_number){
        case WATCHDOG_I2C1:
            reset_ioport();
            break;
        case WATCHDOG_I2C2:
            reset_heatsink_sensor();
            break;
        default:
            reset_ioport();
            reset_heatsink_sensor();
            break;
    }
}

static void setup_i2c_watchdogs(void)
{
    if(drive_dip_pin_state(DRIVE_DIP_UI_PAD)){
        drive_task_i2c_watchdog_set_reset_callback(WATCHDOG_I2C1, on_i2c_watchdog_timeout);
    }
    if(drive_dip_pin_state(DRIVE_DIP_HS_TEMP)){
        drive_task_i2c_watchdog_set_reset_callback(WATCHDOG_I2C2, on_i2c_watchdog_timeout);
    }
}

static void drive_reinit_gui(void)
{
    drive_task_ui_deinit();

    drive_ui_deinit();
    init_drive_ui();

    drive_task_ui_init(DRIVE_TASK_PRIORITY_UI);
}

static void on_drive_reset(void)
{
    if(drive_dip_pin_state(DRIVE_DIP_UI_PAD)){
        drive_task_utils_reset_ui();
    }
}

static void init_drive_tasks(void)
{
    drive_tasks_init();
    
    drive_task_adc_init(DRIVE_TASK_PRIORITY_ADC);
    drive_task_sync_init(DRIVE_TASK_PRIORITY_SYNC);
    drive_task_triacs_init(DRIVE_TASK_PRIORITY_TRIACS);
    drive_task_main_init(DRIVE_TASK_PRIORITY_MAIN);
    drive_task_modbus_init(DRIVE_TASK_PRIORITY_MODBUS);
    drive_task_modbus_setup(modbus_rs485_set_output, modbus_rs485_set_input);
    drive_task_storage_init(DRIVE_TASK_PRIORITY_STORAGE);
    drive_task_utils_init(DRIVE_TASK_PRIORITY_UTILS);
    drive_task_utils_set_reset_ui_callback(drive_reinit_gui);
    drive_task_selftune_init(DRIVE_TASK_PRIORITY_SELFTUNE);

    if(drive_dip_pin_state(DRIVE_DIP_UI_PAD) || drive_dip_pin_state(DRIVE_DIP_HS_TEMP)){
        drive_task_i2c_watchdog_init(DRIVE_TASK_PRIORITY_I2C_WDT);
    }
    setup_i2c_watchdogs();

    drive_task_temp_init(DRIVE_TASK_PRIORITY_TEMP);

    if(drive_dip_pin_state(DRIVE_DIP_UI_PAD)){
        drive_task_buzz_init(DRIVE_TASK_PRIORITY_BUZZ);
        drive_task_ui_init(DRIVE_TASK_PRIORITY_UI);
    }
}

/*
 * FreeRTOS
 * static allocation
 * functions.
 */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer,
                                     StackType_t **ppxTimerTaskStackBuffer,
                                     uint32_t *pulTimerTaskStackSize )
{
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

#if configCHECK_FOR_STACK_OVERFLOW != 0

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pxTask;
    ( void ) pcTaskName;

    __disable_irq();

    drive_mcu_fault();

    //mcu_fault_blink_loop(DRIVE_STACK_OVERFLOW_T_ON, DRIVE_STACK_OVERFLOW_T_OFF);

    for( ;; );
}

#endif

#if configGENERATE_RUN_TIME_STATS == 1

void initHiresCounter(void)
{
}

uint32_t getHiresCounterValue(void)
{
    struct timeval tv;
    drive_hires_timer_value(&tv);
    
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

#endif

#if configASSERT_DEFINED == 1
void vFreeRtosAssert(void)
{
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
#endif

int main(void)
{
    // ACTLR
    //*((unsigned int*)0xE000E008) = 0x7;
    
    extern char _app_origin;
    
    SCB->VTOR = (uint32_t)&_app_origin;
    
    NVIC_SetPriorityGrouping(0x3);
    
    init_dip();

    init_periph_clock();
    init_PDR();
    
    remap_config();

    //init_usart();
    
    //printf("STM32 MCU\r\n");

    init_spi2();
    init_eeprom();
    init_storage();
    
    __enable_irq();

    init_nvdata();

    drive_events_init();
    if(drive_events_read() != E_NO_ERROR){
        drive_events_reset();
    }
    
    settings_init();
    if(settings_read() != E_NO_ERROR){
        settings_default();
    }
    
    __disable_irq();

    init_rtc();
    init_hires_timer();

    init_drive_tasks();
    
    init_gpio();
    
    init_drive();
    init_dio();
    
    init_triacs();
    init_triacs_timers();
    
    init_adc();
    init_adc_timer();
    
    init_watchdog();
    init_drive_main_timer();
    
    if(check_watchdog_reset()){
        clear_watchdog_reset_flag();
        drive_watchdog_timeout();
    }
    
    init_modbus_usart_485();
    init_modbus_485();
    init_modbus_usart_bt();
    init_modbus_bt();
    init_drive_modbus();
    
    if(drive_dip_pin_state(DRIVE_DIP_UI_PAD)){
        init_i2c1();
        init_spi();
        init_drive_ui();
        init_exti_pca9555();
    }
    
    if(drive_dip_pin_state(DRIVE_DIP_HS_TEMP)){
        init_i2c2();
        init_heatsink_sensor();
    }
    init_fan_pwm();
    init_drive_temp();
    
    // Start RTOS.
    vTaskStartScheduler();
    
    for(;;){
    }
    
    return 0;
}
