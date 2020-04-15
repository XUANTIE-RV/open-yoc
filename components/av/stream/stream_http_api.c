/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */


#include "avutil/socket_rw.h"
#include "avutil/web.h"
#include "stream/stream_typedef.h"
#include "stream/stream_ioctl.h"

#define TAG                    "s_http_a"

#define HTTP_READ_TIMEOUT_MS   (6*1000)

struct http_priv_s {
    wsession_t *session;
};
static struct http_priv_s *priv = NULL;
static int32_t                   size; //length of the stream

int stream_http_open(const char *url, int mode)
{
    int rc;
    const char *val;

    wsession_t *session;
    LOGE(TAG,"_stream_http_open mode:%d",mode);

//    UNUSED(mode);
    priv = aos_zalloc(sizeof(struct http_priv_s));
    CHECK_RET_TAG_WITH_RET(NULL != priv, -1);

    session = wsession_create();
    CHECK_RET_TAG_WITH_RET(session != NULL, -1);

    rc = wsession_get(session, url, 3);
    if (rc) {
        LOGE(TAG, "wsession_get fail. rc = %d, code = %d, phrase = %s", rc, session->code, session->phrase);
        goto err;
    }

    val = dict_get_val(&session->hdrs, "Content-Length");
    CHECK_RET_TAG_WITH_GOTO(val != NULL, err);

    priv->session = session;
    size       = atoi(val);

    return 0;
err:
    wsession_destroy(session);
    aos_free(priv);
    return -1;
}

int stream_http_close(void)
{
    wsession_t *session = priv->session;
    LOGE(TAG,"_stream_http_close ");

    if (session) {
        wsession_close(session);
        wsession_destroy(session);
    }

    aos_free(priv);
    priv = NULL;
    return 0;
}

int stream_http_read( uint8_t *buf, size_t count)
{
    int fd = priv->session->fd;
    LOGE(TAG,"_stream_http_read count:%d",count);

    return sock_readn(fd, (char*)buf, count, HTTP_READ_TIMEOUT_MS);
}

int stream_http_write(const uint8_t *buf, size_t count)
{
    //TODO
    return -1;
}

int stream_http_control(int cmd, void *arg, size_t *arg_size)
{
    int ret = 0;
    //struct http_priv *priv = o->priv;
    LOGE(TAG,"_stream_http_control cmd:%d",cmd);

    switch (cmd) {
    //TODO
    case STREAM_CMD_GET_SIZE:
        break;
    default:
        //LOGE(TAG, "stream constrol cmd not support. cmd = %d\n", cmd);
        ret = -1;
    }

    return ret;
}

