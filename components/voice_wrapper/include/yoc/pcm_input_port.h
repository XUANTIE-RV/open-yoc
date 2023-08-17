/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef __PCM_INPUT_PORT_H__
#define __PCM_INPUT_PORT_H__

#include "yoc/pcm_input.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 音频数据采集适配接口
 *        要求从硬件采集到的数据为交织数据
 */
typedef struct _pcm_input_ops {
    int (*init)(int bit_format, int sample_rate, int frame_ms, int chn_num); /* 模块初始化 */
    int (*pcm_acquire)(void *data, int len);                                 /* 音频裸数据获取接口 */
} pcm_input_ops_t;

/**
 * 注册Mic操作ops
 */
void pcm_acquire_register(pcm_input_ops_t *ops);

/**
 * 采集回调使能控制
 */
void pcm_acquire_set_enable(int en);

#ifdef __cplusplus
}
#endif

#endif
