/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <ipc.h>
#include <csi_core.h>
#include <aos/kernel.h>
#include "aef/eq.h"
#include "aef/eq_all.h"
#include "icore/eqicore_internal.h"
#include "eqicore_cp.h"

#define TAG    "eqicore_cp"

struct eqicore_cp_priv {
#define MESSAGE_NUM  (5)
    ipc_t            *ipc;
    aos_queue_t      queue;
    uint8_t          qbuf[sizeof(message_t) * MESSAGE_NUM];
};

static int _icore_eqx_new(icore_msg_t *msg)
{
    eqx_t *eq;
    eqicore_new_t *inp;

    inp = icore_get_msg(msg, eqicore_new_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    eq = eqx_new(inp->rate, inp->eq_segments);
    if (eq == NULL) {
        LOGE(TAG, "eq new faild, rate = %u, seg = %u", inp->rate, inp->eq_segments);
        return -1;
    }

    inp->eq = eq;
    return 0;
}

static int _icore_eqx_set_enable(icore_msg_t *msg)
{
    int rc = -1;
    eqx_t *eq;
    eqicore_seten_t *inp;

    inp = icore_get_msg(msg, eqicore_seten_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    eq = inp->eq;
    if (eq) {
        rc = eqx_set_enable(eq, inp->enable);
    }

    return rc;
}

static int _icore_eqx_process(icore_msg_t *msg)
{
    int rc = -1;
    size_t size;
    eqx_t *eq;
    eqicore_process_t *inp;

    inp = icore_get_msg(msg, eqicore_process_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    eq = inp->eq;
    if (eq) {
        size = inp->nb_samples * sizeof(int16_t);
        csi_dcache_invalid_range((uint32_t*)inp->in, size);
        rc = eqx_process(eq, inp->in, inp->out, inp->nb_samples);
        if (rc == 0) {
            csi_dcache_clean_invalid_range((uint32_t*)inp->out, size);
        }
    }

    return rc;
}

static int _icore_eqx_set_param(icore_msg_t *msg)
{
    int rc = -1;
    eqx_t *eq;
    eqicore_setpa_t *inp;

    inp = icore_get_msg(msg, eqicore_setpa_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    eq = inp->eq;
    if (eq) {
        rc = eqx_set_param(eq, inp->segid, &inp->param);
    }

    return rc;
}

static int _icore_eqx_free(icore_msg_t *msg)
{
    int rc = -1;
    eqx_t *eq;
    eqicore_free_t *inp;

    inp = icore_get_msg(msg, eqicore_free_t);
    CHECK_RET_TAG_WITH_RET(NULL != inp, -1);

    eq = inp->eq;
    if (eq) {
        rc = eqx_free(eq);
    }

    return rc;
}

typedef int (*icore_handler_t)(icore_msg_t *msg);
static struct {
    msgid_t          id;
    icore_handler_t  handler;
} g_handlers[] = {
    { ICORE_CMD_EQ_NEW, _icore_eqx_new         },
    { ICORE_CMD_EQ_SET_ENABLE, _icore_eqx_set_enable  },
    { ICORE_CMD_EQ_SET_PARAM, _icore_eqx_set_param   },
    { ICORE_CMD_EQ_PROCESS, _icore_eqx_process     },
    { ICORE_CMD_EQ_FREE, _icore_eqx_free        },
};

static void _ipc_process(ipc_t *ipc, message_t *msg, void *arg)
{
    struct eqicore_cp_priv *priv = arg;

    switch (msg->command) {
    case IPC_CMD_EQICORE: {
        aos_queue_send(&priv->queue, msg, sizeof(message_t));
    }
    break;
    default:
        return;
    }
}

static void _eqicore_cp_task(void *arg)
{
    int i;
    uint32_t len;
    message_t msg;
    icore_msg_t *data;
    struct eqicore_cp_priv *priv = arg;

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
 * @brief  init equalizer of the cp
 * @return 0/-1
 */
int eqicore_cp_init()
{
    static int inited = 0;

    if (!inited) {
        struct eqicore_cp_priv *priv;

        priv = aos_zalloc(sizeof(struct eqicore_cp_priv));
        CHECK_RET_TAG_WITH_RET(priv, -1);
        aos_queue_new(&priv->queue, priv->qbuf, sizeof(message_t) * MESSAGE_NUM, sizeof(message_t));
        priv->ipc = ipc_get(EQICORE_AP_IDX);
        ipc_add_service(priv->ipc, EQICORE_IPC_SERIVCE_ID, _ipc_process, priv);
        eqx_register();
        aos_task_new("eq_cp_task", _eqicore_cp_task, (void *)priv, 4*1024);
        inited = 1;
    }

    return inited ? 0 : -1;
}



