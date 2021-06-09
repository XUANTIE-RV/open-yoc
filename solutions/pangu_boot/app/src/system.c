/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <soc.h>
#include <csi_core.h>
#include <drv/io.h>
#include <drv/tick.h>
#include <drv/dma.h>
#include <drv/gpio.h>
#include <drv/spiflash.h>

extern int32_t g_top_irqstack;
extern void Reset_Handler(void);
void section_bss_clear(void);

csi_spiflash_t spiflash;
csi_gpio_t chip_gpio_handler;

static void __memcpy(void *dest, const void *src, size_t n)
{
    int i;

    for (i = 0; i < n; i++) {
        *((uint8_t *)dest + i) = *((uint8_t *)src + i);
    }
}

static void section_data_copy(void)
{
    extern uint32_t __stext;
    extern uint32_t __etext;
    extern uint32_t __srodata;
    extern uint32_t __flash_code_end__;
    extern uint32_t __erodata;
    extern uint32_t __data_start__;
    extern uint32_t __data_end__;
    extern uint32_t __ram_code_start__;
    extern uint32_t __ram_code_end__;
    // extern uint32_t __tcm0_start__;
    // extern uint32_t __tcm0_end__;

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
}

static void section_init(void)
{
    section_data_copy();
    section_bss_clear();
}

static void config_irq_priority(void)
{
    int i;

    for (i = 1; i <= 51; i++) {
        csi_vic_set_prio(i, 1);
    }

    csi_vic_set_prio(DW_DMAC1_IRQn, 0);
}

static void vic_init(void)
{
    /* Clear active and pending IRQ, and disable IRQ */
    VIC->IABR[0] = 0x0;
    VIC->ICPR[0] = 0xFFFFFFFF;
    VIC->ICER[0] = 0xFFFFFFFF;
    VIC->IABR[1] = 0x0;
    VIC->ICPR[1] = 0xFFFFFFFF;
    VIC->ICER[1] = 0xFFFFFFFF;
}

static void interrupt_init(void)
{
    vic_init();

    config_irq_priority();
#ifdef CONFIG_KERNEL_NONE
    __enable_excp_irq();
#else
#if defined(CONFIG_SUPPORT_TSPEND)
    __set_Int_SP((uint32_t)&g_top_irqstack);
    __set_CHR(__get_CHR() | CHR_ISE_Msk);
    VIC->TSPR = 0xFF;
#endif
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
    csi_cache_set_range(0, 0x18000000, CACHE_CRCR_16M, 1);
    csi_cache_set_range(1, 0x08000000, CACHE_CRCR_16M, 1);
    csi_icache_enable();
    csi_dcache_enable();

    section_init();
    interrupt_init();

    csi_gpio_init(&chip_gpio_handler, 0);

    int32_t ret;
    ret = csi_spiflash_qspi_init(&spiflash, 0, NULL);

    if (ret == CSI_OK) {
        csi_spiflash_config_data_line(&spiflash, SPIFLASH_DATA_4_LINES);
        csi_spiflash_frequence(&spiflash, 35000000U);
    } else {
        while (1);
    }
}
