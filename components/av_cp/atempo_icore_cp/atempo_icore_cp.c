/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <ipc.h>
#include <csi_core.h>
#include <aos/kernel.h>
#include "atempo/atempo.h"
#include "atempo/atempo_all.h"
#include "icore/atempo_icore_internal.h"
#include "atempo_icore_cp.h"

#define TAG    "atempo_icore_cp"

struct atempo_icore_cp_priv {
#define MESSAGE_NUM  (5)
    ipc_t            *ipc;
    aos_queue_t      queue;
    uint8_t          qbuf[sizeof(message_t) * MESSAGE_NUM];
};

static int _icore_atempo_new(icore_msg_t *msg)
{
    atempo_t *atempo;
    atempo_icore_new_t *inp;

    inp = icore_get_msg(msg, atempo_icore_new_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    atempo = atempo_new(inp->rate);
    if (atempo == NULL) {
        LOGE(TAG, "atempo new faild, rate = %u", inp->rate);
        return -1;
    }

    inp->atempo = atempo;
    return 0;
}

static int _icore_atempo_write(icore_msg_t *msg)
{
    int rc = -1;
    size_t size;
    atempo_t *atempo;
    atempo_icore_write_t *inp;

    inp = icore_get_msg(msg, atempo_icore_write_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    atempo = inp->atempo;
    if (atempo) {
        size = inp->nb_samples * sizeof(int16_t);
        csi_dcache_invalid_range((uint32_t*)inp->in, size);
        rc = atempo_write(atempo, inp->in, inp->nb_samples);
    }

    return rc;
}

static int _icore_atempo_read(icore_msg_t *msg)
{
    int rc = -1;
    atempo_t *atempo;
    atempo_icore_read_t *inp;

    inp = icore_get_msg(msg, atempo_icore_read_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    atempo = inp->atempo;
    if (atempo) {
        rc = atempo_read(atempo, inp->out, inp->nb_samples);
        if (rc > 0) {
            csi_dcache_clean_invalid_range((uint32_t*)inp->out, rc * sizeof(int16_t));
        }
    }

    return rc;
}

static int _icore_atempo_set_speed(icore_msg_t *msg)
{
    int rc = -1;
    atempo_t *atempo;
    atempo_icore_set_speed_t *inp;

    inp = icore_get_msg(msg, atempo_icore_set_speed_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    atempo = inp->atempo;
    if (atempo) {
        rc = atempo_set_speed(atempo, inp->speed);
    }

    return rc;
}

static int _icore_atempo_flush(icore_msg_t *msg)
{
    int rc = -1;
    atempo_t *atempo;
    atempo_icore_flush_t *inp;

    inp = icore_get_msg(msg, atempo_icore_flush_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    atempo = inp->atempo;
    if (atempo) {
        rc = atempo_flush(atempo);
    }

    return rc;
}

static int _icore_atempo_free(icore_msg_t *msg)
{
    int rc = -1;
    atempo_t *atempo;
    atempo_icore_free_t *inp;

    inp = icore_get_msg(msg, atempo_icore_free_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    atempo = inp->atempo;
    if (atempo) {
        rc = atempo_free(atempo);
    }

    return rc;
}

typedef int (*icore_handler_t)(icore_msg_t *msg);
static struct {
    msgid_t          id;
    icore_handler_t  handler;
} g_handlers[] = {
    { ICORE_CMD_ATEMPO_NEW, _icore_atempo_new         },
    { ICORE_CMD_ATEMPO_WRITE, _icore_atempo_write         },
    { ICORE_CMD_ATEMPO_READ, _icore_atempo_read         },
    { ICORE_CMD_ATEMPO_SET_SPEED, _icore_atempo_set_speed     },
    { ICORE_CMD_ATEMPO_FLUSH, _icore_atempo_flush         },
    { ICORE_CMD_ATEMPO_FREE, _icore_atempo_free        },
};

static void _ipc_set_speed(ipc_t *ipc, message_t *msg, void *arg)
{
    struct atempo_icore_cp_priv *priv = arg;

    switch (msg->command) {
    case IPC_CMD_ATEMPO_ICORE: {
        aos_queue_send(&priv->queue, msg, sizeof(message_t));
    }
    break;
    default:
        return;
    }
}

static void _atempo_icore_cp_task(void *arg)
{
    int i;
    uint32_t len;
    message_t msg;
    icore_msg_t *data;
    struct atempo_icore_cp_priv *priv = arg;

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
 * @brief  init audio atempoer of the cp
 * @return 0/-1
 */
int atempo_icore_cp_init()
{
    static int inited = 0;

    if (!inited) {
        struct atempo_icore_cp_priv *priv;

        priv = aos_zalloc(sizeof(struct atempo_icore_cp_priv));
        CHECK_RET_TAG_WITH_RET(priv, -1);
        aos_queue_new(&priv->queue, priv->qbuf, sizeof(message_t) * MESSAGE_NUM, sizeof(message_t));
        priv->ipc = ipc_get(ATEMPO_ICORE_AP_IDX);
        ipc_add_service(priv->ipc, ATEMPO_ICORE_IPC_SERIVCE_ID, _ipc_set_speed, priv);
        atempo_register();
        aos_task_new("atempo_cp_task", _atempo_icore_cp_task, (void *)priv, 4*1024);
        inited = 1;
    }

    return inited ? 0 : -1;
}



