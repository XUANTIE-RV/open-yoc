/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "stream/stream_cls.h"
#include "vfs.h"
#include "avutil/url_parse.h"

#define TAG "s_file"

struct file_priv {
    int        fd;
    char       *path;
    char       avcodec[16];
    char       avformat[16];
    int32_t    rate;
    int32_t    channel;
};

static char* _get_file_name(const char *url)
{
    int len, pre_len;
    char *p;
    char *path = NULL;

    pre_len = strlen("file://");
    p = strstr(url, "?");
    if (p) {
        len = p - url;
        if (len > pre_len) {
            path = strndup(url + pre_len, len - pre_len);
        }
    } else {
        path = strdup(url + pre_len);
    }

    return path;
}

/**
 * @brief
 * @param  [in] o
 * @param  [in] mode
 * url example: file://res/48k.m4a
 *              file://res/hello.raw?avformat=rawaudio&avcodec=pcm_s16le&channel=2&rate=44100
 * @return
 */
static int _stream_file_open(stream_cls_t *o, int mode)
{
    int ret = -1, fd;
    struct stat st;
    char *path = NULL;
    struct file_priv *priv = NULL;

    UNUSED(mode);
    priv = aos_zalloc(sizeof(struct file_priv));
    CHECK_RET_TAG_WITH_RET(NULL != priv, -1);

    path = _get_file_name(o->url);
    CHECK_RET_TAG_WITH_GOTO(path != NULL, err);

    ret = aos_stat(path, &st);
    CHECK_RET_TAG_WITH_GOTO(ret == 0, err);

    fd = aos_open(path, O_RDONLY);
    CHECK_RET_TAG_WITH_GOTO(fd > 0, err);

    url_get_item_value(o->url, "avformat", priv->avformat, sizeof(priv->avformat));
    url_get_item_value(o->url, "avcodec", priv->avcodec, sizeof(priv->avcodec));
    url_get_item_value_int(o->url, "rate", &priv->rate);
    url_get_item_value_int(o->url, "channel", &priv->channel);

    priv->fd   = fd;
    priv->path = path;
    o->size    = st.st_size;
    o->priv    = priv;

    return 0;
err:
    LOGE(TAG, "open fail. ret = %d, url = %s", ret, o->url);
    aos_free(path);
    aos_free(priv);
    return -1;
}

static int _stream_file_close(stream_cls_t *o)
{
    struct file_priv *priv = o->priv;

    aos_close(priv->fd);
    aos_free(priv);
    o->priv = NULL;

    return 0;
}

static int _stream_file_read(stream_cls_t *o, uint8_t *buf, size_t count)
{
    struct file_priv *priv = o->priv;

    return  aos_read(priv->fd, buf, count);
}

static int _stream_file_write(stream_cls_t *o, const uint8_t *buf, size_t count)
{
    //TODO
    return -1;
}

static int _stream_file_seek(stream_cls_t *o, int32_t pos)
{
    struct file_priv *priv = o->priv;

    if (!((pos >= 0) && (pos <= o->size))) {
        return -1;
    }

    return  aos_lseek(priv->fd, pos, 0);
}

static int _stream_file_control(stream_cls_t *o, int cmd, void *arg, size_t *arg_size)
{
    int ret = 0;
    struct file_priv *priv = o->priv;

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

const struct stream_ops stream_ops_file = {
    .name            = "file",
    .type            = STREAM_TYPE_FILE,
    .enable_cache    = 0,
    .protocols       = { "file", NULL },

    .open            = _stream_file_open,
    .close           = _stream_file_close,
    .read            = _stream_file_read,
    .write           = _stream_file_write,
    .seek            = _stream_file_seek,
    .control         = _stream_file_control,
};

