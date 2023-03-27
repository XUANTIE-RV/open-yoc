/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef __RECORD_PROCESS_H__
#define __RECORD_PROCESS_H__

#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 单路PCM，上云数据 */
int voice_get_pcm_data(void *data, int len);

/* 单路PCM，关键词数据 */
int voice_get_kws_data(void *data, int len);

/* 5路PCM，2MIC + 1REF + 线性AEC后AGC前数据 */
int voice_get_feaec_data(void *data, int len, int timeout/*ms*/);

#ifdef __cplusplus
}
#endif

#endif