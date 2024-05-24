/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/10/28     Bernard      The unify RISC-V porting implementation
 * 2018/12/27     Jesven       Add SMP support
 * 2021/02/02     lizhirui     Add userspace support
 */

#include "cpuport.h"
#include "stackframe.h"

    .globl rt_hw_context_switch_to
rt_hw_context_switch_to:
    LOAD sp, (a0)

    RESTORE_ALL
    mret
/*
 * void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to);
 *
 * a0 --> from
 * a1 --> to
 */
    .globl rt_hw_context_switch_interrupt
    .globl rt_hw_context_switch
rt_hw_context_switch:
rt_hw_context_switch_interrupt:
    /* set rt_thread_switch_interrupt_flag to 1 */
    la    t0, rt_thread_switch_interrupt_flag
    lw    t2, 0(t0)
    bnez  t2, _reswitch
    li    t1, 1
    sw    t1, 0(t0)

    /* set rt_interrupt_from_thread */
    la    t0, rt_interrupt_from_thread
    sw    a0, 0(t0)

_reswitch:
    /* set rt_interrupt_to_thread */
    la    t0, rt_interrupt_to_thread
    sw    a1, 0(t0)

    /* trigger the tspend exception (causes context switch) */
    li    t0, 0xE080100C
    lb    t1, (t0)
    li    t2, 0x01
    or    t1, t1, t2
    sb    t1, (t0)

    /* FIXME: trriger soft-irq quickly */
    fence
    nop
    nop
    nop
    nop
    nop

    ret
