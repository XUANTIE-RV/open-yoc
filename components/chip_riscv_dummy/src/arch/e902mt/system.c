/*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 */

#include <csi_config.h>
#include <soc.h>
#include <csi_core.h>
#include <drv/irq.h>
#include <drv/dma.h>
#include <drv/tick.h>
#include <drv/etb.h>
#include <drv/spiflash.h>
#include "riscv_csr.h"

#if (defined(CONFIG_KERNEL_RHINO) || defined(CONFIG_KERNEL_FREERTOS) || defined(CONFIG_KERNEL_RTTHREAD)) && defined(CONFIG_KERNEL_NONE)
#error "Please check the current system is baremetal or not!!!"
#endif

extern void section_data_copy(void);
extern void section_ram_code_copy(void);
extern void section_bss_clear(void);

static void cache_init(void)
{
    csi_icache_enable();
}

static void section_init(void)
{
#ifdef CONFIG_XIP
    section_data_copy();
    section_ram_code_copy();
#endif

    section_bss_clear();
}

static void clic_init(void)
{
    int i;

    /* get interrupt level from info */
    CLIC->CLICCFG = (((CLIC->CLICINFO & CLIC_INFO_CLICINTCTLBITS_Msk) >> CLIC_INFO_CLICINTCTLBITS_Pos) << CLIC_CLICCFG_NLBIT_Pos);

    for (i = 0; i < 64; i++) {
        CLIC->CLICINT[i].IP = 0;
        CLIC->CLICINT[i].ATTR = 1; /* use vector interrupt */
        csi_vic_set_prio(i, 1);
    }

#ifndef CONFIG_KERNEL_NONE
    /* tspend use lower priority */
    csi_vic_set_prio(Machine_Software_IRQn, 0);
    /* tspend use positive interrupt */
    CLIC->CLICINT[Machine_Software_IRQn].ATTR = 0x3;
    csi_irq_enable(Machine_Software_IRQn);
#endif
}

static void interrupt_init(void)
{
    clic_init();
#ifdef CONFIG_KERNEL_NONE
    __enable_excp_irq();
#endif
}

/**
  * @brief  initialize the system
  *         Initialize the psr and vbr.
  * @param  None
  * @return None
  */
void SystemInit(void)
{
    /* enable theadisaee */
    uint32_t status = __get_MXSTATUS();
    status |= (1 << 22);
    __set_MXSTATUS(status);
    cache_init();
    section_init();
    interrupt_init();
    soc_set_sys_freq(20000000);
    csi_tick_init();
}

