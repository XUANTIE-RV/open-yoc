/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "aui_nlp.h"

#define TAG "app-proxy-ws-asr"

#define ASR_NAME "PWSASR"

int aui_nlp_process_proxy_ws_asr(cJSON *js, const char *json_text)
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

    return app_aui_cloud_push_text(data->valuestring);
}

