/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#if defined(CONFIG_DECODER_IPC) && CONFIG_DECODER_IPC

#include "avutil/common.h"
#include "avcodec/ad_cls.h"
#include "icore/adicore_internal.h"
#include "adicore.h"

#define TAG                    "ad_ipc"

struct ad_ipc_priv {
    adicore_t             *hdl;
};

static int _ad_ipc_open(ad_cls_t *o)
{
    int rc;
    struct ad_ipc_priv *priv = NULL;
    adicore_t *hdl;
    adih_t ash;

    priv = aos_zalloc(sizeof(struct ad_ipc_priv));
    CHECK_RET_TAG_WITH_RET(NULL != priv, -1);

    rc = adicore_init();
    CHECK_RET_TAG_WITH_GOTO(rc == 0, err);

    memset(&ash, 0, sizeof(adih_t));
    if (o->ash.id == AVCODEC_ID_MP3) {
        ash.id = ICORE_CODEC_ID_MP3;
    } else if (o->ash.id == AVCODEC_ID_AAC) {
        ash.id = ICORE_CODEC_ID_AAC;
    } else {
        LOGE(TAG, "%s, %d faild. id = %d", __FUNCTION__, __LINE__, o->ash.id);
        goto err;
    }
    ash.extradata      = o->ash.extradata;
    ash.extradata_size = o->ash.extradata_size;

    hdl = adicore_open(&ash);
    CHECK_RET_TAG_WITH_GOTO(hdl, err);
    adicore_get_sf(hdl, &o->ash.sf);

    priv->hdl = hdl;
    o->priv   = priv;

    return 0;
err:
    aos_free(priv);

    return -1;
}

static int _ad_ipc_decode(ad_cls_t *o, avframe_t *frame, int *got_frame, const avpacket_t *pkt)
{
    int ret = -1;
    struct ad_ipc_priv *priv = o->priv;

    ret = adicore_decode(priv->hdl, frame, got_frame, pkt);
    if (*got_frame) {
        o->ash.sf = frame->sf;
    }

    return ret;
}

static int _ad_ipc_control(ad_cls_t *o, int cmd, void *arg, size_t *arg_size)
{
    int ret = -1;
    struct ad_ipc_priv *priv = o->priv;

    switch (cmd) {
    case CODEC_CMD_GET_SAMPLE_FORMAT:
        if (arg && arg_size && (*arg_size == sizeof(sf_t))) {
            sf_t sf = 0;

            ret = adicore_get_sf(priv->hdl, &sf);
            if (ret == 0) {
                *(sf_t*)arg = sf;
            }
        }
        break;
    default:
        //LOGE(TAG, "%s, %d control failed. cmd = %d", __FUNCTION__, __LINE__, cmd);
        return ret;
    }

    return ret;
}

static int _ad_ipc_reset(ad_cls_t *o)
{
    int rc;
    struct ad_ipc_priv *priv = o->priv;

    rc = adicore_reset(priv->hdl);

    return rc;
}

static int _ad_ipc_close(ad_cls_t *o)
{
    struct ad_ipc_priv *priv = o->priv;

    adicore_close(priv->hdl);

    aos_free(priv);
    o->priv = NULL;
    return 0;
}

const struct ad_ops ad_ops_ipc = {
    .name           = "ipc",
    .id             = AVCODEC_ID_AAC | AVCODEC_ID_MP3,

    .open           = _ad_ipc_open,
    .decode         = _ad_ipc_decode,
    .control        = _ad_ipc_control,
    .reset          = _ad_ipc_reset,
    .close          = _ad_ipc_close,
};

#endif

