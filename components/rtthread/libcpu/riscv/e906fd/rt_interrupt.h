/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-10-19     JasonHu      first version
 */

#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <rthw.h>

#define NR_CPUS       1

#define IRQ_OFFSET          16
#ifndef IRQ_MAX_NR
#define IRQ_MAX_NR          240
#endif
#define INTERRUPTS_MAX      (IRQ_OFFSET + IRQ_MAX_NR)

enum {
    EP_INSTRUCTION_ADDRESS_MISALIGNED = 0,
    EP_INSTRUCTION_ACCESS_FAULT,
    EP_ILLEGAL_INSTRUCTION,
    EP_BREAKPOINT,
    EP_LOAD_ADDRESS_MISALIGNED,
    EP_LOAD_ACCESS_FAULT,
    EP_STORE_ADDRESS_MISALIGNED,
    EP_STORE_ACCESS_FAULT,
    EP_ENVIRONMENT_CALL_U_MODE,
    EP_RESERVED9,
    EP_RESERVED10,
    EP_ENVIRONMENT_CALL_M_MODE,
    EP_RESERVED12,
    EP_RESERVED23 = 23,
    EP_NMI
};

void rt_hw_interrupt_init(void);
void rt_hw_interrupt_mask(int vector);
void rt_hw_interrupt_umask(int vector);
rt_isr_handler_t rt_hw_interrupt_install(int vector, rt_isr_handler_t handler, void *param, const char *name);

#endif
