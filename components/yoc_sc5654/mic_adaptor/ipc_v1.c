/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>

#include <aos/log.h>

#include "ipc_v1.h"
#define TAG "IPC"

#define IPC_WRITE_EVENT CHANNEL_WRITE_EVENT
#define IPC_READ_EVENT CHANNEL_READ_EVENT

// message
int message_init(message_t *msg, int sync)
{
    memset(msg, 0x00, sizeof(message_t));
    if (sync) {
        msg->sync = 1;
        aos_sem_new(&msg->sem, 0);
        return 0;
    }
    return -1;
}

void message_deinit(message_t *msg)
{
    if (msg->sync)
        aos_sem_free(&msg->sem);

    if (msg->free_hook)
        msg->free_hook(msg);
}

void message_post(message_t *msg)
{
    if (msg->sync)
        aos_sem_signal(&msg->sem);
}

void message_wait(message_t *msg)
{
    if (msg->sync)
        aos_sem_wait(&msg->sem, -1);
}

// channel
void channel_init(channel_t *ch)
{
    memset(ch, 0, sizeof(channel_t));
}

static int channel_put(channel_t *ch, message_t *msg, int timeout_ms)
{
    int ret = 0;

    if (ch->put_message) {
        ret = ch->put_message(ch, msg, timeout_ms);
    }

    if (ret < 0) {
        LOGW(TAG, "ipc_message_put(%d) error 0x%x\n", msg->command, ret);
    }

    return ret;
}

void channel_event(channel_t *ch, int event)
{
    if (ch->event)
        ch->event(ch->ipc, event);
}

static int channel_get(channel_t *ch, message_t *msg, uint32_t timeout_ms)
{
    if (ch->get_message)
        return ch->get_message(ch, msg, timeout_ms);

    return 0;
}

static void ipc_event(ipc_t *ipc, int event)
{
    aos_event_set(&ipc->evt, event, AOS_EVENT_OR);
}

int ipc_message_put(ipc_t *ipc, message_t *msg, int timeout_ms)
{
    int ret;

    aos_mutex_lock(&ipc->mutex, AOS_WAIT_FOREVER);

    unsigned int flag;

    aos_event_get(&ipc->evt, IPC_WRITE_EVENT, AOS_EVENT_OR_CLEAR, &flag, timeout_ms);
    ret = channel_put(ipc->ch, msg, timeout_ms);
    aos_mutex_unlock(&ipc->mutex);

    return ret;
}

static int ipc_message_get(ipc_t *ipc, message_t *msg, int timeout_ms)
{
    unsigned int flag;

    aos_event_get(&ipc->evt, IPC_READ_EVENT, AOS_EVENT_OR_CLEAR, &flag, timeout_ms);
    aos_mutex_lock(&ipc->mutex, AOS_WAIT_FOREVER);
    int ret = channel_get(ipc->ch, msg, timeout_ms);
    aos_mutex_unlock(&ipc->mutex);

    return ret;
}

#ifdef linux
static void *ipc_task_process_entry(void *arg)
#else
static void ipc_task_process_entry(void *arg)
#endif
{
    ipc_t *   ipc = (ipc_t *)arg;
    message_t msg;
    int       ret;
    int       len = sizeof(message_t);

    while (1) {

        ret = ipc_message_get(ipc, &msg, AOS_WAIT_FOREVER);
        if (ipc->process && (ret == len)) {
            ipc->process(ipc, &msg);
        }
    }

    aos_task_exit(0);
}

ipc_t *ipc_create(channel_t *ch, ipc_process_t cb, int cache_size, void *priv)
{
    ipc_t *ipc = calloc(1, sizeof(ipc_t));
    ch->ipc    = ipc;

    ipc->ch        = ch;
    ipc->process   = cb;
    ipc->priv      = priv;
    ipc->ch->event = ipc_event;
    aos_event_new(&ipc->evt, 0);
    aos_mutex_new(&ipc->mutex);
    aos_event_set(&ipc->evt, IPC_WRITE_EVENT, AOS_EVENT_OR);

    aos_task_new_ext(&ipc->thread, "ipc", ipc_task_process_entry, ipc, 2048, 9);

    return ipc;
}

void *ipc_get_privdata(ipc_t *ipc)
{
    return ipc->priv;
}
