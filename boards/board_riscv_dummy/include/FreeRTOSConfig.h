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

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#ifdef CONFIG_KERNEL_FREERTOS

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/
#ifndef __ASSEMBLY__
/* define extra stack size */
#if __riscv_matrix || __riscv_xtheadmatrix
static inline int _csi_xmlenb_get_value(void)
{
    int result;
    __asm volatile("csrr %0, xmlenb" : "=r"(result) : : "memory");
    return result;
}
#define STACK_M_EXTRAL_SIZE     (_csi_xmlenb_get_value() * 8 + 24)
#else
#define STACK_M_EXTRAL_SIZE     0
#endif

#ifdef __riscv_vector
static inline int _csi_vlenb_get_value(void)
{
    int result;
    __asm volatile("csrr %0, vlenb" : "=r"(result) : : "memory");
    return result;
}

#define STACK_V_EXTRAL_SIZE     (_csi_vlenb_get_value() * 32 + 40)
#else
#define STACK_V_EXTRAL_SIZE     0
#endif

#ifdef __riscv_flen
#define STACK_F_EXTRAL_SIZE     (__riscv_flen / 8 * 32 + 8)
#else
#define STACK_F_EXTRAL_SIZE     0
#endif /*__riscv_flen*/

#if defined(__riscv_matrix) || defined(__riscv_xtheadmatrix) || defined(__riscv_vector)
/* FIXME: for static allocate stack */
#define STATIC_CSK_CPU_STACK_EXTRAL       (16384)
#else
#define STATIC_CSK_CPU_STACK_EXTRAL       (0)
#endif

#define CSK_CPU_STACK_EXTRAL    (STACK_M_EXTRAL_SIZE + STACK_V_EXTRAL_SIZE + STACK_F_EXTRAL_SIZE)

#endif /* __ASSEMBLY__ */

#if CONFIG_NR_CPUS > 1
#define configNUMBER_OF_CORES            CONFIG_NR_CPUS
#define configRUN_MULTIPLE_PRIORITIES    1
#define configUSE_PASSIVE_IDLE_HOOK      0
#define configUSE_CORE_AFFINITY          1
#define configCHECK_FOR_STACK_OVERFLOW   2
#else
#define configNUMBER_OF_CORES           1
#define configCHECK_FOR_STACK_OVERFLOW  1
#endif
#define configUSE_PREEMPTION        1
#define configUSE_IDLE_HOOK         0
#define configUSE_TICK_HOOK         0
#ifdef CONFIG_SYSTICK_HZ
#define configTICK_RATE_HZ          CONFIG_SYSTICK_HZ
#else
#define configTICK_RATE_HZ          100
#endif
#define configMINIMAL_STACK_SIZE    ( ( unsigned short ) (512 + STATIC_CSK_CPU_STACK_EXTRAL / sizeof(long)) ) //stack depth
#define configMAX_TASK_NAME_LEN     ( 32 )
#define configUSE_TRACE_FACILITY    1
#define configUSE_16_BIT_TICKS      0
#define configIDLE_SHOULD_YIELD     1
#define configUSE_CO_ROUTINES       0
#define configUSE_MUTEXES           1
#define configUSE_RECURSIVE_MUTEXES     1
#define configQUEUE_REGISTRY_SIZE       10
#define configUSE_MALLOC_FAILED_HOOK    1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1
#define configUSE_TIMERS    1
#define configTIMER_TASK_PRIORITY    1
#define configTIMER_QUEUE_LENGTH    128
#define configTIMER_TASK_STACK_DEPTH    (1024 + (STATIC_CSK_CPU_STACK_EXTRAL / sizeof(long)))
#define configUSE_TIME_SLICING    1
#define configUSE_COUNTING_SEMAPHORES    1
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5
#define configUSE_APPLICATION_TASK_TAG  1 // for pthread
#define configUSE_TICKLESS_IDLE         1
#define configSUPPORT_STATIC_ALLOCATION         1
#define configKERNEL_PROVIDED_STATIC_MEMORY     1
#define configRECORD_STACK_HIGH_ADDRESS         1

#define portCRITICAL_NESTING_IN_TCB             0
#define portTICK_PERIOD_MS	( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portHAS_STACK_OVERFLOW_CHECKING 1

#define configGENERATE_RUN_TIME_STATS 1

#define configMAX_PRIORITIES            62
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet        1
#define INCLUDE_uxTaskPriorityGet       1
#define INCLUDE_vTaskDelete             1
#define INCLUDE_vTaskCleanUpResources   0
#define INCLUDE_vTaskSuspend            1
#define INCLUDE_vTaskDelayUntil         1
#define INCLUDE_vTaskDelay              1
#define INCLUDE_uxTaskGetStackHighWaterMark    0
#define INCLUDE_xTaskGetCurrentTaskHandle    1
#define INCLUDE_xTaskGetSchedulerState    1
#define INCLUDE_eTaskGetState    1
#define INCLUDE_xSemaphoreGetMutexHolder  1
#define INCLUDE_xTimerPendFunctionCall    1
#define INCLUDE_xTaskGetHandle    1

#define configKERNEL_INTERRUPT_PRIORITY         ( ( unsigned char ) 7 << ( unsigned char ) 5 )  /* Priority 7, or 255 as only the top three bits are implemented.  This is the lowest priority. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( ( unsigned char ) 5 << ( unsigned char ) 5 )  /* Priority 5, or 160 as only the top three bits are implemented. */

#if defined(CONFIG_DEBUG) && CONFIG_DEBUG
#define configASSERT( a )   do {if ((a)==0){portDISABLE_INTERRUPTS();printk("[CPU %d], Assert : %s %d\r\n", portGET_CORE_ID(), __FILE__, __LINE__);while(1);}}while(0)
#else
#define configASSERT( a )   do {if ((a)==0){portDISABLE_INTERRUPTS();printk("[CPU %d], Assert : %s %d\r\n", portGET_CORE_ID(), __FILE__, __LINE__);}}while(0)
#endif

#ifndef __ASSEMBLY__
#if configNUMBER_OF_CORES > 1
extern volatile uint32_t g_irq_nested_level[configNUMBER_OF_CORES];
#define portASSERT_IF_IN_ISR()  configASSERT(g_irq_nested_level[portGET_CORE_ID()] == 0)
#else
extern volatile uint32_t g_irq_nested_level;
#define portASSERT_IF_IN_ISR()  configASSERT(g_irq_nested_level == 0)
#endif
#endif /* __ASSEMBLY__ */

/**
 * if using perf, these macros are defined in perf_freertos_config.h
 * otherwise they are defined to empty
 */
#if CONFIG_PROFILING_PERF
#include <perf_freertos_config.h>
#else
#define PERF_FREERTOS_TASK_CREATE_CALLBACK(pxTCB, pxHighAddr, pxLowAddr)
#define PERF_FREERTOS_ENTER_TASK_CREATE_CALLBACK()
#define PERF_FREERTOS_TASK_DELETE_CALLBACK(pxTCB)
#define PERF_FREERTOS_TASK_SWITCH_CALLBACK(pxNextTask)
#endif /* CONFIG_PROFILING_PERF */

/**
* trace facilities, profiling functions and macros can be added to the 'do...while' blocks
*/

#define traceTASK_CREATE( pxNewTCB )                                    \
    do {                                                                \
        PERF_FREERTOS_TASK_CREATE_CALLBACK(pxNewTCB,                    \
                pxNewTCB->pxEndOfStack,                                 \
                (unsigned long*)((unsigned long)pxNewTCB->pxStack))     \
    } while(0);

#define traceENTER_xTaskCreateStatic(pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer) \
    do {                                                                \
        PERF_FREERTOS_ENTER_TASK_CREATE_CALLBACK()                      \
    } while(0);                                                         \

#define traceENTER_xTaskCreateStaticAffinitySet( pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, puxStackBuffer, pxTaskBuffer, uxCoreAffinityMask ) \
    do {                                                                \
        PERF_FREERTOS_ENTER_TASK_CREATE_CALLBACK()                      \
    } while(0);                                                         \

#define traceENTER_xTaskCreate(pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, pxCreatedTask) \
    do {                                                                \
        PERF_FREERTOS_ENTER_TASK_CREATE_CALLBACK()                      \
    } while(0);                                                         \

#define traceENTER_xTaskCreateAffinitySet(pxTaskCode, pcName, uxStackDepth, pvParameters, uxPriority, uxCoreAffinityMask, pxCreatedTask) \
    do {                                                                \
        PERF_FREERTOS_ENTER_TASK_CREATE_CALLBACK()                      \
    } while(0);                                                         \

#define traceENTER_vTaskDelete(xTaskToDelete)                           \
    do {                                                                \
        PERF_FREERTOS_TASK_DELETE_CALLBACK(xTaskToDelete)               \
    } while(0);                                                         \

#define traceTASK_SWITCHED_IN()                                         \
    do {                                                                \
        PERF_FREERTOS_TASK_SWITCH_CALLBACK(pxCurrentTCB)                \
    } while(0);

#endif /*CONFIG_KERNEL_FREERTOS*/
#endif /* FREERTOS_CONFIG_H */
