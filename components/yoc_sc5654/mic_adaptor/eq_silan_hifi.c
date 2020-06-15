/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <aos/debug.h>
#include "aef/eq_cls.h"
#include <yv_ap.h>

#define TAG "eq_silan_hifi"

/* 与DSP通讯的数据 */
static yv_eq_settings_t g_eq_settings __attribute__((aligned(64)));

struct eq_silan_hifi_priv {
    int debug;
};

static int _eq_silan_hifi_init(eqx_t *eq, uint32_t rate, uint8_t eq_segments)
{
    struct eq_silan_hifi_priv *priv = NULL;

    priv = aos_zalloc(sizeof(struct eq_silan_hifi_priv));
    CHECK_RET_TAG_WITH_RET(priv, -1);

    eq->priv = priv;
    return 0;
}

static int _eq_silan_hifi_set_enable(eqx_t *eq, uint8_t enable)
{
    int                        rc   = 0;
    struct eq_silan_hifi_priv *priv = eq->priv;
    priv->debug                     = 1;

    g_eq_settings.eq_num = 1;
    yv_t *yv_handle = yv_get_handler();

    yv_eq_init(yv_handle, &g_eq_settings);

    /* 适配层固定处理单声道数据 */
    yv_eq_config(yv_handle, 1, eq->rate, 0);

    return rc;
}

static int _eq_silan_hifi_set_param(eqx_t *eq, uint8_t segid, const eqfp_t *param)
{
    int                        rc   = -1;
    struct eq_silan_hifi_priv *priv = eq->priv;
    priv->debug                     = 2;

    if (segid < eq->eq_segments && segid < MAX_EQ_FILTER_NUM) {
        g_eq_settings.filters[0][segid].filterNum = eq->eq_segments;
        g_eq_settings.filters[0][segid].ch        = 0;
        g_eq_settings.filters[0][segid].index     = segid;
        g_eq_settings.filters[0][segid].type      = 0;
        g_eq_settings.filters[0][segid].status    = param->enable;
        g_eq_settings.filters[0][segid].fc        = param->rate; /* 中心频率 */
        g_eq_settings.filters[0][segid].gain      = param->gain; /* 增益 */
        g_eq_settings.filters[0][segid].qvalue    = param->q;    /* 斜率 */
    }

    rc = 0;
    return rc;
}

static int _eq_silan_hifi_process(eqx_t *eq, const int16_t *in, int16_t *out, size_t nb_samples)
{
    struct eq_silan_hifi_priv *priv = eq->priv;
    priv->debug                     = 3;

    if (eq->enable) {
        memcpy(out, in, nb_samples * sizeof(int16_t));

        yv_t *yv_handle = yv_get_handler();
        yv_eq_proc(yv_handle, out, nb_samples * sizeof(int16_t));
    } else {
        memcpy(out, in, nb_samples * sizeof(int16_t));
    }

    return 0;
}

static int _eq_silan_hifi_uninit(eqx_t *eq)
{
    struct eq_silan_hifi_priv *priv = eq->priv;
    priv->debug                     = 4;

    aos_free(priv);
    eq->priv = NULL;

    return 0;
}

const struct eqx_ops eqx_ops_silan = {
    .name       = "eq_silan_hifi",

    .init       = _eq_silan_hifi_init,
    .set_enable = _eq_silan_hifi_set_enable,
    .set_param  = _eq_silan_hifi_set_param,
    .process    = _eq_silan_hifi_process,
    .uninit     = _eq_silan_hifi_uninit,
};
