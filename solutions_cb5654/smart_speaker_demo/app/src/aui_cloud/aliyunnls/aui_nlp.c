/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "aui_nlp.h"
#include "app_main.h"
#include <cJSON.h>
#include <sys/time.h>
#include <devices/wifi.h>

#define TAG "auinlp"

/* tts event */
#define EVENT_TTS_FINISHED              (1 << 0)
#define EVENT_TTS_PLAYER_FINISHED       (1 << 1)

/* wwv event */
#define EVENT_WWV_CONFIRMED             (1 << 0)
#define EVENT_WWV_REJECTED              (1 << 1)

/* tts running state */
#define TTS_STATE_IDLE                   0
#define TTS_STATE_RUN                    1
#define TTS_STATE_PLAY                   2

/* ai cmd */
static aui_cmd_set_t g_aui_nlp_process;

/* ai engine */
static aui_t        g_aui_handler;
static aos_event_t  event_tts_state;
static aos_event_t  event_wwv_result;
static int          tts_running = TTS_STATE_IDLE;
static int          wwv_enabled = 0;

/* 处理云端反馈的 NLP 文件，进行解析处理 */
static void aui_nlp_cb(const char *json_text)
{
    int ret;

    if (app_player_get_mute_state()) {
        printf("Device is mute\n");
        return;
    }

    // LOGI(TAG, "%s, json= %s\n", __func__, json_text);

    /* 处理的主入口, 具体处理见初始化注册的处理函数 */
    ret = aui_nlp_process_run(&g_aui_nlp_process, json_text);
    if (ret < 0) {
        LOGE(TAG, "%s, ret:%d", __func__, ret);
    }

    switch (ret) {
        case AUI_CMD_PROC_ERROR:
            local_audio_play(LOCAL_AUDIO_SORRY); /* 没听清楚 */
            break;

        case AUI_CMD_PROC_NOMATCH:
            local_audio_play(LOCAL_AUDIO_SORRY2); /* 不懂 */
            break;

        case AUI_CMD_PROC_MATCH_NOACTION:
            local_audio_play(LOCAL_AUDIO_SORRY2); /* 不懂 */
            break;

        case AUI_CMD_PROC_NET_ABNORMAL:
            local_audio_play(LOCAL_AUDIO_NET_FAIL); /* 网络 */
            break;
        default:;
    }
}

static void aui_tts_stat_cb(aui_tts_state_e stat)
{
    switch(stat) {
        case AUI_TTS_INIT:
            tts_running = TTS_STATE_RUN;
            break;

        case AUI_TTS_PLAYING:
            tts_running = TTS_STATE_PLAY;
            break;

        case AUI_TTS_FINISH:
            if (tts_running == TTS_STATE_RUN) {
                tts_running = TTS_STATE_IDLE;
            }
            aos_event_set(&event_tts_state, EVENT_TTS_FINISHED, AOS_EVENT_OR);
            break;

        case AUI_TTS_ERROR:
            tts_running = TTS_STATE_IDLE;
            aos_event_set(&event_tts_state, EVENT_TTS_FINISHED, AOS_EVENT_OR);
            break;
    }
}

static void media_state_cb(uint32_t event_id, const void *param, void *context)
{
    switch (event_id) {
        case EVENT_MEDIA_SYSTEM_ERROR:
        case EVENT_MEDIA_SYSTEM_FINISH:
                if (tts_running == TTS_STATE_PLAY) {
                    tts_running = TTS_STATE_IDLE;
                    aos_event_set(&event_tts_state, EVENT_TTS_PLAYER_FINISHED, AOS_EVENT_OR);
                }
            break;
        default:;
    }
}

void aui_wwv_cb(aui_wwv_resut_e wwv_result)
{
    switch (wwv_result) {
        case AUI_WWV_REJECT:
            LOGD(TAG, "wwv rejected");
            aos_event_set(&event_wwv_result, EVENT_WWV_REJECTED, AOS_EVENT_OR);
            break;

        case AUI_WWV_CONFIRM:
            LOGD(TAG, "wwv confirmed");
            local_audio_play(LOCAL_AUDIO_HELLO);
            aos_event_set(&event_wwv_result, EVENT_WWV_CONFIRMED, AOS_EVENT_OR);
            break;

        default: break;
    }
}

static void get_hex_mac(char *hex_mac)
{
    static uint8_t s_mac[6] = {0};
    int ret, try = 0;

    if (s_mac[0] == 0 && s_mac[1] == 0 && s_mac[2] == 0 && 
        s_mac[3] == 0 && s_mac[4] == 0 && s_mac[5] == 0) {
        aos_dev_t *wifi_dev = device_open_id("wifi", 0);

        do {
            ret = hal_wifi_get_mac_addr(wifi_dev, s_mac);
            if (ret == 0) {
                break;
            }

            aos_msleep(100);
        } while (++try < 5);
    }

    aos_check(try != 5, EIO);

    for (int i = 0; i < 6; i++) {
        sprintf(hex_mac + i * 2, "%02x", s_mac[i]);
    }
}

static void get_uuid(char *uuid)
{
    char product_key[32 + 1] = {0};
    char device_name[32 + 1] = {0};
    int pk_len = sizeof(product_key), dn_len = sizeof(device_name);

    int ret1 = aos_kv_get("hal_devinfo_pk", product_key, &pk_len);
    int ret2 = aos_kv_get("hal_devinfo_dn", device_name, &dn_len);
    if (ret1 == 0 && ret2 == 0) {
        sprintf(uuid, "%s&%s", product_key, device_name);
    } else {
        get_hex_mac(uuid);
    }

    LOGD(TAG, "device uuid %s", uuid);
}

static void get_session_id(char *buff, const int number)
{

    char rand_string[] = "0123456789abcdeffdecba9876543210";
    char ss[3]         = {0};

    /* use mac addr as first 12 bytes */
    get_hex_mac(buff);

    /* use random number as last 12 bytes */
    struct timeval time_now; //= {0};
    gettimeofday(&time_now, NULL);
    long time_mil = 0;
    time_mil      = time_now.tv_sec * 1000 + time_now.tv_usec / 1000;
    srand(((unsigned int)time_mil));

    for (int i = 1; i <= (number - 12); i++) {
        memset(ss, 0x0, 3);
        sprintf(ss, "%c", rand_string[(int)(32.0 * rand() / (RAND_MAX + 1.0))]);
        strcat(buff, ss);
    }
    
    //LOGD(TAG, "get_uuid_static_app:%s", buff);
}


int json_string_eq(cJSON *js, const char *str)
{
    if (cJSON_IsString(js)) {
        if (strcmp(js->valuestring, str) == 0) {
            return 1;
        }
    }
    return 0;
}

/* ai engine init */
int app_aui_nlp_init(void)
{
    int ret = -1;
    cJSON *js_account_info = NULL;
    char *s_account_info = NULL;
    char device_uuid[100] = {0};

    get_uuid(device_uuid);

    aos_event_new(&event_tts_state, 0);
    event_subscribe(EVENT_MEDIA_SYSTEM_ERROR, media_state_cb, NULL);
    event_subscribe(EVENT_MEDIA_SYSTEM_FINISH, media_state_cb, NULL);

    js_account_info = cJSON_CreateObject();
    CHECK_RET_WITH_GOTO(js_account_info, END);

    cJSON_AddStringToObject(js_account_info, "device_uuid", device_uuid);
    cJSON_AddStringToObject(js_account_info, "asr_app_key", "y5QsLk2A3acWEhCs");
    cJSON_AddStringToObject(js_account_info, "asr_token", "4a37aa0c0373498ea04f732054841b62");
    cJSON_AddStringToObject(js_account_info, "asr_url", "wss://nls-gateway-inner.aliyuncs.com/ws/v1");
    cJSON_AddStringToObject(js_account_info, "tts_app_key", "9a7f47f2");
    cJSON_AddStringToObject(js_account_info, "tts_token", "a2f8b80e04f14fdb9b7c36024fb03f78");
    cJSON_AddStringToObject(js_account_info, "tts_url", "wss://nls-gateway-inner.aliyuncs.com/ws/v1");

    s_account_info = cJSON_PrintUnformatted(js_account_info);
    CHECK_RET_TAG_WITH_GOTO(s_account_info, END);

    aui_config_t cfg;
    cfg.per             = "aixia";
    cfg.vol             = 100;      /* 音量 0~100 */
    cfg.spd             = 0;        /* -500 ~ 500*/
    cfg.pit             = 0;        /* 音调*/
    cfg.fmt             = 2;        /* 编码格式，1：PCM 2：MP3 */
    cfg.srate           = 16000;    /* 采样率，16000 */
    cfg.tts_cache_path  = NULL;     /* TTS内部缓存路径，NULL：关闭缓存功能 */
    cfg.cloud_vad       = 1;        /* 云端VAD功能使能， 0：关闭；1：打开 */
    cfg.js_account      = s_account_info;
    cfg.nlp_cb          = aui_nlp_cb;
    g_aui_handler.config  = cfg;
    g_aui_handler.context = NULL;

    aui_asr_register_mit(&g_aui_handler);
    aui_tts_register_mit(&g_aui_handler);

    ret = aui_cloud_init(&g_aui_handler);

    if (ret != 0) {
        LOGE(TAG, "ai engine error");
        goto END;
    }

    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_proc_mit);

    if (wwv_enabled) {
        aos_event_new(&event_wwv_result, 0);
        aui_cloud_init_wwv(&g_aui_handler, aui_wwv_cb);
    }

END:
    cJSON_Delete(js_account_info);
    free(s_account_info);
    
    return ret;
}

int app_aui_wwv_init(void)
{
    wwv_enabled = 1;

    return 0;
}

int app_aui_push_wwv_data(void *data, size_t len)
{
    aui_cloud_push_wwv_data(&g_aui_handler, data, len);
    return 0;
}

int app_aui_get_wwv_result(unsigned int timeout)
{
    unsigned int flags = 0;

    aos_event_get(&event_wwv_result, EVENT_WWV_CONFIRMED | EVENT_WWV_REJECTED, AOS_EVENT_OR_CLEAR, &flags, timeout);
    if (flags & EVENT_WWV_CONFIRMED) {
        return 0;
    }

    return -1;
}

int app_aui_cloud_push_audio(void *data, size_t size)
{
    return aui_cloud_push_pcm(&g_aui_handler, data, size);
}

int app_aui_cloud_stop(int force_stop)
{
    if (force_stop) {
        return aui_cloud_stop(&g_aui_handler);

    }

    return aui_cloud_stop_pcm(&g_aui_handler);
}

int app_aui_cloud_start(int do_wwv)
{
    char session_id[32 + 1] = {0};

    get_session_id(session_id, 32);
    aui_cloud_set_asr_session_id(&g_aui_handler, session_id);

    if (do_wwv) {
        aos_event_set(&event_wwv_result, 0, AOS_EVENT_AND);
    }
    aui_cloud_enable_wwv(&g_aui_handler, do_wwv);
    
    return aui_cloud_start_pcm(&g_aui_handler);
}

int app_aui_cloud_start_tts()
{
    return aui_cloud_start_tts(&g_aui_handler);
}

int app_aui_cloud_stop_tts()
{
    return aui_cloud_stop_tts(&g_aui_handler);
}

int app_aui_cloud_tts_wait_finish()
{
    unsigned int flags;

    if (tts_running != TTS_STATE_IDLE) {
        while (tts_running == TTS_STATE_RUN) {
            aos_msleep(100);
        }

        if (tts_running == TTS_STATE_PLAY) {
            aos_event_get(&event_tts_state, EVENT_TTS_PLAYER_FINISHED, AOS_EVENT_OR_CLEAR, &flags, 10000);
        }
    }
    return 0;
}

int app_aui_cloud_tts_run(const char *text, int wait_last)
{
    if (wait_last) {
        app_aui_cloud_tts_wait_finish();
    }
    aui_cloud_set_tts_status_listener(&g_aui_handler, aui_tts_stat_cb);

	aos_event_set(&event_tts_state, 0, AOS_EVENT_AND);
    return aui_cloud_req_tts(&g_aui_handler, text, NULL);
}

int app_aui_cloud_push_text(char *text)
{
    return aui_cloud_push_text(&g_aui_handler, text);
}
