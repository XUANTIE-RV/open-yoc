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

#include "wm_irq.h"

extern void (*g_irqvector[])(void);
extern void Default_Handler(void);
extern void NOVIC_IRQ_Default_Handler(void);

/**
  \brief       enable irq.
  \param[in]   irq_num Number of IRQ.
  \return      None.
*/
void drv_irq_enable(uint32_t irq_num)
{
    tls_irq_enable(irq_num);
}

/**
  \brief       disable irq.
  \param[in]   irq_num Number of IRQ.
  \return      None.
*/
void drv_irq_disable(uint32_t irq_num)
{
    tls_irq_disable(irq_num);
}

/**
  \brief       register irq handler.
  \param[in]   irq_num Number of IRQ.
  \return      None.
*/
void drv_irq_register(uint32_t irq_num, void *irq_handler)
{
    csi_vic_set_vector(irq_num, (uint32_t)NOVIC_IRQ_Default_Handler);
    g_irqvector[irq_num] = irq_handler;
}

/**
  \brief       unregister irq handler.
  \param[in]   irq_num Number of IRQ.
  \param[in]   irq_handler IRQ Handler.
  \return      None.
*/
void drv_irq_unregister(uint32_t irq_num)
{
    csi_vic_set_vector(irq_num, (uint32_t)NOVIC_IRQ_Default_Handler);
    g_irqvector[irq_num] = (void *)Default_Handler;
}

