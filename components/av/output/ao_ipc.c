/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/common.h"
#include "output/ao_cls.h"
#include "aoicore.h"

#define TAG                    "ao_ipc"

#define AO_IPC_SEND_INTERVAL   (5) // ms. FIXME: for interrupt, ao_write quickly when ao_close

struct ao_ipc_priv {
    aoicore_t             *hdl;
};

static int _ao_ipc_open(ao_cls_t *o, sf_t sf)
{
    int rc;
    aoih_t ash;
    aoicore_t *hdl;
    unsigned int rate;
    struct ao_ipc_priv *priv = NULL;

    rate = sf_get_rate(sf);
    switch (rate) {
    case 8000 :
    case 16000:
    case 32000:

    case 12000:
    case 24000:
    case 48000:

    case 11025:
    case 22050:
    case 44100:
        break;
    default:
        LOGE(TAG, "rate is not support, rate = %d\n", rate);
        return -1;
    }

    priv = aos_zalloc(sizeof(struct ao_ipc_priv));
    CHECK_RET_TAG_WITH_RET(NULL != priv, -1);

    rc = aoicore_init();
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);

    ash.sf = sf;
    ash.id = ICORE_AO_ID_ALSA;
    hdl = aoicore_open(&ash);
    CHECK_RET_TAG_WITH_GOTO(NULL != hdl, err);

    o->sf     = sf;
    o->priv   = priv;
    priv->hdl = hdl;
    LOGI(TAG, " ao open");
    return 0;

err:
    aos_free(priv);
    return -1;
}

static int _ao_ipc_start(ao_cls_t *o)
{
    int ret = -1;
    struct ao_ipc_priv *priv = o->priv;

    ret = aoicore_start(priv->hdl);

    return ret;
}

static int _ao_ipc_stop(ao_cls_t *o)
{
    int ret = -1;
    struct ao_ipc_priv *priv = o->priv;

    ret = aoicore_stop(priv->hdl);

    return ret;
}

static int _ao_ipc_drain(ao_cls_t *o)
{
    int ret = -1;
    struct ao_ipc_priv *priv = o->priv;

    ret = aoicore_drain(priv->hdl);

    return ret;
}

static int _ao_ipc_close(ao_cls_t *o)
{
    struct ao_ipc_priv *priv = o->priv;

    aoicore_close(priv->hdl);

    aos_free(priv);
    o->priv = NULL;
    return 0;
}

#ifdef AO_IPC_SEND_INTERVAL
static int _ao_ipc_write(ao_cls_t *o, const uint8_t *buffer, size_t count)
{
    int ret = -1, rc = -1;
    int per_size;
    int remain, len, offset = 0;
    struct ao_ipc_priv *priv = o->priv;

    per_size = (sf_get_rate(o->sf) / 1000) * sf_get_frame_size(o->sf) * AO_IPC_SEND_INTERVAL;
    while ((offset < count) && (!o->interrupt)) {
        remain = count - offset;
        len = (remain > per_size) ? per_size : remain;
        rc = aoicore_write(priv->hdl, buffer + offset, len);
        if (rc != len) {
            break;
        }
        offset += rc;
    }

    ret = rc >= 0 ? offset : ret;
    ret = o->interrupt ? count : ret;

    return ret;
}
#else
static int _ao_ipc_write(ao_cls_t *o, const uint8_t *buffer, size_t count)
{
    int ret = -1;
    struct ao_ipc_priv *priv = o->priv;

    ret = aoicore_write(priv->hdl, buffer, count);

    return ret;
}
#endif

const struct ao_ops ao_ops_ipc = {
    .name          = "ipc",

    .open          = _ao_ipc_open,
    .start         = _ao_ipc_start,
    .stop          = _ao_ipc_stop,
    .drain         = _ao_ipc_drain,
    .close         = _ao_ipc_close,
    .write         = _ao_ipc_write,
};


