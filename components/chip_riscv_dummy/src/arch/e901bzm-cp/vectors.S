 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "riscv_asm_macro.h"

/* Enable interrupts when returning from the handler */
#define MSTATUS_PRV1 0x1880

.section .stack, "aw", @nobits
    .align  2
    .global g_trapstackbase
    .global g_top_trapstack
g_trapstackbase:
    .space CONFIG_ARCH_INTERRUPTSTACK
g_top_trapstack:

.text
    .align  2
    .weak   Default_IRQHandler
    .type   Default_IRQHandler, %function
Default_IRQHandler:
#if CONFIG_PROFILING_PERF && CONFIG_PERF_BACKTRACE_USE_FP
    addi    sp, sp, -4
    sw      s0, (sp)
#endif
    csrw    mscratch, sp
    la      sp, g_top_irqstack
    addi    sp, sp, -48
    sw      t0, 4(sp)
    sw      t1, 8(sp)
    csrr    t0, mepc
    csrr    t1, mcause
    sw      t1, 40(sp)
    sw      t0, 44(sp)

    sw      ra, 0(sp)
    sw      t2, 12(sp)
    sw      a0, 16(sp)
    sw      a1, 20(sp)
    sw      a2, 24(sp)
    sw      a3, 28(sp)
    sw      a4, 32(sp)
    sw      a5, 36(sp)

    la      t0, do_irq
    jalr    t0

    lw      a1, 40(sp)
    andi    a0, a1, 0x3FF
    slli    a0, a0, 2

    /* clear pending */
    li      a2, 0xE0801000
    add     a2, a2, a0
    lb      a3, 0(a2)
    li      a4, 1
    not     a4, a4
    and     a5, a4, a3
    sb      a5, 0(a2)

    li      t0, MSTATUS_PRV1
    csrs    mstatus, t0
    csrw    mcause, a1

    lw      t0, 44(sp)
    csrw    mepc, t0
    lw      ra, 0(sp)
    lw      t0, 4(sp)
    lw      t1, 8(sp)
    lw      t2, 12(sp)
    lw      a0, 16(sp)
    lw      a1, 20(sp)
    lw      a2, 24(sp)
    lw      a3, 28(sp)
    lw      a4, 32(sp)
    lw      a5, 36(sp)

    addi    sp, sp, 48
    csrr    sp, mscratch

#if CONFIG_PROFILING_PERF && CONFIG_PERF_BACKTRACE_USE_FP
    addi    sp, sp, 4
#endif

    mret
/******************************************************************************
 * Functions:
 *     void trap(void);
 * default exception handler
 ******************************************************************************/
    .align  2
    .global trap
    .type   trap, %function
trap:
    csrw    mscratch, sp
    la      sp, g_top_trapstack
    addi    sp, sp, -76
    sw      x1, (0 )(sp)
    sw      x3, (8 )(sp)
    sw      x4, (12)(sp)
    sw      x5, (16)(sp)
    sw      x6, (20)(sp)
    sw      x7, (24)(sp)
    sw      x8, (28)(sp)
    sw      x9, (32)(sp)
    sw      x10,(36)(sp)
    sw      x11,(40)(sp)
    sw      x12,(44)(sp)
    sw      x13,(48)(sp)
    sw      x14,(52)(sp)
    sw      x15,(56)(sp)
    csrr    a0, mepc
    sw      a0, (60)(sp)
    csrr    a0, mstatus
    sw      a0, (64)(sp)
    csrr    a0, mcause
    sw      a0, (68)(sp)
    csrr    a0, mtval
    sw      a0, (72)(sp)
    csrr    a0, mscratch
    sw      a0, (4 )(sp)

    mv      a0, sp
    la      a1, exceptionHandler
    jalr    a1


.section .mtvec, "ax", @progbits
    .weak   Default_Handler
    .type   Default_Handler, %function
Default_Handler:
    j      trap

    .size   Default_Handler, . - Default_Handler

.section .text
/*    Macro to define default handlers. Default handler
 *    will be weak symbol and just dead loops. They can be
 *    overwritten by other handlers */
    .macro  def_irq_handler handler_name
    .weak   \handler_name
    .set    \handler_name, Default_Handler
    .endm

    def_irq_handler tspend_handler
