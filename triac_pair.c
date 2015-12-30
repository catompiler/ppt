#include "triac_pair.h"
#include "utils/utils.h"



err_t triac_pair_init(triac_pair_t* triac_pair, GPIO_TypeDef* GPIO_a, uint16_t pin_a,
                                                  GPIO_TypeDef* GPIO_b, uint16_t pin_b)
{
    RETURN_ERR_IF_FAIL(triac_init(&triac_pair->triac_a, GPIO_a, pin_a));
    RETURN_ERR_IF_FAIL(triac_init(&triac_pair->triac_b, GPIO_b, pin_b));
    
    return E_NO_ERROR;
}

err_t triac_pair_init_gpio(triac_pair_t* triac_pair, GPIOMode_TypeDef mode, GPIOSpeed_TypeDef speed)
{
    RETURN_ERR_IF_FAIL(triac_init_gpio(&triac_pair->triac_a, mode, speed));
    RETURN_ERR_IF_FAIL(triac_init_gpio(&triac_pair->triac_b, mode, speed));
    
    return E_NO_ERROR;
}

