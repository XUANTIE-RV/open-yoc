/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <aos/kv.h>
#include <yoc/mic.h>
#include <ulog/ulog.h>
#include <uservice/uservice.h>

#include "player/app_player.h"
#include "aui_cloud/app_aui_cloud.h"
#include "event_mgr/app_event.h"
#include "wifi/app_net.h"
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
static unsigned int g_send_byte  = 0;
static uint32_t     g_wakeup_cnt = 0;

/* 状态处理 */
static int       session_state = SESSION_STATE_IDLE;
static mic_kws_t g_wk_info_bak = { MIC_WAKEUP_TYPE_NONE, 0, 0, 0, "" };

/* 唤醒词PCM数据 */
static int      g_wwv_data_len = 0;
static uint8_t *g_wwv_data     = NULL;

#ifdef CONFIG_ALG_ASR_LYEVA

#include <cJSON.h>

static aos_timer_t asr_timer = NULL;

int g_continue_talk_mode = 0;

struct asr_intent_map_t {
    const char *cmd;
    const char *intent;
    const char *slot;
} asr_intent_map[] = {
    {
        "打开空调",
        "CTL_ACC_POWER_CTRL",
        "开",
    },
    {
        "关闭空调",
        "CTL_ACC_POWER_CTRL",
        "关",
    },
    {
        "PMV模式",
        "CTL_ACC_MODE_PMV",
        NULL,
    },
    {
        "制冷模式",
        "CTL_ACC_MODE_COLD",
        NULL,
    },
    {
        "制热模式",
        "CTL_ACC_MODE_HOT",
        NULL,
    },
    {
        "除湿模式",
        "CTL_ACC_MODE_DEHUM",
        NULL,
    },
    {
        "送风模式",
        "CTL_ACC_MODE_WIND",
        NULL,
    },
    {
        "PMV模式",
        "CTL_ACC_MODE_PMV",
        "开",
    },
    {
        "制冷模式",
        "CTL_ACC_MODE_COLD",
        "开",
    },
    {
        "制热模式",
        "CTL_ACC_MODE_HOT",
        "开",
    },
    {
        "除湿模式",
        "CTL_ACC_MODE_DEHUM",
        "开",
    },
    {
        "送风模式",
        "CTL_ACC_MODE_WIND",
        "开",
    },
    {
        "升高温度",
        "CTL_ACC_TEMP_SET_INCREASE",
        NULL,
    },
    {
        "降低温度",
        "CTL_ACC_TEMP_SET_DECREASE",
        NULL,
    },
    {
        "温度调到最高",
        "CTL_ACC_TEMP_SET_MAX_MIN",
        "最高",
    },
    {
        "温度调到最低",
        "CTL_ACC_TEMP_SET_MAX_MIN",
        "最低",
    },
    {
        "温度调高%s度",
        "CTL_ACC_TEMP_SET_INCREASE",
        "SLOTS_NUM",
    },
    {
        "温度降低%s度",
        "CTL_ACC_TEMP_SET_DECREASE",
        "SLOTS_NUM",
    },
    { "温度设为%s度", "CTL_ACC_TEMP_SET", "SLOTS_NUM" },
    { "温度设为%s度", "CTL_ACC_TEMP_SET", "SLOTS_NUM" },
    {
        "增大风速",
        "CTL_ACC_WIND_SPEED_INCREASE",
        NULL,
    },
    {
        "减小风速",
        "CTL_ACC_WIND_SPEED_DECREASE",
        NULL,
    },
    {
        "最大风速",
        "CTL_ACC_WIND_SPEED_MAX_MIN",
        "最高",
    },
    {
        "最小风速",
        "CTL_ACC_WIND_SPEED_MAX_MIN",
        "最低",
    },
    {
        "高速风",
        "CTL_ACC_WIND_SPEED_HIGH",
        NULL,
    },
    {
        "中速风",
        "CTL_ACC_WIND_SPEED_MID",
        NULL,
    },
    {
        "低速风",
        "CTL_ACC_WIND_SPEED_LOW",
        NULL,
    },
    {
        "自动风",
        "CTL_ACC_WIND_SPEED_AUTO",
        NULL,
    },
    {
        "打开左右摆风",
        "CTL_ACC_SWING_LEFT_RIGHT",
        "开",
    },
    {
        "打开上下摆风",
        "CTL_ACC_SWING_UP_DOWN",
        "开",
    },
    {
        "关闭左右摆风",
        "CTL_ACC_SWING_LEFT_RIGHT",
        "关",
    },
    {
        "关闭上下摆风",
        "CTL_ACC_SWING_UP_DOWN",
        "关",
    },
    {
        "向中间吹",
        "CTL_ACC_WIND_DIRECT_MIDDLE",
        NULL,
    },
    {
        "风向两边吹",
        "CTL_ACC_WIND_DIRECT_LEFT_RIGHT",
    },
    {
        "向左吹风",
        "CTL_ACC_WIND_DIRECT_LEFT",
        NULL,
    },
    {
        "往左一点吹",
        "CTL_ACC_WIND_DIRECT_LEFT",
        NULL,
    },
    {
        "向右吹风",
        "CTL_ACC_WIND_DIRECT_RIGHT",
        NULL,
    },
    {
        "往右一点吹",
        "CTL_ACC_WIND_DIRECT_RIGHT",
        NULL,
    },
    {
        "向上吹风",
        "CTL_ACC_WIND_DIRECT_UP",
        NULL,
    },
    {
        "往上面一点吹",
        "CTL_ACC_WIND_DIRECT_UP",
        NULL,
    },
    {
        "向下吹风",
        "CTL_ACC_WIND_DIRECT_DOWN",
        NULL,
    },
    {
        "往下面一点吹",
        "CTL_ACC_WIND_DIRECT_DOWN",
        NULL,
    },
    {
        "打开辅热",
        "CTL_ACC_WIND_AUXILIARY_HEATING",
        "开",
    },
    {
        "关闭辅热",
        "CTL_ACC_WIND_AUXILIARY_HEATING",
        "关",
    },
    {
        "打开自清洁",
        "CTL_ACC_CLEAN",
        "开",
    },
    {
        "关闭自清洁",
        "CTL_ACC_CLEAN",
        "关",
    },
    {
        "打开静音",
        "CTL_ACC_MUTE",
        "开",
    },
    {
        "关闭静音",
        "CTL_ACC_MUTE",
        "关",
    },
    {
        "音量大点",
        "CTL_ACC_VOLUME_UP",
        NULL,
    },
    {
        "音量小点",
        "CTL_ACC_VOLUME_DOWN",
        NULL,
    },
    {
        "音量调到最大",
        "CTL_ACC_VOLUME_MAX_MIN",
        "最高",
    },
    {
        "音量调到最小",
        "CTL_ACC_VOLUME_MAX_MIN",
        "最低",
    },
    {
        "关闭语音",
        "CTL_ACC_VOICE",
        "关",
    },
    {
        "取消",
        "CTL_ACC_VOICE",
        NULL,
    },
    {
        "确定关闭",
        "CTL_ACC_VOICE",
        NULL,
    },
    {
        "恢复出厂",
        "CTL_ACC_FACTORY_RESET",
        NULL,
    },
    {
        "确定恢复",
        "CTL_ACC_FACTORY_RESET",
        NULL,
    },
    {
        "空调联网",
        "CTL_ACC_WIFI",
        NULL,
    },
    {
        "空调联网",
        "CTL_ACC_WIFI",
        "开",
    },
    {
        "打开强力",
        "CTL_ACC_BRUTE_FORCE",
        "开",
    },
    {
        "关闭强力",
        "CTL_ACC_BRUTE_FORCE",
        "关",
    },
    {
        "打开睡眠",
        "CTL_ACC_SLEEP",
        NULL,
    },
    {
        "打开睡眠",
        "CTL_ACC_SLEEP",
        "开",
    },
    {
        "关闭睡眠",
        "CTL_ACC_SLEEP",
        "关",
    },
    {
        "打开屏显",
        "CTL_ACC_SCREEN",
        "开",
    },
    {
        "关闭屏显",
        "CTL_ACC_SCREEN",
        "关",
    },
    {
        "向我吹",
        "CTL_ACC_SOUND_SOURCE",
        NULL,
    },
    {
        "不要向我吹",
        "CTL_ACC_SOUND_SOURCE_CLOSE",
        NULL,
    },
    {
        "打开锁定",
        "CTL_ACC_LOCK",
        "开",
    },
    {
        "关闭锁定",
        "CTL_ACC_LOCK",
        "关",
    },
    {
        "打开感应风",
        "CTL_ACC_REACTION",
        "开",
    },
    {
        "关闭感应风",
        "CTL_ACC_REACTION",
        "关",
    },
    {
        "健康模式",
        "CTL_ACC_HEALTH",
        NULL,
    },
    {
        "健康模式",
        "CTL_ACC_HEALTH",
        "开",
    },
    {
        "关闭健康模式",
        "CTL_ACC_HEALTH",
        "关",
    },
    {
        "打开水洗",
        "CTL_ACC_WASH",
        "开",
    },
    {
        "关闭水洗",
        "CTL_ACC_WASH",
        "关",
    },
    {
        "打开水洗",
        "CTL_ACC_WASH",
        NULL,
    },
    {
        "关闭水洗",
        "CTL_ACC_WASH",
        NULL,
    },
    {
        "水洗低风",
        "CTL_ACC_WASH_WIND_LOW",
        NULL,
    },
    {
        "水洗高风",
        "CTL_ACC_WASH_WIND_HIGH",
        NULL,
    },
    {
        "水洗低风",
        "CTL_ACC_WASH_WIND_LOW",
        "开",
    },
    {
        "水洗高风",
        "CTL_ACC_WASH_WIND_HIGH",
        "开",
    },
    {
        "打开人感跟随",
        "CTL_ACC_SENSE_FOLLOW",
        "开",
    },
    {
        "打开人感避开",
        "CTL_ACC_SENSE_ESCAPE",
        "开",
    },
    {
        "关闭人感跟随",
        "CTL_ACC_SENSE_FOLLOW",
        "关",
    },
    {
        "关闭人感避开",
        "CTL_ACC_SENSE_ESCAPE",
        "关",
    },
    {
        "关闭人感",
        "CTL_ACC_SENSE",
        "关",
    },
    {
        "打开健康除湿",
        "CTL_ACC_XERANSIS",
        "开",
    },
    {
        "关闭健康除湿",
        "CTL_ACC_XERANSIS",
        "关",
    },
    {
        "打开甲醛净化",
        "CTL_ACC_METHANAL",
        "开",
    },
    {
        "关闭甲醛净化",
        "CTL_ACC_METHANAL",
        "关",
    },
    {
        "负离子模式",
        "CTL_ACC_ANION",
        NULL,
    },
    {
        "负离子模式",
        "CTL_ACC_ANION",
        "开",
    },
    {
        "关闭负离子模式",
        "CTL_ACC_ANION",
        "关",
    },
    {
        "打开PM二点五净化",
        "CTL_ACC_PM25",
        "开",
    },
    {
        "关闭PM二点五净化",
        "CTL_ACC_PM25",
        "关",
    },
    {
        "打开加湿",
        "CTL_ACC_HUMIDIFICATION",
        "开",
    },
    {
        "关闭加湿",
        "CTL_ACC_HUMIDIFICATION",
        "关",
    },
    {
        "打开新风",
        "CTL_ACC_FRESH",
        "开",
    },
    {
        "关闭新风",
        "CTL_ACC_FRESH",
        "关",
    },
    {
        "打开蓝牙",
        "CTL_ACC_BLUETOOTH",
        "开",
    },
    {
        "关闭蓝牙",
        "CTL_ACC_BLUETOOTH",
        "关",
    },
    {
        "打开离线演示",
        "CTL_ACC_OFFLINE",
        "开",
    },
    {
        "退出离线演示",
        "CTL_ACC_OFFLINE",
        "关",
    },
    {
        "打开连续对话",
        "CTL_ACC_SESSION",
        "开",
    },
    {
        "关闭连续对话",
        "CTL_ACC_SESSION",
        "关",
    },
    { "不支持该意图", "OTHERS", NULL },
    { NULL, NULL, NULL },
};

static const char *intent_get(const char *intent, const char *slot)
{
    int         i                 = 0;
    static char intent_string[60] = { 0 };
    while (asr_intent_map[i].cmd) {
        /* 首先匹配意图 */
        if (0 == strncmp(asr_intent_map[i].intent, intent, strlen(asr_intent_map[i].intent))) {
            /* 没有槽位的情况或者槽位完全匹配的情况 */
            if ((asr_intent_map[i].slot == NULL && slot == NULL)
                || ((slot && asr_intent_map[i].slot
                     && 0 == strncmp(asr_intent_map[i].slot, slot, strlen(asr_intent_map[i].slot)))))
            {
                return asr_intent_map[i].cmd;
            }
            /* 槽位为数字的情况 */
            else if (slot && asr_intent_map[i].slot
                     && 0 == strncmp(asr_intent_map[i].slot, "SLOTS_NUM", strlen(asr_intent_map[i].slot)))
            {
                sprintf(intent_string, asr_intent_map[i].cmd, slot);
                return intent_string;
            }
        }

        i++;
    }

    return "不支持该意图";
}

static void process_asr(const char *asr_buf)
{
    cJSON *j_info = NULL;

    j_info = cJSON_Parse(asr_buf);
    if (j_info == NULL) {
        goto end;
    }

    cJSON *payload = cJSON_GetObjectItem(cJSON_GetArrayItem(j_info, 0), "payload");
    if (payload == NULL) {
        goto end;
    }

    cJSON *asrresult = cJSON_GetObjectItem(payload, "asrresult");
    if (asrresult == NULL) {
        goto end;
    }

    printf(">>>>>>>>语音: %s\r\n", asrresult->valuestring);

    cJSON *semantics = cJSON_GetArrayItem(cJSON_GetObjectItem(payload, "semantics"), 0);
    if (semantics == NULL) {
        goto end;
    }

    cJSON *intent = cJSON_GetObjectItem(semantics, "intent");
    if (intent == NULL) {
        goto end;
    }

    cJSON *slot_value = NULL;
    cJSON *slot       = cJSON_GetArrayItem(cJSON_GetObjectItem(semantics, "slots"), 0);
    if (slot) {
        slot_value = cJSON_GetObjectItem(cJSON_GetArrayItem(slot, 0), "value");
    }

    const char *intet_str = intent_get(
        intent->valuestring, slot_value ? (strlen(slot_value->valuestring) ? slot_value->valuestring : NULL) : NULL);

    printf(">>>>>>>>意图: %s\r\n", intet_str);

    if (0 == strncmp(intet_str, "打开连续对话", strlen("打开连续对话"))) {
        g_continue_talk_mode = 1;
        aos_timer_stop(&asr_timer);
        aui_mic_control(MIC_CTRL_ENABLE_ASR, 1);
    } else if (0 == strncmp(intet_str, "关闭连续对话", strlen("关闭连续对话"))) {
        g_continue_talk_mode = 0;
        aui_mic_control(MIC_CTRL_ENABLE_ASR, 0);
    }

end:
    cJSON_Delete(j_info);

    if (g_continue_talk_mode == 0) {
        aui_mic_control(MIC_CTRL_ENABLE_ASR, 0);
        aos_timer_stop(&asr_timer);
        LOGD(TAG, "disable asr");
    }
}

static void asr_timeout(void *arg1, void *arg2)
{
    LOGD(TAG, "asr timeout ,disable asr");
    aui_mic_control(MIC_CTRL_ENABLE_ASR, 0);
}

#endif

/* 接收到 MIC 事件 */
static void mic_evt_cb(int source, mic_event_id_t evt_id, void *data, int size)
{
    int        ret     = 0;
    mic_kws_t *wk_info = NULL;

    switch (evt_id) {
        case MIC_EVENT_PCM_DATA: {
            // if (session_state == SESSION_STATE_IDLE)
            //     break;
            // LOGD(TAG, "mic_evt_cb session pcm %d\n", size);

#if defined(CONFIG_SMART_SPEAKER_AT) && CONFIG_SMART_SPEAKER_AT
            app_at_pcm_data_out(data, size);
#endif
#if (defined(CONFIG_BOARD_WIFI) && CONFIG_BOARD_WIFI) || (defined(CONFIG_BOARD_ETH) && CONFIG_BOARD_ETH)
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
#endif
        } break;

        case MIC_EVENT_SESSION_START:
            /* 重新记录唤醒后上传的数据量 */
            g_send_byte   = 0;
            wk_info       = (mic_kws_t *)data;
            g_wk_info_bak = *wk_info; /* 保存全局变量，二次确认流程时可以从这里读取信息 */

            LOGI(TAG, "WAKEUP (%s)type:%d id:%d score:%d doa:%d cnt:%u", wk_info->word, wk_info->type, wk_info->id,
                 wk_info->score, wk_info->doa, ++g_wakeup_cnt);

#ifdef CONFIG_ALG_ASR_LYEVA
            printf(">>>>>>>>Wakeup: %s<<<<<<<<<\r\n", wk_info->word);

            printf(">>>>>>>>请说<<<<<<<<<\r\n");

            aui_mic_control(MIC_CTRL_ENABLE_ASR, 1);
            if (g_continue_talk_mode == 0) {
                aos_timer_stop(&asr_timer);
                aos_timer_start(&asr_timer);
                LOGD(TAG, "wakeup mode");
            } else if (g_continue_talk_mode == 1) {
                LOGD(TAG, "continue mode");
            }
#endif

#if 0 /* 打开后，关闭唤醒和云交互, 避免干扰, 用于录音训练 */
            LOGD(TAG, "Record mode, ignore wakeup process");
            return;
#endif

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

#if (defined(CONFIG_BOARD_WIFI) && CONFIG_BOARD_WIFI) || (defined(CONFIG_BOARD_ETH) && CONFIG_BOARD_ETH)
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
            ret = app_aui_cloud_start(0);
            if (ret != 0) {
                session_state = SESSION_STATE_IDLE;
                LOGE(TAG, "aui cloud pcm start err.");
                return;
            }
            aui_mic_control(MIC_CTRL_START_PCM);
#endif
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
                // app_aui_cloud_start(1); /* 云端切换到二次唤醒模式 */
                // app_aui_cloud_push_audio(data, size); /* 发送数据 */
                /* 保存数据,可导出作为调试使用 */
                if (g_wwv_data_len < size) {
                    g_wwv_data     = aos_realloc(g_wwv_data, size);
                    g_wwv_data_len = size;
                }
                memcpy(g_wwv_data, data, size);
            }
            break;
        case MIC_EVENT_SESSION_DOA:
            LOGD(TAG, "MIC_EVENT_SESSION_DOA %ld", (long)data);
            break;
        case MIC_EVENT_LOCAL_ASR:
            LOGI(TAG, "MIC_EVENT_LOCAL_ASR %s", (char *)data);

#ifdef CONFIG_ALG_ASR_LYEVA
            process_asr(data);
#endif
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

#ifdef CONFIG_ALG_ASR_LYEVA
    aos_timer_new_ext(&asr_timer, asr_timeout, NULL, 10000, 0, 0);

    aui_mic_control(MIC_CTRL_ENABLE_ASR, 1);
#endif

    return ret;
}

void app_wwv_get_data(uint8_t **data, int *size)
{
    *data = g_wwv_data;
    *size = g_wwv_data_len;
}
