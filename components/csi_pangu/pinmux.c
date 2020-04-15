/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     pinmux.c
 * @brief    source file for the pinmux
 * @version  V1.0
 * @date     02. June 2017
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/
#include <stdint.h>
#include "pinmux.h"
#include "pin_name.h"
#include <drv/gpio.h>

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define writel(b,addr) (void)((*(volatile unsigned int *) (addr)) = (b))

/*******************************************************************************
 * function: ioreuse_inital
 *
 * description:
 *   initial hobbit_pinmux
 *******************************************************************************/

void yunvoice_initial(void)
{
    //writel(0x3f, 0x80020010);
}

int32_t drv_pinmux_config(pin_name_e pin, pin_func_e pin_func)
{
    unsigned int val = 0;
    unsigned int reg_val = 0;

    uint8_t offset;

    if (pin_func > 3) {      /* select gpio mode */
        if (pin < PB0) {
            offset = pin;
            val = readl(YUNVOIC_PADIO_SEL0);
            val &= ~(1 << offset);
            writel(val, YUNVOIC_PADIO_SEL0);
            return 0;
        } else if (pin >= PB0) {
            offset = pin - 32;
            /* gpio data source select */
            val = readl(YUNVOIC_PADIO_SEL1);
            val &= ~(1 << offset);
            writel(val, YUNVOIC_PADIO_SEL1);
            return 0;
        }
    }

    if (pin >= PB0) {
        offset = pin - 32;

        /* gpio data source select */
        val = readl(YUNVOIC_PADIO_SEL1);
        val |= (1 << offset);
        writel(val, YUNVOIC_PADIO_SEL1);

        if (offset < 16) {
            reg_val = (0x3 << (offset * 2));
            /* reuse function select */
            val = readl(YUNVOIC_PADIO_AFSEL_1L);
            val &= ~(reg_val);
            val |= (pin_func << (2 * offset));
            writel(val, YUNVOIC_PADIO_AFSEL_1L);
        } else {
            offset = offset - 16;
            reg_val = (0x3 << (offset * 2));
            /* reuse function select */
            val = readl(YUNVOIC_PADIO_AFSEL_1H);
            val &= ~(reg_val);
            val |= (pin_func << (2 * offset));
            writel(val, YUNVOIC_PADIO_AFSEL_1H);
        }

        return 0;
    }

    if (pin >= PA16) {
        offset = pin - 16;

        reg_val = (0x3 << (offset * 2));
        /* reuse function select */
        val = readl(YUNVOIC_PADIO_AFSEL_0H);
        val &= ~(reg_val);
        val |= (pin_func << (2 * offset));
        writel(val, YUNVOIC_PADIO_AFSEL_0H);
        val = readl(YUNVOIC_PADIO_SEL0);
        val |= 1 << pin;
        writel(val, YUNVOIC_PADIO_SEL0);
        return 0;
    }

    offset = pin;


    reg_val = (0x3 << (offset * 2));
    /* reuse function select */
    val = readl(YUNVOIC_PADIO_AFSEL_0L);
    val &= ~(reg_val);
    val |= (pin_func << (2 * offset));
    writel(val, YUNVOIC_PADIO_AFSEL_0L);
    val = readl(YUNVOIC_PADIO_SEL0);
    val |= 1 << offset;
    writel(val, YUNVOIC_PADIO_SEL0);
    return 0;
}

static gpio_mode_e pin_support_pull_mode(port_name_e port, uint8_t offset)
{
    if (port == PORTA) {
        if ((offset >= 22 && offset <= 28) || (offset >= 3 && offset <= 20) || offset == 1) {
            return GPIO_MODE_PULLUP;
        } else {
            return GPIO_MODE_PULLDOWN;
        }
    } else if (port == PORTB) {
        if (offset == 25 || offset == 1) {
            return GPIO_MODE_PULLUP;
        } else {
            return GPIO_MODE_PULLDOWN;
        }
    }

    return GPIO_MODE_PULLNONE;
}

int32_t drv_pin_config_mode(port_name_e port, uint8_t offset, gpio_mode_e pin_mode)
{
    uint32_t pull_reg = 0;

    if (port == PORTA) {
        pull_reg = YUNVOIC_PADIO_IO_PA_PE;
    } else if (port == PORTB) {
        pull_reg = YUNVOIC_PADIO_IO_PB_PE;
    } else {
        return -1;
    }

    if (pin_mode != GPIO_MODE_PULLNONE) {
        if (pin_mode != pin_support_pull_mode(port, offset)) {
            return -1;
        }
    }

    uint32_t reg_val = readl(pull_reg);
    if (pin_mode == GPIO_MODE_PULLNONE) {
        reg_val &= ~(1 << offset);
    } else {
        reg_val |= 1 << offset;
    }

    writel(reg_val, pull_reg);

    return 0;
}

int32_t yun_io_mode_sel_pe_enable(int en)
{
    uint32_t reg = YUNVOIC_PADIO_IO_MOD_SEL_PE;

    if (en) {
        writel(1 , reg);
    } else {
        writel(0 , reg);
    }

    return 0;
}

int32_t yun_io_fmc_data_pe_enable(int en)
{
    uint32_t reg = YUNVOIC_PADIO_IO_FMC_DATA_PE;

    if (en) {
        writel(1 , reg);
    } else {
        writel(0 , reg);
    }

    return 0;
}

int32_t yun_mcurst_pe_enable(int en)
{
    uint32_t reg = YUNVOIC_PADIO_IO_MCURST_PE;

    if (en) {
        writel(1 , reg);
    } else {
        writel(0 , reg);
    }

   return 0;
}

int32_t yun_io_pa_ds(port_name_e port, uint8_t offset, int high_driver_en)
{
    uint32_t reg = 0;

    if (port == PORTA) {
        reg = YUNVOIC_PADIO_IO_PA_DS;
    } else if (port == PORTB) {
        reg = YUNVOIC_PADIO_IO_PB_DS;
    } else {
        return -1;
    }

    uint32_t reg_val = readl(reg);

    if (high_driver_en) {
        reg_val |= 1 << offset;
    } else {
        reg_val &= ~(1 << offset);
    }

    writel(reg_val , reg);

    return 0;
}

int yun_io_fmc_addr_ds(int high_driver_en)
{
    uint32_t reg = YUNVOIC_PADIO_IO_FMC_ADDR_DS;

    if (high_driver_en) {
        writel(1 , reg);
    } else {
        writel(0 , reg);
    }

    return 0;
}

int yun_io_fmc_data_ds(int high_driver_en)
{
    uint32_t reg = YUNVOIC_PADIO_IO_FMC_DATA_DS;

    if (high_driver_en) {
        writel(1 , reg);
    } else {
        writel(0 , reg);
    }

    return 0;
}

int yun_io_mem_ds4(uint8_t offset, int high_driver_en)
{
    uint32_t reg = YUNVOIC_PADIO_IO_MEM_DS4;
    uint32_t reg_val = readl(reg);

    if (high_driver_en) {
        reg_val |= 1 << offset;
    } else {
        reg_val &= ~(1 << offset);
    }

    writel(reg_val, reg);

    return 0;
}
