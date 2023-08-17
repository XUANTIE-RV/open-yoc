/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _YOC_PCM_INPUT_H__
#define _YOC_PCM_INPUT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*mic_data_cb)(void *data, unsigned int len, void *arg);

/**
 * 注册Mic操作ops
 */
void pcm_input_register();

/**
 * pcm数据获取初始化
 */
int pcm_input_init(int bit_format, int sample_rate, int frame_ms, int chn_num);

/**
 * 注册Mic pcm数据透传接口
 */
int pcm_input_cb_register(mic_data_cb cb, void *arg);

/**
 * 去注册Mic pcm数据透传接口
 */
int pcm_input_cb_unregister(mic_data_cb cb);

#ifdef __cplusplus
}
#endif

#endif
