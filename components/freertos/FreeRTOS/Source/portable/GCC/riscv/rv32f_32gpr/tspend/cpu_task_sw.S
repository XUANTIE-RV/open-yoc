/*
    FreeRTOS V8.2.3 - Copyright (C) 2015 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/* Enable interrupts when returning from the handler */
#define MSTATUS_PRV1 0x1880
.equ RISCV_MSTATUS_MIE,        (1<<3)       /*machine-level interrupt bit*/
/******************************************************************************
 * Functions:
 *     size_t cpu_intrpt_save(void);
 *     void   cpu_intrpt_restore(size_t psr);
 ******************************************************************************/

.global cpu_intrpt_save
.type cpu_intrpt_save, %function
cpu_intrpt_save:
    csrr    a0, mstatus
    csrc    mstatus, 8
    ret

.global cpu_intrpt_restore
.type cpu_intrpt_restore, %function
cpu_intrpt_restore:
    csrw    mstatus, a0
    ret

.global cpu_is_irq_enable
.type cpu_is_irq_enable, %function
cpu_is_irq_enable:
    csrr a0, mstatus
    andi a0, a0, RISCV_MSTATUS_MIE
    ret

/********************************************************************
 * Functions: vPortYield
 *
 ********************************************************************/
.global vPortYield
.type   vPortYield, %function
vPortYield:
    li      t0, 0xE080100C
    lb      t1, (t0)
    li      t2, 0x01
    or      t1, t1, t2
    sb      t1, (t0)

    ret

/********************************************************************
 * Functions: vPortStartTask
 *
 ********************************************************************/
.global vPortStartTask
.type   vPortStartTask, %function
vPortStartTask:
    la      a0, pxCurrentTCB
    lw      a0, (a0)
    lw      sp, (a0)

    /* Run in machine mode */
    li      t0, MSTATUS_PRV1
    csrs    mstatus, t0

    lw      t0, 120(sp)
    csrw    mepc, t0

    lw      x1, 0(sp)
    lw      x3, 4(sp)
    lw      x4, 8(sp)
    lw      x5, 12(sp)
    lw      x6, 16(sp)
    lw      x7, 20(sp)
    lw      x8, 24(sp)
    lw      x9, 28(sp)
    lw      x10, 32(sp)
    lw      x11, 36(sp)
    lw      x12, 40(sp)
    lw      x13, 44(sp)
    lw      x14, 48(sp)
    lw      x15, 52(sp)
    lw      x16, 56(sp)
    lw      x17, 60(sp)
    lw      x18, 64(sp)
    lw      x19, 68(sp)
    lw      x20, 72(sp)
    lw      x21, 76(sp)
    lw      x22, 80(sp)
    lw      x23, 84(sp)
    lw      x24, 88(sp)
    lw      x25, 92(sp)
    lw      x26, 96(sp)
    lw      x27, 100(sp)
    lw      x28, 104(sp)
    lw      x29, 108(sp)
    lw      x30, 112(sp)
    lw      x31, 116(sp)

    addi    sp, sp, 124

    flw      f31, 0(sp)
    flw      f30, 4(sp)
    flw      f29, 8(sp)
    flw      f28, 12(sp)
    flw      f27, 16(sp)
    flw      f26, 20(sp)
    flw      f25, 24(sp)
    flw      f24, 28(sp)
    flw      f23, 32(sp)
    flw      f22, 36(sp)
    flw      f21, 40(sp)
    flw      f20, 44(sp)
    flw      f19, 48(sp)
    flw      f18, 52(sp)
    flw      f17, 56(sp)
    flw      f16, 60(sp)
    flw      f15, 64(sp)
    flw      f14, 68(sp)
    flw      f13, 72(sp)
    flw      f12, 76(sp)
    flw      f11, 80(sp)
    flw      f10, 84(sp)
    flw      f9, 88(sp)
    flw      f8, 92(sp)
    flw      f7, 96(sp)
    flw      f6, 100(sp)
    flw      f5, 104(sp)
    flw      f4, 108(sp)
    flw      f3, 112(sp)
    flw      f2, 116(sp)
    flw      f1, 120(sp)
    flw      f0, 124(sp)

    addi    sp, sp, 128

    mret

/********************************************************************
 * Functions: tspend_handler
 *
 ********************************************************************/
.global tspend_handler
.type   tspend_handler, %function
tspend_handler:
    addi    sp, sp, -128
    fsw      f31, 0(sp)
    fsw      f30, 4(sp)
    fsw      f29, 8(sp)
    fsw      f28, 12(sp)
    fsw      f27, 16(sp)
    fsw      f26, 20(sp)
    fsw      f25, 24(sp)
    fsw      f24, 28(sp)
    fsw      f23, 32(sp)
    fsw      f22, 36(sp)
    fsw      f21, 40(sp)
    fsw      f20, 44(sp)
    fsw      f19, 48(sp)
    fsw      f18, 52(sp)
    fsw      f17, 56(sp)
    fsw      f16, 60(sp)
    fsw      f15, 64(sp)
    fsw      f14, 68(sp)
    fsw      f13, 72(sp)
    fsw      f12, 76(sp)
    fsw      f11, 80(sp)
    fsw      f10, 84(sp)
    fsw      f9, 88(sp)
    fsw      f8, 92(sp)
    fsw      f7, 96(sp)
    fsw      f6, 100(sp)
    fsw      f5, 104(sp)
    fsw      f4, 108(sp)
    fsw      f3, 112(sp)
    fsw      f2, 116(sp)
    fsw      f1, 120(sp)
    fsw      f0, 124(sp)

    addi    sp, sp, -124

    sw      x1, 0(sp)
    sw      x3, 4(sp)
    sw      x4, 8(sp)
    sw      x5, 12(sp)
    sw      x6, 16(sp)
    sw      x7, 20(sp)
    sw      x8, 24(sp)
    sw      x9, 28(sp)
    sw      x10, 32(sp)
    sw      x11, 36(sp)
    sw      x12, 40(sp)
    sw      x13, 44(sp)
    sw      x14, 48(sp)
    sw      x15, 52(sp)
    sw      x16, 56(sp)
    sw      x17, 60(sp)
    sw      x18, 64(sp)
    sw      x19, 68(sp)
    sw      x20, 72(sp)
    sw      x21, 76(sp)
    sw      x22, 80(sp)
    sw      x23, 84(sp)
    sw      x24, 88(sp)
    sw      x25, 92(sp)
    sw      x26, 96(sp)
    sw      x27, 100(sp)
    sw      x28, 104(sp)
    sw      x29, 108(sp)
    sw      x30, 112(sp)
    sw      x31, 116(sp)
    csrr    t0, mepc
    sw      t0, 120(sp)
    la      a1, pxCurrentTCB
    lw      a1, (a1)
    sw      sp, (a1)

    li      t0, 0xE000E100
    lw      t1, (t0)
    li      t2, 0xFEFFFFFF
    and     t1, t1, t2
    sw      t1, (t0)

    jal     vTaskSwitchContext

    la      a1, pxCurrentTCB
    lw      a1, (a1)
    lw      sp, (a1)

    /* Run in machine mode */
    li      t0, MSTATUS_PRV1
    csrs    mstatus, t0

    lw      t0, 120(sp)
    csrw    mepc, t0

    lw      x1, 0(sp)
    lw      x3, 4(sp)
    lw      x4, 8(sp)
    lw      x5, 12(sp)
    lw      x6, 16(sp)
    lw      x7, 20(sp)
    lw      x8, 24(sp)
    lw      x9, 28(sp)
    lw      x10, 32(sp)
    lw      x11, 36(sp)
    lw      x12, 40(sp)
    lw      x13, 44(sp)
    lw      x14, 48(sp)
    lw      x15, 52(sp)
    lw      x16, 56(sp)
    lw      x17, 60(sp)
    lw      x18, 64(sp)
    lw      x19, 68(sp)
    lw      x20, 72(sp)
    lw      x21, 76(sp)
    lw      x22, 80(sp)
    lw      x23, 84(sp)
    lw      x24, 88(sp)
    lw      x25, 92(sp)
    lw      x26, 96(sp)
    lw      x27, 100(sp)
    lw      x28, 104(sp)
    lw      x29, 108(sp)
    lw      x30, 112(sp)
    lw      x31, 116(sp)

    addi    sp, sp, 124
    
    flw      f31, 0(sp)
    flw      f30, 4(sp)
    flw      f29, 8(sp)
    flw      f28, 12(sp)
    flw      f27, 16(sp)
    flw      f26, 20(sp)
    flw      f25, 24(sp)
    flw      f24, 28(sp)
    flw      f23, 32(sp)
    flw      f22, 36(sp)
    flw      f21, 40(sp)
    flw      f20, 44(sp)
    flw      f19, 48(sp)
    flw      f18, 52(sp)
    flw      f17, 56(sp)
    flw      f16, 60(sp)
    flw      f15, 64(sp)
    flw      f14, 68(sp)
    flw      f13, 72(sp)
    flw      f12, 76(sp)
    flw      f11, 80(sp)
    flw      f10, 84(sp)
    flw      f9, 88(sp)
    flw      f8, 92(sp)
    flw      f7, 96(sp)
    flw      f6, 100(sp)
    flw      f5, 104(sp)
    flw      f4, 108(sp)
    flw      f3, 112(sp)
    flw      f2, 116(sp)
    flw      f1, 120(sp)
    flw      f0, 124(sp)

    addi    sp, sp, 128

    mret
