## 简介

eqicore，即核间音效均衡器(inter-core equalizer)。其可以调整各频段信号的增益值，用来控制不同频率的信号强弱，从而影响到音色。 该库运行在应用程序处理器(ap)侧, 实际equalizer运算工作跑在协处理器(cp)侧。该库屏蔽了底层核间通信的细节，应用开发人员可直接基于该库使用芯片的多核异构equalizer运算能力。

###  核间eq使用示例

```c
#define EQ_SEGMENT 3
eqfp_t bqfparams[EQ_SEGMENT] = {
    {0, EQF_TYPE_PEAK,  3.0, 0.667, 1000},
    {1, EQF_TYPE_PEAK, -13.0, 0.667, 2000 },
    {0, EQF_TYPE_PEAK, 3.0, 0.667, 3000 }
};

int _main(void)
{
    int i;
    eqicore_t *eq;
    int fs = 48000;
    int framelen = 512; //samples;
    short dataIn[512], dataOut[512];
    FILE *fp_in,*fp_out;
    /* 初始化核间eq */
    eqicore_init();
    /* 申请一个核间eq实例 */
    eq = eqicore_new(fs, EQ_SEGMENT);
    if (!eq)
        return -1;

    for (i=0; i< EQ_SEGMENT; i++) {
        /* 配置eq每段的过滤参数 */
        eqicore_set_param(eq, i, &bqfparams[i]);
    }
    /* 使能eq */
    eqicore_set_enable(eq, 1);

    if((fp_in = fopen("audio.pcm", "rb")) == NULL) {
        printf("open audio in file failed\n");
        return -1;
    }
    if((fp_out = fopen("audio_out.pcm", "wb")) == NULL) {
        printf("open audio out file failed\n");
        return -1;
    }
    while(fread(dataIn, 2, framelen, fp_in) == framelen) {
        /* eq处理每帧音频数据 */
        eqicore_process(eq, dataIn, dataOut, framelen);
        fwrite(dataOut,2,framelen,fp_out);
    }

    /* 释放核间eq实例, 销毁相关资源 */
    eqicore_free(eq);
    return 0;
}

```

