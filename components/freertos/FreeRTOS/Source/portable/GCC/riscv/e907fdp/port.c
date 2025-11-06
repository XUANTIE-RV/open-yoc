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

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <sys_clk.h>

// #define ENABLE_KERNEL_DEBUG

#ifdef ENABLE_KERNEL_DEBUG
#define FREERTOS_PORT_DEBUG(...)                printk(__VA_ARGS__)
#else
#define FREERTOS_PORT_DEBUG(...)
#endif

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

#if( configUSE_TICKLESS_IDLE == 1 )

/* The systick is a 64-bit counter. */
#define portMAX_BIT_NUMBER          ( 0xFFFFFFFFFFFFFFFFULL )

/* A fiddle factor to estimate the number of SysTick counts that would have
occurred while the SysTick counter is stopped during tickless idle
calculations. */
#define portMISSED_COUNTS_FACTOR    ( 80UL )

/*
 * The number of SysTick increments that make up one tick period.
 */
static TickType_t ulTimerCountsForOneTick = 0;

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 24 bit resolution of the SysTick timer.
 */
static TickType_t xMaximumPossibleSuppressedTicks = 0;

/*
 * Compensate for the CPU cycles that pass while the SysTick is stopped (low
 * power functionality only.
 */
static TickType_t ulStoppedTimerCompensation = 0;
#endif

#if( portHAS_STACK_OVERFLOW_CHECKING == 1 )
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, StackType_t *pxEndOfStack, TaskFunction_t pxCode, void *pvParameters )
#else
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
#endif
{
    extern int __global_pointer$;
    StackType_t *stk  = NULL;
    uint32_t temp = (uint32_t)pxTopOfStack;
    uint32_t status = __get_MSTATUS();

    temp &= 0xFFFFFFF8UL;
    stk = (StackType_t *)temp;
    *(--stk)  = (uint32_t)status;
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
    *(--stk)  = (uint32_t)pxTopOfStack;      /* X8, aka. fp */
    *(--stk)  = (uint32_t)0x07070707L;       /* X7          */
    *(--stk)  = (uint32_t)0x06060606L;       /* X6          */
    *(--stk)  = (uint32_t)0x05050505L;       /* X5          */
    *(--stk)  = (uint32_t)0x04040404L;       /* X4          */
    *(--stk)  = (uint32_t)&__global_pointer$;/* X3          */
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

#if( configUSE_TICKLESS_IDLE == 1 )
    {
        ulTimerCountsForOneTick = (soc_get_coretim_freq() / CONFIG_SYSTICK_HZ);
        xMaximumPossibleSuppressedTicks = portMAX_BIT_NUMBER / ulTimerCountsForOneTick;
        ulStoppedTimerCompensation = portMISSED_COUNTS_FACTOR / (soc_get_cpu_freq(0) / soc_get_coretim_freq());
    }
#endif /* configUSE_TICKLESS_IDLE */

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

#if( configUSE_TICKLESS_IDLE == 1 )

__attribute__((weak)) void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    uint32_t ulReloadValue, ulCompleteTickPeriods, ulCompletedSysTickDecrements, ulCompleteTickPeriodsMod;
    uint64_t XLastLoadValue, xModifiableIdleTime, ulSysTickIncrementsLeft, xCompareValue;

    FREERTOS_PORT_DEBUG("Enter TickLess %d\n", (uint32_t)xExpectedIdleTime);

    /* Make sure the SysTick reload value does not overflow the counter. */
    if (xExpectedIdleTime > xMaximumPossibleSuppressedTicks) {
        xExpectedIdleTime = xMaximumPossibleSuppressedTicks;
    }

    /* Enter a critical section but don't use the taskENTER_CRITICAL()
    * method as that will mask interrupts that should exit sleep mode. */
    __disable_irq();

    /* If a context switch is pending or a task is waiting for the scheduler
    * to be unsuspended then abandon the low power entry. */
    if (eTaskConfirmSleepModeStatus() == eAbortSleep) {
        /* Re-enable interrupts - see comments above the cpsid instruction
        * above. */
        __enable_irq();
    } else {
        /* Get System timer load value before sleep */
        XLastLoadValue = CORET->MTIME;

        /* Use the SysTick current-value register to determine the number of
        * SysTick increments remaining until the next tick interrupt.  If the
        * current-value register is more than compare-value register, then there are actually
        * ulTimerCountsForOneTick increments remaining */
        xCompareValue = CORET->MTIMECMP;
        if (xCompareValue > XLastLoadValue) {
            ulSysTickIncrementsLeft = xCompareValue - XLastLoadValue;
        } else {
            ulSysTickIncrementsLeft = ulTimerCountsForOneTick;
        }
        if (ulSysTickIncrementsLeft > ulTimerCountsForOneTick) {
            __enable_irq();
            return;
        }

        /* Stop the SysTick momentarily.  The time the SysTick is stopped for
        * is accounted for as best it can be, but using the tickless mode will
        * inevitably result in some tiny drift of the time maintained by the
        * kernel with respect to calendar time. */
        CORET->MTIMECMP = 0xFFFFFFFFFFFFFFFFULL;
        __DSB();

        /* Calculate the reload value required to wait xExpectedIdleTime
        tick periods.  -1 is used because this code will execute part way
        through one of the tick periods. */
        ulReloadValue = ulSysTickIncrementsLeft + ( ulTimerCountsForOneTick * ( xExpectedIdleTime - 1UL ) );
        if( ulReloadValue > ulStoppedTimerCompensation )
        {
            ulReloadValue -= ulStoppedTimerCompensation;
        }

        /* Set the new reload value. */
        CORET->MTIMECMP = XLastLoadValue + ulReloadValue;
        __DSB();

        /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
        * set its parameter to 0 to indicate that its implementation contains
        * its own wait for interrupt or wait for event instruction, and so wfi
        * should not be executed again.  However, the original expected idle
        * time variable must remain unmodified, so a copy is taken. */
        xModifiableIdleTime = xExpectedIdleTime;
        configPRE_SLEEP_PROCESSING( xModifiableIdleTime );

        if( xModifiableIdleTime > 0 )
        {
            __WFI();
        }

        configPOST_SLEEP_PROCESSING( xExpectedIdleTime );

        /* Determine if SysTimer Interrupt is not yet happened,
        (in which case an interrupt other than the SysTick
        must have brought the system out of sleep mode). */
        if (CORET->MTIME >= XLastLoadValue + ulReloadValue) {
            /* As the pending tick will be processed as soon as this
            function exits, the tick value maintained by the tick is stepped
            forward by one less than the time spent waiting. */
            ulCompleteTickPeriods = xExpectedIdleTime - 1UL;
            FREERTOS_PORT_DEBUG("TickLess - SysTimer Interrupt Entered!\n");
        } else {
            /* Something other than the tick interrupt ended the sleep. */

            /* Use the SysTick current-value register to determine the
            * number of SysTick increments remaining until the expected idle
            * time would have ended. */
            ulSysTickIncrementsLeft = CORET->MTIMECMP - CORET->MTIME;

            /* Work out how long the sleep lasted rounded to complete tick
            * periods (not the ulReload value which accounted for part
            * ticks). */
            ulCompletedSysTickDecrements = ( xExpectedIdleTime * ulTimerCountsForOneTick ) - ulSysTickIncrementsLeft;

            /* How many complete tick periods passed while the processor
            was waiting? */
            ulCompleteTickPeriods = ulCompletedSysTickDecrements / ulTimerCountsForOneTick + 1UL;

            ulCompleteTickPeriodsMod = ulCompletedSysTickDecrements % ulTimerCountsForOneTick;

            /* The reload value is set to whatever fraction of a single tick
            period remains.
            Need include the (ulTimerCountsForOneTick - ulCompleteTickPeriodsMod),
            because we add 1 tick for ulCompleteTickPeriods above.*/
            CORET->MTIMECMP = CORET->MTIME + ulTimerCountsForOneTick + (ulTimerCountsForOneTick - ulCompleteTickPeriodsMod);
            __DSB();
            FREERTOS_PORT_DEBUG("TickLess - External Interrupt Happened!\n");
        }

        FREERTOS_PORT_DEBUG("End TickLess %d\n", (uint32_t)ulCompleteTickPeriods);

        /* Step the tick to account for any tick periods that elapsed. */
        vTaskStepTick(ulCompleteTickPeriods);

        /* Exit with interrupts enabled. */
        __enable_irq();
    }
}

#endif /* #if configUSE_TICKLESS_IDLE */
/*-----------------------------------------------------------*/
