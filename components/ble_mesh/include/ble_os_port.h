/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef _BLE_OS_PORT_H
#define _BLE_OS_PORT_H

#ifndef __aligned
#define __aligned(x)          __attribute__((__aligned__(x)))
#endif

#ifndef BIT
#define BIT(nr)                 (1UL << (nr))
#endif

#define __noinit

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(array) \
	((unsigned long) (sizeof(array) / sizeof((array)[0])))
#endif

typedef kbuf_queue_t _queue_t;
typedef ksem_t       _sem_t;
typedef ktask_t      _task_t;
typedef cpu_stack_t  _stack_element_t;
typedef kmutex_t     _mutex_t;

/* Log define*/
enum {
    LOG_LEVEL_NONE = 0,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARN,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_MAX_BIT
};

#define BT_TAG "AOSBT"

#define BT_LOG_DBG 1
#if BT_LOG_DBG
#define SYS_LOG_DBG(fmt,...) LOGD(BT_TAG, "%s:"fmt, __FUNCTION__, ##__VA_ARGS__)
#define SYS_LOG_INF(fmt,...) LOGI(BT_TAG, "%s:"fmt, __FUNCTION__, ##__VA_ARGS__)
#define SYS_LOG_WRN(fmt,...) LOGW(BT_TAG, "%s:"fmt, __FUNCTION__, ##__VA_ARGS__)
#define SYS_LOG_ERR(fmt,...) LOGE(BT_TAG, "%s:"fmt, __FUNCTION__, ##__VA_ARGS__)
#else
#define SYS_LOG_DBG(fmt,...) LOGD(BT_TAG, fmt, ##__VA_ARGS__)
#define SYS_LOG_INF(fmt,...) LOGI(BT_TAG, fmt, ##__VA_ARGS__)
#define SYS_LOG_WRN(fmt,...) LOGW(BT_TAG, fmt, ##__VA_ARGS__)
#define SYS_LOG_ERR(fmt,...) LOGE(BT_TAG, fmt, ##__VA_ARGS__)
#endif

struct k_queue {
    _sem_t sem;
    sys_slist_t queue_list;
    sys_dlist_t poll_events;
};

extern void  k_queue_init(struct k_queue *queue);
extern void  k_queue_uninit(struct k_queue *queue);
extern void  k_queue_cancel_wait(struct k_queue *queue);
extern void  k_queue_append(struct k_queue *queue, void *data);
extern void  k_queue_prepend(struct k_queue *queue, void *data);
extern void  k_queue_insert(struct k_queue *queue, void *prev, void *data);
extern void  k_queue_append_list(struct k_queue *queue, void *head, void *tail);
extern void *k_queue_get(struct k_queue *queue, int32_t timeout);
extern int   k_queue_is_empty(struct k_queue *queue);
extern int   k_queue_count(struct k_queue *queue);

/* lifo define*/
struct k_lifo {
    struct k_queue _queue;
};

#define k_lifo_init(lifo) k_queue_init((struct k_queue *)lifo)

#define k_lifo_put(lifo, data) k_queue_prepend((struct k_queue *)lifo, data)

#define k_lifo_get(lifo, timeout) k_queue_get((struct k_queue *)lifo, timeout)

#define k_lifo_num_get(lifo) k_queue_count((struct k_queue *)lifo)

struct kfifo {
    struct k_queue _queue;
};

#define k_fifo_init(fifo) k_queue_init((struct k_queue *)fifo)

#define k_fifo_cancel_wait(fifo) k_queue_cancel_wait((struct k_queue *)fifo)

#define k_fifo_put(fifo, data) k_queue_append((struct k_queue *)fifo, data)

#define k_fifo_put_list(fifo, head, tail) \
    k_queue_append_list((struct k_queue *)fifo, head, tail)

#define k_fifo_get(fifo, timeout) k_queue_get((struct k_queue *)fifo, timeout)
#define k_fifo_num_get(fifo) k_queue_count((struct k_queue *)fifo)

/* sem define*/
struct k_sem {
    _sem_t      sem;
    sys_dlist_t poll_events;
};

/**
 * @brief Initialize a semaphore.
 *
 * This routine initializes a semaphore object, prior to its first use.
 *
 * @param sem Address of the semaphore.
 * @param initial_count Initial semaphore count.
 * @param limit Maximum permitted semaphore count.
 *
 * @return 0 Creat a semaphore succcess
 */
int k_sem_init(struct k_sem *sem, unsigned int initial_count,
               unsigned int limit);

/**
 * @brief Take a semaphore.
 *
 * This routine takes @a sem.
 *
 * @note Can be called by ISRs, but @a timeout must be set to K_NO_WAIT.
 *
 * @param sem Address of the semaphore.
 * @param timeout Waiting period to take the semaphore (in milliseconds),
 *                or one of the special values K_NO_WAIT and K_FOREVER.
 *
 * @note When porting code from the nanokernel legacy API to the new API, be
 * careful with the return value of this function. The return value is the
 * reverse of the one of nano_sem_take family of APIs: 0 means success, and
 * non-zero means failure, while the nano_sem_take family returns 1 for success
 * and 0 for failure.
 *
 * @retval 0 Semaphore taken.
 * @retval -EBUSY Returned without waiting.
 * @retval -EAGAIN Waiting period timed out.
 */
int k_sem_take(struct k_sem *sem, uint32_t timeout);

/**
 * @brief Give a semaphore.
 *
 * This routine gives @a sem, unless the semaphore is already at its maximum
 * permitted count.
 *
 * @note Can be called by ISRs.
 *
 * @param sem Address of the semaphore.
 *
 * @return 0 Give semaphore success
 */
int k_sem_give(struct k_sem *sem);

/**
 * @brief Delete a semaphore.
 *
 * This routine delete @a sem,
 *
 * @note Can be called by ISRs.
 *
 * @param sem Address of the semaphore.
 *
 * @return 0 delete semaphore success
 */
int k_sem_delete(struct k_sem *sem);

/**
 * @brief Get a semaphore's count.
 *
 * This routine returns the current count of @a sem.
 *
 * @param sem Address of the semaphore.
 *
 * @return Current semaphore count.
 */
unsigned int k_sem_count_get(struct k_sem *sem);

/* mutex define*/
struct k_mutex {
    _mutex_t    mutex;
    sys_dlist_t poll_events;
};

typedef void (*k_timer_handler_t)(void *timer, void *args);
typedef struct k_timer {
    ktimer_t          timer;
    k_timer_handler_t handler;
    void             *args;
    uint32_t          timeout;
    uint32_t          start_ms;
} k_timer_t;

/**
 * @brief Initialize a timer.
 *
 * This routine initializes a timer, prior to its first use.
 *
 * @param timer     Address of timer.
 * @param handle Function to invoke each time the timer expires.
 * @param args   Arguments sent to handle.
 *
 * @return N/A
 */
void k_timer_init(k_timer_t *timer, k_timer_handler_t handle, void *args);

/**
 * @brief Start a timer.
 *
 * @param timer    Address of timer.
 * @param timeout  time before timeout happen(in milliseconds).
 *
 * @return N/A
 */
void k_timer_start(k_timer_t *timer, uint32_t timeout);

/**
 * @brief Stop a timer.
 *
 * @param timer     Address of timer.
 *
 * @return N/A
 */
void k_timer_stop(k_timer_t *timer);

/**
 * @brief Get time now.
 *
 * @return time(in milliseconds)
 */
int64_t k_uptime_get(void);

/**
 * @brief Get time now.
 *
 * @return time(in milliseconds)
 */
uint32_t k_uptime_get_32(void);

/**
 * @brief Judge if a timer started.
 *
 * @param timer     Address of timer.
 *
 * @return N/A
 */
bool k_timer_is_started(k_timer_t *timer);

void k_sleep(int32_t duration);

void k_mutex_init(struct k_mutex *mutex);
int k_mutex_lock(struct k_mutex *mutex, s32_t timeout);
void k_mutex_unlock(struct k_mutex *mutex);

#endif /* KPORT_H */
