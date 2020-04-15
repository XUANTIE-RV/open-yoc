/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <app_config.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <aos/aos.h>
#include "aos/cli.h"
#include <yoc/uservice.h>
#include <yoc/eventid.h>
#include <media.h>
#include <yoc/mic.h>
#include <player.h>

#include "app_init.h"
#include "app_main.h"
#include "audio/audio_res.h"

#define TAG "app"

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
    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_process_aliyun_rasr);
    aui_nlp_process_add(&g_aui_nlp_process, aui_nlp_proc_cb_textcmd);

    return 0;
}

/* 处理云端反馈的 NLP 文件，进行解析处理 */
static void aui_nlp_cb(const char *json_text)
{
    int ret;

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

/* ai engine init */
static int app_aui_nlp_init(void)
{
    int ret;

    aui_config_t _aui_cfg;
    _aui_cfg.per         = "xiaoyan";
    _aui_cfg.vol         = 50;
    _aui_cfg.pit         = 50;
    _aui_cfg.spd         = 50;
    _aui_cfg.nlp_cb      = aui_nlp_cb;
    g_aui_handler.config  = _aui_cfg;
    g_aui_handler.context = NULL;
    g_aui_handler.asr_type = CLOUD_RASR_ALIYUN;

    ret = aui_cloud_init(&g_aui_handler);

    if (ret != 0) {
        LOGE(TAG, "ai engine error");
    }

    return ret;
}

static void media_evt(int type, aui_player_evtid_t evt_id)
{
    //LOGD(TAG, "media_evt type %d,evt_id %d", type, evt_id);

    if (type == MEDIA_MUSIC) {
        switch (evt_id) {
            case AUI_PLAYER_EVENT_START:
                LOGD(TAG, "audio player start %d", AUI_PLAYER_EVENT_START);
                break;
            case AUI_PLAYER_EVENT_ERROR:
                local_audio_play(LOCAL_AUDIO_PLAY_ERR);
                LOGD(TAG, "audio player error %d", AUI_PLAYER_EVENT_ERROR);
                break;
            case AUI_PLAYER_EVENT_FINISH:
                LOGD(TAG, "audio player finish %d", AUI_PLAYER_EVENT_FINISH);
                break;
            default:
                break;
        }
        return;
    }
}

static int app_media_init(utask_t *task)
{
    int ret = -1;

    ret = aui_player_init(task, media_evt);

    return ret;
}

void mic_evt_cb(int source, mic_event_id_t evt_id, void *data, int size)
{
    int        ret;
    static int pcm_started;

    switch (evt_id) {
        case MIC_EVENT_PCM_DATA:
            if (pcm_started == 0)
                break;
            /* 麦克风数据，推到云端 */
            ret = aui_cloud_push_pcm(&g_aui_handler, data, size);
            if (ret < 0) {
                /* 数据推送错误 */
                pcm_started = 0;
                LOGE(TAG, "cloud push pcm finish");
                aui_mic_control(MIC_CTRL_STOP_PCM);
                ret = aui_cloud_stop_pcm(&g_aui_handler);
                if (wifi_internet_is_connected() == 0) {
                    LOGE(TAG, "00mic evt ntp not synced");
                    local_audio_play(LOCAL_AUDIO_NET_FAIL);
                } else {
                    if (ret < 0) {
                        local_audio_play(LOCAL_AUDIO_SORRY2);
                    }                    
                }
            }
            break;
        case MIC_EVENT_SESSION_START:
            LOGE(TAG, "asr ok");
            if (app_player_get_mute_state()) {
                return;
            }
            /* 网络检测 */
            if (wifi_internet_is_connected() == 0) {
                LOGE(TAG, "mic_evt net connect failed");
                aui_mic_control(MIC_CTRL_STOP_PCM);
                aui_cloud_stop_pcm(&g_aui_handler);
                if (wifi_is_pairing())
                    local_audio_play(LOCAL_AUDIO_NET_CFG_CONFIG);
                else
                    local_audio_play(LOCAL_AUDIO_NET_FAIL);
                return;
            }

            /* 开始交互 */
            if (pcm_started == 0) {
                pcm_started = 1;
                local_audio_play(LOCAL_AUDIO_HELLO);
                aos_msleep(500);
                // app_status_update();
                aui_mic_control(MIC_CTRL_START_PCM);
            }
            break;
        case MIC_EVENT_SESSION_STOP:
            /* 交互结束 */
            if (pcm_started == 1) {
                pcm_started = 0;
                LOGI(TAG, "asr session finish");
                // app_set_led_state(LED_NET_READY);
                aui_mic_control(MIC_CTRL_STOP_PCM);
                aui_cloud_stop_pcm(&g_aui_handler);
                // aui_player_resume_music();
            }

            break;
        default:;
    }
}

/* mic init */
static int app_mic_init(utask_t *task)
{
    int ret;

    voice_mic_register();

    ret = aui_mic_start(task, mic_evt_cb);

    return ret;
}

/*************************************************
 * APP的各种测试命令
 *************************************************/
static void cli_reg_cmd_app(void)
{
    extern void cli_reg_cmd_aui(void);
    cli_reg_cmd_aui();

    extern void cli_reg_cmd_apps(void);
    cli_reg_cmd_apps();

    //extern void test_gpio_register_cmd(void);
    //test_gpio_register_cmd();

    // extern void cli_yv_test_init(void);
    // cli_yv_test_init();

    //extern void test_register_system_cmd(void);
    //test_register_system_cmd();

    extern void test_iperf_register_cmd(void);
    test_iperf_register_cmd();
}

void main()
{
extern int posix_init(void);
extern void cxx_system_init(void);

    // must call this func when use c++
    posix_init();
    // must call this func when use c++
    cxx_system_init();
    
    board_yoc_init();

    wifi_mode_e mode = app_network_init();

    if (mode != MODE_WIFI_TEST) {
        /* 启动播放器 */
        utask_t *task_player = utask_new("app_player", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
        app_media_init(task_player);

        /* 启动麦克风服务 */
        utask_t *task_mic = utask_new("task_mic", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
        app_mic_init(task_mic);

        app_player_init();

        if (mode != MODE_WIFI_PAIRING)
            local_audio_play(LOCAL_AUDIO_STARTING);

        app_fota_init();

        app_button_init();
    }

    /* 交互系统初始化 */
    app_aui_nlp_init();
    app_aui_cmd_init();
    app_text_cmd_init();

    /* 命令行测试命令 */
    cli_reg_cmd_app();

#ifdef CONFIG_LED_TASK_ON
    /* LED task */
    led_test_task();
#endif
}

