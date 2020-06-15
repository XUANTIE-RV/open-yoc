/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdint.h>

/**
  \brief       enable irq.
  \param[in]   irq_num Number of IRQ.
  \return      None.
*/
void drv_irq_enable(uint32_t irq_num);

/**
  \brief       disable irq.
  \param[in]   irq_num Number of IRQ.
  \return      None.
*/
void drv_irq_disable(uint32_t irq_num);

/**
  \brief       register irq handler.
  \param[in]   irq_num Number of IRQ.
  \param[in]   irq_handler IRQ Handler.
  \return      None.
*/
void drv_irq_register(uint32_t irq_num, void *irq_handler);

/**
  \brief       unregister irq handler.
  \param[in]   irq_num Number of IRQ.
  \param[in]   irq_handler IRQ Handler.
  \return      None.
*/
void drv_irq_unregister(uint32_t irq_num);
