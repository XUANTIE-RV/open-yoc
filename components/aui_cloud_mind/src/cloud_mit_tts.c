/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <stdbool.h>
#include <aos/debug.h>
#include <ulog/ulog.h>

#include <av/media.h>
#include <av/avutil/named_straightfifo.h>
#include <yoc/aui_cloud.h>

#include "nui_things.h"
#include "nls_nui_things_tts.h"
#include "nls_nui_things_config.h"
#include "cloud_mit.h"

typedef enum {
    TTS_STATE_IDLE = 0,
    TTS_STATE_RUN,
    TTS_STATE_PLAY
} tts_state_t;

/* tts event */
#define EVENT_TTS_FINISHED        (1 << 0)
#define EVENT_TTS_PLAYER_FINISHED (1 << 1)

#define TAG "MinDCloud"

static bool        g_tts_running     = false;
static int         g_cloud_connected = 0;
static tts_state_t g_tts_state       = TTS_STATE_IDLE;

static nsfifo_t             *g_aui_fifo     = NULL;
static char                 *g_mit_tts_fifo = NULL;
static NlsNuiThingsTTSConfig g_mit_tts_config;

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

static int nsfifo_is_reof(nsfifo_t *fifo)
{
    int     ret;
    uint8_t reof;
    uint8_t weof;
    ret = nsfifo_get_eof(fifo, &reof, &weof);

    return (ret == 0) ? reof : 1;
}

static int mit_tts_audio(void *data, int data_len, aui_t *aui)
{
    int total_len;

    if (TTS_STATE_RUN == g_tts_state) {
        if (g_aui_fifo) {
            nsfifo_set_eof(g_aui_fifo, 0, 1); // set weof
        }

        nsfifo_reset(g_aui_fifo);

        if (aui->cb.tts_cb) {
            aui->cb.tts_cb(g_mit_tts_fifo, strlen(g_mit_tts_fifo), aui->cb.tts_priv);
        }

        g_tts_state = TTS_STATE_PLAY;
        LOGD(TAG, "%s: g_tts_state set to %d", __FUNCTION__, g_tts_state);
    }

    int   reof;
    char *pos;
    total_len = data_len;

    while (total_len > 0) {
        reof = nsfifo_is_reof(g_aui_fifo); /** peer read reach to end */
        if (reof) {
            // LOGD(TAG, "named fifo read eof");
            break;
        }

        int len = nsfifo_get_wpos(g_aui_fifo, &pos, 500);
        if (len <= 0) {
            continue;
        }

        len = MIN(len, total_len);
        memcpy(pos, (char *)data + (data_len - total_len), len);
        nsfifo_set_wpos(g_aui_fifo, len);

        total_len -= len;
    }

    return 0;
}

static void mit_tts_handle(aui_tts_state_e mit_tts_state, void* data, int len, void *priv)
{
    aui_t *handler = (aui_t *)priv;

    if (mit_tts_state == AUI_TTS_PLAYING) {
        // LOGD(TAG, "%s tts state(%d) auido recv(%p) len(%d)", __func__, g_tts_state, data, len);
        mit_tts_audio(data, len, handler);
    } else {
        if (mit_tts_state == AUI_TTS_FINISH || mit_tts_state == AUI_TTS_ERROR) {
            LOGD(TAG, "tts end state=%d", mit_tts_state);
            nsfifo_set_eof(g_aui_fifo, 0, 1); // set weof
            g_tts_state = TTS_STATE_IDLE;
        }
    }
}

static int mit_tts_event_cb(void *user_data, NlsNuiThingsTTSEvent event, const char *event_response,
                            const int event_length)
{
    int    ret = 0;
    aui_t *aui = (aui_t *)user_data;

    if (!g_tts_running && event != kNlsNuiThingsTTSEventTTSError) {
        LOGE(TAG, "tts not inited");
        return -1;
    }

    switch (event) {
        case kNlsNuiThingsTTSEventTTSStart:
            LOGD(TAG, "TTSStart");
            break;

        case kNlsNuiThingsTTSEventTTSData:

            // LOGD(TAG, "TTSData %d", event_length);
            if (NULL == event_response || event_length <= 0) {
                LOGE(TAG, "response length err %d", event_length);
                return ret;
            }

            if (aui->cb.tts_cb) {
                mit_tts_handle(AUI_TTS_PLAYING, (void *)event_response, event_length, aui->cb.tts_priv);
            }
            break;

        case kNlsNuiThingsTTSEventTTSEnd:
            g_tts_running = false;
            if (aui->cb.tts_cb) {
                mit_tts_handle(AUI_TTS_FINISH, NULL, 0, aui->cb.tts_priv);
            }

            LOGD(TAG, "TTSEnd");
            goto CB_END;
            break;

        case kNlsNuiThingsTTSEventTTSError:
            LOGE(TAG, "tts error");
            g_tts_running = false;
            if (aui->cb.tts_cb) {
                mit_tts_handle(AUI_TTS_ERROR, NULL, 0, aui->cb.tts_priv);
            }
            goto CB_END;
            break;

        default:
            LOGW(TAG, "TTS unknow event %d\n", event);
            break;
    }

    // LOGD(TAG, "%s return", __FUNCTION__);
    return ret;

/* 结束播放 */
CB_END:
    g_cloud_connected = 0;
    nls_nui_things_tts_disconnect();
    LOGD(TAG, "tts closed");
    return ret;
}

static int mit_start_tts(aui_t *aui)
{
    int ret = 0;

    nsfifo_set_eof(g_aui_fifo, 0, 1); // set weof
    g_tts_state = TTS_STATE_IDLE;

    /* if connected in asr step, do not reconnect */
    if (!g_cloud_connected) {
        LOGD(TAG, "tts reconnect");
        ret = nls_nui_things_tts_connect(NULL);
        if (ret == 0) {
            g_cloud_connected = 1;
        } else {
            LOGD(TAG, "tts reconnect fail");
        }
    }

    return ret;
}

static int mit_stop_tts(aui_t *aui)
{
    g_tts_state = TTS_STATE_IDLE;

    if (g_tts_running) {
        g_tts_running = false;
        nls_nui_things_tts_stop(1);
    }

    if (g_cloud_connected) {
        g_cloud_connected = 0;
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

    LOGD(TAG, "Enter %s", __FUNCTION__);

    if (!text || strlen(text) == 0) {
        LOGD(TAG, "text input none");
        return -3;
    }

    nls_nui_things_tts_stop(1);

    /* if not connected in asr, or connect breaks, reconnect */
    mit_start_tts(aui);

    ret = nls_nui_things_tts_start(text);
    if (0 != ret) {
        LOGE(TAG, "tts start failed %d", ret);
        return ret;
    }

    if (aui->cb.tts_cb) {
        mit_tts_handle(AUI_TTS_INIT, NULL, 0, aui->cb.tts_priv);
    }

    g_tts_running = true;

    g_tts_state = TTS_STATE_RUN;

    int8_t retry_cnt = 0;
    while (TTS_STATE_RUN == g_tts_state) {
        aos_msleep(100);
        retry_cnt++;
        if (50 == retry_cnt) { // wait 50 * 100ms = 5s
            LOGE(TAG, "%s timeout!", __FUNCTION__);
            break;
        }
    }

    return 0;
}

static int mit_tts_init(aui_t *aui)
{
    if (aui == NULL) {
        LOGE(TAG, "mit tts init");
        return -1;
    }

    NlsNuiThingsTTSListener mit_tts_listener = { mit_tts_event_cb, aui, NULL };

    memset(&g_mit_tts_config, 0, sizeof(g_mit_tts_config));
    g_mit_tts_config.app_key    = g_mit_account_info.tts_app_key;
    g_mit_tts_config.token      = g_mit_account_info.tts_token;
    g_mit_tts_config.url        = g_mit_account_info.tts_url;
    g_mit_tts_config.key_id     = g_mit_account_info.tts_key_id;
    g_mit_tts_config.key_secret = g_mit_account_info.tts_key_secret;

    g_mit_tts_config.sample_rate  = 16000; /* 采样率，16000 */
    g_mit_tts_config.volume       = 100;   /* 音量 0~100 */
    g_mit_tts_config.speech_rate  = 0;     /* -500 ~ 500*/
    g_mit_tts_config.pitch_rate   = 0;     /* 音调*/
    g_mit_tts_config.voice        = "aixia";
    g_mit_tts_config.format       = "mp3" /* 支持 "pcm"和 "mp3" */;
    g_mit_tts_config.path         = NULL; /* TTS内部缓存路径，NULL：关闭缓存功能 */
    g_mit_tts_config.cache_enable = 0;
    g_mit_tts_config.log_level    = 4;

    extern int g_silence_log_level;
    g_silence_log_level = 5;

    /*对应g_mit_tts_config.format*/
    g_mit_tts_fifo = "fifo://mittts"; //"mp3"
    //g_mit_tts_fifo =  "fifo://mittts?avformat=rawaudio&avcodec=pcm_s16le&channel=1&rate=16000"; //"pcm"

    g_aui_fifo = nsfifo_open(g_mit_tts_fifo, O_CREAT, 10 * 16 * 1024);

    return nls_nui_things_tts_init(&mit_tts_listener, &g_mit_tts_config);
}

static aui_tts_cls_t g_tts_cls = {
    .init    = mit_tts_init,
    .start   = mit_start_tts,
    .stop    = mit_stop_tts,
    .req_tts = mit_req_tts
};

void aui_tts_register(aui_t *aui, aui_tts_cb_t cb, void *priv)
{
    aos_check_param(aui);
    aui_cloud_tts_register(aui, &g_tts_cls, cb, priv);
}
