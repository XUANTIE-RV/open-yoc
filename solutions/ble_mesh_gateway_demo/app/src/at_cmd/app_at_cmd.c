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
