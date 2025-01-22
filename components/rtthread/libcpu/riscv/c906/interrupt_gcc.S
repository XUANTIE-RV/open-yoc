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

.section      .text.entry
.align 3

.global Mtspend_Handler
.global Stspend_Handler
Mtspend_Handler:
Stspend_Handler:
    SAVE_ALL

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
    /* clear tspend */
    li    t0, RISCV_VIC_TSPDR
    li    t2, 0x0
    sw    t2, 0(t0)

    RESTORE_ALL
    mret

    .size   Mtspend_Handler, . - Mtspend_Handler
    .size   Stspend_Handler, . - Stspend_Handler

.global cpu_intrpt_restore
.global rt_hw_interrupt_enable
.global cpu_intrpt_save
.global rt_hw_interrupt_disable
.global cpu_is_irq_enable
#if defined(CONFIG_RISCV_SMODE) && CONFIG_RISCV_SMODE
cpu_intrpt_save:
rt_hw_interrupt_disable:
    csrrci a0, sstatus, SR_SIE
    ret

cpu_intrpt_restore:
rt_hw_interrupt_enable:
    csrw sstatus, a0
    ret

cpu_is_irq_enable:
    csrr a0, sstatus
    andi a0, a0, SR_SIE
    ret

#else

cpu_intrpt_restore:
rt_hw_interrupt_enable:
    csrw mstatus, a0
    ret

cpu_intrpt_save:
rt_hw_interrupt_disable:
    csrrci a0, mstatus, SR_MIE
    ret

cpu_is_irq_enable:
    csrr a0, mstatus
    andi a0, a0, SR_MIE
    ret
#endif