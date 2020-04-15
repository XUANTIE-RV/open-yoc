/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#if defined(CONFIG_DECODER_FDK) && CONFIG_DECODER_FDK

#include "avutil/common.h"
#include "avutil/byte_rw.h"
#include "avformat/adts_rw.h"
#include "avcodec/ad_cls.h"
#include "fdk_aac/aacdecoder_lib.h"

#define TAG                    "ad_fdk"
#define DECODE_CHN_MAX         (1)
#define DECODE_BUF_SIZE_MAX    (4096 * DECODE_CHN_MAX * sizeof(short))

struct ad_fdk_priv {
    HANDLE_AACDECODER     handle;
    uint8_t               *dbuf;        ///< inner decoder buf
    size_t                dsize;
    uint8_t               inited;       ///< for get the sample format init
    unsigned char         channels;
    unsigned long         sample_rate;
};

static int _fdk_open(struct ad_fdk_priv *priv, uint8_t *extradata, size_t extradata_size)
{
    int channels = DECODE_CHN_MAX;
    int conceal_method = 2;  //0 muting 1 noise 2 interpolation
    CStreamInfo *info;
    AAC_DECODER_ERROR err;
    HANDLE_AACDECODER handle = NULL;
    TRANSPORT_TYPE type = TT_MP4_ADTS;

    if (extradata_size && extradata) {
        uint32_t value;

        value = byte_r16be(extradata);
        type = (value & 0xfff0) != 0xfff0 ? TT_MP4_RAW : type;
    }
    handle = aacDecoder_Open(type, 1);
    CHECK_RET_TAG_WITH_RET(handle, -1);

    aacDecoder_SetParam(handle, AAC_CONCEAL_METHOD, conceal_method);
    aacDecoder_SetParam(handle, AAC_PCM_MAX_OUTPUT_CHANNELS, channels);
    aacDecoder_SetParam(handle, AAC_PCM_MIN_OUTPUT_CHANNELS, channels);
    if (type == TT_MP4_RAW) {
        err = aacDecoder_ConfigRaw(handle, &extradata, &extradata_size);
        CHECK_RET_TAG_WITH_GOTO(err == AAC_DEC_OK, err);
        info = aacDecoder_GetStreamInfo(handle);
        //FIXME
        priv->channels    = channels;
        priv->sample_rate = info->extSamplingRate ? info->extSamplingRate : info->aacSampleRate;
    } else if (extradata_size && extradata) {
        unsigned int valid = extradata_size;
        /* the first pkt as extradata for get stream info accuratly */
        err = aacDecoder_Fill(handle, &extradata, &extradata_size, &valid);
        CHECK_RET_TAG_WITH_GOTO(err == AAC_DEC_OK, err);
        err = aacDecoder_DecodeFrame(handle, (INT_PCM *)priv->dbuf, priv->dsize / sizeof(INT_PCM), 0);
        CHECK_RET_TAG_WITH_GOTO(err == AAC_DEC_OK, err);
        info = aacDecoder_GetStreamInfo(handle);
        priv->channels    = info->numChannels;
        priv->sample_rate = info->sampleRate;
    }

    if (info) {
        if (!(priv->channels > 0 && priv->sample_rate > 0)) {
            LOGE(TAG, "get channel or rate fail, %d, %d.", priv->channels, priv->sample_rate);
            goto err;
        }
        priv->inited = 1;
    }

    priv->handle = handle;
    return 0;
err:
    if (handle)
        aacDecoder_Close(handle);
    return -1;
}

static int _ad_fdk_open(ad_cls_t *o)
{
    int rc;
    int bits = 16;
    uint8_t *dbuf            = NULL;
    struct ad_fdk_priv *priv = NULL;

    priv = aos_zalloc(sizeof(struct ad_fdk_priv));
    dbuf = aos_zalloc(DECODE_BUF_SIZE_MAX);
    CHECK_RET_TAG_WITH_GOTO(priv && dbuf, err);
    priv->dbuf  = dbuf;
    priv->dsize = DECODE_BUF_SIZE_MAX;

    rc = _fdk_open(priv, o->ash.extradata, o->ash.extradata_size);
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);
    if (priv->inited)
        o->ash.sf =  sf_make_channel(priv->channels) | sf_make_rate(priv->sample_rate) | sf_make_bit(bits) | sf_make_signed(bits > 8);
    o->priv = priv;

    return 0;
err:
    aos_free(dbuf);
    aos_free(priv);
    return -1;
}

static int _ad_fdk_decode(ad_cls_t *o, avframe_t *frame, int *got_frame, const avpacket_t *pkt)
{
    int ret = -1;
    sf_t sf;
    int bits = 16;
    CStreamInfo *info;
    AAC_DECODER_ERROR err;
    unsigned int valid       = pkt->len;
    struct ad_fdk_priv *priv = o->priv;
    HANDLE_AACDECODER handle = priv->handle;

    err = aacDecoder_Fill(handle, (UCHAR**)&pkt->data, (const UINT*)&pkt->len, &valid);
    CHECK_RET_TAG_WITH_GOTO(err == AAC_DEC_OK, quit);

    err = aacDecoder_DecodeFrame(handle, (INT_PCM *)priv->dbuf, priv->dsize / sizeof(INT_PCM), 0);
    if (err != AAC_DEC_OK) {
        LOGE(TAG, "aac decode fail. err = 0x%x", err);
        goto quit;
    }
    info = aacDecoder_GetStreamInfo(handle);
    if (info && info->numChannels > 0 && info->sampleRate > 0 && info->frameSize > 0) {
        sf = sf_make_channel(info->numChannels) | sf_make_rate(info->sampleRate) |
             sf_make_bit(bits) | sf_make_signed(bits > 8);
        if (!priv->inited) {
            o->ash.sf         = sf;
            priv->inited      = 1;
            priv->channels    = info->numChannels;
            priv->sample_rate = info->sampleRate;
        }
        frame->sf         = sf;
        frame->nb_samples = info->frameSize;

        ret = avframe_get_buffer(frame);
        if (ret < 0) {
            LOGD(TAG, "avframe_get_buffer failed, may be oom. sf = %u, ch = %d, rate = %d\n", sf,
                 info->numChannels, info->sampleRate);
            goto quit;
        }

        memcpy((void*)frame->data[0], priv->dbuf, info->frameSize * info->numChannels * (16 / 8));
        *got_frame = 1;
    }

    ret = pkt->len - valid;
quit:
    return ret;
}

static int _ad_fdk_control(ad_cls_t *o, int cmd, void *arg, size_t *arg_size)
{
    //TODO
    return 0;
}

static int _ad_fdk_reset(ad_cls_t *o)
{
    int rc;
    struct ad_fdk_priv *priv = o->priv;
    HANDLE_AACDECODER handle = priv->handle;

    aacDecoder_Close(handle);
    priv->handle = NULL;
    priv->inited = 0;
    rc = _fdk_open(priv, o->ash.extradata, o->ash.extradata_size);

    return rc;
}

static int _ad_fdk_close(ad_cls_t *o)
{
    struct ad_fdk_priv *priv = o->priv;
    HANDLE_AACDECODER handle = priv->handle;

    aacDecoder_Close(handle);
    aos_free(priv->dbuf);
    aos_free(priv);
    o->priv = NULL;
    return 0;
}

const struct ad_ops ad_ops_fdk = {
    .name           = "fdk",
    .id             = AVCODEC_ID_AAC,

    .open           = _ad_fdk_open,
    .decode         = _ad_fdk_decode,
    .control        = _ad_fdk_control,
    .reset          = _ad_fdk_reset,
    .close          = _ad_fdk_close,
};

#endif
