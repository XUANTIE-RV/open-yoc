/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <aos/kv.h>
#include "app_main.h"
#include <yoc/atserver.h>
#include <at_cmd.h>
#include <yoc/mic.h>
#include <ulog/ulog.h>
#include <uservice/uservice.h>
#include <yoc/atserver.h>
#include <drv/uart.h>
#include "ota_client.h"
#include "ais_ota/ais_ota_client.h"
#include "ota_trans/ota_client_trans.h"
#include "ota_trans/ota_client_trans_uart.h"
#include "yoc/pcm_input.h"

#include "board.h"

#define SESSION_STATE_IDLE  0
#define SESSION_STATE_START 1
#define SESSION_STATE_WWV   2

#define TAG "AT"

#define AUDIO_TYPE_PCM  1
#define AUDIO_TYPE_OGG  2
#define AUDIO_TYPE_FLAC 3

#define AUDIO_CHANNEL_MIC1 0x1
#define AUDIO_CHANNEL_MIC2 0x2
#define AUDIO_CHANNEL_REF  0x4
#define AUDIO_CHANNEL_AEC  0x8

static int g_audio_type = 0;
static int g_channel    = 0;
static int g_mic_status = 0;
static int g_pcm_idx;
static int session_state;

static void at_cmd_micopen(char *cmd, int type, char *data);
static void at_cmd_micclose(char *cmd, int type, char *data);
static void at_cmd_ais_ota(char *cmd, int type, char *data);

const atserver_cmd_t at_cmd[] = {
    AT,
    AT_HELP,
    AT_FWVER,
    AT_SYSTIME,
    AT_REBOOT,
    AT_KVGET,
    AT_KVSET,
    AT_KVDEL,
    AT_KVGETINT,
    AT_KVSETINT,
    AT_KVDELINT,
    { "AT+MICOPEN", at_cmd_micopen },
    { "AT+MICCLOSE", at_cmd_micclose },
    { "AT+AISOTA", at_cmd_ais_ota },
    AT_NULL,
};

static void pcm_send(void *data, int len)
{
    if (g_mic_status) {
        atserver_lock();
        atserver_send("+MICDATA=%d,%d,", g_pcm_idx++, len);
        atserver_write(data, len);
        atserver_unlock();
    }
}

/* mic 数据回调(20 ms)，格式为mic1/mic2/ref/null/null从回调中取出第三路reference数据，进行FFT转换 */
static void ref_get_cb(void *data, unsigned int len, void* arg)
{
    short *mic1;
    short *mic2;
    short *ref;
    int    conv_size = 0;

    short *output = malloc(len);

    if (output == NULL) {
        return;
    }

    mic1 = ((short *)data);
    mic2 = ((short *)data) + len / 5 / 2;
    ref  = ((short *)data) + len / 5 / 2 * 2;

    LOGD(TAG, "ref_get_cb %d", len);

    if (g_channel & AUDIO_CHANNEL_MIC1 || g_channel & AUDIO_CHANNEL_MIC2 || g_channel & AUDIO_CHANNEL_REF) {
        for (int i = 0; i < len / 5 / 2; i++) {
            if (g_channel & AUDIO_CHANNEL_MIC1) {
                output[conv_size++] = mic1[i];
            }

            if (g_channel & AUDIO_CHANNEL_MIC2) {
                output[conv_size++] = mic2[i];
            }

            if (g_channel & AUDIO_CHANNEL_REF) {
                output[conv_size++] = ref[i];
            }
        }
        pcm_send(output, conv_size * 2);
    }

    free(output);

    return;
}

static void pcm_record_register(int enable)
{
    /* 注册mic回调，获取reference音频数据 */
    if (enable) {
        pcm_input_cb_register(ref_get_cb, NULL);
    } else {
        pcm_input_cb_register(NULL, NULL);
    }
}

static void _push2talk()
{
    aui_mic_control(MIC_CTRL_START_SESSION, 1);
}

// AT+MICOPEN=<g_audio_type>,<channels>
static void at_cmd_micopen(char *cmd, int type, char *data)
{
    char audio_type_str[10];
    char channel_str[10];
    int  ret;

    if (type == WRITE_CMD) {
        ret = atserver_scanf("%[^,],%s", audio_type_str, channel_str);

        if (ret == 2) {
            LOGD(TAG, "%s audio_type_str %s, chanel %s", __func__, audio_type_str, channel_str);

            if (g_mic_status == 1) {
                LOGE(TAG, "%s already open\n", __func__);
                goto err;
            }

            if (strstr(audio_type_str, "pcm")) {
                g_audio_type = AUDIO_TYPE_PCM;
            } else if (strstr(audio_type_str, "ogg")) {
                g_audio_type = AUDIO_TYPE_OGG;
            } else if (strstr(audio_type_str, "flac")) {
                g_audio_type = AUDIO_TYPE_FLAC;
            } else {
                goto err;
            }

            if (strstr(channel_str, "mic1")) {
                g_channel |= AUDIO_CHANNEL_MIC1;
            }

            if (strstr(channel_str, "mic2")) {
                g_channel |= AUDIO_CHANNEL_MIC2;
            }

            if (strstr(channel_str, "ref")) {
                g_channel |= AUDIO_CHANNEL_REF;
            }

            if (strstr(channel_str, "aec")) {
                g_channel |= AUDIO_CHANNEL_AEC;

                if (session_state == SESSION_STATE_IDLE) {
                    LOGD(TAG, "push2talk start session_state %d", session_state);
                    session_state = SESSION_STATE_START;
                    _push2talk();
                }
            }

            if (g_channel == 0) {
                goto err;
            }

            AT_BACK_OK();
            if (g_channel & AUDIO_CHANNEL_MIC1 || g_channel & AUDIO_CHANNEL_MIC2 || g_channel & AUDIO_CHANNEL_REF) {
                pcm_record_register(1);
            }

            g_mic_status = 1;
            return;
        }
    }

err:
    AT_BACK_ERR();
    return;
}

// AT+MICCLOSE
static void at_cmd_micclose(char *cmd, int type, char *data)
{
    if (type == EXECUTE_CMD) {
        LOGD(TAG, "%s", __func__);

        if (g_mic_status == 0) {
            LOGE(TAG, "%s already close\n", __func__);
            goto err;
        }

        g_mic_status = 0;
        g_pcm_idx    = 0;
        g_channel    = 0;
        g_audio_type = 0;
        pcm_record_register(0);

        AT_BACK_OK();
        return;
    }

err:
    AT_BACK_ERR();
    return;
}

#if defined(CONFIG_OTA_CLIENT)
static ota_event_cb ota_event_cb_t = NULL;
;
void ota_event_register_cb(ota_event_cb cb)
{
    ota_event_cb_t = cb;
}

static void pass_th_cb(void *data, int len)
{
    uint8_t *ais_ota_msg_buff = (uint8_t *)malloc(len);
    if (!ais_ota_msg_buff) {
        printf("ota malloc error\n");
        return;
    }
    memcpy(ais_ota_msg_buff, (uint8_t *)data, len);
    if (ota_event_cb_t) {
        ota_event_cb_t(OTA_CLIENT_EVENT_WRITE, ais_ota_msg_buff, len);
    }
    free(ais_ota_msg_buff);
}
#endif

static void at_cmd_ais_ota(char *cmd, int type, char *data)
{
#if defined(CONFIG_OTA_CLIENT)
    int        len;
    static int ota_ais_init_flag = 0;

    if (ota_ais_init_flag == 0) {
        ota_ais_init_flag = 1;
        ota_client_init();
        ais_ota_crc_check_ctl(0);   //关闭crc校验
        ais_ota_auto_reboot_ctl(0); //关闭自动reboot
        if (ota_event_cb_t)
            ota_event_cb_t(OTA_CLIENT_EVENT_CONN, NULL, 0);
    }

    atserver_scanf("%d", &len);
    atserver_pass_through(len, pass_th_cb);
    LOGD(TAG, "ota rcv data");
    atserver_send("OK\r\n");
#endif
}

void app_at_server_init(utask_t *task)
{
    rvm_hal_uart_config_t config;
    rvm_hal_uart_config_default(&config);
    config.baud_rate = CONFIG_AT_UART_BAUD;

    if (task == NULL) {
        task = utask_new("at_svr", 4 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);
    }

    if (task) {
        if (atserver_init(task, CONFIG_AT_UART_NAME, &config) == 0) {
            atserver_set_output_terminator("");
        } else {
            LOGE(TAG, "atserver_init fail");
        }
    }
}

int app_at_cmd_init()
{
    app_at_server_init(NULL);
    return atserver_add_command(at_cmd);
}

void app_at_pcm_data_out(void *data, int len)
{
    if (session_state == SESSION_STATE_IDLE) {
        return;
    }

    if (g_channel == AUDIO_CHANNEL_AEC) {
        pcm_send(data, len);
    }
}

void app_at_session_start(char *wakeup_word)
{
    session_state = SESSION_STATE_START;
    atserver_lock();
    atserver_send("+KWSWAKEUP=\"%s\",1\r\n", wakeup_word);
    atserver_unlock();
}

void app_at_session_stop()
{
    session_state = SESSION_STATE_IDLE;
    atserver_lock();
    atserver_send("+VADEND\r\n");
    atserver_unlock();
}