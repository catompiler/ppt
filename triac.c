#include "triac.h"
#include <stddef.h>



err_t triac_init(triac_t* triac, GPIO_TypeDef* GPIO, uint16_t pin)
{
    // Проверка на степень двойки (только один пин в маске).
    if((pin & (pin - 1)) != 0) return E_INVALID_VALUE;
    
    triac->gpio = GPIO;
    triac->pin = pin;
    
    return E_NO_ERROR;
}

err_t triac_init_gpio(triac_t* triac, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed)
{
    if(triac->gpio == NULL || triac->pin == 0) return E_INVALID_VALUE;
    
    GPIO_InitTypeDef gpio_is;
    
    gpio_is.GPIO_Pin = triac->pin;
    gpio_is.GPIO_Mode = mode;
    gpio_is.GPIO_Speed = speed;
    
    GPIO_Init(triac->gpio, &gpio_is);
    
    return E_NO_ERROR;
}
