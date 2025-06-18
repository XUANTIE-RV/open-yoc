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

#ifndef __PERF_FREERTOS_CONFIG_H__
#define __PERF_FREERTOS_CONFIG_H__

#ifndef __ASSEMBLY__

/**
 * This file should be included in FreeRTOSConfig.h
 */
#ifdef CONFIG_KERNEL_FREERTOS

#include <perf_types.h>

/**
 * Number of bins of the hash table
 */
#ifndef CONFIG_NR_PERF_TASK_TABLE_BINS
#define CONFIG_NR_PERF_TASK_TABLE_BINS 53
#endif

/**
 * For FreeRTOS, stack information in TCB_t is not accessible to users.
 * We should add these macros to its trace hooks on task creating or deleting.
 * information of tasks are maintained in a hash table.
 * So we can access pxTCB or pxCurrentTCB, as well as its members.
 */
struct perf_tcb_st {
    // Define the contents of the TCB as needed
    void* thread_id;
    // task info
    unsigned long* stack_addr_high;
    unsigned long* stack_addr_low;
};
typedef struct perf_tcb_st* perf_tcb_t;

// Hash table node structure
struct perf_thread_hashnode_st {
    void* address;                   // Thread address
    perf_tcb_t tcb;                      // Pointer to the Task Control Block
    struct perf_thread_hashnode_st* next; // Pointer to the next node
};
typedef struct perf_thread_hashnode_st* perf_thread_hashnode_t;

/**
 * This macro should be called by the trace hook when a task is created.
 * It inserts the task to the task table and fill in its information.
 */
#define PERF_FREERTOS_TASK_CREATE_CALLBACK(tcb, stk_hi, stk_lo)         \
    do {                                                                \
        perf_task_table_insert((unsigned long*)tcb,                     \
            (unsigned long*)stk_hi,                                     \
            (unsigned long*)stk_lo);                                    \
    } while(0);                                                         \

#define PERF_FREERTOS_ENTER_TASK_CREATE_CALLBACK()                      \
    do {                                                                \
        perf_task_table_create_entry();                                 \
    } while(0);                                                         \

/**
 * This macro should be called by the trace hook when a task is deleted.
 * It deletes a task from the task table.
 */
#define PERF_FREERTOS_TASK_DELETE_CALLBACK(tcb)                         \
    do{                                                                 \
        if (tcb == NULL)                                                \
            perf_task_table_delete(pxCurrentTCB);                       \
        else                                                            \
            perf_task_table_delete(tcb);                                \
    } while(0);                                                         \

#define PERF_FREERTOS_TASK_SWITCH_CALLBACK(next_task)                   \
    do {                                                                \
        perf_on_task_switch(next_task);                                 \
    } while(0);

/**
 * This hook function creates a task table entry.
 * @return  if success, return PERF_OK, otherwise PERF_ERROR
 */
perf_err_t perf_task_table_create_entry(void);

/**
 * This hook function inserts a task to the task table and fill in its information。
 * Should be called after 'perf_task_table_create_entry'
 * @param[in] tid task to insert
 * @param[in] stack_addr_high  highest address of the stack, if the stack grows downwards, this is the start address
 * @param[in] stack_addr_low  lowest address of the stack, if the stack grows downwards, this is the boundary of it
 * @return  if success, return PERF_OK, otherwise PERF_ERROR
 */
perf_err_t perf_task_table_insert(void* tid, unsigned long* stack_addr_high, unsigned long* stack_addr_low);

/**
*  This hook function deletes a task from the table
*  @param[in] tid task to delete
*  @return  if success, return PERF_OK, otherwise PERF_ERROR
*/
perf_err_t perf_task_table_delete(void* tid);

/**
*  Traversing the task table and execute the callback for each task.
*  @param[in] callback the callback function
*/
void perf_task_table_traverse(void (*callback)(struct perf_thread_hashnode_st* node));

/**
 * This hook function should be call by the trace hooks when a task switch happens.
 * @param[in] to task to switch to
 */
void perf_on_task_switch(void* to);

#endif /* __ASSEMBLY__ */

#endif /* CONFIG_KERNEL_FREERTOS */

#endif /* __PERF_FREERTOS_CONFIG_H__ */
