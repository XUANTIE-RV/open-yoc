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

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "riscv_csr.h"

/* Standard includes. */
#include "string.h"
#if CONFIG_AOS_OSAL
extern void aos_task_exit(int code);
#endif
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
    extern int __global_pointer$;
    StackType_t *stk  = NULL;
    uint64_t temp = (uint64_t)pxTopOfStack;
#if defined(CONFIG_RISCV_SMODE) && CONFIG_RISCV_SMODE
    unsigned long status = SR_SPP_S | SR_SPIE;
#else
    unsigned long status = SR_MPP_M | SR_MPIE;
#endif
#ifdef __riscv_vector
    int vlenb = csi_vlenb_get_value();
    status |= SR_VS_INITIAL;
#endif
#ifdef __riscv_flen
    status |= SR_FS_INITIAL;
#endif

    temp &= 0xFFFFFFFFFFFFFFF8UL;
    stk = (StackType_t *)temp;

    /* FS=0b01 MPP=0b11 MPIE=0b1 */
    *(--stk)  = (uint64_t)status;                    /* status */
    *(--stk)  = (uint64_t)pxCode;                    /* Entry Point */

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
    *(--stk)  = (uint64_t)&__global_pointer$;        /* X3          */
#if CONFIG_AOS_OSAL
    *(--stk)  = (uint64_t)aos_task_exit;             /* X1          */
#else
    *(--stk)  = (uint64_t)vTaskDelete;       /* X1          */
#endif
#ifdef __riscv_flen
    {
        *(--stk)  = (uint64_t)0x0L;                  /* FCSR        */

        int num = __riscv_flen / 8 * 32 / sizeof(StackType_t);
        for (int i = 0; i < num; i++) {
            *(--stk)  = (StackType_t)0x1234567812345678L;    /* F31 ~ F0    */
        }
    }
#endif

#ifdef __riscv_vector
    {
        *(--stk)  = (uint64_t)0x0L;                  /* VXRM        */
        *(--stk)  = (uint64_t)0x0L;                  /* VXSAT       */
        *(--stk)  = (uint64_t)0x0L;                  /* VSTART      */
        *(--stk)  = (uint64_t)0x0L;                  /* VTYPE       */
        *(--stk)  = (uint64_t)0x0L;                  /* VL          */

        int num = vlenb * 32 / sizeof(StackType_t);
        for (int i = 0; i < num; i++) {
            *(--stk)  = (StackType_t)0x1234567812345678L;    /* V31 ~ V0    */
        }
    }
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
    if (ulCriticalNesting == 0) {
        portENABLE_INTERRUPTS();

        if (pendsvflag) {
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
        if (xTaskIncrementTick() != pdFALSE) {
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

__attribute__((weak)) void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    void *pxTopOfStack = (void *)(*(unsigned long *)pxTask);
    printk("!!! task [%s] stack overflow. pxTop: %p\r\n", pcTaskName, pxTopOfStack);
    for(;;);
}

__attribute__((weak)) void vApplicationMallocFailedHook( void )
{
    for(;;);
}
