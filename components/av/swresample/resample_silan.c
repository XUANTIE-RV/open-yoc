/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#if defined(CONFIG_RESAMPLER_SILAN) && CONFIG_RESAMPLER_SILAN

#include "swresample/resample_cls.h"
#include "swresample/audproc_common.h"
#include "swresample/audproc.h"
#include "swresample/aplanis.h"

#define TAG                   "res_silan"

//guess define, no this header
extern void getprocParam_internal(AudProc *proc, int cmd, size_t *sz, int *outsize);

struct res_silan_priv {
    AudProc                      *proc;
};

static int _resample_silan_init(resx_t *r)
{
    AudProc *proc = NULL;
    struct res_silan_priv *priv;
    aFormat informat, outformat;

    priv = aos_zalloc(sizeof(struct res_silan_priv));
    CHECK_RET_TAG_WITH_RET(priv, -1);

    proc = resample_proc_create("resample");
    CHECK_RET_TAG_WITH_GOTO(proc, err);

    proc->init(proc);
    informat.channels    = r->channels;
    informat.sampleRate  = r->irate;
    informat.bits        = r->bits;
    outformat.channels   = r->channels;
    outformat.sampleRate = r->orate;
    outformat.bits       = r->bits;
    copy_audio_format(&(proc->inFormat), &informat);
    copy_audio_format(&(proc->outFormat), &outformat);

    proc->state = AUDPROC_STATE_CREATED;
    priv->proc  = proc;
    r->priv     = priv;

    return 0;
err:
    aos_free(priv);
    return -1;
}

static int _resample_silan_convert(resx_t *r, void **out, size_t nb_osamples, const void **in, size_t nb_isamples)
{
    int i;
    int outsize = 0;
    size_t sz   = sizeof(int);
    struct res_silan_priv *priv = r->priv;
    AudProc *proc               = priv->proc;

    for (i = 0; i < r->channels; i++) {
        proc->in     = (SAMPLE *)in[i];
        proc->inSize = nb_isamples;
        getprocParam_internal(proc, GET_OUTPUT_BUFFER_SIZE, &sz, &outsize);
        if (!((outsize > 0) && (outsize <= nb_osamples))) {
            LOGE(TAG, "proc process outsize failed, outsize = %d, nb_osamples = %d", outsize, nb_osamples);
            return -1;
        }

        proc->outSize = outsize;
        proc->out     = (SAMPLE*)out[i];
        proc->proc(proc);
        outsize = proc->outSize;
    }

    return outsize;
}

static int _resample_silan_uninit(resx_t *r)
{
    struct res_silan_priv *priv = r->priv;
    AudProc *proc               = priv->proc;

    proc->finish(proc);
    proc->state = AUDPROC_STATE_INIT;
    aud_proc_finish(proc);
    aos_free(priv);
    r->priv = NULL;
    return 0;
}

const struct resx_ops resx_ops_silan = {
    .name            = "silan",

    .init            = _resample_silan_init,
    .convert         = _resample_silan_convert,
    .uninit          = _resample_silan_uninit,
};

#endif

