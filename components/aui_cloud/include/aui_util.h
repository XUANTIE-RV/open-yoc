/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __CLOUD_COMMON_H__
#define __CLOUD_COMMON_H__

#include <aos/aos.h>
#include <yoc/aui_cloud.h>
#ifdef __cplusplus
extern "C" {
#endif

int http_url_encode(char *output, const char *input);

#ifdef __cplusplus
}
#endif

#endif
