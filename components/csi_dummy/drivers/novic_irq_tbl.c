/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <csi_config.h>
#include <soc.h>

extern void Default_Handler(void);
extern void CORET_IRQHandler(void);

void (*g_irqvector[32])(void);

void irq_vectors_init(void)
{
    int i;

    for (i = 0; i < 32; i++) {
        g_irqvector[i] = Default_Handler;
    }

    g_irqvector[CORET_IRQn] = CORET_IRQHandler;
}

