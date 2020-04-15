/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "aui_nlp.h"

#define TAG "app"

//#define WORD_PREFIX ("\\\"w\\\":\\\"")
#define WORD_PREFIX ("\"w\":\"")

int aui_nlp_process_xf_rtasr(cJSON *js, const char *json_text)
{
    int len = 0;
    char *next;
    char buf[128] = {0};
    LOGD(TAG, "Enter %s", __FUNCTION__);

    cJSON *code  = cJSON_GetObjectItem(js, "code");
    cJSON *data  = cJSON_GetObjectItem(js, "data");
    if (!(code && cJSON_IsString(code))) {
        LOGD(TAG, "results code error");
        return -1;
    }

    if (atoi(code->valuestring) != 0) {
        LOGD(TAG, "code = %s", code->valuestring);
        return -1;
    }

    if (!(data && data->valuestring)) {
        LOGD(TAG, "results data error");
        return -1;
    }

    next = strstr(data->valuestring, WORD_PREFIX);
    while (next) {
        next += strlen(WORD_PREFIX);
        //printf("next = %s\n", next);
        while ((*next != '"') && (len < sizeof(buf) - 1)) {
            buf[len++] = *next;
            next++;
        }
        next = strstr(next, WORD_PREFIX);
    }
    LOGI(TAG, "parse success, buf = %s, len = %d\n", buf, len);

    return len ? app_aui_cloud_push_text(buf) : -1;
}

