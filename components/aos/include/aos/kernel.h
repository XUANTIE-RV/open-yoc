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

#ifndef AOS_KERNEL_H
#define AOS_KERNEL_H

#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AOS_WAIT_FOREVER    0xffffffffu
#define AOS_NO_WAIT         0x0

#ifndef AOS_DEFAULT_APP_PRI
#define AOS_DEFAULT_APP_PRI 32
#endif

/// AOS优先级数值越大，优先级等级越小
#ifndef AOS_MAX_APP_PRI
#define AOS_MAX_APP_PRI     60
#endif

#define AOS_EVENT_AND              0x02u
#define AOS_EVENT_AND_CLEAR        0x03u
#define AOS_EVENT_OR               0x00u
#define AOS_EVENT_OR_CLEAR         0x01u

/*
 * Scheduling policies
 */
#define AOS_KSCHED_OTHER        0u
#define AOS_KSCHED_FIFO         1u
#define AOS_KSCHED_RR           2u
#define AOS_KSCHED_CFS          0u /* Alias to SCHED_OTHER */

/**
 * @addtogroup aos_kernel_task
 * 提供AliOS Things系统内核任务管理功能的基础API.
 *
 * @{
 */
#define AOS_TASK_NONE              0x0u  /**< 表示不指定任意选项，当调用aos_task_create()创建任务时，用来指定option参数 */
#define AOS_TASK_AUTORUN           0x01u /**< 表示任务创建后即可被调度执行，当调用aos_task_create()创建任务时，用来指定option参数*/
/** @} */

/**
 * @addtogroup aos_kernel_timer
 * 提供AliOS Things系统内核定时器功能的基础API.
 *
 * @{
 */
#define AOS_TIMER_NONE             0x0u  /**< 表示不指定任意选项，当调用aos_timer_create()创建定时器时，用来指定option参数 */
#define AOS_TIMER_AUTORUN          0x01u /**< 表示定时器创建后即启动，当调用aos_timer_create()创建定时器时，用来指定option参数*/
#define AOS_TIMER_REPEAT           0x02u /**< 表示定时器是周期性的，当调用aos_timer_create()创建定时器时，用来指定option参数 */
/** @} */

/// Entry point of a timer call back function.
typedef void (*aos_timer_cb_t)(void *, void *);

typedef void* aos_task_t;
typedef void* aos_mutex_t;
typedef void* aos_sem_t;
typedef void* aos_queue_t;
typedef void* aos_timer_t;
typedef void* aos_work_t;

typedef void* aos_event_t;
#ifdef CONFIG_KERNEL_RTTHREAD
typedef struct {
	char reserve[AOS_RTT_SPIN_LOCK_STRUCT_SIZE];
} aos_spinlock_t;
#else
typedef struct {
	char reserve[8];
} aos_spinlock_t;
#endif

typedef struct {
    void *hdl;
    void *stk;
} aos_workqueue_t;

typedef unsigned int aos_task_key_t;
/* Define the data type for function return */
typedef int32_t aos_status_t; /**< AOS返回值状态类型 */

/**
 * 创建任务，该接口为创建任务分配TCB（任务控制块）并且根据指定的执行体、任务名称、栈大小来初始化对应成员.
 * 该接口任务栈是由内核分配的。
 *
 * @par 使用约束
 * 该接口不能在中断上下文中调用
 *
 * @par 错误处理
 * 如果任务执行体入口函数为NULL，或任务名为NULL，或任务句柄为NULL，则返回错误码-EINVAL \n
 * 如果栈大小为零，则返回错误 \n
 * 如果任务优先级超过配置上限或等于IDLE任务优先级，则返回错误码-EINVAL \n
 *
 * @param[in]  task        任务对象句柄.
 * @param[in]  name        任务名称.若任务名称为空，则使用默认任务名“default_name”。
 * @param[in]  fn          任务执行体入口函数。
 * @param[in]  arg         任务执行体入口函数的参数。
 * @param[in]  stack_buf   栈空间地址，如果地址为空则内核根据stack_size为任务分配栈空间.
 * @param[in]  stack_size  栈大小（字节为单位）。
 * @param[in]  prio        任务优先级，最大指由配置参数AOS_MAX_APP_PRI(默认为60)决定.
 * @param[in]  options     任务创建选项,当前支持选项：\n
 *                         @ref AOS_TASK_AUTORUN 任务创建后自动加入就绪队列，可被调度器调度执行. \n
 *
 * @return  状态码
 * @retval 0 创建任务成功
 * @retval -EINVAL 输入非法参数导致失败
 * @retval -ENOMEM 内存不足导致失败
 * @retval -1 其他原因导致的失败
 */
aos_status_t aos_task_create(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg,void *stack_buf, size_t stack_size, int32_t prio, uint32_t options);

/**
 * 创建任务，该接口为创建任务分配TCB（任务控制块）并且根据指定的执行体、任务名称、栈大小来初始化对应成员.
 * 该接口任务栈是由内核分配的。
 *
 * @par 使用约束
 * 该接口不能在中断上下文中调用
 *
 * @par 错误处理
 * 如果任务执行体入口函数为NULL，或任务名为NULL，或任务句柄为NULL，则返回错误码-EINVAL \n
 * 如果栈大小为零，则返回错误 \n
 * 如果任务优先级超过配置上限或等于IDLE任务优先级，则返回错误码-EINVAL \n
 *
 * @param[in]  task        任务对象句柄.
 * @param[in]  name        任务名称.若任务名称为空，则使用默认任务名“default_name”。
 * @param[in]  fn          任务执行体入口函数。
 * @param[in]  arg         任务执行体入口函数的参数。
 * @param[in]  stack_buf   栈空间地址，如果地址为空则内核根据stack_size为任务分配栈空间.
 * @param[in]  stack_size  栈大小（字节为单位）。
 * @param[in]  prio        任务优先级，最大指由配置参数AOS_MAX_APP_PRI(默认为60)决定.
 * @param[in]  options     任务创建选项,当前支持选项：\n
 *                         @ref AOS_TASK_AUTORUN 任务创建后自动加入就绪队列，可被调度器调度执行. \n
 * @param[in]  cpuid       需要绑定的cpuid
 *
 * @return  状态码
 * @retval 0 创建任务成功
 * @retval -EINVAL 输入非法参数导致失败
 * @retval -ENOMEM 内存不足导致失败
 * @retval -1 其他原因导致的失败
 */
aos_status_t aos_task_create_ext(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg,void *stack_buf, size_t stack_size, int32_t prio, uint32_t options, uint32_t cpuid);

/**
 * 挂起任务，该接口将已创建的任务挂起，暂时不执行，挂起的对象既可以是任务自身也可以是其他任务，\n
 * 但不允许挂起IDLE任务。
 *
 * @par 使用约束
 * 该接口不能在中断上下文中调用
 *
 * @par 错误处理
 * 如果任务句柄为NULL，则返回错误码-EINVAL\n
 * 如果挂起任务为IDLE，则返回错误码-EPERM \n
 *
 * @param[in]  task        任务对象句柄.
 *
 * @return  状态码
 * @retval 0 挂起任务成功
 * @retval -EINVAL 输入非法参数导致失败
 * @retval -EPERM 尝试挂起IDLE任务导致失败
 * @retval -1 其他原因导致的失败
 */
aos_status_t aos_task_suspend(aos_task_t *task);

/**
 * 恢复任务，该接口将挂起任务恢复，取消暂时不执行状态。
 *
 * @par 使用约束
 * 该接口不能在中断上下文中调用
 *
 * @par 错误处理
 * 如果任务句柄为NULL，则返回错误码-EINVAL\n
 *
 * @param[in]  task        任务对象句柄.
 *
 * @return  状态码
 * @retval 0 恢复任务成功
 * @retval -EINVAL 输入非法参数导致失败
 * @retval -1 其他原因导致的失败
 */
aos_status_t aos_task_resume(aos_task_t *task);

/**
 * 删除任务，该接口删除一个任务并回收任务资源，不允许删除IDLE任务。
 *
 * @par 使用约束
 * 该接口不能在中断上下文中调用
 *
 * @par 错误处理
 * 如果任务句柄为NULL，则返回错误码-EINVAL\n
 *  如果删除的任务为IDLE，则返回错误码-EPERM \n
 *
 * @param[in]  task        任务对象句柄.
 *
 * @return  状态码
 * @retval 0 恢复任务成功
 * @retval -EINVAL 输入非法参数导致失败
 * @retval -EPERM 尝试删除IDLE任务导致失败
 * @retval -1 其他原因导致的失败
 */
aos_status_t aos_task_delete(aos_task_t *task);

/**
 * Create a task.
 *
 * @param[in]  name       task name.
 * @param[in]  fn         function to run.
 * @param[in]  arg        argument of the function.
 * @param[in]  stacksize  stack-size in bytes.
 *
 * @return  0: success.
 */
int aos_task_new(const char *name, void (*fn)(void *), void *arg, int stack_size);

/**
 * Create a task.
 *
 * @param[in]  task        handle.
 * @param[in]  name        task name.
 * @param[in]  fn          task function.
 * @param[in]  arg         argument of the function..
 * @param[in]  stack_buf   stack-buf: if stack_buf==NULL, provided by kernel.
 * @param[in]  stack_size  stack-size in bytes.
 * @param[in]  prio        priority value, the max is AOS_MAX_APP_PRI(default 60).
 *
 * @return  0: success.
 */
int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio);

/**
 * show all tasks info.
 *
 */
void aos_task_show_info(void);

/**
 * Yield a task.
 */
void aos_task_yield();

/**
 * Exit a task.
 *
 * @param[in]  code  not used now.
 */
void aos_task_exit(int code);

/**
 * Get the current task handle
 *
 * @return  the task handle
 */
aos_task_t aos_task_self();

/**
 * Find the task handle with task name
 *
 * @return  the task handle
 */
aos_task_t aos_task_find(char *name);

/**
 * Get current task name
 *
 * @return  the name of the current task
 */
const char *aos_task_name(void);

/**
 * Get task name with task handle
 *
 * @return  the name of the task
 */
const char *aos_task_get_name(aos_task_t *task);

/**
 * Create a task key.
 *
 * @param[in]  key  pointer of key object.
 *
 * @return  0: success, -EINVAL: error.
 */
int aos_task_key_create(aos_task_key_t *key);

/**
 * Delete a task key.
 *
 * @param[in]  key  key object.
 */
void aos_task_key_delete(aos_task_key_t key);

/**
 * Associate a task-specific value with a key.
 *
 * @param[in]  key  key object.
 * @param[in]  vp   pointer of a task-specific value.
 *
 * @return  the check status, 0 is OK, -1 indicates invalid.
 */
int aos_task_setspecific(aos_task_key_t key, void *vp);

/**
 * Get the value currently bound to the specified key.
 *
 * @param[in]  key  key object.
 */
void *aos_task_getspecific(aos_task_key_t key);

/**
 * Get a task pthread control block.
 *
 * @param[in]   task        task handle
 * @param[out]  ptcb        the returned ptcb handle
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_ptcb_get(aos_task_t *task, void **ptcb);

/**
 * Set a task pthread control block.
 *
 * @param[in]   task        task handle
 * @param[in]   ptcb        the ptcb handle
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_ptcb_set(aos_task_t *task, void *ptcb);

/**
 * Change a task's schedule priority
 *
 * @param[in]   task        task handle
 * @param[in]   pri         the new task priority
 * @param[out]  old_pri     the returned old task priority
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_pri_change(aos_task_t *task, uint8_t pri, uint8_t *old_pri);

/**
 * Get a task's schedule priority
 *
 * @param[in]   task        task handle
 * @param[out]  priority    the returned old task priority
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_pri_get(aos_task_t *task, uint8_t *priority);

/**
 * Set a task's schedule policy
 *
 * @param[in]   task        task handle
 * @param[in]   policy      the new task's policy
 * @param[in]   pri         the new task's priority
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_sched_policy_set(aos_task_t *task, uint8_t policy, uint8_t pri);

/**
 * Get a task's schedule policy
 *
 * @param[in]   task        task handle
 * @param[out]  policy      the returned task's policy
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_sched_policy_get(aos_task_t *task, uint8_t *policy);

/**
 * Get a task's default schedule policy
 *
 * @return  AOS_KSCHED_CFS or AOS_KSCHED_RR
 */
uint32_t aos_task_sched_policy_get_default(void);

/**
 * Msleep.
 *
 * @param[in]  ms  sleep time in milliseconds.
 */
void aos_msleep(int ms);

/**
 * aos task software watchdog
 */
void aos_task_wdt_attach(void (*will)(void *), void *args);
void aos_task_wdt_detach();
void aos_task_wdt_feed(int time);

/**
 * Set a task's schedule time slice
 *
 * @param[in]   task        task handle
 * @param[in]   slice       the new schedule time slice, time slice in AOS_KSCHED_RR mode (ms)
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_time_slice_set(aos_task_t *task, uint32_t slice);

/**
 * Get a task's schedule time slice
 *
 * @param[in]   task        task handle
 * @param[out]  slice       the returned task's time slice, time slice in AOS_KSCHED_RR mode (ms)
 *
 * @return  0:  success     otherwise failed
 */
aos_status_t aos_task_time_slice_get(aos_task_t *task, uint32_t *slice);

/**
 * Alloc a mutex.
 *
 * @param[in]  mutex  pointer of mutex object, mutex object must be alloced,
 *                    hdl pointer in aos_mutex_t will refer a kernel obj internally.
 *
 * @return  0: success.
 */
int aos_mutex_new(aos_mutex_t *mutex);

/**
 * Free a mutex.
 *
 * @param[in]  mutex  mutex object, mem refered by hdl pointer in aos_mutex_t will
 *                    be freed internally.
 */
void aos_mutex_free(aos_mutex_t *mutex);

/**
 * Lock a mutex.
 *
 * @param[in]  mutex    mutex object, it contains kernel obj pointer which aos_mutex_new alloced.
 * @param[in]  timeout  waiting until timeout in milliseconds.
 *
 * @return  0: success.
 */
int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout);

/**
 * Unlock a mutex.
 *
 * @param[in]  mutex  mutex object, it contains kernel obj pointer which oc_mutex_new alloced.
 *
 * @return  0: success.
 */
int aos_mutex_unlock(aos_mutex_t *mutex);

/**
 * This function will check if mutex is valid.
 *
 * @param[in]  mutex  pointer to the mutex.
 *
 * @return  0: invalid, 1: valid.
 */
int aos_mutex_is_valid(aos_mutex_t *mutex);

/**
 * Alloc a semaphore.
 *
 * @param[out]  sem    pointer of semaphore object, semaphore object must be
 *                     alloced, hdl pointer in aos_sem_t will refer a kernel obj internally.
 * @param[in]   count  initial semaphore counter.
 *
 * @param[in]   options  reserved.
 * @return  0:success.
 */
aos_status_t aos_sem_create(aos_sem_t *sem, uint32_t count, uint32_t options);

/**
 * Alloc a semaphore.
 *
 * @param[out]  sem    pointer of semaphore object, semaphore object must be alloced,
 *                     hdl pointer in aos_sem_t will refer a kernel obj internally.
 * @param[in]   count  initial semaphore counter.
 *
 * @return  0:success.
 */
int aos_sem_new(aos_sem_t *sem, int count);

/**
 * Destroy a semaphore.
 *
 * @param[in]  sem  pointer of semaphore object, mem refered by hdl pointer
 *                  in aos_sem_t will be freed internally.
 */
void aos_sem_free(aos_sem_t *sem);

/**
 * Acquire a semaphore.
 *
 * @param[in]  sem      semaphore object, it contains kernel obj pointer which aos_sem_new alloced.
 * @param[in]  timeout  waiting until timeout in milliseconds.
 *
 * @return  0: success.
 */
int aos_sem_wait(aos_sem_t *sem, unsigned int timeout);

/**
 * Release a semaphore.
 *
 * @param[in]  sem  semaphore object, it contains kernel obj pointer which aos_sem_new alloced.
 */
void aos_sem_signal(aos_sem_t *sem);

/**
 * This function will check if semaphore is valid.
 *
 * @param[in]  sem  pointer to the semaphore.
 *
 * @return  0: invalid, 1: valid.
 */
int aos_sem_is_valid(aos_sem_t *sem);

/**
 * Release all semaphore.
 *
 * @param[in]  sem  semaphore object, it contains kernel obj pointer which aos_sem_new alloced.
 */
void aos_sem_signal_all(aos_sem_t *sem);

/**
 * This function will create an event with an initialization flag set.
 * This function should not be called from interrupt context.
 *
 * @param[in]  event    event object pointer.
 * @param[in]  flags    initialization flag set(provided by caller).
 *
 * @return  0: success.
 */
int aos_event_new(aos_event_t *event, unsigned int flags);

/**
 * This function will free an event.
 * This function shoud not be called from interrupt context.
 *
 * @param[in]  event    memory refered by hdl pointer in event will be freed.
 *
 * @return  N/A.
 */
void aos_event_free(aos_event_t *event);

/**
 * This function will try to get flag set from given event, if the request flag
 * set is satisfied, it will return immediately, if the request flag set is not
 * satisfied with timeout(AOS_WAIT_FOREVER,0xFFFFFFFF), the caller task will be
 * pended on event until the flag is satisfied, if the request flag is not
 * satisfied with timeout(AOS_NO_WAIT, 0x0), it will also return immediately.
 * Note, this function should not be called from interrupt context because it has
 * possible to lead context switch and an interrupt has no TCB to save context.
 *
 * @param[in]  event        event object pointer.
 * @param[in]  flags        request flag set.
 * @param[in]  opt          operation type, such as AND,OR,AND_CLEAR,OR_CLEAR.
 * @param[out] actl_flags   the internal flags value hold by event.
 * @param[in]  timeout      max wait time in millisecond.
 *
 * @return  0: success.
 */
int aos_event_get(aos_event_t *event, unsigned int flags, unsigned char opt,
                  unsigned int *actl_flags, unsigned int timeout);

/**
* This function will set flag set to given event, and it will check if any task
* which is pending on the event should be waken up.
*
* @param[in]  event    event object pointer.
* @param[in]  flags    flag set to be set into event.
* @param[in]  opt      operation type, such as AND,OR.
*
* @return  0: success.
*/
int aos_event_set(aos_event_t *event, unsigned int flags, unsigned char opt);

/**
 * This function will check if event is valid.
 *
 * @param[in]  event    event object pointer.
 *
 * @return  0: invalid, 1: valid.
 */
int aos_event_is_valid(aos_event_t *event);

/**
 * This function will create a queue.
 *
 * @param[in]  queue    pointer to the queue(the space is provided by user).
 * @param[in]  size     the bytes of the buf.
 * @param[in]  max_msg  the max size of one msg.
 * @param[in]  options    reserved.
 * @return  0: success.
 */
aos_status_t aos_queue_create(aos_queue_t *queue, size_t size, size_t max_msgsize, uint32_t options);

/**
 * This function will create a queue.
 *
 * @param[in]  queue    pointer to the queue(the space is provided by user).
 * @param[in]  buf      buf of the queue(provided by user).
 * @param[in]  size     the bytes of the buf.
 * @param[in]  max_msg  the max size of one msg.
 *
 * @return  0: success.
 */
int aos_queue_new(aos_queue_t *queue, void *buf, size_t size, int max_msgsize);

/**
 * This function will delete a queue.
 *
 * @param[in]  queue  pointer to the queue.
 */
void aos_queue_free(aos_queue_t *queue);

/**
 * This function will send a msg to the front of a queue.
 *
 * @param[in]  queue  pointer to the queue.
 * @param[in]  msg    msg to send.
 * @param[in]  size   size of the msg.
 *
 * @return  0: success.
 */
int aos_queue_send(aos_queue_t *queue, void *msg, size_t size);

/**
 * This function will receive msg from a queue.
 *
 * @param[in]   queue  pointer to the queue.
 * @param[in]   ms     ms to wait before receive.
 * @param[out]  msg    buf to save msg.
 * @param[out]  size   size of the msg.
 *
 * @return  0: success.
 */
int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg, size_t *size);

/**
 * This function will check if queue is valid.
 *
 * @param[in]  queue  pointer to the queue.
 *
 * @return  0: invalid, 1: valid.
 */
int aos_queue_is_valid(aos_queue_t *queue);

/**
 * This function will return buf ptr if queue is inited.
 *
 * @param[in]  queue  pointer to the queue.
 *
 * @return  NULL: error.
 */
void *aos_queue_buf_ptr(aos_queue_t *queue);

/**
 * Get number of queued messages in a message queue.
 *
 * @param[in]  queue  message queue handle to operate.
 *
 * @return  number of queued messages.negative indicates error code.
 */
int aos_queue_get_count(aos_queue_t *queue);

/**
 * This function will create a timer and run auto.
 *
 * @param[in]  timer   pointer to the timer.
 * @param[in]  fn      callbak of the timer.
 * @param[in]  arg     the argument of the callback.
 * @param[in]  ms      ms of the normal timer triger.
 * @param[in]  repeat  repeat or not when the timer is created.
 *
 * @return  0: success.
 */
int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat);

/**
 * This function will create a timer.
 *
 * @param[in]  timer   pointer to the timer.
 * @param[in]  fn      callbak of the timer.
 * @param[in]  arg     the argument of the callback.
 * @param[in]  ms      ms of the normal timer triger.
 * @param[in]  repeat  repeat or not when the timer is created.
 * @param[in]  auto_run  run auto or not when the timer is created.
 *
 * @return  0: success.
 */
int aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *),
                      void *arg, int ms, int repeat, unsigned char auto_run);

/**
 * This function will delete a timer.
 *
 * @param[in]  timer  pointer to a timer.
 */
void aos_timer_free(aos_timer_t *timer);

/**
 * This function will start a timer.
 *
 * @param[in]  timer  pointer to the timer.
 *
 * @return  0: success.
 */
int aos_timer_start(aos_timer_t *timer);

/**
 * This function will stop a timer.
 *
 * @param[in]  timer  pointer to the timer.
 *
 * @return  0: success.
 */
int aos_timer_stop(aos_timer_t *timer);

/**
 * This function will change attributes of a timer.
 *
 * @param[in]  timer  pointer to the timer.
 * @param[in]  ms     ms of the timer triger.
 *
 * @return  0: success.
 */
int aos_timer_change(aos_timer_t *timer, int ms);

/**
 * This function will change attributes of a timer.
 *
 * @param[in]  timer  pointer to the timer.
 * @param[in]  ms     ms of the timer triger.
 *
 * @return  0: success.
 */
int aos_timer_change_once(aos_timer_t *timer, int ms);

/**
 * This function will check if timer is valid.
 *
 * @param[in]  timer  pointer to the timer.
 *
 * @return  0: success.
 */
int aos_timer_is_valid(aos_timer_t *timer);

/**
 * This function will check if timer is active.
 *
 * @param[in]  timer  pointer to the timer.
 *
 * @return  1: active. 0: not active
 */
int aos_timer_is_active(aos_timer_t *timer);

/**
 * This function will creat a workqueue.
 *
 * @param[in]  workqueue   the workqueue to be created.
 * @param[in]  pri         the priority of the worker.
 * @param[in]  stack_size  the size of the worker-stack.
 *
 * @return  0: success.
 */
int aos_workqueue_create(aos_workqueue_t *workqueue, int pri, int stack_size);

/**
 * This function will creat a workqueue.
 *
 * @param[in]  workqueue   the workqueue to be created.
 * @param[in]  name        the name of workqueue.
 * @param[in]  pri         the priority of the worker.
 * @param[in]  stack_size  the size of the worker-stack.
 *
 * @return  0: success.
 */
int aos_workqueue_create_ext(aos_workqueue_t *workqueue, const char *name, int pri, int stack_size);

/**
 * This function will delete a workqueue.
 *
 * @param[in]  workqueue  the workqueue to be deleted.
 */
void aos_workqueue_del(aos_workqueue_t *workqueue);

/**
 * This function will initialize a work.
 *
 * @param[in]  work  the work to be initialized.
 * @param[in]  fn    the call back function to run.
 * @param[in]  arg   the paraments of the function.
 * @param[in]  dly   ms to delay before run.
 *
 * @return  0: success.
 */
int aos_work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly);

/**
 * This function will destroy a work.
 *
 * @param[in]  work  the work to be destroied.
 */
void aos_work_destroy(aos_work_t *work);

/**
 * This function will run a work on a workqueue.
 *
 * @param[in]  workqueue  the workqueue to run work.
 * @param[in]  work       the work to run.
 *
 * @return  0: success.
 */
int aos_work_run(aos_workqueue_t *workqueue, aos_work_t *work);

/**
 * This function will run a work on the default workqueue.
 *
 * @param[in]  work  the work to run.
 *
 * @return  0: success.
 */
int aos_work_sched(aos_work_t *work);

/**
 * This function will cancel a work on the default workqueue.
 *
 * @param[in]  work  the work to cancel.
 *
 * @return  0: success.
 */
int aos_work_cancel(aos_work_t *work);

/**
 * Realloc memory.
 *
 * @param[in]  mem   current memory address point.
 * @param[in]  size  new size of the mem to remalloc.
 *
 * @return  NULL: error.
 */
void *aos_realloc(void *mem, size_t size);
void *aos_realloc_check(void *mem, size_t size);

/**
 * Alloc memory.
 *
 * @param[in]  size  size of the mem to malloc.
 *
 * @return  NULL: error.
 */
void *aos_malloc(size_t size);
void *aos_malloc_check(size_t size);

/**
 * Alloc memory and clear to zero.
 *
 * @param[in]  size  size of the mem to malloc.
 *
 * @return  NULL: error.
 */
void *aos_zalloc(size_t size);
void *aos_zalloc_check(size_t size);

void *aos_calloc(size_t size, size_t num);
void *aos_calloc_check(size_t size, size_t num);

/**
 * Trace alloced mems.
 *
 * @param[in]  addr       pointer of the mem alloced by malloc.
 * @param[in]  allocator  buildin_address.
 */
void aos_alloc_trace(void *addr, size_t allocator);

/**
 * Free memory.
 *
 * @param[in]  ptr  address point of the mem.
 */
void aos_free(void *mem);

/**
 * Free memory and set NULL.
 *
 * @param[in]  *ptr  address point of the mem.
 */
void aos_freep(char **ptr);

/**
 * This function allocates a memory block, which address is aligned to the
 * specified alignment size.
 *
 * @param  alignment is the alignment size.
 * @param  size is the allocated memory block size.
 * @return The memory block address was returned successfully, otherwise it was
 *         returned empty NULL.
 */
void *aos_malloc_align(size_t alignment, size_t size);

/**
 * This function release the memory block, which is allocated by
 * aos_malloc_align function and address is aligned.
 *
 * @param ptr is the memory block pointer.
 */
void aos_free_align(void *ptr);


//////////////////////////////////////////////////////////////////////////////////
/**
 * Reboot system.
 */
void aos_reboot(void);

/**
 * Get HZ(ticks per second).
 *
 * @return  ticks freq per second
 */
int aos_get_hz(void);

/**
 * Get YoC SDK version.
 *
 * @return  YoC SDK version.
 */
const char *aos_version_get(void);

/**
 * Get kernel version.
 *
 * @return  kernel version.
 */
const char *aos_kernel_version_get(void);

#define AOS_SCHEDULER_NOT_STARTED   0
#define AOS_SCHEDULER_RUNNING       1
#define AOS_SCHEDULER_SUSPENDED     2
/**
 * Get the kernel status
 *
 * @return  the status of the kernel
 *          AOS_SCHEDULER_NOT_STARTED
 *          AOS_SCHEDULER_RUNNING
 *          AOS_SCHEDULER_SUSPENDED
 */
int aos_kernel_status_get(void);

/**
 * System enter interrupt status.
 *
 * @return execution status code.
 */
int aos_kernel_intrpt_enter(void);

/**
 * System exit interrupt status.
 *
 * @return execution status code.
 */
int aos_kernel_intrpt_exit(void);

/**
 * Suspend the scheduler.
 */
void aos_kernel_sched_suspend(void);

/**
 * Resume the scheduler.
 */
void aos_kernel_sched_resume(void);

/**
 * Ticks to milliseconds
 *
 * @return  milliseconds
 */
uint64_t aos_kernel_tick2ms(uint64_t ticks);

/**
 * Milliseconds to ticks
 *
 * @return  ticks
 */
uint64_t aos_kernel_ms2tick(uint64_t ms);

/**
 * Get the number of ticks before next os tick event.
 *
 * @return  -1 or the number of ticks（time in ticks, for how long the system can sleep or power-down.）
 */
int32_t aos_kernel_next_sleep_ticks_get(void);

/**
 * Announces elapsed ticks to the kernel
 * @param[in]  ticks   time in ticks for how long the system was in sleep or power-down mode.
 */
void aos_kernel_ticks_announce(int32_t ticks);

/**
 * Get current time in nano seconds.
 *
 * @return  elapsed time in nano seconds from system starting.
 */
long long aos_now(void);

/**
 * Get current time in mini seconds.
 *
 * @return  elapsed time in mini seconds from system starting.
 */
long long aos_now_ms(void);

/**
 * Get the current time from system startup, in ticks.
 *
 * @return  system ticks
 */
long long aos_sys_tick_get(void);

/**
 * Set calendar time.
 * @param[in]  now_ms the calender time (unit:ms) to set, for example,
 *                    the number of milliseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
 */
void aos_calendar_time_set(uint64_t now_ms);

/**
 * Get calendar time.
 *
 * @return current calendar time (unit:ms), for example,
 *         the number of milliseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
 */
uint64_t aos_calendar_time_get(void);

/**
 * Get calendar time with utc offset.
 *
 * @return current calendar time with utc offset (unit:ms).
 */
uint64_t aos_calendar_localtime_get(void);

/**
 * Get the timer's time vaule.
 *
 * @param[in]  timer  pointer to the timer.
 * @param[out] value  store the returned time. 
 *                       struct itimerspec {
 *                           struct timespec  it_interval;  // the spacing time
 *                           struct timespec  it_value;     // first delay start time
 *                       };
 *                       it_interval.tv_sec   = value[0];
 *                       it_interval.tv_nsec  = value[1];
 *                       it_value.tv_sec      = value[2];
 *                       it_value.tv_nsec     = value[3];
 * 
 * 
 * @return  0: success, otherwise error.
 */
int aos_timer_gettime(aos_timer_t *timer, uint64_t value[4]);

/**
 * Initialize kernel
 */
void aos_init(void);

/**
 * Start kernel
 */
void aos_start(void);

/**
 * The sys tick handler for OS
 */
void aos_sys_tick_handler(void);

/**
 * Get aos memory used info.
 *
 * @param[out]  total   the total memory can be use.
 * @param[out]  used    the used memory by malloc.
 * @param[out]  mfree   the free memory can be use.
 * @param[out]  peak    the peak memory used.
 *
 * @return  execution status code.
 */
int aos_get_mminfo(int32_t *total, int32_t *used, int32_t *mfree, int32_t *peak);

/**
 * Dump aos memory .
 *
 * @return  execution status code.
 */
int aos_mm_dump(void);

/**
 * Get the maximum number of system's schedule priority
 *
 * @param[in]   policy      the task's schedule policy
 *
 * @return  maximum schedule priority
 */
uint32_t aos_sched_get_priority_max(uint32_t policy);

/**
 * Get the minimum number of system's schedule priority
 *
 * @param[in]   task        task handle
 * @param[in]   policy      the task's schedule policy
 *
 * @return  minimum schedule priority
 */
static inline uint32_t aos_sched_get_priority_min(uint32_t policy)
{
    return 1;
}

/**
 * Whether is in the interrupted context
 *
 * @return    1 is in interrupted context, 0 is not in interrupted context
 */
int aos_irq_context(void);

/**
 * Whether the kernel schedule is disabled
 *
 * @return    1 is disabled, 0 is enable
 */
int aos_is_sched_disable(void);

/**
 * Whether the cpu irq is disabled
 *
 * @return    1 is disabled, 0 is enable
 */
int aos_is_irq_disable(void);

/**
 * Get the current hartid
 *
 * @return    cpu id
 */
int aos_get_cur_cpu_id(void);

/**
 * Init a spinlock
 *
 * @param[in]   lock        the spinlock
 */
void aos_spin_lock_init(aos_spinlock_t *lock);

/**
 * Lock a spinlock
 *
 * @param[in]   lock        the spinlock
 */
void aos_spin_lock(aos_spinlock_t *lock);

/**
 * Unlock a spinlock
 *
 * @param[in]   lock        the spinlock
 */
void aos_spin_unlock(aos_spinlock_t *lock);

/**
 * Lock a spinlock and mask interrupt
 *
 * @param[in]   lock    the spinlock
 *
 * @return  the irq status
 */
unsigned long aos_spin_lock_irqsave(aos_spinlock_t *lock);

/**
 * Unlock a spinlock and restore interrupt masking status
 *
 * @param[in]   lock    the spinlock
 * @param[in]   flags   the irq status before locking
 */
void aos_spin_lock_irqrestore(aos_spinlock_t *lock, unsigned long flags);

#if defined(CONFIG_SMP) && CONFIG_SMP
/**
 * boot secondary cpu
 */
void aos_secondary_cpu_up(void);

#endif

/**
 * The exception callback function set
 */
typedef void (*except_process_t)(int errno, const char *file, int line, const char *func_name, void *caller);
void aos_set_except_callback(except_process_t except);
void aos_set_except_default();

#ifdef __cplusplus
}
#endif

#endif /* AOS_KERNEL_H */

