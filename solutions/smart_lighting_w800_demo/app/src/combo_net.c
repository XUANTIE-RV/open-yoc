/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#ifdef EN_COMBO_NET
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "aos/kernel.h"
#include <aos/yloop.h>
#include <core.h>
//#include <devices/wifi.h>
#include "yoc/netmgr.h"
#include "yoc/netmgr_service.h"
#include "smartliving/iot_export.h"
#include "smartliving/iot_import.h"
#include "smartliving/imports/iot_import_awss.h"
#include "breeze_export.h"
#include "combo_net.h"
#include "app_main.h"
#define HAL_IEEE80211_IS_BEACON     ieee80211_is_beacon
#define HAL_IEEE80211_IS_PROBE_RESP ieee80211_is_probe_resp
#define HAL_IEEE80211_GET_SSID      ieee80211_get_ssid
#define HAL_IEEE80211_GET_BSSID     aw_ieee80211_get_bssid
#define HAL_AWSS_OPEN_MONITOR       HAL_Awss_Open_Monitor
#define HAL_AWSS_CLOSE_MONITOR      HAL_Awss_Close_Monitor
#define HAL_AWSS_SWITCH_CHANNEL     HAL_Awss_Switch_Channel

#define MAX_SSID_SIZE 32
#define MAX_PWD_SIZE  64

#ifndef MAX_SSID_LEN
#define MAX_SSID_LEN (MAX_SSID_SIZE + 1)
#endif

#define TAG "combo"

struct ieee80211_hdr {
    uint16_t frame_control;
    uint16_t duration_id;
    uint8_t  addr1[ETH_ALEN];
    uint8_t  addr2[ETH_ALEN];
    uint8_t  addr3[ETH_ALEN];
    uint16_t seq_ctrl;
    uint8_t  addr4[ETH_ALEN];
};

typedef struct {
    char    ssid[MAX_SSID_SIZE + 1];
    uint8_t bssid[ETH_ALEN];
    char    pwd[MAX_PWD_SIZE + 1];
} netmgr_ap_config_t;

char                      awss_running; // todo
static breeze_apinfo_t    apinfo;
static char               monitor_got_bssid = 0;
static netmgr_ap_config_t config;
uint8_t                   g_ble_state    = 0;
static uint8_t            g_disconn_flag = 0;
extern uint8_t            g_ap_state;
uint8_t                   g_bind_state = 0;
static wifi_prov_cb       prov_cb      = NULL;

extern uint8_t *    os_wifi_get_mac(uint8_t mac[6]);
extern netmgr_hdl_t wifi_network_init(char *ssid, char *psk);

extern int user_combo_get_dev_status_handler(uint8_t *buffer, uint32_t length);
extern int tls_get_mac_addr(uint8_t *mac);

/* Device info(five elements) will be used by ble breeze */
/* ProductKey, ProductSecret, DeviceName, DeviceSecret, ProductID */
char g_combo_pk[PRODUCT_KEY_LEN + 1]    = {0};
char g_combo_ps[PRODUCT_SECRET_LEN + 1] = {0};
char g_combo_dn[DEVICE_NAME_LEN + 1]    = {0};
char g_combo_ds[DEVICE_SECRET_LEN + 1]  = {0};

int g_combo_pid = 0;

uint8_t combo_get_ap_state(void);

/*
 * Note:
 * the linkkit_event_monitor must not block and should run to complete fast
 * if user wants to do complex operation with much time,
 * user should post one task to do this, not implement complex operation in
 * linkkit_event_monitor
 */

static void linkkit_event_monitor(int event)
{
    switch (event) {
    case IOTX_AWSS_START: // AWSS start without enbale, just supports device discover
        // operate led to indicate user
        LOGD(TAG, "IOTX_AWSS_START");
        break;
    case IOTX_AWSS_ENABLE: // AWSS enable, AWSS doesn't parse awss packet until AWSS is enabled.
        LOGD(TAG, "IOTX_AWSS_ENABLE");
        // operate led to indicate user
        break;
    case IOTX_AWSS_LOCK_CHAN: // AWSS lock channel(Got AWSS sync packet)
        LOGD(TAG, "IOTX_AWSS_LOCK_CHAN");
        // operate led to indicate user
        break;
    case IOTX_AWSS_PASSWD_ERR: // AWSS decrypt passwd error
        LOGD(TAG, "IOTX_AWSS_PASSWD_ERR");
        // operate led to indicate user
        break;
    case IOTX_AWSS_GOT_SSID_PASSWD:
        LOGD(TAG, "IOTX_AWSS_GOT_SSID_PASSWD");
        // operate led to indicate user
        //set_net_state(GOT_AP_SSID);
        break;
    case IOTX_AWSS_CONNECT_ADHA: // AWSS try to connnect adha (device
        // discover, router solution)
        LOGD(TAG, "IOTX_AWSS_CONNECT_ADHA");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ADHA_FAIL: // AWSS fails to connect adha
        LOGD(TAG, "IOTX_AWSS_CONNECT_ADHA_FAIL");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_AHA: // AWSS try to connect aha (AP solution)
        LOGD(TAG, "IOTX_AWSS_CONNECT_AHA");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_AHA_FAIL: // AWSS fails to connect aha
        LOGD(TAG, "IOTX_AWSS_CONNECT_AHA_FAIL");
        // operate led to indicate user
        break;
    case IOTX_AWSS_SETUP_NOTIFY: // AWSS sends out device setup information
        // (AP and router solution)
        LOGD(TAG, "IOTX_AWSS_SETUP_NOTIFY");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ROUTER: // AWSS try to connect destination router
        LOGD(TAG, "IOTX_AWSS_CONNECT_ROUTER");
        // operate led to indicate user
        break;
    case IOTX_AWSS_CONNECT_ROUTER_FAIL: // AWSS fails to connect destination
        // router.
        LOGD(TAG, "IOTX_AWSS_CONNECT_ROUTER_FAIL");
        //set_net_state(CONNECT_AP_FAILED);
        // operate led to indicate user
        break;
    case IOTX_AWSS_GOT_IP: // AWSS connects destination successfully and got
        // ip address
        LOGD(TAG, "IOTX_AWSS_GOT_IP");
        // operate led to indicate user
        break;
    case IOTX_AWSS_SUC_NOTIFY: // AWSS sends out success notify (AWSS
        // sucess)
        LOGD(TAG, "IOTX_AWSS_SUC_NOTIFY");
        // operate led to indicate user
        break;
    case IOTX_AWSS_BIND_NOTIFY: // AWSS sends out bind notify information to
        // support bind between user and device
        LOGD(TAG, "IOTX_AWSS_BIND_NOTIFY");
        // operate led to indicate user
        //user_example_ctx_t *user_example_ctx = user_example_get_ctx();
        //user_example_ctx->bind_notified = 1;
        break;
    case IOTX_AWSS_ENABLE_TIMEOUT: // AWSS enable timeout
        // user needs to enable awss again to support get ssid & passwd of router
        LOGD(TAG, "IOTX_AWSS_ENALBE_TIMEOUT");
        // operate led to indicate user
        break;
    case IOTX_CONN_CLOUD: // Device try to connect cloud
        LOGD(TAG, "IOTX_CONN_CLOUD");
        // operate led to indicate user
        break;
    case IOTX_CONN_CLOUD_FAIL: // Device fails to connect cloud, refer to
        // net_sockets.h for error code
        LOGD(TAG, "IOTX_CONN_CLOUD_FAIL");
        //set_net_state(CONNECT_CLOUD_FAILED);
        // operate led to indicate user
        break;
    case IOTX_CONN_CLOUD_SUC: // Device connects cloud successfully
        //set_net_state(CONNECT_CLOUD_SUCCESS);
        LOGD(TAG, "IOTX_CONN_CLOUD_SUC");
        // operate led to indicate user
        break;
    case IOTX_RESET: // Linkkit reset success (just got reset response from
        // cloud without any other operation)
        LOGD(TAG, "IOTX_RESET");
        break;
    case IOTX_CONN_REPORT_TOKEN_SUC:
#ifdef EN_COMBO_NET
        combo_token_report_notify();
#endif
        LOGD(TAG, "---- report token success ----");
        break;
    default:
        break;
    }
}

static void combo_status_change_cb(breeze_event_t event)
{
    switch (event) {
    case CONNECTED:
        LOGD(TAG, "Ble Connected");
        g_ble_state = 1;
        break;

    case DISCONNECTED:
        LOGD(TAG, "Ble Disconnected");
        g_ble_state = 0;
        aos_post_event(EV_BZ_COMBO, COMBO_EVT_CODE_RESTART_ADV, 0);
        break;

    case AUTHENTICATED:
        LOGD(TAG, "Ble Authenticated");
        break;

    case TX_DONE:
        if (g_disconn_flag && g_ble_state) {
            LOGD(TAG, "Payload TX Done");
            breeze_disconnect_ble();
            breeze_stop_advertising();
            g_disconn_flag = 2;
        }
        break;

    case EVT_USER_BIND:
        LOGD(TAG, "Ble bind");
        g_bind_state = 1;
        awss_clear_reset();
        break;

    case EVT_USER_UNBIND:
        LOGD(TAG, "Ble unbind");
        g_bind_state = 0;
        break;

    default:
        break;
    }
}

void combo_net_post_ext(void)
{
    char format[] = "{\"code\":200,\"data\":{},\"id\":\"2\"}";
    breeze_post_ext_fast(3 /*BZ_CMD_REPL*/, (uint8_t *)format, strlen(format)); // BZ_CMD_REPL
}

static void combo_set_dev_status_handler(uint8_t *buffer, uint32_t length)
{
    LOGD(TAG, "COM_SET:%.*s", length, buffer);
}

static void combo_get_dev_status_handler(uint8_t *buffer, uint32_t length)
{
    LOGD(TAG, "COM_GET:%.*s", length, buffer);

    user_combo_get_dev_status_handler(buffer, length);
}

static void combo_apinfo_rx_cb(breeze_apinfo_t *ap)
{
    if (!ap)
        return;

    memcpy(&apinfo, ap, sizeof(apinfo));
    aos_post_event(EV_BZ_COMBO, COMBO_EVT_CODE_AP_INFO, (unsigned long)&apinfo);
}

static int combo_80211_frame_callback(char *buf, int length, enum AWSS_LINK_TYPE link_type,
                                      int with_fcs, signed char rssi)
{
    uint8_t               ssid[MAX_SSID_LEN] = {0}, bssid[ETH_ALEN] = {0};
    struct ieee80211_hdr *hdr;
    int                   fc;
    int                   ret = -1;

    if (link_type != AWSS_LINK_TYPE_NONE) {
        return -1;
    }
    hdr = (struct ieee80211_hdr *)buf;

    fc = hdr->frame_control;
    if (!HAL_IEEE80211_IS_BEACON(fc) && !HAL_IEEE80211_IS_PROBE_RESP(fc)) {
        return -1;
    }
    ret = HAL_IEEE80211_GET_BSSID((uint8_t *)hdr, bssid);
    if (ret < 0) {
        return -1;
    }
    if (memcmp(config.bssid, bssid, ETH_ALEN) != 0) {
        return -1;
    }
    ret = HAL_IEEE80211_GET_SSID((uint8_t *)hdr, length, ssid);
    if (ret < 0) {
        return -1;
    }
    monitor_got_bssid = 1;
    strncpy(config.ssid, (char *)ssid, sizeof(config.ssid) - 1);
    HAL_AWSS_CLOSE_MONITOR();
    return 0;
}

static char is_str_asii(char *str)
{
    for (uint32_t i = 0; str[i] != '\0'; ++i) {
        if ((uint8_t)str[i] > 0x7F) {
            return 0;
        }
    }
    return 1;
}

static void combo_connect_ap(breeze_apinfo_t *info)
{
    int            ms_cnt       = 0;
    uint8_t        ap_connected = 0;
    ap_scan_info_t scan_result;
    int            ap_scan_result = -1;

    memset(&config, 0, sizeof(netmgr_ap_config_t));
    if (!info)
        return;

    strncpy(config.ssid, info->ssid, sizeof(config.ssid) - 1);
    strncpy(config.pwd, info->pw, sizeof(config.pwd) - 1);
    if (info->apptoken_len > 0) {
        memcpy(config.bssid, info->bssid, ETH_ALEN);
        awss_set_token(info->apptoken, info->token_type);
    }
    // get region information
    if (info->region_type == REGION_TYPE_ID) {
        LOGD(TAG, "info->region_id: %d", info->region_id);
        iotx_guider_set_dynamic_region(info->region_id);
    } else if (info->region_type == REGION_TYPE_MQTTURL) {
        LOGD(TAG, "info->region_mqtturl: %s", info->region_mqtturl);
        iotx_guider_set_dynamic_mqtt_url(info->region_mqtturl);
    } else {
        LOGD(TAG, "REGION TYPE not supported");
        iotx_guider_set_dynamic_region(IOTX_CLOUD_REGION_INVALID);
    }

    wifi_prov_result_t res;
    strncpy(res.ssid, info->ssid, sizeof(res.ssid) - 1);
    strncpy(res.password, info->pw, sizeof(res.password) - 1);

    if (prov_cb)
        prov_cb(0, WIFI_RPOV_EVENT_GOT_RESULT, &res);

    aos_msleep(3000);

    netmgr_hdl_t hdl = netmgr_get_handle("wifi");
    if (hdl) {
        while (ms_cnt < 30000) {
            if (!netmgr_is_gotip(hdl)) {
                aos_msleep(500);
                ms_cnt += 500;
                LOGD(TAG, "wait ms_cnt(%d)", ms_cnt);
            } else {
                LOGD(TAG, "AP connected");
                ap_connected = 1;
                break;
            }
        }
    }

    if (!ap_connected) {
        uint16_t err_code = 0;

        memset(&scan_result, 0, sizeof(ap_scan_info_t));
        LOGD(TAG, "start awss_apscan_process");
        ap_scan_result = awss_apscan_process(NULL, config.ssid, &scan_result);
        LOGD(TAG, "stop awss_apscan_process");
        if ((ap_scan_result == 0) && (scan_result.found)) {
            if (scan_result.rssi < -70) {
                err_code = 0xC4E1; // rssi too low
            } else {
                err_code =
                    0xC4E3; // AP connect fail(Authentication fail or Association fail or AP exeption)
            }
            // should set all ap info to err msg
        } else {
            err_code = 0xC4E0; // AP not found
        }

        if (g_ble_state) {
            uint8_t ble_rsp[DEV_ERRCODE_MSG_MAX_LEN + 8] = {0};
            uint8_t ble_rsp_idx                          = 0;
            ble_rsp[ble_rsp_idx++]                       = 0x01; // Notify Code Type
            ble_rsp[ble_rsp_idx++]                       = 0x01; // Notify Code Length
            ble_rsp[ble_rsp_idx++]                       = 0x02; // Notify Code Value, 0x02-fail
            ble_rsp[ble_rsp_idx++]                       = 0x03; // Notify SubErrcode Type
            ble_rsp[ble_rsp_idx++] = sizeof(err_code);           // Notify SubErrcode Length
            memcpy(ble_rsp + ble_rsp_idx, (uint8_t *)&err_code,
                   sizeof(err_code)); // Notify SubErrcode Value
            ble_rsp_idx += sizeof(err_code);
            breeze_post(ble_rsp, ble_rsp_idx);
        }
    }
}

void combo_ap_conn_notify(void)
{
    uint8_t rsp[] = {0x01, 0x01, 0x01};
    if (g_ble_state) {
        breeze_post(rsp, sizeof(rsp));
    }
}

void combo_token_report_notify(void)
{
    uint8_t rsp[] = {0x01, 0x01, 0x03};
    if (g_ble_state) {
        breeze_post(rsp, sizeof(rsp));
        g_disconn_flag = 1;
    }
}

static void combo_service_evt_handler(input_event_t *event, void *priv_data)
{
    if (event->type != EV_BZ_COMBO) {
        return;
    }

    if (event->code == COMBO_EVT_CODE_AP_INFO) {
        awss_running = 1;
        combo_connect_ap((breeze_apinfo_t *)event->value);
    } else if (event->code == COMBO_EVT_CODE_RESTART_ADV) {
        if (g_disconn_flag == 0) {
            breeze_stop_advertising();
            aos_msleep(500); // wait for adv stop
            breeze_start_advertising(combo_get_ap_state() ? 4 : 3, g_bind_state, g_bind_state);
        }
    }
}

static int reverse_byte_array(uint8_t *byte_array, int byte_len)
{
    uint8_t temp = 0;
    int     i    = 0;
    if (!byte_array) {
        LOGE(TAG, "reverse_mac invalid params!");
        return -1;
    }
    for (i = 0; i < (byte_len / 2); i++) {
        temp                         = byte_array[i];
        byte_array[i]                = byte_array[byte_len - i - 1];
        byte_array[byte_len - i - 1] = temp;
    }
    return 0;
}

int combo_net_init(wifi_prov_cb cb)
{
    breeze_dev_info_t dinfo = {0};

    if ((0 == HAL_GetProductKey(g_combo_pk)) || (0 == HAL_GetDeviceName(g_combo_dn)) ||
        (0 == HAL_GetProductSecret(g_combo_ps)) || (0 == HAL_GetDeviceSecret(g_combo_ds)) ||
        (0 != aos_kv_getint("hal_devinfo_pid", &g_combo_pid))) {
        LOGE(TAG, "missing ProductKey/ProductSecret/DeviceName/DeviceSecret/PID!");
        return -1;
    }

    prov_cb = cb;

    aos_register_event_filter(EV_BZ_COMBO, combo_service_evt_handler, NULL);
    g_bind_state = breeze_get_bind_state();
    if ((strlen(g_combo_pk) > 0) && (strlen(g_combo_ps) > 0) && (strlen(g_combo_dn) > 0) &&
        (strlen(g_combo_ds) > 0) && g_combo_pid > 0) {
        uint8_t combo_adv_mac[6] = {0};
        tls_get_mac_addr(combo_adv_mac);
        // Set wifi mac to breeze awss adv, to notify app this is a wifi dev
        // Awss use wifi module device info.
        // Only use BT mac when use breeze and other bluetooth communication functions
        reverse_byte_array(combo_adv_mac, 6);
        dinfo.product_id     = g_combo_pid;
        dinfo.product_key    = g_combo_pk;
        dinfo.product_secret = g_combo_ps;
        dinfo.device_name    = g_combo_dn;
        dinfo.device_secret  = g_combo_ds;
        dinfo.dev_adv_mac    = combo_adv_mac;
        breeze_awss_init(&dinfo, combo_status_change_cb, combo_set_dev_status_handler,
                         combo_get_dev_status_handler, combo_apinfo_rx_cb, NULL);
        breeze_awss_start();
        iotx_event_regist_cb(linkkit_event_monitor);
        breeze_start_advertising(combo_get_ap_state() ? 4 : 3, g_bind_state, g_bind_state);
    } else {
        LOGE(TAG, "combo device info not set!");
    }
    return 0;
}

int combo_net_deinit()
{
    breeze_awss_stop();
    return 0;
}

uint8_t combo_ble_conn_state(void)
{
    return g_ble_state;
}

uint8_t combo_get_bind_state(void)
{
    return 0;
}

uint8_t combo_get_ap_state(void)
{
    netmgr_hdl_t hdl = netmgr_get_handle("wifi");
    if (hdl) {
        return netmgr_is_gotip(hdl);
    }
    return 0;
}

#endif
