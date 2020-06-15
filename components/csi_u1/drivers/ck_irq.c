/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     ck_irq.c
 * @brief    CSI Source File for IRQ Driver
 * @version  V1.0
 * @date     21. Dec 2018
 ******************************************************************************/

#include <stdint.h>
#include <csi_core.h>

extern void Default_Handler(void);
extern void (*g_irqvector[])(void);
extern void (*g_pwr_irqvector[])(void);

/**
  \brief       enable irq.
  \param[in]   irq_num Number of IRQ.
  \return      None.
*/
void drv_irq_enable(uint32_t irq_num)
{
    volatile uint32_t *pwr_intr_en = (uint32_t *)0xb0040128;
#ifdef CONFIG_SYSTEM_SECURE
    csi_vic_enable_sirq(irq_num);
#else
    if (irq_num < 32) {
        csi_vic_enable_irq(irq_num);
    } else {
        *pwr_intr_en |= (1 << (irq_num - 32));
    }
#endif
}

/**
  \brief       disable irq.
  \param[in]   irq_num Number of IRQ.
  \return      None.
*/
void drv_irq_disable(uint32_t irq_num)
{
    volatile uint32_t *pwr_intr_en = (uint32_t *)0xb0040128;
#ifdef CONFIG_SYSTEM_SECURE
    csi_vic_disable_sirq(irq_num);
#else
    if (irq_num < 32) {
        csi_vic_disable_irq(irq_num);
    } else {
        *pwr_intr_en &= ~(1 << (irq_num - 32));
    }
#endif
}

/**
  \brief       register irq handler.
  \param[in]   irq_num Number of IRQ.
  \return      None.
*/
void drv_irq_register(uint32_t irq_num, void *irq_handler)
{
#ifdef CONFIG_KERNEL_NONE
    csi_vic_set_vector(irq_num, (uint32_t)irq_handler);
#else
    if (irq_num < 32) {
        g_irqvector[irq_num] = irq_handler;
    } else {
        g_pwr_irqvector[irq_num-32] = irq_handler;
    }
#endif
}

/**
  \brief       unregister irq handler.
  \param[in]   irq_num Number of IRQ.
  \param[in]   irq_handler IRQ Handler.
  \return      None.
*/
void drv_irq_unregister(uint32_t irq_num)
{
#ifdef CONFIG_KERNEL_NONE
    csi_vic_set_vector(irq_num, (uint32_t)Default_Handler);
#else
    if (irq_num < 32) {
        g_irqvector[irq_num] = (void *)Default_Handler;
    } else {
        g_pwr_irqvector[irq_num-32] = (void *)Default_Handler;
    }
#endif
}
