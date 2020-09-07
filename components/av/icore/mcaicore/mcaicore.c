/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "ipc.h"
#include <csi_core.h>
#include "avutil/mem_block.h"
#include "mcaicore.h"
#include "icore/mcaicore_internal.h"

#define TAG                     "mcaicore_ap"
#define MCA_MB_SIZE_DEFAULT     (1*1024)

struct mcaicore {
    void            *mca;       // mcax_t
    void            *priv;     // priv only for ap/cpu0
};

struct mcaicore_ap_priv {
    mblock_t        *in_mb;
    mblock_t        *out_mb;
};

static struct {
    ipc_t           *ipc;
    int             init;
} g_mcaicore;

static int _ipc_cmd_send(icore_msg_t *data, int sync)
{
    message_t msg;
    memset(&msg, 0, sizeof(message_t));

    msg.service_id      = MCAICORE_IPC_SERIVCE_ID;
    msg.command         = IPC_CMD_MCAICORE;
    msg.req_data        = data;
    msg.req_len         = ICORE_MSG_SIZE + data->size;
    msg.resp_data       = data;
    msg.resp_len        = ICORE_MSG_SIZE + data->size;
    msg.flag           |= sync;

    ipc_message_send(g_mcaicore.ipc, &msg, AOS_WAIT_FOREVER);

    return data->ret.code;
}

/**
 * @brief  alloc a inter-core mca
 * @param  [in] type : MCA_TYPE_XXX
 * @return NULL on error
 */
mcaicore_t* mcaicore_new(int type)
{
    int rc;
    mcaicore_t *hdl               = NULL;
    struct mcaicore_ap_priv *priv = NULL;
    mblock_t *in_mb               = NULL, *out_mb = NULL;
    icore_msg_t *msg_new          = NULL;
    mcaicore_new_t *inp;

    CHECK_PARAM(type, NULL);
    hdl = aos_zalloc(sizeof(mcaicore_t));
    CHECK_RET_TAG_WITH_RET(hdl, NULL);

    in_mb = mblock_new(MCA_MB_SIZE_DEFAULT, 16);
    out_mb = mblock_new(MCA_MB_SIZE_DEFAULT, 16);
    CHECK_RET_TAG_WITH_GOTO(in_mb && out_mb, err);

    msg_new = icore_msg_new(ICORE_CMD_MCA_NEW, sizeof(mcaicore_new_t));
    CHECK_RET_TAG_WITH_GOTO(msg_new, err);

    priv = aos_zalloc(sizeof(struct mcaicore_ap_priv));
    CHECK_RET_TAG_WITH_GOTO(priv, err);

    inp = icore_get_msg(msg_new, mcaicore_new_t);
    inp->type = type;

    rc = _ipc_cmd_send(msg_new, MESSAGE_SYNC);
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);

    priv->in_mb  = in_mb;
    priv->out_mb = out_mb;
    hdl->mca     = inp->mca;
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
 * Configures IIR filter with 32-bit input/ouput and 32-bit coefficients.
 *
 * Note:
 *   # coeff is 32-bit (Q1.7.24), with size of 5.
 *   # Input/output are 32-bit (Q1.31.0).
 * @return -1/0
 */
int mcaicore_iir_fxp32_coeff32_config(mcaicore_t *hdl, const fxp32_t *coeff)
{
    int rc = -1;
    icore_msg_t *msg;
    mcaicore_iir_fxp32_coeff32_config_t *inp;

    CHECK_PARAM(hdl && coeff, -1);
    msg  = icore_msg_new(ICORE_CMD_MCA_IIR_FXP32_COEFF32_CONFIG, sizeof(mcaicore_iir_fxp32_coeff32_config_t));
    CHECK_RET_TAG_WITH_RET(msg, -1);

    inp = icore_get_msg(msg, mcaicore_iir_fxp32_coeff32_config_t);
    inp->mca = hdl->mca;
    memcpy((void*)inp->coeff, (const void*)coeff, 5 * sizeof(fxp32_t));

    rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    icore_msg_free(msg);

    return rc;
}

/**
 * IIR filter with 32-bit input/output.
 *
 * Note:
 *   # input is 32-bit (Q1.31.0), output also 32-bit but its precision is
 *     determined by configuration.
 *   # input_size must be greater than order, and size of output is
 *     (input_size - 2).
 *   # yn1 and yn2 are the 1st and 2nd samples from the last output.
 * @return -1/0
 */
int mcaicore_iir_fxp32(mcaicore_t *hdl, const fxp32_t *input, size_t input_size, fxp32_t yn1, fxp32_t yn2,
                       fxp32_t *output)
{
    int rc = -1;
    size_t size;
    icore_msg_t *msg;
    mcaicore_iir_fxp32_t *inp;
    struct mcaicore_ap_priv *priv;

    CHECK_PARAM(hdl && input && input_size && output, -1);
    priv = hdl->priv;
    msg  = icore_msg_new(ICORE_CMD_MCA_IIR_FXP32, sizeof(mcaicore_iir_fxp32_t));
    CHECK_RET_TAG_WITH_RET(msg, -1);

    size = input_size * sizeof(fxp32_t);
    rc = mblock_grow(priv->in_mb, ICORE_ALIGN_BUFZ(size));
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);
    rc = mblock_grow(priv->out_mb, ICORE_ALIGN_BUFZ(size));
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);

    inp = icore_get_msg(msg, mcaicore_iir_fxp32_t);
    inp->mca        = hdl->mca;
    inp->input_size = input_size;
    inp->yn1        = yn1;
    inp->yn2        = yn2;
    inp->input      = priv->in_mb->data;
    inp->output     = priv->out_mb->data;
    memcpy((void*)inp->input, (const void*)input, size);
    csi_dcache_invalid_range((uint32_t*)inp->output, size);
    csi_dcache_clean_invalid_range((uint32_t*)inp->input, size);

    rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    if (rc == 0) {
        size = (input_size - 2) * sizeof(fxp32_t);
        memcpy((void*)output, (const void*)inp->output, size);
    }
    icore_msg_free(msg);

    return rc;
err:
    icore_msg_free(msg);
    return -1;
}

/**
 * @brief  free the inter-core mca
 * @param  [in] hdl
 * @return 0/-1
 */
int mcaicore_free(mcaicore_t *hdl)
{
    int rc = -1;
    icore_msg_t *msg;
    mcaicore_free_t *inp;
    struct mcaicore_ap_priv *priv;

    CHECK_PARAM(hdl, -1);
    priv = hdl->priv;
    msg  = icore_msg_new(ICORE_CMD_MCA_FREE, sizeof(mcaicore_free_t));
    CHECK_RET_TAG_WITH_RET(msg, -1);
    inp = icore_get_msg(msg, mcaicore_free_t);
    inp->mca = hdl->mca;

    rc = _ipc_cmd_send(msg, MESSAGE_SYNC);
    icore_msg_free(msg);
    mblock_free(priv->in_mb);
    mblock_free(priv->out_mb);
    aos_free(hdl->priv);
    aos_free(hdl);

    return rc;
}

/**
 * @brief  init the icore inter-core mca
 * @return 0/-1
 */
int mcaicore_init()
{
    if (!g_mcaicore.init) {
        g_mcaicore.ipc = ipc_get(MCAICORE_CP_IDX);
        CHECK_RET_TAG_WITH_RET(g_mcaicore.ipc, -1);
        ipc_add_service(g_mcaicore.ipc, MCAICORE_IPC_SERIVCE_ID, NULL, NULL);

        g_mcaicore.init = 1;
    }

    return g_mcaicore.init ? 0 : -1;
}



