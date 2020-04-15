/*
 *  silan_padmux
*/

//#define __DEBUG__

#include "silan_iomux.h"
#include "silan_iomux_regs.h"
#include "silan_syscfg_regs.h"
#include "silan_types.h"
#include "silan_printf.h"
#include "silan_gpio_regs.h"

#define  IOMUX_BIAS_ADDR (64)

void io_config(unsigned int mod,unsigned int func)
{
    int reg_bias,data_bias,tmp;
    func &=0x7;
    reg_bias = mod/4;
    data_bias= (mod - reg_bias*4)*8;
    tmp = __REG32(SILAN_SYSCFG_BASE+((IOMUX_BIAS_ADDR +reg_bias)<<2));
    tmp = (tmp & (~(0xff<<data_bias)))| ((0x60+func)<<data_bias);
    __REG32(SILAN_SYSCFG_BASE+((IOMUX_BIAS_ADDR + reg_bias)<<2)) = tmp;
}

void silan_io_pullup_config(unsigned int mod,unsigned int func)
{
    io_attr_t io_attr;
    int reg_bias,data_bias;
    // io_attr_reg_t io_reg;
    int io_reg;

    reg_bias = mod/4;
    data_bias= mod - reg_bias*4;
    io_reg = __REG32(SILAN_SYSCFG_BASE+((IOMUX_BIAS_ADDR +reg_bias)<<2));
    io_attr.d8 = (io_reg >> (data_bias*8)) & 0xFF;
    io_attr.b.ren = func;
    io_reg &= ~(0xFF << (data_bias*8));
    io_reg |=  ((io_attr.d8) << (data_bias*8));
    __REG32(SILAN_SYSCFG_BASE+((IOMUX_BIAS_ADDR + reg_bias)<<2)) = io_reg;
}

void silan_io_ie_config(unsigned int mod,unsigned int func)
{
    io_attr_t io_attr;
    int reg_bias,data_bias;
    // io_attr_reg_t io_reg;
    int io_reg;

    reg_bias = mod/4;
    data_bias= mod - reg_bias*4;
    io_reg = __REG32(SILAN_SYSCFG_BASE+((IOMUX_BIAS_ADDR +reg_bias)<<2));
    io_attr.d8 = (io_reg >> (data_bias*8)) & 0xFF;
    io_attr.b.ie = func;
    io_reg &= ~(0xFF << (data_bias*8));
    io_reg |=  ((io_attr.d8) << (data_bias*8));
    __REG32(SILAN_SYSCFG_BASE+((IOMUX_BIAS_ADDR + reg_bias)<<2)) = io_reg;
}

void silan_io_driver_config(unsigned int mod,unsigned int func)
{
    io_attr_t io_attr;
    int reg_bias,data_bias;
    // io_attr_reg_t io_reg;
    int io_reg;

    reg_bias = mod/4;
    data_bias= mod - reg_bias*4;
    io_reg = __REG32(SILAN_SYSCFG_BASE+((IOMUX_BIAS_ADDR +reg_bias)<<2));
    io_attr.d8 = (io_reg >> (data_bias*8)) & 0xFF;
    io_attr.b.ds = func;
    io_reg &= ~(0xFF << (data_bias*8));
    io_reg |=  ((io_attr.d8) << (data_bias*8));
    __REG32(SILAN_SYSCFG_BASE+((IOMUX_BIAS_ADDR + reg_bias)<<2)) = io_reg;
}

int silan_iomux_adc_open(uint8_t channel)
{
    if(channel>7)
        return -1;
    __REG32(SILAN_SYSCFG_REG4) |= ((1<<channel));//|(1<<8));
    return 0;
}
int silan_iomux_adc_close(uint8_t channel)
{
    if(channel>7)
        return -1;
    __REG32(SILAN_SYSCFG_REG4) &= ~((1<<channel));//|(1<<8));
    return 0;
}

void silan_io_attr_set(int mod, io_attr_t attr)
{
    int reg_bias,data_bias;
    // io_attr_reg_t io_reg;
    int io_reg;

    reg_bias = mod/4;
    data_bias= mod - reg_bias*4;
    io_reg = __REG32(SILAN_SYSCFG_BASE+((IOMUX_BIAS_ADDR +reg_bias)<<2));
    io_reg &= ~(0xFF << (data_bias*8));
    io_reg |=  (attr.d8 << (data_bias*8));
    __REG32(SILAN_SYSCFG_BASE+((IOMUX_BIAS_ADDR + reg_bias)<<2)) = io_reg;
}

io_attr_t silan_io_attr_get(int mod)
{
    int reg_bias,data_bias;
    // io_attr_reg_t io_reg;
    int io_reg;
    io_attr_t attr;

    reg_bias = mod/4;
    data_bias= mod - reg_bias*4;
    io_reg = __REG32(SILAN_SYSCFG_BASE+((IOMUX_BIAS_ADDR +reg_bias)<<2));
    attr.d8 = (io_reg >> (data_bias*8)) & 0xFF;
    return attr;
}

void silan_io_func_config(uint32_t mod, uint32_t func)
{
    io_attr_t io_attr;
    int reg_bias,data_bias;
    // io_attr_reg_t io_reg;
    int io_reg;

    reg_bias = mod/4;
    data_bias= mod - reg_bias*4;
    io_reg = __REG32(SILAN_SYSCFG_BASE+((IOMUX_BIAS_ADDR +reg_bias)<<2));
    io_attr.d8 = (io_reg >> (data_bias*8)) & 0xFF;
    io_attr.b.func = func;
    io_reg &= ~(0xFF << (data_bias*8));
    io_reg |=  ((io_attr.d8) << (data_bias*8));
    __REG32(SILAN_SYSCFG_BASE+((IOMUX_BIAS_ADDR + reg_bias)<<2)) = io_reg;
}

