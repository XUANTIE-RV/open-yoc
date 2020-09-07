/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <ipc.h>
#include <csi_core.h>
#include <aos/kernel.h>
#include "avcodec/avcodec.h"
#include "avcodec/avcodec_all.h"
#include "icore/adicore_internal.h"
#include "adicore_cp.h"

#define TAG    "adicore_cp"

struct adicore_cp_priv {
#define MESSAGE_NUM 5
    ipc_t          *ipc;
    aos_queue_t    queue;
    uint8_t        qbuf[sizeof(message_t) * MESSAGE_NUM];
};

static int _icore_ad_open(icore_msg_t *msg)
{
    ad_conf_t ad_cnf;
    adicore_open_t *inp;
    ad_cls_t *ad = NULL;

    inp = icore_get_msg(msg, adicore_open_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    ad_conf_init(&ad_cnf);
    ad_cnf.sf             = inp->adi_cnf.sf;
    ad_cnf.block_align    = inp->adi_cnf.block_align;
    ad_cnf.bps            = inp->adi_cnf.bps;
    ad_cnf.extradata      = inp->adi_cnf.extradata;
    ad_cnf.extradata_size = inp->adi_cnf.extradata_size;
    csi_dcache_invalid_range((uint32_t*)ad_cnf.extradata, ad_cnf.extradata_size);
    ad = ad_open(inp->id, &ad_cnf);
    if (!ad) {
        LOGE(TAG, "ad open faild, codecid = %d", inp->id);
        return -1;
    }
    inp->ad = ad;
    inp->sf = ad->ash.sf;

    return 0;
}

static int _icore_ad_decode(icore_msg_t *msg)
{
    int rc = -1;
    adicore_decode_t *inp;
    ad_cls_t  *ad;
    avpacket_t pkt;
    avframe_t frame;
    int got_frame = 0;

    inp = icore_get_msg(msg, adicore_decode_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    ad = inp->ad;
    memset(&pkt, 0, sizeof(avpacket_t));
    memset(&frame, 0, sizeof(avframe_t));
    avframe_set_mempool(&frame, inp->frame.mpool, inp->frame.msize);
    pkt.size = inp->es_len;
    pkt.len  = inp->es_len;
    pkt.data = inp->es_data;
    if (ad) {
        csi_dcache_invalid_range((uint32_t*)pkt.data, pkt.len);
        rc = ad_decode(ad, &frame, &got_frame, (const avpacket_t*)&pkt);
        if ((rc >= 0) && got_frame) {
            inp->got_frame = got_frame;
            memcpy(&inp->frame, &frame, sizeof(avframe_t));
            csi_dcache_clean_invalid_range((uint32_t*)frame.data[0], frame.linesize[0]);
        }
    }

    return rc;
}

static int _icore_ad_reset(icore_msg_t *msg)
{
    int rc = -1;
    adicore_reset_t *inp;

    inp = icore_get_msg(msg, adicore_reset_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    if (inp->ad) {
        rc = ad_reset(inp->ad);
    }

    return rc;
}

static int _icore_ad_close(icore_msg_t *msg)
{
    int rc = -1;
    adicore_close_t *inp;

    inp = icore_get_msg(msg, adicore_close_t);
    CHECK_RET_TAG_WITH_RET(inp, -1);

    if (inp->ad) {
        rc = ad_close(inp->ad);
    }

    return rc;
}

typedef int (*icore_handler_t)(icore_msg_t *msg);
static struct {
    msgid_t          id;
    icore_handler_t  handler;
} g_handlers[] = {
    { ICORE_CMD_AD_OPEN, _icore_ad_open         },
    { ICORE_CMD_AD_DECODE, _icore_ad_decode       },
    { ICORE_CMD_AD_RESET, _icore_ad_reset       },
    { ICORE_CMD_AD_CLOSE, _icore_ad_close        },
};

static void _ipc_process(ipc_t *ipc, message_t *msg, void *arg)
{
    struct adicore_cp_priv *priv = arg;

    switch (msg->command) {
    case IPC_CMD_ADICORE: {
        aos_queue_send(&priv->queue, msg, sizeof(message_t));
    }
    break;
    default:
        return;
    }
}

static void _adicore_cp_task(void *arg)
{
    int i;
    uint32_t len;
    message_t msg;
    icore_msg_t *data;
    struct adicore_cp_priv *priv = arg;

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
 * @brief  init audio decoder of the cp
 * @return 0/-1
 */
int adicore_cp_init()
{
    static int inited = 0;

    if (!inited) {
        struct adicore_cp_priv *priv;

        priv = aos_zalloc(sizeof(struct adicore_cp_priv));
        CHECK_RET_TAG_WITH_RET(priv, -1);
        aos_queue_new(&priv->queue, priv->qbuf, sizeof(message_t) * MESSAGE_NUM, sizeof(message_t));
        priv->ipc = ipc_get(ADICORE_AP_IDX);
        ipc_add_service(priv->ipc, ADICORE_IPC_SERIVCE_ID, _ipc_process, priv);
        aos_task_new("ad_cp_task", _adicore_cp_task, (void *)priv, 96*1024);
        ad_register_all();
        inited = 1;
    }

    return inited ? 0 : -1;
}



