/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <aos/debug.h>
#include <media.h>
#include <yoc/aui_cloud.h>
#include <avutil/named_straightfifo.h>

#include "cJSON.h"
#include "nui_things.h"
#include "nls_nui_things_tts.h"
#include "nls_nui_things_config.h"
#include "cloud_mit.h"

#define MIT_TTS_APP_KEY "9a7f47f2"
#define MIT_TTS_TOKEN "a2f8b80e04f14fdb9b7c36024fb03f78"
#define MIT_TTS_URL "wss://nls-gateway-inner.aliyuncs.com/ws/v1"

#define TAG "MIT_TTS"

static bool  tts_running = false;
static int   cloud_connected = 0;


#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

static int mit_tts_event_cb(void *user_data, NlsNuiThingsTTSEvent event, const char *event_response,
                            const int event_length)
{
    int ret = 0;
    aui_t *aui = (aui_t *)user_data;

    if (!tts_running && event != kNlsNuiThingsTTSEventTTSError) {
        LOGE(TAG, "tts not inited");
        return -1;
    }

    switch (event) {
        case kNlsNuiThingsTTSEventTTSStart:
            LOGD(TAG, "TTSStart");
            break;

        case kNlsNuiThingsTTSEventTTSData:

            //LOGD(TAG, "TTSData %d", event_length);
            if (NULL == event_response || event_length <= 0) {
                LOGE(TAG, "response length err %d", event_length);
                return ret;
            }

            if (aui->cb.tts_cb) {
                char text[60] = {0};
                snprintf(text, 60, "{\"aui_tts_state\":\"%d\",\"data\":\"%d\",\"len\":\"%d\"}", \
                                    AUI_TTS_PLAYING, (int)event_response, event_length);
                aui->cb.tts_cb(text, strlen(text), aui->cb.tts_priv);
            }
            break;

        case kNlsNuiThingsTTSEventTTSEnd:
            tts_running = false;
            if (aui->cb.tts_cb) {
                char text[40] = {0};
                snprintf(text, 40, "{\"aui_tts_state\":\"%d\"}", AUI_TTS_FINISH);
                aui->cb.tts_cb(text, strlen(text), aui->cb.tts_priv);
            }

            LOGD(TAG, "TTSEnd");
            goto CB_END;
            break;

        case kNlsNuiThingsTTSEventTTSError:
            LOGE(TAG, "tts error");
            tts_running = false;
            if (aui->cb.tts_cb) {
                char text[40] = {0};
                snprintf(text, 40, "{\"aui_tts_state\":\"%d\"}", AUI_TTS_ERROR);
                aui->cb.tts_cb(text, strlen(text), aui->cb.tts_priv);
            }
            goto CB_END;
            break;

        default:
            LOGW(TAG, "TTS unknow event %d\n", event);
            break;
    }

    //LOGD(TAG, "%s return", __FUNCTION__);
    return ret;

/* 结束播放 */
CB_END:
    cloud_connected = 0;
    nls_nui_things_tts_disconnect();
    LOGD(TAG, "tts closed");
    return ret;
}

static int mit_start_tts(aui_t *aui)
{
    int ret = 0;

    /* if connected in asr step, do not reconnect */
    if (!cloud_connected) {
        LOGD(TAG, "tts reconnect");
        ret = nls_nui_things_tts_connect(NULL);
        if (ret == 0) {
            cloud_connected = 1;
        }
    }

    return ret;
}

static int mit_stop_tts(aui_t *aui)
{
    if (tts_running) {
        tts_running    = false;
        nls_nui_things_tts_stop(1);
    }

    if (cloud_connected) {
        cloud_connected = 0;
        nls_nui_things_tts_disconnect();
    }

    return 0;
}

/**
 * 对文本进行tts播放
*/
static int mit_req_tts(aui_t *aui, const char *text)
{
    int ret = -1;

    // (void)player_fifo_name;

    LOGD(TAG, "Enter %s", __FUNCTION__);
    LOGD(TAG, "text:%s", text);

    if (!text || strlen(text) == 0) {
        LOGD(TAG, "text input none");
        return -3;
    }

    nls_nui_things_tts_stop(1);

    /* if not connected in asr, or connect breaks, reconnect */
    aui_cloud_start_tts(aui);

    ret = nls_nui_things_tts_start(text);
    if (0 != ret) {
        LOGE(TAG, "tts start failed %d", ret);
        return ret;
    }

    if (aui->cb.tts_cb) {
        char text[40] = {0};
        snprintf(text, 40, "{\"aui_tts_state\":\"%d\"}", AUI_TTS_INIT);
        aui->cb.tts_cb(text, strlen(text), aui->cb.tts_priv);
    }

    tts_running = true;

    return 0;
}

static int mit_tts_init(aui_t *aui)
{
    if (aui == NULL) {
        LOGE(TAG, "mit tts init");
        return -1;
    }

    NlsNuiThingsTTSListener mit_tts_listener = {mit_tts_event_cb, aui};
    NlsNuiThingsTTSConfig   mit_tts_config;

    memset(&mit_tts_config, 0, sizeof(mit_tts_config));
    mit_tts_config.app_key    = g_mit_account_info.tts_app_key;
    mit_tts_config.token      = g_mit_account_info.tts_token;
    mit_tts_config.url        = g_mit_account_info.tts_url;
    mit_tts_config.key_id     = g_mit_account_info.tts_key_id;
    mit_tts_config.key_secret = g_mit_account_info.tts_key_secret;

    mit_tts_config.sample_rate  = aui->config.srate;
    mit_tts_config.volume       = aui->config.vol;
    mit_tts_config.speech_rate  = aui->config.spd;
    mit_tts_config.pitch_rate   = aui->config.pit;
    mit_tts_config.voice        = aui->config.per;
    mit_tts_config.format       = aui->config.asr_fmt == 1 ? "pcm" : "mp3";
    mit_tts_config.path         = aui->config.tts_cache_path;
    mit_tts_config.cache_enable = aui->config.tts_cache_path ? 1 : 0;
    mit_tts_config.log_level    = 4;

    return nls_nui_things_tts_init(&mit_tts_listener, &mit_tts_config);
}

static aui_tts_cls_t g_tts_cls = {
    .init    = mit_tts_init,
    .start   = mit_start_tts,
    .stop    = mit_stop_tts,
    .req_tts = mit_req_tts
};

void aui_tts_register_mit(aui_t *aui, aui_tts_cb_t cb, void *priv)
{
    aos_check_param(aui);
    aui_cloud_tts_register(aui, &g_tts_cls, cb, priv);
}

__attribute__((weak)) int aos_log_tag(const char *tag, int log_level, const char *fmt, ...)
{
    return 0;
}