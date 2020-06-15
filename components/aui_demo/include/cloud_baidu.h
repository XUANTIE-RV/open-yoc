/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#ifndef __CLOUD_BAIDU_H__
#define __CLOUD_BAIDU_H__

#include <yoc/aui_cloud.h>
#include <webclient.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum aui_cloud_err_code {
    AUI_ERR_FAIL = -1,
    AUI_ERR_OK = 0,
    AUI_ERR_HTTP,
    AUI_ERR_MEM,
    AUI_ERR_PARAM,
    AUI_ERR_JSON,
}aui_cloud_err_code_t;

/* baidu proxy */
#define BAIDU_ASR_IP "ai.c-sky.com"

#define BAIDU_ASR_PORT 8001

int baidu_music(aui_t *aui, char *text);

#ifdef __cplusplus
}
#endif

#endif