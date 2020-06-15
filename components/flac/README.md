## 简介

FLAC(FreeLossless Audio Codec)是一套自由的音频压缩编解码器. 
不同于其它有损音频压缩格式，FLAC格式不会破坏任何原有的音频资讯。这种压缩类似于ZIP的方式，但FLAC是专门针对PCM音频特点设计的压缩格式，其压缩率要大于ZIP方式。FLAC是一种非专有的，不受专利影响，开放源码，并且完全免>费的无损音频压缩格式，它的编码算法相当成熟，已经通过了严格的测试，被很多软件以及硬件音频产品所支持，如支持大多数的操作系统，包括Windows、Unix类系统、Mac等等，应用在移动多媒体播放器、汽车音响、家用音响等等设备。
FLAC编解码复杂度比较低，对计算要求不高，在普通的硬件平台就可以轻松实现实时解码播放.
FLAC有专门的项目组维护，可以在https://xiph.org/flac下载完整的FLAC编解码源码，对于C开发环境，其对应的库为libFLAC。

## 相关接口

具体接口使用请参见stream_decoder.h中的说明。

## 如何使用

### flac解码示例

```c
#include "FLAC/stream_decoder.h"
#define DEC_IBUF_SIZE_MIN      (FLAC_FRAME_SIZE_DEFAULT)

struct ad_flac_priv {
    FLAC__StreamDecoder        *dec;
    struct flac_stream_info    si;
    uint8_t                    si_find;       ///< may be not have stream info

    uint8_t                    *ibuf;
    size_t                     ipos;          ///< pos of read or write
    size_t                     isize;         ///< valid data size
    size_t                     icap;          ///< cap size

    FLAC__FrameHeader          fhdr;
    FLAC__int32                *opbuf[2];     ///< output pointer: 2 channels max support.
};

static FLAC__StreamDecoderReadStatus _read_callback(const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], size_t *bytes, void *client_data)
{
    UNUSED(decoder);
    int rc = *bytes, remain;
    struct ad_flac_priv *priv = client_data;

    remain = priv->isize - priv->ipos;
    rc = rc > remain ? remain : rc;
    if (rc > 0) {
        memcpy(buffer, priv->ibuf + priv->ipos, rc);
        priv->ipos += rc;
        remain     -= rc;
        if (remain > 0) {
            memmove(priv->ibuf, priv->ibuf + priv->ipos, remain);
        }
        priv->isize = remain;
        priv->ipos  = 0;
    }
    *bytes = rc;

    return (rc == 0 ? FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM : FLAC__STREAM_DECODER_READ_STATUS_CONTINUE);
}

static FLAC__StreamDecoderWriteStatus _write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame,
        const FLAC__int32 * const buffer[], void *client_data)
{
    //FIXME:
    UNUSED(decoder);
    struct ad_flac_priv *priv   = client_data;

    priv->fhdr = frame->header;
    if (!(frame->header.blocksize && frame->header.channels && (frame->header.channels <= 2) && frame->header.bits_per_sample)) {
        LOGE(TAG, "write callback err: blocksize = %u, channels = %u, bits = %u", frame->header.blocksize,
             frame->header.channels, frame->header.bits_per_sample);
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }

    memcpy(priv->opbuf, buffer, sizeof(const FLAC__int32 * const) * frame->header.channels);
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void _metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data)
{
    UNUSED(decoder);
    if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
        struct ad_flac_priv *priv   = client_data;
        struct flac_stream_info *si = &priv->si;

        si->nb_samples = metadata->data.stream_info.total_samples;
        si->rate       = metadata->data.stream_info.sample_rate;
        si->channels   = metadata->data.stream_info.channels;
        si->bits       = metadata->data.stream_info.bits_per_sample;
    }
}

static void _error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
{
    UNUSED(decoder);
    UNUSED(client_data);
}

static int _flac_open(struct ad_flac_priv *priv, uint8_t *extradata, size_t extradata_size)
{
    int rc;
    FLAC__StreamDecoderInitStatus init_status;
    uint8_t *ibuf               = NULL;
    FLAC__StreamDecoder *dec    = NULL;
    struct flac_stream_info *si = &priv->si;

    dec = FLAC__stream_decoder_new();

    init_status = FLAC__stream_decoder_init_stream(
                      dec,
                      _read_callback,
                      NULL /* seek_callback */,
                      NULL /* tell_callback */,
                      NULL /* length_callback */,
                      NULL /* eof_callback */,
                      _write_callback,
                      _metadata_callback,
                      _error_callback,
                      (void *)priv);

    if (extradata_size && extradata && !priv->si_find) {
        rc = flac_parse_si((const uint8_t*)extradata, si);
        si->fsize_min = si->fsize_min ? si->fsize_min : FLAC_FRAME_SIZE_DEFAULT;
        si->fsize_max = si->fsize_max ? si->fsize_max : FLAC_FRAME_SIZE_DEFAULT;
        priv->si_find = 1;
    }

    //FIXME:
    if (!priv->ibuf) {
        size_t isize = si->fsize_max * 2;

        isize = isize > DEC_IBUF_SIZE_MIN ? isize : DEC_IBUF_SIZE_MIN;
        ibuf  = aos_zalloc(isize);

        priv->ibuf = ibuf;
        priv->icap = isize;
    }

    priv->dec = dec;
    return 0;
err:
    aos_free(ibuf);
    FLAC__stream_decoder_delete(dec);
    return -1;
}

static int _ad_flac_open(ad_cls_t *o)
{
    int rc;
    struct ad_flac_priv *priv;

    priv = aos_zalloc(sizeof(struct ad_flac_priv));

    rc = _flac_open(priv, o->ash.extradata, o->ash.extradata_size);
    if (priv->si_find) {
        struct flac_stream_info *si = &priv->si;
        o->ash.sf = sf_make_channel(si->channels) | sf_make_rate(si->rate) | sf_make_bit(si->bits) | sf_make_signed(1);
    }
    o->priv = priv;

    return 0;
err:
    aos_free(priv);
    return -1;
}

static int _ad_flac_decode(ad_cls_t *o, avframe_t *frame, int *got_frame, const avpacket_t *pkt)
{
    sf_t sf;
    int ret = -1, i;
    struct ad_flac_priv *priv = o->priv;
    FLAC__StreamDecoder *dec  = priv->dec;
    FLAC__FrameHeader *fhdr   = &priv->fhdr;

    if (pkt->len > priv->icap - priv->isize) {
        goto quit;
    }

    memcpy(priv->ibuf + priv->isize, pkt->data, pkt->len);
    priv->isize += pkt->len;

    memset(priv->opbuf, 0, sizeof(priv->opbuf));
    memset(fhdr, 0, sizeof(FLAC__FrameHeader));
    if (!FLAC__stream_decoder_process_single(dec)) {
        goto quit;
    }

    if (!(fhdr->channels == 1 || fhdr->channels == 2) && (fhdr->bits_per_sample == 8 || fhdr->bits_per_sample == 16)) {
        goto quit;
    }

    sf = sf_make_channel(fhdr->channels) | sf_make_rate(fhdr->sample_rate) | sf_make_bit(fhdr->bits_per_sample) | sf_make_signed(1);
    if (!((!o->ash.sf || o->ash.sf == sf) && sf && priv->opbuf[0])) {
        goto quit;
    }

    o->ash.sf         = sf;
    frame->sf         = sf;
    frame->nb_samples = fhdr->blocksize;

    ret = avframe_get_buffer(frame);
    if (ret < 0) {
        goto quit;
    }

    /* write decoded PCM samples */
    if (fhdr->channels == 1 && fhdr->bits_per_sample == 8) {
        int8_t *d = (int8_t*)frame->data[0];
        for(i = 0; i < fhdr->blocksize; i++) {
            *d++ = priv->opbuf[0][i];
        }
    } else if (fhdr->channels == 2 && fhdr->bits_per_sample == 8) {
        int8_t *d = (int8_t*)frame->data[0];
        for(i = 0; i < fhdr->blocksize; i++) {
            *d++ = priv->opbuf[0][i];
            *d++ = priv->opbuf[1][i];
        }
    } else if (fhdr->channels == 1 && fhdr->bits_per_sample == 16) {
        int16_t *d = (int16_t*)frame->data[0];
        for(i = 0; i < fhdr->blocksize; i++) {
            *d++ = priv->opbuf[0][i];
        }
    } else if (fhdr->channels == 2 && fhdr->bits_per_sample == 16) {
        int16_t *d = (int16_t*)frame->data[0];
        for(i = 0; i < fhdr->blocksize; i++) {
            *d++ = priv->opbuf[0][i];
            *d++ = priv->opbuf[1][i];
        }
    }
    *got_frame = 1;
    ret = pkt->len;

quit:
    return ret;
}

static int _ad_flac_control(ad_cls_t *o, int cmd, void *arg, size_t *arg_size)
{
    //TODO
    return 0;
}

static int _ad_flac_reset(ad_cls_t *o)
{
    int rc;
    struct ad_flac_priv *priv = o->priv;
    FLAC__StreamDecoder* dec  = priv->dec;

    FLAC__stream_decoder_delete(dec);
    priv->dec   = NULL;
    priv->isize = 0;
    priv->ipos  = 0;
    rc = _flac_open(priv, o->ash.extradata, o->ash.extradata_size);

    return rc;
}

static int _ad_flac_close(ad_cls_t *o)
{
    struct ad_flac_priv *priv = o->priv;
    FLAC__StreamDecoder* dec  = priv->dec;

    FLAC__stream_decoder_delete(dec);

    aos_free(priv);
    aos_free(priv->ibuf);
    o->priv = NULL;
    return 0;
}
```

