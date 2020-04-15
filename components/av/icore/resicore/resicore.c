/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "ipc.h"
#include <csi_core.h>
#include "avutil/mem_block.h"
#include "resicore.h"
#include "icore/resicore_internal.h"
#include "swresample/resample.h"

#define TAG                     "resicore_ap"
#define RES_MB_SIZE_DEFAULT     (1*1024)

struct resicore {
    uint32_t        irate;
    uint32_t        orate;
    uint8_t         bits;
    uint8_t         channels;

    void            *r;        // resx_t
    void            *priv;     // priv only for ap/cpu0
};

struct resicore_ap_priv {
    mblock_t        *in_mb;
    mblock_t        *out_mb;
};

static struct {
    ipc_t           *ipc;
    int             init;
} g_resicore;

static int _ipc_cmd_send(icore_msg_t *data, int sync)
{
    message_t msg;
    memset(&msg, 0, sizeof(message_t));

    msg.service_id  = RESICORE_IPC_SERIVCE_ID;
    msg.command     = IPC_CMD_RESICORE;
    msg.req_data        = data;
    msg.req_len         = ICORE_MSG_SIZE + data->size;
    msg.resp_data        = data;
    msg.resp_len         = ICORE_MSG_SIZE + data->size;
    msg.flag       |= sync;

    ipc_message_send(g_resicore.ipc, &msg, AOS_WAIT_FOREVER);

    return data->ret.code;
}

/**
 * @brief  new a resampler
 * @param  [in] irate    : input sample rate
 * @param  [in] orate    : output sample rate
 * @param  [in] channels : 1/2, mono/stereo, support mono now only
 * @param  [in] bits     : 16 only
 * @return NULL on error
 */
resicore_t *resicore_new(uint32_t irate, uint32_t orate, uint8_t channels, uint8_t bits)
{
    int rc;
    resicore_t *hdl               = NULL;
    struct resicore_ap_priv *priv = NULL;
    mblock_t *in_mb               = NULL, *out_mb = NULL;
    icore_msg_t *msg_new          = NULL;
    resicore_new_t *inp;

    CHECK_PARAM(irate && orate && (channels == 1) && bits, NULL);
    hdl = aos_zalloc(sizeof(resicore_t));
    CHECK_RET_TAG_WITH_RET(hdl, NULL);

    in_mb = mblock_new(RES_MB_SIZE_DEFAULT, 16);
    out_mb = mblock_new(RES_MB_SIZE_DEFAULT, 16);
    CHECK_RET_TAG_WITH_GOTO(in_mb && out_mb, err);

    msg_new = icore_msg_new(ICORE_CMD_RES_NEW, sizeof(resicore_new_t));
    CHECK_RET_TAG_WITH_GOTO(msg_new, err);

    priv = aos_zalloc(sizeof(struct resicore_ap_priv));
    CHECK_RET_TAG_WITH_GOTO(NULL != priv, err);

    inp = icore_get_msg(msg_new, resicore_new_t);
    inp->irate    = irate;
    inp->orate    = orate;
    inp->channels = channels;
    inp->bits     = bits;

    rc = _ipc_cmd_send(msg_new, MESSAGE_SYNC);
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);

    priv->in_mb   = in_mb;
    priv->out_mb  = out_mb;
    hdl->irate    = irate;
    hdl->orate    = orate;
    hdl->channels = channels;
    hdl->bits     = bits;
    hdl->r        = inp->r;
    hdl->priv     = priv;
    icore_msg_free(msg_new);

    return hdl;
err:
    mblock_free(in_mb);
    mblock_free(out_mb);
    icore_msg_free(msg_new);
    aos_free(hdl);
    aos_free(priv);
    return NULL;
}

/**
 * @brief  get the max out samples number per channel
 * @param  [in] irate
 * @param  [in] orate
 * @param  [in] nb_isamples : number of input samples available in one channel
 * @return -1 on error
 */
int resicore_get_osamples_max(uint32_t irate, uint32_t orate, size_t nb_isamples)
{
    return resx_get_osamples_max(irate, orate, nb_isamples);
}

/**
 * @brief  convert nb_samples from src to dst sample format
 * @param  [in] hdl
 * @param  [in] out
 * @param  [in] nb_osamples : amount of space available for output in samples per channel
 * @param  [in] in
 * @param  [in] nb_isamples : number of input samples available in one channel
 * @return number of samples output per channel, -1 on error
 */
int resicore_convert(resicore_t *hdl, void **out, size_t nb_osamples, const void **in, size_t nb_isamples)
{
    int i, rc = -1;
    size_t isize, osize;
    icore_msg_t *msg;
    resicore_convert_t *inp;
    struct resicore_ap_priv *priv;

    CHECK_PARAM(hdl && in && out && nb_isamples && nb_osamples, -1);
    for (i = 0; i < hdl->channels; i++) {
        if (!(in[i] && out[i])) {
            LOGE(TAG, "out/in params is err.");
            return -1;
        }
    }

    priv = hdl->priv;
    msg  = icore_msg_new(ICORE_CMD_RES_CONVERT, sizeof(resicore_convert_t));
    CHECK_RET_TAG_WITH_RET(msg, -1);

    isize = nb_isamples * hdl->bits / 8;
    rc = mblock_grow(priv->in_mb, ICORE_ALIGN_BUFZ(isize));
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);
    osize = nb_osamples * hdl->bits / 8;
    rc = mblock_grow(priv->out_mb, ICORE_ALIGN_BUFZ(osize));
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);

    inp = icore_get_msg(msg, resicore_convert_t);
    inp->r           = hdl->r;
    inp->nb_isamples = nb_isamples;
    inp->nb_osamples = nb_osamples;
    inp->in          = priv->in_mb->data;
    inp->out         = priv->out_mb->data;
    memcpy((void*)inp->in, (const void*)in[0], isize);
    csi_dcache_clean_invalid_range((uint32_t*)inp->in, isize);

    rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    if (rc > 0) {
        osize = rc * hdl->bits / 8;
        csi_dcache_invalid_range((uint32_t*)inp->out, osize);
        memcpy((void*)out[0], (const void*)inp->out, osize);
    }
    icore_msg_free(msg);

    return rc;
err:
    icore_msg_free(msg);
    return -1;
}

/**
 * @brief  free the rualizer
 * @param  [in] hdl
 * @return 0/-1
 */
int resicore_free(resicore_t *hdl)
{
    int rc = -1;
    icore_msg_t *msg;
    resicore_free_t *inp;
    struct resicore_ap_priv *priv;

    CHECK_PARAM(hdl, -1);
    priv = hdl->priv;
    msg  = icore_msg_new(ICORE_CMD_RES_FREE, sizeof(resicore_free_t));
    CHECK_RET_TAG_WITH_RET(msg, -1);
    inp = icore_get_msg(msg, resicore_free_t);
    inp->r = hdl->r;

    rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    icore_msg_free(msg);
    mblock_free(priv->in_mb);
    mblock_free(priv->out_mb);
    aos_free(hdl->priv);
    aos_free(hdl);

    return rc;
}

/**
 * @brief  init the icore resample
 * @return 0/-1
 */
int resicore_init()
{
    if (!g_resicore.init) {
        g_resicore.ipc = ipc_get(RESICORE_CP_IDX);
        CHECK_RET_TAG_WITH_RET(g_resicore.ipc, -1);
        ipc_add_service(g_resicore.ipc, RESICORE_IPC_SERIVCE_ID, NULL, NULL);

        g_resicore.init = 1;
    }

    return g_resicore.init ? 0 : -1;
}



