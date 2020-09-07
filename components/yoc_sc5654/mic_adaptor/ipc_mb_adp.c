/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>
#include <drv/mailbox.h>

#include "ipc_v1.h"

#define TAG "MAILBOX"

static channel_t channel_mailbox;

#define EVENT_WRITE (0x01)

static int channel_mailbox_message_put(channel_t *ch, message_t *msg, uint32_t timeout)
{
    int32_t ret;
    mailbox_handle_t handle = (mailbox_handle_t)ch->context;

    /* 5654 DSP ID == 1*/
    ret = csi_mailbox_send(handle, 1, msg, sizeof(message_t));

    return ret;
}

static int channel_mailbox_message_get(channel_t *ch, message_t *msg, uint32_t timeout)
{
    mailbox_handle_t handle = (mailbox_handle_t)ch->context;

    return csi_mailbox_receive(handle, 1, msg, sizeof(message_t));
}

static void mailbox_event_cb(mailbox_handle_t handle, int32_t mailbox_id, uint32_t received_len, mailbox_event_e event)
{
    if (event == MAILBOX_EVENT_SEND_COMPLETE) {
        channel_event(&channel_mailbox, CHANNEL_WRITE_EVENT);
    } else if (event == MAILBOX_EVENT_RECEIVED) {
        channel_event(&channel_mailbox, CHANNEL_READ_EVENT);
    }
}

channel_t *yv_channel_mailbox_get(void)
{
    if (channel_mailbox.context == 0) {
        channel_init(&channel_mailbox);
        channel_mailbox.context = csi_mailbox_initialize(mailbox_event_cb);

        if (channel_mailbox.context != NULL) {
            channel_mailbox.put_message = channel_mailbox_message_put;
            channel_mailbox.get_message = channel_mailbox_message_get;
            return &channel_mailbox;
        }
    }

    return NULL;
}
