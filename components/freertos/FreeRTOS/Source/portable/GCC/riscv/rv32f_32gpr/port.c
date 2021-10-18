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

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

extern void vPortStartTask(void);

/* Used to keep track of the number of nested calls to taskENTER_CRITICAL().  This
will be set to 0 prior to the first task being started. */
portLONG ulCriticalNesting = 0x9999UL;

/* Used to record one tack want to swtich task after enter critical area, we need know it
 * and implement task switch after exit critical area */
portLONG pendsvflag = 0;

StackType_t *pxPortInitialiseStack( StackType_t * pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    StackType_t *stk  = NULL;
    register int *gp asm("x3");
    uint32_t temp = (uint32_t)pxTopOfStack;

    temp &= 0xFFFFFFF8UL;
    stk = (StackType_t *)temp;
    *(--stk)  = (uint32_t)0x12345678L;       /* F31         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F30         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F29         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F28         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F27         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F26         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F25         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F24         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F23         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F22         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F21          */
    *(--stk)  = (uint32_t)0x12345678L;       /* F20          */
    *(--stk)  = (uint32_t)0x12345678L;       /* F19         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F18         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F17         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F16         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F15         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F14         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F13         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F12         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F11         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F10         */
    *(--stk)  = (uint32_t)0x12345678L;       /* F9          */
    *(--stk)  = (uint32_t)0x12345678L;       /* F8          */
    *(--stk)  = (uint32_t)0x12345678L;       /* F7          */
    *(--stk)  = (uint32_t)0x12345678L;       /* F6          */
    *(--stk)  = (uint32_t)0x12345678L;       /* F5          */
    *(--stk)  = (uint32_t)0x12345678L;       /* F4          */
    *(--stk)  = (uint32_t)0x12345678L;       /* F3          */
    *(--stk)  = (uint32_t)0x12345678L;       /* F2          */
    *(--stk)  = (uint32_t)0x12345678L;       /* F1          */
    *(--stk)  = (uint32_t)0x12345678L;       /* F0          */

    *(--stk)  = (uint32_t)pxCode;            /* Entry Point */
    *(--stk)  = (uint32_t)0x31313131L;       /* X31         */
    *(--stk)  = (uint32_t)0x30303030L;       /* X30         */
    *(--stk)  = (uint32_t)0x29292929L;       /* X29         */
    *(--stk)  = (uint32_t)0x28282828L;       /* X28         */
    *(--stk)  = (uint32_t)0x27272727L;       /* X27         */
    *(--stk)  = (uint32_t)0x26262626L;       /* X26         */
    *(--stk)  = (uint32_t)0x25252525L;       /* X25         */
    *(--stk)  = (uint32_t)0x24242424L;       /* X24         */
    *(--stk)  = (uint32_t)0x23232323L;       /* X23         */
    *(--stk)  = (uint32_t)0x22222222L;       /* X22         */
    *(--stk)  = (uint32_t)0x21212121L;       /* X21         */
    *(--stk)  = (uint32_t)0x20202020L;       /* X20         */
    *(--stk)  = (uint32_t)0x19191919L;       /* X19         */
    *(--stk)  = (uint32_t)0x18181818L;       /* X18         */
    *(--stk)  = (uint32_t)0x17171717L;       /* X17         */
    *(--stk)  = (uint32_t)0x16161616L;       /* X16         */
    *(--stk)  = (uint32_t)0x15151515L;       /* X15         */
    *(--stk)  = (uint32_t)0x14141414L;       /* X14         */
    *(--stk)  = (uint32_t)0x13131313L;       /* X13         */
    *(--stk)  = (uint32_t)0x12121212L;       /* X12         */
    *(--stk)  = (uint32_t)0x11111111L;       /* X11         */
    *(--stk)  = (uint32_t)pvParameters;      /* X10         */
    *(--stk)  = (uint32_t)0x09090909L;       /* X9          */
    *(--stk)  = (uint32_t)0x08080808L;       /* X8          */
    *(--stk)  = (uint32_t)0x07070707L;       /* X7          */
    *(--stk)  = (uint32_t)0x06060606L;       /* X6          */
    *(--stk)  = (uint32_t)0x05050505L;       /* X5          */
    *(--stk)  = (uint32_t)0x04040404L;       /* X4          */
    *(--stk)  = (uint32_t)gp;                /* X3          */
    *(--stk)  = (uint32_t)vTaskExit;         /* X1          */
    
    return stk;
}

BaseType_t xPortStartScheduler( void )
{
    ulCriticalNesting = 0UL;

    vPortStartTask();

    return pdFALSE;
}


void vPortEndScheduler( void )
{
    /* Not implemented as there is nothing to return to. */
}

void vPortEnterCritical( void )
{
    portDISABLE_INTERRUPTS();
    ulCriticalNesting ++;
}

void vPortExitCritical( void )
{
    if (ulCriticalNesting == 0) {
        while(1);
    }

    ulCriticalNesting --;
    if (ulCriticalNesting == 0)
    {
        portENABLE_INTERRUPTS();

        if (pendsvflag)
        {
            pendsvflag = 0;
            portYIELD();
        }
    }
}

#if configUSE_PREEMPTION == 0
void xPortSysTickHandler( void )
{
    portLONG ulDummy;

    ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
    xTaskIncrementTick();
    portCLEAR_INTERRUPT_MASK_FROM_ISR( ulDummy );
}

#else
void xPortSysTickHandler( void )
{
    portLONG ulDummy;

    ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
    {
        if (xTaskIncrementTick() != pdFALSE)
        {
            portYIELD_FROM_ISR(pdTRUE);
        }
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR( ulDummy );
}
#endif

void vPortYieldHandler( void )
{
    uint32_t ulSavedInterruptMask;

    ulSavedInterruptMask = portSET_INTERRUPT_MASK_FROM_ISR();

    vTaskSwitchContext();

    portCLEAR_INTERRUPT_MASK_FROM_ISR( ulSavedInterruptMask );
}

__attribute__((weak)) void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName )
{
    for(;;);
}

__attribute__((weak)) void vApplicationMallocFailedHook( void )
{
    for(;;);
}
