/**
 * @file main.c Главный файл проекта ППТ.
 */
#include "stm32f10x.h"
#define USART_STDIO
#include "usart/usart_buf.h"
#include "usart/usart_bus.h"
#include "modbus/modbus_rtu.h"
#include <stdio.h>
#include "counter/counter.h"
#include "rtc/rtc.h"
#include "spi/spi.h"
#include "dma/dma.h"
#include "utils/delay.h"
#include "utils/utils.h"
#include "tft9341/tft9341.h"
#include "fixed/fixed32.h"
#include "pca9555/pca9555.h"
#include "scheduler/scheduler.h"
#include "m95x/m95x.h"
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
#include <string.h>
#include <stdlib.h>


//! Буфер записи USART.
#define USART_WRITE_BUFFER_SIZE 128
static uint8_t usart_write_buffer[USART_WRITE_BUFFER_SIZE];
//! Буфер чтения USART.
#define USART_READ_BUFFER_SIZE 32
static uint8_t usart_read_buffer[USART_READ_BUFFER_SIZE];
//! Буферизированый USART.
static usart_buf_t usart_buf;

//! Шина USART для Modbus.
static usart_bus_t usart_bus;

//! GPIO контроля интерфейса RS485.
#define RS485_IFACE_CTL_GPIO GPIOC
//! Пин контроля интерфейса RS485.
#define RS485_IFACE_CTL_PIN GPIO_Pin_9

//! Modbus.
static modbus_rtu_t modbus;
//! Сообщения Modbus.
static modbus_rtu_message_t modbus_rx_msg, modbus_tx_msg;

//! Счётчик.
static counter_t counter = 0;

//! Счётчик миллисекунд для функции gettimeofday.
static counter_t counter_ms_gtod = 0;

//! Планеровщик задач.
//! Максимум задач.
#define SCHEDULER_TASKS_MAX 5
//! Буфер задач.
static TASKS_BUFFER(tasks_buffer, SCHEDULER_TASKS_MAX);

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


//! Шина i2c.
static i2c_bus_t i2c;

//! Расширитель ввода-вывода.
static pca9555_t ioport;

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
//! Порт EXTI.
#define EXTI_PHASE_GPIO GPIOE
//! Пин датчика нуля фазы A.
#define EXTI_PHASE_A_Pin GPIO_Pin_13
//! Пин датчика нуля фазы B.
#define EXTI_PHASE_B_Pin GPIO_Pin_14
//! Пин датчика нуля фазы C.
#define EXTI_PHASE_C_Pin GPIO_Pin_15
//! Линия фазы A.
#define EXTI_PHASE_A_LINE EXTI_Line13
//! Линия фазы B.
#define EXTI_PHASE_B_LINE EXTI_Line14
//! Линия фазы C.
#define EXTI_PHASE_C_LINE EXTI_Line15
//! GPIO PCA9555.
#define EXTI_PCA9555_GPIO GPIOD
//! Пин PCA95555.
#define EXTI_PCA9555_Pin GPIO_Pin_7
//! Линия PCA9555.
#define EXTI_PCA9555_LINE EXTI_Line7


//! Длина буфера ADC 1 и 2 в трансферах 32 бит.
#define ADC12_RAW_BUFFER_DMA_TRANSFERS 4
//! Размер буфера ADC 1 и 2.
#define ADC12_RAW_BUFFER_SIZE (ADC12_RAW_BUFFER_DMA_TRANSFERS * 2)
//! Длина буфера ADC 3 в трансферах 16 бит.
#define ADC3_RAW_BUFFER_DMA_TRANSFERS 3
//! Размер буфера ADC 3.
#define ADC3_RAW_BUFFER_SIZE (ADC3_RAW_BUFFER_DMA_TRANSFERS)
//! Буфер ADC.
static volatile uint16_t adc_raw_buffer[ADC12_RAW_BUFFER_SIZE + ADC3_RAW_BUFFER_SIZE] = {0};
//! Число измерений ADC.
#define ADC_MEASUREMENTS_PER_PERIOD 128
//! Число измерений ADC для вычисления питания.
#define ADC_MEASUREMENTS_FOR_CALCULATION (ADC_MEASUREMENTS_PER_PERIOD / 4)
//! Число выполненных измерений ADC.
static unsigned int adc_data_measured = 0;


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
#define IRQ_PRIOR_TRIACS_TIMER 0
#define IRQ_PRIOR_TRIAC_EXC_TIMER 0
#define IRQ_PRIOR_NULL_SENSORS 1
#define IRQ_PRIOR_ADC_DMA 2
#define IRQ_PRIOR_NULL_TIMER 4
#define IRQ_PRIOR_PHASES_TIMER 4
#define IRQ_PRIOR_MODBUS_USART 3
#define IRQ_PRIOR_I2C1 5
#define IRQ_PRIOR_SPI1 5
#define IRQ_PRIOR_SPI2 5
#define IRQ_PRIOR_KEYPAD 6
#define IRQ_PRIOR_USART 5
#define IRQ_PRIOR_DMA_CH1 5
#define IRQ_PRIOR_DMA_CH2 5 // spi1
#define IRQ_PRIOR_DMA_CH3 5 // spi1
#define IRQ_PRIOR_DMA_CH4 5 // spi2 usart1 (modbus)
#define IRQ_PRIOR_DMA_CH5 5 // spi2 usart1 (modbus)
#define IRQ_PRIOR_DMA_CH6 5 // i2c1 usart2 (modbus)
#define IRQ_PRIOR_DMA_CH7 5 // i2c1 usart2 (modbus)
#define IRQ_PRIOR_RTC 6
#define IRQ_PRIOR_RTC_ALARM 6


//! Атрибуты обработчиков прерываний.
#define IRQ_ATTRIBS __attribute__ ((interrupt ("IRQ")))


/*
 * Обработчики исключений.
 */

IRQ_ATTRIBS void HardFault_Handler(void)
{
    for(;;);
}

IRQ_ATTRIBS void MemManage_Handler(void)
{
    for(;;);
}

IRQ_ATTRIBS void BusFault_Handler(void)
{
    for(;;);
}

IRQ_ATTRIBS void UsageFault_Handler(void)
{
    for(;;);
}

IRQ_ATTRIBS void SVC_Handler(void)
{
    for(;;);
}

IRQ_ATTRIBS void PendSV_Handler(void)
{
    for(;;);
}

/*
 * Обработчики прерываний.
 */

IRQ_ATTRIBS void USART1_IRQHandler(void)
{
    usart_bus_irq_handler(&usart_bus);
}

IRQ_ATTRIBS void USART3_IRQHandler(void)
{
    usart_buf_irq_handler(&usart_buf);
}

IRQ_ATTRIBS void SysTick_Handler(void)
{
    system_counter_tick();
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
    spi_bus_dma_rx_channel_irq_handler(&spi);
}

IRQ_ATTRIBS void DMA1_Channel3_IRQHandler(void)
{
    spi_bus_dma_tx_channel_irq_handler(&spi);
}

IRQ_ATTRIBS void DMA1_Channel4_IRQHandler(void)
{
    usart_bus_dma_tx_channel_irq_handler(&usart_bus) ||
    spi_bus_dma_rx_channel_irq_handler(&spi2);
}

IRQ_ATTRIBS void DMA1_Channel5_IRQHandler(void)
{
    usart_bus_dma_rx_channel_irq_handler(&usart_bus) ||
    spi_bus_dma_tx_channel_irq_handler(&spi2);
}

IRQ_ATTRIBS void DMA1_Channel6_IRQHandler(void)
{
    i2c_bus_dma_tx_channel_irq_handler(&i2c);
}

IRQ_ATTRIBS void DMA1_Channel7_IRQHandler(void)
{
    i2c_bus_dma_rx_channel_irq_handler(&i2c);
}

IRQ_ATTRIBS void I2C1_EV_IRQHandler(void)
{
    i2c_bus_event_irq_handler(&i2c);
}

IRQ_ATTRIBS void I2C1_ER_IRQHandler(void)
{
    i2c_bus_error_irq_handler(&i2c);
}

IRQ_ATTRIBS void RTCAlarm_IRQHandler(void)
{
    rtc_alarm_interrupt_handler();
}

IRQ_ATTRIBS void RTC_IRQHandler(void)
{
    counter_ms_gtod = system_counter_ticks();
    rtc_interrupt_handler();
}

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
    tv->tv_usec = system_counter_diff(&counter_ms_gtod) * 1000;
    
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
 * Обработчик окончания передачи данных от ADC.
 */
IRQ_ATTRIBS void DMA1_Channel1_IRQHandler(void)
{
    if(DMA_GetITStatus(DMA1_IT_TC1)){
        DMA_ClearITPendingBit(DMA1_IT_TC1);
        
        drive_process_power_adc_values(DRIVE_POWER_CHANNELS, (uint16_t*)adc_raw_buffer);
        
        if(++ adc_data_measured >= ADC_MEASUREMENTS_FOR_CALCULATION){
            adc_data_measured = 0;
            
            drive_process_power_accumulated_data(DRIVE_POWER_CHANNELS);
        }
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

/**
 * Разрешает прерывание от заданных линий EXTI.
 * @param lines_mask Линии EXTI.
 */
ALWAYS_INLINE static void exti_enable_lines(uint16_t lines_mask)
{
    EXTI->IMR |= lines_mask;
}

/**
 * Запрещает прерывание от заданных линий EXTI.
 * @param lines_mask Линии EXTI.
 */
ALWAYS_INLINE static void exti_disable_lines(uint16_t lines_mask)
{
    EXTI->IMR &= ~lines_mask;
}

#ifdef DISABLE_LAST_NULL_SENSOR
/**
 * Включает и отключает прерывания от EXTI
 * согласно текущей фазе и направлению движения.
 * @param phase Фаза датчика нуля.
 */
static void update_exti_inputs(phase_t phase)
{
    if(phase == PHASE_UNK) return;
    
    drive_dir_t dir = drive_direction();
    
    if(dir == DRIVE_DIR_UNK) return;
    
    // Обработаем фазу.
    switch(phase){
        case PHASE_A:
            exti_disable_lines(EXTI_PHASE_A_LINE);
            exti_enable_lines((dir == DRIVE_DIR_FORW) ? EXTI_PHASE_B_LINE : EXTI_PHASE_C_LINE);
            break;
        case PHASE_B:
            exti_disable_lines(EXTI_PHASE_B_LINE);
            exti_enable_lines((dir == DRIVE_DIR_FORW) ? EXTI_PHASE_C_LINE : EXTI_PHASE_A_LINE);
            break;
        case PHASE_C:
            exti_disable_lines(EXTI_PHASE_C_LINE);
            exti_enable_lines((dir == DRIVE_DIR_FORW) ? EXTI_PHASE_A_LINE : EXTI_PHASE_B_LINE);
            break;
        default:
            return;
    }
}
#endif

/**
 * Получает состояние флага прерывания линии EXTI.
 * @param exti_line Линия EXTI.
 * @param imr Регистр IMR EXTI.
 * @retrun Состояние флага прерывания линии EXTI.
 */
ALWAYS_INLINE static bool exti_it_status(uint16_t exti_line, uint32_t imr)
{
    return EXTI->PR & imr & exti_line;
}

/**
 * Очищает флаг прерывания линии EXTI.
 * @param exti_line Линия EXTI.
 */
ALWAYS_INLINE static void exti_clear_it(uint16_t exti_line)
{
    EXTI->PR |= exti_line;
}

ALWAYS_INLINE static null_sensor_edge_t exti_gpio_edge(GPIO_TypeDef* GPIO, uint16_t pin)
{
    return (GPIO->IDR & pin) ? NULL_SENSOR_EDGE_TRAILING : NULL_SENSOR_EDGE_LEADING;
}

/**
 * Прерывание по датчикам нуля.
 */
IRQ_ATTRIBS void EXTI15_10_IRQHandler(void)
{
    uint32_t imr = EXTI->IMR;
    // Датчик нуля фазы A.
    if (exti_it_status(EXTI_PHASE_A_LINE, imr) != RESET)
    {
        drive_process_null_sensor(PHASE_A,
                exti_gpio_edge(EXTI_PHASE_GPIO, EXTI_PHASE_A_Pin)); // Обрабатывает событие.
#ifdef DISABLE_LAST_NULL_SENSOR
        update_exti_inputs(PHASE_A);        // Обновим разрешения внешних прерываний.
#endif
        exti_clear_it(EXTI_PHASE_A_LINE);  // очищаем флаг прерывания 13
    }

    // Датчик нуля фазы B.
    if (exti_it_status(EXTI_PHASE_B_LINE, imr) != RESET)
    {
        drive_process_null_sensor(PHASE_B,
                exti_gpio_edge(EXTI_PHASE_GPIO, EXTI_PHASE_B_Pin)); // Обрабатывает событие.
#ifdef DISABLE_LAST_NULL_SENSOR
        update_exti_inputs(PHASE_B);        // Обновим разрешения внешних прерываний.
#endif
        exti_clear_it(EXTI_PHASE_B_LINE);  // очищаем флаг прерывания 14
    }

    // Датчик нуля фазы C.
    if (exti_it_status(EXTI_PHASE_C_LINE, imr) != RESET)
    {
        drive_process_null_sensor(PHASE_C,
                exti_gpio_edge(EXTI_PHASE_GPIO, EXTI_PHASE_C_Pin)); // Обрабатывает событие.
#ifdef DISABLE_LAST_NULL_SENSOR
        update_exti_inputs(PHASE_C);        // Обновим разрешения внешних прерываний.
#endif
        exti_clear_it(EXTI_PHASE_C_LINE);  // очищаем флаг прерывания 15
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

IRQ_ATTRIBS void TIM6_IRQHandler(void)
{
    drive_phases_timer_irq_handler();
}

IRQ_ATTRIBS void TIM7_IRQHandler(void)
{
    drive_null_timer_irq_handler();
}

/*
 * Функции обратного вызова (каллбэки).
 */

static bool i2c_callback(void)
{
    return pca9555_i2c_callback(&ioport);
}

static bool spi_callback(void)
{
    return tft9341_spi_callback(&tft);
}

static bool spi2_callback(void)
{
    return m95x_spi_callback(&eeprom);
}

ALWAYS_INLINE static void modbus_rs485_set_input(void)
{
    RS485_IFACE_CTL_GPIO->BRR = RS485_IFACE_CTL_PIN;
}

ALWAYS_INLINE static void modbus_rs485_set_output(void)
{
    RS485_IFACE_CTL_GPIO->BSRR = RS485_IFACE_CTL_PIN;
}

static bool usart_rx_byte_callback(uint8_t byte)
{
    return modbus_rtu_usart_rx_byte_callback(&modbus, byte);
}

static bool usart_rx_callback(void)
{
    return modbus_rtu_usart_rx_callback(&modbus);
}

static bool usart_tx_callback(void)
{
    return modbus_rtu_usart_tx_callback(&modbus);
}

static bool usart_tc_callback(void)
{
    modbus_rs485_set_input();
    
    return true;
}

static void modbus_on_msg_recv(void)
{
    modbus_rs485_set_output();
    if(modbus_rtu_dispatch(&modbus) != E_NO_ERROR){
        modbus_rs485_set_input();
    }
    drive_gui_modbus_set_last_time();
}

static void modbus_on_msg_sent(void)
{
    drive_gui_modbus_set_last_time();
}

/*
 * Инициализация.
 */

static void init_sys_counter(void)
{
    system_counter_init(1000);
    counter = system_counter_ticks();
    SysTick_Config(SystemCoreClock / 1000);
}

static void remap_config(void)
{
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);          
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
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
    // SPI.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    // SPI2.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    // I2C1.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
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
    // TIM7.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);    // Включаем тактирование Basic TIM7
    // Backup domain + RTC.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);
}

static void init_usart(void)
{
    GPIO_InitTypeDef gpio_tx =
        {.GPIO_Pin = GPIO_Pin_10, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_AF_PP};
    GPIO_InitTypeDef gpio_rx =
        {.GPIO_Pin = GPIO_Pin_11, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_IN_FLOATING};
    GPIO_Init(GPIOC, &gpio_tx);
    GPIO_Init(GPIOC, &gpio_rx);
    
    USART_InitTypeDef usart_is =
        {.USART_BaudRate = 115200, .USART_WordLength = USART_WordLength_8b, .USART_StopBits = USART_StopBits_1,
         .USART_Parity = USART_Parity_No, .USART_Mode = USART_Mode_Rx | USART_Mode_Tx, .USART_HardwareFlowControl = USART_HardwareFlowControl_None};
    USART_Init(USART3, &usart_is);
    USART_Cmd(USART3, ENABLE);
    
    usart_buf_init_t usartb_is = {.usart = USART3,
         .write_buffer = usart_write_buffer, .write_buffer_size = USART_WRITE_BUFFER_SIZE,
         .read_buffer = usart_read_buffer, .read_buffer_size = USART_READ_BUFFER_SIZE};
    usart_buf_init(&usart_buf, &usartb_is);
    usart_setup_stdio(&usart_buf);
    
    NVIC_SetPriority(USART3_IRQn, IRQ_PRIOR_USART);
    NVIC_EnableIRQ(USART3_IRQn);
}

static void init_PDR(void)
{
    PWR->CR |= PWR_CR_PLS_2V9;
    PWR->CR |= PWR_CR_PVDE;
}

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
    
    NVIC_SetPriority(RTC_IRQn, IRQ_PRIOR_RTC);
    NVIC_SetPriority(RTCAlarm_IRQn, IRQ_PRIOR_RTC_ALARM);
    NVIC_EnableIRQ(RTC_IRQn);
    NVIC_EnableIRQ(RTCAlarm_IRQn);
}

/*
static void init_modbus_usart2(void)
{
    GPIO_InitTypeDef gpio_tx =
        {.GPIO_Pin = GPIO_Pin_5, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_AF_PP};
    GPIO_InitTypeDef gpio_rx =
        {.GPIO_Pin = GPIO_Pin_6, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_IN_FLOATING};
    
    USART_InitTypeDef usart_is =
        {.USART_BaudRate = 9600, .USART_WordLength = USART_WordLength_8b, .USART_StopBits = USART_StopBits_1,
         .USART_Parity = USART_Parity_No, .USART_Mode = USART_Mode_Rx | USART_Mode_Tx, .USART_HardwareFlowControl = USART_HardwareFlowControl_None};
    
    usart_bus_init_t usartb_is = {
        .usart_device = USART2,
        .dma_tx_channel = DMA1_Channel7,
        .dma_rx_channel = DMA1_Channel6
    };
    
    param_t* baud_param = settings_param_by_id(PARAM_ID_MODBUS_BAUD);
    if(baud_param){
        usart_is.USART_BaudRate = settings_param_valueu(baud_param);
    }
    
    GPIO_Init(GPIOD, &gpio_tx);
    GPIO_Init(GPIOD, &gpio_rx);
    
    USART_Init(USART2, &usart_is);
    USART_Cmd(USART2, ENABLE);
    usart_bus_init(&usart_bus, &usartb_is);
    
    usart_bus_set_rx_callback(&usart_bus, usart_rx_callback);
    usart_bus_set_tx_callback(&usart_bus, usart_tx_callback);
    usart_bus_set_rx_byte_callback(&usart_bus, usart_rx_byte_callback);
    
    usart_bus_set_idle_mode(&usart_bus, USART_IDLE_MODE_END_RX);
    
    NVIC_SetPriority(USART2_IRQn, IRQ_PRIOR_MODBUS_USART);
    NVIC_EnableIRQ(USART2_IRQn);
    
    //NVIC_SetPriority(DMA1_Channel6_IRQn, IRQ_PRIOR_DMA_CH6);
    //NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    //NVIC_SetPriority(DMA1_Channel7_IRQn, IRQ_PRIOR_DMA_CH7);
    //NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}
*/

static void init_modbus_usart(void)
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
    usart_bus_init(&usart_bus, &usartb_is);
    
    usart_bus_set_rx_callback(&usart_bus, usart_rx_callback);
    usart_bus_set_tx_callback(&usart_bus, usart_tx_callback);
    usart_bus_set_tc_callback(&usart_bus, usart_tc_callback);
    usart_bus_set_rx_byte_callback(&usart_bus, usart_rx_byte_callback);
    
    usart_bus_set_idle_mode(&usart_bus, USART_IDLE_MODE_END_RX);
    
    NVIC_SetPriority(USART1_IRQn, IRQ_PRIOR_MODBUS_USART);
    NVIC_EnableIRQ(USART1_IRQn);
    
    /*NVIC_SetPriority(DMA1_Channel4_IRQn, IRQ_PRIOR_DMA_CH4);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    NVIC_SetPriority(DMA1_Channel5_IRQn, IRQ_PRIOR_DMA_CH5);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);*/
}

static void init_modbus(void)
{
    modbus_rtu_init_t modbus_is;
    
    modbus_is.usart = &usart_bus;
    modbus_is.mode = MODBUS_RTU_MODE_SLAVE;
    modbus_is.address = 0xaa;//0x10;
    modbus_is.rx_message = &modbus_rx_msg;
    modbus_is.tx_message = &modbus_tx_msg;
    
    modbus_rtu_init(&modbus, &modbus_is);
    modbus_rtu_set_msg_recv_callback(&modbus, modbus_on_msg_recv);
    modbus_rtu_set_msg_sent_callback(&modbus, modbus_on_msg_sent);
    
    modbus_rs485_set_input();
}

static void init_drive_modbus(void)
{
    drive_modbus_init_t drive_modbus_is;
    
    drive_modbus_is.apply_settings_callback = (apply_settings_callback_t)drive_tasks_apply_settings;
    drive_modbus_is.save_settings_callback = (save_settings_callback_t)drive_tasks_save_settings;
    
    drive_modbus_init(&modbus, &drive_modbus_is);
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

static void init_i2c_periph(void)
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

static void reset_i2c(void)
{
    i2c_bus_reset(&i2c);
    i2c_device_reset(I2C1);
    
    init_i2c_periph();
}

static void init_i2c(void)
{
    GPIO_InitTypeDef gpio_i2c =
        {.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7, .GPIO_Speed = GPIO_Speed_10MHz, .GPIO_Mode = GPIO_Mode_AF_OD};
    GPIO_Init(GPIOB, &gpio_i2c);
    
    init_i2c_periph();
    
    i2c_bus_init_t i2c_bus_is;
    i2c_bus_is.i2c_device = I2C1;
    i2c_bus_is.dma_rx_channel = DMA1_Channel7;
    i2c_bus_is.dma_tx_channel = DMA1_Channel6;
    
    i2c_bus_init(&i2c, &i2c_bus_is);
    
    i2c_bus_set_callback(&i2c, i2c_callback);
    
    NVIC_SetPriority(I2C1_EV_IRQn, IRQ_PRIOR_I2C1);
    NVIC_SetPriority(I2C1_ER_IRQn, IRQ_PRIOR_I2C1);
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    NVIC_EnableIRQ(I2C1_ER_IRQn);
    
    NVIC_SetPriority(DMA1_Channel6_IRQn, IRQ_PRIOR_DMA_CH6);
    NVIC_SetPriority(DMA1_Channel7_IRQn, IRQ_PRIOR_DMA_CH7);
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);
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
    dma_is.DMA_MemoryBaseAddr = (uint32_t)adc_raw_buffer;
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
    dma_is.DMA_MemoryBaseAddr = (uint32_t)(adc_raw_buffer + ADC12_RAW_BUFFER_SIZE);
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
    adc_is.ADC_NbrOfChannel = ADC12_RAW_BUFFER_DMA_TRANSFERS;
    
    ADC_Init(ADC1, &adc_is);
    
    ADC_StructInit(&adc_is);
    //Настройка параметров  ADC2
    adc_is.ADC_Mode = ADC_Mode_RegSimult;
    adc_is.ADC_ScanConvMode = ENABLE;
    adc_is.ADC_ContinuousConvMode = DISABLE;
    adc_is.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    adc_is.ADC_DataAlign = ADC_DataAlign_Right;
    adc_is.ADC_NbrOfChannel = ADC12_RAW_BUFFER_DMA_TRANSFERS;
    
    ADC_Init(ADC2, &adc_is);
    
    ADC_StructInit(&adc_is);
    //Настройка параметров  ADC3
    adc_is.ADC_Mode = ADC_Mode_Independent;
    adc_is.ADC_ScanConvMode = ENABLE;
    adc_is.ADC_ContinuousConvMode = DISABLE;
    adc_is.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;
    adc_is.ADC_DataAlign = ADC_DataAlign_Right;
    adc_is.ADC_NbrOfChannel = ADC3_RAW_BUFFER_DMA_TRANSFERS;
    
    ADC_Init(ADC3, &adc_is);
    
    
#define ADC_SAMPLE_TIME ADC_SampleTime_71Cycles5
    
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

static void init_ioport_simple(void)
{
    pca9555_init(&ioport, &i2c, PCA9555_I2C_DEFAULT_ADDRESS);
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

static void init_drive(void)
{
    drive_init();
    drive_set_error_callback((drive_error_callback_t)drive_tasks_write_error_event);
    drive_set_warning_callback((drive_warning_callback_t)drive_tasks_write_warning_event);
}

static void init_drive_ui(void)
{
    init_ioport_simple();
    init_tft_simple();
    
    drive_ui_init_t ui_is;
    
    ui_is.ioport = &ioport;
    ui_is.tft = &tft;
    ui_is.reset_i2c_bus_proc = reset_i2c;
    
    drive_ui_init(&ui_is);
}

/******************************************************************************/
static void init_adc_timer(void)
{
    TIM_DeInit(TIM1);
    
    TIM_TimeBaseInitTypeDef tim1_is;
    TIM_TimeBaseStructInit(&tim1_is);
            tim1_is.TIM_Prescaler = 10-1;                    // Делитель (0000...FFFF)
            tim1_is.TIM_CounterMode = TIM_CounterMode_Up;    // Режим счетчика
            tim1_is.TIM_Period = 1125-1;                     // Значение периода (0000...FFFF)
            tim1_is.TIM_ClockDivision = 0;                   // определяет тактовое деление
    TIM_TimeBaseInit(TIM1, &tim1_is);

    TIM_OCInitTypeDef tim1_oc_is;
    TIM_OCStructInit(&tim1_oc_is);
        tim1_oc_is.TIM_OCMode = TIM_OCMode_PWM1;
        tim1_oc_is.TIM_OutputState = TIM_OutputState_Disable;
        tim1_oc_is.TIM_OutputNState = TIM_OutputNState_Disable;
        tim1_oc_is.TIM_Pulse = tim1_is.TIM_Period / 2;
        tim1_oc_is.TIM_OCPolarity = TIM_OCPolarity_Low;
        tim1_oc_is.TIM_OCNPolarity = TIM_OCNPolarity_Low;
        tim1_oc_is.TIM_OCIdleState = TIM_OCIdleState_Reset;
        tim1_oc_is.TIM_OCNIdleState = TIM_OCNIdleState_Reset ;
    TIM_OC3Init(TIM1, &tim1_oc_is);
    TIM_CCxCmd (TIM1, TIM_Channel_3, TIM_CCx_Enable);
    
    TIM_Cmd(TIM1, ENABLE);                                                      // Начать отсчёт!
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

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
    TIM_OC3Init(TIM, &tim_oc_is);
    TIM_OC4Init(TIM, &tim_oc_is);
    
    TIM_CCxCmd (TIM, TRIACS_A_OPEN_CHANNEL,  TIM_CCx_Enable);
    TIM_CCxCmd (TIM, TRIACS_A_CLOSE_CHANNEL, TIM_CCx_Enable);
    TIM_CCxCmd (TIM, TRIACS_B_OPEN_CHANNEL,  TIM_CCx_Enable);
    TIM_CCxCmd (TIM, TRIACS_B_CLOSE_CHANNEL, TIM_CCx_Enable);
    
    TIM_ITConfig(TIM, TRIACS_A_OPEN_CHANNEL_IT,  ENABLE); // Разрешаем прерывание OC 1 от таймера
    TIM_ITConfig(TIM, TRIACS_A_CLOSE_CHANNEL_IT, ENABLE); // Разрешаем прерывание OC 2 от таймера
    TIM_ITConfig(TIM, TRIACS_B_OPEN_CHANNEL_IT,  ENABLE); // Разрешаем прерывание OC 3 от таймера
    TIM_ITConfig(TIM, TRIACS_B_CLOSE_CHANNEL_IT, ENABLE); // Разрешаем прерывание OC 4 от таймера
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
    
    TIM_ITConfig(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_OPEN_CHANNEL_IT,   ENABLE); // Разрешаем прерывание OC 1 от таймера
    TIM_ITConfig(TIM, TRIAC_EXC_FIRST_HALF_CYCLE_CLOSE_CHANNEL_IT,  ENABLE); // Разрешаем прерывание OC 2 от таймера
    TIM_ITConfig(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_OPEN_CHANNEL_IT,  ENABLE); // Разрешаем прерывание OC 3 от таймера
    TIM_ITConfig(TIM, TRIAC_EXC_SECOND_HALF_CYCLE_CLOSE_CHANNEL_IT, ENABLE); // Разрешаем прерывание OC 4 от таймера
}

static void init_phases_timer(void)
{
    TIM_DeInit(TIM6);
    TIM_TimeBaseInitTypeDef tim_is;
            tim_is.TIM_Prescaler = DRIVE_PHASE_STATE_TIMER_CNT_PRESCALER; // Настраиваем делитель чтобы таймер тикал 1 000 000 раз в секунду
            tim_is.TIM_CounterMode = TIM_CounterMode_Up;    // Режим счетчика
            tim_is.TIM_Period = DRIVE_PHASE_STATE_TIMER_CNT_PERIOD; // Значение периода (0000...FFFF)
            tim_is.TIM_ClockDivision = TIM_CKD_DIV1;        // определяет тактовое деление
    TIM_TimeBaseInit(TIM6, &tim_is);
    TIM_SelectOnePulseMode(TIM6, TIM_OPMode_Single);                    // Однопульсный режим таймера
    TIM_SetCounter(TIM6, 0);                                            // Сбрасываем счетный регистр в ноль
    
    drive_set_phase_state_timer(TIM6);
    
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);                        // Разрешаем прерывание от таймера
    //TIM_Cmd(TIM6, ENABLE);
    
    NVIC_SetPriority(TIM6_IRQn, IRQ_PRIOR_PHASES_TIMER);
    NVIC_EnableIRQ(TIM6_IRQn);
}

static void init_null_timer(void)
{
    TIM_DeInit(TIM7);
    TIM_TimeBaseInitTypeDef tim_is;
            tim_is.TIM_Prescaler = DRIVE_NULL_TIMER_CNT_PRESCALER; // Настраиваем делитель чтобы таймер тикал 1 000 000 раз в секунду
            tim_is.TIM_CounterMode = TIM_CounterMode_Up;    // Режим счетчика
            tim_is.TIM_Period = DRIVE_NULL_TIMER_CNT_PERIOD; // Значение периода (0000...FFFF)
            tim_is.TIM_ClockDivision = TIM_CKD_DIV1;        // определяет тактовое деление
    TIM_TimeBaseInit(TIM7, &tim_is);
    TIM_SelectOnePulseMode(TIM7, TIM_OPMode_Repetitive);                // Повторяющийся режим таймера
    TIM_SetCounter(TIM7, 0);                                            // Сбрасываем счетный регистр в ноль
    
    drive_set_null_timer(TIM7);
    
    TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);                        // Разрешаем прерывание от таймера
    TIM_Cmd(TIM7, ENABLE);                                            // Начать отсчёт!
    
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

static void init_exti_zero_sensors(void)
{
    /*
     * Zero sensor
     */
    /* GPIOB Configuration: 44 (PE13 - ZeroSensor_1); 45 (PE14 - ZeroSensor_2); 46 (PE15 - ZeroSensor_3) as input floating */
    GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin = EXTI_PHASE_A_Pin | EXTI_PHASE_B_Pin | EXTI_PHASE_C_Pin;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(EXTI_PHASE_GPIO, &GPIO_InitStructure);
    
    /*
     * Привязка Прерывания к портам МК.
     */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource13);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource14);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource15);
    
    
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_PHASE_A_LINE | EXTI_PHASE_B_LINE | EXTI_PHASE_C_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_SetPriority(EXTI15_10_IRQn, IRQ_PRIOR_NULL_SENSORS);
 
    NVIC_EnableIRQ (EXTI15_10_IRQn);     // Разрешаем прерывание от 10-15 ног
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
        
    /* GPIOC Configuration: 64 (PC7 - FAN) as output pp */
        gpio_is.GPIO_Pin = GPIO_Pin_7;
        gpio_is.GPIO_Speed = GPIO_Speed_2MHz;
        gpio_is.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &gpio_is);
    //GPIO_SetBits(GPIOC, GPIO_Pin_7);
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


static void* main_task(void* arg)
{
    drive_ui_process();
    
    return NULL;
}

int main(void)
{
    // ACTLR
    //*((unsigned int*)0xE000E008) = 0x7;
    
    NVIC_SetPriorityGrouping(0x3);
    
    init_sys_counter();
    
    init_periph_clock();
    init_PDR();
    
    remap_config();
    
    init_usart();
    
    init_rtc();
    
    printf("STM32 MCU\r\n");
    
    init_i2c();
    init_spi();
    
    init_spi2();
    init_eeprom();
    init_storage();
    
    drive_events_init();
    if(drive_events_read() != E_NO_ERROR){
        drive_events_reset();
    }
    
    settings_init();
    if(settings_read() != E_NO_ERROR){
        settings_default();
    }
    
    scheduler_init(tasks_buffer, SCHEDULER_TASKS_MAX);
    
    drive_tasks_init();
    
    init_gpio();
    
    init_drive();
    init_dio();
    
    init_triacs();
    init_triacs_timers();
    
    init_adc();
    init_adc_timer();
    
    init_phases_timer();
    init_null_timer();
    
    init_exti_zero_sensors();
    
    init_modbus_usart();
    init_modbus();
    init_drive_modbus();
    
    init_drive_ui();
    
    init_exti_pca9555();
    
    //drive_set_reference(REFERENCE_MAX);
    
    scheduler_add_task(main_task, 0, NULL, 0, NULL);
    
    for(;;){
        scheduler_process();
        //if(!scheduler_process()) __WFI();
    }
    return 0;
}
