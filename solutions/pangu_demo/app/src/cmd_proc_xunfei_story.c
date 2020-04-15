/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <yoc_config.h>

#include <stdlib.h>
#include <string.h>
#include <aos/log.h>
#include <media.h>

#include "app_main.h"

#define TAG "app"

/**
 * 解析到""data.result.playUrl ，播放故事
*/
int aui_nlp_proc_cb_story_nlp(cJSON *js, const char *json_text)
{
    LOGD(TAG, "Enter %s", __FUNCTION__);

    cJSON *intent  = xunfei_nlp_get_first_intent(js);
    cJSON *service = cJSON_GetObjectItemByPath(intent, "service");
    if (service == NULL) {
        return -1;
    }

    if (strcmp(service->valuestring, "story") != 0) {
        return -1;
    }

    cJSON *ai_answer = cJSON_GetObjectItemByPath(intent, "data.result");

    if (ai_answer == NULL) {
        LOGE(TAG, "answer.data.result type error");
        return -1;
    }

    cJSON *play_obj = cJSON_GetArrayItem(ai_answer, 0);

    if (play_obj == NULL) {

        return -1;
    }

    cJSON *url = cJSON_GetObjectItem(play_obj, "playUrl");

   if (!cJSON_IsString(url)) {
        LOGE(TAG, "playUrl type error");
        return -1;
    }

    LOGD(TAG, "aui_nlp_proc_cb_tts_nlp:%s", url->valuestring);

    aui_player_stop(MEDIA_MUSIC);
    aui_player_play(MEDIA_MUSIC,url->valuestring, 1);

    return 0;
}
