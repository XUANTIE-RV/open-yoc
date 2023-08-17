/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/time.h>

#include <k_api.h>
#include <k_config.h>
#include <k_default_config.h>
#include <k_sys.h>
#include <k_time.h>

#include <aos/kernel.h>
#include <aos/list.h>
#include <aos/version.h>
#include <aos/wdt.h>
#include <aos/debug.h>

#define AOS_MIN_STACK_SIZE       64
#define MS2TICK(ms) aos_kernel_ms2tick(ms)
#if (RHINO_CONFIG_KOBJ_DYN_ALLOC == 0)
#warning "RHINO_CONFIG_KOBJ_DYN_ALLOC is disabled!"
#endif

static aos_task_key_t used_bitmap;
static long long start_time_ms = 0;

static int rhino2stderrno(int ret)
{
    switch (ret) {
    case RHINO_SUCCESS:
    case RHINO_MUTEX_OWNER_NESTED:
        return 0;
    case RHINO_SYS_SP_ERR:
    case RHINO_NULL_PTR:
    case RHINO_MM_FREE_ADDR_ERR:
        return -EFAULT;

    case RHINO_INV_PARAM:
    case RHINO_INV_ALIGN:
    case RHINO_KOBJ_TYPE_ERR:
    case RHINO_MM_POOL_SIZE_ERR:
    case RHINO_MM_ALLOC_SIZE_ERR:
    case RHINO_INV_SCHED_WAY:
    case RHINO_TASK_INV_STACK_SIZE:
    case RHINO_BEYOND_MAX_PRI:
    case RHINO_BUF_QUEUE_INV_SIZE:
    case RHINO_BUF_QUEUE_SIZE_ZERO:
    case RHINO_BUF_QUEUE_MSG_SIZE_OVERFLOW:
    case RHINO_QUEUE_FULL:
    case RHINO_QUEUE_NOT_FULL:
    case RHINO_SEM_OVF:
    case RHINO_WORKQUEUE_EXIST:
    case RHINO_WORKQUEUE_NOT_EXIST:
    case RHINO_WORKQUEUE_WORK_EXIST:
        return -EINVAL;

    case RHINO_BLK_TIMEOUT:
        return -ETIMEDOUT ;
    case RHINO_NO_PEND_WAIT:
        return -EBUSY;
    case RHINO_KOBJ_BLK:
        return -EAGAIN;

    case RHINO_NO_MEM:
        return -ENOMEM;

    case RHINO_KOBJ_DEL_ERR:
    case RHINO_SCHED_DISABLE:
    case RHINO_SCHED_ALREADY_ENABLED:
    case RHINO_SCHED_LOCK_COUNT_OVF:
    case RHINO_TASK_NOT_SUSPENDED:
    case RHINO_TASK_DEL_NOT_ALLOWED:
    case RHINO_TASK_SUSPEND_NOT_ALLOWED:
    case RHINO_SUSPENDED_COUNT_OVF:
    case RHINO_PRI_CHG_NOT_ALLOWED:
    case RHINO_NOT_CALLED_BY_INTRPT:
    case RHINO_NO_THIS_EVENT_OPT:
    case RHINO_TIMER_STATE_INV:
    case RHINO_BUF_QUEUE_FULL:
    case RHINO_SEM_TASK_WAITING:
    case RHINO_MUTEX_NOT_RELEASED_BY_OWNER:
    case RHINO_WORKQUEUE_WORK_RUNNING:
        return -EPERM;

    case RHINO_TRY_AGAIN:
    case RHINO_WORKQUEUE_BUSY:
        return -EAGAIN;

    default:
        return -1;
    }

    return -1;
}

#define CHECK_HANDLE(handle)                                 \
    do                                                       \
    {                                                        \
        if(handle == NULL || (void *)(*handle) == NULL)      \
        {                                                    \
            return -EINVAL;                                  \
        }                                                    \
    } while (0)

/* imp in watchdog.c */
//int aos_reboot(void)
int aos_get_hz(void)
{
    return RHINO_CONFIG_TICKS_PER_SECOND;
}

const char *aos_version_get(void)
{
    return aos_get_os_version();
}

const char *aos_kernel_version_get(void)
{
    static char ver_buf[24] = {0};
    if (ver_buf[0] == 0) {
        snprintf(ver_buf, sizeof(ver_buf), "AliOS Things %d", krhino_version_get());
    }
    return ver_buf;
}

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
aos_status_t aos_task_create(aos_task_t *task, const char *name, void (*fn)(void *),
                             void *arg, void *stack, size_t stack_size, int32_t prio, uint32_t options)
{
    int       ret;
    ktask_t   *task_obj;

    if (task == NULL) {
        return -EINVAL;
    }

#if defined(CSK_CPU_STACK_EXTRAL)
    stack_size += CSK_CPU_STACK_EXTRAL;
#endif

    if (stack == NULL) {
        // dynamic
#if (RHINO_CONFIG_SCHED_CFS > 0)
        ret = (int)krhino_cfs_task_dyn_create((ktask_t **)task, name, arg, AOS_DEFAULT_APP_PRI,
                                            stack_size / sizeof(cpu_stack_t), fn, options & AOS_TASK_AUTORUN);
#else
        ret = (int)krhino_task_dyn_create((ktask_t **)task, name, arg, prio,
                                        0, stack_size / sizeof(cpu_stack_t), fn, options & AOS_TASK_AUTORUN);
#endif
        return rhino2stderrno(ret);
    }

    task_obj = aos_malloc(sizeof(ktask_t));
    if (task_obj == NULL) {
        return -ENOMEM;
    }

    *task = task_obj;
#if (RHINO_CONFIG_SCHED_CFS > 0)
    ret = (int)krhino_cfs_task_create(task_obj, name, arg, prio, (cpu_stack_t *)stack,
                                      stack_size / sizeof(cpu_stack_t), fn, options & AOS_TASK_AUTORUN);
#else
    ret = (int)krhino_task_create(task_obj, name, arg, prio, 0, (cpu_stack_t *)stack,
                                  stack_size / sizeof(cpu_stack_t), fn, options & AOS_TASK_AUTORUN);
#endif
    if ((ret != RHINO_SUCCESS) && (ret != RHINO_STOPPED)) {
        aos_free(task_obj);
        *task = NULL;
    }

    return rhino2stderrno(ret);
}

aos_status_t aos_task_suspend(aos_task_t *task)
{
    int ret;
    ktask_t *ktask;

    CHECK_HANDLE(task);
    ktask = (ktask_t *)(*task);
    ret = krhino_task_suspend(ktask);

    return rhino2stderrno(ret);
}

aos_status_t aos_task_resume(aos_task_t *task)
{
    int ret;
    ktask_t *ktask;

    CHECK_HANDLE(task);

    ktask = (ktask_t *)(*task);
    ret = krhino_task_resume(ktask);

    return rhino2stderrno(ret);
}

aos_status_t aos_task_delete(aos_task_t *task)
{
    int ret;
    ktask_t *ktask;

    CHECK_HANDLE(task);

    ktask = (ktask_t *)(*task);

    ret = (int)krhino_task_dyn_del(ktask);

    return rhino2stderrno(ret);
}

int aos_task_new(const char *name, void (*fn)(void *), void *arg,
                 int stack_size)
{
    int ret;

    ktask_t *task_handle;

    aos_check_return_einval(name && fn && (stack_size >= AOS_MIN_STACK_SIZE));

#if defined(CSK_CPU_STACK_EXTRAL)
    stack_size += CSK_CPU_STACK_EXTRAL;
#endif
    ret = (int)krhino_task_dyn_create(&task_handle, name, arg, AOS_DEFAULT_APP_PRI, 0,
                                      stack_size / sizeof(cpu_stack_t), fn, 1u);

    return rhino2stderrno(ret);
}

int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg,
                     int stack_size, int prio)
{
    int ret;

    aos_check_return_einval(task && fn && (stack_size >= AOS_MIN_STACK_SIZE) &&
                            (prio >= 0 && prio < RHINO_CONFIG_PRI_MAX));

#if defined(CSK_CPU_STACK_EXTRAL)
    stack_size += CSK_CPU_STACK_EXTRAL;
#endif
    ret = (int)krhino_task_dyn_create((ktask_t **)(task), name, arg, prio, 0,
                                      stack_size / sizeof(cpu_stack_t), fn, 1u);

    return rhino2stderrno(ret);
}

// ktask_t *debug_task_find(char *name)
// {
// #if (RHINO_CONFIG_KOBJ_LIST > 0)
//     klist_t *listnode;
//     ktask_t *task;

//     for (listnode = g_kobj_list.task_head.next;
//          listnode != &g_kobj_list.task_head; listnode = listnode->next) {
//         task = krhino_list_entry(listnode, ktask_t, task_stats_item);
//         if (0 == strcmp(name, task->task_name)) {
//             return task;
//         }
//     }
// #endif

//     return NULL;
// }

void aos_task_wdt_attach(void (*will)(void *), void *args)
{
#ifdef CONFIG_SOFTWDT
    aos_wdt_attach((long)krhino_cur_task_get(), will, args);
#else
    (void)will;
    (void)args;
#endif
}

void aos_task_wdt_detach()
{
#ifdef CONFIG_SOFTWDT
    long index = (long)krhino_cur_task_get();

    aos_wdt_feed(index, 0);
    aos_wdt_detach(index);
#endif
}

void aos_task_wdt_feed(int time)
{
#ifdef CONFIG_SOFTWDT
    ktask_t *task = krhino_cur_task_get();

    if (!aos_wdt_exists((long)task))
        aos_wdt_attach((long)task, NULL, (void*)task->task_name);

    aos_wdt_feed((long)task, time);
#endif
}

void aos_task_exit(int code)
{
    (void)code;

    krhino_task_dyn_del(NULL);
}

aos_task_t aos_task_self(void)
{
    return (aos_task_t)krhino_cur_task_get();
}

#endif

aos_task_t aos_task_find(char *name)
{
    return (aos_task_t)krhino_task_find(name);
}

const char *aos_task_name(void)
{
    return krhino_cur_task_get()->task_name;
}

const char *aos_task_get_name(aos_task_t *task)
{
    return ((ktask_t*)*task)->task_name;
}

int aos_task_key_create(aos_task_key_t *key)
{
    int i;
    if (key == NULL) 
        return -EINVAL;

    for (i = RHINO_CONFIG_TASK_INFO_NUM - 1; i >= 0; i--) {
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
    if (key >= RHINO_CONFIG_TASK_INFO_NUM) {
        return;
    }

    uint32_t bit = 1 << key;

    if ((used_bitmap & bit) == bit) {
        used_bitmap &= ~(bit);
        krhino_task_info_set(krhino_cur_task_get(), key, NULL);
    }
}

#if (RHINO_CONFIG_TASK_INFO > 0)
int aos_task_setspecific(aos_task_key_t key, void *vp)
{
    uint32_t bit = 1 << key;

    if ((used_bitmap & bit) == bit) {
        int ret = krhino_task_info_set(krhino_cur_task_get(), key, vp);
        return rhino2stderrno(ret);
    } else {
        return -EINVAL;
    }
}

void *aos_task_getspecific(aos_task_key_t key)
{
    void *vp = NULL;
    if (31 < key)
        return NULL;

    uint32_t bit = 1 << key;

    if ((used_bitmap & bit) == bit) {
        krhino_task_info_get(krhino_cur_task_get(), key, &vp);
    }

    return vp;
}
#endif

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
int aos_mutex_new(aos_mutex_t *mutex)
{
    kstat_t ret;
    kmutex_t *m;

    aos_check_return_einval(mutex);

    m = aos_malloc(sizeof(kmutex_t));
    if (m == NULL) {
        return -ENOMEM;
    }

    ret = krhino_mutex_create(m, "AOS");

    if (ret != RHINO_SUCCESS) {
        aos_free(m);
        return rhino2stderrno(ret);
    }
    *mutex = m;

    return 0;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    aos_check_return(mutex && *mutex);

    krhino_mutex_del((kmutex_t *)*mutex);

    aos_free(*mutex);

    *mutex = NULL;
}

int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout)
{
    kstat_t ret;

    CHECK_HANDLE(mutex);

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_mutex_lock((kmutex_t *)*mutex, RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_mutex_lock((kmutex_t *)*mutex, MS2TICK(timeout));
    }

    /* rhino allow nested */
    if (ret == RHINO_MUTEX_OWNER_NESTED) {
        ret = RHINO_SUCCESS;
    }

    return rhino2stderrno(ret);
}

int aos_mutex_unlock(aos_mutex_t *mutex)
{
    kstat_t ret;

    CHECK_HANDLE(mutex);

    ret = krhino_mutex_unlock((kmutex_t *)*mutex);
    /* rhino allow nested */
    if (ret == RHINO_MUTEX_OWNER_NESTED) {
        ret = RHINO_SUCCESS;
    }

    return rhino2stderrno(ret);
}

int aos_mutex_is_valid(aos_mutex_t *mutex)
{
    kmutex_t *k_mutex;

    if (mutex == NULL) {
        return 0;
    }

    k_mutex = (kmutex_t *)*mutex;

    if (k_mutex == NULL) {
        return 0;
    }

    if (k_mutex->blk_obj.obj_type != RHINO_MUTEX_OBJ_TYPE) {
        return 0;
    }

    return 1;
}

#endif

#if ((RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)&&(RHINO_CONFIG_SEM > 0))
aos_status_t aos_sem_create(aos_sem_t *sem, uint32_t count, uint32_t options)
{
    kstat_t ret;
    ksem_t *s;

    (void)options;
    if (sem == NULL) {
        return -EINVAL;
    }

    s = aos_malloc(sizeof(ksem_t));
    if (s == NULL) {
        return -ENOMEM;
    }

    ret = krhino_sem_create(s, "AOS", count);
    if (ret != RHINO_SUCCESS) {
        aos_free(s);
        return rhino2stderrno(ret);
    }

    *sem = s;

    return 0;
}

int aos_sem_new(aos_sem_t *sem, int count)
{
    kstat_t ret;
    ksem_t *s;

    aos_check_return_einval(sem && (count >= 0));

    s = aos_malloc(sizeof(ksem_t));
    if (s == NULL) {
        return -ENOMEM;
    }

    ret = krhino_sem_create(s, "AOS", count);
    if (ret != RHINO_SUCCESS) {
        aos_free(s);
        return rhino2stderrno(ret);
    }

    *sem = s;

    return 0;
}

void aos_sem_free(aos_sem_t *sem)
{
    aos_check_return(sem && *sem);

    krhino_sem_del((ksem_t *)*sem);

    aos_free(*sem);

    *sem = NULL;
}

int aos_sem_wait(aos_sem_t *sem, unsigned int timeout)
{
    kstat_t ret;

    CHECK_HANDLE(sem);

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_sem_take((ksem_t *)*sem, RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_sem_take((ksem_t *)*sem, MS2TICK(timeout));
    }

    return rhino2stderrno(ret);
}

void aos_sem_signal(aos_sem_t *sem)
{
    aos_check_return(sem && *sem);

    krhino_sem_give((ksem_t *)*sem);
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    aos_check_return(sem && *sem);

    krhino_sem_give_all((ksem_t *)*sem);
}

int aos_sem_is_valid(aos_sem_t *sem)
{
    ksem_t *k_sem;

    if (sem == NULL) {
        return 0;
    }

    k_sem = *sem;

    if (k_sem == NULL) {
        return 0;
    }

    if (k_sem->blk_obj.obj_type != RHINO_SEM_OBJ_TYPE) {
        return 0;
    }

    return 1;
}

#endif

#if (RHINO_CONFIG_TASK_SEM > 0)

int aos_task_sem_new(aos_task_t *task, aos_sem_t *sem, const char *name, int count)
{
    kstat_t ret;

    CHECK_HANDLE(task);
    aos_check_return_einval(sem);

    *sem = aos_malloc(sizeof(ksem_t));

    if (*sem == NULL) {
        return -ENOMEM;
    }
    if (name == NULL) {
        ret = krhino_task_sem_create((ktask_t *)(*task), (ksem_t *)*sem, "AOS", count);
    } else {
        ret = krhino_task_sem_create((ktask_t *)(*task), (ksem_t *)*sem, name, count);
    }

    if (ret != RHINO_SUCCESS) {
        aos_free(*sem);
        *sem = NULL;
    }
    return rhino2stderrno(ret);
}

int aos_task_sem_free(aos_task_t *task)
{
    kstat_t ret;
    ktask_t *ktask;

    CHECK_HANDLE(task);

    ktask = (ktask_t *)(*task);
    ret = krhino_task_sem_del(ktask);
    if (ret == RHINO_SUCCESS) {
        aos_free(ktask->task_sem_obj);
    }
    return rhino2stderrno(ret);
}

void aos_task_sem_signal(aos_task_t *task)
{
    aos_check_return(task && *task);

    krhino_task_sem_give((ktask_t *)(*task));
}

int aos_task_sem_wait(unsigned int timeout)
{
    kstat_t ret;

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_task_sem_take(RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_task_sem_take(MS2TICK(timeout));
    }

    return rhino2stderrno(ret);
}

int aos_task_sem_count_set(aos_task_t *task, int count)
{
    kstat_t ret;

    CHECK_HANDLE(task);

    ret = krhino_task_sem_count_set((ktask_t *)(*task), count);
    return rhino2stderrno(ret);
}

int aos_task_sem_count_get(aos_task_t *task, int *count)
{
    kstat_t ret;

    CHECK_HANDLE(task);
    aos_check_return_einval(count);

    ret = krhino_task_sem_count_get((ktask_t *)(*task), (sem_count_t *)count);
    return rhino2stderrno(ret);
}

#endif /* RHINO_CONFIG_TASK_SEM */

#if (RHINO_CONFIG_EVENT_FLAG > 0)

int aos_event_new(aos_event_t *event, unsigned int flags)
{
    int ret;

    aos_check_return_einval(event);

    ret = krhino_event_dyn_create((kevent_t **)(event), "AOS", flags);

    return rhino2stderrno(ret);
}

void aos_event_free(aos_event_t *event)
{
    aos_check_return(event && *event);

    (void)krhino_event_dyn_del((kevent_t *)*event);

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
    kstat_t ret;

    CHECK_HANDLE(event);

    if (timeout == AOS_WAIT_FOREVER) {
        ret = krhino_event_get((kevent_t *)*event, flags, opt, actl_flags, RHINO_WAIT_FOREVER);
    } else {
        ret = krhino_event_get((kevent_t *)*event, flags, opt, actl_flags, MS2TICK(timeout));
    }

    return rhino2stderrno(ret);
}

int aos_event_set(aos_event_t *event, unsigned int flags, unsigned char opt)
{
    kstat_t ret;

    CHECK_HANDLE(event);

    ret = krhino_event_set((kevent_t *)*event, flags, opt);

    return rhino2stderrno(ret);
}

int aos_event_is_valid(aos_event_t *event)
{
    kevent_t *k_event;

    if (event == NULL) {
        return 0;
    }

    k_event = (kevent_t *)*event;

    if (k_event == NULL) {
        return 0;
    }

    if (k_event->blk_obj.obj_type != RHINO_EVENT_OBJ_TYPE) {
        return 0;
    }

    return 1;
}
#endif

#if (RHINO_CONFIG_BUF_QUEUE > 0)

aos_status_t aos_queue_create(aos_queue_t *queue, size_t size, size_t max_msgsize, uint32_t options)
{
    kstat_t      ret;
    kbuf_queue_t *q;
    size_t       malloc_len;
    void        *real_buf;

    (void)options;
    if (queue == NULL || size == 0) {
        return -EINVAL;
    }

    malloc_len = sizeof(kbuf_queue_t) + size;
    q = (kbuf_queue_t *)aos_malloc(malloc_len);
    if (q == NULL) {
        return -ENOMEM;
    }
    real_buf = (uint8_t *)q + sizeof(kbuf_queue_t);

    ret = krhino_buf_queue_create(q, "AOS", real_buf, size, max_msgsize);
    if (ret != RHINO_SUCCESS) {
        aos_free(q);
        return rhino2stderrno(ret);
    }

    *queue = q;

    return 0;
}

int aos_queue_new(aos_queue_t *queue, void *buf, size_t size, int max_msgsize)
{
    kstat_t      ret;
    kbuf_queue_t *q;
    size_t       malloc_len;
    void        *real_buf;

    aos_check_return_einval(queue && buf && (size > 0) && (max_msgsize > 0));

    malloc_len = sizeof(kbuf_queue_t) + (buf == NULL? size : 0);
    q = (kbuf_queue_t *)aos_malloc(malloc_len);
    if (q == NULL) {
        return -ENOMEM;
    }
    real_buf = (buf == NULL) ? ((uint8_t *)q + sizeof(kbuf_queue_t)) : buf;

    ret = krhino_buf_queue_create(q, "AOS", real_buf, size, max_msgsize);
    if (ret != RHINO_SUCCESS) {
        aos_free(q);
        return rhino2stderrno(ret);
    }

    *queue = q;

    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    aos_check_return(queue && *queue);

    krhino_buf_queue_del((kbuf_queue_t *)*queue);

    aos_free(*queue);

    *queue = NULL;
}

int aos_queue_send(aos_queue_t *queue, void *msg, size_t size)
{
    int ret;

    CHECK_HANDLE(queue);

    ret = krhino_buf_queue_send((kbuf_queue_t *)*queue, msg, size);
    return rhino2stderrno(ret);
}

int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg, size_t *size)
{
    int ret;

    CHECK_HANDLE(queue);
    if (ms == AOS_WAIT_FOREVER) {
        ret = krhino_buf_queue_recv((kbuf_queue_t *)*queue, RHINO_WAIT_FOREVER, msg, size);
    } else {
        ret = krhino_buf_queue_recv((kbuf_queue_t *)*queue, MS2TICK(ms), msg, size);
    }
    return rhino2stderrno(ret);
}

int aos_queue_is_valid(aos_queue_t *queue)
{
    kbuf_queue_t *k_queue;

    if (queue == NULL) {
        return 0;
    }

    k_queue = (kbuf_queue_t *)*queue;

    if (k_queue == NULL) {
        return 0;
    }

    if (k_queue->blk_obj.obj_type != RHINO_BUF_QUEUE_OBJ_TYPE) {
        return 0;
    }

    return 1;
}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    if (!aos_queue_is_valid(queue)) {
        return NULL;
    }

    return ((kbuf_queue_t *)*queue)->buf;
}

int aos_queue_get_count(aos_queue_t *queue)
{
    kbuf_queue_info_t info;
    CHECK_HANDLE(queue);
    kstat_t ret = krhino_buf_queue_info_get((kbuf_queue_t *)*queue, &info);

    return ret == RHINO_SUCCESS ? info.cur_num : -EPERM;
}
#endif

#if (RHINO_CONFIG_TIMER > 0)
int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *),
                  void *arg, int ms, int repeat)
{
    kstat_t ret;

    aos_check_return_einval(timer && fn);
    sys_time_t round = (repeat == 0 ? 0 : ms);

    ret = krhino_timer_dyn_create((ktimer_t **)(timer), "AOS", (timer_cb_t)fn, MS2TICK(ms), MS2TICK(round), arg, 1);

    return rhino2stderrno(ret);
}

int aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *),
                      void *arg, int ms, int repeat, unsigned char auto_run)
{
    kstat_t ret;

    aos_check_return_einval(timer && fn);
    sys_time_t round = (repeat == 0 ? 0 : ms);

    ret = krhino_timer_dyn_create((ktimer_t **)(timer), "AOS", (timer_cb_t)fn, MS2TICK(ms), MS2TICK(round), arg, auto_run);

    return rhino2stderrno(ret);
}

void aos_timer_free(aos_timer_t *timer)
{
    aos_check_return(timer && *timer);

    krhino_timer_dyn_del((ktimer_t *)*timer);
    *timer = NULL;
}

int aos_timer_start(aos_timer_t *timer)
{
    int ret;

    CHECK_HANDLE(timer);

    ret = krhino_timer_start((ktimer_t *)*timer);
    return rhino2stderrno(ret);
}

int aos_timer_stop(aos_timer_t *timer)
{
    int ret;

    CHECK_HANDLE(timer);

    ret = krhino_timer_stop((ktimer_t *)*timer);
    return rhino2stderrno(ret);
}

int aos_timer_change(aos_timer_t *timer, int ms)
{
    int ret;

    CHECK_HANDLE(timer);

    ret = krhino_timer_change((ktimer_t *)*timer, MS2TICK(ms), MS2TICK(ms));
    return rhino2stderrno(ret);
}

int aos_timer_change_once(aos_timer_t *timer, int ms)
{
    int ret;

    CHECK_HANDLE(timer);

    ret = krhino_timer_change((ktimer_t *)*timer, MS2TICK(ms), 0);

    return rhino2stderrno(ret);
}

int aos_timer_is_valid(aos_timer_t *timer)
{
    ktimer_t *k_timer;

    if (timer == NULL) {
        return 0;
    }

    k_timer = (ktimer_t *)*timer;

    if (k_timer == NULL) {
        return 0;
    }

    if (k_timer->obj_type != RHINO_TIMER_OBJ_TYPE) {
        return 0;
    }

    return 1;
}

int aos_timer_is_active(aos_timer_t *timer)
{
    ktimer_t *k_timer;

    if (timer == NULL) {
        return 0;
    }

    k_timer = (ktimer_t *)*timer;

    if (k_timer == NULL) {
        return 0;
    }

    k_timer_state_t get = k_timer->timer_state;

    if (get == TIMER_DEACTIVE) {
        return 0;
    }
    return 1;
}

int aos_timer_gettime(aos_timer_t *timer, uint64_t value[4])
{
    ktimer_t *ktimer = NULL;
    uint64_t init_ms;
    uint64_t round_ms;

    CHECK_HANDLE(timer);
    ktimer = (ktimer_t *)*timer;
    init_ms = (uint64_t)krhino_ticks_to_ms(ktimer->init_count);
    round_ms = (uint64_t)krhino_ticks_to_ms(ktimer->round_ticks);

    value[0] = round_ms / 1000;
    value[1] = (round_ms % 1000) * 1000000UL;
    value[2] = init_ms / 1000;
    value[3] = (init_ms % 1000) * 1000000UL;

    return 0;
}
#endif

#if (RHINO_CONFIG_WORKQUEUE  > 0)
int aos_workqueue_create(aos_workqueue_t *workqueue, int pri, int stack_size)
{
    kstat_t ret;

    aos_check_return_einval(workqueue && stack_size >= AOS_MIN_STACK_SIZE);

    workqueue->hdl = (cpu_stack_t *)aos_malloc(sizeof(kworkqueue_t) + stack_size);

    if (workqueue->hdl == NULL) {
        return -ENOMEM;
    }

    workqueue->stk = (cpu_stack_t*)((char *)workqueue->hdl + sizeof(kworkqueue_t));

    ret = krhino_workqueue_create(workqueue->hdl, "AOS", pri, workqueue->stk,
                                  stack_size / sizeof(cpu_stack_t));

    if (ret != RHINO_SUCCESS) {
        aos_free(workqueue->hdl);
        workqueue->hdl = NULL;
        workqueue->stk = NULL;
    }

    return rhino2stderrno(ret);
}

int aos_workqueue_create_ext(aos_workqueue_t *workqueue, const char *name, int pri, int stack_size)
{
    kstat_t ret;

    aos_check_return_einval(workqueue && name);

    workqueue->hdl = (cpu_stack_t *)aos_malloc(sizeof(kworkqueue_t) + stack_size);

    if (workqueue->hdl == NULL) {
        return -ENOMEM;
    }

    workqueue->stk = (cpu_stack_t*)((char *)workqueue->hdl + sizeof(kworkqueue_t));

    ret = krhino_workqueue_create(workqueue->hdl, name, pri, workqueue->stk,
                                  stack_size / sizeof(cpu_stack_t));

    if (ret != RHINO_SUCCESS) {
        aos_free(workqueue->hdl);
        workqueue->hdl = NULL;
        workqueue->stk = NULL;
    }

    return rhino2stderrno(ret);
}

void aos_workqueue_del(aos_workqueue_t *workqueue)
{
    aos_check_return(workqueue && workqueue->hdl && workqueue->stk);

    krhino_workqueue_del(workqueue->hdl);
    aos_free(workqueue->hdl);
}

int aos_work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly)
{
    kstat_t  ret;
    kwork_t *w;

    aos_check_return_einval(work);

    w = aos_malloc(sizeof(kwork_t));

    if (w == NULL) {
        return -ENOMEM;
    }

    ret = krhino_work_init(w, fn, arg, MS2TICK(dly));

    if (ret != RHINO_SUCCESS) {
        aos_free(w);
        return rhino2stderrno(ret);
    }
    *work = w;
    return 0;
}

void aos_work_destroy(aos_work_t *work)
{
    kwork_t *w;

    aos_check_return(work && *work);

    w = *work;

    if (w->timer != NULL) {
        krhino_timer_stop(w->timer);
        krhino_timer_dyn_del(w->timer);
    }

    aos_free(w);
    *work = NULL;
}

int aos_work_run(aos_workqueue_t *workqueue, aos_work_t *work)
{
    int ret;

    if (workqueue == NULL) {
        return -EINVAL;
    }
    CHECK_HANDLE(work);

    ret = krhino_work_run(workqueue->hdl, (kwork_t *)*work);

    return rhino2stderrno(ret);
}

int aos_work_sched(aos_work_t *work)
{
    int ret;

    CHECK_HANDLE(work);

    ret = krhino_work_sched((kwork_t *)*work);

    return rhino2stderrno(ret);
}

int aos_work_cancel(aos_work_t *work)
{
    int ret;

    CHECK_HANDLE(work);

    ret = krhino_work_cancel((kwork_t *)*work);

    if (ret != RHINO_SUCCESS) {
        return -EBUSY;
    }

    return 0;
}
#endif

long long aos_now(void)
{
    return krhino_sys_time_get() * 1000 * 1000;
}

long long aos_now_ms(void)
{
    return krhino_sys_time_get();
}

long long aos_sys_tick_get(void)
{
    return krhino_sys_tick_get();
}

void aos_msleep(int ms)
{
    krhino_task_sleep(MS2TICK(ms));
}

static void trap_c_cb()
{
    aos_except_process(EPERM, NULL, 0, NULL, NULL);
}

extern void (*trap_c_callback)();


void aos_init(void)
{
    krhino_init();

#ifdef CONFIG_KERNEL_PWR_MGMT
    extern void cpu_pwrmgmt_init(void);
    cpu_pwrmgmt_init();
#endif
    // intercept trap_c
    trap_c_callback = trap_c_cb;
}

void aos_start(void)
{
    krhino_start();
}

int aos_kernel_intrpt_enter(void)
{
    kstat_t ret = krhino_intrpt_enter();

    if (ret == RHINO_SUCCESS) {
        return 0;
    } else {
        return -EPERM;
    }

    return 0;
}

int aos_kernel_intrpt_exit(void)
{
    krhino_intrpt_exit();
    return 0;
}

/* YoC extend aos API */

int aos_get_mminfo(int32_t *total, int32_t *used, int32_t *mfree, int32_t *peak)
{
    aos_check_return_einval(total && used && mfree && peak);

    *total = g_kmm_head->used_size + g_kmm_head->free_size;
    *used = g_kmm_head->used_size;
    *mfree = g_kmm_head->free_size;
    *peak = g_kmm_head->maxused_size;

    return 0;
}

int aos_mm_dump(void)
{
#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_MM)
    extern uint32_t dumpsys_mm_info_func(uint32_t mm_status);
    dumpsys_mm_info_func(0);
#endif

    return 0;
}

static int aos_task_list(void *task_array, uint32_t array_items)
{
    if (task_array == NULL || array_items == 0) {
        return 0;
    }

    uint32_t real_tsk_num = 0;
#if (RHINO_CONFIG_SYSTEM_STATS > 0) && (RHINO_CONFIG_KOBJ_LIST > 0)
    klist_t *taskhead;
    klist_t *taskend;
    klist_t *tmp;
    ktask_t  *task;

    void **tk_tmp = task_array;
    taskhead = &g_kobj_list.task_head;
    taskend  = taskhead;

#ifdef CONFIG_BACKTRACE
    uint32_t task_free;
    CPSR_ALLOC();
#endif
#ifdef CONFIG_STACK_GUARD
    int      stack_flags;
#endif

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        real_tsk_num ++;
        task  = krhino_list_entry(tmp, ktask_t, task_stats_item);
#ifdef CONFIG_BACKTRACE
        krhino_task_stack_min_free(task, &task_free);
        printf("\n%s:\n\t    state %d, pri %d, stack: total %d, free %d\n",
               task->task_name, task->task_state, task->prio, 4 * task->stack_size, 4 * task_free);
#endif
    }

    if (array_items < real_tsk_num) {
        real_tsk_num = array_items;
    }

    for (tmp = taskhead->next; tmp != taskend && real_tsk_num >= 1 && (array_items--); tmp = tmp->next) {
        task  = krhino_list_entry(tmp, ktask_t, task_stats_item);
        *tk_tmp = task;
        tk_tmp ++;
    }

#ifdef CONFIG_BACKTRACE
    RHINO_CPU_INTRPT_DISABLE();

#ifdef CONFIG_STACK_GUARD
    extern int stack_guard_save(void);
    stack_flags = stack_guard_save();
#endif

    for (tmp = taskhead->next; tmp != taskend; tmp = tmp->next) {
        task  = krhino_list_entry(tmp, ktask_t, task_stats_item);
        krhino_task_stack_min_free(task, &task_free);

        printf("\n%s:\n", task->task_name);
        extern int csky_task_backtrace(void *stack, char *buf, int len);
        csky_task_backtrace(task->task_stack, NULL, 0);
    }

#ifdef CONFIG_STACK_GUARD
    extern void stack_guard_restore(int value);
    stack_guard_restore(stack_flags);
#endif

    RHINO_CPU_INTRPT_ENABLE();

#endif /* CONFIG_BACKTRACE */
#endif /* RHINO_CONFIG_SYSTEM_STATS */

    return real_tsk_num;
}

static uint32_t aos_task_get_stack_space(void *task_handle)
{
    if (task_handle == NULL) {
        return 0;
    }

    size_t stack_free;
    kstat_t ret = krhino_task_stack_min_free(task_handle, &stack_free);

    if (ret == RHINO_SUCCESS) {
        return (uint32_t)(sizeof(cpu_stack_t) * stack_free);
    } else {
        return 0;
    }
}

extern uint32_t debug_task_cpu_usage_get(ktask_t *task);
void aos_task_show_info(void)
{
    int i, task_num;
    ktask_t **task_array;
#define TASK_MAX_NUM 64

    task_array = aos_zalloc(sizeof(ktask_t*) * TASK_MAX_NUM);
    if (task_array == NULL)
        return;

#if (RHINO_CONFIG_SYS_STATS > 0)
#ifdef AOS_COMP_DEBUG
    extern void debug_task_cpu_usage_stats(void);
    static int show_first = 1;
    debug_task_cpu_usage_stats();
    if (show_first) {
        // FIXME: make cpu-usage more accurate on the first time
        aos_msleep(100);
        debug_task_cpu_usage_stats();
        show_first = 0;
    }
#endif
#endif

#if (RHINO_CONFIG_CPU_USAGE_STATS > 0)
#if (RHINO_CONFIG_SYS_STATS > 0)
    printf("\nCPU USAGE: %d/10000\n", 10000 - debug_task_cpu_usage_get(&g_idle_task[0]));
#else
    printf("\nCPU USAGE: %d/10000\n", krhino_get_cpu_usage());
#endif
#endif

#if (RHINO_CONFIG_SYS_STATS > 0)
    printf("task                 pri status      sp     stack size max used ratio left tick  %%CPU\n");
    printf("-------------------- --- ------- ---------- ---------- -------- ----- ---------  ------\n");
#else
    printf("task                 pri status      sp     stack size max used ratio left tick\n");
    printf("-------------------- --- ------- ---------- ---------- -------- ----- ---------\n");
#endif

    task_num = aos_task_list(task_array, TASK_MAX_NUM);

    for (i = 0; i < task_num; i++) {
        printf("%-20s %3d ", task_array[i]->task_name, task_array[i]->prio);

        if (task_array[i]->task_state == K_RDY)            printf("ready  ");
        else if (task_array[i]->task_state == K_PEND)      printf("pend   ");
        else if (task_array[i]->task_state == K_SUSPENDED) printf("suspend");
        else if (task_array[i]->task_state == K_SLEEP)     printf("sleep  ");
        else if (task_array[i]->task_state == K_SEED)      printf("seed   ");
        else if (task_array[i]->task_state == K_DELETED)   printf("deleted");
        else                                               printf("%-7d", task_array[i]->task_state);

        uint32_t strack_size = task_array[i]->stack_size * sizeof(cpu_stack_t);
        uint32_t min_free = aos_task_get_stack_space(task_array[i]);
#if (RHINO_CONFIG_SYS_STATS > 0)
        printf(" %p   %8d %8d   %2d%% %8"PRId64" %8.1f\n",
               task_array[i]->task_stack_base,
               strack_size,
               strack_size - min_free,
               (strack_size - min_free) * 100 / strack_size,
               task_array[i]->tick_remain,
               debug_task_cpu_usage_get(task_array[i]) / 100.0);
#else
        printf(" %p   %8d %8d   %2d%% %8"PRId64"\n",
               task_array[i]->task_stack_base,
               strack_size,
               strack_size - min_free,
               (strack_size - min_free) * 100 / strack_size,
               task_array[i]->tick_remain);
#endif
    }
    aos_free(task_array);
}

/// Suspend the scheduler.
void aos_kernel_sched_suspend(void)
{
    if (g_sys_stat == RHINO_RUNNING)
        krhino_sched_disable();
}

/// Resume the scheduler.
void aos_kernel_sched_resume()
{
    if (g_sys_stat == RHINO_RUNNING)
        krhino_sched_enable();
}

int aos_kernel_status_get(void)
{
    kstat_t get = g_sys_stat;

    if (get == RHINO_STOPPED) {
        return AOS_SCHEDULER_SUSPENDED;
    } else if (get == RHINO_RUNNING) {
        return AOS_SCHEDULER_RUNNING;
    }
    return AOS_SCHEDULER_NOT_STARTED;
}

void aos_task_yield()
{
    krhino_task_yield();
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
    return (uint64_t)krhino_ticks_to_ms(ticks);
}

uint64_t aos_kernel_ms2tick(uint64_t ms)
{
    return krhino_ms_to_ticks(ms);
}

int32_t aos_kernel_next_sleep_ticks_get(void)
{
    if (is_klist_empty(&g_tick_head)) {
        return -1;
    }

    ktask_t * p_tcb  = krhino_list_entry(g_tick_head.next, ktask_t, tick_list);
    return  p_tcb->tick_match > g_tick_count ?  p_tcb->tick_match - g_tick_count : 0;
}

void aos_kernel_ticks_announce(int32_t ticks)
{
    tick_list_update((tick_i_t)ticks);
    core_sched();
}

int aos_irq_context(void)
{
    return g_intrpt_nested_level[cpu_cur_get()] > 0u;
}

int aos_is_sched_disable(void)
{
    return g_sched_lock[cpu_cur_get()] > 0u;
}

int aos_is_irq_disable(void)
{
    return !cpu_is_irq_enable();
}

void *aos_zalloc(size_t size)
{
    void *tmp = NULL;

    if (size == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_alloc(size | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_alloc(size);
#endif

    if (tmp) {
        memset(tmp, 0, size);
    }

    return tmp;
}

void *aos_malloc(size_t size)
{
    void *tmp = NULL;

    if (size == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_alloc(size | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_alloc(size);
#endif

    return tmp;
}

void *aos_calloc(size_t nitems, size_t size)
{
    void *tmp = NULL;
    size_t len = (size_t)nitems*size;

    if (len == 0) {
        return NULL;
    }

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_alloc(len | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_alloc(len);
#endif

    if (tmp) {
        memset(tmp, 0, len);
    }

    return tmp;
}

void *aos_realloc(void *mem, size_t size)
{
    void *tmp = NULL;

#if (RHINO_CONFIG_MM_DEBUG > 0u)
    tmp = krhino_mm_realloc(mem, size | AOS_UNSIGNED_INT_MSB);
    krhino_owner_return_addr(tmp);
#else
    tmp = krhino_mm_realloc(mem, size);
#endif

    return tmp;
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

void aos_alloc_trace(void *addr, uintptr_t allocator)
{
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    krhino_owner_attach(addr, allocator);
#endif
}

void aos_free(void *mem)
{
    if (mem == NULL) {
        return;
    }

    krhino_mm_free(mem);
}

void aos_calendar_time_set(uint64_t now_ms)
{
    start_time_ms = now_ms - krhino_sys_time_get();
}

uint64_t aos_calendar_time_get(void)
{
    return krhino_sys_time_get() + start_time_ms;
}

uint64_t aos_calendar_localtime_get(void)
{
    if ((aos_calendar_time_get() - 8 * 3600 * 1000) < 0) {
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

void aos_freep(char **ptr)
{
    if (ptr && (*ptr)) {
        aos_free(*ptr);
        *ptr = NULL;
    }
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
#if (RHINO_CONFIG_MM_DEBUG > 0u)
    ptr = aos_malloc(align_size | AOS_UNSIGNED_INT_MSB);
    aos_alloc_trace(ptr, (size_t)__builtin_return_address(0));
#else
    ptr = aos_malloc(align_size);
#endif
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

aos_status_t aos_task_ptcb_get(aos_task_t *task, void **ptcb)
{
    CPSR_ALLOC();
    CHECK_HANDLE(task);
    if (ptcb == NULL) {
        return -EINVAL;
    }

    RHINO_CRITICAL_ENTER();
    *(ptcb) = ((ktask_t *)*task)->ptcb;
    RHINO_CRITICAL_EXIT();

    return 0;
}

aos_status_t aos_task_ptcb_set(aos_task_t *task, void *ptcb)
{
    CPSR_ALLOC();
    CHECK_HANDLE(task);

    RHINO_CRITICAL_ENTER();
    ((ktask_t *)*task)->ptcb = ptcb;
    RHINO_CRITICAL_EXIT();

    return 0;
}

aos_status_t aos_task_pri_change(aos_task_t *task, uint8_t pri, uint8_t *old_pri)
{
    kstat_t ret;

    CHECK_HANDLE(task);

    ret = krhino_task_pri_change((ktask_t *)*task, pri, old_pri);

    return rhino2stderrno(ret);
}

aos_status_t aos_task_pri_get(aos_task_t *task, uint8_t *priority)
{
    CPSR_ALLOC();
    CHECK_HANDLE(task);
    if (priority == NULL) {
        return -EINVAL;
    }

    RHINO_CRITICAL_ENTER();
    *priority = ((ktask_t *)*task)->b_prio;
    RHINO_CRITICAL_EXIT();

    return 0;
}

static int rhino_sched_2_aos_sched_policy(int rhino_sched_policy)
{
    switch (rhino_sched_policy)
    {
    case KSCHED_CFS:
        return AOS_KSCHED_CFS;
    case KSCHED_FIFO:
        return AOS_KSCHED_FIFO;
    case KSCHED_RR:
        return AOS_KSCHED_RR;
    default:
        break;
    }
    return AOS_KSCHED_RR;
}

static int aos_sched_2_rhino_sched_policy(int aos_sched_policy)
{
    switch (aos_sched_policy)
    {
    case AOS_KSCHED_CFS:
        return KSCHED_CFS;
    case AOS_KSCHED_FIFO:
        return KSCHED_FIFO;
    case AOS_KSCHED_RR:
        return KSCHED_RR;
    default:
        break;
    }
    return KSCHED_RR;
}

aos_status_t aos_task_sched_policy_set(aos_task_t *task, uint8_t policy, uint8_t pri)
{
    kstat_t ret;

    CHECK_HANDLE(task);

    if (policy != AOS_KSCHED_FIFO && policy != AOS_KSCHED_RR && policy != AOS_KSCHED_CFS) {
        return -EINVAL;
    }
    policy = aos_sched_2_rhino_sched_policy(policy);
    ret = krhino_sched_param_set((ktask_t *)*task, policy, pri);
    return rhino2stderrno(ret);
}

aos_status_t aos_task_sched_policy_get(aos_task_t *task, uint8_t *policy)
{
    kstat_t ret = 0;

    CHECK_HANDLE(task);
    if (policy == NULL) {
        return -EINVAL;
    }
    uint8_t rhino_policy;
    ret = krhino_sched_policy_get((ktask_t *)*task, &rhino_policy);
    *policy = rhino_sched_2_aos_sched_policy(rhino_policy);
    return rhino2stderrno(ret);
}

uint32_t aos_task_sched_policy_get_default()
{
#if (RHINO_CONFIG_SCHED_CFS > 0)
    return rhino_sched_2_aos_sched_policy(KSCHED_CFS);
#else
    return rhino_sched_2_aos_sched_policy(KSCHED_RR);
#endif
}

aos_status_t aos_task_time_slice_set(aos_task_t *task, uint32_t slice)
{
    kstat_t ret;
    CHECK_HANDLE(task);

    ret = krhino_task_time_slice_set((ktask_t *)*task, MS2TICK(slice));
    return rhino2stderrno(ret);
}

aos_status_t aos_task_time_slice_get(aos_task_t *task, uint32_t *slice)
{
    uint32_t time_slice;
    CPSR_ALLOC();
    CHECK_HANDLE(task);
    if (slice == NULL) {
        return -EINVAL;
    }

    RHINO_CRITICAL_ENTER();
    time_slice = ((ktask_t *)*task)->time_slice;
    RHINO_CRITICAL_EXIT();
    *slice = (uint32_t)krhino_ticks_to_ms(time_slice);

    return 0;
}

uint32_t aos_sched_get_priority_max(uint32_t policy)
{
    return RHINO_CONFIG_PRI_MAX;
}

void aos_sys_tick_handler(void)
{
    krhino_tick_proc();
}
