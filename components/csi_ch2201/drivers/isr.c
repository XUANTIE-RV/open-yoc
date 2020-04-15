/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     isr.c
 * @brief    source file for the interrupt server route
 * @version  V1.0
 * @date     25. August 2017
 ******************************************************************************/
#include <drv/common.h>
#include <csi_config.h>
#include "soc.h"
#ifndef CONFIG_KERNEL_NONE
#include <csi_kernel.h>
#endif

extern void dw_usart_irqhandler(int32_t idx);
extern void dw_timer_irqhandler(int32_t idx);
extern void dw_gpio_irqhandler(int32_t idx);
extern void dw_iic_irqhandler(int32_t idx);
extern void ck_rtc_irqhandler(int32_t idx);
extern void dw_spi_irqhandler(int32_t idx);
extern void dw_wdt_irqhandler(int32_t idx);
extern void ck_dma_irqhandler(int32_t idx);
extern void ck_aes_irqhandler(int32_t idx);
extern void ck_sha_irqhandler(int32_t idx);
extern void dw_dmac_irqhandler(int32_t idx);
extern void ck_adc_irqhandler(int32_t idx);
extern void ck_i2s_irqhandler(int32_t idx);
extern void ck_pwm_irqhandler(int32_t idx);
extern void systick_handler(void);
extern void xPortSysTickHandler(void);
extern void OSTimeTick(void);

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#ifndef CONFIG_KERNEL_NONE
#define CSI_INTRPT_ENTER() csi_kernel_intrpt_enter()
#define CSI_INTRPT_EXIT()  csi_kernel_intrpt_exit()
#else
#define CSI_INTRPT_ENTER()
#define CSI_INTRPT_EXIT()
#endif

#if defined(CONFIG_SUPPORT_TSPEND) || defined(CONFIG_KERNEL_NONE)
#define ATTRIBUTE_ISR __attribute__((isr))
#else
#define ATTRIBUTE_ISR
#endif

ATTRIBUTE_ISR void CORET_IRQHandler(void)
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

ATTRIBUTE_ISR void TIMA0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIMA1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIMB0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(2);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIMB1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(3);
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

ATTRIBUTE_ISR void I2C0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_iic_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void I2C1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_iic_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void RTC_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_rtc_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void RTC1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_rtc_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void AES_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_aes_irqhandler(0);
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

ATTRIBUTE_ISR void SPI0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_spi_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void SPI1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_spi_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void WDT_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_wdt_irqhandler(0);
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

ATTRIBUTE_ISR void ADC_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_adc_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void I2S_IRQHandler(void)
{
    //CSI_INTRPT_ENTER();
    //ck_i2s_irqhandler(0);
    //CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void PWM_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    ck_pwm_irqhandler(0);
    CSI_INTRPT_EXIT();
}
