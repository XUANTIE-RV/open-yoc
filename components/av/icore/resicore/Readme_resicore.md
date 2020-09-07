## 简介

resicore，即核间音频重采样库(inter-core resampler)。 该库运行在应用程序处理器(ap)侧, 实际重采样运算工作跑在协处理器(cp)侧。该库屏蔽了底层核间通信的细节，应用开发人员可直接基于该库使用芯片的多核异构重采样运算能力。

###  核间重采样使用示例

```c
#include "resicore/resicore.h"

#define TAG                   "res_ipc"

struct res_ipc_priv {
    resicore_t                *hdl;
};

static int _resample_ipc_init(resx_t *r)
{
    int rc;
    resicore_t *hdl = NULL;
    struct res_ipc_priv *priv;
    /* 核间重采样初始化 */
    rc = resicore_init();
    CHECK_RET_TAG_WITH_RET(rc == 0, -1);
    
    priv = aos_zalloc(sizeof(struct res_ipc_priv));
    CHECK_RET_TAG_WITH_RET(priv, -1);
    /* 实例化一个核间重采样 */
    hdl = resicore_new(r->irate, r->orate, r->channels, r->bits);
    CHECK_RET_TAG_WITH_GOTO(hdl, err);
    
    priv->hdl  = hdl;
    r->priv     = priv;
    
    return 0;
err:
    aos_free(priv);
    return -1;
}

static int _resample_ipc_convert(resx_t *r, void **out, size_t nb_osamples, const void **in, size_t nb_isamples)
{
    int rc;
    struct res_ipc_priv *priv = r->priv;
    resicore_t *hdl           = priv->hdl;
    /* 对输入的一帧音频pcm做核间重采样转换 */
    rc = resicore_convert(hdl, out, nb_osamples, in, nb_isamples);

    return rc;
}

static int _resample_ipc_uninit(resx_t *r)
{
    struct res_ipc_priv *priv = r->priv;
    resicore_t *hdl           = priv->hdl;
	/* 销毁核间重采样实例 */
    resicore_free(hdl);
    aos_free(priv);
    r->priv = NULL;
    return 0;
}
```

## 使用注意事项

当前仅能对单声道数据做音频重采样。