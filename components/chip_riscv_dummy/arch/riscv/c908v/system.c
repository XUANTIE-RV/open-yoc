/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     system.c
 * @brief    CSI Device System Source File
 * @version  V1.0
 * @date     02. Oct 2018
 ******************************************************************************/

#include <soc.h>
#include <csi_core.h>
#include <drv/tick.h>
#include <drv/porting.h>
#include <drv/irq.h>

#if (defined(CONFIG_KERNEL_RHINO) || defined(CONFIG_KERNEL_FREERTOS)) && defined(CONFIG_KERNEL_NONE)
#error "Please check the current system is baremetal or not!!!"
#endif

extern void section_data_copy(void);
extern void section_ram_code_copy(void);
extern void section_bss_clear(void);

static void section_init(void)
{
#ifdef CONFIG_XIP
    section_data_copy();
    section_ram_code_copy();
    csi_dcache_clean();
    csi_icache_invalid();
#endif

    section_bss_clear();
}

static void cache_init(void)
{
    /* enable cache */
    csi_dcache_enable();
    csi_icache_enable();
}

static void interrupt_init(void)
{
    int i;

    for (i = 0; i < 1023; i++) {
        PLIC->PLIC_PRIO[i] = 31;
    }

    for (i = 0; i < 32; i++) {
        PLIC->PLIC_IP[i] = 0;
    }

    for (i = 0; i < 32; i++) {
        PLIC->PLIC_H0_MIE[i] = 0;
        PLIC->PLIC_H0_SIE[i] = 0;
    }

    /* set hart threshold 0, enable all interrupt */
    PLIC->PLIC_H0_MTH = 0;
    PLIC->PLIC_H0_STH = 0;

    for (i = 0; i < 1023; i++) {
        PLIC->PLIC_H0_MCLAIM = i;
        PLIC->PLIC_H0_SCLAIM = i;
    }

    /* set PLIC_PER */
    PLIC->PLIC_PER = 0x1;

    /* enable msoft interrupt ; Machine_Software_IRQn*/
    uint32_t mie = __get_MIE();
    mie |= (1 << 11 | 1 << 7 | 1 << 3);
    __set_MIE(mie);
}

void SystemInit(void)
{
    cache_init();
    section_init();
    interrupt_init();
    soc_set_sys_freq(10000000);
    csi_tick_init();
}
