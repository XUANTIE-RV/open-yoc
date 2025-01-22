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
    StackType_t temp = (StackType_t)pxTopOfStack;
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
#if __riscv_matrix || __riscv_xtheadmatrix
    status |= SR_MS_INITIAL;
#endif

#if __riscv_xlen == 32
    StackType_t ivalue = (StackType_t)0x12345678L;
    temp &= 0xFFFFFFF8UL;
#else
    StackType_t ivalue = (StackType_t)0x1234567812345678L;
    temp &= 0xFFFFFFFFFFFFFFF8UL;
#endif
    stk = (StackType_t *)temp;

    /* FS=0b01 MPP=0b11 MPIE=0b1 */
    *(--stk)  = (StackType_t)status;                    /* status */
    *(--stk)  = (StackType_t)pxCode;                    /* Entry Point */

    *(--stk)  = (StackType_t)ivalue;                    /* X31         */
    *(--stk)  = (StackType_t)ivalue;                    /* X30         */
    *(--stk)  = (StackType_t)ivalue;                    /* X29         */
    *(--stk)  = (StackType_t)ivalue;                    /* X28         */
    *(--stk)  = (StackType_t)ivalue;                    /* X27         */
    *(--stk)  = (StackType_t)ivalue;                    /* X26         */
    *(--stk)  = (StackType_t)ivalue;                    /* X25         */
    *(--stk)  = (StackType_t)ivalue;                    /* X24         */
    *(--stk)  = (StackType_t)ivalue;                    /* X23         */
    *(--stk)  = (StackType_t)ivalue;                    /* X22         */
    *(--stk)  = (StackType_t)ivalue;                    /* X21         */
    *(--stk)  = (StackType_t)ivalue;                    /* X20         */
    *(--stk)  = (StackType_t)ivalue;                    /* X19         */
    *(--stk)  = (StackType_t)ivalue;                    /* X18         */
    *(--stk)  = (StackType_t)ivalue;                    /* X17         */
    *(--stk)  = (StackType_t)ivalue;                    /* X16         */
    *(--stk)  = (StackType_t)ivalue;                    /* X15         */
    *(--stk)  = (StackType_t)ivalue;                    /* X14         */
    *(--stk)  = (StackType_t)ivalue;                    /* X13         */
    *(--stk)  = (StackType_t)ivalue;                    /* X12         */
    *(--stk)  = (StackType_t)ivalue;                    /* X11         */
    *(--stk)  = (StackType_t)pvParameters;              /* X10         */
    *(--stk)  = (StackType_t)ivalue;                    /* X9          */
    *(--stk)  = (StackType_t)ivalue;                    /* X8          */
    *(--stk)  = (StackType_t)ivalue;                    /* X7          */
    *(--stk)  = (StackType_t)ivalue;                    /* X6          */
    *(--stk)  = (StackType_t)ivalue;                    /* X5          */
    *(--stk)  = (StackType_t)ivalue;                    /* X4          */
    *(--stk)  = (StackType_t)&__global_pointer$;        /* X3          */
#if CONFIG_AOS_OSAL	
    *(--stk)  = (StackType_t)aos_task_exit;             /* X1          */
#else
    *(--stk)  = (StackType_t)vTaskDelete;               /* X1          */
#endif
#ifdef __riscv_flen
    {
        *(--stk)  = (StackType_t)0x0L;                  /* FCSR        */

        int num = __riscv_flen / 8 * 32 / sizeof(StackType_t);
        for (int i = 0; i < num; i++) {
            *(--stk)  = (StackType_t)ivalue;            /* F31 ~ F0    */
        }
    }
#endif

#ifdef __riscv_vector
    {
        *(--stk)  = (StackType_t)0x0L;                  /* VXRM        */
        *(--stk)  = (StackType_t)0x0L;                  /* VXSAT       */
        *(--stk)  = (StackType_t)0x0L;                  /* VSTART      */
        *(--stk)  = (StackType_t)0x0L;                  /* VTYPE       */
        *(--stk)  = (StackType_t)0x0L;                  /* VL          */

        int num = vlenb * 32 / sizeof(StackType_t);
        for (int i = 0; i < num; i++) {
            *(--stk)  = (StackType_t)ivalue;            /* V31 ~ V0    */
        }
    }
#endif

#if __riscv_matrix || __riscv_xtheadmatrix
    {
        *(--stk)  = (StackType_t)0x0L;                  /* XMRSTART    */
        *(--stk)  = (StackType_t)0x0L;                  /* XMCSR       */
        *(--stk)  = (StackType_t)0x0L;                  /* XMSIZE      */

        int xmlenb = csi_xmlenb_get_value();
        int num = xmlenb * 8 / sizeof(StackType_t);
        for (int i = 0; i < num; i++) {
            *(--stk)  = (StackType_t)ivalue;            /* M7 ~ M0    */
        }
    }
#endif /* __riscv_matrix || __riscv_xtheadmatrix */

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
