/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <aos/aos.h>
#include <ulog/ulog.h>

#include "recio.h"
#include "uri_parse.h"

#define TAG "blusb"

extern int blusbm_init(int chncnt);
extern int blusbm_write(const uint8_t *data, uint32_t data_len);

//驱动未提供卸载接口，确认启动后只初始化一次
static int g_blusb_inited = 0;

typedef struct _blusb_conf {
    int    chncnt;
} blusb_conf_t;

static int blusb_open(recio_t *io, const char *path)
{
    int value;

    blusb_conf_t *priv = aos_zalloc_check(sizeof(blusb_conf_t));
    if (priv == NULL) {
        LOGE(TAG, "blusb open e,blusb out");
        return -1;
    }

    if (uri_get_item_value_int(path, "chncnt", &value) == 0) {
        priv->chncnt = value;
    } else {
        goto err;
    }

    LOGD(TAG, "priv->chncnt: %d", priv->chncnt);
    io->private = (void *)priv;

    if (g_blusb_inited == 0) {
        if (blusbm_init(priv->chncnt) < 0) {
            goto err;
        }
        g_blusb_inited = 1;
    }

    return 0;
err:
    aos_free(priv);
    return -1;
}

static int blusb_close(recio_t *io)
{
    if (io->private) {
        aos_free(io->private);
    }
    return 0;
}

static int blusb_read(recio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    return -1;
}

static int blusb_write(recio_t *io, uint8_t *buffer, int length, int timeoutms)
{
    //blusb_conf_t *priv = (blusb_conf_t *)io->private;

    return blusbm_write((uint8_t *)buffer, length);
}

static int blusb_seek(recio_t *io, size_t offset, int whence)
{
    return 0;
}

const recio_cls_t blusb = {
    .name = "blusb",
    .open = blusb_open,
    .close = blusb_close,
    .write = blusb_write,
    .read = blusb_read,
    .seek = blusb_seek,
};

int recio_register_usb(const char *path)
{
    LOGD(TAG, "%s, %d, path:%s", __func__, __LINE__, path);

    return recio_register(&blusb, path);
}