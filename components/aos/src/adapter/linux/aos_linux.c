/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/prctl.h>
#include <pthread.h>
#include <errno.h>

#undef WITH_LWIP
#undef WITH_SAL
#include <poll.h>
#include <aos/kernel.h>
#include <aos/debug.h>
#include <aos/list.h>
#include <ulog/ulog.h>

#include <semaphore.h>
#include <signal.h>
#include <assert.h>

#define TAG "AOS"

#define CLOCKID CLOCK_MONOTONIC

static timer_t         s_timerid;
static dlist_t         s_aos_timer_listhead;
static pthread_mutex_t s_list_mutex;
static sem_t           s_timer_sem;

typedef void (*timer_cb_t)(void *timer, void *args);

typedef struct timer_s {
    timer_cb_t handler;
    void *     args;
    uint64_t   timeout;
    uint64_t   start_ms;
    int        repeat;
} p_timer_t;

typedef struct timer_list {
    dlist_t    node;
    p_timer_t *timer;
} timer_list_t;

void aos_reboot(void)
{
    exit(0);
}

int aos_get_hz(void)
{
    return 1000;
}

const char *aos_version_get(void)
{
    return "aos-linux-xxx";
}

struct targ {
    const char *name;
    void (*fn)(void *);
    void *arg;
};

static void *dfl_entry(void *arg)
{
    struct targ *targ  = arg;
    void (*fn)(void *) = targ->fn;
    void *farg         = targ->arg;
    prctl(PR_SET_NAME, (unsigned long)targ->name, 0, 0, 0);
    free(targ);

    fn(farg);

    return 0;
}

int aos_task_new(const char *name, void (*fn)(void *), void *arg, int stack_size)
{
    int          ret;
    pthread_t    th;
    struct targ *targ = malloc(sizeof(*targ));
    targ->name        = strdup(name);
    targ->fn          = fn;
    targ->arg         = arg;
    ret               = pthread_create(&th, NULL, dfl_entry, targ);
    if (ret == 0)
        ret = pthread_detach(th);
    return ret;
}

int aos_task_new_ext(aos_task_t *task, const char *name, void (*fn)(void *), void *arg, int stack_size, int prio)
{
    return aos_task_new(name, fn, arg, stack_size);
}

void aos_task_exit(int code)
{
    int ret;
    pthread_exit(&ret);
}

const char *aos_task_name(void)
{
    static char name[16];
    prctl(PR_GET_NAME, (unsigned long)name, 0, 0, 0);
    return name;
}

int aos_task_key_create(aos_task_key_t *key)
{
    return pthread_key_create(key, NULL);
}

void aos_task_key_delete(aos_task_key_t key)
{
    pthread_key_delete(key);
}

int aos_task_setspecific(aos_task_key_t key, void *vp)
{
    return pthread_setspecific(key, vp);
}

void *aos_task_getspecific(aos_task_key_t key)
{
    return pthread_getspecific(key);
}

int aos_mutex_new(aos_mutex_t *mutex)
{
    pthread_mutex_t *   mtx = malloc(sizeof(*mtx));
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);

    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);

    pthread_mutex_init(mtx, &attr);
    *mutex = mtx;
    return 0;
}

void aos_mutex_free(aos_mutex_t *mutex)
{
    pthread_mutex_destroy((pthread_mutex_t *)*mutex);
    free(*mutex);
    *mutex = NULL;
}

int aos_mutex_lock(aos_mutex_t *mutex, unsigned int timeout)
{
    if (!mutex)
        return -EINVAL;

    if (timeout == AOS_WAIT_FOREVER) {
        pthread_mutex_lock((pthread_mutex_t *)*mutex);
    } else {
        struct timespec abs_timeout;

        abs_timeout.tv_sec  = timeout / 1000;
        abs_timeout.tv_nsec = timeout % 1000 * 1000000;

        return pthread_mutex_timedlock((pthread_mutex_t *)*mutex, &abs_timeout);
    }
    return 0;
}

int aos_mutex_unlock(aos_mutex_t *mutex)
{
    if (mutex) {
        pthread_mutex_unlock((pthread_mutex_t *)*mutex);
    }
    return 0;
}

int aos_mutex_is_valid(aos_mutex_t *mutex)
{
    return mutex && *mutex != NULL;
}

#include <semaphore.h>
int aos_sem_new(aos_sem_t *sem, int count)
{
    sem_t *s = malloc(sizeof(*s));
    sem_init(s, 0, count);
    *sem = s;
    return 0;
}

void aos_sem_free(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    sem_destroy((sem_t *)*sem);
    free(*sem);
    *sem = NULL;
}

int aos_sem_wait(aos_sem_t *sem, unsigned int timeout)
{
    int sec;
    int nsec;

    if (sem == NULL) {
        return -EINVAL;
    }

    if (timeout == AOS_WAIT_FOREVER) {
        return sem_wait((sem_t *)*sem);
    } else if (timeout == 0) {
        return sem_trywait((sem_t *)*sem);
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    sec  = timeout / 1000;
    nsec = (timeout % 1000) * 1000;

    ts.tv_nsec += nsec;
    sec += (ts.tv_nsec / 1000000000);
    ts.tv_nsec %= 1000000000;
    ts.tv_sec += sec;

    return sem_timedwait((sem_t *)*sem, &ts);
}

void aos_sem_signal(aos_sem_t *sem)
{
    if (sem == NULL) {
        return;
    }

    sem_post((sem_t *)*sem);
}

int aos_sem_is_valid(aos_sem_t *sem)
{
    return sem && *sem != NULL;
}

void aos_sem_signal_all(aos_sem_t *sem)
{
    sem_post((sem_t *)*sem);
}

struct queue {
    int   fds[2];
    void *buf;
    int   size;
    int   msg_size;
};

int aos_queue_new(aos_queue_t *queue, void *buf, size_t size, int max_msg)
{
    struct queue *q = malloc(sizeof(*q));
    pipe(q->fds);
    q->buf      = buf;
    q->size     = size;
    q->msg_size = max_msg;
    *queue      = q;
    return 0;
}

void aos_queue_free(aos_queue_t *queue)
{
    struct queue *q = *queue;
    close(q->fds[0]);
    close(q->fds[1]);
    free(q);
    q = NULL;
}

int aos_queue_send(aos_queue_t *queue, void *msg, size_t size)
{
    struct queue *q = *queue;
    write(q->fds[1], msg, size);
    return 0;
}

int aos_queue_recv(aos_queue_t *queue, unsigned int ms, void *msg, size_t *size)
{
    struct queue *q   = *queue;
    struct pollfd rfd = {
        .fd     = q->fds[0],
        .events = POLLIN,
    };

    poll(&rfd, 1, ms);
    if (rfd.revents & POLLIN) {
        int len = read(q->fds[0], msg, q->msg_size);
        *size   = len;
        return len < 0 ? -1 : 0;
    }

    return -1;
}

int aos_queue_is_valid(aos_queue_t *queue)
{
    return queue && *queue != NULL;
}

void *aos_queue_buf_ptr(aos_queue_t *queue)
{
    struct queue *q = *queue;
    return q->buf;
}

struct work {
    void (*fn)(void *);
    void *arg;
    int   dly;
};

int aos_work_init(aos_work_t *work, void (*fn)(void *), void *arg, int dly)
{
    struct work *w = malloc(sizeof(*w));
    w->fn          = fn;
    w->arg         = arg;
    w->dly         = dly;
    *work          = w;
    return 0;
}

void aos_work_destroy(aos_work_t *work)
{
    free(*work);
    *work = NULL;
}

int aos_work_run(aos_workqueue_t *workqueue, aos_work_t *work)
{
    return aos_work_sched(work);
}

static void worker_entry(void *arg)
{
    struct work *w = arg;
    if (w->dly) {
        usleep(w->dly * 1000);
    }
    w->fn(w->arg);
}

int aos_work_sched(aos_work_t *work)
{
    struct work *w = *work;
    return aos_task_new("worker", worker_entry, w, 8192);
}

int aos_work_cancel(aos_work_t *work)
{
    return -1;
}

void *aos_zalloc(size_t size)
{
    return calloc(size, 1);
}

void *aos_zalloc_check(size_t size)
{
    return calloc(size, 1);
}

void *aos_malloc(size_t size)
{
    return malloc(size);
}

void *aos_malloc_check(size_t size)
{
    void *p = aos_malloc(size);
    aos_check_mem(p);

    return p;
}

void *aos_calloc(size_t size, size_t num)
{
    void *ptr = malloc(size * num);
    if (ptr) {
        memset(ptr, 0, size * num);
    }

    return ptr;
}

void *aos_realloc(void *mem, size_t size)
{
    return realloc(mem, size);
}

void aos_alloc_trace(void *addr, size_t allocator) {}

void aos_free(void *mem)
{
    free(mem);
}

long long aos_now()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_BOOTTIME, &ts) == -1) {
        LOGE(TAG, "%s unable to get current time: %s", __func__, strerror(errno));
        return 0;
    }

    return (ts.tv_sec * 1000000000LL) + (ts.tv_nsec);
}

long long aos_now_ms()
{
    struct timespec ts;
    if (clock_gettime(CLOCK_BOOTTIME, &ts) == -1) {
        LOGE(TAG, "%s unable to get current time: %s", __func__, strerror(errno));
        return 0;
    }

    return (ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);
}

void aos_msleep(int ms)
{
    usleep(ms * 1000);
}

void aos_init(void) {}

void aos_start(void)
{
    while (1) {
        usleep(1000 * 1000 * 100);
    }
}

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
void dumpsys_task_func(void)
{
    DIR *proc = opendir("/proc/self/task");
    while (1) {
        struct dirent *ent = readdir(proc);
        if (!ent) {
            break;
        }
        if (ent->d_name[0] == '.') {
            continue;
        }

        char fn[128];
        snprintf(fn, sizeof fn, "/proc/self/task/%s/comm", ent->d_name);
        FILE *fp = fopen(fn, "r");
        if (!fp) {
            continue;
        }
        bzero(fn, sizeof fn);
        fread(fn, sizeof(fn) - 1, 1, fp);
        fclose(fp);
        printf("%8s - %s", ent->d_name, fn);
    }
    closedir(proc);
}

char *aos_get_app_version(void)
{
    int32_t     ret;
    static char str_version[65] = "app_v1.0";

    return str_version;
}

int aos_event_get(aos_event_t *event, unsigned int flags, unsigned char opt, unsigned int *actl_flags,
                  unsigned int timeout)
{
    int sec;
    int nsec;

    if (event == NULL) {
        return -EINVAL;
    }

    if (timeout == AOS_WAIT_FOREVER) {
        return sem_wait((sem_t *)*event);
    } else if (timeout == 0) {
        return sem_trywait((sem_t *)*event);
    }

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    sec  = timeout / 1000;
    nsec = (timeout % 1000) * 1000;

    ts.tv_nsec += nsec;
    sec += (ts.tv_nsec / 1000000000);
    ts.tv_nsec %= 1000000000;
    ts.tv_sec += sec;

    return sem_timedwait((sem_t *)*event, &ts);
}

int aos_event_set(aos_event_t *event, unsigned int flags, unsigned char opt)
{
    if (event == NULL) {
        return -EINVAL;
    }

    sem_post((sem_t *)*event);

    return 0;
}

int aos_event_is_valid(aos_event_t *event)
{
    return event && *event != NULL;
}

void aos_event_free(aos_event_t *event)
{
    if (event == NULL) {
        return;
    }

    sem_destroy((sem_t *)*event);
    free(*event);
    *event = NULL;
}

int aos_event_new(aos_event_t *event, unsigned int flags)
{
    sem_t *s = malloc(sizeof(*s));
    sem_init(s, 0, 0);
    *event = s;
    return 0;
}

static void timer_callback(void *ptr)
{
    sem_post(&s_timer_sem);
}

static uint64_t now(void)
{
    struct timespec ts;
    if (clock_gettime(CLOCK_BOOTTIME, &ts) == -1) {
        LOGE(TAG, "%s unable to get current time: %s", __func__, strerror(errno));
        return 0;
    }

    return (ts.tv_sec * 1000LL) + (ts.tv_nsec / 1000000LL);
}

// static void start_timer(uint64_t timerout_ms)
// {
//     struct itimerspec wakeup_time;
//     memset(&wakeup_time, 0, sizeof(wakeup_time));

//     // LOGD(TAG, "%s timer start %d\n", __func__, timerout_ms);

//     wakeup_time.it_value.tv_sec = (timerout_ms / 1000);
//     wakeup_time.it_value.tv_nsec = (timerout_ms % 1000) * 1000000LL;

//     if (timer_settime(s_timerid, TIMER_ABSTIME, &wakeup_time, NULL) == -1)
//         LOGE(TAG, "%s unable to set timer: %s",
//                     __func__, strerror(errno));
// }

static void restart_timer(uint64_t timerout_ms)
{
    struct itimerspec wakeup_time;
    memset(&wakeup_time, 0, sizeof(wakeup_time));

    // LOGD(TAG, "%s timeout %d\n", __func__, timerout_ms);

    /* stop the timer */
    timer_settime(s_timerid, TIMER_ABSTIME, &wakeup_time, NULL);

    wakeup_time.it_value.tv_sec  = (timerout_ms / 1000);
    wakeup_time.it_value.tv_nsec = (timerout_ms % 1000) * 1000000LL;

    if (timer_settime(s_timerid, TIMER_ABSTIME, &wakeup_time, NULL) == -1)
        LOGE(TAG, "%s unable to set timer: %s", __func__, strerror(errno));
}

static void add_node_to_list(p_timer_t *timer)
{
    timer_list_t *node = malloc(sizeof(timer_list_t));
    timer_list_t *tmp_node;

    if (node == NULL) {
        LOGE(TAG, "OOM\n");
        return;
    }

    timer->start_ms = now() + timer->timeout;
    node->timer     = timer;

    if (!dlist_empty(&s_aos_timer_listhead)) {
        dlist_for_each_entry(&s_aos_timer_listhead, tmp_node, timer_list_t, node)
        {
            if (tmp_node->timer->start_ms > timer->start_ms) {
                break;
            }
        }

        dlist_add_tail(&node->node, &tmp_node->node);
    } else {
        dlist_add_tail(&node->node, &s_aos_timer_listhead);
    }
}

static int is_in_list(p_timer_t *timer)
{
    timer_list_t *tmp_node;

    dlist_for_each_entry(&s_aos_timer_listhead, tmp_node, timer_list_t, node)
    {
        if (tmp_node->timer == timer) {
            return 1;
        }
    }

    return 0;
}

static void *timer_dispatch(void *args)
{
    while (1) {
        sem_wait(&s_timer_sem);

        // LOGD(TAG, "%s entry 1\n", __func__);

        if (dlist_empty(&s_aos_timer_listhead)) {
            continue;
        }

        timer_list_t *tmp_node;

        // LOGD(TAG, "%s entry 2\n", __func__);

        pthread_mutex_lock(&s_list_mutex);

        while (1) {
            int        del = 0;
            p_timer_t *tmp_timer;
            uint64_t   timeout_ms = now();

            dlist_for_each_entry(&s_aos_timer_listhead, tmp_node, timer_list_t, node)
            {
                if (tmp_node->timer->start_ms <= timeout_ms) {
                    tmp_timer = tmp_node->timer;
                    tmp_node->timer->handler(tmp_node->timer, tmp_node->timer->args);
                    // LOGD(TAG, "%s handler\n", __func__);
                    del = 1;
                    break;
                }
            }

            if (del == 0) {
                break;
            } else {
                /* check the list for avoid call aos_timer_stop in timer->handle */
                if (is_in_list(tmp_timer) && tmp_timer->start_ms <= timeout_ms) {
                    dlist_del(&tmp_node->node);

                    if (tmp_timer->repeat == 1) {
                        add_node_to_list(tmp_timer);
                    }
                    free(tmp_node);
                }
            }
        }

        if (!dlist_empty(&s_aos_timer_listhead)) {
            tmp_node = dlist_first_entry(&s_aos_timer_listhead, timer_list_t, node);

            // LOGD(TAG, "%s timer start %p\n", __func__, tmp_node->timer);

            restart_timer(tmp_node->timer->start_ms);
        }

        pthread_mutex_unlock(&s_list_mutex);
    }

    return NULL;
}

static void lazy_init()
{
    static int init = 0;

    if (init == 0) {
        struct sigevent sigevent;

        memset(&sigevent, 0, sizeof(sigevent));
        sigevent.sigev_notify          = SIGEV_THREAD;
        sigevent.sigev_notify_function = (void (*)(union sigval))timer_callback;

        if (timer_create(CLOCKID, &sigevent, &s_timerid) == -1) {
            LOGE(TAG, "%s unable to create timer with clock %d: %s\n", __func__, CLOCKID, strerror(errno));
            assert(NULL);
        }

        dlist_init(&s_aos_timer_listhead);

        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);

        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);

        pthread_mutex_init(&s_list_mutex, &attr);

        sem_init(&s_timer_sem, 0, 0);

        pthread_t tid = 0;

        pthread_create(&tid, NULL, timer_dispatch, NULL);

        pthread_setname_np(tid, "timer_task");
    }

    init = 1;
}

int aos_timer_new(aos_timer_t *timer, void (*fn)(void *, void *), void *arg, int ms, int repeat)
{
    lazy_init();

    p_timer_t *t = malloc(sizeof(p_timer_t));

    if (t == NULL) {
        LOGE(TAG, "%s OOM", __func__);
        return -1;
    }

    t->handler  = fn;
    t->args     = arg;
    t->start_ms = 0;
    t->repeat   = repeat;
    t->timeout  = ms;

    *timer = (void *)t;

    return 0;
}

int aos_timer_new_ext(aos_timer_t *timer, void (*fn)(void *, void *), void *arg, int ms, int repeat,
                      unsigned char auto_run)
{
    aos_timer_new(timer, fn, arg, ms, repeat);

    if (auto_run == 1) {
        return aos_timer_start(timer);
    } else {
        return 0;
    }
}

int aos_timer_start(aos_timer_t *timer)
{
    timer_list_t *tmp_node;

    assert(timer);

    p_timer_t *t = *timer;
    // LOGD(TAG, "%s timer %p,timeout %u\n", __func__, timer, timeout);

    t->start_ms = now() + t->timeout;

    timer_list_t *node = malloc(sizeof(timer_list_t));

    if (node == NULL) {
        LOGE(TAG, "OOM\n");
        return -1;
    }

    node->timer = t;

    pthread_mutex_lock(&s_list_mutex);

    /* rm the timer if the timer is in the list */
    dlist_for_each_entry(&s_aos_timer_listhead, tmp_node, timer_list_t, node)
    {
        if (tmp_node->timer == t) {
            dlist_del(&tmp_node->node);
            free(tmp_node);
            break;
        }
    }

    if (!dlist_empty(&s_aos_timer_listhead)) {
        int first = 0;
        dlist_for_each_entry(&s_aos_timer_listhead, tmp_node, timer_list_t, node)
        {
            if (tmp_node->timer->start_ms > t->start_ms) {
                first++;
                break;
            }
        }

        dlist_add_tail(&node->node, &tmp_node->node);

        /* if the timer is earliest, restart posix timer */
        if (first == 1) {
            restart_timer(t->start_ms);
        }
    } else {
        dlist_add_tail(&node->node, &s_aos_timer_listhead);

        restart_timer(node->timer->start_ms);
    }

    pthread_mutex_unlock(&s_list_mutex);

    return 0;
}

int aos_timer_stop(aos_timer_t *timer)
{
    timer_list_t *tmp_node;
    assert(timer);
    p_timer_t *t = *timer;

    pthread_mutex_lock(&s_list_mutex);

    /* rm the timer if the timer is in the list */
    dlist_for_each_entry(&s_aos_timer_listhead, tmp_node, timer_list_t, node)
    {
        if (tmp_node->timer == t) {
            dlist_del(&tmp_node->node);
            free(tmp_node);
            break;
        }
    }

    pthread_mutex_unlock(&s_list_mutex);
    return 0;
}

void aos_timer_free(aos_timer_t *timer)
{
    free(*timer);
}

int aos_timer_change(aos_timer_t *timer, int ms)
{
    assert(timer);

    p_timer_t *t = *timer;

    pthread_mutex_lock(&s_list_mutex);
    t->timeout = ms;
    pthread_mutex_unlock(&s_list_mutex);

    return aos_timer_start(timer);
}

int aos_timer_change_once(aos_timer_t *timer, int ms)
{
    assert(timer);

    p_timer_t *t = *timer;

    pthread_mutex_lock(&s_list_mutex);
    t->repeat  = 0;
    t->timeout = ms;
    pthread_mutex_unlock(&s_list_mutex);

    return aos_timer_start(timer);
}

int aos_timer_is_valid(aos_timer_t *timer)
{
    p_timer_t *t;

    if (timer == NULL) {
        return 0;
    }

    t = *timer;
    if (t == NULL) {
        return 0;
    }

    if (t->handler == NULL) {
        return 0;
    }

    return 1;
}
