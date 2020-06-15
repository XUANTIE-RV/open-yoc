/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef AOS_MAILBOX_H
#define AOS_MAILBOX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <aos/list.h>


typedef struct channel     channel_t;

// irq hardware channel interface

#define CHANNEL_WRITE_EVENT (0x01)
#define CHANNEL_READ_EVENT (0x02)

typedef void (*channel_cb)(int event, void *priv);

struct channel {
    slist_t    next;

    channel_cb cb;
    void      *priv;
    uint32_t   des_id;
    void      *context;
};

int channel_put_message(channel_t *ch, void *msg, int len, uint32_t timeout_ms);// send message to channel
int channel_get_message(channel_t *ch, void *msg, int len, uint32_t timeout_ms);// read message from channel

//mailbox
channel_t *channel_mailbox_get(int cpu_id, channel_cb cb, void *priv);
int channel_mailbox_lpm(channel_t *ch, int state);

#endif // AOS_IPC_H
