/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avfilter/avfilter_swr.h"
#include "swresample/swresample.h"

#define TAG                   "filter_swr"

struct avfilter_swr {
    struct avfilter           f;

    sf_t                      isf;
    sf_t                      osf;
    swr_t                     *s;
};

static int _avf_swr_control(avfilter_t *avf, int cmd, void *arg, size_t *arg_size)
{
    //TODO
    return 0;
}

static int _avf_swr_filter_frame(avfilter_t *avf, const avframe_t *in, avframe_t *out)
{
    int rc = -1;
    avframe_t *oframe;
    struct avfilter_swr *dobj = (struct avfilter_swr*)avf;
    swr_t *s                  = dobj->s;

    if (dobj->s && (in->sf == dobj->isf)) {
        oframe = AVF_IS_SINK(avf) ? out : avf->oframe;
        rc = swr_convert_frame(s, in, oframe);
        if (rc > 0) {
            rc = AVF_IS_SINK(avf) ? rc : avf_filter_frame(avf->next, oframe, out);
        } else if (rc == 0) {
            //FIXME: need more data & interleaved output only
            out->nb_samples  = 0;
            out->linesize[0] = 0;
            return 0;
        } else {
            LOGE(TAG, "swr convert frame failed, rc = %d", rc);
            return -1;
        }
    }

    return rc > 0 ? 0 : rc;
}

static int _avf_swr_uninit(avfilter_t *avf)
{
    struct avfilter_swr *dobj = (struct avfilter_swr*)avf;

    swr_free(dobj->s);
    return 0;
}

const struct avfilter_ops avf_ops_swr = {
    .name            = "swr",
    .type            = AVMEDIA_TYPE_AUDIO,

    .control         = _avf_swr_control,
    .filter_frame    = _avf_swr_filter_frame,
    .uninit          = _avf_swr_uninit,
};

/**
 * @brief  open swr avfilter
 * @param  [in] inst_name : the name of avfilter_t instance
 * @param  [in] swrp
 * @return NULL on error
 */
avfilter_t *avf_swr_open(const char *inst_name, const swr_avfp_t *swrp)
{
    int rc;
    swr_t *s;
    struct avfilter *f        = NULL;
    struct avfilter_swr *dobj = NULL;

    CHECK_PARAM(inst_name && swrp, NULL);
    CHECK_PARAM(swrp->isf != swrp->osf, NULL);
    dobj = aos_zalloc(sizeof(struct avfilter_swr));
    CHECK_RET_TAG_WITH_RET(dobj, NULL);
    f       = (avfilter_t*)dobj;
    f->ops  = &avf_ops_swr;
    f->dobj = dobj;

    rc = avf_init(inst_name, f);
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);

    rc = f->ops->init ? f->ops->init((avfilter_t*)dobj) : 0;
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);

    s  = swr_new(swrp->isf, swrp->osf);
    CHECK_RET_TAG_WITH_GOTO(s, err);
    dobj->s   = s;
    dobj->isf = swrp->isf;
    dobj->osf = swrp->osf;
    LOGD(TAG, "open a avfilter, name = %s", f->ops->name);

    return (avfilter_t*)dobj;
err:
    avf_uninit(f);
    aos_free(dobj);
    return NULL;
}



