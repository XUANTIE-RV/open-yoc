/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     system.c
 * @brief    CSI Device System Source File for YUNVOICE
 * @version  V1.0
 * @date     06. Mar 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#include <stdint.h>
#include <io.h>
#include <soc.h>
#include <csi_core.h>
#include <drv/irq.h>

#include <sys_freq.h>

#ifndef CONFIG_SYSTICK_HZ
#define CONFIG_SYSTICK_HZ 100
#endif

extern uint32_t __Vectors[];
extern int32_t g_top_irqstack;
extern void irq_vectors_init(void);
extern void mm_heap_initialize(void);
extern void Reset_Handler(void);

int g_system_clock = 24000000;

static void config_system_clock(void)
{
}

static void config_irq_priority(void)
{
    int i;

    for (i = 1; i <= 51; i++) {
        csi_vic_set_prio(i, 1);
    }
}

int32_t drv_get_cpu_id(void)
{
    return 0;
}

#ifdef CONFIG_KERNEL_NONE
static void _system_init_for_baremetal(void)
{
    __enable_excp_irq();

    csi_coret_config(drv_get_cur_cpu_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);    //10ms

    mm_heap_initialize();
}
#endif

#ifndef CONFIG_KERNEL_NONE
static void _system_init_for_kernel(void)
{
#if defined(CONFIG_SUPPORT_TSPEND)
    __set_Int_SP((uint32_t)&g_top_irqstack);
    __set_CHR(__get_CHR() | CHR_ISE_Msk);
    VIC->TSPR = 0xFF;
#endif

#if !defined(CONFIG_SUPPORT_TSPEND)
    irq_vectors_init();
#endif

    csi_coret_config(drv_get_cur_cpu_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);    //10ms
    drv_irq_enable(CORET_IRQn);

#ifndef CONFIG_KERNEL_RHINO
#ifndef CONFIG_NUTTXMM_NONE
    mm_heap_initialize();
#endif
#endif
}
#endif

#ifdef CONFIG_SYSTEM_SECURE
static void __memcpy(void *dest, const void *src, size_t n)
{
    int i;

    for (i = 0; i < n; i++) {
        *((uint8_t *)dest + i) = *((uint8_t *)src + i);
    }
}
#endif

/**
  * @brief  initialize the system
  *         Initialize the psr and vbr.
  * @param  None
  * @return None
  */
void SystemInit(void)
{
    __set_VBR((uint32_t) & (__Vectors));

    /* Clear active and pending IRQ, and disable IRQ */
    VIC->IABR[0] = 0x0;
    VIC->ICPR[0] = 0xFFFFFFFF;
    VIC->ICER[0] = 0xFFFFFFFF;

#ifndef CONFIG_SYSTEM_SECURE
    csi_cache_set_range(0, 0x18000000, CACHE_CRCR_8M, 1);
    csi_icache_enable();
    csi_dcache_enable();
#endif

    config_irq_priority();
    config_system_clock();
#ifdef CONFIG_KERNEL_NONE
    _system_init_for_baremetal();
#else
    _system_init_for_kernel();
#endif
}


