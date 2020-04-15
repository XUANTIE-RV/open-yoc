/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#if defined(CONFIG_RESAMPLER_IPC) && CONFIG_RESAMPLER_IPC

#include "swresample/resample_cls.h"
#include "resicore.h"

#define TAG                   "res_ipc"

struct res_ipc_priv {
    resicore_t                *hdl;
};

static int _resample_ipc_init(resx_t *r)
{
    int rc;
    resicore_t *hdl = NULL;
    struct res_ipc_priv *priv;

    rc = resicore_init();
    CHECK_RET_TAG_WITH_RET(rc == 0, -1);

    priv = aos_zalloc(sizeof(struct res_ipc_priv));
    CHECK_RET_TAG_WITH_RET(priv, -1);

    hdl = resicore_new(r->irate, r->orate, r->channels, r->bits);
    CHECK_RET_TAG_WITH_GOTO(hdl, err);

    priv->hdl  = hdl;
    r->priv     = priv;

    return 0;
err:
    aos_free(priv);
    return -1;
}

static int _resample_ipc_convert(resx_t *r, void **out, size_t nb_osamples, const void **in, size_t nb_isamples)
{
    int rc;
    struct res_ipc_priv *priv = r->priv;
    resicore_t *hdl           = priv->hdl;

    rc = resicore_convert(hdl, out, nb_osamples, in, nb_isamples);

    return rc;
}

static int _resample_ipc_uninit(resx_t *r)
{
    struct res_ipc_priv *priv = r->priv;
    resicore_t *hdl           = priv->hdl;

    resicore_free(hdl);
    aos_free(priv);
    r->priv = NULL;
    return 0;
}

const struct resx_ops resx_ops_ipc = {
    .name            = "ipc",

    .init            = _resample_ipc_init,
    .convert         = _resample_ipc_convert,
    .uninit          = _resample_ipc_uninit,
};

#endif


