/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef __RECORD_PROCESS_H__
#define __RECORD_PROCESS_H__

#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif

int voice_get_pcm_data(void *data, int len);

int voice_get_kws_data(void *data, int len);

#ifdef __cplusplus
}
#endif

#endif