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

#ifndef PORTMACRO_H
#define PORTMACRO_H

#include <stdlib.h>
#include <stdint.h>
#include <csi_core.h>
#include <drv/tick.h>
#if defined(AOS_COMP_DEBUG) && (AOS_COMP_DEBUG > 0)
#include <debug/dbg.h>
#else
#define printk printf
#endif

#ifdef __cplusplus
extern "C" {
#endif
/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#if __riscv_xlen == 64
	#define portSTACK_TYPE			uint64_t
	#define portBASE_TYPE			int64_t
	#define portUBASE_TYPE			uint64_t
	#define portMAX_DELAY 			( TickType_t ) 0xffffffffffffffffUL
	#define portPOINTER_SIZE_TYPE 	uint64_t
#elif __riscv_xlen == 32
	#define portSTACK_TYPE	uint32_t
    #define portBASE_TYPE	int32_t
	#define portUBASE_TYPE	uint32_t
	#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#else
	#error Assembler did not define __riscv_xlen
#endif


typedef portSTACK_TYPE StackType_t;
typedef portBASE_TYPE BaseType_t;
typedef portUBASE_TYPE UBaseType_t;
typedef portUBASE_TYPE TickType_t;

/* Legacy type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
not need to be guarded with a critical section. */
#define portTICK_TYPE_IS_ATOMIC 1
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#ifdef __riscv64
	#error This is the RV32 port that has not yet been adapted for 64.
	#define portBYTE_ALIGNMENT			16
#else
	#define portBYTE_ALIGNMENT			16
#endif
/*-----------------------------------------------------------*/

static inline portLONG SaveLocalPSR (void)
{
    portLONG flags = csi_irq_save();
    return flags;
}

static inline void RestoreLocalPSR (portLONG newMask)
{
    csi_irq_restore(newMask);
}

#if configNUMBER_OF_CORES > 1
#define portGET_CORE_ID()                       csi_get_cpu_id()
#define __FENCE(p, s) __ASM volatile ("fence " #p "," #s : : : "memory")
#define mb()        __FENCE(iorw,iorw)

#define portRTOS_SPINLOCK_COUNT                     2
typedef volatile uint32_t spin_lock_t;
extern spin_lock_t hw_sync_locks[portRTOS_SPINLOCK_COUNT];
extern void SecondaryCoresUp(void);
extern void vPortRecursiveLock(unsigned long ulCoreID, unsigned long ulLockNum, spin_lock_t *pxSpinLock, BaseType_t uxAcquire);

extern volatile UBaseType_t uxYieldCoreAgain[configNUMBER_OF_CORES];

extern UBaseType_t uxCriticalNestings[ configNUMBER_OF_CORES ];
#define portGET_CRITICAL_NESTING_COUNT(xCoreID)            ( uxCriticalNestings[xCoreID] )
#define portSET_CRITICAL_NESTING_COUNT(xCoreID, xCount)    ( uxCriticalNestings[xCoreID] = (xCount) )
#define portINCREMENT_CRITICAL_NESTING_COUNT(xCoreID)      ( uxCriticalNestings[xCoreID]++ )
#define portDECREMENT_CRITICAL_NESTING_COUNT(xCoreID)      ( uxCriticalNestings[xCoreID]-- )

extern unsigned long cpu_intrpt_save();
extern void cpu_intrpt_restore(unsigned long ulstate);

#define portDISABLE_INTERRUPTS()                __ASM volatile("csrc mstatus, 8")
#define portENABLE_INTERRUPTS()                 __ASM volatile("csrs mstatus, 8")
#define portSET_INTERRUPT_MASK_FROM_ISR()       cpu_intrpt_save();
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)    cpu_intrpt_restore(x)
#define portSET_INTERRUPT_MASK()                cpu_intrpt_save()
#define portCLEAR_INTERRUPT_MASK(a)             cpu_intrpt_restore(a)
#define portENTER_CRITICAL_FROM_ISR()           vTaskEnterCriticalFromISR()
#define portEXIT_CRITICAL_FROM_ISR(a)           vTaskExitCriticalFromISR(a)

#define portGET_ISR_LOCK(xCoreID)               vPortRecursiveLock(xCoreID, 0, &hw_sync_locks[0], pdTRUE)
#define portRELEASE_ISR_LOCK(xCoreID)           vPortRecursiveLock(xCoreID, 0, &hw_sync_locks[0], pdFALSE)
#define portGET_TASK_LOCK(xCoreID)              vPortRecursiveLock(xCoreID, 1, &hw_sync_locks[1], pdTRUE)
#define portRELEASE_TASK_LOCK(xCoreID)          vPortRecursiveLock(xCoreID, 1, &hw_sync_locks[1], pdFALSE)
#define portYIELD_CORE(a)                       vPortYield_Core(a)
#define portENTER_CRITICAL()                    vTaskEnterCritical()
#define portEXIT_CRITICAL()                     vTaskExitCritical()
#else  /* configNUMBER_OF_CORES == 1 */
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );

static inline void vPortEnableInterrupt( void )
{
    __enable_irq();
}

static inline void vPortDisableInterrupt( void )
{
    __disable_irq();
}

#define portDISABLE_INTERRUPTS()                vPortDisableInterrupt()
#define portENABLE_INTERRUPTS()                 vPortEnableInterrupt()
#define portSET_INTERRUPT_MASK_FROM_ISR()       SaveLocalPSR()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(a)    RestoreLocalPSR(a)
#define portENTER_CRITICAL()                    vPortEnterCritical()
#define portEXIT_CRITICAL()                     vPortExitCritical()
#endif  /* configNUMBER_OF_CORES > 1 */

#if configGENERATE_RUN_TIME_STATS
#define portGET_RUN_TIME_COUNTER_VALUE()        csi_tick_get_ms()
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#endif

#define portNOP() __asm volatile 	( " nop " )

extern portLONG ulCriticalNesting;
extern portLONG pendsvflag;

/* Scheduler utilities. */
extern void vPortYield( void );
#define portYIELD()                 vPortYield();

/* Added as there is no such function in FreeRTOS. */
extern void *pvPortRealloc( uint8_t *srcaddr,size_t xWantedSize );
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters ) __attribute__((noreturn))
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )
/*-----------------------------------------------------------*/

#define portEND_SWITCHING_ISR( xSwitchRequired )    do {    \
                                                            if( xSwitchRequired != pdFALSE )    \
                                                            {   \
                                                                portYIELD();    \
                                                            }   \
                                                    }while(0)

#define portYIELD_FROM_ISR( a )     portEND_SWITCHING_ISR( a )

extern int g_fr_next_sleep_ticks;
#define configPRE_SUPPRESS_TICKS_AND_SLEEP_PROCESSING(x) \
do                                                       \
{                                                        \
    g_fr_next_sleep_ticks = x;                           \
} while (0)

/*-----------------------------------------------------------*/

#define portINLINE	__inline

#ifndef portFORCE_INLINE
	#define portFORCE_INLINE inline __attribute__(( always_inline))
#endif

/*-----------------------------------------------------------*/

#if configNUMBER_OF_CORES > 1

enum eCoreID
{
    CORE_0 = 0,
    CORE_1,
    CORE_2,
    CORE_3
};

portFORCE_INLINE void clear_software_irq(int id)
{
    CLINT_Type *clint = (CLINT_Type *)CORET_BASE;

    switch (id)
    {
    case CORE_0:
        clint->MSIP0 &= ~(uint32_t)0x1;
        break;
    case CORE_1:
        clint->MSIP1 &= ~(uint32_t)0x1;
        break;
    case CORE_2:
        clint->MSIP2 &= ~(uint32_t)0x1;
        break;
    case CORE_3:
        clint->MSIP3 &= ~(uint32_t)0x1;
        break;
    default:
        break;
    }

    mb();
}

portFORCE_INLINE void vPortYield_Core(int xCoreID)
{
    CLINT_Type *clint = (CLINT_Type *)CORET_BASE;

    switch (xCoreID)
    {
    case CORE_0:
        clint->MSIP0 |= (uint32_t)0x1;
        break;
    case CORE_1:
        clint->MSIP1 |= (uint32_t)0x1;
        break;
    case CORE_2:
        clint->MSIP2 |= (uint32_t)0x1;
        break;
    case CORE_3:
        clint->MSIP3 |= (uint32_t)0x1;
        break;
    default:
        break;
    }

    mb();
}
#endif /* configNUMBER_OF_CORES > 1 */

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

