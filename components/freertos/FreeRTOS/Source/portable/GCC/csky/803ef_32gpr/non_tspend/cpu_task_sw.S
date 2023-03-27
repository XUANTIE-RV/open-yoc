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

#include <csi_config.h>

/********************************************************************
 * Functions: vPortStartTask
 *
 ********************************************************************/
.global vPortStartTask
.type   vPortStartTask, %function
vPortStartTask:
    psrclr  ie
    lrw     r4, pxCurrentTCB
    ld.w    r4, (r4)
    ld.w    sp, (r4)

    ldw     r0, (sp, 200)
    mtcr    r0, epc
    ldw     r0, (sp, 196)
    mtcr    r0, epsr

    ldw     r1, (sp, 128)
    mthi    r1
    ldw     r1, (sp, 124)
    mtlo    r1

    ldm     r0-r13, (sp)
    ldw     r15, (sp, 56)
    addi    sp, 60
    ldm     r16-r31, (sp)
    addi    sp, 72
    fldms   vr0-vr15, (sp)
    addi    sp, 72

    rte

/********************************************************************
 * Functions: vPortYield
 *
 ********************************************************************/
.global vPortYield
.type   vPortYield, %function
vPortYield:
    psrclr  ee
    subi    sp, 204
    stm     r0-r13, (sp)
    stw     r15, (sp, 56)
    addi    r0, sp, 60
    stm     r16-r31, (r0)
    mflo    r1
    stw     r1, (r0, 64)
    mfhi    r1
    stw     r1, (r0, 68)
    addi    r0, 72
    fstms   vr0-vr15, (r0)
    mfcr    r1, psr
    bseti   r1, 8
    stw     r1, (r0, 64)
    stw     r15, (r0, 68)

    lrw     r2, pxCurrentTCB
    ld.w    r3, (r2)
    st.w    sp, (r3)

    jbsr    vTaskSwitchContext
    lrw     r4, pxCurrentTCB
    ld.w    r4, (r4)
    ld.w    sp, (r4)

    ldw     r0, (sp, 200)
    mtcr    r0, epc
    ldw     r0, (sp, 196)
    mtcr    r0, epsr

    ldw     r1, (sp, 128)
    mthi    r1
    ldw     r1, (sp, 124)
    mtlo    r1

    ldm     r0-r13, (sp)
    ldw     r15, (sp, 56)
    addi    sp, 60
    ldm     r16-r31, (sp)
    addi    sp, 72
    fldms   vr0-vr15, (sp)
    addi    sp, 72

    rte

/********************************************************************
 * Functions: Default_IRQHandler
 *
 ********************************************************************/
.global Default_IRQHandler
.type   Default_IRQHandler, %function
Default_IRQHandler:
    psrset  ee
    subi    sp, 204
    stm     r0-r13, (sp)
    stw     r15, (sp, 56)
    addi    r0, sp, 60
    stm     r16-r31, (r0)
    mflo    r1
    stw     r1, (r0, 64)
    mfhi    r1
    stw     r1, (r0, 68)
    addi    r0, 72
    fstms   vr0-vr15, (r0)
    mfcr    r1, epsr
    stw     r1, (r0, 64)
    mfcr    r1, epc
    stw     r1, (r0, 68)

    lrw     r7, pxCurrentTCB
    ldw     r7, (r7)
    stw     sp, (r7)

    lrw     sp, g_top_irqstack

    lrw     r1, do_irq
    jsr     r1

    lrw     r7, pxCurrentTCB
    ldw     r7, (r7)
    ldw     sp, (r7)

    ldw     r0, (sp, 200)
    mtcr    r0, epc
    ldw     r0, (sp, 196)
    mtcr    r0, epsr

    ldw     r1, (sp, 128)
    mthi    r1
    ldw     r1, (sp, 124)
    mtlo    r1

    ldm     r0-r13, (sp)
    ldw     r15, (sp, 56)
    addi    sp, 60
    ldm     r16-r31, (sp)
    addi    sp, 72
    fldms   vr0-vr15, (sp)
    addi    sp, 72

    rte
