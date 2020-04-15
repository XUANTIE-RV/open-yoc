## 简介

adicore是一个支持mp3、aac等编码格式的多核异构核间解码库(inter-core audio decoder)。 该解码库运行在应用程序处理器(ap)侧, 实际解码运算工作跑在协处理器(cp)侧。该库屏蔽了底层核间通信的细节，应用开发人员可直接基于该库使用芯片的多核异构解码运算能力。

###  核间解码使用示例

```c
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

    /* 初始化核间解码库 */
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

    /* 打开核间解码库 */
    hdl = adicore_open(&ash);
    CHECK_RET_TAG_WITH_GOTO(NULL != hdl, err);
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

    /* 核间解码一帧音频数据, 结果存放在avframe_t结构中 */
    ret = adicore_decode(priv->hdl, frame, got_frame, pkt);
    if (*got_frame) {
        o->ash.sf = frame->sf;
    }

    return ret;
}

static int _ad_ipc_close(ad_cls_t *o)
{
    struct ad_ipc_priv *priv = o->priv;

    /* 关闭核间解码库, 销毁相关资源 */
    adicore_close(priv->hdl);

    aos_free(priv);
    o->priv = NULL;
    return 0;
}
```

