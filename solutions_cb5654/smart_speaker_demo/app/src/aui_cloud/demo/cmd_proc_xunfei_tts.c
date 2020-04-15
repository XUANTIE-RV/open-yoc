/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include "aui_nlp.h"

#define TAG "app"

/**
 * 解析到"answer.text"，对文本进行tts播放
*/
int aui_nlp_proc_cb_tts_nlp(cJSON *js, const char *json_text)
{
    // static char *tts_service[] = {
    //     "weather",         "joke",   "openQA",          "datetimeX", "ZUOMX.queryCapital",
    //     "AIUI.idiomsDict", "poetry", "KLLI3.areaScaler", "calc", "video", "englishEveryday"};

    static char *tts_black_server[] = {"musicX"};

    LOGD(TAG, "Enter %s", __FUNCTION__);

    cJSON *intent  = xunfei_nlp_get_first_intent(js);
    cJSON *service = cJSON_GetObjectItemByPath(intent, "service");
    if (service == NULL) {
        return -1;
    }

    if (strcmp(service->valuestring, "story") == 0) {
        return -1;
    }

    int bfind = 1;
    for (int i = 0; i < sizeof(tts_black_server) / sizeof(char *); i++) {
        if (strcmp(service->valuestring, tts_black_server[i]) == 0) {
            bfind = 0;
            break;
        }
    }

    if (!bfind) {
        LOGD(TAG, "process_tts_nlp unknown service %s", service->valuestring);
        return -1;
    }

    cJSON *ai_answer = cJSON_GetObjectItemByPath(intent, "answer.text");

    if (!cJSON_IsString(ai_answer)) {
        LOGE(TAG, "answer.text type error");
        return -1;
    }

    LOGD(TAG, "aui_nlp_proc_cb_tts_nlp:%s", ai_answer->valuestring);

    int ret;
    aui_player_stop(MEDIA_SYSTEM);
    ret = app_aui_cloud_tts_run(ai_answer->valuestring, 0);

    return ret;
}
