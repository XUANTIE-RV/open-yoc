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
#define portCHAR        char
#define portFLOAT       float
#define portDOUBLE      double
#define portLONG        long
#define portSHORT       short
#define portSTACK_TYPE  uint32_t
#define portBASE_TYPE   long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;
typedef void (*portvectorfunc)(void);

#if( configUSE_16_BIT_TICKS == 1 )
    typedef uint16_t  TickType_t;
    #define portMAX_DELAY ( TickType_t ) 0xffff
#else
    typedef uint32_t  TickType_t;
    #define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#endif


/* Hardware specifics. */
#define portBYTE_ALIGNMENT          8
#define portSTACK_GROWTH            -1
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )

static inline void vPortEnableInterrupt( void )
{
    __enable_irq();
}

static inline void vPortDisableInterrupt( void )
{
    __disable_irq();
}

static inline portLONG SaveLocalPSR (void)
{
    portLONG flags = csi_irq_save();
    return flags;
}

static inline void RestoreLocalPSR (portLONG newMask)
{
    csi_irq_restore(newMask);
}

extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );
extern __attribute__((naked)) void cpu_yeild(void);

#define portDISABLE_INTERRUPTS()                vPortDisableInterrupt()
#define portENABLE_INTERRUPTS()                 vPortEnableInterrupt()
#define portENTER_CRITICAL()                    vPortEnterCritical()
#define portEXIT_CRITICAL()                     vPortExitCritical()
#define portSET_INTERRUPT_MASK_FROM_ISR()       SaveLocalPSR()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(a)    RestoreLocalPSR(a)

#if configGENERATE_RUN_TIME_STATS
#define portGET_RUN_TIME_COUNTER_VALUE()        csi_tick_get_ms()
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#endif

#define portNOP()                   asm("nop")

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

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

