/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "stream/stream_cls.h"
#include "avutil/url_parse.h"

#define TAG                    "s_mem"

struct mem_priv {
    char       *data;
    int32_t    size;
    int32_t    offset;
    char       avcodec[16];
    char       avformat[16];
    int32_t    rate;
    int32_t    channel;
};

/* url example: mem://addr=765432&size=1024&avformat=mp3&acodec=mp3 */
static int _stream_mem_open(stream_cls_t *o, int mode)
{
    int rc;
    struct mem_priv *priv = NULL;

    UNUSED(mode);
    priv = aos_zalloc(sizeof(struct mem_priv));
    CHECK_RET_TAG_WITH_RET(NULL != priv, -1);

    rc = url_get_item_value_int(o->url, "addr", (int*)&priv->data);
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);
    rc = url_get_item_value_int(o->url, "size", &priv->size);
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);
    if (!((priv->data != 0) && ((priv->size > 0)))) {
        LOGE(TAG, "stream mem open fail. data = %d, size = %d", priv->data, priv->size);
        goto err;
    }

    url_get_item_value(o->url, "avformat", priv->avformat, sizeof(priv->avformat));
    url_get_item_value(o->url, "avcodec", priv->avcodec, sizeof(priv->avcodec));
    url_get_item_value_int(o->url, "rate", &priv->rate);
    url_get_item_value_int(o->url, "channel", &priv->channel);

    o->size = priv->size;
    o->priv = priv;

    return 0;
err:
    aos_free(priv);
    return -1;
}

static int _stream_mem_close(stream_cls_t *o)
{
    struct mem_priv *priv = o->priv;

    aos_free(priv);
    o->priv = NULL;
    return 0;
}

static int _stream_mem_read(stream_cls_t *o, uint8_t *buf, size_t count)
{
    int len = 0;
    struct mem_priv *priv = o->priv;

    len = priv->size - priv->offset;
    len = MIN(len, count);

    if (len > 0) {
        memcpy(buf, priv->data + priv->offset, len);
        priv->offset += len;
    }

    return len;
}

static int _stream_mem_write(stream_cls_t *o, const uint8_t *buf, size_t count)
{
    //TODO
    return -1;
}

static int _stream_mem_seek(stream_cls_t *o, int32_t pos)
{
    struct mem_priv *priv = o->priv;

    if ((pos >= 0) && (pos <= priv->size)) {
        priv->offset = pos;
        return 0;
    }

    return -1;
}

static int _stream_mem_control(stream_cls_t *o, int cmd, void *arg, size_t *arg_size)
{
    int ret = 0;
    struct mem_priv *priv = o->priv;

    switch (cmd) {
    case STREAM_CMD_GET_CODEC:
        snprintf((char*)arg, *arg_size, "%s", priv->avcodec);
        break;
    case STREAM_CMD_GET_FORMAT:
        snprintf((char*)arg, *arg_size, "%s", priv->avformat);
        break;
    case STREAM_CMD_GET_RATE:
        *(int32_t*)arg = priv->rate;
        break;
    case STREAM_CMD_GET_CHANNEL:
        *(int32_t*)arg = priv->channel;
        break;
    default:
        //LOGE(TAG, "%s, %d control failed. cmd = %d", __FUNCTION__, __LINE__, cmd);
        return -1;
    }

    return ret;
}

const struct stream_ops stream_ops_mem = {
    .name            = "mem",
    .type            = STREAM_TYPE_MEM,
    .enable_cache    = 0,
    .protocols       = { "mem", NULL },

    .open            = _stream_mem_open,
    .close           = _stream_mem_close,
    .read            = _stream_mem_read,
    .write           = _stream_mem_write,
    .seek            = _stream_mem_seek,
    .control         = _stream_mem_control,
};

