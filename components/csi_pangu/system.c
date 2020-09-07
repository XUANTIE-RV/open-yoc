/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
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
#ifndef CONFIG_SYSTICK_HZ
#define CONFIG_SYSTICK_HZ 100
#endif

extern uint32_t __Vectors[];
extern int32_t g_top_irqstack;
extern void irq_vectors_init(void);
extern void mm_heap_initialize(void);
extern void Reset_Handler(void);

static void config_system_clock(void)
{
    int module_id;

    drv_set_aon_clk_src(ELS_CLK);

    drv_clk_enable(AONTIM_CLK);
    drv_clk_enable(RTC_CLK);

    for (module_id = FFT_CLK; module_id <= PWM0_CLK; module_id += 4) {
        drv_clk_enable(module_id);
    }

    drv_clk_enable(UART1_CLK);

    for (module_id = TIM2_CLK; module_id <= PWM1_CLK; module_id += 4) {
        drv_clk_enable(module_id);
    }

    drv_clk_enable(CODEC_CLK);
}

static void config_irq_priority(void)
{
    int i;

    for (i = 1; i <= 51; i++) {
        csi_vic_set_prio(i, 1);
    }

    csi_vic_set_prio(DMAC1_IRQn, 0);
}

int32_t drv_get_cpu_id(void)
{
#ifdef CONFIG_CHIP_PANGU_CPU0
    return 0;
#endif

#ifdef CONFIG_CHIP_PANGU_CPU1
    return 1;
#endif

    return 2;
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

#if defined(CONFIG_FREQ_TEST)
	csi_coret_config(1000000, CORET_IRQn);
#else
    csi_coret_config(drv_get_cur_cpu_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);    //10ms
#endif
    drv_irq_enable(CORET_IRQn);

#ifndef CONFIG_KERNEL_RHINO
#ifndef CONFIG_NUTTXMM_NONE
    mm_heap_initialize();
#endif
#endif
}
#endif

#ifdef CONFIG_CHIP_PANGU_CPU0
static void __memcpy(void *dest, const void *src, size_t n)
{
    int i;

    for (i = 0; i < n; i++) {
        *((uint8_t *)dest + i) = *((uint8_t *)src + i);
    }
}
#endif

static void section_data_copy(void)
{
#ifdef CONFIG_CHIP_PANGU_CPU0
    extern uint32_t __stext;
    extern uint32_t __etext;
    extern uint32_t __srodata;
    extern uint32_t __flash_code_end__;
    extern uint32_t __erodata;
    extern uint32_t __data_start__;
    extern uint32_t __data_end__;
    extern uint32_t __ram_code_start__;
    extern uint32_t __ram_code_end__;

    uint32_t src_addr;

    uint32_t xip_flag = (uint32_t)&__stext;

    /* if run in flash */
    if ((xip_flag & 0xFF000000) == 0x8000000) {
        src_addr = (uint32_t)&__erodata;
        __memcpy((void *)(&__data_start__), \
                 (void *)src_addr, \
                 (uint32_t)(&__data_end__) - (uint32_t)(&__data_start__));

        src_addr += (uint32_t)(&__data_end__) - (uint32_t)(&__data_start__);
        __memcpy((void *)(&__ram_code_start__), \
                 (void *)src_addr, \
                 (uint32_t)(&__ram_code_end__) - (uint32_t)(&__ram_code_start__));
    } else {
        uint32_t entry_addr = (uint32_t)&Reset_Handler;

        /* if reset_handler run in flash */
        if ((entry_addr & 0xFF000000) == 0x8000000) {
            src_addr = (uint32_t)&__flash_code_end__;
            __memcpy((void *)(&__stext), \
                     (void *)src_addr, \
                     (uint32_t)(&__etext) - (uint32_t)(&__stext));

            src_addr += (uint32_t)(&__etext) - (uint32_t)(&__stext);
            __memcpy((void *)(&__srodata), \
                     (void *)src_addr, \
                     (uint32_t)(&__erodata) - (uint32_t)(&__srodata));

            src_addr += (uint32_t)(&__erodata) - (uint32_t)(&__srodata);
            __memcpy((void *)(&__data_start__), \
                     (void *)src_addr, \
                     (uint32_t)(&__data_end__) - (uint32_t)(&__data_start__));

            src_addr += (uint32_t)(&__data_end__) - (uint32_t)(&__data_start__);
            __memcpy((void *)(&__ram_code_start__), \
                     (void *)src_addr, \
                     (uint32_t)(&__ram_code_end__) - (uint32_t)(&__ram_code_start__));
        }
    }

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
    section_data_copy();
    __set_VBR((uint32_t) & (__Vectors));

    /* Clear active and pending IRQ, and disable IRQ */
    VIC->IABR[0] = 0x0;
    VIC->ICPR[0] = 0xFFFFFFFF;
    VIC->ICER[0] = 0xFFFFFFFF;

    /* config APB1 clock from 18M to 48M */
    putreg32(2, (uint32_t *)0x8b000004);

#if defined(CONFIG_CHIP_PANGU_CPU2)
    csi_cache_set_range(0, 0x18000000, CACHE_CRCR_16M, 1);
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
    drv_set_sdio_freq(0, 52500000);
}


