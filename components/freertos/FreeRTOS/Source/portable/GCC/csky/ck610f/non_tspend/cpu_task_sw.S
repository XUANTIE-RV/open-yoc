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
    link: http://www.freertos.org/sp0114.html

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

/* Kernel includes. */
#include <csi_config.h>

/********************************************************************
 * Functions: vPortStartTask
 *
 ********************************************************************/
.global vPortStartTask
.type   vPortStartTask, %function
vPortStartTask:
    psrclr   ie
    lrw      r4, pxCurrentTCB
    ld.w     r4, (r4)
    ld.w     sp, (r4)

    mov     r1, sp
    addi    r1, 32
    addi    r1, 28

    ldw     r2, (r1, 0)
    mtlo    r2
    ldw     r2, (r1, 4)
    mthi    r2
    addi    r1, 8

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr0
    fmts    r3, fr1
    fmts    r4, fr2
    fmts    r5, fr3
    fmts    r6, fr4
    fmts    r7, fr5
    fmts    r8, fr6
    fmts    r9, fr7
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr8
    fmts    r3, fr9
    fmts    r4, fr10
    fmts    r5, fr11
    fmts    r6, fr12
    fmts    r7, fr13
    fmts    r8, fr14
    fmts    r9, fr15
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr16
    fmts    r3, fr17
    fmts    r4, fr18
    fmts    r5, fr19
    fmts    r6, fr20
    fmts    r7, fr21
    fmts    r8, fr22
    fmts    r9, fr23
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr24
    fmts    r3, fr25
    fmts    r4, fr26
    fmts    r5, fr27
    fmts    r6, fr28
    fmts    r7, fr29
    fmts    r8, fr30
    fmts    r9, fr31
    addi    r1, 32

    ldw     r2, (r1, 0)
    mtcr    r2, epsr
    ldw     r2, (r1, 4)
    mtcr    r2, epc

    ldm     r1-r15, (sp)

    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 12

    rte

/********************************************************************
 * Functions: trap0_handler
 *
 ********************************************************************/
.global vPortYield
.type   vPortYield, %function
vPortYield:
    psrclr  ee
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 12

    stm     r1-r15, (sp)
    mov     r1, sp
    addi    r1, 32
    addi    r1, 28

    mflo    r2
    stw     r2, (r1, 0)
    mfhi    r2
    stw     r2, (r1, 4)
    addi    r1, 8

    fmfs    r2, fr0
    fmfs    r3, fr1
    fmfs    r4, fr2
    fmfs    r5, fr3
    fmfs    r6, fr4
    fmfs    r7, fr5
    fmfs    r8, fr6
    fmfs    r9, fr7
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r9, (r1, 28)
    addi    r1, 32

    fmfs    r2, fr8
    fmfs    r3, fr9
    fmfs    r4, fr10
    fmfs    r5, fr11
    fmfs    r6, fr12
    fmfs    r7, fr13
    fmfs    r8, fr14
    fmfs    r9, fr15
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r9, (r1, 28)
    addi    r1, 32

    fmfs    r2, fr16
    fmfs    r3, fr17
    fmfs    r4, fr18
    fmfs    r5, fr19
    fmfs    r6, fr20
    fmfs    r7, fr21
    fmfs    r8, fr22
    fmfs    r9, fr23
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r9, (r1, 28)
    addi    r1, 32

    fmfs    r2, fr24
    fmfs    r3, fr25
    fmfs    r4, fr26
    fmfs    r5, fr27
    fmfs    r6, fr28
    fmfs    r7, fr29
    fmfs    r8, fr30
    fmfs    r9, fr31
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r8, (r1, 28)
    addi    r1, 32

    mfcr    r2, psr
    bseti   r2, 8
    stw     r2, (r1, 0)
    stw     r15, (r1, 4)


    lrw     r2, pxCurrentTCB
    ld.w    r3, (r2)
    st.w    sp, (r3)

    jbsr    vTaskSwitchContext
    lrw     r4, pxCurrentTCB
    ld.w    r4, (r4)
    ld.w    sp, (r4)

    mov     r1, sp
    addi    r1, 32
    addi    r1, 28

    ldw     r2, (r1, 0)
    mtlo    r2
    ldw     r2, (r1, 4)
    mthi    r2
    addi    r1, 8

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr0
    fmts    r3, fr1
    fmts    r4, fr2
    fmts    r5, fr3
    fmts    r6, fr4
    fmts    r7, fr5
    fmts    r8, fr6
    fmts    r9, fr7
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr8
    fmts    r3, fr9
    fmts    r4, fr10
    fmts    r5, fr11
    fmts    r6, fr12
    fmts    r7, fr13
    fmts    r8, fr14
    fmts    r9, fr15
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr16
    fmts    r3, fr17
    fmts    r4, fr18
    fmts    r5, fr19
    fmts    r6, fr20
    fmts    r7, fr21
    fmts    r8, fr22
    fmts    r9, fr23
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr24
    fmts    r3, fr25
    fmts    r4, fr26
    fmts    r5, fr27
    fmts    r6, fr28
    fmts    r7, fr29
    fmts    r8, fr30
    fmts    r9, fr31
    addi    r1, 32

    ldw     r2, (r1, 0)
    mtcr    r2, epsr
    ldw     r2, (r1, 4)
    mtcr    r2, epc

    ldm     r1-r15, (sp)

    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 12

    rte

/********************************************************************
 * Functions: Default_IRQHandler
 *
 ********************************************************************/
.global Default_IRQHandler
.type   Default_IRQHandler, %function
Default_IRQHandler:
    psrset   ee
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 32
    subi    sp, 12

    stm     r1-r15, (sp)
    mov     r1, sp
    addi    r1, 32
    addi    r1, 28

    mflo    r2
    stw     r2, (r1, 0)
    mfhi    r2
    stw     r2, (r1, 4)
    addi    r1, 8

    fmfs    r2, fr0
    fmfs    r3, fr1
    fmfs    r4, fr2
    fmfs    r5, fr3
    fmfs    r6, fr4
    fmfs    r7, fr5
    fmfs    r8, fr6
    fmfs    r9, fr7
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r9, (r1, 28)
    addi    r1, 32

    fmfs    r2, fr8
    fmfs    r3, fr9
    fmfs    r4, fr10
    fmfs    r5, fr11
    fmfs    r6, fr12
    fmfs    r7, fr13
    fmfs    r8, fr14
    fmfs    r9, fr15
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r9, (r1, 28)
    addi    r1, 32

    fmfs    r2, fr16
    fmfs    r3, fr17
    fmfs    r4, fr18
    fmfs    r5, fr19
    fmfs    r6, fr20
    fmfs    r7, fr21
    fmfs    r8, fr22
    fmfs    r9, fr23
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r9, (r1, 28)
    addi    r1, 32

    fmfs    r2, fr24
    fmfs    r3, fr25
    fmfs    r4, fr26
    fmfs    r5, fr27
    fmfs    r6, fr28
    fmfs    r7, fr29
    fmfs    r8, fr30
    fmfs    r9, fr31
    stw     r2, (r1, 0)
    stw     r3, (r1, 4)
    stw     r4, (r1, 8)
    stw     r5, (r1, 12)
    stw     r6, (r1, 16)
    stw     r7, (r1, 20)
    stw     r8, (r1, 24)
    stw     r8, (r1, 28)
    addi    r1, 32

    mfcr    r2, epsr
    stw     r2, (r1, 0)
    mfcr    r2, epc
    stw     r2, (r1, 4)

    lrw      r7, pxCurrentTCB
    ldw      r7, (r7)
    stw      sp, (r7)

    lrw      r1, g_top_irqstack
    mov      sp, r1

    lrw      a5, do_irq
    jsr      a5

    lrw      r7, pxCurrentTCB
    ldw      r7, (r7)
    ldw      sp, (r7)

    mov     r1, sp
    addi    r1, 32
    addi    r1, 28

    ldw     r2, (r1, 0)
    mtlo    r2
    ldw     r2, (r1, 4)
    mthi    r2
    addi    r1, 8

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr0
    fmts    r3, fr1
    fmts    r4, fr2
    fmts    r5, fr3
    fmts    r6, fr4
    fmts    r7, fr5
    fmts    r8, fr6
    fmts    r9, fr7
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr8
    fmts    r3, fr9
    fmts    r4, fr10
    fmts    r5, fr11
    fmts    r6, fr12
    fmts    r7, fr13
    fmts    r8, fr14
    fmts    r9, fr15
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr16
    fmts    r3, fr17
    fmts    r4, fr18
    fmts    r5, fr19
    fmts    r6, fr20
    fmts    r7, fr21
    fmts    r8, fr22
    fmts    r9, fr23
    addi    r1, 32

    ldw     r2, (r1, 0)
    ldw     r3, (r1, 4)
    ldw     r4, (r1, 8)
    ldw     r5, (r1, 12)
    ldw     r6, (r1, 16)
    ldw     r7, (r1, 20)
    ldw     r8, (r1, 24)
    ldw     r9, (r1, 28)
    fmts    r2, fr24
    fmts    r3, fr25
    fmts    r4, fr26
    fmts    r5, fr27
    fmts    r6, fr28
    fmts    r7, fr29
    fmts    r8, fr30
    fmts    r9, fr31
    addi    r1, 32

    ldw     r2, (r1, 0)
    bclri   r2, 16
    bclri   r2, 17
    bclri   r2, 18
    bclri   r2, 19
    bclri   r2, 20
    bclri   r2, 21
    bclri   r2, 22
    mtcr    r2, epsr
    ldw     r2, (r1, 4)
    mtcr    r2, epc

    ldm     r1-r15, (sp)

    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 32
    addi    sp, 12

    rte
