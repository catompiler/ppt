//Проект gui_test (из шаблона).
#include "stm32f10x.h"
#define USART_STDIO
#include "usart/usart.h"
#include <stdio.h>
#include "counter/counter.h"
#include "spi/spi.h"
#include "dma/dma.h"
#include "utils/delay.h"
#include "utils/utils.h"
#include "tft9341/tft9341.h"
#include "tft9341/tft9341_cache.h"
#include "tft9341/tft9341_cache_vbuf.h"
#include "graphics/graphics.h"
#include "graphics/painter.h"
#include "graphics/font_5x8_utf8.h"
#include "graphics/font_10x16_utf8.h"
#include "fixed/fixed32.h"
#include "pca9555/pca9555.h"
#include "gui/gui.h"
#include "gui/gui_object.h"
#include "gui/gui_widget.h"
#include "gui/gui_label.h"
#include "gui/gui_number_label.h"
#include "gui/gui_checkbox.h"
#include <string.h>
#include <stdlib.h>


/******************************************************************************/
#include "Mylib/defines.h"
#include "Mylib/mylib.h"
#include "I2Clib/I2Clib.h"
#include "phase_state/phase_state.h"
/******************************************************************************/


#define USART_WRITE_BUFFER_SIZE 64
static uint8_t usart_write_buffer[USART_WRITE_BUFFER_SIZE];

#define USART_READ_BUFFER_SIZE 32
static uint8_t usart_read_buffer[USART_READ_BUFFER_SIZE];

static usart_buf_t usart_buf;

static counter_t counter;

//! Шина spi.
static spi_bus_t spi;

//! Шина i2c.
static i2c_bus_t i2c;

//! Расширитель ввода-вывода.
static pca9555_t ioport;

#define TFT_IOPIN PCA9555_PIN_0
//#define TFT_IOPIN PCA9555_PIN_0

//static volatile bool need_update = false;

static tft9341_t tft;
#define TFT_PIXEL_SIZE 2
#define TFT_WIDTH 320
#define TFT_HEIGHT 240

#define TFT_CACHE_BUFS_COUNT 2
#define TFT_CACHE_BUF0_PIXELS 240
#define TFT_CACHE_BUF0_SIZE (TFT_CACHE_BUF0_PIXELS * TFT_PIXEL_SIZE)
#define TFT_CACHE_BUF1_PIXELS 80
#define TFT_CACHE_BUF1_SIZE (TFT_CACHE_BUF1_PIXELS * TFT_PIXEL_SIZE)

static uint8_t tft_cache_buf_data0[TFT_CACHE_BUF0_SIZE];
static uint8_t tft_cache_buf_data1[TFT_CACHE_BUF1_SIZE];

static tft9341_cache_buffer_t tft_cache_bufs[TFT_CACHE_BUFS_COUNT] = {
    make_tft9341_cache_buffer(tft_cache_buf_data0, TFT_CACHE_BUF0_SIZE),
    make_tft9341_cache_buffer(tft_cache_buf_data1, TFT_CACHE_BUF1_SIZE)
};
static tft9341_cache_t tft_cache = make_tft9341_cache(&tft, TFT_PIXEL_SIZE, tft_cache_bufs, TFT_CACHE_BUFS_COUNT, TFT9341_ROW_COL_REVERSE_MODE);
static graphics_vbuf_t graph_vbuf = make_tft9341_cache_vbuf();
static graphics_t graphics = make_tft9341_cache_graphics(&tft_cache, &graph_vbuf, TFT_WIDTH, TFT_HEIGHT, GRAPHICS_FORMAT_RGB_565);
//static painter_t painter = make_painter(&graphics);
static const font_bitmap_t font_5x8_utf8_bitmaps[] = {
    make_font_bitmap(32, 127, font_5x8_utf8_part0_data, FONT_5X8_UTF8_PART0_WIDTH, FONT_5X8_UTF8_PART0_HEIGHT, GRAPHICS_FORMAT_BW_1_V),
    make_font_bitmap(0xb0, 0xb0, font_5x8_utf8_part1_data, FONT_5X8_UTF8_PART1_WIDTH, FONT_5X8_UTF8_PART1_HEIGHT, GRAPHICS_FORMAT_BW_1_V),
    make_font_bitmap(0x400, 0x451, font_5x8_utf8_part2_data, FONT_5X8_UTF8_PART2_WIDTH, FONT_5X8_UTF8_PART2_HEIGHT, GRAPHICS_FORMAT_BW_1_V)
};
static font_t font5x8 = make_font(font_5x8_utf8_bitmaps, 3, 5, 8, 1, 0);

const font_bitmap_t font_10x16_utf8_bitmaps[] = {
    make_font_bitmap(32, 127, font_10x16_utf8_part0_data, FONT_10X16_UTF8_PART0_WIDTH, FONT_10X16_UTF8_PART0_HEIGHT, GRAPHICS_FORMAT_BW_1_V),
    make_font_bitmap(0xb0, 0xb0, font_10x16_utf8_part1_data, FONT_10X16_UTF8_PART1_WIDTH, FONT_10X16_UTF8_PART1_HEIGHT, GRAPHICS_FORMAT_BW_1_V),
    make_font_bitmap(0x400, 0x451, font_10x16_utf8_part2_data, FONT_10X16_UTF8_PART2_WIDTH, FONT_10X16_UTF8_PART2_HEIGHT, GRAPHICS_FORMAT_BW_1_V)
};
static font_t font10x16 = make_font(font_10x16_utf8_bitmaps, 3, 10, 16, 1, 0);

#ifdef WORK_RELAY
#define TFT_DC_GPIO		GPIOB
#define TFT_DC_PIN		GPIO_Pin_4
#define TFT_CE_GPIO		GPIOA//
#define TFT_CE_PIN		GPIO_Pin_15//
#define TFT_RST_GPIO            GPIOC
#define TFT_RST_PIN		GPIO_Pin_12//10
#endif


/******************************************************************************/
#define TARGET_ANGLE 5000
#define ADC_CH          4  //количество каналов

uint8_t value=0, value1=0, value2=0, PCA_interrupt_flag = DISABLE;
uint8_t nul_A = 0, nul_B = 0, nul_C = 0, impuls_timer_2 = 0, impuls_timer_3 = 0;
uint16_t time_A, time_B, time_C;

#define ADC1_DR_Address    ((uint32_t)0x4001244C)
//! Длина буфера для DMA.
#define ADC_RAW_BUFFER_LEN 4
#define ADC_RAW_BUFFER_LN 8
//! Буфер ADC.
static volatile uint16_t adc_raw_buffer[ADC_RAW_BUFFER_LN] = {0};
    
//static DriveState_t DriveState;
/******************************************************************************/


// fucking spi remap && i2c smbus
/* удалить
   static void i2c_io_begin(void)
{
    spi_bus_wait(&spi);
    SPI_Cmd(SPI1, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    I2C_Cmd(I2C1, ENABLE);
}


static void spi_io_begin(void)
{
    i2c_bus_wait(&i2c);
    I2C_Cmd(I2C1, DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    SPI_Cmd(SPI1, ENABLE);
}
*/

void USART1_IRQHandler(void)
{
    usart_buf_irq_handler(&usart_buf);
}

void SysTick_Handler(void)
{
    system_counter_tick();
}

void SPI1_IRQHandler(void)
{
    spi_bus_irq_handler(&spi);
}

void DMA1_Channel2_IRQHandler(void)
{
    spi_bus_dma_rx_channel_irq_handler(&spi);
}

void DMA1_Channel3_IRQHandler(void)
{
    spi_bus_dma_tx_channel_irq_handler(&spi);
}

void DMA1_Channel6_IRQHandler(void)
{
    i2c_bus_dma_tx_channel_irq_handler(&i2c);
}

void DMA1_Channel7_IRQHandler(void)
{
    i2c_bus_dma_rx_channel_irq_handler(&i2c);
}

void I2C1_EV_IRQHandler(void)
{
    i2c_bus_event_irq_handler(&i2c);
}

void I2C1_ER_IRQHandler(void)
{
    i2c_bus_error_irq_handler(&i2c);
}

/******************************************************************************/
/****************************************************************************************/
/***********	Прерывание от регистра PCA9555 по изменению состояния кнопок	*********/
void EXTI9_5_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
		PCA_interrupt_flag = ENABLE;						// Флаг прерывания по нажатию кнопки
	EXTI_ClearITPendingBit(EXTI_Line7);						// очищаем флаг прерывания 7
    }

}

/****************************************************************************************/
/***************	Прерывание по датчикам нуля		*************************************/
void EXTI15_10_IRQHandler(void)
{

	if (EXTI_GetITStatus(EXTI_Line13) != RESET)
    {
		time_A = interrupt_time_count();						// получаем значение со счетчика и управляем счетным таймером
		phase_state_handle(PHASE_A);				// детектируем срабатывание фазы
		nul_A = phase_state_drive_direction();		// получаем направление вращения
		//open_tiristor(alfa_pid);					// открываем тиристор с углом альфа и последующей подачей импульсов на вторую пару
		if (impuls_timer_2 == 0) {
			impuls_timer_2 = 11;
			TIM_SetCounter(TIM2, TARGET_ANGLE);			// Устанавливаем счетный регистр в значение.
			TIM_Cmd(TIM2, ENABLE);   					// Начать отсчёт.
		}
		EXTI_ClearFlag(EXTI_Line13);				// очищаем флаг прерывания 13
    }

	if (EXTI_GetITStatus(EXTI_Line14) != RESET)
    {
		time_B = interrupt_time_count();
		phase_state_handle(PHASE_B);
		nul_A = phase_state_drive_direction();
		if (impuls_timer_2 == 0) {
			impuls_timer_2 = 21;
			TIM_SetCounter(TIM2, TARGET_ANGLE);			// Устанавливаем счетный регистр в значение.
			TIM_Cmd(TIM2, ENABLE);   					// Начать отсчёт.
		}
		EXTI_ClearFlag(EXTI_Line14);				// очищаем флаг прерывания 14
    }

	if (EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
		time_C = interrupt_time_count();
		phase_state_handle(PHASE_C);
		nul_A = phase_state_drive_direction();
		if (impuls_timer_2 == 0) {
			impuls_timer_2 = 31;
			TIM_SetCounter(TIM2, TARGET_ANGLE);			// Устанавливаем счетный регистр в значение.
			TIM_Cmd(TIM2, ENABLE);   					// Начать отсчёт.
		}
		EXTI_ClearFlag(EXTI_Line15);				// очищаем флаг прерывания 15
    }
}

// Timer2 подача импульсов на верхнее плечо
void TIM2_IRQHandler(void)
{
	// Если прерывание произошло успешно то делаем следующее
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		//if (DriveState==DriveReadyForw){
        First_Pulse_Sequence();
			
		//}
/*
		if (DriveState==DriveReadyBack){
			if (impuls_timer_2 == 11) {
				THYRISTOR_VS5_ON; THYRISTOR_VS2_ON;
				TIM_SetCounter(TIM2, THYRISTOR_IMPULS_TIME);		// Выставляем угол открытия тиристора.
				TIM_Cmd(TIM2, ENABLE);								// Старт таймера.
				impuls_timer_2 = 12;
			} else if (impuls_timer_2 == 12) {
				THYRISTOR_VS5_OFF; THYRISTOR_VS2_OFF;
				TIM_SetCounter(TIM3, THYRISTOR_ON_INTERVAL);		// Выставляем угол открытия тиристора 60гр
				TIM_Cmd(TIM3, ENABLE);
				impuls_timer_2 = 0;
				impuls_timer_3 = 13;
			} else if (impuls_timer_2 == 21) {
				THYRISTOR_VS3_ON; THYRISTOR_VS6_ON;
				TIM_SetCounter(TIM2, THYRISTOR_IMPULS_TIME);		// Выставляем угол открытия тиристора
				TIM_Cmd(TIM2, ENABLE);
				impuls_timer_2 = 22;
			} else if (impuls_timer_2 == 22) {
				THYRISTOR_VS3_OFF; THYRISTOR_VS6_OFF;
				TIM_SetCounter(TIM3, THYRISTOR_ON_INTERVAL);		// Выставляем угол открытия тиристора 60гр
				TIM_Cmd(TIM3, ENABLE);
				impuls_timer_2 = 0;
				impuls_timer_3 = 23;
			} else if (impuls_timer_2 == 31) {
				THYRISTOR_VS1_ON; THYRISTOR_VS4_ON;
				TIM_SetCounter(TIM2, THYRISTOR_IMPULS_TIME);		// Выставляем угол открытия тиристора
				TIM_Cmd(TIM2, ENABLE);
				impuls_timer_2 = 32;
			} else if (impuls_timer_2 == 32) {
				THYRISTOR_VS1_OFF; THYRISTOR_VS4_OFF;
				TIM_SetCounter(TIM3, THYRISTOR_ON_INTERVAL);		// Выставляем угол открытия тиристора 60гр
				TIM_Cmd(TIM3, ENABLE);
				impuls_timer_2 = 0;
				impuls_timer_3 = 33;
			}
		}*/
		// Очищаем флаг прерывания UIF
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

// Timer3 подача импульсов на нижнее плечо
void TIM3_IRQHandler(void)
{
	// Если прерывание произошло успешно то делаем следующее
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		//if (DriveState==DriveReadyForw){
			Second_Pulse_Sequence();
		//}
		/*if (DriveState==DriveReadyBack){
			if (impuls_timer_3 == 13) {
				THYRISTOR_VS3_ON; THYRISTOR_VS2_ON;
				TIM_SetCounter(TIM3, THYRISTOR_IMPULS_TIME);		// Выставляем угол открытия тиристора
				TIM_Cmd(TIM3, ENABLE);
				impuls_timer_3 = 14;
			} else if (impuls_timer_3 == 14) {
				THYRISTOR_VS3_OFF; THYRISTOR_VS2_OFF;
				impuls_timer_3 = 0;
			} else if (impuls_timer_3 == 23) {
				THYRISTOR_VS1_ON; THYRISTOR_VS6_ON;
				TIM_SetCounter(TIM3, THYRISTOR_IMPULS_TIME);		// Выставляем угол открытия тиристора
				TIM_Cmd(TIM3, ENABLE);
				impuls_timer_3 = 24;
			} else if (impuls_timer_3 == 24) {
				THYRISTOR_VS1_OFF; THYRISTOR_VS6_OFF;
				impuls_timer_3 = 0;
			} else if (impuls_timer_3 == 33) {
				THYRISTOR_VS5_ON; THYRISTOR_VS4_ON;
				TIM_SetCounter(TIM3, THYRISTOR_IMPULS_TIME);		// Выставляем угол открытия тиристора
				TIM_Cmd(TIM3, ENABLE);
				impuls_timer_3 = 34;
			} else if (impuls_timer_3 == 34) {
				THYRISTOR_VS5_OFF; THYRISTOR_VS4_OFF;
				impuls_timer_3 = 0;
			}
		}*/
	// Очищаем флаг прерывания UIF
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
/******************************************************************************/




static bool i2c_callback(void)
{
    return pca9555_i2c_callback(&ioport);
}

static bool spi_callback(void)
{
    //printf("[SPI] callback\r\n");
    return tft9341_spi_callback(&tft);
}

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
    // USART.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    // DMA.
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // SPI.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    // I2C1.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    // TIM2.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	// Включаем тактирование General-purpose TIM2
    // TIM3.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	// Включаем тактирование General-purpose TIM3
    // TIM6.
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);	// Включаем тактирование Basic TIM6
    // SPI2.
    //RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);	// Запуск тактирования SPI2
}

static void init_usart(void)
{
    GPIO_InitTypeDef gpio_tx =
        {.GPIO_Pin = GPIO_Pin_9, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_AF_PP};
    GPIO_InitTypeDef gpio_rx =
        {.GPIO_Pin = GPIO_Pin_10, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_IN_FLOATING};
    GPIO_Init(GPIOA, &gpio_tx);
    GPIO_Init(GPIOA, &gpio_rx);
    
    USART_InitTypeDef usart_is =
        {.USART_BaudRate = 115200, .USART_WordLength = USART_WordLength_8b, .USART_StopBits = USART_StopBits_1,
         .USART_Parity = USART_Parity_No, .USART_Mode = USART_Mode_Rx | USART_Mode_Tx, .USART_HardwareFlowControl = USART_HardwareFlowControl_None};
    USART_Init(USART1, &usart_is);
    USART_Cmd(USART1, ENABLE);
    
    usart_buf_init_t usartb_is = {.usart = USART1,
         .write_buffer = usart_write_buffer, .write_buffer_size = USART_WRITE_BUFFER_SIZE,
         .read_buffer = usart_read_buffer, .read_buffer_size = USART_READ_BUFFER_SIZE};
    usart_buf_init(&usart_buf, &usartb_is);
    usart_setup_stdio(&usart_buf);
    
    NVIC_SetPriority(USART1_IRQn, 3);
    NVIC_EnableIRQ(USART1_IRQn);
}

static void init_spi(void)
{
    /*GPIO_InitTypeDef gpio_sck_mosi =
        {.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_AF_PP};
    GPIO_InitTypeDef gpio_miso =
        {.GPIO_Pin = GPIO_Pin_6, .GPIO_Speed = GPIO_Speed_50MHz, .GPIO_Mode = GPIO_Mode_IN_FLOATING};*/
    
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
    spi_is.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
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
    
    NVIC_SetPriority(SPI1_IRQn, 2);
    NVIC_EnableIRQ(SPI1_IRQn);
    
    NVIC_SetPriority(DMA1_Channel2_IRQn, 2);
    NVIC_SetPriority(DMA1_Channel3_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
}

static void init_i2c_periph(void)
{
    I2C_InitTypeDef i2c_is;
    i2c_is.I2C_Ack = I2C_Ack_Disable;
    i2c_is.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    i2c_is.I2C_OwnAddress1 = 0x11;
    i2c_is.I2C_ClockSpeed = 400000;
    i2c_is.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c_is.I2C_Mode = I2C_Mode_I2C;
    
    I2C_DeInit(I2C1);
    I2C_Init(I2C1, &i2c_is);
    I2C_StretchClockCmd(I2C1, ENABLE);
    I2C_Cmd(I2C1, ENABLE);
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
    
    NVIC_SetPriority(I2C1_EV_IRQn, 1);
    NVIC_SetPriority(I2C1_ER_IRQn, 1);
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    NVIC_EnableIRQ(I2C1_ER_IRQn);
    
    NVIC_SetPriority(DMA1_Channel6_IRQn, 2);
    NVIC_SetPriority(DMA1_Channel7_IRQn, 2);
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);
}

static void init_adc(void)
{

    GPIO_InitTypeDef gpio_adcA =
        {.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2, .GPIO_Speed = GPIO_Speed_2MHz, .GPIO_Mode = GPIO_Mode_AIN};
    GPIO_InitTypeDef gpio_adcB =
        {.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1, .GPIO_Speed = GPIO_Speed_2MHz, .GPIO_Mode = GPIO_Mode_AIN};
    GPIO_InitTypeDef gpio_adcC =
        {.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5, .GPIO_Speed = GPIO_Speed_2MHz, .GPIO_Mode = GPIO_Mode_AIN};
    GPIO_Init(GPIOB, &gpio_adcA);
    GPIO_Init(GPIOB, &gpio_adcB);
    GPIO_Init(GPIOC, &gpio_adcC);

    
    
     dma_channel_lock(DMA1_Channel1);

    DMA_InitTypeDef dma_is;     //Variable used to setup the DMA               
    DMA_StructInit(&dma_is);
       dma_is.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR; // ADC1_DR_Address;
       dma_is.DMA_MemoryBaseAddr = (uint32_t)adc_raw_buffer; //Variable to which ADC values will be stored
       dma_is.DMA_DIR = DMA_DIR_PeripheralSRC;
       dma_is.DMA_BufferSize = ADC_RAW_BUFFER_LEN; // ADC_CH; //Buffer size (8 because we using 8 channels)
       dma_is.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
       dma_is.DMA_MemoryInc = DMA_MemoryInc_Enable;
       dma_is.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
       dma_is.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
       dma_is.DMA_Mode = DMA_Mode_Circular;
       dma_is.DMA_Priority = DMA_Priority_Medium;
       dma_is.DMA_M2M = DMA_M2M_Disable;
       //DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);
    DMA_Init(DMA1_Channel1, &dma_is); //Initialise the DMA
        
    ADC_InitTypeDef adc1_is;
    ADC_StructInit(&adc1_is);
        //Настройка параметров  ADC1
        adc1_is.ADC_Mode = ADC_Mode_RegSimult;
        adc1_is.ADC_ScanConvMode = ENABLE;
        adc1_is.ADC_ContinuousConvMode = ENABLE;
        adc1_is.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC3;
        adc1_is.ADC_DataAlign = ADC_DataAlign_Right;
        adc1_is.ADC_NbrOfChannel = ADC_RAW_BUFFER_LEN; //We using 3 channels
    ADC_Init(ADC1, &adc1_is); //Initialise ADC1
    
    ADC_InitTypeDef adc2_is;
    ADC_StructInit(&adc2_is);
        //Настройка параметров  ADC1
        adc2_is.ADC_Mode = ADC_Mode_RegSimult;
        adc2_is.ADC_ScanConvMode = ENABLE;
        adc2_is.ADC_ContinuousConvMode = ENABLE;
        adc2_is.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
        adc2_is.ADC_DataAlign = ADC_DataAlign_Right;
        adc2_is.ADC_NbrOfChannel = ADC_RAW_BUFFER_LEN; //We using 3 channels
    ADC_Init(ADC2, &adc2_is); //Initialise ADC1
    
    //Порядок оцифровки
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 4, ADC_SampleTime_55Cycles5);
    
    //Порядок оцифровки
    ADC_RegularChannelConfig(ADC2, ADC_Channel_12, 1, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_13, 2, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_0, 3, ADC_SampleTime_55Cycles5);
    ADC_RegularChannelConfig(ADC2, ADC_Channel_2, 4, ADC_SampleTime_55Cycles5);
    
    ADC_DMACmd(ADC1, ENABLE); //Enable ADC1 DMA
    DMA_Cmd(DMA1_Channel1, ENABLE);

    //ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

    ADC_Cmd(ADC1, ENABLE); //Enable ADC1 
    ADC_Cmd(ADC2, ENABLE); //Enable ADC1 
    
    //Калибровка ADC1
    //Enable ADC1 reset calibaration register
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1)); //Check the end of ADC1 reset calibration register
    ADC_ResetCalibration(ADC2);
    while (ADC_GetResetCalibrationStatus(ADC2)); //Check the end of ADC1 reset calibration register
    //Start ADC1 calibaration
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1)); //Check the end of ADC1 calibration
    ADC_StartCalibration(ADC2);
    while (ADC_GetCalibrationStatus(ADC2)); //Check the end of ADC1 calibration
    //NVIC_SetPriority(ADC1_2_IRQn, 1);
    //NVIC_EnableIRQ(ADC1_2_IRQn);

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    ADC_SoftwareStartConvCmd(ADC2, ENABLE);
}

static void init_ioport(void)
{
    //удалить i2c_io_begin();
    pca9555_init(&ioport, &i2c, PCA9555_I2C_DEFAULT_ADDRESS);
    pca9555_set_pins_direction(&ioport, PCA9555_PIN_ALL, PCA9555_PIN_INPUT);
    pca9555_set_pins_direction(&ioport, 
            PCA9555_PIN_1 | PCA9555_PIN_3 | PCA9555_PIN_4 |
            PCA9555_PIN_5 | PCA9555_PIN_6 | PCA9555_PIN_7
            , PCA9555_PIN_OUTPUT);
    pca9555_set_pins_direction(&ioport, TFT_IOPIN, PCA9555_PIN_OUTPUT);
    pca9555_write_pins_direction(&ioport);
    pca9555_set_pins_state(&ioport, PCA9555_PIN_ALL, PCA9555_PIN_ON);
    /*pca9555_set_pins_state(&ioport, 
            PCA9555_PIN_8 | PCA9555_PIN_9 | PCA9555_PIN_10 |
            PCA9555_PIN_11 | PCA9555_PIN_12 | PCA9555_PIN_13
            , PCA9555_PIN_OFF);*/
    pca9555_set_pins_state(&ioport, 
            PCA9555_PIN_5, PCA9555_PIN_OFF);
    pca9555_write_pins_state(&ioport);
}

static void ioport_next_leds(void)
{
    //удалить i2c_io_begin();
    pca9555_pins_t on_pins = pca9555_pins_output_state(&ioport, PCA9555_PIN_OFF);
    
    if(on_pins & PCA9555_PIN_7){
        on_pins = PCA9555_PIN_3;
    }else{
        on_pins <<= 1;
    }
    pca9555_set_pins_state(&ioport, PCA9555_PIN_ALL, PCA9555_PIN_ON);
    pca9555_set_pins_state(&ioport, on_pins, PCA9555_PIN_OFF);
    pca9555_write_pins_state(&ioport);
    //pca9555_wait(&ioport);
}

static void ioport_reset_tft(void)
{
    //удалить i2c_io_begin();
    pca9555_set_pins_state(&ioport, TFT_IOPIN, PCA9555_PIN_OFF);
    pca9555_write_pins_state(&ioport);
    delay_ms(10);
    pca9555_set_pins_state(&ioport, TFT_IOPIN, PCA9555_PIN_ON);
    pca9555_write_pins_state(&ioport);
    pca9555_wait(&ioport);
    delay_ms(150);
}

static void init_tft(void)
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
    
    //tft9341_reset(&tft);
    ioport_reset_tft();
    
    // удалить spi_io_begin();
    
    tft9341_madctl_t madctl;
    madctl.row_address_order = TFT9341_ROW_BOTTOM_TO_TOP;//TFT9341_ROW_TOP_TO_BOTTOM;
    madctl.col_address_order = TFT9341_COL_RIGHT_TO_LEFT;//TFT9341_COL_LEFT_TO_RIGHT;
    madctl.row_col_exchange = TFT9341_ROW_COL_REVERSE_MODE;
    madctl.vertical_refresh = TFT9341_REFRESH_TOP_TO_BOTTOM;
    madctl.color_order = TFT9341_COLOR_ORDER_BGR;
    madctl.horizontal_refresh = TFT9341_REFRESH_LEFT_TO_RIGHT;
    
    tft9341_set_madctl(&tft, &madctl);
    tft9341_set_pixel_format(&tft, TFT9341_PIXEL_16BIT, TFT9341_PIXEL_16BIT);
    tft9341_sleep_out(&tft);
    tft9341_display_on(&tft);
}
/******************************************************************************/
static void init_tim1(void)
{
    TIM_TimeBaseInitTypeDef TIM1_InitStructure;
            TIM1_InitStructure.TIM_Prescaler = 7200-1;					// Делитель (0000...FFFF)
            TIM1_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;	// Режим счетчика
            TIM1_InitStructure.TIM_Period = 10000;						// Значение периода (0000...FFFF)
            TIM1_InitStructure.TIM_ClockDivision = 0;					// определяет тактовое деление
    TIM_TimeBaseInit(TIM1, &TIM1_InitStructure);
    //TIM_SelectOnePulseMode(TIM1, TIM_OPMode_Single);				// Однопульсный режим таймера
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);						// Разрешаем прерывание от таймера
    TIM_Cmd(TIM1, ENABLE);                                                      // Начать отсчёт!

    TIM_OCInitTypeDef tim1_is;
        tim1_is.TIM_OCMode = 0;
        tim1_is.TIM_OutputState = 0;
        tim1_is.TIM_OutputNState = 0;
        tim1_is.TIM_Pulse = 0;
        tim1_is.TIM_OCPolarity = 0;
        tim1_is.TIM_OCNPolarity = 0;
        tim1_is.TIM_OCIdleState = 0;
        tim1_is.TIM_OCNIdleState = 0;
    TIM_OCInit(TIM1, &tim1_is);
    TIM_CCxCmd (TIM1, TIM_Channel_3, TIM_CCx_Enable);
    //NVIC_SetPriority(TIM1_IRQn, 1);
    //NVIC_EnableIRQ (TIM1_IRQn); 		// Разрешаем прерывания по Таймеру2
}

static void init_tim2(void)
{
    TIM_TimeBaseInitTypeDef TIM2_InitStructure;
            TIM2_InitStructure.TIM_Prescaler = 72-1;					// Делитель (0000...FFFF)
            TIM2_InitStructure.TIM_CounterMode = TIM_CounterMode_Down;	// Режим счетчика
            TIM2_InitStructure.TIM_Period = 60000;						// Значение периода (0000...FFFF)
            TIM2_InitStructure.TIM_ClockDivision = 0;					// определяет тактовое деление
    TIM_TimeBaseInit(TIM2, &TIM2_InitStructure);
    TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Single);				// Однопульсный режим таймера
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);						// Разрешаем прерывание от таймера
    //TIM_Cmd(TIM2, ENABLE);   										// Начать отсчёт!
    
    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ (TIM2_IRQn); 		// Разрешаем прерывания по Таймеру2
}

static void init_tim3(void)
{
    TIM_TimeBaseInitTypeDef TIM3_InitStructure;
            TIM3_InitStructure.TIM_Prescaler = 72-1;                        // Делитель (0000...FFFF)
            TIM3_InitStructure.TIM_CounterMode = TIM_CounterMode_Down;      // Режим счетчика
            TIM3_InitStructure.TIM_Period = 60000;                          // Значение периода (0000...FFFF)
            TIM3_InitStructure.TIM_ClockDivision = 0;                       // определяет тактовое деление
    TIM_TimeBaseInit(TIM3, &TIM3_InitStructure);
    TIM_SelectOnePulseMode(TIM3, TIM_OPMode_Single);                        // Однопульсный режим таймера
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);                              // Разрешаем прерывание от таймера
    //TIM_Cmd(TIM3, ENABLE);                                                // Начать отсчёт!    
    
    NVIC_SetPriority(TIM3_IRQn, 1);
    NVIC_EnableIRQ (TIM3_IRQn); 		// Разрешаем прерывания по Таймеру3
}

static void init_tim6(void)
{
    TIM_TimeBaseInitTypeDef TIM6_InitStructure;
            TIM6_InitStructure.TIM_Prescaler = 72-1;			// Настраиваем делитель чтобы таймер тикал 1 000 000 раз в секунду
            TIM6_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;	// Режим счетчика
            TIM6_InitStructure.TIM_Period = 60000;			// Значение периода (0000...FFFF)
            TIM6_InitStructure.TIM_ClockDivision = 0;			// определяет тактовое деление
    TIM_TimeBaseInit(TIM6, &TIM6_InitStructure);
    TIM_SelectOnePulseMode(TIM6, TIM_OPMode_Single);			// Однопульсный режим таймера
    TIM_SetCounter(TIM6, 0);						// Сбрасываем счетный регистр в ноль
    //TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);			// Разрешаем прерывание от таймера
    //TIM_Cmd(TIM6, ENABLE);   						// Начать отсчёт!    
}

static void init_exti(void)
{
/**********************************************************************************************************/
/*******************	PCA9555: I2c1 and Interrupt		***************************************************/
    GPIO_InitTypeDef GPIO_InitStructure;
    /* GPIOB Configuration: 95 (PD7 - Int_PCA9555) as input pull-down */
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
        
        
/**********************************************************************************************************/
/******************		Zero sensor		*******************************************************************/
    /* GPIOB Configuration: 44 (PE13 - ZeroSensor_1); 45 (PE14 - ZeroSensor_2); 46 (PE15 - ZeroSensor_3) as input floating */
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
        
        
/**********************************************************************************************************/
/***************	Привязка Прерывания к портам МК		***************************************************/       
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource7);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource13);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource14);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource15);
    
    
    EXTI_InitTypeDef EXTI_InitStructure;
    EXTI_InitStructure.EXTI_Line = EXTI_Line7 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    
    NVIC_SetPriority(EXTI9_5_IRQn, 3);
    NVIC_SetPriority(EXTI15_10_IRQn, 2);
 
    NVIC_EnableIRQ (EXTI9_5_IRQn); 		// Разрешаем прерывание от 5-9 ног
    NVIC_EnableIRQ (EXTI15_10_IRQn); 	// Разрешаем прерывание от 10-15 ног
}

static void init_gpio (void)
{
/**********************************************************************************************************/
/******************		Digital Outputs	(Relay)		*******************************************************/
        GPIO_InitTypeDef GPIO_InitStructure;
	/* GPIOB Configuration: 7 (PC13 - DigitalOut_4) as output push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* GPIOE Configuration: 3 (PE4 - DigitalOut_3); 4 (PE5 - DigitalOut_2); 5 (PE6 - DigitalOut_1) as output push-pull */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	
/**********************************************************************************************************/
/******************		Digital Inputs	(24v)	***********************************************************/

	/* GPIOB Configuration: 96 (PB9 - DigitalIn_1) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* GPIOB Configuration: 97 (PE0 - DigitalIn_2); 98 (PE1 - DigitalIn_3);
	 * 						1 (PE2 - DigitalIn_4); 2 (PE3 - DigitalIn_5) 	as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);


/**********************************************************************************************************/
/******************		Thyristors and Triac	***********************************************************/

	/* GPIOB Configuration: 57 (PD10 - VS_1); 58 (PD11 - VS_2); 59 (PD12 - VS_3); as output open drain
	 * 						60 (PD13 - VS_4); 61 (PD14 - VS_5); 62 (PD15 - VS_6); as output open drain	*/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	/* GPIOB Configuration: 63 (PC6 - VS_xS) as output open drain */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);


/**********************************************************************************************************/
/*******************	Dip 8 switch	*******************************************************************/

	/* GPIOB Configuration: 37 (PB2 - Dip_1) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* GPIOB Configuration: 38 (PE7 - Dip_2);  39 (PE8 - Dip_3);  40 (PE9 - Dip_4);
	 * 						41 (PE10 - Dip_5); 42 (PE11 - Dip_6); 43 (PE12 - Dip_7)  as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* GPIOB Configuration: 65 (PC8 - Dip_8) as input floating */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/******************************************************************************/

#define MAKE_RGB(r, g, b) TFT9341_MAKE_RGB565(r, g, b)
#define THEME_COLOR_BACK MAKE_RGB(0xff, 0xff, 0xff)
#define THEME_COLOR_FRONT MAKE_RGB(0x00, 0x00, 0x00)
#define THEME_COLOR_PANEL MAKE_RGB(0xf8, 0xf8, 0xf8)//0xf0
#define THEME_COLOR_WIDGET MAKE_RGB(0xe1, 0xe1, 0xe1)
#define THEME_COLOR_BORDER MAKE_RGB(0xb1, 0xb1, 0xb1)
#define THEME_COLOR_FONT MAKE_RGB(0x00, 0x00, 0x00)
#define THEME_COLOR_FOCUS MAKE_RGB(0x00, 0x78, 0xd7)

static gui_theme_t theme = MAKE_GUI_THEME(
                    THEME_COLOR_BACK, THEME_COLOR_FRONT,
                    THEME_COLOR_PANEL, THEME_COLOR_WIDGET,
                    THEME_COLOR_BORDER, THEME_COLOR_FONT,
                    THEME_COLOR_FOCUS, &font5x8, &font10x16);
static gui_t gui = MAKE_GUI(&graphics, &theme);
// GUI.
static gui_widget_t root_widget;
static gui_widget_t parent_widget;
static gui_label_t label1;
static gui_label_t label11;
static gui_label_t label12;
static gui_label_t label13;
static gui_label_t label14;
static gui_label_t label21;
static gui_label_t label22;
static gui_label_t label23;
static gui_label_t label24;
static gui_number_label_t label_num;
static gui_number_label_t label_num11;
static gui_number_label_t label_num12;
static gui_number_label_t label_num13;
static gui_number_label_t label_num14;
static gui_number_label_t label_num21;
static gui_number_label_t label_num22;
static gui_number_label_t label_num23;
static gui_number_label_t label_num24;
static gui_checkbox_t checkbox1;
static gui_checkbox_t checkbox2;

static void make_gui_adc(void)
{
    gui_label_init_parent(&label11, &gui, &parent_widget);
    gui_label_set_text(&label11, "adc1_in1:");
    gui_widget_move(GUI_WIDGET(&label11), 120, 10);
    gui_widget_resize(GUI_WIDGET(&label11), 60, 20);
    gui_widget_set_border(GUI_WIDGET(&label11), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label11), true);
    
    gui_label_init_parent(&label12, &gui, &parent_widget);
    gui_label_set_text(&label12, "adc1_in2:");
    gui_widget_move(GUI_WIDGET(&label12), 120, 30);
    gui_widget_resize(GUI_WIDGET(&label12), 60, 20);
    gui_widget_set_border(GUI_WIDGET(&label12), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label12), true); 
    
    gui_label_init_parent(&label13, &gui, &parent_widget);
    gui_label_set_text(&label13, "adc1_in3:");
    gui_widget_move(GUI_WIDGET(&label13), 120, 50);
    gui_widget_resize(GUI_WIDGET(&label13), 60, 20);
    gui_widget_set_border(GUI_WIDGET(&label13), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label13), true);

    gui_label_init_parent(&label14, &gui, &parent_widget);
    gui_label_set_text(&label14, "adc1_in4:");
    gui_widget_move(GUI_WIDGET(&label14), 120, 70);
    gui_widget_resize(GUI_WIDGET(&label14), 60, 20);
    gui_widget_set_border(GUI_WIDGET(&label14), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label14), true);
    
    gui_label_init_parent(&label21, &gui, &parent_widget);
    gui_label_set_text(&label21, "adc2_in1:");
    gui_widget_move(GUI_WIDGET(&label21), 120, 110);
    gui_widget_resize(GUI_WIDGET(&label21), 60, 20);
    gui_widget_set_border(GUI_WIDGET(&label21), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label21), true);
    
    gui_label_init_parent(&label22, &gui, &parent_widget);
    gui_label_set_text(&label22, "adc2_in2:");
    gui_widget_move(GUI_WIDGET(&label22), 120, 130);
    gui_widget_resize(GUI_WIDGET(&label22), 60, 20);
    gui_widget_set_border(GUI_WIDGET(&label22), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label22), true);
    
    gui_label_init_parent(&label23, &gui, &parent_widget);
    gui_label_set_text(&label23, "adc2_in3:");
    gui_widget_move(GUI_WIDGET(&label23), 120, 150);
    gui_widget_resize(GUI_WIDGET(&label23), 60, 20);
    gui_widget_set_border(GUI_WIDGET(&label23), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label23), true);

    gui_label_init_parent(&label24, &gui, &parent_widget);
    gui_label_set_text(&label24, "adc2_in4:");
    gui_widget_move(GUI_WIDGET(&label24), 120, 170);
    gui_widget_resize(GUI_WIDGET(&label24), 60, 20);
    gui_widget_set_border(GUI_WIDGET(&label24), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label24), true);
    
    gui_number_label_init_parent(&label_num11, &gui, &parent_widget);
    gui_number_label_set_number(&label_num11, 1);//0x1234
    gui_number_label_set_format(&label_num11, GUI_NUMBER_LABEL_DEC);
    gui_widget_move(GUI_WIDGET(&label_num11), 180, 10);
    gui_widget_resize(GUI_WIDGET(&label_num11), 50, 20);
    gui_widget_set_border(GUI_WIDGET(&label_num11), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label_num11), true);
    
    gui_number_label_init_parent(&label_num12, &gui, &parent_widget);
    gui_number_label_set_number(&label_num12, 2);//0x1234
    gui_number_label_set_format(&label_num12, GUI_NUMBER_LABEL_DEC);
    gui_widget_move(GUI_WIDGET(&label_num12), 180, 30);
    gui_widget_resize(GUI_WIDGET(&label_num12), 50, 20);
    gui_widget_set_border(GUI_WIDGET(&label_num12), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label_num12), true);
    
    gui_number_label_init_parent(&label_num13, &gui, &parent_widget);
    gui_number_label_set_number(&label_num13, 3);//0x1234
    gui_number_label_set_format(&label_num13, GUI_NUMBER_LABEL_DEC);
    gui_widget_move(GUI_WIDGET(&label_num13), 180, 50);
    gui_widget_resize(GUI_WIDGET(&label_num13), 50, 20);
    gui_widget_set_border(GUI_WIDGET(&label_num13), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label_num13), true);
    
    gui_number_label_init_parent(&label_num14, &gui, &parent_widget);
    gui_number_label_set_number(&label_num14, 3);//0x1234
    gui_number_label_set_format(&label_num14, GUI_NUMBER_LABEL_DEC);
    gui_widget_move(GUI_WIDGET(&label_num14), 180, 70);
    gui_widget_resize(GUI_WIDGET(&label_num14), 50, 20);
    gui_widget_set_border(GUI_WIDGET(&label_num14), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label_num14), true);
    
    gui_number_label_init_parent(&label_num21, &gui, &parent_widget);
    gui_number_label_set_number(&label_num21, 4);//0x1234
    gui_number_label_set_format(&label_num21, GUI_NUMBER_LABEL_DEC);
    gui_widget_move(GUI_WIDGET(&label_num21), 180, 110);
    gui_widget_resize(GUI_WIDGET(&label_num21), 50, 20);
    gui_widget_set_border(GUI_WIDGET(&label_num21), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label_num21), true);
    
    gui_number_label_init_parent(&label_num22, &gui, &parent_widget);
    gui_number_label_set_number(&label_num22, 5);//0x1234
    gui_number_label_set_format(&label_num22, GUI_NUMBER_LABEL_DEC);
    gui_widget_move(GUI_WIDGET(&label_num22), 180, 130);
    gui_widget_resize(GUI_WIDGET(&label_num22), 50, 20);
    gui_widget_set_border(GUI_WIDGET(&label_num22), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label_num22), true);
    
    gui_number_label_init_parent(&label_num23, &gui, &parent_widget);
    gui_number_label_set_number(&label_num23, 6);//0x1234
    gui_number_label_set_format(&label_num23, GUI_NUMBER_LABEL_DEC);
    gui_widget_move(GUI_WIDGET(&label_num23), 180, 150);
    gui_widget_resize(GUI_WIDGET(&label_num23), 50, 20);
    gui_widget_set_border(GUI_WIDGET(&label_num23), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label_num23), true);
 
    gui_number_label_init_parent(&label_num24, &gui, &parent_widget);
    gui_number_label_set_number(&label_num24, 6);//0x1234
    gui_number_label_set_format(&label_num24, GUI_NUMBER_LABEL_DEC);
    gui_widget_move(GUI_WIDGET(&label_num24), 180, 170);
    gui_widget_resize(GUI_WIDGET(&label_num24), 50, 20);
    gui_widget_set_border(GUI_WIDGET(&label_num24), GUI_BORDER_SOLID);
    gui_widget_set_visible(GUI_WIDGET(&label_num24), true);
    
}

static void make_gui(void)
{
    gui_widget_init(&root_widget, &gui);
    gui_widget_move(&root_widget, 0, 0);
    gui_widget_resize(&root_widget, TFT_WIDTH, TFT_HEIGHT);
    
    gui_widget_init_parent(&parent_widget, &gui, &root_widget);
    gui_widget_move(&parent_widget, 10, 10);
    gui_widget_resize(&parent_widget, 300, 220);
    gui_widget_set_border(&parent_widget, GUI_BORDER_SOLID);
    gui_widget_set_visible(&parent_widget, true);
    
    gui_label_init_parent(&label1, &gui, &parent_widget);
    gui_label_set_text(&label1, "Метка:");
    gui_widget_move(GUI_WIDGET(&label1), 10, 10);
    gui_widget_resize(GUI_WIDGET(&label1), 50, 20);
    gui_widget_set_border(GUI_WIDGET(&label1), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&label2), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&label1), true);
    
    gui_number_label_init_parent(&label_num, &gui, &parent_widget);
    gui_number_label_set_number(&label_num, 0);//0x1234
    gui_number_label_set_format(&label_num, GUI_NUMBER_LABEL_DEC);
    gui_widget_move(GUI_WIDGET(&label_num), 60, 10);
    gui_widget_resize(GUI_WIDGET(&label_num), 50, 20);
    gui_widget_set_border(GUI_WIDGET(&label_num), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&label3), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&label_num), true);
    
    make_gui_adc();
            
    gui_checkbox_init_parent(&checkbox1, &gui, &parent_widget);
    gui_checkbox_set_text(&checkbox1, "Флажок 0");
    gui_checkbox_set_checked(&checkbox1, false);
    gui_widget_move(GUI_WIDGET(&checkbox1), 10, 40);
    gui_widget_resize(GUI_WIDGET(&checkbox1), 70, 20);
    gui_widget_set_border(GUI_WIDGET(&checkbox1), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&checkbox1), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&checkbox1), true);
    
    gui_checkbox_init_parent(&checkbox2, &gui, &parent_widget);
    gui_checkbox_set_text(&checkbox2, "Флажок 1");
    gui_checkbox_set_checked(&checkbox2, true);
    gui_widget_move(GUI_WIDGET(&checkbox2), 10, 70);
    gui_widget_resize(GUI_WIDGET(&checkbox2), 70, 20);
    gui_widget_set_border(GUI_WIDGET(&checkbox2), GUI_BORDER_SOLID);
    //gui_widget_set_back_color(GUI_WIDGET(&checkbox2), THEME_COLOR_WIDGET);
    gui_widget_set_visible(GUI_WIDGET(&checkbox2), true);
    
    //gui_set_focus_widget(&gui, GUI_WIDGET(&label2));
    gui_set_root_widget(&gui, &root_widget);

    gui_widget_set_visible(&root_widget, true);
    //gui_focus_next_widget(&gui);
    //gui_focus_prev_widget(&gui);
    //gui_focus_next_widget(&gui);
    
    gui_widget_t* widget = gui_widget_from_point(&gui, 30, 80);
    gui_set_focus_widget(&gui, widget);
}

static void gui_iter(void)
{
    int secs = counter / 1000;
    gui_number_label_set_number(&label_num, secs);
    gui_checkbox_set_checked(&checkbox1, secs & 0x1);
    gui_checkbox_set_checked(&checkbox2, !gui_checkbox_checked(&checkbox1));
}

static void screen_clear(void)
{
    // удалить spi_io_begin();
    
    make_gui();
}

static void screen_repaint(void)
{
    // удалить spi_io_begin();
    
    gui_iter();
}



int main(void)
{
    NVIC_SetPriorityGrouping(0x3);
    
    init_sys_counter();
    init_periph_clock();
    remap_config();
    init_usart();
    init_tim1();
    init_tim2();
    init_tim3();
    init_tim6();
    init_exti();
    init_gpio();
    init_adc();    
    printf("STM32 MCU\r\n");
    
    init_i2c();
    init_spi();
    init_ioport();
    init_tft();
    
    screen_clear();
    
    screen_repaint();
    
    for(;;){
        
        if(system_counter_diff(&counter) >= system_counter_ticks_per_sec()){
        //if(need_update){
            //need_update = false;
            counter = system_counter_ticks();
            
            printf("%d\r\n", ((int)counter));
            
            screen_repaint();
            
            ioport_next_leds();
            

            
        }
            gui_number_label_set_number(&label_num11, adc_raw_buffer[0]);
            gui_number_label_set_number(&label_num12, adc_raw_buffer[2]);
            gui_number_label_set_number(&label_num13, adc_raw_buffer[4]);
            gui_number_label_set_number(&label_num14, adc_raw_buffer[6]);
            gui_number_label_set_number(&label_num21, adc_raw_buffer[1]);
            gui_number_label_set_number(&label_num22, adc_raw_buffer[3]);
            gui_number_label_set_number(&label_num23, adc_raw_buffer[5]);
            gui_number_label_set_number(&label_num24, adc_raw_buffer[7]);
    }
    return 0;
}
