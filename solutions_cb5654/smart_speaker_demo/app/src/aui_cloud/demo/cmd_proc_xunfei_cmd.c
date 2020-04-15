/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "aui_nlp.h"

#define TAG "app"

/**
 * 讯飞NLP的媒体控制命令解析
*/
int aui_nlp_process_cmd(cJSON *js, const char *json_text)
{
    LOGD(TAG, "Enter %s", __FUNCTION__);

    cJSON *data_array = cJSON_GetObjectItem(js, "data");
    cJSON *service_obj = xunfei_nlp_find_service(data_array, "cmd");

    if (service_obj == NULL) {
        service_obj = xunfei_nlp_find_service(data_array, "musicX");

        if (service_obj == NULL) {
            LOGD(TAG, "No cmd service found");
            return -1;
        }
    }

    cJSON *sema_intent = cJSON_GetObjectItemByPath(service_obj, "intent.semantic[0].intent");
    if (!cJSON_IsString(sema_intent)) {
        LOGE(TAG, "sema.intent type error");
        return -1;
    }

    cJSON *slots_array = cJSON_GetObjectItemByPath(service_obj, "intent.semantic[0].slots");

    if (strcmp(sema_intent->valuestring, "INSTRUCTION") == 0) {
        cJSON *slot_obj = xunfei_nlp_find(slots_array, "name", "insType");
        cJSON *cmd = cJSON_GetObjectItemByPath(slot_obj, "value");
        if (!cJSON_IsString(cmd)) {
            LOGE(TAG, "slots[0].value type error");
            return -1;
        }

        /* 音量及播放控制 */
        LOGD(TAG, "process_cmd:%s", cmd->valuestring);
        if (strcmp(cmd->valuestring, "volume_plus") == 0) {
            app_volume_inc(1);
        } else if (strcmp(cmd->valuestring, "volume_minus") == 0) {
            app_volume_dec(1);
        } else if (strcmp(cmd->valuestring, "mute") == 0) {
            // app_volume_mute();
        } else if (strcmp(cmd->valuestring, "replay") == 0) {
            app_player_resume();
        } else if (strcmp(cmd->valuestring, "pause") == 0) {
            app_player_pause();
        } else if (strcmp(cmd->valuestring, "close") == 0) {
            app_player_pause();
        }  else if (strcmp(cmd->valuestring, "volume_max") == 0) {
            app_volume_set(100,1);
        } else if (strcmp(cmd->valuestring, "volume_min") == 0) {
            aui_player_mute(MEDIA_MUSIC);
        }
        else {
            LOGE(TAG, "unknown INSTRUCTION");
            return -1;
        }
    } else if (strcmp(sema_intent->valuestring, "SET") == 0) {
        cJSON *slot_obj = xunfei_nlp_find(slots_array, "name", "series");
        if (slot_obj == NULL) {
            slot_obj = xunfei_nlp_find(slots_array, "name", "percent");
        }
        cJSON *series_value = cJSON_GetObjectItemByPath(slot_obj, "value");
        if (cJSON_IsString(series_value)) {
            app_volume_set(atoi(series_value->valuestring), 1);
        } else {
            LOGE(TAG, "SET value type error");
            return -1;
        }
    } else {
        LOGE(TAG, "unknown sema_intent %s", sema_intent->valuestring);
        return -1;
    }

    return 0;
}
