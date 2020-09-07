/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <ipc.h>
#include <csi_core.h>
#include <aos/kernel.h>
#include "swresample/resample.h"
#include "swresample/resample_all.h"
#include "icore/resicore_internal.h"
#include "resicore_cp.h"

#define TAG    "resicore_cp"

struct resicore_cp_priv {
#define MESSAGE_NUM  (5)
    ipc_t            *ipc;
    aos_queue_t      queue;
    uint8_t          qbuf[sizeof(message_t) * MESSAGE_NUM];
};

static int _icore_resx_new(icore_msg_t *msg)
{
    resx_t *r;
    resicore_new_t *inp;

    inp = icore_get_msg(msg, resicore_new_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    r = resx_new(inp->irate, inp->orate, inp->channels, inp->bits);
    if (r == NULL) {
        LOGE(TAG, "res new faild, irate = %u, orate = %u, ch = %u, bits = %u", inp->irate, inp->orate, inp->channels, inp->bits);
        return -1;
    }

    inp->r = r;
    return 0;
}

static int _icore_resx_convert(icore_msg_t *msg)
{
    int rc = -1;
    size_t size;
    resx_t *r;
    resicore_convert_t *inp;

    inp = icore_get_msg(msg, resicore_convert_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    r = inp->r;
    if (r) {
        size = inp->nb_isamples * (r->bits / 8);
        csi_dcache_invalid_range((uint32_t*)inp->in, size);
        rc = resx_convert(r, &inp->out, inp->nb_osamples, (const void **)&inp->in, inp->nb_isamples);
        if (rc > 0) {
            size = rc * (r->bits / 8);
            csi_dcache_clean_invalid_range((uint32_t*)inp->out, size);
        }
    }

    return rc;
}

static int _icore_resx_free(icore_msg_t *msg)
{
    int rc = -1;
    resx_t *r;
    resicore_free_t *inp;

    inp = icore_get_msg(msg, resicore_free_t);
    CHECK_RET_TAG_WITH_RET(NULL != inp, -1);

    r = inp->r;
    if (r) {
        resx_free(r);
        rc = 0;
    }

    return rc;
}

typedef int (*icore_handler_t)(icore_msg_t *msg);
static struct {
    msgid_t          id;
    icore_handler_t  handler;
} g_handlers[] = {
    { ICORE_CMD_RES_NEW, _icore_resx_new         },
    { ICORE_CMD_RES_CONVERT, _icore_resx_convert     },
    { ICORE_CMD_RES_FREE, _icore_resx_free        },
};

static void _ipc_process(ipc_t *ipc, message_t *msg, void *arg)
{
    struct resicore_cp_priv *priv = arg;

    switch (msg->command) {
    case IPC_CMD_RESICORE: {
        aos_queue_send(&priv->queue, msg, sizeof(message_t));
    }
    break;
    default:
        return;
    }
}

static void _resicore_cp_task(void *arg)
{
    int i;
    uint32_t len;
    message_t msg;
    icore_msg_t *data;
    struct resicore_cp_priv *priv = arg;

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

/**
 * @brief  init resampler of the cp
 * @return 0/-1
 */
int resicore_cp_init()
{
    static int inited = 0;

    if (!inited) {
        struct resicore_cp_priv *priv;

        priv = aos_zalloc(sizeof(struct resicore_cp_priv));
        CHECK_RET_TAG_WITH_RET(priv, -1);
        aos_queue_new(&priv->queue, priv->qbuf, sizeof(message_t) * MESSAGE_NUM, sizeof(message_t));
        priv->ipc = ipc_get(RESICORE_AP_IDX);
        ipc_add_service(priv->ipc, RESICORE_IPC_SERIVCE_ID, _ipc_process, priv);
        resample_register();
        aos_task_new("res_cp_task", _resicore_cp_task, (void *)priv, 8*1024);
        inited = 1;
    }

    return inited ? 0 : -1;
}



