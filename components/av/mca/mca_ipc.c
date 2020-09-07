/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "mca/mca_cls.h"
#include "mcaicore.h"

#define TAG                   "mca_ipc"

struct mca_ipc_priv {
    mcaicore_t                *hdl;
};

static int _mca_ipc_init(mcax_t *mca, int32_t type)
{
    int rc;
    void *hdl = NULL;
    struct mca_ipc_priv *priv = NULL;

    rc = mcaicore_init();
    CHECK_RET_TAG_WITH_RET(rc == 0, -1);

    priv = aos_zalloc(sizeof(struct mca_ipc_priv));
    CHECK_RET_TAG_WITH_RET(priv, -1);
    hdl = mcaicore_new(type);
    CHECK_RET_TAG_WITH_GOTO(hdl, err);

    priv->hdl = hdl;
    mca->priv = priv;
    return 0;
err:
    aos_free(priv);
    return -1;
}

static int _mca_ipc_iir_fxp32_coeff32_config(mcax_t *mca, const fxp32_t *coeff)
{
    struct mca_ipc_priv *priv = mca->priv;

    mcaicore_iir_fxp32_coeff32_config(priv->hdl, coeff);

    return 0;
}

static int _mca_ipc_iir_fxp32(mcax_t *mca, const fxp32_t *input, size_t input_size,
                              fxp32_t yn1, fxp32_t yn2, fxp32_t *output)
{
    struct mca_ipc_priv *priv = mca->priv;

    mcaicore_iir_fxp32(priv->hdl, input, input_size, yn1, yn2, output);

    return 0;
}

static int _mca_ipc_uninit(mcax_t *mca)
{
    struct mca_ipc_priv *priv = mca->priv;

    mcaicore_free(priv->hdl);
    aos_free(priv);
    mca->priv = NULL;
    return 0;
}

const struct mcax_ops mcax_ops_ipc = {
    .name                             = "mca_ipc",

    .init                             = _mca_ipc_init,
    .iir_fxp32_coeff32_config         = _mca_ipc_iir_fxp32_coeff32_config,
    .iir_fxp32                        = _mca_ipc_iir_fxp32,
    .uninit                           = _mca_ipc_uninit,
};

