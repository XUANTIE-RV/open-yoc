/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>

#include "app_main.h"

#define TAG "app"


cJSON *xunfei_nlp_get_first_intent(cJSON *js)
{
    cJSON *data_array = cJSON_GetObjectItem(js, "data");
    cJSON *intent     = NULL;

    if (data_array == NULL) {
        return NULL;
    }

    int arr_count = cJSON_GetArraySize(data_array);

    for (int i = 0; i < arr_count; i++) {
        cJSON *service_obj = cJSON_GetArrayItem(data_array, i);
        intent             = cJSON_GetObjectItem(service_obj, "intent");
        if (intent == NULL) {
            continue;
        }
        cJSON *service = cJSON_GetObjectItem(intent, "service");
        if (service && service->type == cJSON_String) {
            break;
        }
    }

    return intent;
}

cJSON *xunfei_nlp_find(cJSON *array, const char *key, const char *value)
{
    if (array == NULL || key == NULL || value == NULL) {
        return NULL;
    }

    if (array->type != cJSON_Array) {
        return NULL;
    }

    int arr_count = cJSON_GetArraySize(array);

    cJSON *find_obj = NULL;
    for (int i = 0; i < arr_count; i++) {
        find_obj     = cJSON_GetArrayItem(array, i);
        cJSON *itemv = cJSON_GetObjectItem(find_obj, key);
        if (cJSON_IsString(itemv) && strcmp(itemv->valuestring, value) == 0) {
            break;
        } else {
            find_obj = NULL;
        }
    }

    return find_obj;
}

cJSON *xunfei_nlp_find_service(cJSON *service_array, const char *service_name)
{
    if (service_array == NULL || service_name == NULL) {
        return NULL;
    }

    if (service_array->type != cJSON_Array) {
        return NULL;
    }

    int arr_count = cJSON_GetArraySize(service_array);

    cJSON *service_obj = NULL;
    for (int i = 0; i < arr_count; i++) {
        service_obj    = cJSON_GetArrayItem(service_array, i);
        cJSON *service = cJSON_GetObjectItemByPath(service_obj, "intent.service");
        if (cJSON_IsString(service) && strcmp(service->valuestring, service_name) == 0) {
            break;
        } else {
            service_obj = NULL;
        }
    }

    return service_obj;
}

int aui_nlp_process_pre_check(cJSON *js, const char *json_text)
{
    /* 文本本地快速匹配处理 */
    cJSON *cmd = cJSON_GetObjectItemByPath(js, "textcmd");
    if (cJSON_IsString(cmd)) {
        return 0;
    }

    /* 讯飞NLP */
    cJSON *intent  = xunfei_nlp_get_first_intent(js);
    cJSON *service = cJSON_GetObjectItemByPath(intent, "service");
    if (service != NULL) {
        return 0;
    }

    /* 讯飞rtasr */
    cJSON *rt  = cJSON_GetObjectItem(js, "action");
    if (cJSON_IsString(rt)) {
        return 0;
    }

    /* aliyun rasr */
    cJSON *aliyunrt  = cJSON_GetObjectItem(js, "identify");
    if (cJSON_IsString(aliyunrt)) {
        return 0;
    }

    /* 引擎获取到歌曲URL */
    cJSON *music_url = cJSON_GetObjectItemByPath(js, "music::url");

    if (cJSON_IsString(music_url)) {
        return 0;
    }

    /* AUI 错误返回*/
    cJSON *aui_ret  = cJSON_GetObjectItem(js, "aui_result");
    if (cJSON_IsNumber(aui_ret)) {
        if (aui_ret->valueint != 0) {
            return aui_ret->valueint;
        }
    }

    return -1;
}
