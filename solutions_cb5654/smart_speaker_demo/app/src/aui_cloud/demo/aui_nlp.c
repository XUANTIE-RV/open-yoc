/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include "aui_nlp.h"
#include "app_main.h"

#define TAG "auinlp"

/* ai cmd */
aui_cmd_set_t g_aui_nlp_process;

/* ai engine */
aui_t        g_aui_handler;


/* ai cmd init */
static int app_aui_cmd_init(void)
{
    aui_nlp_process_set_pre_check(&g_aui_nlp_process, aui_nlp_process_pre_check);

    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_proc_cb_story_nlp);
    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_proc_cb_tts_nlp);
    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_process_music_nlp);
    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_process_music_url);
    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_process_cmd);
    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_process_baidu_asr);
    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_process_xf_rtasr);
    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_process_proxy_ws_asr);
    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_proc_cb_textcmd);

    return 0;
}

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
    int ret;

    aui_config_t cfg;
    cfg.per         = "xiaoyan";
    cfg.vol         = 50;
    cfg.pit         = 50;
    cfg.spd         = 50;
    cfg.nlp_cb      = aui_nlp_cb;
    g_aui_handler.config  = cfg;
    g_aui_handler.context = NULL;
    g_aui_handler.asr_type = CLOUD_RASR_XFYUN;

    ret = aui_cloud_init(&g_aui_handler);

    if (ret != 0) {
        LOGE(TAG, "ai engine error");
    }

    return ret;
}

int app_aui_wwv_init(void)
{
    return 0;
}

int app_aui_push_wwv_data(void *data, size_t len)
{
    return 0;
}

int app_aui_get_wwv_result(unsigned int timeout)
{
    return 0;
}

int app_aui_cloud_push_audio(void *data, size_t size)
{
    return aui_cloud_push_pcm(&g_aui_handler, data, size);
}

int app_aui_cloud_stop(int force_stop)
{
    return aui_cloud_stop_pcm(&g_aui_handler);
}

int app_aui_cloud_start(int do_wwv)
{
    aui_cloud_enable_wwv(&g_aui_handler, do_wwv);

    return aui_cloud_start_pcm(&g_aui_handler);
}

int app_aui_cloud_tts_wait_finish()
{
    return 0;
}

int app_aui_cloud_tts_run(const char *text, int wait_last)
{
    int ret;
    ret = aui_cloud_req_tts(&g_aui_handler, text, "fifo://ttsdemo");
    if (ret == 0) {
        aui_player_play(MEDIA_SYSTEM, "fifo://ttsdemo", 1);
    }
    return ret;
}

int app_aui_cloud_push_text(char *text)
{
    if (strncasecmp(text, MUSIC_PREFIX, strlen(MUSIC_PREFIX)) == 0) {
        LOGI(TAG, "get music url start");
        return baidu_music(&g_aui_handler, text);
    }
    return aui_cloud_push_text(&g_aui_handler, text); 
}
