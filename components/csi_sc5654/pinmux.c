/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     pinmux.c
 * @brief    source file for the pinmux
 * @version  V1.0
 * @date     16. August 2018
 ******************************************************************************/

#include <csi_config.h>
#include <stdint.h>
#include "pinmux.h"
#include "pin_name.h"
#include <drv/gpio.h>
#include <soc.h>

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define writel(b,addr) (void)((*(volatile unsigned int *) (addr)) = (b))

/**
  \brief       initial hobbit pinmux.
  \return      none
*/
void ioreuse_initial(void)
{
}

/**
  \brief       config the pin function.
  \param[in]   pin       refs to pin_name_e.
  \param[in]   pin_func  refs to pin_func_e.
  \return      0-success or -1-failure
*/
int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func)
{
    io_attr_t io_attr;
    int reg_bias,data_bias;
    int io_reg;

    if (pin >= PA0 && pin <= PD10) {
        reg_bias  = pin / 4;
        data_bias = pin - reg_bias * 4;
        io_reg = readl(SILAN_SYSCFG_BASE + ((SILAN_IOMUX_BIAS_ADDR + reg_bias) << 2));
        io_attr.d8 = (io_reg >> (data_bias * 8)) & 0xFF;
        io_attr.b.func = pin_func;
        io_reg &= ~(0xFF << (data_bias * 8));
        io_reg |=  ((io_attr.d8) << (data_bias * 8));
        writel(io_reg, SILAN_SYSCFG_BASE + ((SILAN_IOMUX_BIAS_ADDR + reg_bias) << 2));
        return 0;
    }

    return -1;
}

/**
  \brief       config the pin mode.
  \param[in]   port      refs to port_name_e.
  \param[in]   offset    the offset of the pin in corresponding port.
  \param[in]   pin_mode  refs to gpio_mode_e.
  \return      0-success or -1-failure
*/
int32_t drv_pin_config_mode(port_name_e port, uint8_t offset, gpio_mode_e pin_mode)
{
    return 0;
}
