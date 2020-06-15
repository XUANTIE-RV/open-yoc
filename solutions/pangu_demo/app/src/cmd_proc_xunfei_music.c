/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */


#include <stdlib.h>
#include <string.h>
#include <aos/log.h>
#include "cloud_baidu.h"
#include "app_main.h"
#include "audio/audio_res.h"

#define TAG "app"

/**
 * 对播放音乐请求的解析处理，在NLP Json中解析出音乐的作者及歌名，然后通过百度去搜索播放资源
 * 百度音乐资源搜索的协议 "music:艺术家名字+歌曲名字",该字符串传入aui_cloud_push_text
 * 继续解析
 */
int aui_nlp_process_music_nlp(cJSON *js, const char *json_text)
{
    LOGD(TAG, "Enter %s", __FUNCTION__);

    cJSON *data_array  = cJSON_GetObjectItem(js, "data");
    cJSON *service_obj = xunfei_nlp_find_service(data_array, "musicX");

    if (service_obj == NULL) {
        LOGD(TAG, "No musicX service found");
        return -1;
    }

    cJSON *semantic_intent = cJSON_GetObjectItemByPath(service_obj, "intent.semantic[0].intent");

    if (!cJSON_IsString(semantic_intent)) {
        LOGE(TAG, "type error");
        return -1;
    }

    if (strcmp(semantic_intent->valuestring, "PLAY") == 0) {
        char music_text[128] = "music:";
        cJSON *slots = cJSON_GetObjectItemByPath(service_obj, "intent.semantic[0].slots");

        cJSON *slot_artist_obj = xunfei_nlp_find(slots, "name", "artist");
        if (slot_artist_obj == NULL) {
            slot_artist_obj = xunfei_nlp_find(slots, "name", "band");
        }

        if (slot_artist_obj == NULL) {
            slot_artist_obj = xunfei_nlp_find(slots, "name", "tags");
        }

        cJSON *slot_artist_value = cJSON_GetObjectItem(slot_artist_obj, "value");
        if (cJSON_IsString(slot_artist_value)) {
            strcat(music_text, slot_artist_value->valuestring);
        }

        cJSON *slot_song_obj = xunfei_nlp_find(slots, "name", "song");
        cJSON *slot_song_value = cJSON_GetObjectItem(slot_song_obj, "value");
        if (cJSON_IsString(slot_song_value)) {
            strcat(music_text, "+");
            strcat(music_text, slot_song_value->valuestring);
        }

        LOGD(TAG, "process_music_nlp:%s", music_text);
        if (baidu_music(&g_aui_handler, music_text) < 0) {
            LOGE(TAG, "baidu_music");
            return -1;
        }

    } else if (strcmp(semantic_intent->valuestring, "RANDOM_SEARCH") == 0) {
        if (baidu_music(&g_aui_handler, "music:") < 0) {
            LOGE(TAG, "baidu_music");
            return -1;
        }
    } else if (strcmp(semantic_intent->valuestring, "INSTRUCTION") == 0) {
        cJSON *slots_array = cJSON_GetObjectItemByPath(service_obj, "intent.semantic[0].slots");
        cJSON *slot_instype_obj = xunfei_nlp_find(slots_array, "name", "insType");
        cJSON *slot0_value = cJSON_GetObjectItemByPath(slot_instype_obj, "value");
        if (!cJSON_IsString(slot0_value)) {
            LOGE(TAG, "type error");
            return -1;
        }

        if (strcmp(slot0_value->valuestring, "volume_select") == 0) {
            cJSON *slot_series_obj = xunfei_nlp_find(slots_array, "name", "series");
            if (slot_series_obj == NULL) {
                slot_series_obj = xunfei_nlp_find(slots_array, "name", "percent");
            }
            cJSON *series_value = cJSON_GetObjectItemByPath(slot_series_obj, "value");
            if (cJSON_IsString(series_value)) {
                app_volume_set(atoi(series_value->valuestring), 1);
            } else {
                 LOGD(TAG, "series value type error");
            }
        } else if (strcmp(slot0_value->valuestring, "repeat") == 0) {
            app_player_resume();
        } else {
            LOGD(TAG, "process_music unknown INSTRUCTION:%s", slot0_value->valuestring);
            return -1;
        }
    } else {
        LOGD(TAG, "process_music unknown semantic_intent:%s", semantic_intent->valuestring);
        return -1;
    }

    return 0;
}
