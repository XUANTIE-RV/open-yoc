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

#include "stackframe.h"

.equ RISCV_MSTATUS_MIE,        (1<<3)       /*machine-level interrupt bit*/

  .section      .text.entry
  .align 3
  .extern __stack_cpu0
  .extern g_top_irqstack

  .global Mcoret_Handler
Mcoret_Handler:
    SAVE_ALL
    /* switch to interrupt stack */
    move  s0, sp

    /* get cpu id */
    csrr  t0, mhartid

    /* switch interrupt stack of current cpu */
    la    sp, g_base_irqstack
    addi  t1, t0, 1
    li    t2, CONFIG_ARCH_INTERRUPTSTACK
    mul   t1, t1, t2
    add   sp, sp, t1 /* sp = (cpuid + 1) * CONFIG_ARCH_INTERRUPTSTACK + g_base_irqstack */

    call  rt_interrupt_enter
    csrr  a0, mcause
    csrrc a1, mtval, zero
    csrr  a2, mepc
    mv    a3, sp

    /* mcause, mtval, mepc, sp */
    call  CORET_IRQHandler
    call  rt_interrupt_leave

#ifdef RT_USING_SMP
    /* s0 --> sp */
    mv   sp, s0
    mv   a0, s0
    call rt_scheduler_do_irq_switch

    RESTORE_ALL
    mret

#else
    /* switch to from_thread stack */
    move  sp, s0

    /* need to switch new thread */
    la    s0, rt_thread_switch_interrupt_flag
    lw    s2, 0(s0)
    beqz  s2, spurious_interrupt1
    sw    zero, 0(s0)

    la    s0, rt_interrupt_from_thread
    LOAD  s1, 0(s0)
    STORE sp, 0(s1)

    la    s0, rt_interrupt_to_thread
    LOAD  s1, 0(s0)
    LOAD  sp, 0(s1)

#endif
spurious_interrupt1:
    RESTORE_ALL

    mret


  .global Mirq_Handler
Mirq_Handler:
    SAVE_ALL
    /* switch to interrupt stack */
    move  s0, sp

    /* get cpu id */
    csrr  t0, mhartid

    /* switch interrupt stack of current cpu */
    la    sp, g_base_irqstack
    addi  t1, t0, 1
    li    t2, CONFIG_ARCH_INTERRUPTSTACK
    mul   t1, t1, t2
    add   sp, sp, t1 /* sp = (cpuid + 1) * CONFIG_ARCH_INTERRUPTSTACK + g_base_irqstack */
    call  rt_interrupt_enter
    call  do_irq
    call  rt_interrupt_leave
#ifdef RT_USING_SMP
    /* s0 --> sp */
    mv  sp, s0
    mv  a0, s0
    call rt_scheduler_do_irq_switch

    RESTORE_ALL
    mret

#else
    /* switch to from_thread stack */
    move  sp, s0

    /* need to switch new thread */
    la    s0, rt_thread_switch_interrupt_flag
    lw    s2, 0(s0)
    beqz  s2, spurious_interrupt
    sw    zero, 0(s0)

    la    s0, rt_interrupt_from_thread
    LOAD  s1, 0(s0)
    STORE sp, 0(s1)

    la    s0, rt_interrupt_to_thread
    LOAD  s1, 0(s0)
    LOAD  sp, 0(s1)

#endif
spurious_interrupt:
    RESTORE_ALL

    mret

  .section      .text.entry
  .align 3
  .global Mtspend_Handler
Mtspend_Handler:
    SAVE_ALL

    /* switch to interrupt stack */
    move  s0, sp

    /* get cpu id */
    csrr  t0, mhartid

    /* switch interrupt stack of current cpu */
    la    sp, g_base_irqstack
    addi  t1, t0, 1
    li    t2, CONFIG_ARCH_INTERRUPTSTACK
    mul   t1, t1, t2
    add   sp, sp, t1 /* sp = (cpuid + 1) * CONFIG_ARCH_INTERRUPTSTACK + g_base_irqstack */
    call  rt_interrupt_enter
    call  ipi_irq
    call  rt_interrupt_leave
#ifdef RT_USING_SMP
    /* s0 --> sp */
    mv  sp, s0
    mv  a0, s0
    call rt_scheduler_do_irq_switch

    RESTORE_ALL
    mret

#else
    /* switch to from_thread stack */
    move  sp, s0

    /* need to switch new thread */
    la    s0, rt_thread_switch_interrupt_flag
    lw    s2, 0(s0)
    beqz  s2, spurious_interrupt2
    sw    zero, 0(s0)

    la    s0, rt_interrupt_from_thread
    LOAD  s1, 0(s0)
    STORE sp, 0(s1)

    la    s0, rt_interrupt_to_thread
    LOAD  s1, 0(s0)
    LOAD  sp, 0(s1)

#endif
spurious_interrupt2:
    RESTORE_ALL
    mret

#ifdef  RT_USING_SMP
#define rt_hw_interrupt_disable rt_hw_local_irq_disable
#define rt_hw_interrupt_enable  rt_hw_local_irq_enable
#endif

.global cpu_intrpt_restore
.global rt_hw_interrupt_enable
cpu_intrpt_restore:
rt_hw_interrupt_enable:
    csrs mstatus, a0    /* restore to old csr */
    jr ra

.global cpu_intrpt_save
.global rt_hw_interrupt_disable
cpu_intrpt_save:
rt_hw_interrupt_disable:
    csrrci a0, mstatus, 8  /*  clear MIE */
    jr ra

.global cpu_is_irq_enable
cpu_is_irq_enable:
    csrr a0, mstatus
    andi a0, a0, RISCV_MSTATUS_MIE
    ret
