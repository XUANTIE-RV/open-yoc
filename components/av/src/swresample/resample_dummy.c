/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#if defined(CONFIG_RESAMPLER_DUMMY) && CONFIG_RESAMPLER_DUMMY

#include "av/swresample/resample_cls.h"

#define TAG                   "res_dummy"

struct res_dummy_priv {
    //TODO:
    int                       dummy;
};

static int _resample_dummy_init(resx_t *r)
{
    struct res_dummy_priv *priv;

    priv    = av_zalloc(sizeof(struct res_dummy_priv));
    r->priv = priv;

    return 0;
}

static int _resample_dummy_convert(resx_t *r, void **out, size_t nb_osamples, const void **in, size_t nb_isamples)
{
    //TODO:
    return 0;
}

static int _resample_dummy_uninit(resx_t *r)
{
    struct res_dummy_priv *priv = r->priv;

    av_free(priv);
    r->priv = NULL;
    return 0;
}

const struct resx_ops resx_ops_dummy = {
    .name            = "dummy",

    .init            = _resample_dummy_init,
    .convert         = _resample_dummy_convert,
    .uninit          = _resample_dummy_uninit,
};

#endif


