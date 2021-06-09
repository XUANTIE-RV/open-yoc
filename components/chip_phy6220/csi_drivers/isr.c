/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     isr.c
 * @brief    source file for the interrupt server route
 * @version  V1.0
 * @date     15. May 2019
 ******************************************************************************/

#include <drv/common.h>
#include "soc.h"
#include <k_api.h>
#ifndef CONFIG_KERNEL_NONE
#include <aos/kernel.h>
#endif

extern void dw_usart_irqhandler(int32_t idx);
extern void dw_timer_irqhandler(int32_t idx);
extern void dw_gpio_irqhandler(int32_t idx);
extern void dw_iic_irqhandler(int32_t idx);
extern void ck_rtc_irqhandler(int32_t idx);
extern void dw_spi_irqhandler(int32_t idx);
extern void dw_wdt_irqhandler(int32_t idx);
extern void ck_dma_irqhandler(int32_t idx);
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

extern kstat_t krhino_intrpt_enter(void);
extern void krhino_intrpt_exit(void);

#ifndef CONFIG_KERNEL_NONE
#define CSI_INTRPT_ENTER() krhino_intrpt_enter()
#define CSI_INTRPT_EXIT()  krhino_intrpt_exit()
#else
#define CSI_INTRPT_ENTER()
#define CSI_INTRPT_EXIT()
#endif

static uint32_t _coret_to_osal_tick = 0;
extern uint32_t osal_sys_tick;
void CORET_IRQHandler(void)
{
    static uint32_t count = 0;
#ifndef CONFIG_KERNEL_FREERTOS
    CSI_INTRPT_ENTER();
#endif
    uint32_t cs = csi_irq_save();
    readl(0xE000E010);
    readl(0xE000E010);
    csi_irq_restore(cs);

#if defined(CONFIG_KERNEL_RHINO)
    krhino_tick_proc();
#elif defined(CONFIG_KERNEL_FREERTOS)
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_UCOS)
    OSTimeTick();
#endif
    _coret_to_osal_tick += 1000;
    do {
         _coret_to_osal_tick -= 625;
         osal_sys_tick += 1;
    } while(_coret_to_osal_tick > 625);

#ifndef CONFIG_KERNEL_FREERTOS
    CSI_INTRPT_EXIT();
#endif

   unsigned int v = *((volatile unsigned int *) (0xE000E010));
    while((v & 0x10000) > 0)
    {
        v = *((volatile unsigned int *) (0xE000E010));
        count++;
    }
}

extern void dw_usart_irqhandler(int32_t idx);
extern void dw_gpio_irqhandler(int32_t idx);

void UART0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_usart_irqhandler(0);
    CSI_INTRPT_EXIT();
}
void UART1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_usart_irqhandler(1);
    CSI_INTRPT_EXIT();
}

void CSI_GPIO_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_gpio_irqhandler(0);
    CSI_INTRPT_EXIT();
}

void CSI_AP_TIMER_IRQHandler(void)
{
    if (AP_TIM1->status & 0x1) {
        dw_timer_irqhandler(0);
    }

    if (AP_TIM2->status & 0x1) {
        dw_timer_irqhandler(1);
    }

    if (AP_TIM3->status & 0x1) {
        dw_timer_irqhandler(2);
    }

    if (AP_TIM4->status & 0x1) {
        dw_timer_irqhandler(3);
    }

    if (AP_TIM5->status & 0x1) {
        dw_timer_irqhandler(4);
    }

    if (AP_TIM6->status & 0x1) {
        dw_timer_irqhandler(5);
    }
}

//void CSI_RTC_IRQHandler(void)
//{
//    CSI_INTRPT_ENTER();
//    phy_rtc_irqhandler(0);
//    CSI_INTRPT_EXIT();
//}

void CSI_SPI0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_spi_irqhandler(0);
    CSI_INTRPT_EXIT();
}

void CSI_SPI1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_spi_irqhandler(1);
    CSI_INTRPT_EXIT();
}

void CSI_IIC0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_iic_irqhandler(0);
    CSI_INTRPT_EXIT();
}

void CSI_IIC1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_iic_irqhandler(1);
    CSI_INTRPT_EXIT();
}

void CSI_DMAC_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_dmac_irqhandler(0);
    CSI_INTRPT_EXIT();
}

void CSI_AP_TIMER5_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(0);
    CSI_INTRPT_EXIT();
}

void CSI_AP_TIMER6_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    dw_timer_irqhandler(1);
    CSI_INTRPT_EXIT();
}

