/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <intc.h>


void hal_intc_init(void)
{
    volatile uint32_t *intc_reg_addr = (volatile uint32_t *)HOBBIT_VIC_BASE;

    intc_reg_addr[VIC_IPR0 >> 2] = 0x00000000;
    intc_reg_addr[VIC_IPR1 >> 2] = 0x00000000;
    intc_reg_addr[VIC_IPR2 >> 2] = 0x00000000;
    intc_reg_addr[VIC_IPR3 >> 2] = 0x00000000;
    intc_reg_addr[VIC_IPR4 >> 2] = 0x00000000;
    intc_reg_addr[VIC_IPR5 >> 2] = 0x00000000;
    intc_reg_addr[VIC_IPR6 >> 2] = 0x00000000;
    intc_reg_addr[VIC_IPR7 >> 2] = 0x00000000;

    __asm__ volatile("psrset ee");
}
