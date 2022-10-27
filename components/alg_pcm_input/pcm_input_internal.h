/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef __PCM_INPUT_H__
#define __PCM_INPUT_H__

#include <aos/aos.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief http模拟采集数据
 *   采集数据流程中，可调用该接口从http服务器取数据，并覆盖
 * @return 从服务器取到的数据长度
 */
int voice_pcm_http_rewrite(void *data, int len);

/**
 * @brief 音频数据采集适配接口
 *        要求从硬件采集到的数据为交织数据
 */
typedef struct _pcm_acquire_ops {
    int (*init)(int bit_format, int sample_rate, int frame_ms, int chn_num); /* 模块初始化 */
    int (*acquire)(void *data, int len);                                     /* 音频数据采集 */
} pcm_acquire_ops_t;

void voice_pcm_acquire_register(pcm_acquire_ops_t* ops);

extern pcm_acquire_ops_t g_pcm_acquire_ops;

#ifdef __cplusplus
}
#endif

#endif
