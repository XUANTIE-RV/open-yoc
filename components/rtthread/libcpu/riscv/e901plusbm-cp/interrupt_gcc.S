/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/10/02     Bernard      The first version
 * 2018/12/27     Jesven       Add SMP schedule
 * 2021/02/02     lizhirui     Add userspace support
 */

#include <asm/riscv_csr.h>
#include "stackframe.h"

.equ RISCV_MSTATUS_MIE,        (1<<3)       /*machine-level interrupt bit*/

  .section      .text.entry  
  .align 3

  .global tspend_handler
  .global Mtspend_Handler
tspend_handler:
Mtspend_Handler:
    SAVE_ALL

    /* get rt_thread_switch_interrupt_flag */
    la    t0, rt_thread_switch_interrupt_flag
    lw    t2, 0(t0)
    beqz  t2, tspend_exit       /* tspend already handled */
    /* clear rt_thread_switch_interrupt_flag to 0 */
    sw    zero, 0(t0)

    /* switch thread */
    la    t0, rt_interrupt_from_thread
    lw    t1, 0(t0)
    sw    sp, 0(t1)

    la    t0, rt_interrupt_to_thread
    lw    t1, 0(t0)
    lw    sp, 0(t1)

tspend_exit:
    RESTORE_ALL
    mret

    .size   tspend_handler, . - tspend_handler
    .size   Mtspend_Handler, . - Mtspend_Handler

.global cpu_intrpt_restore
.global rt_hw_interrupt_enable
cpu_intrpt_restore:
rt_hw_interrupt_enable:
    csrw    mstatus, a0
    ret

.global cpu_intrpt_save
.global rt_hw_interrupt_disable
cpu_intrpt_save:
rt_hw_interrupt_disable:
    csrr    a0, mstatus
    csrc    mstatus, 8
    ret

.global cpu_is_irq_enable
cpu_is_irq_enable:
    csrr a0, mstatus
    andi a0, a0, RISCV_MSTATUS_MIE
    ret
