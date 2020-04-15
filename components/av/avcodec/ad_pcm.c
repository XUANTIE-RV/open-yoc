/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#if defined(CONFIG_DECODER_PCM) && CONFIG_DECODER_PCM

#include "avutil/common.h"
#include "avcodec/ad_cls.h"

#define TAG                    "ad_pcm"

struct ad_pcm_priv {
    //TODO
    sf_t sf;
};

static int _ad_pcm_open(ad_cls_t *o)
{
    //TODO
    return 0;
}

static int _ad_pcm_decode(ad_cls_t *o, avframe_t *frame, int *got_frame, const avpacket_t *pkt)
{
    int rc;
    sf_t sf;

    sf                = o->ash.sf;
    frame->sf         = sf;
    frame->nb_samples = pkt->len / sf_get_frame_size(sf);

    rc = avframe_get_buffer(frame);
    if (rc < 0) {
        LOGD(TAG, "avframe_get_buffer failed, may be oom. sf = %u, bit = %u, rate = %u, ch = %u\n", sf,
             sf_get_bit(sf), sf_get_rate(sf), sf_get_channel(sf));
        return -1;
    }

    rc = frame->nb_samples * sf_get_frame_size(sf);
    memcpy((void*)frame->data[0], pkt->data, rc);
    *got_frame = 1;

    //TODO
    return rc;
}

static int _ad_pcm_control(ad_cls_t *o, int cmd, void *arg, size_t *arg_size)
{
    //TODO
    return 0;
}

static int _ad_pcm_close(ad_cls_t *o)
{
    //TODO

    return 0;
}

const struct ad_ops ad_ops_pcm = {
    .name           = "pcm",
    .id             = AVCODEC_ID_RAWAUDIO,

    .open           = _ad_pcm_open,
    .decode         = _ad_pcm_decode,
    .control        = _ad_pcm_control,
    .close          = _ad_pcm_close,
};

#endif

