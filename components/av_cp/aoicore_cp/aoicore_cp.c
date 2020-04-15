/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <ipc.h>
#include <csi_core.h>
#include <aos/kernel.h>
#include "output/output.h"
#include "output/output_all.h"
#include "icore/aoicore_internal.h"
#include "aoicore_cp.h"
#include <alsa/mixer.h>

#define TAG    "aoicore_cp"

struct aoicore_cp_priv {
#define MESSAGE_NUM  (5)
    ipc_t            *ipc;
    aos_queue_t      queue;
    uint8_t          qbuf[sizeof(message_t) * MESSAGE_NUM];
};

static struct {
    aos_mixer_t      *mixer;
    aos_mixer_elem_t *elem;
} g_media;

static int _icore_ao_open(icore_msg_t *msg)
{
    aoh_t aoh;
    char *name = NULL;
    aoicore_open_t *inp;
    ao_cls_t *ao = NULL;

    inp = icore_get_msg(msg, aoicore_open_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    if (inp->ash.id == ICORE_AO_ID_ALSA) {
        name = "alsa";
    } else {
        return -1;
    }

    memset(&aoh, 0, sizeof(aoh_t));
    aoh.name = name;
    aoh.sf   = inp->ash.sf;
    ao = ao_open(&aoh);
    if (ao == NULL) {
        LOGE(TAG, "ao open faild, codecid = %d", inp->ash.id);
        return -1;
    }

    inp->ao = ao;
    return 0;
}

static int _icore_ao_start(icore_msg_t *msg)
{
    int rc = -1;
    aoicore_start_t *inp;

    inp = icore_get_msg(msg, aoicore_start_t);
    CHECK_RET_TAG_WITH_RET(NULL != inp, -1);

    if (inp->ao) {
        rc = ao_start(inp->ao);
    }

    return rc;
}

static int _icore_ao_drain(icore_msg_t *msg)
{
    int rc = -1;
    aoicore_drain_t *inp;

    inp = icore_get_msg(msg, aoicore_drain_t);
    CHECK_RET_TAG_WITH_RET(NULL != inp, -1);

    if (inp->ao) {
        rc = ao_drain(inp->ao);
    }

    return rc;
}

static int _icore_ao_stop(icore_msg_t *msg)
{
    int rc = -1;
    aoicore_stop_t *inp;

    inp = icore_get_msg(msg, aoicore_stop_t);
    CHECK_RET_TAG_WITH_RET(NULL != inp, -1);

    if (inp->ao) {
        rc = ao_stop(inp->ao);
    }

    return rc;
}

static int _icore_ao_write(icore_msg_t *msg)
{
    int rc = -1;
    aoicore_write_t *inp;
    ao_cls_t  *ao;

    inp = icore_get_msg(msg, aoicore_write_t);
    CHECK_RET_TAG_WITH_RET(NULL != inp, -1);

    ao = inp->ao;
    if (ao) {
        csi_dcache_invalid_range((uint32_t*)inp->buf, inp->count);
        rc = ao_write(ao, (const uint8_t*)inp->buf, inp->count);
    }

    return rc;
}

static int _icore_ao_close(icore_msg_t *msg)
{
    int rc = -1;
    aoicore_close_t *inp;

    inp = icore_get_msg(msg, aoicore_close_t);
    CHECK_RET_TAG_WITH_RET(NULL != inp, -1);

    if (inp->ao) {
        rc = ao_close(inp->ao);
    }

    return rc;
}

static int _icore_vol_set(icore_msg_t *msg)
{
    int rc = -1, vol;
    volicore_set_t *inp;

    inp = icore_get_msg(msg, volicore_set_t);
    CHECK_RET_TAG_WITH_RET(NULL != inp, -1);
    vol = inp->vol;

    if (vol >= 0 && vol <= 100) {
        rc = aos_mixer_selem_set_playback_volume_all(g_media.elem, vol);
    }

    return rc;
}

typedef int (*icore_handler_t)(icore_msg_t *msg);
static struct {
    msgid_t          id;
    icore_handler_t  handler;
} g_handlers[] = {
    { ICORE_CMD_AO_OPEN             , _icore_ao_open         },
    { ICORE_CMD_AO_START            , _icore_ao_start        },
    { ICORE_CMD_AO_WRITE            , _icore_ao_write        },
    { ICORE_CMD_AO_DRAIN            , _icore_ao_drain        },
    { ICORE_CMD_AO_STOP             , _icore_ao_stop         },
    { ICORE_CMD_AO_CLOSE            , _icore_ao_close        },

    { ICORE_CMD_VOL_SET             , _icore_vol_set         },
};

static void _ipc_process(ipc_t *ipc, message_t *msg, void *arg)
{
    struct aoicore_cp_priv *priv = arg;

    switch (msg->command) {
    case IPC_CMD_AOICORE: {
        aos_queue_send(&priv->queue, msg, sizeof(message_t));
    }
    break;
    default:
        return;
    }
}

static void _aoicore_cp_task(void *arg)
{
    int i;
    uint32_t len;
    message_t msg;
    icore_msg_t *data;
    struct aoicore_cp_priv *priv = arg;

    for (;;) {
        aos_queue_recv(&priv->queue, AOS_WAIT_FOREVER, &msg, &len);
        data = (icore_msg_t*)msg.req_data;

        if (data && (msg.req_len == (ICORE_MSG_SIZE + data->size))) {
            data->ret.code = -1;
            for (i = 0; i < ARRAY_SIZE(g_handlers); i++) {
                if (g_handlers[i].id == data->id) {
                    data->ret.code = g_handlers[i].handler(data);
                    break;
                }
            }
        }

        if (msg.flag & MESSAGE_SYNC) {
            memcpy(msg.resp_data, msg.req_data, msg.req_len);
            ipc_message_ack(priv->ipc, &msg, AOS_WAIT_FOREVER);
        }
    }
}

static int _mixer_init(void)
{
    int rc = 0;
    aos_mixer_open(&g_media.mixer, 0);
    aos_mixer_attach(g_media.mixer, "card0");
    aos_mixer_load(g_media.mixer);
    g_media.elem = aos_mixer_first_elem(g_media.mixer);

    if (g_media.elem == NULL) {
        rc = -1;
        LOGE(TAG, "elem fine NULL");
    }

    return rc;
}

/**
 * @brief  init audio output of the cp
 * @return 0/-1
 */
int aoicore_cp_init()
{
    static int inited = 0;

    if (!inited) {
        int rc;
        struct aoicore_cp_priv *priv;

        rc = _mixer_init();
        CHECK_RET_TAG_WITH_RET(rc == 0, -1);

        priv = aos_zalloc(sizeof(struct aoicore_cp_priv));
        CHECK_RET_TAG_WITH_RET(NULL != priv, -1);
        aos_queue_new(&priv->queue, priv->qbuf, sizeof(message_t) * MESSAGE_NUM, sizeof(message_t));
        priv->ipc = ipc_get(AOICORE_AP_IDX);
        ipc_add_service(priv->ipc, AOICORE_IPC_SERIVCE_ID, _ipc_process, priv);
        ao_register_alsa();
        aos_task_new("ao_cp_task", _aoicore_cp_task, (void *)priv, 4*1024);
        inited = 1;
    }

    return inited ? 0 : -1;
}



