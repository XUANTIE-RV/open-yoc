/*
 * Copyright (C) 2014-2023 Alibaba Group Holding Limited
 */

#ifndef PORTMACRO_H
#define PORTMACRO_H

#include <stdlib.h>
#include <stdint.h>
#include <csi_core.h>
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

#define portNOP()                   asm("nop")

extern portLONG ulCriticalNesting;
extern portLONG pendsvflag;

/* Scheduler utilities. */
extern void vPortYield( void );
#define portYIELD()                 vPortYield();

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

#define configASSERT( a )   do {if ((a)==0){printk("Assert : %s %d\r\n", __FILE__, __LINE__);while(1);}}while(0)


#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

