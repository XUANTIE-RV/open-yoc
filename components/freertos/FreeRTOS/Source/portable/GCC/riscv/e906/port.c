/*
 * Copyright (C) 2014-2023 Alibaba Group Holding Limited
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

#if CONFIG_AOS_OSAL
extern void aos_task_exit(int code);
#endif
extern void vPortStartTask(void);

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
    uint32_t temp = (uint32_t)pxTopOfStack;

    temp &= 0xFFFFFFF8UL;
    stk = (StackType_t *)temp;
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
#if CONFIG_AOS_OSAL
    *(--stk)  = (uint32_t)aos_task_exit;     /* X1          */
#else
    *(--stk)  = (uint32_t)vTaskDelete;       /* X1          */
#endif

#ifdef __riscv_flen
    {
        *(--stk)  = (uint32_t)0x0L;                  /* FCSR        */

        int num = __riscv_flen / 8 * 32 / sizeof(StackType_t);
        for (int i = 0; i < num; i++) {
            *(--stk)  = (StackType_t)0x12345678L;    /* F31 ~ F0    */
        }
    }
#endif

#if __riscv_dsp
    *(--stk)  = (uint32_t)0x0L;              /* P-ext vxsat reg    */
#endif /*__riscv_dsp */

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