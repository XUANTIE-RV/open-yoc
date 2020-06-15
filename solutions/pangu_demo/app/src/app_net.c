/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <app_config.h>

#include <stdlib.h>
#include <string.h>
#include <devices/devicelist.h>
#include <devices/wifi.h>
#include <devices/rtl8723ds.h>
#include <yoc/netmgr_service.h>
#include <yoc/eventid.h>
#include <yoc/uservice.h>
#include <yoc/init.h>
#include <ntp.h>

#include "app_main.h"
#include "audio_res.h"
#include "pair_broadcast.h"
#include "ap_pair.h"

#define TAG "app_net"

#define NET_RESET_DELAY_TIME            15 /*second*/
#define NET_LPM_RECONNECT_DELAY_TIME    180 /*second*/
#define MAX_NET_RESET_ERR_TIMES         -1
#define MAX_NET_RETRY_TIMES             5

netmgr_hdl_t app_netmgr_hdl;
static int ntp_sync_flag;
static int wifi_pairing;
static int wifi_pair_need_broadcast;
static int net_reset_err_times = 0;
static int net_connecting = 1;
static int net_lpm_en = 0;

int wifi_internet_is_connected()
{
    return (ntp_sync_flag == 1);
}

void wifi_set_lpm(int lpm_on)
{
    if (app_netmgr_hdl) {
        aos_dev_t *dev = netmgr_get_dev(app_netmgr_hdl);
        hal_wifi_set_lpm(dev, lpm_on ? WIFI_LPM_KEEP_LINK : WIFI_LPM_NONE);
    }
}

static void internet_set_connected(int connected)
{
    ntp_sync_flag = connected;
}

static int wifi_network_inited()
{
    return (app_netmgr_hdl != NULL);
}

int wifi_is_pairing()
{
    return (wifi_pairing);
}

static void wifi_set_pairing(int pairing)
{
    wifi_pairing = pairing;
}


static void network_reset_handle(uint32_t event_id)
{
    switch(event_id) {
    case EVENT_NETMGR_NET_DISCON:
        net_reset_err_times++;
        LOGD(TAG, "net reset cnt %d", net_reset_err_times);
        
        if (net_reset_err_times >= MAX_NET_RESET_ERR_TIMES && MAX_NET_RESET_ERR_TIMES > 0) {
            LOGD(TAG, "Net Reset times %d, reboot", net_reset_err_times);
            //do reboot
            aos_reboot();
        } else {
            if (net_lpm_en && net_reset_err_times > MAX_NET_RETRY_TIMES) {
                // if in low power mode, net should stop retry to allow system to goto sleep
                event_publish(EVENT_NET_LPM_RECONNECT, NULL);
            } else {
                net_connecting = 1;
                netmgr_reset(app_netmgr_hdl, NET_RESET_DELAY_TIME);
            }
        }
        break;
    case EVENT_NETMGR_GOT_IP:
        net_connecting = 0;
        net_reset_err_times = 0;
        break;
    case EVENT_NET_LPM_RECONNECT:
        LOGD(TAG, "reconnect %d s later", NET_LPM_RECONNECT_DELAY_TIME);
        net_connecting = 0;
        net_reset_err_times = 0;
        event_publish_delay(EVENT_NETMGR_NET_DISCON, NULL, NET_LPM_RECONNECT_DELAY_TIME * 1000);
    default:
        break;
    }
}


static void network_normal_handle(uint32_t event_id, const void *param)
{
    switch (event_id) {
    case EVENT_NETMGR_GOT_IP: {
        char *ip_addr_str = ipaddr_ntoa((ip4_addr_t *)ip_2_ip4(param));
        LOGD(TAG, "got ip, %s", ip_addr_str);
        if (strcmp(ip_addr_str, "0.0.0.0") == 0) {
            break;
        }
        internet_set_connected(0);  //init
        event_publish(EVENT_NTP_RETRY_TIMER, NULL);

        /** pair done broadcast */
        if (wifi_pair_need_broadcast) {
            wifi_pair_need_broadcast = 0;
            static uint8_t mac[6] = {0};
            aos_dev_t *dev = netmgr_get_dev(app_netmgr_hdl);
            hal_wifi_get_mac_addr(dev, mac);
            wifi_pair_broadcast(mac);
        }
    } break;

    case EVENT_NETMGR_NET_DISCON: {
        LOGD(TAG, "Net down");
        /* 不主动语音提示异常，等有交互再提示 */
        internet_set_connected(0);

        // app_status_update();
    } break;

    case EVENT_NET_CHECK_TIMER: {
        if (wifi_internet_is_connected() == 0) {
            /* (配网超时)您输入的密码好像不太对，请检查后再试一次吧  */
            local_audio_play(LOCAL_AUDIO_NET_CFG_FAIL);
            // aos_msleep(2 * 1000);
            // wifi_pair_start();
        } else {
            LOGI(TAG, "wifi connection: check ok");
        }
    } break;

    case EVENT_NTP_RETRY_TIMER:
        if (wifi_is_pairing()) {
            LOGD(TAG, "is pairing wifi...");
            break;
        }
        if (ntp_sync_time(NULL) == 0) {

            /* 网络对时成功,同步到RTC中 */
            app_rtc_from_system();

            if (wifi_internet_is_connected() == 0){
                /* 同步到时间,确认网络成功,提示音和升级只在第一次启动 */
                internet_set_connected(1);
                local_audio_play(LOCAL_AUDIO_NET_SUCC);
                event_publish(EVENT_NET_NTP_SUCCESS, NULL);

                app_fota_start();
            }
        } else {
            /* 同步时间失败重试 */
            event_publish_delay(EVENT_NTP_RETRY_TIMER, NULL, 6000);
        }
        break;
    default:
        break;
    }
}


static void user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    if ((wifi_is_pairing() == 0) && wifi_network_inited()) {
        network_normal_handle(event_id, param);
        network_reset_handle(event_id);
    } else {
        LOGE(TAG, "Critical network status callback %d", event_id);
    }
}


static void wifi_network_deinit()
{
    LOGI(TAG, "Stop wifi network");

    netmgr_stop(app_netmgr_hdl);
    netmgr_dev_wifi_deinit(app_netmgr_hdl);
    netmgr_service_deinit();

    app_netmgr_hdl = NULL;
}

static netmgr_hdl_t wifi_network_init(char *ssid, char *psk)
{
    LOGI(TAG, "Start wifi network");
    LOGD(TAG, "SSID=%s PASS=%s", ssid, psk);

    utask_t *task = utask_new("netmgr", 4 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);

    netmgr_hdl_t hdl = netmgr_dev_wifi_init();
    netmgr_service_init(task);
    netmgr_config_wifi(hdl, ssid, strlen(ssid), psk, strlen(psk));
    // aos_dev_t *dev = netmgr_get_dev(hdl);
    // hal_net_set_hostname(dev, "T-head");
    netmgr_start(hdl);
    app_netmgr_hdl = hdl;

    return hdl;
}


static int wifi_pair_callback(int timeout, char *ssid, char *password)
{
    if (timeout) {
        LOGD(TAG, "wifi pair timeout...");
        local_audio_play(LOCAL_AUDIO_NET_CFG_TIMEOUT); /* 配网超时 */

        static char wifi_ssid[32 + 1];
        int wifi_ssid_len = sizeof(wifi_ssid);
        static char wifi_psk[64 + 1];
        int wifi_psk_len = sizeof(wifi_psk);

        aos_kv_get("wifi_ssid", wifi_ssid, &wifi_ssid_len);
        aos_kv_get("wifi_psk", wifi_psk, &wifi_psk_len);

        if (strlen(wifi_ssid) > 0)
            wifi_network_init(wifi_ssid, wifi_psk);
    } else {
        LOGD(TAG, "wifi pair got passwd...");
        local_audio_play(LOCAL_AUDIO_NET_CFG_CONN); /* 收到密码，开始连接 */
    }

    if (ssid && strlen(ssid) > 0) {
        wifi_network_init(ssid, password);
        event_publish_delay(EVENT_NET_CHECK_TIMER, NULL, 30*1000);
    } else {
        LOGI(TAG, "nothing I can do");
    }

    wifi_set_pairing(0);

    return 0;
}

static void _wifi_pair_thread(void *arg)
{
    wifi_set_pairing(1);
    wifi_pair_need_broadcast = 1;

    LOGD(TAG, "start net config");
    app_player_stop();  // stop play when start wifi pair
    // app_status_update();

    local_audio_play(LOCAL_AUDIO_NET_CFG_START); /* 进入配网模式 */
    internet_set_connected(0);

    if (wifi_network_inited()) {
        wifi_network_deinit();
    }

    ap_pair_start(300*1000, "YoC", "", wifi_pair_callback);
}

void wifi_pair_start(void)
{
    aos_task_t task_handle;

    if (wifi_is_pairing()) {
        local_audio_play(LOCAL_AUDIO_NET_CFG_CONFIG); /* 正在配置网络，请稍后 */
        return;
    }

    if (0 != aos_task_new_ext(&task_handle, "pair_start", _wifi_pair_thread, NULL,
                                3*1024, AOS_DEFAULT_APP_PRI)) {
        LOGE(TAG, "Create pair_start task failed.");
    }
}

int wifi_connecting()
{
    return net_connecting;
}

void wifi_lpm_enable(int lpm_en)
{
    net_lpm_en = lpm_en;
}

int wifi_getmac(uint8_t mac[6])
{
    int ret = -1;

    aos_dev_t *dev = netmgr_get_dev(app_netmgr_hdl);

    if (dev) {
        ret = hal_wifi_get_mac_addr(dev, mac);
    }
    return ret;
}

static wifi_mode_e app_net_init(void)
{
    char ssid[32 + 1] = {0};
    int ssid_len = sizeof(ssid);
    char psk[64 + 1] = {0};
    int psk_len = sizeof(psk);

    /* 系统事件订阅 */
    event_subscribe(EVENT_NETMGR_GOT_IP, user_local_event_cb, NULL);
    event_subscribe(EVENT_NETMGR_NET_DISCON, user_local_event_cb, NULL);

    /* 使用系统事件的定时器 */
    event_subscribe(EVENT_NTP_RETRY_TIMER, user_local_event_cb, NULL);
    event_subscribe(EVENT_NET_CHECK_TIMER, user_local_event_cb, NULL);
    event_subscribe(EVENT_NET_LPM_RECONNECT, user_local_event_cb, NULL);

    aos_kv_get("wifi_ssid", ssid, &ssid_len);
    aos_kv_get("wifi_psk", psk, &psk_len);
    if (strlen(ssid) == 0) {
        wifi_pair_start();
        return MODE_WIFI_PAIRING;
    } else {
        wifi_network_init(ssid, psk);
    }
    return MODE_WIFI_NORMAL;
}

wifi_mode_e app_network_init(void)
{
    int wifi_en;
    int ret;
    ret = aos_kv_getint("wifi_en", &wifi_en);
    if (ret < 0) {
        /* 若没有设置KV,默认使能 */
        wifi_en = 1;
    }

    if (wifi_en == 0) {
        return MODE_WIFI_CLOSE;
    }

    /* init wifi driver and network */
    rtl8723ds_gpio_pin pin = {
        .wl_en = WLAN_ENABLE_PIN,
        .power = WLAN_POWER_PIN,
    };
    wifi_rtl8723ds_register(&pin);

    /* 该模式，初始化驱动，但返回错误，关闭应用网络流程*/
    if (wifi_en == 2) {
        LOGI(TAG, "@@@ WiFi Debug Mode @@@");
        extern void hal_wifi_test_enabled(int en);
        hal_wifi_test_enabled(1);
        return MODE_WIFI_TEST;
    }

    return app_net_init();
}
