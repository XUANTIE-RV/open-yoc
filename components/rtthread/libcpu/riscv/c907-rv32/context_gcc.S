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

#ifdef RT_USING_SMP
    mv    a0, a1
    call  rt_cpus_lock_status_restore
#endif
    RESTORE_ALL
    mret
/*
 * void rt_hw_context_switch(rt_ubase_t from, rt_ubase_t to);
 *
 * a0 --> from
 * a1 --> to
 */
    .globl rt_hw_context_switch
rt_hw_context_switch:
    /* set mstatus.mpp */
    csrr t0, mxstatus
    srli t0, t0, 30
    andi t0, t0, 0x3
    slli t0, t0, 11
    csrs mstatus, t0//if enter here,caller must be in system thread
    csrw mepc, ra//return address
    //saved from thread context
    SAVE_ALL

    STORE sp, (a0)

    //restore to thread context
    LOAD sp, (a1)

#ifdef RT_USING_SMP
    mv   a0, a2
    call  rt_cpus_lock_status_restore
#endif /*RT_USING_SMP*/

    RESTORE_ALL

    mret

#ifdef RT_USING_SMP
/*
 * void rt_hw_context_switch_interrupt(void *context, rt_ubase_t from, rt_ubase_t to, struct rt_thread *to_thread);
 *
 * a0 --> context
 * a1 --> from
 * a2 --> to
 * a3 --> to_thread
 */
    .globl rt_hw_context_switch_interrupt
rt_hw_context_switch_interrupt:

    STORE a0, 0(a1)

    LOAD  sp, 0(a2)
    move  a0, a3
    call rt_cpus_lock_status_restore

    RESTORE_ALL

    mret
#endif /* RT_USING_SMP */
