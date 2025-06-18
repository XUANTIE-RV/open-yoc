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

#ifdef RT_USING_SMP
#define SMP_EXT_IRQ_STACK_SIZE 4096
.section .bss
    .align  3
    .global g_base_mcore_irqstack
    .global g_top_mcore_irqstack
g_base_mcore_irqstack:
    .space SMP_EXT_IRQ_STACK_SIZE * CONFIG_NR_CPUS
g_top_mcore_irqstack:
    .global g_base_tspend_irqstack
    .global g_top_tspend_irqstack
g_base_tspend_irqstack:
    .space SMP_EXT_IRQ_STACK_SIZE * CONFIG_NR_CPUS
g_top_tspend_irqstack:
#endif

.section      .text.entry
.align 3

#ifdef RT_USING_SMP
    .global Mcoret_Handler
    .type   Mcoret_Handler, %function
Mcoret_Handler:
    SAVE_ALL
    /* switch to interrupt stack */
    move  s0, sp

    /* get cpu id */
    csrr  t0, mhartid

    /* switch interrupt stack of current cpu */
    la    sp, g_base_mcore_irqstack
    addi  t1, t0, 1
    li    t2, SMP_EXT_IRQ_STACK_SIZE
    mul   t1, t1, t2
    add   sp, sp, t1 /* sp = (cpuid + 1) * SMP_EXT_IRQ_STACK_SIZE + g_base_mcore_irqstack */

    call  CORET_IRQHandler

    # /* s0 --> sp */
    mv   sp, s0
    mv   a0, s0
#if CONFIG_INTC_CLIC_PLIC
#if CONFIG_SUPPORT_IRQ_NESTED
    csrr  t3, mhartid
    la    t0, irq_nested_level
    slli  t2, t3, 3    /* mhartid * 8 */
    add   t0, t0, t2
    ld    t1, (t0)
    bnez  t1, 1f
#endif
#endif
    call rt_scheduler_do_irq_switch
1:
    RESTORE_ALL
    mret

    .size   Mcoret_Handler, . - Mcoret_Handler
#endif

    .global tspend_handler
    .type   tspend_handler, %function
    .global Mtspend_Handler
    .type   Mtspend_Handler, %function
tspend_handler:
Mtspend_Handler:
    SAVE_ALL
#ifdef RT_USING_SMP
    /* switch to interrupt stack */
    move  s0, sp

    /* get cpu id */
    csrr  t0, mhartid

    /* switch interrupt stack of current cpu */
    la    sp, g_base_tspend_irqstack
    addi  t1, t0, 1
    li    t2, SMP_EXT_IRQ_STACK_SIZE
    mul   t1, t1, t2
    add   sp, sp, t1 /* sp = (cpuid + 1) * SMP_EXT_IRQ_STACK_SIZE + g_base_tspend_irqstack */
    call  rt_interrupt_enter
    call  ipi_irq
    call  rt_interrupt_leave

    /* s0 --> sp */
    mv  sp, s0
    mv  a0, s0
#if CONFIG_INTC_CLIC_PLIC
#if CONFIG_SUPPORT_IRQ_NESTED
    csrr  t3, mhartid
    la    t0, irq_nested_level
    slli  t2, t3, 3    /* mhartid * 8 */
    add   t0, t0, t2
    ld    t1, (t0)
    bnez  t1, 1f
#endif
#endif
    call rt_scheduler_do_irq_switch
1:
#else
    /* get rt_thread_switch_interrupt_flag */
    la    t0, rt_thread_switch_interrupt_flag
    LOAD  t2, 0(t0)
    beqz  t2, tspend_exit       /* tspend already handled */
    /* clear rt_thread_switch_interrupt_flag to 0 */
    STORE zero, 0(t0)

    /* switch thread */
    la    t0, rt_interrupt_from_thread
    LOAD  t1, 0(t0)
    STORE sp, 0(t1)

    la    t0, rt_interrupt_to_thread
    LOAD  t1, 0(t0)
    LOAD  sp, 0(t1)

tspend_exit:
#if !CONFIG_INTC_CLIC_PLIC
    /* clear tspend */
    li    t0, RISCV_VIC_TSPDR
    li    t2, 0x0
    sw    t2, 0(t0)
#endif
#endif /* RT_USING_SMP */

    RESTORE_ALL
    mret

    .size   tspend_handler, . - tspend_handler
    .size   Mtspend_Handler, . - Mtspend_Handler

#ifdef  RT_USING_SMP
#define rt_hw_interrupt_disable rt_hw_local_irq_disable
#define rt_hw_interrupt_enable  rt_hw_local_irq_enable
#endif

.global cpu_intrpt_restore
.type   cpu_intrpt_restore, %function
.global rt_hw_interrupt_enable
.type   rt_hw_interrupt_enable, %function
cpu_intrpt_restore:
rt_hw_interrupt_enable:
    csrs mstatus, a0    /* restore to old csr */
    jr ra

.size cpu_intrpt_restore, . - cpu_intrpt_restore
.size rt_hw_interrupt_enable, . - rt_hw_interrupt_enable

.global cpu_intrpt_save
.type   cpu_intrpt_save, %function
.global rt_hw_interrupt_disable
.type   rt_hw_interrupt_disable, %function
cpu_intrpt_save:
rt_hw_interrupt_disable:
    csrrci a0, mstatus, 8  /*  clear MIE */
    jr ra

.size cpu_intrpt_save, . - cpu_intrpt_save
.size rt_hw_interrupt_disable, . - rt_hw_interrupt_disable

.global cpu_is_irq_enable
.type   cpu_is_irq_enable, %function
cpu_is_irq_enable:
    csrr a0, mstatus
    andi a0, a0, RISCV_MSTATUS_MIE
    ret

.size cpu_is_irq_enable, . - cpu_is_irq_enable
