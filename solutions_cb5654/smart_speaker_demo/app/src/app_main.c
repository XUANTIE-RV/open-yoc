/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <devices/devicelist.h>
#include <devices/iic.h>
#include <devices/drv_snd_sc5654.h>
#include <devices/drv_snd_add2010.h>
#include <yoc/mic.h>
#include <pin_name.h>
#include <media.h>
#include "app_main.h"
#include "app_lpm.h"
#include "app_sys.h"

#define TAG "app"

#define SESSION_STATE_IDLE 0
#define SESSION_STATE_START 1
#define SESSION_STATE_WWV 2

//i2c_dev_t g_i2c_dev;

static void app_clock_alarm_cb(uint8_t clock_id)
{
    LOGI(TAG, "clock_id %d alarm cb handle", clock_id);

    aui_player_play(MEDIA_MUSIC,
        "https://cop-image-prod.oss-cn-hangzhou.aliyuncs.com/resource/undefined/1577166283488/AudioTest1.mp3", 0);
}

/* 接收到 MIC 事件 */
static void mic_evt_cb(int source, mic_event_id_t evt_id, void *data, int size)
{
    int        ret;
    static int session_state;

    if (g_fct_mode) {
        return;
    }

    switch (evt_id) {
        case MIC_EVENT_PCM_DATA:
            if (session_state == SESSION_STATE_IDLE)
                break;
            /* 麦克风数据，推到云端 */
            ret = app_aui_cloud_push_audio(data, size);
            if (ret < 0) {
                /* 数据推送错误 */
                session_state = SESSION_STATE_IDLE;
                LOGE(TAG, "cloud push pcm finish");
                aui_mic_control(MIC_CTRL_STOP_PCM);
                ret = app_aui_cloud_stop(1);
                if (wifi_internet_is_connected() == 0) {
                    LOGE(TAG, "mic evt ntp not synced");
                    local_audio_play(LOCAL_AUDIO_NET_FAIL);
                } else {
                    if (ret < 0) {
                        local_audio_play(LOCAL_AUDIO_SORRY2);
                    }
                }
                app_status_update();
            }
            break;
        case MIC_EVENT_VAD:
            //LOGD(TAG, "MIC_EVENT_VAD = %d", *((int*)data));
            app_lpm_vad_check();
            break;
        case MIC_EVENT_SESSION_START:
            if (app_player_get_mute_state()) {
                printf("Device is mute\n");
                return;
            }
            LOGD(TAG, "MIC_EVENT_SESSION_START,type= %d", *((int *)data));
            int type   = *((int *)data);
            int do_wwv = 0;

            /* 网络检测 */
            if (wifi_internet_is_connected() == 0) {
                LOGE(TAG, "mic_evt net connect failed");
                aui_mic_control(MIC_CTRL_STOP_PCM);
                app_aui_cloud_stop(1);
                if (wifi_is_pairing())
                    local_audio_play(LOCAL_AUDIO_NET_CFG_CONFIG);
                else
                    local_audio_play(LOCAL_AUDIO_NET_FAIL);
                return;
            }

            /* 开始交互 */
            session_state = SESSION_STATE_START;

            if (type & KWS_ID_WWV_MASK) {
                do_wwv        = 1;
                session_state = SESSION_STATE_WWV;
            } else {
                /* play wakeup voice only when wwv is not needed,
                   otherwise do it in the wwv result callback */
                local_audio_play(LOCAL_AUDIO_HELLO);
            }

            app_status_update();
#ifndef CONFIG_MIT_DSP
            aos_msleep(250); //等待回答结束后开始录音
#endif

            ret = app_aui_cloud_start(do_wwv);
            if (ret != 0) {
                session_state = SESSION_STATE_IDLE;
                return;
            }

            aui_mic_control(MIC_CTRL_START_PCM);

            break;
        case MIC_EVENT_SESSION_STOP:
            LOGD(TAG, "MIC_EVENT_SESSION_STOP");
            /* 交互结束 */
            if (session_state != SESSION_STATE_IDLE) {
                ret = 0;
                if (session_state == SESSION_STATE_WWV) {
                    ret = app_aui_get_wwv_result(2000);
                    LOGD(TAG, "wwv result %s", ret == 0 ? "success" : "failed");
                }

                if (ret == 0) {
                    app_aui_cloud_stop(0);
                } else {
                    /* wwv rejected or timeout */
                    app_aui_cloud_stop(1);
                }

                aui_mic_control(MIC_CTRL_STOP_PCM);
                app_status_update();
                session_state = SESSION_STATE_IDLE;
            }
            break;
        case MIC_EVENT_KWS_DATA:
            if (session_state == SESSION_STATE_WWV) {
                size_t len;
                void * data;

                data = aui_mic_get_wwv_data(&len);
                if (data) {
                    LOGD(TAG, "push wwv data %p %d", data, len);
                    app_aui_push_wwv_data(data, len);
                } else {
                    LOGW(TAG, "get wwv data failed");
                }
            } else {
                LOGW(TAG, "wwv event in wrong state");
            }

            break;
        default:;
    }
}

/*************************************************
 * 音频相关初始化
 *************************************************/
static int app_mic_init(int wwwv_enable)
{
    int ret;

#ifdef CONFIG_HUWEN_DSP
    extern void mic_huwen_register(void);
    mic_huwen_register();
#else
    mic_thead_v1_register();
#endif

    utask_t *task_mic = utask_new("task_mic", 3 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
    ret               = aui_mic_start(task_mic, mic_evt_cb);

    mic_param_t param;
    memset(&param, 0, sizeof(param));

    if (CONFIG_LEFT_GAIN > 0 && CONFIG_RIGHT_GAIN < 0) {
        /* 左声道为参考音 */
        param.channels = 0x0100;
    } else if (CONFIG_LEFT_GAIN < 0 && CONFIG_RIGHT_GAIN > 0) {
        /* 右声道为参考音 */
        param.channels = 0x0200;
    } else if (CONFIG_LEFT_GAIN < 0 && CONFIG_RIGHT_GAIN < 0) {
        /* PA后 参考音 */
        param.channels = 0x0300;
    } else {
        /* 异常配置，DSP进行处理 */
        param.channels = 0x0400;
        LOGE(TAG, "unknown ref config");
    }

#if defined(CONFIG_MIT_DSP) && CONFIG_MIT_DSP
    param.sentence_time_ms = 0;
    param.noack_time_ms    = 0;
    param.max_time_ms      = 0;
    param.nsmode           = 0; /* 无非线性处理 */
    param.aecmode          = 0; /* 无非线性处理 */
    param.vadmode          = 0; /* 使能VAD */
#else
    param.sentence_time_ms = 1000;
    param.noack_time_ms    = 8000;
    param.max_time_ms      = 20000;
    param.nsmode           = 4; /* 无非线性处理 */
    param.aecmode          = 4; /* 无非线性处理 */
    param.vadmode          = 3;
#endif
    aui_mic_set_param(&param);

    if (wwwv_enable) {
        aui_mic_control(MIC_CTRL_ENABLE_WWV);
        app_aui_wwv_init();
    } else {
        aui_mic_control(MIC_CTRL_DISABLE_WWV);
    }

    return ret;
}

int app_mic_is_busy()
{
    mic_state_t mic_st;
    if (aui_mic_get_state(&mic_st) == 0) {
        if (mic_st == MIC_STATE_SESSION) {
            return 1;
        }
    }

    return 0;
}
/*************************************************
 * APP的各种测试命令
 *************************************************/

static void cli_reg_cmds(void)
{
    cli_reg_cmd_aui();
    cli_reg_cmd_yvdbg();
    cli_reg_cmd_app();
    cli_reg_cmd_gpio();

    cli_reg_cmd_iperf();
}

/*************************************************
 * YoC入口函数
 *************************************************/
void main()
{
    extern int  posix_init(void);
    extern void cxx_system_init(void);

    // must call this func when use c++
    posix_init();
    // must call this func when use c++
    cxx_system_init();

    board_base_init();

#ifdef BOARD_MIT_V3
    app_subboard_ldo_enable(1);
#endif

    yoc_base_init();

    LOGD(TAG, "enter app");

    // g_i2c_dev.port = 1;
    // g_i2c_dev.config.mode = I2C_MODE_MASTER;
    // g_i2c_dev.config.freq = I2C_BUS_BIT_RATES_400K;
    // g_i2c_dev.config.dev_addr = 0x34;
    // g_i2c_dev.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    // hal_i2c_init(&g_i2c_dev);

    // iic register at `board_base_init`
    aos_dev_t *i2c_dev = iic_open_id("iic", 0);
    iic_config_t config;
    config.mode = MODE_MASTER;
    config.speed = BUS_SPEED_FAST;
    config.addr_mode = ADDR_7BIT;
    config.slave_addr = 0x34;
    iic_config(i2c_dev, &config);

#if (defined(BOARD_MIT_V2) || defined(BOARD_MIT_V3))
    add2010_init(i2c_dev);
    aw9523_init(i2c_dev);
    aw2013_init(i2c_dev);
#endif

    /* 关闭功放 */
    app_audio_pa_ctrl(0);

    /* 系统事件处理 */
    sys_event_init();

#ifndef CONFIG_HUWEN_DSP
    app_low_power_init();
#endif

    app_sys_init();

#ifdef CONFIG_BATTERY_ADC
    /*电源初始化检测*/
    int volt = app_battery_init(PIN_BATTERY_ADC, PIN_BATTERY_CHARGE_PIN, 60);
    if (volt > 0 && volt < BATTERY_VOLT_MIN) {
        app_lpm_sleep(LPM_POLICY_DEEP_SLEEP, 0);
        aos_msleep(10000);
    }
#endif

    app_status_init();
    app_set_led_state(LED_LOADING);

#if defined(CONFIG_RTC_EN) && CONFIG_RTC_EN
    /* set rtc time to systime */
    // app_rtc_to_system(1);
    clock_alarm_init(app_clock_alarm_cb);
#endif
    /* 音频初始化 */
#if (defined(BOARD_MIT_V2) || defined(BOARD_MIT_V3))
    sound_drv_add2010_register(CONFIG_VOL_MAX, CONFIG_LEFT_GAIN, CONFIG_RIGHT_GAIN);
#else
    sound_drv_sc5654_register(CONFIG_VOL_MAX, CONFIG_LEFT_GAIN, CONFIG_RIGHT_GAIN);
#endif

    /* soft vad 初始化 */
    app_softvad_init();

    /* 音频数据采集 */
    board_audio_init();

    wifi_mode_e mode = app_network_init();

    if (mode != MODE_WIFI_TEST) {
        /* 启动播放器 */
        app_player_init();

        /* 启动麦克风服务 */
        app_mic_init(0);

        /* 开启功放 */
        app_audio_pa_ctrl(1);

        if (mode != MODE_WIFI_PAIRING)
            local_audio_play(LOCAL_AUDIO_STARTING);
#if defined(APP_FOTA_EN) && APP_FOTA_EN
        app_fota_init();
#endif

        if (g_fct_mode) {
            fct_case_init();
        }

        /* 交互系统初始化 */
        app_aui_nlp_init();
        app_text_cmd_init();
    }

    /* 按键初始化 */
    app_button_init();

    app_set_led_state(LED_NET_DOWN);

    /* 命令行测试命令 */
    cli_reg_cmds();
}
