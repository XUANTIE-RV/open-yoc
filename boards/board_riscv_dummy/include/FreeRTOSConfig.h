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

#include "stdio.h"
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

#define CSK_CPU_STACK_EXTRAL    (STACK_M_EXTRAL_SIZE + STACK_V_EXTRAL_SIZE + STACK_F_EXTRAL_SIZE)

#endif /* __ASSEMBLY__ */

#define configUSE_PREEMPTION        1
#define configUSE_IDLE_HOOK         0
#define configUSE_TICK_HOOK         0
#ifdef CONFIG_SYSTICK_HZ
#define configTICK_RATE_HZ          CONFIG_SYSTICK_HZ
#else
#define configTICK_RATE_HZ          100
#endif
#define configMINIMAL_STACK_SIZE    ( ( unsigned short ) (512 + CSK_CPU_STACK_EXTRAL / sizeof(long)) ) //stack depth
#define configMAX_TASK_NAME_LEN     ( 32 )
#define configUSE_TRACE_FACILITY    1
#define configUSE_16_BIT_TICKS      0
#define configIDLE_SHOULD_YIELD     1
#define configUSE_CO_ROUTINES       0
#define configUSE_MUTEXES           1
#define configCHECK_FOR_STACK_OVERFLOW  1
#define configUSE_RECURSIVE_MUTEXES     1
#define configQUEUE_REGISTRY_SIZE       10
#define configUSE_MALLOC_FAILED_HOOK    1
#define configUSE_STATS_FORMATTING_FUNCTIONS    1
#define configUSE_TIMERS    1
#define configTIMER_TASK_PRIORITY    1
#define configTIMER_QUEUE_LENGTH    128
#define configTIMER_TASK_STACK_DEPTH    (1024 + (CSK_CPU_STACK_EXTRAL / sizeof(long)))
#define configUSE_TIME_SLICING    1
#define configUSE_COUNTING_SEMAPHORES    1
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS 5
#define configUSE_APPLICATION_TASK_TAG  1 // for pthread
#define configUSE_TICKLESS_IDLE         1

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

#define configKERNEL_INTERRUPT_PRIORITY         ( ( unsigned char ) 7 << ( unsigned char ) 5 )  /* Priority 7, or 255 as only the top three bits are implemented.  This is the lowest priority. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( ( unsigned char ) 5 << ( unsigned char ) 5 )  /* Priority 5, or 160 as only the top three bits are implemented. */

extern volatile unsigned long ulHighFrequencyTimerTicks;
/* There is already a high frequency timer running - just reset its count back
to zero. */
/*
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() ( ulHighFrequencyTimerTicks = 0UL )
#define portGET_RUN_TIME_COUNTER_VALUE()    ulHighFrequencyTimerTicks
*/
#endif /*CONFIG_KERNEL_FREERTOS*/
#endif /* FREERTOS_CONFIG_H */
