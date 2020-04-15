/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <yoc_config.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>

#include "app_main.h"

#define TAG "app-aliyunrasr"

#define ASR_NAME "ALIRASR"

int aui_nlp_process_aliyun_rasr(cJSON *js, const char *json_text)
{
    LOGD(TAG, "Enter %s", __FUNCTION__);

    cJSON *identify  = cJSON_GetObjectItem(js, "identify");
    cJSON *code  = cJSON_GetObjectItem(js, "code");
    cJSON *data  = cJSON_GetObjectItem(js, "data");
    if (!(identify && code && cJSON_IsString(code))) {
        LOGD(TAG, "results code error");
        return -1;
    }

    if (atoi(code->valuestring) != 0) {
        LOGD(TAG, "code = %s", code->valuestring);
        return -1;
    }

    if (!(identify && identify->valuestring && strcmp(identify->valuestring, ASR_NAME) == 0)) {
        LOGD(TAG, "identify is not %s", ASR_NAME);
        return -1;
    }   

    if (!(data && data->valuestring)) {
        LOGD(TAG, "results data error");
        return -1;
    }

    LOGI(TAG, "parse success, buf = %s\n", data->valuestring);

    return aui_cloud_push_text(&g_aui_handler, data->valuestring);
}

