/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef SC5654_IPC_H
#define SC5654_IPC_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef linux
#include <semaphore.h>
#include <pthread.h>
typedef pthread_mutex_t aos_mutex_t;
typedef pthread_t aos_task_t;
typedef sem_t aos_sem_t;
#else
#include <aos/kernel.h>
#endif

#include <aos/list.h>

#define  AP_CPU_IDX 0
#define  CP_CPU_IDX 1

typedef struct message_msg message_t;
typedef struct ipc         ipc_t;
typedef struct channel     channel_t;

typedef void (*ipc_process_t)(ipc_t *ipc, message_t *msg);
typedef void (*msg_free_hook_t)(message_t *msg);

struct message_msg {
    slist_t         next;
    msg_free_hook_t free_hook;
    uint16_t        command;
    uint8_t         sync;
#ifdef linux
    sem_t           sem;
#else
    aos_sem_t       sem;
#endif
    uint32_t        id;
    int             len;
    void           *data;
};

struct ipc {
    ipc_process_t process;
    channel_t *ch;

    aos_task_t thread;
    aos_event_t evt;
    aos_mutex_t mutex;
    void *priv;
};

// irq hardware channel interface

#define CHANNEL_WRITE_EVENT (0x01)
#define CHANNEL_READ_EVENT (0x02)

struct channel {
    int (*put_message)(channel_t *ch, message_t *msg, uint32_t timeout_ms);// send message to channel
    int (*get_message)(channel_t *ch, message_t *msg, uint32_t timeout_ms);// read message from channel

    void (*event)(ipc_t *ipc, int event);
    ipc_t *ipc;
    void  *context;
};

//mailbox
channel_t *yv_channel_mailbox_get(void);

// channel
void       channel_init(channel_t *ch);
void channel_event(channel_t *ch, int event);

// ipc
ipc_t     *ipc_create(channel_t *ch, ipc_process_t cb, int cache_size, void *priv);
int        ipc_message_put(ipc_t *ipc, message_t *msg, int timeout_ms);
void      *ipc_get_privdata(ipc_t *ipc);


// message
int message_init(message_t *msg, int sync);
void message_deinit(message_t *msg);
void message_post(message_t *msg);
void message_wait(message_t *msg);

#endif // SC5654_IPC_H
