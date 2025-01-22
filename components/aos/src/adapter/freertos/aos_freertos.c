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

#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include <FreeRTOSConfig.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>
#include <event_groups.h>
#include <aos/aos.h>
#include <aos/wdt.h>
#include <debug/dbg.h>

#define AOS_MIN_STACK_SIZE       64

#define CHECK_HANDLE(handle)                                 \
    do                                                       \
    {                                                        \
        if(handle == NULL || (void *)(*handle) == NULL)      \
        {                                                    \
            return -EINVAL;                                  \
        }                                                    \
    } while (0)

static aos_task_key_t used_bitmap;
static long long start_time_ms = 0;

// weak hook
__attribute__((weak)) void aos_task_create_hook_lwip_thread_sem(aos_task_t *task)
{

}

__attribute__((weak)) void aos_task_del_hook_lwip_thread_sem(aos_task_t *task, void *arg)
{
    
}

void vApplicationMallocFailedHook( void )
{
    // do not assert
}

volatile int g_intrpt_nested_cnt;
static inline bool is_in_intrp(void)
{
    return g_intrpt_nested_cnt > 0;
}

void aos_reboot_ext(int cmd)
{
    extern void drv_reboot(int cmd);
    drv_reboot(cmd);
}

void aos_reboot(void)
{
    aos_reboot_ext(0);
}

int aos_get_hz(void)
{
    return configTICK_RATE_HZ;
}

const char *aos_version_get(void)
{
    return aos_get_os_version();
}

const char *aos_kernel_version_get(void)
{
    static char ver_buf[32] = {0};
    if (ver_buf[0] == 0) {
        snprintf(ver_buf, sizeof(ver_buf), "FreeRTOS %s", tskKERNEL_VERSION_NUMBER);
    }
    return ver_buf;
}

int aos_kernel_status_get(void)
{
    BaseType_t state = xTaskGetSchedulerState();
    if (state == taskSCHEDULER_SUSPENDED)
        return AOS_SCHEDULER_SUSPENDED;
    else if (state == taskSCHEDULER_RUNNING)
        return AOS_SCHEDULER_RUNNING;
    else
        return AOS_SCHEDULER_NOT_STARTED;
}

#define AOS_MAGIC 0x20171020
typedef struct {
    uint32_t key_bitmap;
    void *keys[4];
    void *stack;
    char name[32];
    uint32_t magic;
} AosStaticTask_t;

void vPortCleanUpTCB(void *pxTCB)
{
    AosStaticTask_t *task = (AosStaticTask_t *)pxTCB;

    if (task->magic != AOS_MAGIC) {
        return;
    }

    free(task->stack);
    free(task);
}

aos_status_t aos_task_suspend(aos_task_t *task)
{
    CHECK_HANDLE(task);

    TaskHandle_t ptask = (TaskHandle_t)*task;
    vTaskSuspend(ptask);

    return 0;
}

aos_status_t aos_task_resume(aos_task_t *task)
{
    CHECK_HANDLE(task);

    TaskHandle_t ptask = (TaskHandle_t)*task;
    vTaskResume(ptask);

    return 0;
}

/*in aos_task_create, it is not recommended to set argsument options to AOS_TASK_NONE 
because it is not recommended in FreeRTOS to create a task and then immediately suspend it.*/

aos_status_t aos_task_create(aos_task_t *task, const char *name, void (*fn)(void *),
                             void *arg, void *stack, size_t stack_size, int32_t prio, uint32_t options)
{
    int       ret;
    uint8_t   old_pri = prio;

    if (task == NULL) {
        return -EINVAL;
    }

    if (stack != NULL) {
        // TODO:
    }

    /*The task priority is set to the lowest level for preventing immediate task switching 
    due to high priority when the task is successfully created.*/

    if (options == AOS_TASK_NONE) {
        prio = configMAX_PRIORITIES - 1;
    }

    ret = aos_task_new_ext(task,name,fn,arg,stack_size,prio);
    if (ret != 0) {
        return ret;
    }

    if (options == AOS_TASK_NONE) { 
        ret = aos_task_suspend(task);
        if (ret != 0) {
            return ret;
        } 
        ret = aos_task_pri_change(task, prio, &old_pri);
        if (ret != 0) {
            return ret;
        } 
    }

    return 0;
}

int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 int stack_size)
{
    aos_task_t task;

    aos_check_return_einval(name && fn && (stack_size >= AOS_MIN_STACK_SIZE));

    return aos_task_new_ext(&task,name,fn,arg,stack_size,AOS_DEFAULT_APP_PRI);
}

int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio)
{
    TaskHandle_t xHandle;
    /* vreify param */
    int prio_freertos = configMAX_PRIORITIES - 1 - prio;

    aos_check_return_einval(task && fn && (stack_size >= AOS_MIN_STACK_SIZE) &&
                            (prio_freertos >= 0 && prio_freertos < configMAX_PRIORITIES));

    if (name == NULL) {
        return -EFAULT; // match as rhino
    }

#if defined(CSK_CPU_STACK_EXTRAL)
    stack_size += CSK_CPU_STACK_EXTRAL;
#endif
    BaseType_t ret = xTaskCreate(fn, name, stack_size / sizeof( StackType_t ), arg, prio_freertos, &xHandle);
    if (ret == pdPASS) {
        if(task) {
            *task = xHandle;
        }
        aos_task_create_hook_lwip_thread_sem(task);
        return 0;
    }
    return -1;
}

void aos_task_exit(int code)
{
    /* task exit by itself */
    vTaskDelete(NULL);
    aos_task_t task = aos_task_self();

    aos_task_del_hook_lwip_thread_sem(&task, NULL);
}

aos_status_t aos_task_delete(aos_task_t *task)
{
    CHECK_HANDLE(task);

    TaskHandle_t ptask = (TaskHandle_t)*task;

    vTaskDelete(ptask);

    return 0;
}

const char *aos_task_name(void)
{
    TaskHandle_t ptask = (TaskHandle_t)xTaskGetCurrentTaskHandle();
    return pcTaskGetTaskName(ptask);
}

const char *aos_task_get_name(aos_task_t *task)
{
    TaskHandle_t ptask = (TaskHandle_t)*task;

    return pcTaskGetTaskName(ptask);
}

aos_task_t aos_task_find(char *name)
{
    // TODO: Not support
    return NULL;
}

void aos_task_show_info(void)
{
    uint32_t total_task_number = 0;
    uint32_t i;
    uint32_t total_time;
    TaskStatus_t *pTaskStatus = NULL, *pTaskStatus_bk = NULL;
    total_task_number = uxTaskGetNumberOfTasks();
    pTaskStatus = (TaskStatus_t*)aos_malloc(sizeof(TaskStatus_t)* total_task_number);
    pTaskStatus_bk = pTaskStatus;
    if(pTaskStatus == NULL) {
        return;
    }

    total_task_number = uxTaskGetSystemState(pTaskStatus,total_task_number,&total_time);
    printf("task                 pri stack_base sp         status  min_remained  \n");
    printf("-------------------- --- ---------- ---------  ------  ------------  \n");


    for (i = 0; i < total_task_number; i++) {

        printf("%-20s %3d %p %p ", pTaskStatus->pcTaskName, (uint32_t)(pTaskStatus->uxCurrentPriority), pTaskStatus->pxStackBase, (void *)(*(unsigned long *)pTaskStatus->xHandle));

        if (pTaskStatus->eCurrentState == eRunning)            printf("runing ");
        else if ( pTaskStatus->eCurrentState == eReady)        printf("ready  ");
        else if ( pTaskStatus->eCurrentState == eBlocked)      printf("blocked");
        else if ( pTaskStatus->eCurrentState == eSuspended)    printf("suspend");
        else if ( pTaskStatus->eCurrentState == eDeleted)      printf("deleted");
        else
            printf("%-7d", pTaskStatus->eCurrentState);

        printf(" %-13d \n",pTaskStatus->usStackHighWaterMark);
        pTaskStatus++;
    }

    aos_free(pTaskStatus_bk);
}

aos_status_t aos_task_ptcb_get(aos_task_t *task, void **ptcb)
{
    CHECK_HANDLE(task);
    if (ptcb == NULL) {
        return -EINVAL;
    }
#if ( configUSE_APPLICATION_TASK_TAG == 1 )
    TaskHandle_t xTask = (TaskHandle_t)*task;
    /* Return a reference to this pthread object, which is stored in the
     * FreeRTOS task tag. */
    void *pxthread = (void *)xTaskGetApplicationTaskTag( xTask );
    *ptcb = pxthread;
#endif
    return 0;
}

aos_status_t aos_task_ptcb_set(aos_task_t *task, void *ptcb)
{
    CHECK_HANDLE(task);
#if ( configUSE_APPLICATION_TASK_TAG == 1 )
    TaskHandle_t xTask = (TaskHandle_t)*task;
    /* Store the pointer to the thread object in the task tag. */
    vTaskSetApplicationTaskTag( xTask, ( TaskHookFunction_t ) ptcb );
#endif
    return 0;
}

aos_status_t aos_task_pri_change(aos_task_t *task, uint8_t pri, uint8_t *old_pri)
{
    CHECK_HANDLE(task);

    TaskHandle_t ptask = (TaskHandle_t)*task;

    if (pri > configMAX_PRIORITIES - 1)
        return -EINVAL;

    if (old_pri == NULL)
        return -EFAULT; // match as rhino

    vTaskPrioritySet(ptask, configMAX_PRIORITIES - 1 - pri);

    return 0;
}

aos_status_t aos_task_pri_get(aos_task_t *task, uint8_t *priority)
{
    CHECK_HANDLE(task);
    if (priority == NULL) {
        return -EINVAL;
    }

    TaskHandle_t ptask = (TaskHandle_t)*task;

    UBaseType_t pri = uxTaskPriorityGet(ptask);
    if (pri > configMAX_PRIORITIES - 1)
        return -1;
    *priority = configMAX_PRIORITIES - 1 - pri;

    return 0;
}

int aos_task_key_create(aos_task_key_t *key)
{
    int i;
    if (NULL == key)
        return -EINVAL;

    for (i = configNUM_THREAD_LOCAL_STORAGE_POINTERS - 1; i >= 0; i--) {
        if (!((1 << i) & used_bitmap)) {
            used_bitmap |= 1 << i;
            *key = i;

            return 0;
        }
    }

    return -EINVAL;
}

void aos_task_key_delete(aos_task_key_t key)
{
    if (key >= configNUM_THREAD_LOCAL_STORAGE_POINTERS) {
        return;
    }

    uint32_t bit = 1 << key;
    TaskHandle_t task = (TaskHandle_t)xTaskGetCurrentTaskHandle();

    if ((used_bitmap & bit) == bit) {
        used_bitmap &= ~(bit);
        vTaskSetThreadLocalStoragePointer(task, key, NULL);
    }
}

int aos_task_setspecific(aos_task_key_t key, void *vp)
{
    TaskHandle_t task = (TaskHandle_t)xTaskGetCurrentTaskHandle();
    uint32_t bit = 1 << key;

    if ((used_bitmap & bit) == bit) {
        vTaskSetThreadLocalStoragePointer(task, key, vp);
    } else {
        return -EINVAL;
    }

    return 0;
}

void *aos_task_getspecific(aos_task_key_t key)
{
    void *vp = NULL;
    uint32_t bit = 1 << key;
    TaskHandle_t task = (TaskHandle_t)xTaskGetCurrentTaskHandle();

    if ((used_bitmap & bit) == bit) {
        vp = pvTaskGetThreadLocalStoragePointer(task, key);
    }

    return vp;
}

void aos_task_wdt_attach(void (*will)(void *), void *args)
{
#ifdef CONFIG_SOFTWDT
    aos_wdt_attach((long)xTaskGetCurrentTaskHandle(), will, args);
#else
    (void)will;
    (void)args;
#endif
}

void aos_task_wdt_detach()
{
#ifdef CONFIG_SOFTWDT
    long index = (long)xTaskGetCurrentTaskHandle();

    aos_wdt_feed(index, 0);
    aos_wdt_detach(index);
#endif
}

void aos_task_wdt_feed(int time)
{
#ifdef CONFIG_SOFTWDT
    TaskHandle_t task = xTaskGetCurrentTaskHandle();
    char *task_name = pcTaskGetTaskName(task);

    if (!aos_wdt_exists((long)task))
        aos_wdt_attach((long)task, NULL, (void *)task_name);

    aos_wdt_feed((long)task, time);
#endif
}

int aos_mutex_new(aos_mutex_t *mutex)
{
    aos_check_return_einval(mutex);

    SemaphoreHandle_t mux = xSemaphoreCreateRecursiveMutex();
    *mutex = mux;
    return mux != NULL ? 0 : -1;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    aos_check_return(mutex && *mutex);

    vSemaphoreDelete(*mutex);

    *mutex = NULL;
}

int aos_mutex_lock(aos_mutex_t *mutex, unsigned int ms)
{
    CHECK_HANDLE(mutex);

    if (is_in_intrp()) {
        return -EPERM;
    }
    if (mutex && *mutex) {
        if ((xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED) && ms != 0) {
            return -EPERM;
        }
        BaseType_t ret = xSemaphoreTakeRecursive(*mutex, ms == AOS_WAIT_FOREVER ? portMAX_DELAY : pdMS_TO_TICKS(ms));
        if (ret != pdPASS) {
            return ms == AOS_NO_WAIT ? -EBUSY : -ETIMEDOUT;
        }
    }
    return 0;
}

int aos_mutex_unlock(aos_mutex_t *mutex)
{
    CHECK_HANDLE(mutex);

    if (is_in_intrp()) {
        return -EPERM;
    }
    if (mutex && *mutex) {
        BaseType_t ret = xSemaphoreGiveRecursive(*mutex);
        if (ret != pdPASS)
            return -1;
    }

    return 0;
}

int aos_mutex_is_valid(aos_mutex_t *mutex)
{
    return mutex && *mutex != NULL;
}

#define SEM_MAXCOUNT 0xffffffffu
aos_status_t aos_sem_create(aos_sem_t *sem, uint32_t count, uint32_t options)
{
    aos_check_return_einval(sem);

    SemaphoreHandle_t s = xSemaphoreCreateCounting(SEM_MAXCOUNT, count);
    *sem = s;
    return s != NULL ? 0 : -1;
}

int aos_sem_new(aos_sem_t *sem, int count)
{
    aos_check_return_einval(sem && (count >= 0));

    SemaphoreHandle_t s = xSemaphoreCreateCounting(SEM_MAXCOUNT, count);
    *sem = s;
    return s != NULL ? 0 : -1;
}

void aos_sem_free(aos_sem_t *sem)
{
    aos_check_return(sem && *sem);

    vSemaphoreDelete(*sem);

    *sem = NULL;
}

int aos_sem_wait(aos_sem_t *sem, unsigned int ms)
{
    aos_check_return_einval(sem && *sem);

    if (ms == 0) {
        return (-EBUSY);
    }
    if (is_in_intrp()) {
        return -EPERM;
    }
    int ret = xSemaphoreTake(*sem, ms == AOS_WAIT_FOREVER ? portMAX_DELAY : pdMS_TO_TICKS(ms));
    return ret == pdPASS ? 0 : -ETIMEDOUT;
}

void aos_sem_signal(aos_sem_t *sem)
{
    aos_check_return(sem && *sem);

    if(is_in_intrp()) {
        BaseType_t  temp = pdTRUE;
        xSemaphoreGiveFromISR(*sem, &temp);
    } else {
        xSemaphoreGive(*sem);
    }
}

int aos_sem_is_valid(aos_sem_t *sem)
{
    return sem && *sem != NULL;
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    // TODO: Not support
}
typedef struct {
    xQueueHandle q;
    int msg_size;
} queue_hdl_t;

aos_status_t aos_queue_create(aos_queue_t *queue, size_t size, size_t max_msgsize, uint32_t options)
{
    aos_check_return_einval(queue && (size > 0) && (max_msgsize > 0));

    xQueueHandle q;
    queue_hdl_t *q_hdl = aos_zalloc(sizeof(queue_hdl_t));
    if (q_hdl == NULL)
        return -ENOMEM;

    /* create queue object */
    q = xQueueCreate(size / max_msgsize, max_msgsize);
    if(q == NULL) {
        aos_free(q_hdl);
        return -1;
    }

    q_hdl->q = q;
    q_hdl->msg_size = max_msgsize;
    *queue = q_hdl;

    return 0;
}

int aos_queue_new(aos_queue_t *queue, void *buf, size_t size, int max_msgsize)
{
    aos_check_return_einval(queue && buf && (size > 0) && (max_msgsize > 0));

    xQueueHandle q;
    (void)(buf);

    /* create queue object */
    q = xQueueCreate(size / max_msgsize, max_msgsize);
    if(q == NULL)
        return -ENOMEM;
    queue_hdl_t *q_hdl = aos_zalloc(sizeof(queue_hdl_t));
    if (q_hdl == NULL)
        return -ENOMEM;
    q_hdl->q = q;
    q_hdl->msg_size = max_msgsize;
    *queue = q_hdl;

    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    aos_check_return(queue && *queue);

    queue_hdl_t *q_hdl = *queue;
    /* delete queue object */
    vQueueDelete(q_hdl->q);
    aos_free(q_hdl);

    *queue = NULL;

    return;
}

int aos_queue_send(aos_queue_t *queue, void *msg, size_t size)
{
    /* verify param */
    CHECK_HANDLE(queue);

    if (!msg)
        return -EFAULT; //same as rhino return value

    if (!(size > 0)) {
        return -EINVAL;
    }
    queue_hdl_t *q_hdl = *queue;
    /* send msg  to specific queue */
    return xQueueSend(q_hdl->q, msg, portMAX_DELAY) == pdPASS ? 0 : -1;
}

int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg, size_t *size)
{
    /* verify param */
    CHECK_HANDLE(queue);
    if (size == NULL || msg == NULL) {
        return -EFAULT; //same as rhino return value
    }
    queue_hdl_t *q_hdl = *queue;
    /* receive msg from specific queue */
    BaseType_t ret = xQueueReceive(q_hdl->q, msg, ms == AOS_WAIT_FOREVER ? portMAX_DELAY : pdMS_TO_TICKS(ms));
    if (ret != pdPASS) {
        *size = 0;
        return -1;
    }
    *size = q_hdl->msg_size;
    return 0;
}

int aos_queue_is_valid(aos_queue_t *queue)
{
    return queue && *queue != NULL;
}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    // TODO: Not support
    (void)queue;
    return NULL;
}

int aos_queue_get_count(aos_queue_t *queue)
{
    BaseType_t ret;
    CHECK_HANDLE(queue);
    queue_hdl_t *q_hdl = *queue;
    ret = uxQueueMessagesWaiting(q_hdl->q);
    return ret;
}

typedef struct tmr_adapter {
    TimerHandle_t timer;
    void (*func)(void *, void *);
    void *func_arg;
    uint8_t bIsRepeat;
    uint16_t valid_flag;
    uint64_t init_ms;
    uint64_t round_ms;
} tmr_adapter_t;

static void tmr_adapt_cb(TimerHandle_t xTimer)
{
    tmr_adapter_t *pTmrObj = (tmr_adapter_t *)pvTimerGetTimerID(xTimer);

    if(pTmrObj->func)
        pTmrObj->func(pTmrObj,pTmrObj->func_arg);
    return;
}


int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat)
{
    aos_check_return_einval(timer && fn);

    return aos_timer_new_ext(timer,fn,arg,ms,repeat,1);
}

int aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *),
                      void *arg, int ms, int repeat, unsigned char auto_run)
{
    /* verify param */
    aos_check_return_einval(timer && fn);

    if (ms == 0)
        return -EINVAL;

    /* create timer wrap object ,then initlize timer object */
    tmr_adapter_t *tmr_adapter = pvPortMalloc(sizeof(tmr_adapter_t));

    if (tmr_adapter == NULL) {
        return -1;
    }

    tmr_adapter->func = fn;
    tmr_adapter->func_arg = arg;
    tmr_adapter->bIsRepeat = repeat;
    tmr_adapter->init_ms = ms;
    tmr_adapter->round_ms = (repeat == true) ? ms : 0;

    /* create timer by kernel api */
    TimerHandle_t ptimer = xTimerCreate("Timer", pdMS_TO_TICKS(ms), repeat, tmr_adapter, tmr_adapt_cb);

    if (timer == NULL) {
        vPortFree(tmr_adapter);
        return -1;
    }
    tmr_adapter->valid_flag = 0xA598;

    tmr_adapter->timer = ptimer;
    *timer = (void*)tmr_adapter;

    /* start timer if auto_run == TRUE */
    if(auto_run) {
        if(aos_timer_start(timer) != 0) {
            return -1;
        }
    }

    return 0;
}

void aos_timer_free(aos_timer_t *timer)
{
    aos_check_return(timer && *timer);

    tmr_adapter_t *tmr_adapter = *timer;
    int ret = xTimerDelete(tmr_adapter->timer, 0);

    if (!ret) {
        return ;
    }
    tmr_adapter->valid_flag = 0;
    vPortFree(tmr_adapter);
    *timer = NULL;
}

int aos_timer_start(aos_timer_t *timer)
{
    CHECK_HANDLE(timer);

    /* start timer  */
    tmr_adapter_t *tmr_adapter = *timer;
    int ret;

    if(is_in_intrp()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        ret = xTimerStartFromISR(tmr_adapter->timer, &xHigherPriorityTaskWoken);
    } else {
        ret = xTimerStart(tmr_adapter->timer, 0);
    }

    if (ret != pdPASS) {
        return -1;
    }

    return 0;
}

int aos_timer_stop(aos_timer_t *timer)
{
    CHECK_HANDLE(timer);

    /* stop timer */
    tmr_adapter_t *tmr_adapter = *timer;
    int ret;

    if(is_in_intrp()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        ret = xTimerStopFromISR(tmr_adapter->timer, &xHigherPriorityTaskWoken);
    } else {
        ret = xTimerStop(tmr_adapter->timer, 0);
    }

    if (ret != pdPASS) {
        return -1;
    }

    return 0;
}

int aos_timer_is_valid(aos_timer_t *timer)
{
    if (timer == NULL || *timer == NULL) {
        return 0;
    }
    tmr_adapter_t *tmr_adapter = *timer;
    if (tmr_adapter->valid_flag != 0xA598)
        return 0;
    return 1;
}

int aos_timer_is_active(aos_timer_t *timer)
{
    if ((timer == NULL) || (*timer == NULL)) {
        return 0;
    }
    
    tmr_adapter_t *pTimer = *timer;

    if( xTimerIsTimerActive(pTimer->timer) == pdFALSE ) {
        return 0;
    }
    else {
        return 1;
    }
}

int aos_timer_change(aos_timer_t *timer, int ms)
{
    int ret = -1;
    CHECK_HANDLE(timer);

    tmr_adapter_t *pTimer = *timer;

    if(is_in_intrp()) {
        BaseType_t  xHigherProTskWoken  = pdFALSE;
        ret = xTimerChangePeriodFromISR(pTimer->timer,pdMS_TO_TICKS(ms),&xHigherProTskWoken);
    } else {
        ret = xTimerChangePeriod(pTimer->timer,pdMS_TO_TICKS(ms),10);
    }

    if (ret == pdPASS) {
        return 0;
    } else {
        return -1;
    }

    return ret;
}

/*
该接口的使用与rhino内核的有区别
freertos:
创建的多周期时钟，调用该接口无效
只有创建单周期时钟，调用该接口才有效
rhino:
创建的多周期时钟，调用该接口也是有效的
*/
int aos_timer_change_once(aos_timer_t *timer, int ms)
{
    CHECK_HANDLE(timer);
    
    int ret = -1;

    tmr_adapter_t *pTimer = *timer;

    if(is_in_intrp()) {
        BaseType_t xHigherProTskWoken = pdFALSE;
        ret = xTimerChangePeriodFromISR(pTimer->timer,pdMS_TO_TICKS(ms),&xHigherProTskWoken);
    } else {
        ret = xTimerChangePeriod(pTimer->timer, pdMS_TO_TICKS(ms), 10);
    }

    if (ret == pdPASS) {
        return 0;
    } else {
        return -1;
    }

    return ret;
}

int aos_timer_gettime(aos_timer_t *timer, uint64_t value[4])
{
    CHECK_HANDLE(timer);

    tmr_adapter_t *pTimer = NULL;
    uint64_t init_ms;
    uint64_t round_ms;

    pTimer = (tmr_adapter_t *)*timer;
    init_ms = pTimer->init_ms;
    round_ms = pTimer->round_ms;

    value[0] = round_ms / 1000;
    value[1] = (round_ms % 1000) * 1000000UL;
    value[2] = init_ms / 1000;
    value[3] = (init_ms % 1000) * 1000000UL;

    return 0;
}

int aos_workqueue_create(aos_workqueue_t *workqueue, int pri, int stack_size)
{
    return ENOSYS;
}

void aos_workqueue_del(aos_workqueue_t *workqueue)
{
    // TODO: 不支持
}

int aos_work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly)
{
    return ENOSYS;
}

void aos_work_destroy(aos_work_t *work)
{
    // TODO: 不支持
}

int aos_work_run(aos_workqueue_t *workqueue, aos_work_t *work)
{
    return ENOSYS;
}

int aos_work_sched(aos_work_t *work)
{
    return ENOSYS;
}

int aos_work_cancel(aos_work_t *work)
{
    return ENOSYS;
}

void *aos_zalloc(size_t size)
{
    void* ptr = pvPortMalloc(size);
    if(ptr) {
        bzero(ptr,size);
    }
    return ptr;
}

void *aos_malloc(size_t size)
{
    if (size == 0) {
        return NULL;
    }    

    return pvPortMalloc(size);
}

void *aos_realloc(void *mem, size_t size)
{
    return pvPortRealloc(mem,size);
}


void aos_alloc_trace(void *addr, size_t allocator)
{
    // 在分配的内存区加跟踪信息
    // TODO: freertos不支持，无法使用 CONFIG_DEBUG_MM 功能
}

void aos_free(void *mem)
{
    if (mem == NULL) {
        return;
    }

    vPortFree(mem);
}


void *aos_malloc_align(size_t alignment, size_t size)
{
    void *ptr;
    void *align_ptr;
    size_t align_size = sizeof(void*);

    if (alignment > align_size) {
        for (;;) {
            align_size = align_size << 1;
            if (align_size >= alignment)
                break;
        }
    }
    alignment = align_size;

    /* get total aligned size */
    align_size = size + (alignment << 1);
    /* allocate memory block from heap */
    ptr = aos_malloc(align_size);
    if (ptr != NULL) {
        /* the allocated memory block is aligned */
        if (((unsigned long)ptr & (alignment - 1)) == 0) {
            align_ptr = (void *)((unsigned long)ptr + alignment);
        } else {
            align_ptr = (void *)(((unsigned long)ptr + (alignment - 1)) & ~(alignment - 1));
        }

        /* set the pointer before alignment pointer to the real pointer */
        *((unsigned long *)((unsigned long)align_ptr - sizeof(void *))) = (unsigned long)ptr;
        ptr = align_ptr;
    }

    return ptr;
}

void aos_free_align(void *ptr)
{
    if (ptr) {
        void *real_ptr = (void *)*(unsigned long *)((unsigned long)ptr - sizeof(void *));
        aos_free(real_ptr);
    }
}


void *aos_zalloc_check(size_t size)
{
    void *ptr = aos_malloc(size);

    aos_check_mem(ptr);
    if (ptr) {
        memset(ptr, 0, size);
    }

    return ptr;
}

void *aos_malloc_check(size_t size)
{
    void *p = aos_malloc(size);
    aos_check_mem(p);

    return p;
}

void *aos_calloc(size_t nitems, size_t size)
{
    size_t len = (size_t)nitems*size;
    void *tmp = aos_malloc(len);

    if (tmp) {
        memset(tmp, 0, len);
    }

    return tmp;
}

void *aos_calloc_check(size_t size, size_t num)
{
    return aos_zalloc_check(size * num);
}

void *aos_realloc_check(void *ptr, size_t size)
{
    void *new_ptr = aos_realloc(ptr, size);
    aos_check_mem(new_ptr);

    return new_ptr;
}

//static struct timeval sys_start_time;
long long aos_now(void)
{
    return aos_now_ms()*1000*1000;
}

long long aos_now_ms(void)
{
    long long ms;
    uint32_t tick_cnt = xTaskGetTickCount();
    ms = tick_cnt * 1000 / configTICK_RATE_HZ;
    return ms;
}

long long aos_sys_tick_get(void)
{
    return xTaskGetTickCount();
}

void aos_calendar_time_set(uint64_t now_ms)
{
    start_time_ms = now_ms - aos_now_ms();
}

uint64_t aos_calendar_time_get(void)
{
    return aos_now_ms() + start_time_ms;
}

uint64_t aos_calendar_localtime_get(void)
{
    if ((aos_calendar_time_get() - 8 * 3600 * 1000) < 0) {
        return aos_calendar_time_get();
    }
    return aos_calendar_time_get() + 8 * 3600 * 1000;
}

void aos_msleep(int ms)
{
    vTaskDelay(aos_kernel_ms2tick(ms));
    return;
}

void aos_init(void)
{
    // nothing to do
    return;
}

void aos_start(void)
{
    vTaskStartScheduler();
}

void dumpsys_task_func(void)
{
#if configUSE_TRACE_FACILITY
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    uint32_t ulTotalRunTime, ulStatsAsPercentage;

    uxArraySize = uxTaskGetNumberOfTasks();
    pxTaskStatusArray = malloc( uxArraySize * sizeof( TaskStatus_t ) );
    if( pxTaskStatusArray == NULL )
        return;

    uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalRunTime );
    // For percentage calculations.
    ulTotalRunTime /= 100UL;

    // Avoid divide by zero errors.
    if( ulTotalRunTime > 0 ) {
        // For each populated position in the pxTaskStatusArray array,
        // format the raw data as human readable ASCII data
        for( x = 0; x < uxArraySize; x++ ) {
            // What percentage of the total run time has the task used?
            // This will always be rounded down to the nearest integer.
            // ulTotalRunTimeDiv100 has already been divided by 100.
            ulStatsAsPercentage = pxTaskStatusArray[ x ].ulRunTimeCounter / ulTotalRunTime;

            if( ulStatsAsPercentage > 0UL ) {
                printf("%s\t\t%u\t\t%u%%\r\n", pxTaskStatusArray[ x ].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter, ulStatsAsPercentage );
            } else {
                printf("%s\t\t%u\t\t<1%%\r\n", pxTaskStatusArray[ x ].pcTaskName, pxTaskStatusArray[ x ].ulRunTimeCounter );
            }
        }
    }

    free( pxTaskStatusArray );
#endif
}

int32_t aos_irq_context(void)
{
    return is_in_intrp();
}

void aos_task_yield()
{
    taskYIELD();
}

aos_task_t aos_task_self(void)
{
    return xTaskGetCurrentTaskHandle();
}

/*
freertos限制 flags　<=0x00FFFFFF
*/
int aos_event_new(aos_event_t *event, unsigned int flags)
{
    EventGroupHandle_t event_handle;
    aos_check_return_einval(event);
    *event = NULL;

    if ((flags & 0xff000000UL) != 0) {
        return -EPERM;
    }

    /* create event handle */
    event_handle = xEventGroupCreate();
    /* initlized event */
    if(event_handle != NULL) {
        EventBits_t event_bits = xEventGroupSetBits(event_handle,flags);
        if (flags != 0) {
            if (!(event_bits & flags)) {
                return -1;
            }
        }

    } else {
        return -1;
    }
    *event = event_handle;

    return 0;
}

void aos_event_free(aos_event_t *event)
{
    aos_check_return(event && *event);

    vEventGroupDelete((EventGroupHandle_t)*event);

    *event = NULL;
}

int aos_event_get
(
    aos_event_t *event,
    unsigned int flags,
    unsigned char opt,
    unsigned int *actl_flags,
    unsigned int timeout
)
{
    uint32_t   wait_bits = 0, ticks_wait = 0;;
    uint8_t clean_exit = 0, wait_all_bits = 0;
    aos_check_return_einval(event && *event);

    if (is_in_intrp()) {
        return -EPERM;
    }
    if ((flags & 0xff000000UL) != 0) {
        return -EPERM;
    }
    if ((xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED) && timeout != 0) {
        return -EPERM;
    }

    if (opt == AOS_EVENT_AND) {
        clean_exit = pdFALSE;
        wait_all_bits = pdTRUE;

    } else if (opt == AOS_EVENT_AND_CLEAR) {
        clean_exit = pdTRUE;
        wait_all_bits = pdTRUE;

    } else if (opt == AOS_EVENT_OR) {
        clean_exit = pdFALSE;
        wait_all_bits = pdFALSE;

    } else if (opt == AOS_EVENT_OR_CLEAR) {
        clean_exit = pdTRUE;
        wait_all_bits = pdFALSE;
    }

    ticks_wait = AOS_WAIT_FOREVER;
    if (timeout != AOS_WAIT_FOREVER) {
        ticks_wait = pdMS_TO_TICKS(timeout);
    }

    wait_bits = xEventGroupWaitBits(*event,
                                    flags,
                                    clean_exit,
                                    wait_all_bits,
                                    ticks_wait
                                    );

    *actl_flags = wait_bits;
    if (opt == AOS_EVENT_AND || opt == AOS_EVENT_AND_CLEAR) {
        if ((wait_bits & flags) != flags) {
            return (timeout == AOS_NO_WAIT) ? -EBUSY : -ETIMEDOUT; 
        }
    } else if (opt == AOS_EVENT_OR || opt == AOS_EVENT_OR_CLEAR) {
        if (!(wait_bits & flags)) {
            return (timeout == AOS_NO_WAIT) ? -EBUSY : -ETIMEDOUT;
        }
    }
    return 0;
}

int aos_event_set(aos_event_t *event, unsigned int flags, unsigned char opt)
{
    aos_check_return_einval(event && *event);

    // for compatible with rhino
    if (opt == AOS_EVENT_AND) {
        flags = ~flags;
        flags &= ~0xff000000UL;
    }

    if ((flags & 0xff000000UL) != 0) {
        return -EPERM;
    }

    if(is_in_intrp()) {
        BaseType_t xHighProTaskWoken = pdFALSE;
        if (opt == AOS_EVENT_AND) {
            xEventGroupClearBitsFromISR(*event, flags);
        } else {
            xEventGroupSetBitsFromISR(*event, flags, &xHighProTaskWoken);
        }
    } else {
        if (opt == AOS_EVENT_AND) {
            xEventGroupClearBits(*event, flags);
        } else {
            xEventGroupSetBits(*event, flags);
        }
    }
    return 0;
}

int aos_event_is_valid(aos_event_t *event)
{
    return event && *event != NULL;
}

/// Suspend the scheduler.
void aos_kernel_sched_suspend(void)
{
    vTaskSuspendAll();
}

/// Resume the scheduler.
void aos_kernel_sched_resume()
{
    xTaskResumeAll();
}

int aos_get_mminfo(int32_t *total, int32_t *used, int32_t *mfree, int32_t *peak)
{
    return ENOSYS;
}

int aos_mm_dump(void)
{
    return ENOSYS;
}

uint64_t aos_kernel_tick2ms(uint64_t ticks)
{
    uint64_t   padding;
    uint64_t   time;

    padding = configTICK_RATE_HZ / 1000;
    padding = (padding > 0) ? (padding - 1) : 0;

    time    = ((ticks + padding) * 1000) / configTICK_RATE_HZ;
    return time;
}

uint64_t aos_kernel_ms2tick(uint64_t ms)
{
    uint64_t padding;
    uint64_t   ticks;

    padding = 1000 / configTICK_RATE_HZ;
    padding = (padding > 0) ? (padding - 1) : 0;

    ticks   = ((ms + padding) * configTICK_RATE_HZ) / 1000;

    return ticks;
}

int aos_kernel_intrpt_enter(void)
{
    g_intrpt_nested_cnt ++;
    return 0;
}

int aos_kernel_intrpt_exit(void)
{
    g_intrpt_nested_cnt --;
    BaseType_t state = xTaskGetSchedulerState();
    if (state == taskSCHEDULER_RUNNING)
        portYIELD_FROM_ISR(pdTRUE);
    else
        portYIELD_FROM_ISR(pdFALSE);
    return 0;
}

void aos_sys_tick_handler(void)
{
    extern void xPortSysTickHandler(void);
    xPortSysTickHandler();
}

int aos_is_sched_disable(void)
{
    BaseType_t state = xTaskGetSchedulerState();
    if (state != taskSCHEDULER_RUNNING)
        return 1;
    return 0u;
}

int aos_is_irq_disable(void)
{
    extern unsigned long cpu_is_irq_enable();
    return !cpu_is_irq_enable();
}

void aos_freep(char **ptr)
{
    if (ptr && (*ptr)) {
        aos_free(*ptr);
        *ptr = NULL;
    }
}

int g_fr_next_sleep_ticks = -1;
int32_t aos_kernel_next_sleep_ticks_get(void)
{
    return g_fr_next_sleep_ticks;
}

void aos_kernel_ticks_announce(int32_t ticks)
{
    g_fr_next_sleep_ticks = -1;
    vTaskStepTick(ticks);
    vPortYield();
}

aos_status_t aos_task_sched_policy_set(aos_task_t *task, uint8_t policy, uint8_t pri)
{
    // TODO: Not Support
    // freertos 的任务调度策略是对于整个系统而言的,
    // 通过 FreeRTOSConfig.h 中宏进行配置
    // configUSE_PREEMPTION（是否支持抢占）
    // configUSE_TIME_SLICING（是否支持时间片的轮转）
    // configIDLE_SHOULD_YIELD（空闲任务是否会让步）
    return ENOSYS;
}

aos_status_t aos_task_sched_policy_get(aos_task_t *task, uint8_t *policy)
{
    CHECK_HANDLE(task);
    if (policy == NULL) {
        return -EINVAL;
    }
#if (configUSE_PREEMPTION == 1)
#if (configUSE_TIME_SLICING == 1)
    *policy = AOS_KSCHED_RR;
#else
    *policy = AOS_KSCHED_FIFO;
#endif /*(configUSE_TIME_SLICING == 1)*/
#else
    *policy = AOS_KSCHED_OTHER;
#endif
    return 0;
}

uint32_t aos_task_sched_policy_get_default(void)
{
#if (configUSE_PREEMPTION == 1)
#if (configUSE_TIME_SLICING == 1)
    return AOS_KSCHED_RR;
#else
    return AOS_KSCHED_FIFO;
#endif /*(configUSE_TIME_SLICING == 1)*/
#else
    return AOS_KSCHED_OTHER;
#endif
}

aos_status_t aos_task_time_slice_set(aos_task_t *task, uint32_t slice)
{
    // TODO: freertos 的时间片就是一个 tick 的时间，是根据硬件一个 tick 的时间对宏 configTICK_RATE_HZ 配置实现的
    return ENOSYS;
}

aos_status_t aos_task_time_slice_get(aos_task_t *task, uint32_t *slice)
{
    CHECK_HANDLE(task);
    if (!slice)
        return -EINVAL;
    *slice = 1000 / configTICK_RATE_HZ;
    return 0;
}

uint32_t aos_sched_get_priority_max(uint32_t policy)
{
    return configMAX_PRIORITIES - 1;
}

int aos_workqueue_create_ext(aos_workqueue_t *workqueue, const char *name, int pri, int stack_size)
{
    return ENOSYS;
}
