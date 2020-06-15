/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "silan_voice_adc.h"

#include <silan_gpio_regs.h>
#include <silan_iomux_regs.h>
#include <silan_gpio_source.h>
#include <silan_gpio.h>
#include <silan_iomux.h>
#include <soc.h>
#define __REG32(addr)             (*(volatile uint32_t*)(addr))
#define GPIO_CONFIG_OFFSET(x)    (x / (GPIO1_MAX_NUM+1))
#define GPIO_CONFIG_PIN(x)       (x % (GPIO1_MAX_NUM+1))
#define IO_TO_GPIO(x)            ((x>=8) ? (x-8) : (34+x))
#define  IOMUX_BIAS_ADDR (64)

SRAM_TEXT void sram_silan_io_pullup_config(unsigned int mod,unsigned int func)
{
    io_attr_t io_attr;
    int reg_bias,data_bias;
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

SRAM_TEXT int32_t sram_drv_gpio_input_init(int32_t gpio_pin)
{
    int32_t offset, pin, gpio;

    gpio = IO_TO_GPIO(gpio_pin);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);

    sram_silan_io_pullup_config(gpio_pin, 1);
    silan_gpio_input(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));

    return 0;
}

SRAM_TEXT int32_t sram_drv_gpio_read(int32_t gpio_pin)
{
    int32_t offset, pin, gpio;

    gpio = IO_TO_GPIO(gpio_pin);
    offset = GPIO_CONFIG_OFFSET(gpio);
    pin = GPIO_CONFIG_PIN(gpio);

    return silan_gpio_get(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
}

// SRAM_TEXT int32_t sram_drv_gpio_irq_set(int32_t gpio_pin, int32_t mode, int32_t enable)
// {
//     int32_t offset, pin, gpio;

//     gpio = IO_TO_GPIO(gpio_pin);
//     offset = GPIO_CONFIG_OFFSET(gpio);
//     pin = GPIO_CONFIG_PIN(gpio);

//     if (enable) {
//         __REG32(SILAN_INTR_STS_BASE + 0x200 + (GPIO1_IRQn + offset) * 4) |= GPIO_PIN(pin);
//         silan_gpio_irq_mode(SILAN_GPIO_BASE(offset), GPIO_PIN(pin), mode);
//         silan_gpio_irq_enable(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
//     } else {
//         __REG32(SILAN_INTR_STS_BASE + 0x200 + (GPIO1_IRQn + offset) * 4) &= ~GPIO_PIN(pin);
//         silan_gpio_irq_clear(SILAN_GPIO_BASE(offset), GPIO_PIN(pin));
//     }
//     return 0;
// }
