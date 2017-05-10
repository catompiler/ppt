#include "nvdata.h"
#include "utils/utils.h"
#include <stm32f10x.h>
#include <stdbool.h>


#define NVDATA_BKP_REGS_COUNT (NVDATA_SIZE / 2)

static const uint8_t nvdata_bkp_regs_table [NVDATA_BKP_REGS_COUNT] = {
    BKP_DR1,  BKP_DR2,  BKP_DR3,  BKP_DR4,  BKP_DR5,  BKP_DR6,  BKP_DR7,  BKP_DR8,  BKP_DR9,  BKP_DR10,
#if defined(STM32F10X_HD) || defined(STM32F10X_HD_VL) ||\
    defined(STM32F10X_XL) || defined(STM32F10X_CL)
    BKP_DR11, BKP_DR12, BKP_DR13, BKP_DR14, BKP_DR15, BKP_DR16, BKP_DR17, BKP_DR18, BKP_DR19, BKP_DR20,
    BKP_DR21, BKP_DR22, BKP_DR23, BKP_DR24, BKP_DR25, BKP_DR26, BKP_DR27, BKP_DR28, BKP_DR29, BKP_DR30,
    BKP_DR31, BKP_DR32, BKP_DR33, BKP_DR34, BKP_DR35, BKP_DR36, BKP_DR37, BKP_DR38, BKP_DR39, BKP_DR40,
    BKP_DR41, BKP_DR42
#endif
};



static uint16_t nvdata_read_reg(size_t reg_num)
{
    if(reg_num >= NVDATA_BKP_REGS_COUNT) return 0;

    return BKP_ReadBackupRegister(nvdata_bkp_regs_table[reg_num]);
}

static void nvdata_write_reg(size_t reg_num, uint16_t reg_data)
{
    if(reg_num >= NVDATA_BKP_REGS_COUNT) return;

    uint32_t pwr_dbp = PWR->CR & PWR_CR_DBP;
    if(!pwr_dbp) PWR->CR |= PWR_CR_DBP;
    
    BKP_WriteBackupRegister(nvdata_bkp_regs_table[reg_num], reg_data);
    
    if(!pwr_dbp) PWR->CR &= ~PWR_CR_DBP;
}

void nvdata_clear(void)
{
    uint32_t pwr_dbp = PWR->CR & PWR_CR_DBP;
    if(!pwr_dbp) PWR->CR |= PWR_CR_DBP;
    
    size_t i;
    for(i = 0; i < NVDATA_BKP_REGS_COUNT; i ++){
        BKP_WriteBackupRegister(nvdata_bkp_regs_table[i], 0);
    }
    
    if(!pwr_dbp) PWR->CR &= ~PWR_CR_DBP;
}

err_t nvdata_get_byte(size_t address, uint8_t* data)
{
    if(data == NULL) return E_NULL_POINTER;
    if(address >= NVDATA_SIZE) return E_OUT_OF_RANGE;
    
    size_t reg_n = address >> 1;
    
    uint16_t reg_data = nvdata_read_reg(reg_n);
    
    size_t byte_n = address & 0x1;
    
    if(byte_n == 0) *data = reg_data & 0xff;
    else *data = (reg_data >> 8) & 0xff;
    
    return E_NO_ERROR;
}

err_t nvdata_put_byte(size_t address, uint8_t data)
{
    if(address >= NVDATA_SIZE) return E_OUT_OF_RANGE;
    
    size_t reg_n = address >> 1;
    
    uint16_t reg_data = nvdata_read_reg(reg_n);
    
    size_t byte_n = address & 0x1;
    
    if(byte_n == 0){
        reg_data &= 0xff00;
        reg_data |= data;
    }else{
        reg_data &= 0xff;
        reg_data |= (data << 8);
    }
    
    nvdata_write_reg(reg_n, reg_data);
    
    return E_NO_ERROR;
}

err_t nvdata_get_half_word(size_t address, uint16_t* data)
{
    if(data == NULL) return E_NULL_POINTER;
    if(address >= NVDATA_SIZE - 1) return E_OUT_OF_RANGE;
    
    bool aligned = !(address & 0x1);
    
    if(aligned){
        size_t reg_n = address >> 1;
        *data = nvdata_read_reg(reg_n);
    }else{
        uint8_t reg_data[2];
        RETURN_ERR_IF_FAIL(nvdata_get_byte(address,     &reg_data[0]));
        RETURN_ERR_IF_FAIL(nvdata_get_byte(address + 1, &reg_data[1]));
        *data = (((uint16_t)reg_data[1]) << 8) | reg_data[0];
    }
    
    return E_NO_ERROR;
}

err_t nvdata_put_half_word(size_t address, uint16_t data)
{
    if(address >= NVDATA_SIZE - 1) return E_OUT_OF_RANGE;
    
    bool aligned = !(address & 0x1);
    
    if(aligned){
        size_t reg_n = address >> 1;
        nvdata_write_reg(reg_n, data);
    }else{
        uint8_t reg_data[2];
        reg_data[0] = data & 0xff;
        reg_data[1] = (data >> 8) & 0xff;
        RETURN_ERR_IF_FAIL(nvdata_put_byte(address,     reg_data[0]));
        RETURN_ERR_IF_FAIL(nvdata_put_byte(address + 1, reg_data[1]));
    }
    
    return E_NO_ERROR;
}

err_t nvdata_get_word(size_t address, uint32_t* data)
{
    if(data == NULL) return E_NULL_POINTER;
    if(address >= NVDATA_SIZE - 1) return E_OUT_OF_RANGE;
    
    bool aligned = !(address & 0x1);
    
    if(aligned){
        size_t reg_n = address >> 1;
        
        uint16_t reg_data[2];
        
        reg_data[0] = nvdata_read_reg(reg_n);
        reg_data[1] = nvdata_read_reg(reg_n + 1);
        
        *data = (((uint32_t)reg_data[1]) << 16) | reg_data[0];
    }else{
        size_t reg_n = (address + 1) >> 1;
        
        uint8_t reg_data0, reg_data3;
        uint16_t reg_data12;
        
        RETURN_ERR_IF_FAIL(nvdata_get_byte(address,     &reg_data0));
        reg_data12 = nvdata_read_reg(reg_n);
        RETURN_ERR_IF_FAIL(nvdata_get_byte(address + 3, &reg_data3));
        
        *data = (((uint32_t)reg_data3) << 24) |
                (((uint32_t)reg_data12) << 8) |
                reg_data0;
    }
    
    return E_NO_ERROR;
}

err_t nvdata_put_word(size_t address, uint32_t data)
{
    if(address >= NVDATA_SIZE - 1) return E_OUT_OF_RANGE;
    
    bool aligned = !(address & 0x1);
    
    if(aligned){
        size_t reg_n = address >> 1;
        
        uint16_t reg_data[2];
        
        reg_data[0] = data & 0xffff;
        reg_data[1] = (data >> 16) & 0xffff;
        
        nvdata_write_reg(reg_n,     reg_data[0]);
        nvdata_write_reg(reg_n + 1, reg_data[1]);
        
    }else{
        size_t reg_n = (address + 1) >> 1;
        
        uint8_t reg_data0, reg_data3;
        uint16_t reg_data12;
        
        reg_data0  = data & 0xff;
        reg_data12 = (data >> 8) & 0xffff;
        reg_data3  = (data >> 24) & 0xff;
        
        RETURN_ERR_IF_FAIL(nvdata_put_byte(address,     reg_data0));
        nvdata_write_reg(reg_n, reg_data12);
        RETURN_ERR_IF_FAIL(nvdata_put_byte(address + 3, reg_data3));
    }
    
    return E_NO_ERROR;
}
