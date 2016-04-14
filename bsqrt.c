#include "bsqrt.h"


uint32_t bsqrti(uint32_t value)
{
    uint32_t rem = 0;
    uint32_t root = 0;
    
    uint32_t i = 0;
    
    for(; i < 16; i ++){
        root <<= 1;
        rem = ((rem << 2) + (value >> 30));
        value <<= 2;
        root ++;
        if(root <= rem){
            rem -= root;
            root ++;
        }else{
            root --;
        }
    }
    return root >> 1;
}

fixed32_t bsqrtf(fixed32_t value)
{
    uint32_t rem = 0;
    uint32_t root = 0;
    
    uint32_t i = 0;
    
    for(; i < 16; i ++){
        root <<= 1;
        rem = ((rem << 2) + ((uint32_t)value >> 30));
        value <<= 2;
        root ++;
        if(root <= rem){
            rem -= root;
            root ++;
        }else{
            root --;
        }
    }
    return (fixed32_t)(root << 7);
}
