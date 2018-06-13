#include "drive_dip.h"
#include <stm32f10x.h>
#include <string.h>
#include "bits/bits.h"
#include "defs/defs.h"


// Определения портов и пинов переключателя.
// DIP0.
#define DIP0_PORT GPIOB
#define DIP0_PIN BIT(2)
// DIP1.
#define DIP1_PORT GPIOE
#define DIP1_PIN BIT(7)
// DIP2.
#define DIP2_PORT GPIOE
#define DIP2_PIN BIT(8)
// DIP3.
#define DIP3_PORT GPIOE
#define DIP3_PIN BIT(9)
// DIP4.
#define DIP4_PORT GPIOE
#define DIP4_PIN BIT(10)
// DIP5.
#define DIP5_PORT GPIOE
#define DIP5_PIN BIT(11)
// DIP6.
#define DIP6_PORT GPIOE
#define DIP6_PIN BIT(12)
// DIP7.
//#define DIP7_PORT GPIOC
//#define DIP7_PIN BIT(8)


//! Структура DIP-переключателя.
typedef struct _Dip {
    uint8_t data;
} dip_t;

//! Переключатель.
static dip_t dip;


void drive_dip_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPEEN;
    memset(&dip, 0x0, sizeof(dip_t));
}

ALWAYS_INLINE static bool dip_pin_value(GPIO_TypeDef* port, uint16_t pin)
{
    return (port->IDR & pin) != 0;
}

void drive_dip_update(void)
{
    dip.data  = dip_pin_value(DIP0_PORT, DIP0_PIN);
    dip.data |= dip_pin_value(DIP1_PORT, DIP1_PIN) << 1;
    dip.data |= dip_pin_value(DIP2_PORT, DIP2_PIN) << 2;
    dip.data |= dip_pin_value(DIP3_PORT, DIP3_PIN) << 3;
    dip.data |= dip_pin_value(DIP4_PORT, DIP4_PIN) << 4;
    dip.data |= dip_pin_value(DIP5_PORT, DIP5_PIN) << 5;
    dip.data |= dip_pin_value(DIP6_PORT, DIP6_PIN) << 6;
}

bool drive_dip_pin_state(drive_dip_pin_t pin)
{
    return (dip.data & (uint8_t)pin) != 0;
}
