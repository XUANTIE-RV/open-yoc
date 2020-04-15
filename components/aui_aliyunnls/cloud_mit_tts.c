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

static char *mit_tts_fifo = NULL;

#define TAG "MIT_TTS"

static bool        tts_running = false;
static bool        player_running = false;
static nsfifo_t *  aui_fifo    = NULL;
static aui_tts_cb_t  tts_stat_cb = NULL;

#define MIN(x, y) ((x) < (y) ? (x) : (y))

static int nsfifo_is_reof(nsfifo_t *fifo)
{
    int     ret;
    uint8_t reof;
    uint8_t weof;
    ret = nsfifo_get_eof(fifo, &reof, &weof);

    return (ret == 0) ? reof : 1;
}

static int mit_tts_event_cb(void *user_data, NlsNuiThingsTTSEvent event, const char *event_response,
                            const int event_length)
{
    int ret = 0;
    int total_len;

    if (!mit_tts_fifo || !tts_running) {
        LOGE(TAG, "tts not inited");
        return -1;
    }

    switch (event) {
        case kNlsNuiThingsTTSEventTTSStart:
            LOGD(TAG, "TTSStart");
            break;

        case kNlsNuiThingsTTSEventTTSData:
            if (!tts_running) {
                LOGE(TAG, "tts not running");
                return -1;
            }

            //LOGD(TAG, "TTSData %d", event_length);
            if (NULL == event_response || event_length <= 0) {
                LOGE(TAG, "response length err %d", event_length);
                return ret;
            }

            if (tts_stat_cb) {
                tts_stat_cb(AUI_TTS_CONTINUE);
            }

            if (!player_running) {
                if (aui_fifo) {
                    nsfifo_set_eof(aui_fifo, 0, 1); //set weof
                    aui_player_stop(MEDIA_SYSTEM);
                    nsfifo_close(aui_fifo);
                }
                aui_fifo = nsfifo_open(mit_tts_fifo, O_CREAT, 10 * 16 * 1024);

                ret = aui_player_play(MEDIA_SYSTEM, mit_tts_fifo, 1);
                if (ret < 0) {
                    nls_nui_things_tts_stop(1);
                    tts_running = false;
                    LOGD(TAG, "start player failed");
                    if (tts_stat_cb) {
                        tts_stat_cb(AUI_TTS_ERROR);
                    }
                    return -1;
                }
                player_running = true;
            }

            int   reof;
            char *pos;
            total_len = event_length;

            while (total_len > 0) {
                reof = nsfifo_is_reof(aui_fifo); /** peer read reach to end */
                if (reof) {
                    //LOGD(TAG, "named fifo read eof");
                    break;
                }

                int len = nsfifo_get_wpos(aui_fifo, &pos, 500);
                if (len <= 0) {
                    continue;
                }

                len = MIN(len, total_len);
                memcpy(pos, event_response + (event_length - total_len), len);
                nsfifo_set_wpos(aui_fifo, len);

                total_len -= len;
            }
            break;

        case kNlsNuiThingsTTSEventTTSEnd:
            tts_running = false;
            if (tts_stat_cb) {
                tts_stat_cb(AUI_TTS_FINISH);
            }
            LOGD(TAG, "TTSEnd");
            goto CB_END;
            break;

        case kNlsNuiThingsTTSEventTTSError:
            LOGE(TAG, "tts error");
            if (tts_stat_cb) {
                tts_stat_cb(AUI_TTS_ERROR);
            }
            tts_running = false;
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
    player_running = false;
    nsfifo_set_eof(aui_fifo, 0, 1); //set weof
    nls_nui_things_tts_disconnect();
    LOGD(TAG, "chunkfifo closed");
    return ret;
}

int aui_cloud_start_tts(aui_t *aui)
{
    return nls_nui_things_tts_connect(NULL);
}

int aui_cloud_stop_tts(aui_t *aui)
{
    if (tts_running) {
        tts_running = false;
        player_running = false;
        aui_player_stop(MEDIA_SYSTEM);
        nls_nui_things_tts_stop(1);
    }

    return nls_nui_things_tts_disconnect();
}

/**
 * 对文本进行tts播放
*/
int aui_cloud_req_tts(aui_t *aui, const char *player_fifo_name, const char *text, aui_tts_cb_t stat_cb)
{
    int ret = -1;

    LOGD(TAG, "Enter %s", "aui_cloud_req_tts");

    if (strlen(text) == 0) {
        LOGD(TAG, "text input none");
        return -3;
    }
    
    nls_nui_things_tts_stop(1);
    ret = nls_nui_things_tts_start(text);
    if (0 != ret) {
        LOGE(TAG, "tts start failed %d", ret);
        return ret;
    }

    tts_stat_cb = stat_cb;
    if (tts_stat_cb) {
        tts_stat_cb(AUI_TTS_INIT);
    }
    
    tts_running = true;
    player_running = false;
    return 0;
}

int mit_tts_init(aui_t *aui)
{
    if (aui == NULL) {
        LOGE(TAG, "mit tts init");
        return -1;
    }

    NlsNuiThingsTTSListener mit_tts_listener = {mit_tts_event_cb, NULL};
    NlsNuiThingsTTSConfig   mit_tts_config;

    memset(&mit_tts_config, 0, sizeof(mit_tts_config));
    mit_tts_config.app_key      = g_mit_account_info.tts_app_key;
    mit_tts_config.token        = g_mit_account_info.tts_token;
    mit_tts_config.url          = g_mit_account_info.tts_url;
    mit_tts_config.key_id       = g_mit_account_info.tts_key_id;
    mit_tts_config.key_secret   = g_mit_account_info.tts_key_secret;

    mit_tts_config.sample_rate  = aui->config.srate;
    mit_tts_config.volume       = aui->config.vol;
    mit_tts_config.speech_rate  = aui->config.spd;
    mit_tts_config.pitch_rate   = aui->config.pit;
    mit_tts_config.voice        = aui->config.per;
    mit_tts_config.format       = aui->config.fmt == 1 ? "pcm" : "mp3";
    mit_tts_config.secret       = NULL;
	mit_tts_config.path         = aui->config.tts_cache_path;
	mit_tts_config.cache_enable = aui->config.tts_cache_path ? 1 : 0;
    mit_tts_config.log_level    = 4;
    mit_tts_fifo = aui->config.fmt == 1 ? "fifo://mittts?avformat=rawaudio&avcodec=pcm_s16le&channel=1&rate=16000" : "fifo://mittts";

    return nls_nui_things_tts_init(&mit_tts_listener, &mit_tts_config);
}
