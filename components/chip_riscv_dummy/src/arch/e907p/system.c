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
    csi_dcache_enable();
    csi_icache_enable();
}

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

static void clic_init(void)
{
    int i;

    /* get interrupt level from info */
    CLIC->CLICCFG = (((CLIC->CLICINFO & CLIC_INFO_CLICINTCTLBITS_Msk) >> CLIC_INFO_CLICINTCTLBITS_Pos) << CLIC_CLICCFG_NLBIT_Pos);

    for (i = 0; i < 64; i++) {
        CLIC->CLICINT[i].IP = 0;
        CLIC->CLICINT[i].ATTR = 1; /* use vector interrupt */
    }

    /* tspend use positive interrupt */
    CLIC->CLICINT[Machine_Software_IRQn].ATTR = 0x3;
    csi_irq_enable(Machine_Software_IRQn);
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
    /* enable theadisaee & MM */
    uint32_t status = __get_MXSTATUS();
    status |= (1 << 22 | 1 << 15);
    __set_MXSTATUS(status);

#if __riscv_flen
    /* enable float ISA */
    status = __get_MSTATUS();
    status |= (1 << MSTATUS_FS_SHIFT);
    __set_MSTATUS(status);
#endif
     /* enable mexstatus SPUSHEN and disable SPSWAPEN */
#if CONFIG_CPU_E906 || CONFIG_CPU_E906F || CONFIG_CPU_E906FD || CONFIG_CPU_E906P || CONFIG_CPU_E906FP || CONFIG_CPU_E906FDP \
    || CONFIG_CPU_E907 || CONFIG_CPU_E907F || CONFIG_CPU_E907FD || CONFIG_CPU_E907P || CONFIG_CPU_E907FP || CONFIG_CPU_E907FDP
    status = __get_MEXSTATUS();
    status |= (0x1 << 16);
    status &= ~(0x2 << 16);
    __set_MEXSTATUS(status);
#endif

    cache_init();
    section_init();
    interrupt_init();
    soc_set_sys_freq(20000000);
    csi_tick_init();
}

