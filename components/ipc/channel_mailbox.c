/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <string.h>
#include <aos/debug.h>
#include "internal.h"
#ifdef CONFIG_CSI_V2
#include <drv/mbox.h>
#else
#include <drv/mailbox.h>
#endif
#define TAG "MAILBOX"

static slist_t channel_list;
#define EVENT_WRITE (0x01)

#ifdef CONFIG_CSI_V2
extern uint32_t soc_get_cpu_id(void);
static int cpuid_to_mb_chid(int cpu_id)
{
    switch(soc_get_cpu_id()) {
        case 0:
            return cpu_id - 1;
        case 1:
            if (cpu_id == 2) {
                return cpu_id - 1;
            }
            return cpu_id;
        case 2:
            return cpu_id;
        default:
            break;
    }

    return 0;
}

static int mb_chid_to_cpuid(int ch_id)
{
    switch(soc_get_cpu_id()) {
        case 0:
            return ch_id + 1;
        case 1:
            if (ch_id == 1) {
                return ch_id + 1;
            }
            return ch_id;
        case 2:
            return ch_id;
        default:
            break;
    }

    return 0;
}

static int32_t csi_mailbox_send(csi_mbox_t *mbox, uint32_t cpu_id, const void *data, uint32_t size)
{
    uint32_t channel_id = cpuid_to_mb_chid(cpu_id);
    return csi_mbox_send(mbox, channel_id, data, size);
}

static int32_t csi_mailbox_receive(csi_mbox_t *mbox, uint32_t cpu_id, void *data, uint32_t size)
{
    uint32_t channel_id = cpuid_to_mb_chid(cpu_id);
    return csi_mbox_receive(mbox, channel_id, data, size);
}
#endif

int channel_put_message(channel_t *ch, void *msg, int len, uint32_t timeout_ms)
{
#ifdef CONFIG_CSI_V2
    csi_mbox_t *handle = (csi_mbox_t *)ch->context;
#else
    mailbox_handle_t handle = (mailbox_handle_t)ch->context;
#endif
    csi_mailbox_send(handle, ch->des_id, msg, len);
    return 0;
}

int channel_get_message(channel_t *ch, void *msg, int len, uint32_t timeout_ms)
{
#ifdef CONFIG_CSI_V2
    csi_mbox_t *handle = (csi_mbox_t *)ch->context;
#else
    mailbox_handle_t handle = (mailbox_handle_t)ch->context;
#endif
    int ret = csi_mailbox_receive(handle, ch->des_id, msg, len);
    return ret;
}

static void channel_event(channel_t *ch, int event)
{
    if (ch->cb)
        ch->cb(event, ch->priv);
}

#ifdef CONFIG_CSI_V2
static void mailbox_event_cb(csi_mbox_t *mbox, csi_mbox_event_t event, uint32_t mailbox_id, uint32_t received_len, void *arg)
#else
static void mailbox_event_cb(mailbox_handle_t handle, int32_t mailbox_id, uint32_t received_len, mailbox_event_e event)
#endif
{
    channel_t *ch = NULL;
    channel_t *it;

#ifdef CONFIG_CSI_V2
    slist_for_each_entry (&channel_list, it, channel_t, next) {
        if (it->des_id == mb_chid_to_cpuid(mailbox_id)) {
            ch = it;
            break;
        }
    }

    if (ch) {
        if (event == MBOX_EVENT_SEND_COMPLETE) {
            channel_event(ch, CHANNEL_WRITE_EVENT);
        } else if (event == MBOX_EVENT_RECEIVED) {
            channel_event(ch, CHANNEL_READ_EVENT);
        }
    }
#else
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
#endif

}

static void *get_mailbox()
{
#ifdef CONFIG_CSI_V2
    csi_error_t ret;
    static csi_mbox_t handle;

    if (handle.callback == NULL) {
        ret = csi_mbox_init(&handle, 0);
        if (ret != CSI_OK) {
            printf("ERR->csi_mbox_init failed\n");
        }

        ret = csi_mbox_attach_callback(&handle, mailbox_event_cb, NULL);

        if (ret != CSI_OK) {
            printf("ERR->csi_mbox_attach_callback failed\n");
        }

        aos_check(ret == CSI_OK, NULL);
    }
    return &handle;

#else
    static mailbox_handle_t handle = NULL;

    if (handle == NULL) {
        handle = csi_mailbox_initialize(mailbox_event_cb);
        aos_check(handle, NULL);
    }
    return handle;
#endif
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
#ifdef CONFIG_CSI_V2
    csi_error_t ret;
    csi_mbox_t *handle = (csi_mbox_t *)ch->context;

    if (state) {
        csi_mbox_detach_callback(handle);
        csi_mbox_uninit(handle);
    } else {
        ret = csi_mbox_init(handle, 0);

        if (ret != CSI_OK) {
            printf("ERR->csi_mbox_init failed\n");
            return -1;
        }

        ret = csi_mbox_attach_callback(handle, mailbox_event_cb, NULL);

        if (ret != CSI_OK) {
            printf("ERR->csi_mbox_attach_callback failed\n");
            return -1;
        }
    }

#else
    mailbox_handle_t handle = (mailbox_handle_t)ch->context;

    if (state) {
        csi_mailbox_uninitialize(handle);
        ch->context = NULL;
    } else {
        ch->context = csi_mailbox_initialize(mailbox_event_cb);
    }

#endif
    return 0;
}
