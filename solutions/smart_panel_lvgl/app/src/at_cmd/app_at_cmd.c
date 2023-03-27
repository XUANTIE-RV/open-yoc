/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <board.h>
#include <yoc/atserver.h>
#include <at_cmd.h>
#include <at_mesh.h>
#include <at_ota.h>
#if defined (CONFIG_ERS_CONNECTIVITY) && (CONFIG_ERS_CONNECTIVITY > 0)
#include <at_smartliving.h>
#include <at_network.h>
#endif
#include "app_factory.h"
#include "app_main.h"
#include "app_sys.h"

#define TAG "app_at_cmd"

#define SESSION_STATE_IDLE  0
#define SESSION_STATE_START 1
#define SESSION_STATE_WWV   2

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

extern int smartliving_client_control(const int start);
#if defined (CONFIG_SMARTLIVING_AT_MODULE) && (CONFIG_SMARTLIVING_AT_MODULE > 0)
extern int user_at_post_property(int device_id, char *message, int len);
extern int user_at_post_event(int device_id, char *evtid, int evtid_len, char *evt_payload, int len);
extern int smartliving_client_is_connected(void);
#endif

#if defined(CONFIG_GW_NETWORK_SUPPORT) && CONFIG_GW_NETWORK_SUPPORT
void at_cmd_reinit_network(void *arg)
{
    app_network_reinit();

    aos_task_exit(0);
}
#endif

static void event_app_at_cmd_handler(uint32_t event_id, const void *param, void *context)
{
#if defined(CONFIG_SMARTLIVING_AT_MODULE) && CONFIG_SMARTLIVING_AT_MODULE
    if (event_id == EVENT_APP_AT_CMD) {
        APP_AT_CMD_TYPE type = (APP_AT_CMD_TYPE)param;
        switch (type) {
     
            case APP_AT_CMD_IWS_START:
                smartliving_client_control(0);
                app_wifi_pair_start();
                break;
                
            case APP_AT_CMD_WJAP:
                aos_task_new("WJAP", at_cmd_reinit_network, NULL, 4 * 1024);
                break;
            
            case APP_AT_CMD_CON:
                smartliving_client_control(1);
                break;
            
            case APP_AT_CMD_CLS:
                smartliving_client_control(0);
                break;


            default:
                break;
        }
    }
#endif 
}

void user_gateway_rst(char *cmd, int type, char *data)
{
    if (type == EXECUTE_CMD) {
        AT_BACK_OK();
        app_ftsetting_restore();
    }
}

const atserver_cmd_t at_cmd[] = {
    AT,
    AT_HELP,
    AT_CGMR,
    AT_FWVER,
    AT_SYSTIME,
    AT_SAVE,
    AT_FACTORYW,
    AT_FACTORYR,
    AT_REBOOT,
    AT_EVENT,
    AT_ECHO,
#ifdef CONFIG_YOC_LPM
    AT_SLEEP,
#endif
    AT_MODEL,
    AT_KVGET,
    AT_KVSET,
    AT_KVDEL,
    AT_KVGETINT,
    AT_KVSETINT,
    AT_KVDELINT,

    AT_CIPSTART,
    AT_CIPSTOP,
    AT_CIPRECVCFG,
    AT_CIPID,
    AT_CIPSTATUS,
    AT_CIPSEND,
#ifdef CONFIG_YOC_LPM
    AT_CIPSENDPSM,
#endif
    AT_CIPRECV,
    AT_BTMESH_LOG_LEVEL,
    AT_BTMESH_AT_EN,
    AT_BTMESH_RST_DEV,
    AT_BTMESH_QUERY_STA,
    AT_BTMESH_PROV_CONFIG,
    AT_BTMESH_PROV_AUTO_PROV,
    AT_BTMESH_PROV_EN,
    AT_BTMESH_APPKEY_SET,
    AT_BTMESH_CLEAR_RPL,
    AT_BTMESH_PROV_FILTER_DEV,
    AT_BTMESH_PROV_SHOW_DEV,
    AT_BTMESH_ADDDEV,
    AT_BTMESH_DELDEV,
    AT_BTMESH_OOB,
    AT_BTMESH_AUTOCONFIG,
    AT_BTMESH_GET_NODE_VERSION,
    AT_BTMESH_ADD_NODE,
    AT_BTMESH_GET_NODE_INFO,
    AT_BTMESH_CFG_COMP_GET,
    AT_BTMESH_NETKEY_GET,
    AT_BTMESH_NETKEY_SET,
    AT_BTMESH_APPKEY_GET,
    AT_BTMESH_APPKEY_ADD,
    AT_BTMESH_APPKEY_BIND,
    AT_BTMESH_APPKEY_UNBIND,
    AT_BTMESH_CFG_RELAY,
    AT_BTMESH_CFG_PROXY,
    AT_BTMESH_CFG_FRIEND,
    AT_BTMESH_SUB_GET,
    AT_BTMESH_SUB_SET,
    AT_BTMESH_SUB_DEL,
    AT_BTMESH_SUBLIST_OVERWRITE,
    AT_BTMESH_HB_PUB_SET,
    AT_BTMESH_HB_PUB_GEL,
    AT_BTMESH_HB_REPORT,
    AT_BTMESH_HB_SUB_SET,
    AT_BTMESH_HB_SUB_GET,
    AT_BTMESH_ACTIVE_GET,
    AT_BTMESH_PUB_GET,
    AT_BTMESH_PUB_SET,
    AT_BTMESH_RST,
    AT_BTMESH_ONOFF,
    AT_BTMESH_LEVEL,
    AT_BTMESH_LEVEL_MOVE,
    AT_BTMESH_LEVEL_DELTA,
    AT_BTMESH_LIGHTNESS_RANGE,
    AT_BTMESH_LIGHTNESS_DEF,
    AT_BTMESH_LIGHTNESS,
    AT_BTMESH_LIGHTNESS_LIN,
    AT_BTMESH_LIGHT_CTL_RANGE,
    AT_BTMESH_LIGHT_CTL_DEF,
    AT_BTMESH_LIGHT_CTL,
    AT_BTMESH_LIGHT_CTL_TEMP,
    AT_BTMESH_TRS,
    AT_BTMESH_PROV_FILTER_MAC,
    AT_BTMESH_PROV_FILTER_MAC_ADD,
    AT_BTMESH_PROV_FILTER_MAC_RM,
    AT_BTMESH_PROV_FILTER_MAC_CLEAR,
#ifdef CONFIG_BT_MESH_LPM
    AT_BTMESH_PROV_SET_NODE_LPM_FLAG,
#endif
    AT_ADDOTAFW,
    AT_GETOTAFWID,
    AT_ADDOTANODE,
    AT_RMOTANODE,
    AT_ADDOTANODEBYUNICAST,
    AT_RMOTANODEBYUNICAST,
    AT_STARTOTA,
    AT_RMOTAFW,
#if defined(CONFIG_SMARTLIVING_AT_MODULE) && CONFIG_SMARTLIVING_AT_MODULE
    { "AT+IDMAU", idm_au_handler },
#if defined(EN_COMBO_NET) && (EN_COMBO_NET == 1)
    { "AT+IDMPID", idm_pid_handler },
#endif
    { "AT+IWSSTART", iws_start_handler },
    { "AT+IWSSTOP", iws_stop_handler },
    { "AT+DEVINFO", at_cmd_dev_info },
    { "AT+AUICFG", at_cmd_aui_cfg },
    { "AT+AUIFMT", at_cmd_aui_fmt },
    { "AT+AUIMICEN", at_cmd_aui_micen },
    { "AT+AUIWWVEN", at_cmd_wwv_en },
    { "AT+AUIKWS", at_cmd_aui_kws },
    { "AT+AUICTRL", at_cmd_aui_ctrl },
    { "AT+WJAP", wjap_handler },
    { "AT+WJAPD", wjapd_handler },
    { "AT+WJAPQ", wjapq_handler },
    { "AT+IDMCON", idm_con_handler },
    { "AT+IDMCLS", idm_cls_handler },
    { "AT+IDMSTA", idm_sta_handler },
    { "AT+IDMPP", idm_pp_handler },
    { "AT+IDMPS", idm_ps_handler },
    { "AT+IDMEP", idm_ep_handler },
#endif
    { "AT+WSCAN", user_at_wscan_handler },
    { "AT+WJOIN", user_at_wjoin_handler },
    { "AT+TPSRAM", user_at_psram_handler },
    { "AT+TGPIO", user_at_gpio_handler },
    { "AT+Z", user_at_z_handler },
    { "AT+GWRST", user_gateway_rst },
    { "AT+MICOPEN", at_cmd_micopen },
    { "AT+MICCLOSE", at_cmd_micclose },
    { "AT+AISOTA", at_cmd_ais_ota },
    // TODO:
    AT_NULL,
};

#if defined(CONFIG_SUPPORT_YMODEM) && (CONFIG_SUPPORT_YMODEM > 0)
static int app_at_ota_ymodem_network_enable(uint8_t enable)
{
    int ret = 0;
    if (enable) {
        ret = MODE_WIFI_NORMAL;
#if defined(CONFIG_GW_NETWORK_SUPPORT) && CONFIG_GW_NETWORK_SUPPORT
        ret = app_network_reinit();
#endif
#if defined(CONFIG_GW_FOTA_EN) && CONFIG_GW_FOTA_EN && defined(CONFIG_GW_USE_YOC_BOOTLOADER)                           \
    && CONFIG_GW_USE_YOC_BOOTLOADER
        extern void app_fota_start(void);
        if (ret == MODE_WIFI_NORMAL) {
            app_fota_start();
        }
#endif
#if defined(CONFIG_GW_SMARTLIVING_SUPPORT) && CONFIG_GW_SMARTLIVING_SUPPORT
        if (ret == MODE_WIFI_NORMAL) {
            smartliving_client_control(1);
        }
#endif

    } else {
#if defined(CONFIG_GW_FOTA_EN) && CONFIG_GW_FOTA_EN
        extern void app_fota_stop(void);
        app_fota_stop();
#endif

#if defined(CONFIG_GW_SMARTLIVING_SUPPORT) && CONFIG_GW_SMARTLIVING_SUPPORT
        smartliving_client_control(0);
#endif
#if defined(CONFIG_GW_NETWORK_SUPPORT) && CONFIG_GW_NETWORK_SUPPORT
        if (app_wifi_network_inited()) {
            app_wifi_network_deinit();
        }
#endif
    }

    return ret;
}

static inline int _app_at_ota_prepare()
{
    return app_at_ota_ymodem_network_enable(0);
}

__attribute__((weak)) int board_ota_gateway_upgrade(const void *image, uint32_t size)
{
    LOGE(TAG, "Board or chip components should implement the function of upgrading the local gateway");
    return 0;
}

static inline int _app_at_ota_upgrade(at_ota_target_en target, void *file, uint32_t file_size)
{
    int ret;

    /* the local target file is for the gateway itself, so call board_ota_xxx function to upgrade */
    if (target == AT_OTA_TARGET_LOCAL) {
        ret = board_ota_gateway_upgrade(file, file_size);
        if (ret) {
            return AT_OTA_ERR;
        }
        return AT_OTA_UPGRADED;
    }

    /* Other file is for remote subdevice, at_ota module will handles it internally */
    return AT_OTA_UPGRAD_SKIP;
}

static inline int _app_at_ota_finish(int result)
{
    return app_at_ota_ymodem_network_enable(1);
}

static at_ota_ops_t _app_at_ota_ops = {
    .prepare = _app_at_ota_prepare,
    .upgrade = _app_at_ota_upgrade,
    .finish  = _app_at_ota_finish,
};

#endif

void app_rpt_fwver(char *version)
{
#if defined(CONFIG_GW_AT_EN) && CONFIG_GW_AT_EN
    atserver_send("+FWVER=%s\r\n", version);
#endif
}

static void pcm_send(void *data, int len)
{
    if (g_mic_status) {
        atserver_lock();
        atserver_send("+MICDATA=%d,%d,", g_pcm_idx++, len);
        atserver_write(data, len);
        atserver_unlock();
    }
}

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

void app_at_server_init(utask_t *task)
{
    uart_config_t config;
    uart_config_default(&config);
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
    int ret;

    atserver_add_command(at_cmd);

    event_subscribe(EVENT_APP_AT_CMD, event_app_at_cmd_handler, NULL);

#if defined (CONFIG_SMARTLIVING_AT_MODULE) && (CONFIG_SMARTLIVING_AT_MODULE > 0)
    at_cmd_post_property_cb_reg(user_at_post_property);
    at_cmd_post_event_cb_reg(user_at_post_event);
    at_cmd_is_connected_cb_reg(smartliving_client_is_connected);
#endif

#if defined(CONFIG_SUPPORT_YMODEM) && (CONFIG_SUPPORT_YMODEM > 0)
    at_ota_config_t config = {
        .at_ota_port = YMODEM_UART_IDX,
        .ops         = &_app_at_ota_ops,
    };

    ret = at_ota_init(config);
    if (ret) {
        return ret;
    }
#endif
    return 0;
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
