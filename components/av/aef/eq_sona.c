/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#if defined(CONFIG_EQXER_SONA) && CONFIG_EQXER_SONA

#include "aef/eq_cls.h"
#include "aef/sonaaef.h"

#define TAG                   "eq_sona"

struct eq_sona_priv {
    void                      *hdl;
};

static int _eq_sona_init(eqx_t *eq, uint32_t rate, uint8_t eq_segments)
{
    void *hdl = NULL;
    struct eq_sona_priv *priv = NULL;

    priv = aos_zalloc(sizeof(struct eq_sona_priv));
    CHECK_RET_TAG_WITH_RET(priv, -1);
    hdl = sona_eq_create(rate, eq_segments, eq->nsamples_max);
    CHECK_RET_TAG_WITH_GOTO(hdl, err);

    priv->hdl = hdl;
    eq->priv  = priv;
    return 0;
err:
    aos_free(priv);
    return -1;
}

static int _eq_sona_set_enable(eqx_t *eq, uint8_t enable)
{
    int rc;
    struct eq_sona_priv *priv = eq->priv;

    rc = sona_eq_set_enable(priv->hdl, enable);

    return rc;
}

static int _convert_to_BQFType(enum eqf_type type, eBQFType *itype)
{
    int rc = 0;

    switch (type) {
    case EQF_TYPE_PEAK:
        *itype = BQF_PEAK;
        break;
    case EQF_TYPE_NOTCH:
        *itype = BQF_NOTCH;
        break;
    case EQF_TYPE_LP1:
        *itype = BQF_LP1;
        break;
    case EQF_TYPE_HP1:
        *itype = BQF_HP1;
        break;
    case EQF_TYPE_LP2:
        *itype = BQF_LP2;
        break;
    case EQF_TYPE_HP2:
        *itype = BQF_HP2;
        break;
    case EQF_TYPE_BP2:
        *itype = BQF_BP2;
        break;
    default:
        LOGE(TAG, "convert inner type faild. type = %u", type);
        rc = -1;
        break;
    }

    return rc;
}

static int _convert_inner_param(const eqfp_t *param, tBiQuadFilterParam *inp)
{
    int rc;
    eBQFType itype;

    rc = _convert_to_BQFType(param->type, &itype);
    CHECK_RET_TAG_WITH_RET(rc == 0, -1);

    memset(inp, 0, sizeof(tBiQuadFilterParam));
    inp->type   = itype;
    inp->enable = param->enable;
    inp->gain   = param->gain;
    inp->q      = param->q;
    inp->freq   = param->rate;

    return 0;
}

static int _eq_sona_set_param(eqx_t *eq, uint8_t segid, const eqfp_t *param)
{
    int rc = -1;
    tBiQuadFilterParam inp;
    struct eq_sona_priv *priv = eq->priv;

    if (_convert_inner_param(param, &inp) == 0) {
        rc = sona_eq_set_param(priv->hdl, segid, &inp);
    }

    return rc;
}

static int _eq_sona_process(eqx_t *eq, const int16_t *in, int16_t *out, size_t nb_samples)
{
    struct eq_sona_priv *priv = eq->priv;

    sona_eq_process(priv->hdl, (short*)in, out, (int)nb_samples);

    return 0;
}

static int _eq_sona_uninit(eqx_t *eq)
{
    struct eq_sona_priv *priv = eq->priv;

    sona_eq_destroy(priv->hdl);
    aos_free(priv);
    eq->priv = NULL;
    return 0;
}

const struct eqx_ops eqx_ops_sona = {
    .name            = "eq_sona",

    .init            = _eq_sona_init,
    .set_enable      = _eq_sona_set_enable,
    .set_param       = _eq_sona_set_param,
    .process         = _eq_sona_process,
    .uninit          = _eq_sona_uninit,
};

#endif

