/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     isr.c
 * @brief    source file for the interrupt server route
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <csi_config.h>
#include "wm_regs.h"
#include <k_api.h>

extern void systick_handler(void);
extern void socv_SysTick_Handler(void);
extern void xPortSysTickHandler(void);
extern void OSTimeTick(void);
extern void SDIOA_IRQHandler(void);
extern void HSPI_IRQHandler(void);
extern void TIMER0_5_IRQHandler(void);
extern void CRYPTION_IRQHandler(void);
extern void RSA_F_IRQHandler(void);
extern void lspi_irqhandler(int32_t idx);
extern void iic_irqhandler(int32_t idx);
extern void i2s_irqhandler(int32_t idx);
extern void timer_irqhandler(int32_t idx);
extern void rtc_irqhandler(int32_t idx);
extern void pmu_wake_irqhandler(int idx);
extern void wdt_irqhandler(int32_t idx);
extern void uart_irqhandler(int32_t idx);
extern void GPIO_UART0_IRQHandler(void);
extern void adc_irqhandler(int32_t idx);

extern void __GPIOA_IRQHandler(void);
extern void __GPIOB_IRQHandler(void);
extern void __ADC_IRQHandler(void);
extern void __DMA_Channel0_IRQHandler(void);
extern void __DMA_Channel1_IRQHandler(void);
extern void __DMA_Channel2_IRQHandler(void);
extern void __DMA_Channel3_IRQHandler(void);
extern void __DMA_Channel4_7_IRQHandler(void);

extern void tls_wl_mac_isr(void);
extern void tls_wl_rx_isr(void);

extern int csi_kernel_intrpt_enter(void);
extern int csi_kernel_intrpt_exit(void);

extern void PMU_TIMER0_IRQHandler(void);
extern void PMU_TIMER1_IRQHandler(void);
extern void PMU_GPIO_WAKE_IRQHandler(void);
extern void PMU_RTC_IRQHandler(void);
#if (TLS_CONFIG_BLE == CFG_ON)
extern void wmble_isr();
extern void wmbt_isr();
#endif


//#define  ATTRIBUTE_ISR //__attribute__((isr))

#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#ifndef CONFIG_KERNEL_NONE
#define  CSI_INTRPT_ENTER() krhino_intrpt_enter()
#define  CSI_INTRPT_EXIT()  krhino_intrpt_exit()
#else
#define  CSI_INTRPT_ENTER()
#define  CSI_INTRPT_EXIT()
#endif

/*************** no wifi/bt/ble *****************/
__attribute__((weak)) void tls_wl_mac_isr(void)
{

}

__attribute__((weak)) void tls_wl_rx_isr(void)
{

}

__attribute__((weak)) void wmble_isr(void)
{

}

__attribute__((weak)) void wmbt_isr(void)
{

}
/*************** no wifi/bt/ble *****************/


//static int tick_test = 0;
ATTRIBUTE_ISR void CORET_IRQHandler(void)
{
//wm_printf("%s\n", __func__);
#ifndef CONFIG_KERNEL_FREERTOS
    CSI_INTRPT_ENTER();
#endif

    readl(0xE000E010);

#if defined(CONFIG_KERNEL_RHINO)
    krhino_tick_proc();
#elif defined(CONFIG_KERNEL_FREERTOS)
    xPortSysTickHandler();
#elif defined(CONFIG_KERNEL_UCOS)
    OSTimeTick();
#endif

#ifndef CONFIG_KERNEL_FREERTOS
    CSI_INTRPT_EXIT();
#endif
}

#if 0
ATTRIBUTE_ISR void SDIO_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
#ifndef WM_WIFI_SIMULATION_PROJECT
//    SDIOA_IRQHandler();
#endif
    CSI_INTRPT_EXIT();
}
#endif

ATTRIBUTE_ISR void GPIOA_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    __GPIOA_IRQHandler();
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void GPIOB_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    __GPIOB_IRQHandler();
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void ADC_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    adc_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void DMA_Channel0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    __DMA_Channel0_IRQHandler();
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void DMA_Channel1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    __DMA_Channel1_IRQHandler();
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void DMA_Channel2_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    __DMA_Channel2_IRQHandler();
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void DMA_Channel3_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    __DMA_Channel3_IRQHandler();
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void DMA_Channel4_7_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    __DMA_Channel4_7_IRQHandler();
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void GPSEC_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    CRYPTION_IRQHandler();
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void RSA_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    RSA_F_IRQHandler();
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void TIM0_5_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    //TIMER0_5_IRQHandler();
    timer_irqhandler(0);
    CSI_INTRPT_EXIT();
}

#if 0
ATTRIBUTE_ISR void SPI_HS_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
#ifndef WM_WIFI_SIMULATION_PROJECT
//    HSPI_IRQHandler();
#endif
    CSI_INTRPT_EXIT();
}
#endif

ATTRIBUTE_ISR void MAC_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
#ifndef CONFIG_NO_WIFI
    tls_wl_mac_isr();
#endif
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void SEC_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
#ifndef CONFIG_NO_WIFI
    tls_wl_rx_isr();
#endif
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void PMU_RTC_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
#if 0
    if (tls_reg_read32(HR_PMU_INTERRUPT_SRC) & BIT(0))
    {
    	PMU_TIMER0_IRQHandler();
    }
    if (tls_reg_read32(HR_PMU_INTERRUPT_SRC) & BIT(1)) /* timer1 interrupt */
    {
    	PMU_TIMER1_IRQHandler();
    }
#endif
    if (tls_reg_read32(HR_PMU_INTERRUPT_SRC) & BIT(2)) /* gpio wake interrupt */
    {
        tls_reg_write32(HR_PMU_INTERRUPT_SRC, BIT(2)); /* clear gpio wake interrupt */
    	pmu_wake_irqhandler(0);
    }
    if (tls_reg_read32(HR_PMU_INTERRUPT_SRC) & BIT(4)) /* rtc interrupt */
    {
    	rtc_irqhandler(0);
    }
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void UART0_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    uart_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void UART1_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    uart_irqhandler(1);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void UART234_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    uart_irqhandler(2);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void WDG_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    wdt_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void I2C_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    iic_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void I2S_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    i2s_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void SPI_LS_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
    lspi_irqhandler(0);
    CSI_INTRPT_EXIT();
}

ATTRIBUTE_ISR void BLE_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
#if (TLS_CONFIG_BLE == CFG_ON)	
    wmble_isr();
#endif
    CSI_INTRPT_EXIT();
}
ATTRIBUTE_ISR void BT_IRQHandler(void)
{
    CSI_INTRPT_ENTER();
#if (TLS_CONFIG_BLE == CFG_ON)
    wmbt_isr();
#endif
    CSI_INTRPT_EXIT();
}
