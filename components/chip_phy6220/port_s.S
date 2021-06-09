/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <k_config.h>

#define VIC_IRQ0_PEND 0xE000E200

.extern g_active_task
.extern g_preferred_ready_task
.extern krhino_task_sched_stats_get
.extern krhino_stack_ovf_check
/******************************************************************************
 *                                 EXPORT FUNCTIONS
 ******************************************************************************/

.global cpu_intrpt_save
.global cpu_intrpt_restore
.weak   cpu_task_switch
.weak   cpu_intrpt_switch
.weak   cpu_first_task_start

.global tspend_handler

/******************************************************************************
 *                                 EQUATES
 ******************************************************************************/

/******************************************************************************
 *                                 CODE GENERATION DIRECTIVES
 ******************************************************************************/

.text
.align 2

/******************************************************************************
 * Functions:
 *     void cpu_intrpt_switch(void);
 *     void cpu_task_switch(void);
 ******************************************************************************/

.section .text.cpu_task_switch
.type cpu_task_switch, %function
cpu_task_switch:
    lrw    r0, VIC_IRQ0_PEND
    ldw    r1, (r0)
    ori    r1, r1, 1
    stw    r1, (r0)
    rts

.section .text.cpu_intrpt_switch
.type cpu_intrpt_switch, %function
cpu_intrpt_switch:
    lrw    r0, VIC_IRQ0_PEND
    ldw    r1, (r0)
    ori    r1, r1, 1
    stw    r1, (r0)
    rts

.section .bss
.global irq_nested_level
irq_nested_level:
.long 0
.global task_sp
task_sp:
.long 0

.text
.global Default_IRQHandler_phy6220
.type   Default_IRQHandler_phy6220, %function
Default_IRQHandler_phy6220:
    subi    sp, 8
    stw     r0, (sp, 0)
    stw     r1, (sp, 4)
    lrw     r0, irq_nested_level
    ldw     r1, (r0)
    addi    r1, 1
    stw     r1, (r0)
    cmpnei  r1, 1
    ldw     r0, (sp, 0)
    ldw     r1, (sp, 4)
    addi    sp, 8
    bt      .Lnested

    subi    sp, 68
    stm     r0-r13, (sp)
    stw     r15, (sp, 56)
    mfcr    r0, epsr
    stw     r0, (sp, 60)
    mfcr    r0, epc
    stw     r0, (sp, 64)
    lrw     r0, task_sp
    stw     sp, (r0)
    lrw     sp, 0x1fff0d88
    psrset  ie, ee
    br      .Lnonest

.Lnested:
    subi     sp, 36
    stm      r0-r3, (sp)
    mfcr     r0, epsr
    mfcr     r1, epc
    psrset   ie, ee
    stw      r12, (sp, 16)
    stw      r13, (sp, 20)
    stw      r15, (sp, 24)
    stw      r0, (sp, 28)
    stw      r1, (sp, 32)

.Lnonest:

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    //jbsr     krhino_stack_ovf_check
#endif

#ifdef CONFIG_SUPPORT_REE_SCHEDULE_IN_TEE
    //jbsr     csky_get_tee_caller_task
#endif

    lrw      r1, g_irqvector
    mfcr     r0, psr
    lsri     r0, 16
    sextb    r0
    subi     r0, 32
    lsli     r0, 2
    add      r1, r0
    ldw      r13, (r1)
    //lsri     r0, 2
    //mov      r12, r0
    //jbsr     krhino_intrpt_enter_hook
    // mov      r0, r12
    jsr      r13
    // mov      r0, r12
    //jbsr     krhino_intrpt_exit_hook

    psrclr  ie
    lrw     r0, irq_nested_level
    ldw     r1, (r0)
    subi    r1, 1
    stw     r1, (r0)
    cmpnei  r1, 0
    bt      .Lnested_ret

    lrw     r0, task_sp
    ldw     sp, (r0)
    ldw     r0, (sp, 64)
    mtcr    r0, epc
    ldw     r0, (sp, 60)
    mtcr    r0, epsr

    ldm     r0-r13, (sp)
    ldw     r15, (sp, 56)
    addi    sp, 68

    rte

.Lnested_ret:
    ldw      r0, (sp, 28)
    ldw      r1, (sp, 32)
    mtcr     r0, epsr
    mtcr     r1, epc
    ldm      r0-r3, (sp)
    ldw      r12, (sp, 16)
    ldw      r13, (sp, 20)
    ldw      r15, (sp, 24)
    addi     sp, 36
    rte

/******************************************************************************
 * Functions:
 *     void cpu_first_task_start(void);
 ******************************************************************************/

.section .text.cpu_first_task_start
.type cpu_first_task_start, %function
cpu_first_task_start:
    psrclr  ie
    jbr     __tspend_handler_nosave

/******************************************************************************
 * Functions:
 *     void __task_switch(void);
 ******************************************************************************/

.section .text.tspend_handler
.type tspend_handler, %function
tspend_handler:
    subi    sp, 68
    stm     r0-r13, (sp)
    stw     r15, (sp, 56)
    mfcr    r0, epsr
    stw     r0, (sp, 60)
    mfcr    r0, epc
    stw     r0, (sp, 64)

    lrw     r1, g_active_task
    ldw     r1, (r1)
    stw     sp, (r1)

    lrw     r0, g_sched_lock
    ldw     r1, (r0)
    cmpnei  r1, 0
    bt      .Lnot_switch

#if (RHINO_CONFIG_TASK_STACK_OVF_CHECK > 0)
    jbsr    krhino_stack_ovf_check
#endif

#if (RHINO_CONFIG_SYS_STATS > 0)
    jbsr    krhino_task_sched_stats_get
#endif

__tspend_handler_nosave:
    lrw     r4, g_active_task
    lrw     r5, g_preferred_ready_task
    ldw     r6, (r5)
    stw     r6, (r4)
    ldw     sp, (r6)

.Lnot_switch:
    ldw     r0, (sp, 64)
    mtcr    r0, epc
    ldw     r0, (sp, 60)
    mtcr    r0, epsr
    ldw     r15, (sp, 56)
    ldm     r0-r13, (sp)
    addi    sp, 68
    rte

