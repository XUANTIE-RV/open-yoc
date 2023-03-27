/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef __PCM_INPUT_H__
#define __PCM_INPUT_H__

#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief http模拟采集数据
 *   采集数据流程中，可调用该接口从http服务器取数据，并覆盖
 * @return 从服务器取到的数据长度
 */
int voice_pcm_http_rewrite(void *data, int len);

#ifdef __cplusplus
}
#endif

#endif
