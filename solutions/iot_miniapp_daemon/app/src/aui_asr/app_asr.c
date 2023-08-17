/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifdef CONFIG_ALG_ASR_LYEVA

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/kernel.h>
#include <cJSON.h>
#include <yoc/mic.h>

#include "jsapi_publish.h"
#include "player/app_player.h"
#include "app_asr.h"

#define TAG "appasr"

static aos_timer_t asr_timer = NULL;

int g_continue_talk_mode = 0;

static cJSON *cJSON_GetObjectItemByPath(cJSON *object, const char *path)
{
    if (object == NULL) {
        return NULL;
    }

    char *dup_path = strdup(path);
    char *saveptr  = NULL;
    char *next     = strtok_r(dup_path, ".", &saveptr);

    cJSON *item = object;
    while (next) {
        char *arr_ptr = strchr(next, '[');
        if (arr_ptr) {
            *arr_ptr++ = '\0';
            item = cJSON_GetObjectItem(item, next);
            if (item) {
                item = cJSON_GetArrayItem(item, atoi(arr_ptr));
            }
        } else {
            item = cJSON_GetObjectItem(item, next);
        }
        if (item == NULL) {
            break;
        }

        next = strtok_r(NULL, ".", &saveptr);
    }

    free(dup_path);

    return item;
}

static void asr_timeout(void *arg1, void *arg2)
{
    LOGD(TAG, "asr timeout ,disable asr");
    aui_mic_control(MIC_CTRL_ENABLE_ASR, 0);
    //fix me ：disable asr后，很久之后才会触发MIC_EVENT_SESSION_STOP
    //优化mind_process.cc，收到thead::voice::proto::ENABLE_ASR，中止asr，发出thead::voice::proto::END
    jsapi_voice_publish_sessionEnd();
    local_audio_play("npl_nothing.mp3");
}

void asr_process_init()
{
    aos_timer_new_ext(&asr_timer, asr_timeout, NULL, 10000, 0, 0);
    aui_mic_control(MIC_CTRL_ENABLE_ASR, 1);
}

void asr_process_session_start(mic_kws_t *wk_info)
{
    aui_mic_control(MIC_CTRL_ENABLE_ASR, 1);
    if (g_continue_talk_mode == 0) {
        aos_timer_stop(&asr_timer);
        aos_timer_start(&asr_timer);
        LOGD(TAG, "wakeup mode");
    } else if (g_continue_talk_mode == 1) {
        LOGD(TAG, "continue mode");
    }
}

void app_aui_shortcutCMD_offline(const char *resultCMD)
{
    cJSON *js = cJSON_Parse(resultCMD);
    
    //c处理麦克风控制逻辑，页面处理业务逻辑
    jsapi_voice_publish_shortcutCMD(js , resultCMD);

    cJSON *asrResult = cJSON_GetObjectItemByPath(cJSON_GetArrayItem(js, 0), "payload.asrresult");
    if(cJSON_IsString(asrResult)){
        LOGD(TAG, "asrresult=%s\n", asrResult->valuestring);
        if (0 == strcmp(asrResult->valuestring, "打开连续对话")) {
            g_continue_talk_mode = 1;
            aos_timer_stop(&asr_timer);
            aui_mic_control(MIC_CTRL_ENABLE_ASR, 1);
        } else if (0 == strcmp(asrResult->valuestring, "关闭连续对话")) {
            g_continue_talk_mode = 0;
            aui_mic_control(MIC_CTRL_ENABLE_ASR, 0);
        }
    }

    cJSON_Delete(js);

    if (g_continue_talk_mode == 0) {
        aui_mic_control(MIC_CTRL_ENABLE_ASR, 0);
        //fix me ：disable asr后，很久之后才会触发MIC_EVENT_SESSION_STOP
        //优化mind_process.cc，收到thead::voice::proto::ENABLE_ASR，中止asr，发出thead::voice::proto::END
        aos_timer_stop(&asr_timer);
        jsapi_voice_publish_sessionEnd();
        LOGD(TAG, "disable asr");
    }
}

#endif
