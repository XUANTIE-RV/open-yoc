/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <aos/log.h>
#include <aos/debug.h>
#include <yoc/aui_cloud.h>
#include <cJSON.h>
#include "cloud_mit.h"

#define TAG "AUIMIT"

mit_account_info_t g_mit_account_info;

int aui_cloud_init(aui_t *aui)
{
    int ret;

    ret = mit_asr_init(aui);
    if (ret != 0) {
        LOGE(TAG, "asr start error");
        return -1;
    }

    ret = mit_tts_init(aui);
    if (ret != 0) {
        LOGE(TAG, "tts start error");
        return -1;
    }

    return 0;
}

int aui_cloud_set_account(const char *json_account_info)
{
    cJSON *j_info           = NULL;
    j_info                  = cJSON_Parse(json_account_info);
    cJSON *device_uuid      = cJSON_GetObjectItem(j_info, "device_uuid");
    cJSON *asr_app_key      = cJSON_GetObjectItem(j_info, "asr_app_key");
    cJSON *asr_token        = cJSON_GetObjectItem(j_info, "asr_token");
    cJSON *asr_url          = cJSON_GetObjectItem(j_info, "asr_url");
    cJSON *tts_app_key      = cJSON_GetObjectItem(j_info, "tts_app_key");
    cJSON *tts_token        = cJSON_GetObjectItem(j_info, "tts_token");
    cJSON *tts_url          = cJSON_GetObjectItem(j_info, "tts_url");
    cJSON *tts_key_id       = cJSON_GetObjectItem(j_info, "tts_key_id");
    cJSON *tts_key_secret   = cJSON_GetObjectItem(j_info, "tts_key_secret");

    CHECK_RET_TAG_WITH_GOTO(
        j_info &&
        device_uuid && cJSON_IsString(device_uuid) &&
        asr_app_key && cJSON_IsString(asr_app_key) &&
        asr_token && cJSON_IsString(asr_token) &&
        asr_url && cJSON_IsString(asr_url) &&
        tts_app_key && cJSON_IsString(tts_app_key) &&
        tts_token && cJSON_IsString(tts_token) &&
        tts_url && cJSON_IsString(tts_url),
        ERR
    );

    g_mit_account_info.device_uuid  = device_uuid->valuestring;
    g_mit_account_info.asr_app_key  = asr_app_key->valuestring;
    g_mit_account_info.asr_token    = asr_token->valuestring;
    g_mit_account_info.asr_url      = asr_url->valuestring;
    g_mit_account_info.tts_app_key  = tts_app_key->valuestring;
    g_mit_account_info.tts_token    = tts_token->valuestring;
    g_mit_account_info.tts_url      = tts_url->valuestring;

    if ((tts_key_id && cJSON_IsString(tts_key_id)) && (tts_key_secret && cJSON_IsString(tts_key_secret))) {
        g_mit_account_info.tts_key_id      = tts_key_id->valuestring;
        g_mit_account_info.tts_key_secret  = tts_key_secret->valuestring;
    }

    return 0;

ERR:
    if (j_info) {
        cJSON_Delete(j_info);
    }
    
    return -1;
}

int aui_cloud_push_text(aui_t *aui, char *text)
{
    LOGE(TAG, "mit not support");
    return 0;
}
