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

#ifndef __PERF_IPC_H__
#define __PERF_IPC_H__

#include <perf.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PERFDATA_DUMP_TASK_STACK_SIZE 8192

/**
 * @brief semaphore handle type
 */
typedef void* perf_sem_t;
typedef void* perf_task_t;

/**
 * @brief Function pointer type for Thread API
 *
 */
typedef void (*perf_thread_func)(void *arg);

/**
 * @brief Function pointer type for task switch hook callbacks
 */
typedef void (*perf_task_switch_hook)(perf_thread_id thread);

/**
 * @brief Create and start a new thread
 * 
 * @param name Thread name
 * @param func Thread entry function
 * @param args Arguments passed to thread function
 * @param stack_start Lowest valid address of the stack
 * @param stack_size Stack size in bytes
 * @param prio Thread priority
 * @param cpu_id CPU to bind
 * @return New task on success, NULL on failure
 */
perf_task_t perf_thread_create(char *name,
                               void (*func)(void *args),
                               void *args,
                               void *stack_start,
                               size_t stack_size,
                               int prio,
                               int cpu_id);

/**
 * @brief Delete a thread
 *
 * @param task Thread name
 * @return PERF_OK on success, PERF_ERROR on failure
 */
perf_err_t perf_thread_delete(perf_task_t task);

/**
 * @brief Create and initialize a semaphore
 * 
 * @param sem pointer to perf_sem_t
 * @param init_count The initial count value for the semaphore
 * @return PERF_OK on success, PERF_ERROR on failure or timeout
 */
perf_err_t perf_sem_new(perf_sem_t* sem, uint32_t init_count);

/**
 * @brief Acquire the semaphore
 * 
 * @param sem Pointer to the semaphore handle
 * @param timeout_ms Timeout in milliseconds, 0 for no wait
 * @return PERF_OK on success, PERF_ERROR on failure or timeout
 */
perf_err_t perf_sem_wait(perf_sem_t sem, uint32_t timeout_ms);

/**
 * @brief Release the semaphore
 * 
 * @param sem Pointer to the semaphore handle
 * @return PERF_OK on success, PERF_ERROR on failure
 */
perf_err_t perf_sem_signal(perf_sem_t sem);

/**
 * @brief Destroy the semaphore and free resources
 * 
 * @param sem Pointer to the semaphore handle
 * @return PERF_OK on success, PERF_ERROR on failure
 */
perf_err_t perf_sem_free(perf_sem_t sem);

/**
 * @brief Yield the current thread to other threads of the same priority
 * 
 * This function causes the calling thread to yield the processor to another
 * thread that is ready to run and has the same priority. If no other threads
 * of equal priority are ready to run, the calling thread continues execution.
 * 
 * @return perf_err_t PERF_OK on success, PERF_ERROR on failure
 */
perf_err_t perf_thread_yield(void);

/**
 * @brief Set a hook function that will be called on each thread switch
 * 
 * This function registers a callback function that will be invoked whenever
 * the system switches from one thread to another. The hook function receives
 * the thread being switched to as its parameter.
 * 
 * @param hook Function pointer to the hook function
 */
void perf_task_switch_sethook(perf_task_switch_hook hook);

/**
 * @brief Check if an address is within the current thread's stack
 * 
 * This function determines whether the given memory address belongs to
 * the stack of the currently running thread.
 * 
 * @param addr Memory address to check
 * @return bool true if the address is within the current thread's stack, false otherwise
 */
bool perf_is_valid_stack(unsigned long addr);

#ifdef __cplusplus
}
#endif

#endif  // __PERF_IPC_H__
