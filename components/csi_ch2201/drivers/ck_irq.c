/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
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

/**
  \brief       enable irq.
  \param[in]   irq_num Number of IRQ.
  \return      None.
*/
void drv_irq_enable(uint32_t irq_num)
{
#ifdef CONFIG_SYSTEM_SECURE
    csi_vic_enable_sirq(irq_num);
#else
    csi_vic_enable_irq(irq_num);
#endif
}

/**
  \brief       disable irq.
  \param[in]   irq_num Number of IRQ.
  \return      None.
*/
void drv_irq_disable(uint32_t irq_num)
{
#ifdef CONFIG_SYSTEM_SECURE
    csi_vic_disable_sirq(irq_num);
#else
    csi_vic_disable_irq(irq_num);
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
    g_irqvector[irq_num] = irq_handler;
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
    g_irqvector[irq_num] = (void *)Default_Handler;
#endif
}
