/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>
#include <aos/log.h>
#include <aos/debug.h>
#include "internal.h"
#include <drv/mailbox.h>

#define TAG "MAILBOX"

static slist_t channel_list;

#define EVENT_WRITE (0x01)
int channel_put_message(channel_t *ch, void *msg, int len, uint32_t timeout_ms)
{
    mailbox_handle_t handle = (mailbox_handle_t)ch->context;

    csi_mailbox_send(handle, ch->des_id, msg, len);

    return 0;
}

int channel_get_message(channel_t *ch, void *msg, int len, uint32_t timeout_ms)
{
    mailbox_handle_t handle = (mailbox_handle_t)ch->context;

    return csi_mailbox_receive(handle, ch->des_id, msg, len);
}

static void channel_event(channel_t *ch, int event)
{
    if (ch->cb)
        ch->cb(event, ch->priv);
}

static void mailbox_event_cb(mailbox_handle_t handle, int32_t mailbox_id, uint32_t received_len, mailbox_event_e event)
{
    channel_t *ch = NULL;
    channel_t *it;

    slist_for_each_entry (&channel_list, it, channel_t, next) {
        if (it->des_id == mailbox_id) {
            ch = it;
            break;
        }
    }

    if (ch) {
        if (event == MAILBOX_EVENT_SEND_COMPLETE) {
            channel_event(ch, CHANNEL_WRITE_EVENT);
        } else if (event == MAILBOX_EVENT_RECEIVED) {
            channel_event(ch, CHANNEL_READ_EVENT);
        }
    }

}

static void *get_mailbox()
{
    static mailbox_handle_t handle = NULL;

    if (handle == NULL) {
        handle = csi_mailbox_initialize(mailbox_event_cb);

        aos_check(handle, EIO);
    }

    return handle;
}

channel_t *channel_mailbox_get(int cpu_id, channel_cb cb, void *priv)
{
    channel_t *ch;

    slist_for_each_entry (&channel_list, ch, channel_t, next) {
        if (ch->des_id == cpu_id) {
            return ch;
        }
    }

    ch  = calloc(1, sizeof(channel_t));

    if (ch == NULL) {
        return NULL;
    }

    ch->context = get_mailbox();

    if (ch->context != NULL) {
        slist_add_tail(&ch->next, &channel_list);
        ch->des_id = cpu_id;
        ch->cb = cb;
        ch->priv = priv;
        return ch;
    }

    free(ch);

    return NULL;
}

int channel_mailbox_lpm(channel_t *ch, int state)
{
    mailbox_handle_t handle = (mailbox_handle_t)ch->context;

    if (state) {
        csi_mailbox_uninitialize(handle);
        ch->context = NULL;
    } else {
        ch->context = csi_mailbox_initialize(mailbox_event_cb);
    }
    
    return 0;
}
