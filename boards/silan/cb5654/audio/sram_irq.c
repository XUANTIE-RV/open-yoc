/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <string.h>
#include "silan_syscfg.h"
#include "silan_adev.h"
#include "silan_voice_adc.h"
#include "silan_rtc_regs.h"

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })
#define __sREG32(addr, offset)    (*(volatile uint32_t*)((addr) + offset))

#define ATTRIBUTE_ISR __attribute__((isr))

void sram_CORET_IRQHandler(void);
extern void sram_silan_dmac_i2s_callback(DMA_REQUEST *req);
extern void sram_rtc_irq_set(int32_t ret);

SRAM_BSS int32_t g_sram_tick;
SRAM_BSS int32_t g_sram_tick_conv;

__attribute__((section(".sram.vectors"))) void (*g_sram_vector[64])(void) = {
};

SRAM_TEXT void sram_trap(void)
{
    while(1);
}

SRAM_TEXT ATTRIBUTE_ISR void sram_CORET_IRQHandler(void)
{
    g_sram_tick++;
    readl(0xE000E010);
}

SRAM_TEXT ATTRIBUTE_ISR void sram_UARTHandler(void)
{
    *(volatile uint32_t *)0x41010000; 
    *(volatile uint32_t *)0x41010000; 
    *(volatile uint32_t *)0x41010000;
}

SRAM_TEXT ATTRIBUTE_ISR void sram_ADMAC_IRQHandler(void)
{
    extern ADEV_I2S_HANDLE *pi2s_mic_dev;
    sram_silan_dmac_i2s_callback(pi2s_mic_dev->adev_cfg.rx_dma_req);
    *(volatile uint32_t *)0x43300008 = 0x3;
}

SRAM_TEXT ATTRIBUTE_ISR void sram_GPIO2_IRQHandler(void)
{
    *(volatile uint32_t*)(0x41080000 + 0x41c) |= (0x01UL << 12);
}

SRAM_TEXT ATTRIBUTE_ISR void sram_RTC_IRQHandler(void)
{
    if ((__sREG32(RTC_BASE, RTC_CS0)) & RTC_CS0_AF) {
        __sREG32(RTC_BASE, RTC_CS0) = __sREG32(RTC_BASE, RTC_CS0) & (~RTC_CS0_AF);
    }
    sram_rtc_irq_set(1);
}

SRAM_TEXT void sram_irq_init(void)
{
    int i;

    for (i = 0; i < 64; i++) {
        g_sram_vector[i] = sram_trap;
    }

    g_sram_vector[63] = sram_CORET_IRQHandler;
    g_sram_vector[48] = sram_ADMAC_IRQHandler;
    g_sram_vector[36] = sram_UARTHandler;
    g_sram_vector[39] = sram_GPIO2_IRQHandler;
    g_sram_vector[52] = sram_RTC_IRQHandler;
}
