/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

/******************************************************************************
 * @file     irq.c
 * @brief    CSI Source File for IRQ Driver
 * @version  V1.0
 * @date     7. April 2020
 ******************************************************************************/

#include <stdint.h>
#include <soc.h>
#include <csi_core.h>
#include <csi_config.h>
#include <drv/common.h>
#ifndef CONFIG_KERNEL_NONE
#include <aos/kernel.h>
#endif

extern void Default_Handler(void);
extern uint32_t soc_irq_get_irq_num(void);

#ifndef CONFIG_KERNEL_NONE
#define CSI_INTRPT_ENTER() aos_kernel_intrpt_enter()
#define CSI_INTRPT_EXIT()  aos_kernel_intrpt_exit()
#else
#define CSI_INTRPT_ENTER()
#define CSI_INTRPT_EXIT()
#endif

uint32_t  g_irq_nested_level;
csi_dev_t *g_irq_table[CONFIG_IRQ_NUM];

/**
  \brief       register irq handler.
  \param[in]   irq_num Number of IRQ.
  \return      None.
*/
void csi_irq_attach(uint32_t irq_num, void *irq_handler, csi_dev_t *dev)
{
    dev->irq_handler = irq_handler;
    g_irq_table[irq_num] = dev;
}

/**
  \brief       unregister irq handler.
  \param[in]   irq_num Number of IRQ.
  \param[in]   irq_handler IRQ Handler.
  \return      None.
*/
void csi_irq_detach(uint32_t irq_num)
{
    g_irq_table[irq_num]->irq_handler = (void *)Default_Handler;
}

/**
  \brief       gets whether in irq context
  \return      true or false.
*/
bool csi_irq_context(void)
{
    return ((g_irq_nested_level > 0U) ? true : false);
}

/**
  \brief       dispatching irq handlers
  \return      None.
*/
void do_irq(void)
{
    uint32_t irqn;

    g_irq_nested_level++;

    CSI_INTRPT_ENTER();

    ///< 获取是哪个中断发生了（irqn）
    irqn = soc_irq_get_irq_num();

    if (irqn == Machine_External_IRQn)
        irqn =  *(uint32_t *)(PLIC_BASE + 0x200000 + 4) & 0x3FF;

    if (g_irq_table[irqn] && g_irq_table[irqn]->irq_handler) {
        g_irq_table[irqn]->irq_handler(g_irq_table[irqn]);

        *(volatile uint32_t *)(PLIC_BASE + 0x200000 + 4) = irqn;
    } else {
        Default_Handler();
    }

    CSI_INTRPT_EXIT();

    g_irq_nested_level--;
}
