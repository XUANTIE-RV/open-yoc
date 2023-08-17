/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <sys/time.h>

#include <aos/kv.h>
#include <devices/wifi.h>
#include <av/avutil/named_straightfifo.h>
#include <cJSON.h>
#include <yoc/aui_cloud.h>
#include <yoc/mic.h>

#include "aui_cloud/app_aui_cloud.h"
#include "aui_cloud/ex_cjson.h"
#include "aui_action/aui_action.h"
#include "player/app_player.h"
#include "event_mgr/app_event.h"

#define TAG "auinlp"

/* ai engine */
static aui_t      *g_aui_handler;
static int         g_initialized = 0;

static void nlp_handle(void *data, int len, void *priv)
{
    aui_audio_req_ack_info_t *req_ack_info = (aui_audio_req_ack_info_t *)data;

    if (req_ack_info->data_type == AUI_AUDIO_REQ_TYPE_WWV) {
        LOGD(TAG, "WWV JSON=%s\n", req_ack_info->data);
        cJSON *js    = cJSON_Parse(req_ack_info->data);
        cJSON *state = cJSON_GetObjectItem(js, "aui_kws_result");

        /* 二次确认后的处理 */
        int confirmed = atoi(state->valuestring);
        app_aui_cloud_wwv(confirmed);
        cJSON_Delete(js);
    } else if (req_ack_info->data_type == AUI_AUDIO_REQ_TYPE_NLP) {
        LOGD(TAG, "JSON=%s\n", req_ack_info->data);
        cJSON *js = cJSON_Parse(req_ack_info->data);
        cJSON *name = cJSON_GetObjectItemByPath(js, "header.name");
        //nlp only
        if( cJSON_IsString(name) && strcmp(name->valuestring, "DialogResultGenerated") == 0 ){
            app_aui_nlpEnd(req_ack_info->data);
        }
        //asr + 异常处理
        else{
            app_aui_shortcutCMD(js , req_ack_info->data);
        }
        cJSON_Delete(js);
    } else {
        LOGE(TAG, "unknown type %d", req_ack_info->data_type);
    }
}

static void tts_handle(void *data, int data_len, void *priv)
{
    smtaudio_stop(MEDIA_SYSTEM);
    smtaudio_start(MEDIA_SYSTEM, data, 0, 1);

    return;
}

/* ai engine init */
int app_aui_cloud_init(void)
{
    g_aui_handler = aui_cloud_init();

    aui_nlp_register(g_aui_handler, nlp_handle, g_aui_handler);
    aui_tts_register(g_aui_handler, tts_handle, g_aui_handler);

    g_initialized = 1;

    return 0;
}

int app_aui_cloud_push_audio(void *data, size_t size)
{
    if (g_initialized != 1) {
        return 0;
    }
    return aui_cloud_push_audio(g_aui_handler, data, size);
}

int app_aui_cloud_stop(int force_stop)
{
    if (g_initialized != 1) {
        return 0;
    }
    if (force_stop) {
        return aui_cloud_stop(g_aui_handler);
    }

    return aui_cloud_stop_audio(g_aui_handler);
}

int app_aui_cloud_start(int do_wwv)
{
    if (g_initialized != 1) {
        return 0;
    }
    return aui_cloud_start_audio(g_aui_handler, do_wwv ? AUI_AUDIO_REQ_TYPE_WWV : AUI_AUDIO_REQ_TYPE_NLP);
}

int app_aui_cloud_start_tts()
{
    return aui_cloud_start_tts(g_aui_handler);
}

int app_aui_push_wwv_data(void *data, size_t len)
{
    if (g_initialized != 1) {
        return 0;
    }
    aui_cloud_start_audio(g_aui_handler, AUI_AUDIO_REQ_TYPE_WWV);
    aui_cloud_push_audio(g_aui_handler, data, len);
    return 0;
}

int app_aui_cloud_stop_tts()
{
    return aui_cloud_stop_tts(g_aui_handler);
}

int app_aui_cloud_tts_run(const char *text, int wait_last)
{
    return aui_cloud_req_tts(g_aui_handler, text);
}

int app_aui_cloud_push_text(char *text)
{
    return aui_cloud_push_text(g_aui_handler, text);
}
