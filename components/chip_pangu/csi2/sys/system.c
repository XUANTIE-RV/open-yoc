/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     system.c
 * @brief    CSI Device System Source File for YUNVOICE
 * @version  V1.0
 * @date     11. Aug 2020
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/

#include <csi_config.h>
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

#if (defined CONFIG_CHIP_PANGU_CPU0) || (defined CONFIG_CHIP_PANGU_CPU2)
static csi_dma_t dma_hd0;
static csi_dma_t dma_hd1;
#endif

#ifdef CONFIG_CHIP_PANGU_CPU0
csi_spiflash_t spiflash;
csi_gpio_t chip_gpio_handler;
#endif

uint32_t soc_get_cpu_id(void)
{
#ifdef CONFIG_CHIP_PANGU_CPU0
    return 0;
#endif

#ifdef CONFIG_CHIP_PANGU_CPU1
    return 1;
#endif

    return 2;
}

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

#endif
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

static void mpu_init(void)
{
#if defined(CONFIG_CHIP_PANGU_CPU1)
    mpu_region_attr_t attr;

    attr.nx = 0;
    attr.ap = AP_BOTH_RW;
    attr.s = 0;

    csi_mpu_config_region(0, 0U, REGION_SIZE_4GB, attr, 1);
    attr.nx = 1;
    csi_mpu_config_region(1, 0x20000000, REGION_SIZE_512MB, attr, 1);
    csi_mpu_config_region(2, 0x40000000, REGION_SIZE_1GB, attr, 1);
    //attention: not protect 0x1A000000, lpm would modify it.
    //attr.ap = AP_BOTH_INACCESSIBLE;
    //csi_mpu_config_region(3, 0x1A000000, REGION_SIZE_4KB, attr, 1);
    csi_mpu_enable();
#elif defined(CONFIG_CHIP_PANGU_CPU2)
    mpu_region_attr_t attr;

    attr.nx = 0;
    attr.ap = AP_BOTH_RW;
    attr.s = 0;

    csi_mpu_config_region(0, 0U, REGION_SIZE_4GB, attr, 1);
    attr.nx = 1;
    csi_mpu_config_region(1, 0x20000000, REGION_SIZE_512MB, attr, 1);
    csi_mpu_config_region(2, 0x40000000, REGION_SIZE_1GB, attr, 1);
    //attention: not protect 0x1A000000, lpm would modify it.
    //attr.ap = AP_BOTH_INACCESSIBLE;
    //csi_mpu_config_region(3, 0x1A000000, REGION_SIZE_4KB, attr, 1);
    csi_mpu_enable();
#else /* CPU0 */
    /* Bootloader inited MPU */
    /* 0x20000000 - 0x21000000: BOTH INACCESSIBLE */
#endif
}

/**
  * @brief  initialize the system
  *         Initialize the psr and vbr.
  * @param  None
  * @return None
  */

__attribute__((weak)) void SystemInit(void)
{
#if (defined CONFIG_CHIP_PANGU_CPU0)
    csi_cache_set_range(0, 0x18000000, CACHE_CRCR_16M, 1);
    csi_cache_set_range(1, 0x08000000, CACHE_CRCR_16M, 1);
    csi_icache_enable();
    csi_dcache_enable();
#endif

    section_init();
    interrupt_init();

    mpu_init();

#if (defined CONFIG_CHIP_PANGU_CPU0)
    soc_clk_enable(GPIO0_CLK);
    soc_clk_enable(DMA0_CLK);
    soc_clk_enable(DMA1_CLK);
    soc_clk_enable(CODEC_CLK);
    soc_clk_enable(TIM3_CLK);
    csi_gpio_init(&chip_gpio_handler, 0);
    csi_dma_init(&dma_hd0, 0);
    csi_dma_init(&dma_hd1, 1);
#endif

#if (defined CONFIG_CHIP_PANGU_CPU1)
    csi_cache_set_range(0, 0x18000000, CACHE_CRCR_16M, 1);
    csi_icache_enable();
    csi_dcache_enable();
#endif

#if (defined CONFIG_CHIP_PANGU_CPU2)
    soc_clk_enable(DMA0_CLK);
    csi_cache_set_range(0, 0x18000000, CACHE_CRCR_16M, 1);
    csi_icache_enable();
    csi_dcache_enable();
    csi_dma_init(&dma_hd0, 0);
#endif

    csi_tick_init();

#if (defined CONFIG_CHIP_PANGU_CPU0)
    int32_t ret;
    soc_clk_enable(QSPI_CLK);
    ret = csi_spiflash_qspi_init(&spiflash, 0, NULL);

    if (ret == CSI_OK) {
        csi_spiflash_config_data_line(&spiflash, SPIFLASH_DATA_4_LINES);
        csi_spiflash_frequence(&spiflash, 35000000U);
    } else {
        while (1);
    }

#endif
}
