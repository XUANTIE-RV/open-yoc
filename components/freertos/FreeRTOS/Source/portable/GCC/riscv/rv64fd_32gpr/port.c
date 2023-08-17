/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V RV32 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "riscv_csr.h"

/* Standard includes. */
#include "string.h"

extern void aos_task_exit(int code);
extern void vPortStartTask(void);
/*-----------------------------------------------------------*/

/* Used to keep track of the number of nested calls to taskENTER_CRITICAL().  This
will be set to 0 prior to the first task being started. */
portLONG ulCriticalNesting = 0x9999UL;

/* Used to record one tack want to swtich task after enter critical area, we need know it
 * and implement task switch after exit critical area */
portLONG pendsvflag = 0;

#if( portHAS_STACK_OVERFLOW_CHECKING == 1 )
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, StackType_t *pxEndOfStack, TaskFunction_t pxCode, void *pvParameters )
#else
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
#endif
{
    StackType_t *stk  = NULL;
    register int *gp asm("x3");
    uint64_t temp = (uint64_t)pxTopOfStack;

    temp &= 0xFFFFFFFFFFFFFFF8UL;
    stk = (StackType_t *)temp;

    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F31         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F30         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F29         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F28         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F27         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F26         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F25         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F24         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F23         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F22         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F21          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F20          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F19         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F18         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F17         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F16         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F15         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F14         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F13         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F12         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F11         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F10         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F9          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F8          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F7          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F6          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F5          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F4          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F3          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F2          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F1          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* F0          */

    /* FS=0b01 MPP=0b11 MPIE=0b1 */
    *(--stk)  = (uint64_t)SR_FS_INITIAL | SR_MPP_M | SR_MPIE; /* mstatus */
    *(--stk)  = (uint64_t)pxCode;                     /* Entry Point */

    *(--stk)  = (uint64_t)0x3131313131313131L;       /* X31         */
    *(--stk)  = (uint64_t)0x3030303030303030L;       /* X30         */
    *(--stk)  = (uint64_t)0x2929292929292929L;       /* X29         */
    *(--stk)  = (uint64_t)0x2828282828282828L;       /* X28         */
    *(--stk)  = (uint64_t)0x2727272727272727L;       /* X27         */
    *(--stk)  = (uint64_t)0x2626262626262626L;       /* X26         */
    *(--stk)  = (uint64_t)0x2525252525252525L;       /* X25         */
    *(--stk)  = (uint64_t)0x2424242424242424L;       /* X24         */
    *(--stk)  = (uint64_t)0x2323232323232323L;       /* X23         */
    *(--stk)  = (uint64_t)0x2222222222222222L;       /* X22         */
    *(--stk)  = (uint64_t)0x2121212121212121L;       /* X21         */
    *(--stk)  = (uint64_t)0x2020202020202020L;       /* X20         */
    *(--stk)  = (uint64_t)0x1919191919191919L;       /* X19         */
    *(--stk)  = (uint64_t)0x1818181818181818L;       /* X18         */
    *(--stk)  = (uint64_t)0x1717171717171717L;       /* X17         */
    *(--stk)  = (uint64_t)0x1616161616161616L;       /* X16         */
    *(--stk)  = (uint64_t)0x1515151515151515L;       /* X15         */
    *(--stk)  = (uint64_t)0x1414141414141414L;       /* X14         */
    *(--stk)  = (uint64_t)0x1313131313131313L;       /* X13         */
    *(--stk)  = (uint64_t)0x1212121212121212L;       /* X12         */
    *(--stk)  = (uint64_t)0x1111111111111111L;       /* X11         */
    *(--stk)  = (uint64_t)pvParameters;              /* X10         */
    *(--stk)  = (uint64_t)0x0909090909090909L;       /* X9          */
    *(--stk)  = (uint64_t)0x0808080808080808L;       /* X8          */
    *(--stk)  = (uint64_t)0x0707070707070707L;       /* X7          */
    *(--stk)  = (uint64_t)0x0606060606060606L;       /* X6          */
    *(--stk)  = (uint64_t)0x0505050505050505L;       /* X5          */
    *(--stk)  = (uint64_t)0x0404040404040404L;       /* X4          */
    *(--stk)  = (uint64_t)gp;                        /* X3          */
    *(--stk)  = (uint64_t)aos_task_exit;             /* X1          */
#ifdef __riscv_vector
    *(--stk)  = (uint64_t)0x0L;                      /* VXRM        */
    *(--stk)  = (uint64_t)0x0L;                      /* VXSAT       */
    *(--stk)  = (uint64_t)0x0L;                      /* VSTART      */
    *(--stk)  = (uint64_t)0x0L;                      /* VTYPE       */
    *(--stk)  = (uint64_t)0x0L;                      /* VL          */

    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V31         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V31         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V30         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V30         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V29         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V29         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V28         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V28         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V27         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V27         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V26         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V26         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V25         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V25         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V24         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V24         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V23         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V23         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V22         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V22         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V21          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V21          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V20          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V20          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V19         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V19         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V18         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V18         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V17         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V17         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V16         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V16         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V15         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V15         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V14         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V14         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V13         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V13         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V12         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V12         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V11         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V11         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V10         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V10         */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V9          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V9          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V8          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V8          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V7          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V7          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V6          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V6          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V5          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V5          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V4          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V4          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V3          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V3          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V2          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V2          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V1          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V1          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V0          */
    *(--stk)  = (uint64_t)0x1234567812345678L;       /* V0          */
#endif

#if( portHAS_STACK_OVERFLOW_CHECKING == 1 )
    if (stk <= pxEndOfStack) {
        printk("pxTopOfStack: %p, pxEndOfStack: %p, stk: %p\r\n", pxTopOfStack, pxEndOfStack, stk);
        configASSERT(pdFALSE);
        return NULL;
    }
#endif
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
	/* Not implemented. */
	for( ;; );
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
    void *pxTopOfStack = (void *)(*(unsigned long *)pxTask);
    printk("!!! task [%s] statck overflow. pxTop: %p\r\n", pcTaskName, pxTopOfStack);
    for(;;);
}

__attribute__((weak)) void vApplicationMallocFailedHook( void )
{
    for(;;);
}