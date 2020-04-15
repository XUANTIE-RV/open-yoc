/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "ipc.h"
#include <csi_core.h>
#include "avutil/mem_block.h"
#include "ffticore.h"
#include "icore/ffticore_internal.h"

#define TAG                     "ffticore_ap"
#define FFT_MB_SIZE_DEFAULT     (2*1024)

struct ffticore {
    size_t          size;      // N

    void            *fft;       // fftx_t
    void            *priv;     // priv only for ap/cpu0
};

struct ffticore_ap_priv {
    mblock_t        *in_mb;
    mblock_t        *out_mb;
};

static struct {
    ipc_t           *ipc;
    int             init;
} g_ffticore;

static int _ipc_cmd_send(icore_msg_t *data, int sync)
{
    message_t msg;
    memset(&msg, 0, sizeof(message_t));

    msg.service_id      = FFTICORE_IPC_SERIVCE_ID;
    msg.command         = IPC_CMD_FFTICORE;
    msg.req_data        = data;
    msg.req_len         = ICORE_MSG_SIZE + data->size;
    msg.resp_data       = data;
    msg.resp_len        = ICORE_MSG_SIZE + data->size;
    msg.flag           |= sync;

    ipc_message_send(g_ffticore.ipc, &msg, AOS_WAIT_FOREVER);

    return data->ret.code;
}

/**
 * @brief  alloc a inter-core fft
 * @param  [in] size : N of the fft
 * @return NULL on error
 */
ffticore_t* ffticore_new(size_t size)
{
    int rc;
    ffticore_t *hdl               = NULL;
    struct ffticore_ap_priv *priv = NULL;
    mblock_t *in_mb               = NULL, *out_mb = NULL;
    icore_msg_t *msg_new          = NULL;
    ffticore_new_t *inp;

    CHECK_PARAM(size, NULL);
    hdl = aos_zalloc(sizeof(ffticore_t));
    CHECK_RET_TAG_WITH_RET(hdl, NULL);

    in_mb = mblock_new(FFT_MB_SIZE_DEFAULT, 16);
    out_mb = mblock_new(FFT_MB_SIZE_DEFAULT, 16);
    CHECK_RET_TAG_WITH_GOTO(in_mb && out_mb, err);

    msg_new = icore_msg_new(ICORE_CMD_FFT_NEW, sizeof(ffticore_new_t));
    CHECK_RET_TAG_WITH_GOTO(msg_new, err);

    priv = aos_zalloc(sizeof(struct ffticore_ap_priv));
    CHECK_RET_TAG_WITH_GOTO(priv, err);

    inp = icore_get_msg(msg_new, ffticore_new_t);
    inp->size = size;

    rc = _ipc_cmd_send(msg_new, MESSAGE_SYNC);
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);

    priv->in_mb  = in_mb;
    priv->out_mb = out_mb;
    hdl->fft     = inp->fft;
    hdl->size    = size;
    hdl->priv    = priv;
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
 * @brief  fft forward
 * @param  [in] hdl
 * @param  [in] in
 * @param  [in] out
 * @return 0/-1
 */
int ffticore_forward(ffticore_t *hdl, const int16_t *in, int16_t *out)
{
    int rc = -1;
    size_t size;
    icore_msg_t *msg;
    ffticore_forward_t *inp;
    struct ffticore_ap_priv *priv;

    CHECK_PARAM(hdl && in && out, -1);
    priv = hdl->priv;
    msg  = icore_msg_new(ICORE_CMD_FFT_FORWARD, sizeof(ffticore_forward_t));
    CHECK_RET_TAG_WITH_RET(msg, -1);

    size = hdl->size * sizeof(int16_t);
    rc = mblock_grow(priv->in_mb, ICORE_ALIGN_BUFZ(size));
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);
    rc = mblock_grow(priv->out_mb, ICORE_ALIGN_BUFZ(size));
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);

    inp = icore_get_msg(msg, ffticore_forward_t);
    inp->fft = hdl->fft;
    inp->in  = priv->in_mb->data;
    inp->out = priv->out_mb->data;
    memcpy((void*)inp->in, (const void*)in, size);
    csi_dcache_invalid_range((uint32_t*)inp->out, size);
    csi_dcache_clean_invalid_range((uint32_t*)inp->in, size);

    rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    if (rc == 0) {
        memcpy((void*)out, (const void*)inp->out, size);
    }
    icore_msg_free(msg);

    return rc;
err:
    icore_msg_free(msg);
    return -1;
}

/**
 * @brief  fft backward
 * @param  [in] hdl
 * @param  [in] in
 * @param  [in] out
 * @return 0/-1
 */
int ffticore_backward(ffticore_t *hdl, const int16_t *in, int16_t *out)
{
    int rc = -1;
    size_t size;
    icore_msg_t *msg;
    ffticore_backward_t *inp;
    struct ffticore_ap_priv *priv;

    CHECK_PARAM(hdl && in && out, -1);
    priv = hdl->priv;
    msg  = icore_msg_new(ICORE_CMD_FFT_BACKWARD, sizeof(ffticore_backward_t));
    CHECK_RET_TAG_WITH_RET(msg, -1);

    size = hdl->size * sizeof(int16_t);
    rc = mblock_grow(priv->in_mb, ICORE_ALIGN_BUFZ(size));
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);
    rc = mblock_grow(priv->out_mb, ICORE_ALIGN_BUFZ(size));
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);

    inp = icore_get_msg(msg, ffticore_backward_t);
    inp->fft = hdl->fft;
    inp->in  = priv->in_mb->data;
    inp->out = priv->out_mb->data;
    memcpy((void*)inp->in, (const void*)in, size);
    csi_dcache_invalid_range((uint32_t*)inp->out, size);
    csi_dcache_clean_invalid_range((uint32_t*)inp->in, size);

    rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    if (rc == 0) {
        memcpy((void*)out, (const void*)inp->out, size);
    }
    icore_msg_free(msg);

    return rc;
err:
    icore_msg_free(msg);
    return -1;
}

/**
 * @brief  free the inter-core fft
 * @param  [in] hdl
 * @return 0/-1
 */
int ffticore_free(ffticore_t *hdl)
{
    int rc = -1;
    icore_msg_t *msg;
    ffticore_free_t *inp;
    struct ffticore_ap_priv *priv;

    CHECK_PARAM(hdl, -1);
    priv = hdl->priv;
    msg  = icore_msg_new(ICORE_CMD_FFT_FREE, sizeof(ffticore_free_t));
    CHECK_RET_TAG_WITH_RET(msg, -1);
    inp = icore_get_msg(msg, ffticore_free_t);
    inp->fft = hdl->fft;

    rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    icore_msg_free(msg);
    mblock_free(priv->in_mb);
    mblock_free(priv->out_mb);
    aos_free(hdl->priv);
    aos_free(hdl);

    return rc;
}

/**
 * @brief  init the icore inter-core fft
 * @return 0/-1
 */
int ffticore_init()
{
    if (!g_ffticore.init) {
        g_ffticore.ipc = ipc_get(FFTICORE_CP_IDX);
        CHECK_RET_TAG_WITH_RET(g_ffticore.ipc, -1);
        ipc_add_service(g_ffticore.ipc, FFTICORE_IPC_SERIVCE_ID, NULL, NULL);

        g_ffticore.init = 1;
    }

    return g_ffticore.init ? 0 : -1;
}



