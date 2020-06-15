/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     pinmux.c
 * @brief    source file for the pinmux
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <stdint.h>
#include "pinmux.h"
#include "pin_name.h"
#include "drv_gpio.h"

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define writel(b,addr) (void)((*(volatile unsigned int *) (addr)) = (b))

/*******************************************************************************
 * function: ioreuse_inital
 *
 * description:
 *   initial hobbit_pinmux
 *******************************************************************************/

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

        if ( offset < 16) {
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

int32_t drv_pin_config_mode(port_name_e port, uint8_t offset, gpio_mode_e pin_mode)
{
    return 0;
}
