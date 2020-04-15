/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef IPC_INTERNAL_H
#define IPC_INTERNAL_H

#include <ipc.h>
#include "channel_mailbox.h"
#include <aos/list.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef linux
#   define aos_mutex_new(mutex)        pthread_mutex_init(mutex, NULL)
#   define aos_mutex_lock(mutex, n)    pthread_mutex_lock(mutex)
#   define aos_mutex_unlock(mutex)     pthread_mutex_unlock(mutex)
#   define aos_sem_new(sem, n)         sem_init(sem, 0, n)
static int aos_sem_wait(aos_sem_t *sem, int timeout_ms)
{
    if (timeout_ms >= 0) {
        struct timespec abs_timeout;
        abs_timeout.tv_sec  = timeout_ms / 1000;
        abs_timeout.tv_nsec = (timeout_ms % 1000) * 1000;

        return sem_timedwait(sem, &abs_timeout);
    } else {
        return sem_wait(sem);
    }
}

#   define aos_sem_signal(sem)         sem_post(sem)
#   define aos_sem_free(sem)           sem_destroy(sem)
#   define aos_task_new_ext(task, n, fn, arg, size, prio)  pthread_create(task, NULL, fn, arg)
#   define aos_task_exit(n) pthread_exit(n)
#endif

#define PHY_ACK (0X80)

#define SHM_ALIGN_CACHE (16)

#define SHM_ALIGN(addr, align) ((void *)(((uint32_t)(addr) + align - 1U) & (~(uint32_t)(align - 1U))))
#define SHM_ALIGN_SIZE(size, align) (((uint32_t)(size) + align - 1U) & (~(align - 1U)))
#define SHM_ALIGN_CHECK(size, align) (size == SHM_ALIGN_SIZE(size, align))

#define SER_NAME_MAX_LEN 16

typedef struct _shm {
    void       *addr;
    size_t      size;
    int         flag;
    size_t      widx;
} shm_t;

typedef struct phy_msg {
    uint8_t         flag;            /** flag for MESSAGE_SYNC and MESSAGE_ACK */
    uint8_t         service_id;      /** service id for the service want to comtunicate */
    uint16_t        command;         /** command id the service provide */
    uint32_t        seq;             /** message seq */
    void           *data;            /** message data */
    int             len;             /** message len */
    aos_sem_t       sem;             /** queue for SYNC MESSAGE */
} phy_data_t;

#define SERVER_QUEUE_SIZE (sizeof(phy_data_t) * 10)
typedef struct service {
    int id;
    ipc_process_t process;
    void *priv;
    slist_t next;
    aos_task_t task;
    ipc_t *ipc;
    char ser_name[SER_NAME_MAX_LEN];
    char que_buf[SERVER_QUEUE_SIZE];
    aos_queue_t queue;
} service_t;

typedef struct _dispatch {
    int total_len;
    int resp_len;
    aos_queue_t queue;
} dispatch_t;

struct ipc {
    slist_t next;
    channel_t *ch;
    slist_t    service_list;

    uint32_t   des_cpu_id;

    aos_task_t thread;
    aos_event_t evt;
    aos_mutex_t ch_mutex;
    aos_mutex_t tx_mutex;
    aos_mutex_t rx_mutex;
    aos_sem_t   sem;
    void *priv;
    shm_t shm;
    uint32_t seq;
    uint32_t seq_bake;
};



#ifdef __cplusplus
}
#endif

#endif
