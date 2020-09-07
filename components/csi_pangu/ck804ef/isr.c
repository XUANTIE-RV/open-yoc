/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */


/******************************************************************************
 * @file     isr.c
 * @brief    source file for the interrupt server route
 * @version  V1.0
 * @date     06. Mar 2019
 * @vendor   csky
 * @chip     pangu
 ******************************************************************************/
#include <drv/common.h>
#include "soc.h"
#ifndef CONFIG_KERNEL_NONE
#include <csi_kernel.h>
#endif

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#define writel(b,addr) (void)((*(volatile unsigned int *) (addr)) = (b))

extern void dw_dmac_irqhandler(int idx);
extern void dw_timer_irqhandler(int idx);
extern void ck_usi_irqhandler(int idx);
extern void ck_i2s_irqhandler(int32_t idx);
extern void dw_wdt_irqhandler(int32_t idx);
extern void ck_rtc_irqhandler(int32_t idx);
extern void dw_gpio_irqhandler(int32_t idx);
extern void ck_sha_irqhandler(int32_t idx);
extern void ck_dma_irqhandler(int32_t idx);
extern void ck_mailbox_irqhandler(int32_t idx);
extern void ck_pwm_irqhandler(int32_t idx);
extern void dw_usart_irqhandler(int32_t idx);
extern void systick_handler(void);
extern void xPortSysTickHandler(void);
extern void OSTimeTick(void);
extern void dw_sdio_irqhandler(int32_t idx);
extern void ck_codec_irqhandler(int32_t idx);
#ifndef CONFIG_KERNEL_NONE
#define  CSI_INTRPT_ENTER() csi_kernel_intrpt_enter()
#define  CSI_INTRPT_EXIT()  csi_kernel_intrpt_exit()
#else
#define  CSI_INTRPT_ENTER()
#define  CSI_INTRPT_EXIT()
#endif

#if defined(CONFIG_SUPPORT_TSPEND) || defined(CONFIG_KERNEL_NONE)
#define  ATTRIBUTE_ISR __attribute__((isr, no_icf))
#else
#define ATTRIBUTE_ISR
#endif

#if defined(CONFIG_FREQ_TEST)
unsigned int tick_count;
ATTRIBUTE_ISR  void CORET_IRQHandler(void)
{
	tick_count++;
#ifndef CONFIG_KERNEL_FREERTOS
    CSI_INTRPT_ENTER();
#endif

    readl(0xE000E010);

#if defined(CONFIG_KERNEL_RHINO)
    systick_handler();
#elif defined(CONFIG_KERNEL_FREERTOS)
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_UCOS)
    OSTimeTick();
#endif

#ifndef CONFIG_KERNEL_FREERTOS
    CSI_INTRPT_EXIT();
#endif

}
#else

ATTRIBUTE_ISR  void CORET_IRQHandler(void)
{
#ifndef CONFIG_KERNEL_FREERTOS
    CSI_INTRPT_ENTER();
#endif

    readl(0xE000E010);

#if defined(CONFIG_KERNEL_RHINO)
    systick_handler();
#elif defined(CONFIG_KERNEL_FREERTOS)
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_UCOS)
    OSTimeTick();
#endif

#ifndef CONFIG_KERNEL_FREERTOS
    CSI_INTRPT_EXIT();
#endif

}
#endif
ATTRIBUTE_ISR void USI0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_usi_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void USI1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_usi_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void USI2_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_usi_irqhandler(2);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void USI3_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_usi_irqhandler(3);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIM0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIM1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIM2_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(2);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIM3_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(3);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIM4_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(4);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIM5_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(5);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIM6_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(6);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIM7_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(7);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void PWM0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_pwm_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void PWM1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_pwm_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void GPIO0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_gpio_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void GPIO1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_gpio_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void DMAC0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_dmac_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void DMAC1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_dmac_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void SDIO_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_sdio_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void SDMMC_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_sdio_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void WDT_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_wdt_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void RTC_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_rtc_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void SHA_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_sha_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TRNG_IRQHandler(void)
{
    CSI_INTRPT_ENTER();

    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void RSA_IRQHandler(void)
{
    CSI_INTRPT_ENTER();

    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void MAESTRO_IRQHandler(void)
{
    writel(0x10, 0x80020210);
}

ATTRIBUTE_ISR void FSMC_IRQHandler(void)
{
    CSI_INTRPT_ENTER();

    writel(0x1, 0x3ffffd24);

    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void MAILBOX0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_mailbox_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void MAILBOX1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_mailbox_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void USART0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_usart_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void USART1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_usart_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void USART2_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_usart_irqhandler(2);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void USART3_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_usart_irqhandler(3);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void I2S0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_i2s_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void I2S1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_i2s_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void I2S2_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_i2s_irqhandler(2);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void I2S3_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_i2s_irqhandler(3);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void CODEC_GSK_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_codec_irqhandler(0);
    CSI_INTRPT_EXIT();
}


