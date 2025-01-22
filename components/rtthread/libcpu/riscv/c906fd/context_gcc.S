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
    LOAD  t2, 0(t0)
    bnez  t2, _reswitch
    li    t1, 1
    STORE t1, 0(t0)

    /* set rt_interrupt_from_thread */
    la    t0, rt_interrupt_from_thread
    STORE a0, 0(t0)

_reswitch:
    /* set rt_interrupt_to_thread */
    la    t0, rt_interrupt_to_thread
    STORE a1, 0(t0)

    /* trigger the tspend exception (causes context switch) */
    li      t0, RISCV_VIC_TSPDR
    li      t2, 0x1
    sw      t2, 0(t0)

__loop:
    lw      t2, 0(t0)
    seqz    t1, t2
#if defined(CONFIG_RISCV_SMODE) && CONFIG_RISCV_SMODE
    csrr    t2, sip
    andi    t2, t2, 0x2
#else
    csrr    t2, mip
    andi    t2, t2, 0x8
#endif
    snez    t3, t2
    add     t2, t3, t1
    beqz    t2, __loop
    ret
