/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#if defined(CONFIG_EQXER_IPC) && CONFIG_EQXER_IPC

#include "aef/eq_cls.h"
#include "eqicore.h"

#define TAG                   "eq_ipc"

struct eq_ipc_priv {
    eqicore_t                 *hdl;
};

static int _eq_ipc_init(eqx_t *eq, uint32_t rate, uint8_t eq_segments)
{
    int rc;
    eqicore_t *hdl = NULL;
    struct eq_ipc_priv *priv = NULL;

    rc = eqicore_init();
    CHECK_RET_TAG_WITH_RET(rc == 0, -1);

    priv = aos_zalloc(sizeof(struct eq_ipc_priv));
    CHECK_RET_TAG_WITH_RET(priv, -1);
    hdl = eqicore_new(rate, eq_segments);
    CHECK_RET_TAG_WITH_GOTO(hdl, err);

    priv->hdl = hdl;
    eq->priv  = priv;
    return 0;
err:
    aos_free(priv);
    return -1;
}

static int _eq_ipc_set_enable(eqx_t *eq, uint8_t enable)
{
    int rc;
    struct eq_ipc_priv *priv = eq->priv;

    rc = eqicore_set_enable(priv->hdl, enable);

    return rc;
}

static int _eq_ipc_set_param(eqx_t *eq, uint8_t segid, const eqfp_t *param)
{
    int rc = -1;
    struct eq_ipc_priv *priv = eq->priv;

    rc = eqicore_set_param(priv->hdl, segid, param);

    return rc;
}

static int _eq_ipc_process(eqx_t *eq, const int16_t *in, int16_t *out, size_t nb_samples)
{
    int rc = -1;
    struct eq_ipc_priv *priv = eq->priv;

    rc = eqicore_process(priv->hdl, in, out, nb_samples);

    return rc;
}

static int _eq_ipc_uninit(eqx_t *eq)
{
    struct eq_ipc_priv *priv = eq->priv;

    eqicore_free(priv->hdl);
    aos_free(priv);
    eq->priv = NULL;
    return 0;
}

const struct eqx_ops eqx_ops_ipc = {
    .name            = "eq_ipc",

    .init            = _eq_ipc_init,
    .set_enable      = _eq_ipc_set_enable,
    .set_param       = _eq_ipc_set_param,
    .process         = _eq_ipc_process,
    .uninit          = _eq_ipc_uninit,
};

#endif

