/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <aos/kv.h>
#include <yoc/mic.h>
#include <ulog/ulog.h>
#include <uservice/uservice.h>

#include "app_player.h"
#include "aui_cloud/app_aui_cloud.h"
#include "event_mgr/app_event.h"
#include "app_net.h"
#include "at_cmd/app_at_cmd.h"
#include "app_voice.h"

#define TAG "APPVOICE"

#define SESSION_STATE_IDLE  0
#define SESSION_STATE_START 1
#define SESSION_STATE_WWV   2

/*************************************************
 * 麦克风
 *************************************************/
/* 调试变量 */
static unsigned int g_send_byte    = 0;
static uint32_t     g_wakeup_cnt = 0;

/* 状态处理 */
static int       session_state = SESSION_STATE_IDLE;
static mic_kws_t g_wk_info_bak = { MIC_WAKEUP_TYPE_NONE, 0, 0, 0, "" };

/* 唤醒词PCM数据 */
static int g_wwv_data_len = 0;
static uint8_t * g_wwv_data = NULL;

/* 接收到 MIC 事件 */
static void mic_evt_cb(int source, mic_event_id_t evt_id, void *data, int size)
{
    int ret = 0;
    mic_kws_t *wk_info = NULL;

    switch (evt_id) {
        case MIC_EVENT_PCM_DATA: {
            // if (session_state == SESSION_STATE_IDLE)
            //     break;
            // LOGD(TAG, "mic_evt_cb session pcm %d\n", size);

            #if defined(CONFIG_SMART_SPEAKER_AT) && CONFIG_SMART_SPEAKER_AT
            app_at_pcm_data_out(data, size);
            #endif

            /* 麦克风数据，推到云端 */
            ret = app_aui_cloud_push_audio(data, size);
            if (ret < 0) {
                /* 数据推送错误 */
                session_state = SESSION_STATE_IDLE;
                LOGE(TAG, "cloud push pcm finish. state %d", session_state);
                // aui_mic_control(MIC_CTRL_STOP_PCM);
                ret = app_aui_cloud_stop(1);

                /* 网络检测 */
                if (app_network_internet_is_connected() == 0) {
                    LOGE(TAG, "mic evt ntp not synced");
                    app_event_update(EVENT_STATUS_WIFI_CONN_FAILED);
                } else {
                    if (ret < 0) {
                        app_event_update(EVENT_STATUS_NLP_UNKNOWN);
                    }
                }
            }
            g_send_byte += size;
        } break;

        case MIC_EVENT_SESSION_START:
            app_event_update(EVENT_STATUS_SESSION_STOP);
            /* 重新记录唤醒后上传的数据量 */
            g_send_byte = 0;
            wk_info       = (mic_kws_t *)data;
            g_wk_info_bak = *wk_info; /* 保存全局变量，二次确认流程时可以从这里读取信息 */

            LOGI(TAG,
                 "WAKEUP (%s)type:%d id:%d score:%d doa:%d cnt:%u",
                 wk_info->word,
                 wk_info->type,
                 wk_info->id,
                 wk_info->score,
                 wk_info->doa,
                 ++g_wakeup_cnt);

            #if defined(CONFIG_SMART_SPEAKER_AT) && CONFIG_SMART_SPEAKER_AT
            app_at_session_start(wk_info->word);
            #endif

            /* 判断是否进入二次确认 */
            if (app_wwv_get_confirm()) {
                session_state = SESSION_STATE_WWV;
                LOGD(TAG, "WWV process, wait check result");
                return;
            }
            /* 如果不是二次唤醒，继续执行后续代码 */

        case MIC_EVENT_SESSION_WWV:
            wk_info = &g_wk_info_bak;

            if (evt_id == MIC_EVENT_SESSION_WWV) {
                /* 是二次确认返回结果 */
                LOGD(TAG, "WWV checked %ld\n", (long)data);
                if ((long)data == 0) {
                    return; /* 确认失败返回 */
                }
            }

            /*唤醒事件*/
            app_speaker_mute(0);
            if (SMTAUDIO_STATE_MUTE == smtaudio_get_state()) {
                LOGD(TAG, "Device is mute\n");
                return;
            }

            /* 网络检测 */
            if (app_network_internet_is_connected() == 0) {
                LOGE(TAG, "mic_evt net connect failed");
                aui_mic_control(MIC_CTRL_STOP_PCM);
                app_aui_cloud_stop(1);
                if (wifi_is_pairing()) {
                    app_event_update(EVENT_STATUS_WIFI_PROV_ALREADY_START);
                } else {
                    app_event_update(EVENT_STATUS_WIFI_CONN_FAILED);
                }
                return;
            }

            /* 开始交互 */
            session_state = SESSION_STATE_START;

            /* 停止上一次TTS请求, TTS通过MEDIA_SYSTEM播放 */
            smtaudio_stop(MEDIA_SYSTEM);
            app_aui_cloud_stop(0);
            app_aui_cloud_stop_tts();

            /* 确认是唤醒，更新状态 */
            app_event_update(EVENT_STATUS_SESSION_START);
            //event_publish(EVENT_GUI_USER_VOICEASRRESULT_UPDATE, "暂停");
            ret = app_aui_cloud_start(0);
            if (ret != 0) {
                session_state = SESSION_STATE_IDLE;
                LOGE(TAG, "aui cloud pcm start err.");
                return;
            }
            aui_mic_control(MIC_CTRL_START_PCM);
            break;

        case MIC_EVENT_SESSION_STOP:
            /* 交互结束 */
            LOGD(TAG, "MIC_EVENT_SESSION_STOP, Send_Bytes = %d bytes", g_send_byte);
            app_event_update(EVENT_STATUS_SESSION_STOP);

            if (session_state != SESSION_STATE_IDLE) {
                app_aui_cloud_stop(0);
                aui_mic_control(MIC_CTRL_STOP_PCM);
                session_state = SESSION_STATE_IDLE;
                #if defined(CONFIG_SMART_SPEAKER_AT) && CONFIG_SMART_SPEAKER_AT
                app_at_session_stop();
                #endif
            }
            break;

        case MIC_EVENT_KWS_DATA:
            LOGD(TAG, "MIC_EVENT_KWS_DATA %p %d", data, size);
            if (session_state == SESSION_STATE_WWV) {
                //app_aui_cloud_start(1); /* 云端切换到二次唤醒模式 */
                //app_aui_cloud_push_audio(data, size); /* 发送数据 */
                /* 保存数据,可导出作为调试使用 */
                if (g_wwv_data_len < size) {
                    g_wwv_data = aos_realloc(g_wwv_data, size);
                    g_wwv_data_len = size;
                }
                memcpy(g_wwv_data, data, size);
            } break;
        case MIC_EVENT_SESSION_DOA:
            LOGD(TAG, "MIC_EVENT_SESSION_DOA %ld", (long)data);
            break;
        default:;
    }
}

int app_mic_is_wakeup(void)
{
    return (session_state == SESSION_STATE_START);
}

int app_mic_init(void)
{
    int ret = 0;

    aui_mic_register();

    utask_t *task_mic = utask_new("task_mic", 10 * 1024, 20, AOS_DEFAULT_APP_PRI);
    ret               = aui_mic_init(task_mic, mic_evt_cb);

    aui_mic_start();

    return ret;
}

void app_wwv_get_data(uint8_t **data, int *size)
{
    *data = g_wwv_data;
    *size = g_wwv_data_len;
}
