## 简介

pvmp3dec是一个开源的mp3解码库，其遵循apache协议。

###  pvmp3dec使用示例

```c
#include "pvmp3decoder_api.h"

struct ad_pvmp3_priv {
    void                     *obuf;  // buf for output
    void                     *dbuf;  // buf for decoder
    tPVMP3DecoderExternal    config;
};

static int _ad_pvmp3_open(ad_cls_t *o)
{
    uint32_t msize;
    void *dbuf = NULL, *obuf = NULL;
    tPVMP3DecoderExternal *config;
    struct ad_pvmp3_priv *priv = NULL;

    priv = aos_zalloc(sizeof(struct ad_pvmp3_priv));
    CHECK_RET_TAG_WITH_RET(NULL != priv, -1);
    config = &priv->config;
    /* 获取内部解码所需要内存 */
    msize = pvmp3_decoderMemRequirements();
    dbuf = aos_malloc(msize);
    obuf = aos_malloc(PVMP3DEC_OBUF_SIZE);
    CHECK_RET_TAG_WITH_GOTO(dbuf && obuf, err);
    /* 初始化解码器 */
    pvmp3_InitDecoder(config, dbuf);
    config->crcEnabled    = false;
    config->equalizerType = flat;
    config->pOutputBuffer = obuf;

    priv->obuf = obuf;
    priv->dbuf = dbuf;
    o->priv    = priv;
    return 0;

err:
    aos_free(obuf);
    aos_free(dbuf);
    aos_free(priv);
    return -1;
}

static int _ad_pvmp3_decode(ad_cls_t *o, avframe_t *frame, int *got_frame, const avpacket_t *pkt)                                                                                                           
{
    sf_t sf;
    int ret = -1;
    ERROR_CODE ecode;
    tPVMP3DecoderExternal *config;
    int rc, channel, bits = 16;
    struct ad_pvmp3_priv *priv = o->priv;
    
    *got_frame = 0;
    config = &priv->config;
    
    config->inputBufferMaxLength     = 0;
    config->inputBufferUsedLength    = 0;
    config->pInputBuffer             = pkt->data;
    config->inputBufferCurrentLength = pkt->len;
    config->outputFrameSize          = PVMP3DEC_OBUF_SIZE / sizeof(int16_t);
    /* 解码一帧mp3数据 */
    ecode = pvmp3_framedecoder(config, priv->dbuf);
    if (!((ecode == NO_DECODING_ERROR) &&
          config->outputFrameSize && config->num_channels && config->samplingRate)) {
        LOGE(TAG, "ecode = %d, frame size = %d, ch = %d, rate = %d.", ecode,
             config->outputFrameSize, config->num_channels, config->samplingRate);
        goto quit;
    }
    
    channel           = config->num_channels;
    sf                = sf_make_channel(channel) | sf_make_rate(config->samplingRate) | sf_make_bit(bits) | sf_make_signed(bits > 8);
    frame->sf         = sf;
    o->ash.sf         = sf;
    frame->nb_samples = config->outputFrameSize / channel ;
    
    rc = avframe_get_buffer(frame);
    if (rc < 0) { 
        LOGD(TAG, "avframe_get_buffer failed, may be oom. sf = %u, ch = %d, rate = %d\n", sf, channel,
             config->samplingRate);
        ret = -1;
        goto quit;
    }
    
    rc = sf_get_frame_size(sf) * frame->nb_samples;
    memcpy(frame->data[0], config->pOutputBuffer, rc);
    ret = config->inputBufferUsedLength;
    *got_frame = 1;

quit:
    return ret;
}

static int _ad_pvmp3_close(ad_cls_t *o)
{
    struct ad_pvmp3_priv *priv = o->priv;

    aos_free(priv->obuf);
    aos_free(priv->dbuf);
    aos_free(priv);
    o->priv = NULL;

    return 0;
}

```

