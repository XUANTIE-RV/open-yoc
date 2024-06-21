/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include <rtthread.h>
#include <rthw.h>
#include <rtt_ipc/workqueue.h>
#include <aos/aos.h>
#include <drv/irq.h>
#include <aos/wdt.h>

#define AOS_MIN_STACK_SIZE  64

#define GetTCBFromHandle(pxHandle) (((pxHandle) == NULL) ? (rt_thread_self()) : (pxHandle))

#define CHECK_HANDLE(handle)                             \
    do                                                   \
    {                                                    \
        if (handle == NULL || (void *)(*handle) == NULL) \
        {                                                \
            return -EINVAL;                              \
        }                                                \
    } while (0)

static unsigned int used_bitmap;
static long long start_time_ms = 0;

extern volatile rt_uint8_t rt_interrupt_nest;

static volatile rt_uint8_t mutex_index = 0;
static volatile rt_uint8_t event_index = 0;
static volatile rt_uint8_t sem_index = 0;
static volatile rt_uint8_t mq_index = 0;
static volatile rt_uint8_t timer_index = 0;
static volatile rt_uint8_t work_index = 0;
typedef struct IpcDefinition
{
    struct rt_ipc_object *rt_ipc;
} xIPC;
typedef xIPC Ipc_t;

extern int rtthread_startup(void);
int _aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg,
                      void *stack_start, int stack_size, int prio, uint32_t options, int cpuid);

static uint64_t ticks_to_ms(uint64_t ticks)
{
    uint32_t padding;
    uint64_t time;

    padding = RT_TICK_PER_SECOND / 1000;
    padding = (padding > 0) ? (padding - 1) : 0;

    time = ((ticks + padding) * 1000) / RT_TICK_PER_SECOND;

    return time;
}

static uint64_t ms_to_ticks(uint64_t ms)
{
    uint16_t padding;
    uint64_t ticks;

    padding = 1000 / RT_TICK_PER_SECOND;
    padding = (padding > 0) ? (padding - 1) : 0;

    ticks = ((ms + padding) * RT_TICK_PER_SECOND) / 1000;

    return ticks;
}

static inline bool is_in_intrp(void)
{
    return rt_interrupt_get_nest() > 0;
}

/* imp in watchdog.c */
int aos_get_hz(void)
{
    return RT_TICK_PER_SECOND;
}

const char *aos_version_get(void)
{
    return aos_get_os_version();
}

const char *aos_kernel_version_get(void)
{
    static char ver_buf[24] = {0};
    if (ver_buf[0] == 0)
    {
        snprintf(ver_buf, sizeof(ver_buf), "RT-Thread: %d", RTTHREAD_VERSION);
    }
    return ver_buf;
}

int aos_kernel_status_get(void)
{
    if (rt_thread_self() == RT_NULL)
        return AOS_SCHEDULER_NOT_STARTED;
    if (rt_critical_level() == 0)
        return AOS_SCHEDULER_RUNNING;
    else
        return AOS_SCHEDULER_SUSPENDED;
}

aos_status_t aos_task_create(aos_task_t *task, const char *name, void (*fn)(void *),
                             void *arg, void *stack, size_t stack_size, int32_t prio, uint32_t options)
{
    int ret;

    if (task == NULL)
    {
        return -EINVAL;
    }

    ret = _aos_task_new_ext(task, name, fn, arg, stack, stack_size, prio, options, -1);
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}

aos_status_t aos_task_create_ext(aos_task_t *task, const char *name, void (*fn)(void *),
                     void *arg,void *stack_buf, size_t stack_size, int32_t prio, uint32_t options, uint32_t cpuid)
{
    int ret;

    if (task == NULL)
    {
        return -EINVAL;
    }

    ret = _aos_task_new_ext(task, name, fn, arg, stack_buf, stack_size, prio, options, cpuid);
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}

aos_status_t aos_task_suspend(aos_task_t *task)
{
    CHECK_HANDLE(task);

    rt_thread_t ktask;

    ktask = (rt_thread_t) * task;
    rt_thread_t thread = (rt_thread_t)GetTCBFromHandle(ktask);
    if (rt_thread_suspend(thread) == RT_EOK)
    {
        rt_schedule();
    }

    return 0;
}

aos_status_t aos_task_resume(aos_task_t *task)
{
    CHECK_HANDLE(task);

    rt_thread_t thread = (rt_thread_t) * task;
    rt_bool_t need_schedule = RT_FALSE;
    rt_base_t level;

    if (thread != NULL && thread != rt_thread_self())
    {
        level = rt_hw_interrupt_disable();
        /* A task with higher priority than the current running task is ready */
        if (rt_thread_resume(thread) == RT_EOK && RT_SCHED_PRIV(thread).current_priority <= RT_SCHED_PRIV(rt_thread_self()).current_priority)
        {
            need_schedule = RT_TRUE;
        }
        rt_hw_interrupt_enable(level);
    }
    if (need_schedule == RT_TRUE)
    {
        rt_schedule();
    }
    return 0;
}

aos_status_t aos_task_delete(aos_task_t *task)
{
    rt_thread_t thread;

    thread = (rt_thread_t)*task;
    thread = (rt_thread_t)GetTCBFromHandle(thread);

    if (rt_object_is_systemobject((rt_object_t)thread))
    {
        rt_thread_detach(thread);
        aos_free(thread);
    }
    else
    {
        rt_thread_delete(thread);
    }

    if (thread == rt_thread_self())
    {
        rt_schedule();
    }

    return 0;
}

int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 int stack_size)
{
    aos_task_t task;

    aos_check_return_einval(name && fn && (stack_size >= AOS_MIN_STACK_SIZE));

    return aos_task_new_ext(&task, name, fn, arg, stack_size, AOS_DEFAULT_APP_PRI);
}

int _aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg,
                      void *stack_start, int stack_size, int prio, uint32_t options, int cpuid)
{
    rt_err_t ret;
    rt_thread_t tid;

    aos_check_return_einval(task && fn && (stack_size > AOS_MIN_STACK_SIZE) &&
                            (prio >= 0 && prio < RT_THREAD_PRIORITY_MAX));

    if (name == NULL)
    {
        return -EFAULT;
    }

    if (stack_start) {
        tid = (struct rt_thread *)aos_zalloc(sizeof(struct rt_thread));
        if (tid == NULL)
            goto failure;
        ret = rt_thread_init(tid, name, fn, arg, stack_start, stack_size, prio, 10u);
        if (ret != 0) {
            aos_free(tid);
            goto failure;
        }
    } else {
#if defined(CSK_CPU_STACK_EXTRAL)
        stack_size += CSK_CPU_STACK_EXTRAL;
#endif
        tid = rt_thread_create(name, fn, arg, stack_size, prio, 10u);
    }

    if (tid != RT_NULL)
    {
        *task = tid;
#if defined(CONFIG_SMP) && CONFIG_SMP
        if (cpuid != -1) {
            rt_thread_control(tid, RT_THREAD_CTRL_BIND_CPU, (void *)(unsigned long)cpuid);
        }
#endif
        if (options == AOS_TASK_NONE)
        {
            RT_SCHED_PRIV(tid).number = RT_SCHED_PRIV(tid).current_priority >> 3;            /* 5bit */
            RT_SCHED_PRIV(tid).number_mask = 1L << RT_SCHED_PRIV(tid).number;
            RT_SCHED_PRIV(tid).high_mask   = 1L << (RT_SCHED_PRIV(tid).current_priority & 0x07);  /* 3bit */
            /* change thread stat */
            RT_SCHED_CTX(tid).stat = RT_THREAD_SUSPEND;
        }
        else if (options == AOS_TASK_AUTORUN)
        {
            rt_thread_startup(tid);
        }
        return 0;
    }

failure:
    *task = 0;
    return -EPERM;
}

int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio)
{
    return _aos_task_new_ext(task, name, fn, arg, NULL, stack_size, prio, AOS_TASK_AUTORUN, -1);
}

void aos_task_wdt_attach(void (*will)(void *), void *args)
{
#ifdef CONFIG_SOFTWDT
    aos_wdt_attach((long)rt_thread_self(), will, args);
#else
    (void)will;
    (void)args;
#endif
}

void aos_task_wdt_detach()
{
#ifdef CONFIG_SOFTWDT
    uint32_t index = (long)rt_thread_self();

    aos_wdt_feed(index, 0);
    aos_wdt_detach(index);
#endif
}

#include "rtdef.h"
void aos_task_wdt_feed(int time)
{
#ifdef CONFIG_SOFTWDT
    rt_thread_t task = rt_thread_self();

    if (!aos_wdt_exists((long)task))
        aos_wdt_attach((long)task, NULL, (void *)task->parent.name);

    aos_wdt_feed((long)task, time);
#endif
}

void aos_task_exit(int code)
{
    (void)code;

    rt_thread_t thread = rt_thread_self();
    if (rt_object_is_systemobject((rt_object_t)thread))
    {
        rt_thread_detach(thread);
        aos_free(thread);
    }
    else
    {
        rt_thread_delete(thread);
    }

    if (thread == rt_thread_self())
    {
        rt_schedule();
    }
}

aos_task_t aos_task_self(void)
{
    return rt_thread_self();
}

const char *aos_task_name(void)
{
    return rt_thread_self()->parent.name;
}

const char *aos_task_get_name(aos_task_t *task)
{
    rt_thread_t ktask;
    ktask = (rt_thread_t) * task;

    return ktask->parent.name;
}

aos_task_t aos_task_find(char *name)
{
    return rt_thread_find(name);
}

int aos_task_key_create(aos_task_key_t *key)
{
    aos_check_return_einval(key);

    for (int i = 5 - 1; i >= 0; i--)
    {
        if (!((1 << i) & used_bitmap))
        {
            used_bitmap |= 1 << i;
            *key = i;

            return 0;
        }
    }

    return -EINVAL;
}

uint8_t rt_task_info_set(rt_thread_t task, size_t idx, void *info)
{
    if (idx >= 5)
    {
        return -1;
    }
    rt_base_t level;

    level = rt_hw_interrupt_disable();
    task->user_info[idx] = info;
    rt_hw_interrupt_enable(level);

    return 0;
}

uint8_t rt_task_info_get(rt_thread_t task, size_t idx, void **info)
{
    if (idx >= 5)
    {
        return -1;
    }

    *info = task->user_info[idx];

    return 0;
}

void aos_task_key_delete(aos_task_key_t key)
{
    if (key >= 5)
    {
        return;
    }

    uint32_t bit = 1 << key;

    if ((used_bitmap & bit) == bit)
    {
        used_bitmap &= ~(bit);
        rt_task_info_set(rt_thread_self(), key, NULL);
    }
}

aos_status_t aos_task_setspecific(aos_task_key_t key, void *vp)
{
    uint32_t bit = 1 << key;

    if ((used_bitmap & bit) == bit)
    {
        rt_task_info_set(rt_thread_self(), key, vp);
        return 0;
    }
    else
    {
        return -EINVAL;
    }
}

void *aos_task_getspecific(aos_task_key_t key)
{
    void *vp = NULL;

    uint32_t bit = 1 << key;

    if ((used_bitmap & bit) == bit)
    {
        rt_task_info_get(rt_thread_self(), key, &vp);
    }

    return vp;
}

int aos_mutex_new(aos_mutex_t *mutex)
{
    aos_check_return_einval(mutex);

    char name[RT_NAME_MAX] = {0};
    rt_snprintf(name, RT_NAME_MAX, "mutex%02d", mutex_index++);

    rt_mutex_t mux = rt_mutex_create(name, RT_IPC_FLAG_PRIO);
    *mutex = (rt_mutex_t)mux;
    return mux != NULL ? 0 : -1;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    aos_check_return(mutex && *mutex);

    rt_mutex_delete(*mutex);

    *mutex = NULL;
}

int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout)
{
    CHECK_HANDLE(mutex);
    rt_err_t res = 0;

    if (is_in_intrp())
    {
        return -EPERM;
    }

    if (rt_thread_self() == RT_NULL)
        return 0;

    if (mutex && *mutex)
    {
        if (timeout == AOS_WAIT_FOREVER)
        {
            res = rt_mutex_take(*mutex, RT_WAITING_FOREVER);
        }
        else
        {
            res = rt_mutex_take(*mutex, rt_tick_from_millisecond(timeout));
            if (res == -RT_ETIMEOUT)
                res = -ETIMEDOUT;
        }
    }
    return res;
}

int aos_mutex_unlock(aos_mutex_t *mutex)
{
    CHECK_HANDLE(mutex);

    rt_err_t res = RT_EOK;

    if (is_in_intrp())
    {
        return -EPERM;
    }

    if (rt_thread_self() == RT_NULL)
        return 0;

    res = rt_mutex_release(*mutex);

    return res;
}

int aos_mutex_is_valid(aos_mutex_t *mutex)
{
    return mutex && *mutex != NULL;
}

aos_status_t aos_sem_create(aos_sem_t *sem, uint32_t count, uint32_t options)
{
    aos_check_return_einval(sem);

    char name[RT_NAME_MAX] = {0};
    rt_snprintf(name, RT_NAME_MAX, "sem%02d", sem_index++);

    rt_sem_t s = rt_sem_create(name, count, RT_IPC_FLAG_PRIO);
    *sem = s;
    return s != NULL ? 0 : -1;
}

int aos_sem_new(aos_sem_t *sem, int count)
{
    aos_check_return_einval(sem && (count < 0x10000U || count > 0));

    char name[RT_NAME_MAX] = {0};
    rt_snprintf(name, RT_NAME_MAX, "sem%02d", sem_index++);

    rt_sem_t s = rt_sem_create(name, count, RT_IPC_FLAG_PRIO);
    *sem = s;
    return s != NULL ? 0 : -1;
}

void aos_sem_free(aos_sem_t *sem)
{
    aos_check_return(sem && *sem);

    rt_sem_delete(*sem);

    *sem = NULL;
}

int aos_sem_wait(aos_sem_t *sem, unsigned int timeout)
{
    aos_check_return_einval(sem && *sem);

    if (timeout == 0)
        return (-EBUSY);

    if (is_in_intrp()) {
        /* not called by interrupt */
        return -EPERM;
    }

    rt_err_t ret = RT_EOK;
    rt_uint8_t type;
    Ipc_t *const psem = (Ipc_t *)sem;
    struct rt_ipc_object *pipc;

    pipc = psem->rt_ipc;
    RT_ASSERT(pipc != RT_NULL);
    type = rt_object_get_type(&pipc->parent);

    if (type != RT_Object_Class_Semaphore)
        return -EINVAL;

    if (timeout == AOS_WAIT_FOREVER)
    {
        ret = rt_sem_take((rt_sem_t) * sem, RT_WAITING_FOREVER);
    }
    else
    {
        ret = rt_sem_take((rt_sem_t) * sem, rt_tick_from_millisecond(timeout));
    }

    if (ret == -RT_ETIMEOUT)
        ret = -ETIMEDOUT;

    return ret;
}

void aos_sem_signal(aos_sem_t *sem)
{
    aos_check_return(sem && *sem);

    Ipc_t *const psem = (Ipc_t *)sem;
    struct rt_ipc_object *pipc;
    rt_uint8_t type;
    rt_base_t level;
    rt_err_t err = -RT_ERROR;

    pipc = psem->rt_ipc;
    RT_ASSERT(pipc != RT_NULL);
    type = rt_object_get_type(&pipc->parent);
    if (type == RT_Object_Class_Mutex)
    {
        err = rt_mutex_release((rt_mutex_t) pipc);
        if (err != RT_EOK)
            rt_kprintf("rt mutex release error\r\n");
    }
    else if (type == RT_Object_Class_Semaphore)
    {
        level = rt_hw_interrupt_disable();
        rt_sem_release((rt_sem_t) pipc);
        rt_hw_interrupt_enable(level);
    }
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    aos_check_return(sem && *sem);

    rt_sem_control((rt_sem_t)*sem, RT_IPC_CMD_WAKE_ALL, RT_NULL);
}

int aos_task_sem_new(aos_task_t *task, aos_sem_t *sem, const char *name, int count)
{
    rt_err_t ret;

    CHECK_HANDLE(task);
    aos_check_return_einval(sem);

    rt_thread_t ktask = (rt_thread_t) * task;

    if (name == NULL)
    {
        ret = rt_thread_sem_create(ktask, "AOS", count);
    }
    else
    {
        ret = rt_thread_sem_create(ktask, name, count);
    }

    return ret;
}

int aos_task_sem_free(aos_task_t *task)
{
    rt_err_t ret;

    CHECK_HANDLE(task);

    rt_thread_t ktask = (rt_thread_t) * task;
    ret = rt_thread_sem_del(ktask);
    return ret;
}

void aos_task_sem_signal(aos_task_t *task)
{
    aos_check_return(task && *task);

    rt_thread_t ktask = (rt_thread_t) * task;
    if (ktask->thread_sem_obj)
        rt_thread_sem_give(ktask);
}

int aos_task_sem_wait(unsigned int timeout)
{
    rt_err_t ret;

    if (is_in_intrp()) {
        /* not called by interrupt */
        return -EPERM;
    }

    if (timeout == AOS_WAIT_FOREVER)
    {
        ret = rt_thread_sem_take(RT_WAITING_FOREVER);
    }
    else
    {
        ret = rt_thread_sem_take(ms_to_ticks(timeout));
    }

    return ret;
}

int aos_task_sem_count_set(aos_task_t *task, int count)
{
    rt_err_t ret;

    CHECK_HANDLE(task);

    rt_thread_t ktask = (rt_thread_t) * task;
    ret = rt_thread_sem_count_set(ktask, count);
    return ret;
}

int aos_task_sem_count_get(aos_task_t *task, int *count)
{
    rt_err_t ret;

    CHECK_HANDLE(task);
    aos_check_return_einval(count);

    rt_thread_t ktask = (rt_thread_t) * task;
    ret = rt_thread_sem_count_get(ktask, (rt_uint32_t *)count);
    return ret;
}

int aos_sem_is_valid(aos_sem_t *sem)
{
    return sem && *sem != NULL;
}

int aos_event_new(aos_event_t *event, unsigned int flags)
{
    aos_check_return_einval(event);

    rt_event_t event_handle;
    char name[RT_NAME_MAX] = {0};
    *event = NULL;

    aos_check_return_einval(event);

    rt_snprintf(name, RT_NAME_MAX, "event%02d", event_index++);
    event_handle = rt_event_create(name, RT_IPC_FLAG_PRIO);
    event_handle->set = flags;

    if (event_handle != NULL)
    {
        rt_event_send(event_handle, (rt_uint32_t)flags);
    }
    else
    {
        return -1;
    }

    *event = event_handle;

    return 0;
}

void aos_event_free(aos_event_t *event)
{
    aos_check_return(event && *event);

    rt_event_delete(*event);

    *event = NULL;
}

int aos_event_get(
    aos_event_t *event,
    unsigned int flags,
    unsigned char opt,
    unsigned int *actl_flags,
    unsigned int timeout)
{
    rt_err_t ret = 0;
    uint8_t option = 0;
    rt_uint32_t recved = 0;

    aos_check_return_einval(event && *event);

    if (is_in_intrp()) {
        /* not called by interrupt */
        return -EPERM;
    }

    switch (opt)
    {
    case AOS_EVENT_AND:
        option |= RT_EVENT_FLAG_AND;
        break;
    case AOS_EVENT_AND_CLEAR:
        option |= RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR;
        break;
    case AOS_EVENT_OR:
        option |= RT_EVENT_FLAG_OR;
        break;
    case AOS_EVENT_OR_CLEAR:
        option |= RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR;
        break;
    default:
        break;
    }

    if (timeout == AOS_WAIT_FOREVER)
    {
        ret = rt_event_recv(*event,
                                  flags,
                                  option,
                                  RT_WAITING_FOREVER,
                                  &recved);
    }
    else
    {
        ret = rt_event_recv(*event,
                                  flags,
                                  option,
                                  rt_tick_from_millisecond(timeout),
                                  &recved);
    }

    if (ret != RT_EOK) {
        return -1;
    }

    if (actl_flags) {
        rt_event_t ptr = *event;
        *actl_flags = ptr->set | recved;
    }

    return 0;
}

/* rtthread opt nu*/
int aos_event_set(aos_event_t *event, unsigned int flags, unsigned char opt)
{
    aos_check_return_einval(event && *event);

    if ((opt != AOS_EVENT_AND) && (opt != AOS_EVENT_OR))
    {
        return -EPERM;
    }

    if (opt & AOS_EVENT_AND) {
        rt_base_t level;
        level = rt_hw_interrupt_disable();
        rt_event_t ptr = *event;
        ptr->set &= (rt_uint32_t)flags;
        rt_hw_interrupt_enable(level);
        return 0;
    }

    rt_event_send(*event, (rt_uint32_t)flags);

    return 0;
}

int aos_event_is_valid(aos_event_t *event)
{
    return event && *event != NULL;
}

aos_status_t aos_queue_create(aos_queue_t *queue, size_t size, size_t max_msg, uint32_t options)
{
    aos_check_return_einval(queue && (size > 0) && (max_msg > 0));

    rt_mq_t mq;
    char name[RT_NAME_MAX] = {0};
    rt_snprintf(name, RT_NAME_MAX, "mq%02d", mq_index++);
    mq = rt_mq_create(name, max_msg, size / max_msg, RT_IPC_FLAG_PRIO);
    if (mq == NULL)
    {
        return -1;
    }

    *queue = mq;
    return 0;
}

int aos_queue_new(aos_queue_t *queue, void *buf, size_t size, int max_msg)
{
    aos_check_return_einval(queue && buf && (size > 0) && (max_msg > 0) && (size <= RT_MQ_ENTRY_MAX));

    rt_mq_t mq;
    size_t malloc_len;
    rt_err_t result;
    (void)(buf);

    malloc_len = sizeof(struct rt_messagequeue) + (buf == NULL ? size : 0);
    mq = (rt_mq_t)aos_malloc(malloc_len);
    if (mq == NULL)
    {
        return -ENOMEM;
    }

    /* create queue object */
    char name[RT_NAME_MAX] = {0};
    rt_snprintf(name, RT_NAME_MAX, "mq%02d", mq_index++);
    result = rt_mq_init(mq, name, buf, max_msg, size, RT_IPC_FLAG_PRIO);
    if (result != RT_EOK)
    {
        return -1;
    }
    *queue = mq;

    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    aos_check_return(queue && *queue);

    rt_mq_t mq = (rt_mq_t) * queue;
    if (rt_object_is_systemobject((rt_object_t)mq))
    {
        rt_mq_detach(mq);
    }
    else
    {
        rt_mq_delete(mq);
    }

    *queue = NULL;
}

int aos_queue_send(aos_queue_t *queue, void *msg, size_t size)
{
    int res = 0;

    aos_check_return_einval(size > 0);
    CHECK_HANDLE(queue);

    if (msg == NULL)
    {
        return -EFAULT;
    }

    rt_mq_t mq = (rt_mq_t) * queue;
    if (rt_object_get_type(&mq->parent.parent) != RT_Object_Class_MessageQueue)
    {
        return -EINVAL;
    }

    res = rt_mq_send(mq, msg, size);
    if (res != RT_EOK)
        return -EINVAL;

    return 0;
}

int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg, size_t *size)
{
    CHECK_HANDLE(queue);
    /* verify param */
    if (size == NULL || msg == NULL)
    {
        return -EFAULT;
    }

    rt_err_t ret;
    rt_mq_t mq = (rt_mq_t) * queue;
    *size = mq->msg_size;

    if (rt_object_get_type(&mq->parent.parent) != RT_Object_Class_MessageQueue)
    {
        return -EINVAL;
    }
    if (ms == AOS_WAIT_FOREVER)
    {
        ret = rt_mq_recv(mq, msg, *size, RT_WAITING_FOREVER);
    }
    else
    {
        ret = rt_mq_recv(mq, msg, *size, rt_tick_from_millisecond(ms));
    }
    if (ret <= 0)
    {
        *size = 0;
        return -1;
    }
    return 0;
}

int aos_queue_is_valid(aos_queue_t *queue)
{
    return queue && *queue != NULL;
}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    if (!aos_queue_is_valid(queue))
    {
        return NULL;
    }

    rt_mq_t mq = (rt_mq_t) * queue;
    return mq->msg_pool;
}

int aos_queue_get_count(aos_queue_t *queue)
{
    CHECK_HANDLE(queue);

    rt_ubase_t uxReturn = 0;
    struct rt_ipc_object *pipc;
    rt_uint8_t type;
    rt_base_t level;
    rt_mq_t mq = (rt_mq_t) * queue;

    pipc = (struct rt_ipc_object *)mq;
    RT_ASSERT(pipc != RT_NULL);
    type = rt_object_get_type(&pipc->parent);

    level = rt_hw_interrupt_disable();

    if (type == RT_Object_Class_Mutex)
    {
        if (((rt_mutex_t)pipc)->owner == RT_NULL)
        {
            uxReturn = 1;
        }
        else
        {
            uxReturn = 0;
        }
    }
    else if (type == RT_Object_Class_Semaphore)
    {
        uxReturn = ((rt_sem_t)pipc)->value;
    }
    else if (type == RT_Object_Class_MessageQueue)
    {
        uxReturn = ((rt_mq_t)pipc)->entry;
    }

    rt_hw_interrupt_enable(level);

    return uxReturn;
}

typedef struct tmr_adapter
{
    rt_timer_t timer;
    void (*func)(void *, void *);
    void *func_arg;
    uint8_t bIsRepeat;
    uint16_t valid_flag;
    uint64_t init_ms;
    uint64_t round_ms;
} tmr_adapter_t;

static void tmr_adapt_cb(void *adapter)
{
    if (((tmr_adapter_t *)adapter)->func)
        ((tmr_adapter_t *)adapter)->func(((tmr_adapter_t *)adapter)->timer, ((tmr_adapter_t *)adapter)->func_arg);
    return;
}

int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat)
{
    return aos_timer_new_ext(timer, fn, arg, ms, repeat, 1);
}

int aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *),
                      void *arg, int ms, int repeat, unsigned char auto_run)
{
    /* verify param */
    aos_check_return_einval(timer && fn && ms);

    rt_uint8_t flag;
    rt_timer_t rt_timer;

    if (repeat)
        flag = RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER;
    else
        flag = RT_TIMER_FLAG_ONE_SHOT | RT_TIMER_FLAG_SOFT_TIMER;

    /* create timer wrap object ,then initlize timer object */
    tmr_adapter_t *tmr_adapter = aos_malloc(sizeof(tmr_adapter_t));
    if (tmr_adapter == NULL)
    {
        return -RT_ERROR;
    }

    tmr_adapter->func = fn;
    tmr_adapter->func_arg = arg;
    tmr_adapter->bIsRepeat = repeat;
    tmr_adapter->init_ms = ms;
    tmr_adapter->round_ms = (repeat == true) ? ms : 0;

    /* create queue object */
    char name[RT_NAME_MAX] = {0};
    rt_snprintf(name, RT_NAME_MAX, "timer%02d", timer_index++);

    rt_tick_t tick = aos_kernel_ms2tick(ms);
    rt_timer = rt_timer_create(name, tmr_adapt_cb, tmr_adapter, tick, flag);

    if (timer == NULL)
    {
        aos_free(tmr_adapter);
        return -RT_ERROR;
    }

    tmr_adapter->valid_flag = 0xA598;
    tmr_adapter->timer = rt_timer;
    *timer = tmr_adapter;

    if (auto_run)
    {
        if (aos_timer_start(timer) != 0)
        {
            return -1;
        }
    }

    return 0;
}

void aos_timer_free(aos_timer_t *timer)
{
    aos_check_return(timer && *timer);

    tmr_adapter_t *tmr_adapter = *timer;
    rt_timer_delete(tmr_adapter->timer);

    tmr_adapter->valid_flag = 0;
    aos_free(tmr_adapter);
    *timer = NULL;
}

int aos_timer_start(aos_timer_t *timer)
{
    aos_check_return_einval(timer && *timer);

    int ret;
    /* start timer  */
    tmr_adapter_t *tmr_adapter = *timer;

    ret = rt_timer_start(tmr_adapter->timer);
    if (ret != RT_EOK)
        return -EINVAL;

    return 0;
}

int aos_timer_stop(aos_timer_t *timer)
{
    aos_check_return_einval(timer && *timer);

    int ret;
    /* stop timer */
    tmr_adapter_t *tmr_adapter = *timer;

    ret = rt_timer_stop(tmr_adapter->timer);
    if (ret != RT_EOK)
        return -EINVAL;

    return 0;
}

int aos_timer_change(aos_timer_t *timer, int ms)
{
    aos_check_return_einval(timer && *timer);

    tmr_adapter_t *tmr_adapter = *timer;
    rt_tick_t tick = aos_kernel_ms2tick(ms);

    if (rt_timer_control(tmr_adapter->timer, RT_TIMER_CTRL_SET_TIME, &tick) != RT_EOK)
        return -RT_ERROR;

    return 0;
}

int aos_timer_change_once(aos_timer_t *timer, int ms)
{
    aos_check_return_einval(timer && *timer);

    tmr_adapter_t *tmr_adapter = *timer;
    rt_tick_t tick = aos_kernel_ms2tick(ms);

    if (rt_timer_control(tmr_adapter->timer, RT_TIMER_CTRL_SET_TIME, &tick) != RT_EOK)
        return -RT_ERROR;

    return 0;
}

int aos_timer_is_valid(aos_timer_t *timer)
{
    if (timer == NULL || *timer == NULL)
    {
        return 0;
    }
    tmr_adapter_t *tmr_adapter = *timer;
    if (tmr_adapter->valid_flag != 0xA598)
        return 0;
    return 1;
}

int aos_timer_is_active(aos_timer_t *timer)
{
    if ((timer == NULL) || (*timer == NULL))
    {
        return 0;
    }

    rt_uint32_t arg;
    tmr_adapter_t *tmr_adapter = *timer;

    rt_timer_control(tmr_adapter->timer, RT_TIMER_CTRL_GET_STATE, &arg);
    if (arg == RT_TIMER_FLAG_ACTIVATED)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int aos_timer_gettime(aos_timer_t *timer, uint64_t value[4])
{
    tmr_adapter_t *pTimer = NULL;

    uint64_t init_ms;
    uint64_t round_ms;
    CHECK_HANDLE(timer);

    pTimer = (tmr_adapter_t *)*timer;
    init_ms = pTimer->init_ms;
    round_ms = pTimer->round_ms;

    value[0] = round_ms / 1000;
    value[1] = (round_ms % 1000) * 1000000UL;
    value[2] = init_ms / 1000;
    value[3] = (init_ms % 1000) * 1000000UL;

    return 0;
}

typedef struct workqueue_adapter
{
    struct rt_workqueue *wq;
} workqueue_adapter_t;
typedef struct work_adapter
{
    struct rt_work work;
    void (*func)(void *);
    void *func_arg;
    int timeout;
} work_adapter_t;

int aos_workqueue_create_ext(aos_workqueue_t *workqueue, const char *name, int pri, int stack_size)
{
    if (name == NULL)
        return -EINVAL;
    return aos_workqueue_create(workqueue, pri, stack_size);
}

int aos_workqueue_create(aos_workqueue_t *workqueue, int pri, int stack_size)
{
    aos_check_return_einval(workqueue && (stack_size >= AOS_MIN_STACK_SIZE) && (stack_size < INT32_MAX) && stack_size);

    if (pri >= RT_THREAD_PRIORITY_MAX)
        return -EINVAL;

    char name[RT_NAME_MAX] = {0};
    rt_snprintf(name, RT_NAME_MAX, "work%02d", work_index++);

    workqueue_adapter_t *wq_adapter = aos_malloc(sizeof(workqueue_adapter_t));

    wq_adapter->wq = rt_workqueue_create(name, stack_size, pri);
    if (wq_adapter->wq == RT_NULL)
    {
        workqueue->hdl = NULL;
        workqueue->stk = NULL;
        return -1;
    }
    workqueue->stk = (uint32_t *)((char *)workqueue->hdl + sizeof(struct rt_workqueue));
    workqueue->hdl = wq_adapter->wq;

    return 0;
}

void aos_workqueue_del(aos_workqueue_t *workqueue)
{
    aos_check_return(workqueue && workqueue->hdl && workqueue->stk);

    workqueue_adapter_t *wq_adapter = workqueue->hdl;

    rt_workqueue_destroy(wq_adapter->wq);
}

static void work_adapt_cb(struct rt_work *work, void *work_data)
{
    work_adapter_t *data = work_data;

    if (data->func)
        data->func(&data->work);
    return;
}

int aos_work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly)
{
    aos_check_return_einval(work);

    if (fn == NULL)
        return -EFAULT;

    work_adapter_t *work_adapter = aos_malloc(sizeof(work_adapter_t));

    rt_work_init(&work_adapter->work, work_adapt_cb, work_adapter);

    work_adapter->func_arg = arg;
    work_adapter->timeout = dly;
    work_adapter->func = fn;

    *work = work_adapter;

    return 0;
}

int aos_work_run(aos_workqueue_t *workqueue, aos_work_t *work)
{
    int ret;

    CHECK_HANDLE(work);

    if (workqueue == NULL)
    {
        return -EINVAL;
    }

    work_adapter_t *worker = *work;
    workqueue_adapter_t *wq_adapter = workqueue->hdl;
    rt_tick_t ticks = ms_to_ticks(worker->timeout);

    ret = rt_workqueue_submit_work(wq_adapter->wq, &worker->work, ticks);
    if (ret != RT_EOK)
    {
        return -EINVAL;
    }

    return 0;
}

void aos_work_destroy(aos_work_t *work)
{
    aos_check_return(work && *work);

    work_adapter_t *data = *work;

    rt_base_t level;

    level = rt_hw_interrupt_disable();
    rt_list_remove(&(data->work.list));
    data->work.flags &= ~RT_WORK_STATE_PENDING;
    /* Timer started */
    if (data->work.flags & RT_WORK_STATE_SUBMITTING)
    {
        rt_timer_stop(&(data->work.timer));
        rt_timer_detach(&(data->work.timer));
        data->work.flags &= ~RT_WORK_STATE_SUBMITTING;
    }
    data->work.workqueue = RT_NULL;
    rt_hw_interrupt_enable(level);

    aos_free(data);
    *work = NULL;
}

int aos_work_cancel(aos_work_t *work)
{
    int ret;

    CHECK_HANDLE(work);

    work_adapter_t *data = *work;

    ret = rt_work_cancel(&data->work);

    if (ret != RT_EOK)
    {
        return -EBUSY;
    }

    return 0;
}

int aos_work_sched(aos_work_t *work)
{
    int ret;

    CHECK_HANDLE(work);

    work_adapter_t *data = *work;

    rt_tick_t ticks = ms_to_ticks(data->timeout);
    ret = rt_work_submit(&data->work, ticks);

    if (ret != RT_EOK)
    {
        return -EBUSY;
    }

    return 0;
}

long long aos_now(void)
{
    return aos_now_ms() * 1000 * 1000;
}

long long aos_now_ms(void)
{
    return rt_tick_get_millisecond();
}

long long aos_sys_tick_get(void)
{
    return rt_tick_get();
}

void aos_msleep(int ms)
{
    rt_thread_mdelay(ms);
}

static void trap_c_cb()
{
    aos_except_process(EPERM, NULL, 0, NULL, NULL);
}

extern void (*trap_c_callback)();
extern void rt_hw_board_init(void);

int rtthread_startup(void)
{
#ifdef RT_USING_SMP
    rt_hw_spin_lock_init(&_cpus_lock);
#endif
    rt_hw_local_irq_disable();

    /* board level initialization
     * NOTE: please initialize heap inside board initialization.
     */
    rt_hw_board_init();

    /* show RT-Thread version */
    rt_show_version();

    /* timer system initialization */
    rt_system_timer_init();

    /* scheduler system initialization */
    rt_system_scheduler_init();

#ifdef RT_USING_SIGNALS
    /* signal system initialization */
    rt_system_signal_init();
#endif /* RT_USING_SIGNALS */

    /* timer thread initialization */
    rt_system_timer_thread_init();

    /* idle thread initialization */
    rt_thread_idle_init();

#ifdef RT_USING_SMP
    rt_hw_spin_lock(&_cpus_lock);
#endif /* RT_USING_SMP */
    return 0;
}

void aos_init(void)
{
    rtthread_startup();
    // intercept trap_c
#if defined(CONFIG_KERNEL_RTTHREAD) && CONFIG_KERNEL_RTTHREAD
    /* RT-Thread components initialization */
    extern void rt_components_init(void);
    rt_components_init();
#endif /* CONFIG_KERNEL_RTTHREAD */

    trap_c_callback = trap_c_cb;
}

void aos_start(void)
{
    /* start scheduler */
    rt_system_scheduler_start();
}

int aos_kernel_intrpt_enter(void)
{
    rt_interrupt_enter();
    return 0;
}

int aos_kernel_intrpt_exit(void)
{
    rt_interrupt_leave();
    return 0;
}

void aos_sys_tick_handler(void)
{
    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
}

/* YoC extend aos API */

int aos_get_mminfo(int32_t *total, int32_t *used, int32_t *mfree, int32_t *peak)
{
    aos_check_return_einval(total && used && mfree && peak);

    rt_size_t rtotal, rused, rmax_used;
    rt_memory_info(&rtotal, &rused, &rmax_used);
    *total = (int32_t)rtotal;
    *used = (int32_t)rused;
    *peak = (int32_t)rmax_used;
    *mfree = *total - *used;

    return 0;
}

int aos_mm_dump(void)
{
#ifdef RT_USING_MEMHEAP_AS_HEAP
    extern void list_memheap(void);
    list_memheap();
#else
    rt_size_t total = 0, used = 0, max_used = 0;

    rt_memory_info(&total, &used, &max_used);
    rt_kprintf("total    : %d\n", total);
    rt_kprintf("used     : %d\n", used);
    rt_kprintf("maximum  : %d\n", max_used);
    rt_kprintf("available: %d\n", total - used);
#endif
    return 0;
}

int aos_task_list(void *task_array, uint32_t array_items)
{
    if (task_array == NULL || array_items == 0)
    {
        return 0;
    }

    uint32_t real_tsk_num = 0;
    return real_tsk_num;
}

uint32_t aos_task_get_stack_space(void *task_handle)
{
    if (task_handle == NULL)
    {
        return 0;
    }

    size_t stack_free;
    stack_free = rt_thread_self()->stack_size;

    return (uint32_t)(4 * stack_free);
}

uint32_t TaskGetNumbers(void)
{
    uint32_t uxReturn = 0;
    rt_base_t level;
    struct rt_object_information *information;
    struct rt_list_node *node = RT_NULL;

    information = rt_object_get_information(RT_Object_Class_Thread);
    RT_ASSERT(information != RT_NULL);

    level = rt_hw_interrupt_disable();

    rt_list_for_each(node, &(information->object_list))
    {
        uxReturn += 1;
    }

    rt_hw_interrupt_enable(level);

    return uxReturn;
}

void aos_task_show_info(void)
{
#ifdef AOS_COMP_CLI
    extern long list_thread(void);
    list_thread();
#endif
}

void dumpsys_task_func(void)
{
#ifdef AOS_COMP_CLI
    extern long list_thread(void);
    list_thread();
#endif
}

/// Suspend the scheduler.
/// \return time in ticks, for how long the system can sleep or power-down.
void aos_kernel_sched_suspend(void)
{
    rt_enter_critical();
}

/// Resume the scheduler.
/// \param[in]     sleep_ticks   time in ticks for how long the system was in sleep or power-down mode.
void aos_kernel_sched_resume(void)
{
    rt_exit_critical();
}

void aos_task_yield(void)
{
    rt_thread_yield();
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

uint64_t aos_kernel_tick2ms(uint64_t ticks)
{
    return (uint64_t)ticks_to_ms(ticks);
}

uint64_t aos_kernel_ms2tick(uint64_t ms)
{
    return ms_to_ticks(ms);
}

int32_t aos_irq_context(void)
{
    return is_in_intrp();
}

void *aos_zalloc(size_t size)
{
    void *tmp = NULL;

    if (size == 0)
    {
        return NULL;
    }

    tmp = rt_malloc(size);
    if (tmp)
    {
        rt_memset(tmp, 0, size);
    }

    return tmp;
}

void *aos_malloc(size_t size)
{
    void *tmp = NULL;

    if (size == 0)
    {
        return NULL;
    }

    tmp = rt_malloc(size);

    return tmp;
}

void *aos_calloc(size_t nitems, size_t size)
{
    void *tmp = NULL;

    tmp = rt_calloc(size, nitems);

    return tmp;
}

void *aos_realloc(void *mem, size_t size)
{
    void *tmp = NULL;

    tmp = rt_realloc(mem, size);
    return tmp;
}

void *aos_zalloc_check(size_t size)
{
    void *ptr = rt_malloc(size);

    aos_check_mem(ptr);
    if (ptr)
    {
        rt_memset(ptr, 0, size);
    }

    return ptr;
}

void *aos_malloc_align(size_t alignment, size_t size)
{
    void *ptr;
    void *align_ptr;
    size_t align_size = sizeof(void *);

    if (alignment > align_size)
    {
        for (;;)
        {
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
    if (ptr != NULL)
    {
        /* the allocated memory block is aligned */
        if (((unsigned long)ptr & (alignment - 1)) == 0)
        {
            align_ptr = (void *)((unsigned long)ptr + alignment);
        }
        else
        {
            align_ptr = (void *)(((unsigned long)ptr + (alignment - 1)) & ~(alignment - 1));
        }

        /* set the pointer before alignment pointer to the real pointer */
        *((unsigned long *)((unsigned long)align_ptr - sizeof(void *))) = (unsigned long)ptr;
        ptr = align_ptr;
    }

    return ptr;
}

void aos_alloc_trace(void *addr, uintptr_t allocator)
{
}

void aos_free_align(void *ptr)
{
    if (ptr)
    {
        void *real_ptr = (void *) * (unsigned long *)((unsigned long)ptr - sizeof(void *));
        aos_free(real_ptr);
    }
}

void aos_free(void *mem)
{
    if (mem == NULL)
    {
        return;
    }

    rt_free(mem);
}

void aos_calendar_time_set(uint64_t now_ms)
{
    start_time_ms = now_ms - rt_tick_get_millisecond();
}

uint64_t aos_calendar_time_get(void)
{
    return rt_tick_get_millisecond() + start_time_ms;
}

uint64_t aos_calendar_localtime_get(void)
{
    if ((aos_calendar_time_get() - 8 * 3600 * 1000) < 0)
    {
        return aos_calendar_time_get();
    }
    return aos_calendar_time_get() + 8 * 3600 * 1000;
}

void *aos_malloc_check(size_t size)
{
    void *p = aos_malloc(size);
    aos_check_mem(p);

    return p;
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

int aos_is_sched_disable(void)
{
    if (rt_critical_level() != 0)
    {
        return 1;
    }
    return 0u;
}

int aos_is_irq_disable(void)
{
    extern unsigned long cpu_is_irq_enable();
    return !cpu_is_irq_enable();
}

void aos_freep(char **ptr)
{
    if (ptr && (*ptr))
    {
        aos_free(*ptr);
        *ptr = NULL;
    }
}

aos_status_t aos_task_ptcb_get(aos_task_t *task, void **ptcb)
{
    CHECK_HANDLE(task);
    if (ptcb == NULL)
    {
        return -EINVAL;
    }

    rt_base_t level;

    level = rt_hw_interrupt_disable();
    *(ptcb) = ((rt_thread_t) * task)->ptcb;
    rt_hw_interrupt_enable(level);

    return 0;
}

aos_status_t aos_task_ptcb_set(aos_task_t *task, void *ptcb)
{
    CHECK_HANDLE(task);

    rt_base_t level;

    level = rt_hw_interrupt_disable();
    ((rt_thread_t)*task)->ptcb = ptcb;
    rt_hw_interrupt_enable(level);

    return 0;
}

aos_status_t aos_task_pri_change(aos_task_t *task, uint8_t pri, uint8_t *old_pri)
{
    CHECK_HANDLE(task);

    rt_base_t level;
    rt_err_t res = 0;
    rt_thread_t ktask = (rt_thread_t) * task;

    if (pri > RT_THREAD_PRIORITY_MAX - 1)
        return -EINVAL;

    if (old_pri == NULL)
        return -EFAULT;
    else
        *old_pri = RT_SCHED_PRIV(ktask).current_priority;

    level = rt_hw_interrupt_disable();
    res = rt_thread_control(ktask, RT_THREAD_CTRL_CHANGE_PRIORITY, &pri);
    rt_hw_interrupt_enable(level);

    return res;
}

aos_status_t aos_task_pri_get(aos_task_t *task, uint8_t *priority)
{
    CHECK_HANDLE(task);
    if (priority == NULL)
    {
        return -EINVAL;
    }

    rt_uint8_t pri;
    rt_thread_t ktask = (rt_thread_t) * task;
    rt_base_t level;

    level = rt_hw_interrupt_disable();
    pri = RT_SCHED_PRIV(ktask).current_priority;
    rt_hw_interrupt_enable(level);

    if (pri > RT_THREAD_PRIORITY_MAX - 1)
        return -1;
    *priority = pri;

    return 0;
}

aos_status_t aos_task_sched_policy_set(aos_task_t *task, uint8_t policy, uint8_t pri)
{
    CHECK_HANDLE(task);

    rt_base_t level;
    rt_err_t res = 0;
    rt_thread_t ktask = (rt_thread_t) * task;

    if (policy != AOS_KSCHED_FIFO && policy != AOS_KSCHED_RR && policy != AOS_KSCHED_CFS)
    {
        return -EINVAL;
    }

    if (pri > RT_THREAD_PRIORITY_MAX - 1)
        return -EINVAL;

    level = rt_hw_interrupt_disable();
    res = rt_thread_control(ktask, RT_THREAD_CTRL_CHANGE_PRIORITY, &pri);
    rt_hw_interrupt_enable(level);

    return res;
}

aos_status_t aos_task_sched_policy_get(aos_task_t *task, uint8_t *policy)
{
    CHECK_HANDLE(task);
    if (policy == NULL)
    {
        return -EINVAL;
    }

    *policy = RT_ETIMEOUT; // rt-thread does not support scheduling policy attributes

    return 0;
}

uint32_t aos_task_sched_policy_get_default()
{
    return AOS_KSCHED_OTHER;
}

aos_status_t aos_task_time_slice_set(aos_task_t *task, uint32_t slice)
{
    return 0;
}

aos_status_t aos_task_time_slice_get(aos_task_t *task, uint32_t *slice)
{
    CHECK_HANDLE(task);
    if (slice == NULL)
    {
        return -EINVAL;
    }
    *slice = 1000 / RT_TICK_PER_SECOND;
    return 0;
}

uint32_t aos_sched_get_priority_max(uint32_t policy)
{
    return RT_THREAD_PRIORITY_MAX;
}

int aos_get_cur_cpu_id(void)
{
    return rt_hw_cpu_id();
}

#if defined(CONFIG_SMP) && CONFIG_SMP
void aos_spin_lock_init(aos_spinlock_t *lock)
{
    if (lock)
        rt_spin_lock_init((struct rt_spinlock *)lock);
}

void aos_spin_lock(aos_spinlock_t *lock)
{
    if (lock)
        rt_spin_lock((struct rt_spinlock *)lock);
}

void aos_spin_unlock(aos_spinlock_t *lock)
{
    if (lock)
        rt_spin_unlock((struct rt_spinlock *)lock);
}

unsigned long aos_spin_lock_irqsave(aos_spinlock_t *lock)
{
    if (lock)
        return rt_spin_lock_irqsave((struct rt_spinlock *)lock);
    return 0;
}

void aos_spin_lock_irqrestore(aos_spinlock_t *lock, unsigned long flags)
{
    if (lock && flags != 0)
        rt_spin_unlock_irqrestore((struct rt_spinlock *)lock, flags);
}

void aos_secondary_cpu_up(void)
{
    rt_hw_secondary_cpu_up();
}

#else

void aos_spin_lock_init(aos_spinlock_t *lock)
{
}

void aos_spin_lock(aos_spinlock_t *lock)
{
    rt_enter_critical();
}

void aos_spin_unlock(aos_spinlock_t *lock)
{
    rt_exit_critical();
}

unsigned long aos_spin_lock_irqsave(aos_spinlock_t *lock)
{
    return rt_hw_interrupt_disable();
}

void aos_spin_lock_irqrestore(aos_spinlock_t *lock, unsigned long flags)
{
    rt_hw_interrupt_enable(flags);
}
#endif /*CONFIG_SMP*/
