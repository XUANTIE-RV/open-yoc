/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     system.c
 * @brief    CSI Device System Source File
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/

#include "soc.h"
#include "io.h"
#include "csi_core.h"
#include "drv/timer.h"
#include "drv/irq.h"


#ifndef CONFIG_SYSTICK_HZ
#define CONFIG_SYSTICK_HZ 100
#endif

#ifndef CONFIG_LPM_TICKLESS_SYSTIM
#define CONFIG_LPM_TICKLESS_SYSTIM 0
#endif

#ifndef CONFIG_LPM_TICKLESS_CNTTIM
#define CONFIG_LPM_TICKLESS_CNTTIM 1
#endif

int g_system_clock = 24000000;

extern uint32_t __Vectors[];
extern timer_handle_t system_timer;
extern timer_handle_t count_timer;
extern void systick_handler(void);
extern void irq_vectors_init(void);
extern void mm_heap_initialize(void);

#ifdef CONFIG_LPM
static void config_system_clock(void)
{
    /* clock open module: ETB, Coretim, SMS, APB0, APB1, SASC, SIPC, GPIO0, GPIO1

       clock close module: SPI0, SPI1, WDT, RTC, PWM,
       UART0, UART1, UART2, I2C0, I2C1, TIM0, TIM1, I2S, ADC, ACMP,
       DMA0, DMA1, CRC, RTC1, SHA, TRNG, AES, RSA
    */
    putreg32(0x4e098001, (uint32_t *)CLOCK_CONTROL_REGISTER0);
    putreg32(0x10, (uint32_t *)CLOCK_CONTROL_REGISTER1);
}
#endif

#ifdef CONFIG_KERNEL_NONE
static void _system_init_for_baremetal(void)
{
    csi_coret_config(drv_get_sys_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);    //10ms

    __enable_excp_irq();

    mm_heap_initialize();
}
#endif

#ifndef CONFIG_KERNEL_NONE
static void _system_init_for_kernel(void)
{
#ifndef CONFIG_SUPPORT_TSPEND
    irq_vectors_init();
#endif

    csi_coret_config(drv_get_sys_freq() / CONFIG_SYSTICK_HZ, CORET_IRQn);    //10ms
    drv_irq_enable(CORET_IRQn);

#ifndef CONFIG_KERNEL_RHINO
#ifndef CONFIG_NUTTXMM_NONE
    mm_heap_initialize();
#endif
#endif
}
#endif

/**
  * @brief  initialize the system
  *         Initialize the psr and vbr.
  * @param  None
  * @return None
  */
__attribute__((weak)) void SystemInit(void)
{
    __set_VBR((uint32_t) & (__Vectors));

    /* Clear active and pending IRQ, and disable IRQ */
    VIC->IABR[0] = 0x0;
    VIC->ICPR[0] = 0xFFFFFFFF;
    VIC->ICER[0] = 0xFFFFFFFF;

#ifdef CONFIG_SYSTEM_SECURE
    drv_set_sys_freq(EHS_CLK, OSR_8M_CLK_24M);
#endif

    /* to reduce power consumption */
#ifdef CONFIG_LPM
    config_system_clock();
#endif

#ifdef CONFIG_KERNEL_NONE
    _system_init_for_baremetal();
#else
    _system_init_for_kernel();
#endif
}
