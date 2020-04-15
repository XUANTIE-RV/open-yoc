/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AO_CLS_H__
#define __AO_CLS_H__

#include <yoc_config.h>
#include "avutil/common.h"
#include "avutil/av_typedef.h"
#include "avfilter/avfilter.h"
#include "avfilter/avfilter_all.h"

__BEGIN_DECLS__

#define AO_ONE_PERIOD_MS            (5)
#define AO_TOTAL_PERIOD_NUM         (12)

typedef struct ao_cls ao_cls_t;

struct ao_ops {
    const char          *name;

    int (*open)         (ao_cls_t *o, sf_t sf);
    int (*start)        (ao_cls_t *o);
    int (*stop)         (ao_cls_t *o);
    int (*drain)        (ao_cls_t *o);
    int (*close)        (ao_cls_t *o);
    int (*write)        (ao_cls_t *o, const uint8_t *buf, size_t count);
};

struct ao_cls {
    sf_t                sf;
    void                *priv;
    uint8_t             start;
    uint8_t             interrupt;

    sf_t                ori_sf;
    uint32_t            period_ms;     ///< period cache size(ms) for audio out. 0 means use default
    uint32_t            period_num;    ///< number of period_ms. total cache size for ao is (period_num * period_ms * (rate / 1000) * 2 * (16/8)). 0 means use default
    uint32_t            resample_rate; ///< none zereo means need to resample
    avfilter_t          *avfc;         ///< saved the first avfilter in the chain
    avfilter_t          *avf_vol;      ///< used for soft vol config
    uint8_t             vol_en;        ///< soft vol scale enable
    uint8_t             vol_index;     ///< soft vol scale index (0~255)
    uint8_t             *aef_conf;     ///< config data for aef
    size_t              aef_conf_size; ///< size of the config data for aef
    uint8_t             eq_en;         ///< used for equalizer config
    uint8_t             eq_segments;   ///< used for equalizer config
    eqfp_t              *eq_params;
    avfilter_t          *avf_eq;       ///< used for equalizer config
    avframe_t           *oframe;       ///< used for filtering

    const struct ao_ops *ops;
    aos_mutex_t         lock;
};

__END_DECLS__

#endif /* __AO_CLS_H__ */

