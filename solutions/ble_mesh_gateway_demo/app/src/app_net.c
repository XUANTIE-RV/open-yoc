/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <devices/wifi.h>
#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>
#include <ntp.h>

#include "app_main.h"
#include "app_sys.h"
//#include <softap_prov.h>
#include <wifi_provisioning.h>
#include "app_voice.h"

#if defined(CONFIG_WIFI_SMARTLIVING) && CONFIG_WIFI_SMARTLIVING
#include <smartliving/sl_wifi_prov.h>
#endif
#if defined(EN_COMBO_NET) && EN_COMBO_NET
#include "combo_net.h"
#endif
#if defined(CONFIG_SAVE_JS_TO_RAM) && (CONFIG_SAVE_JS_TO_RAM == 1)
#include "linkkit_gateway/app_gateway_ut.h"
#endif
#include <gateway_occ_auth.h>
#include <gateway.h>
#include <app_fota.h>

#define TAG "app_net"

#define NET_RESET_DELAY_TIME         30  /* 最大网络重连时间 second*/
#define NET_LPM_RECONNECT_DELAY_TIME 180 /*second*/
#define MAX_NET_RESET_ERR_TIMES      -1  /* -1无效，若设置该值次数，达到该次数后系统重启 */
#define MAX_NET_RETRY_TIMES          3   /* 该值在开启低功耗模式生效，达到次数允许进入睡眠 */

netmgr_hdl_t            app_netmgr_hdl;
static int              ntp_sync_flag;
static int              wifi_pairing;
static int              wifi_pair_need_broadcast;
static int              net_reset_err_times  = 0;
static int              net_reset_delay_time = 2;
static int              net_connecting       = 1;
static int              net_lpm_en           = 0;
static int              pair_thread_running  = 0;
static app_init_state_e state                = APP_INIT_GATEWAY;

#if defined(CONFIG_WIFI_SMARTLIVING) && CONFIG_WIFI_SMARTLIVING
int wifi_prov_method = WIFI_PROVISION_SL_BLE;
#else
int wifi_prov_method = WIFI_PROVISION_SOFTAP;
#endif

extern int wifi_is_connected_to_ap(void);

static int _wifi_internet_is_connected()
{
    return (ntp_sync_flag == 1);
}

void wifi_set_lpm(int lpm_on)
{
    if (app_netmgr_hdl) {
        rvm_dev_t *dev = netmgr_get_dev(app_netmgr_hdl);
        rvm_hal_wifi_set_lpm(dev, lpm_on ? WIFI_LPM_KEEP_LINK : WIFI_LPM_NONE);
    }
}

static void _internet_set_connected(int connected)
{
    ntp_sync_flag = connected;
}

static int _wifi_is_pairing()
{
    return (wifi_pairing);
}

static void wifi_set_pairing(int pairing)
{
    wifi_pairing = pairing;
}

static void _network_reset_handle(uint32_t event_id)
{
    switch (event_id) {
        case EVENT_NETMGR_NET_DISCON:
            net_reset_err_times++;
            LOGD(TAG, "net reset cnt %d", net_reset_err_times);

            if (net_reset_err_times >= MAX_NET_RESET_ERR_TIMES && MAX_NET_RESET_ERR_TIMES > 0) {
                LOGD(TAG, "Net Reset times %d, reboot", net_reset_err_times);
                // do reboot
                aos_reboot();
            } else {
                if (net_lpm_en && net_reset_err_times > MAX_NET_RETRY_TIMES) {
                    LOGD(TAG, "Net Reset times %d, goto sleep", net_reset_err_times);
                    // if in low power mode, net should stop retry to allow system to goto sleep
                    event_publish(EVENT_NET_LPM_RECONNECT, NULL);
                } else {
                    if (!_wifi_is_pairing()) {
                        net_connecting = 1;
                        netmgr_reset(app_netmgr_hdl, net_reset_delay_time);

                        /* double delay time to reconnect */
                        net_reset_delay_time *= 2;
                        if (net_reset_delay_time > NET_RESET_DELAY_TIME) {
                            net_reset_delay_time = NET_RESET_DELAY_TIME;
                        }
                    }
                }
            }
            break;
        case EVENT_NETMGR_GOT_IP:
            net_connecting       = 0;
            net_reset_err_times  = 0;
            net_reset_delay_time = 2;
            break;
        case EVENT_NET_LPM_RECONNECT:
            LOGD(TAG, "reconnect %d s later", NET_LPM_RECONNECT_DELAY_TIME);
            net_connecting       = 0;
            net_reset_err_times  = 0;
            net_reset_delay_time = 2;
            event_publish_delay(EVENT_NETMGR_NET_DISCON, NULL, NET_LPM_RECONNECT_DELAY_TIME * 1000);
        default:
            break;
    }
}

static void _network_normal_handle(uint32_t event_id, const void *param)
{
    static int synced_before = 0;

    switch (event_id) {
        case EVENT_NETMGR_GOT_IP: {
            event_publish(EVENT_NTP_RETRY_TIMER, NULL);

            /* 配网成功后广播通知 */
            if (wifi_pair_need_broadcast == 1) {
                wifi_pair_need_broadcast = 2;

                if (wifi_prov_method == WIFI_PROVISION_SOFTAP) {
                    static uint8_t mac[6] = { 0 };
                    rvm_dev_t *    dev    = netmgr_get_dev(app_netmgr_hdl);
                    rvm_hal_wifi_get_mac_addr(dev, mac);
                    // wifi_pair_broadcast(mac);
                }
#if defined(CONFIG_GW_SMARTLIVING_SUPPORT) && CONFIG_GW_SMARTLIVING_SUPPORT && defined(EN_COMBO_NET) && EN_COMBO_NET
                if (wifi_prov_method == WIFI_PROVISION_SL_BLE) {
                    combo_ap_conn_notify();
                }
#endif
            }
        } break;

        case EVENT_NETMGR_NET_DISCON: {
            LOGD(TAG, "Net down");
            /* 不主动语音提示异常，等有交互再提示 */
            _internet_set_connected(0);

            // app_status_update();
        } break;

        case EVENT_NET_CHECK_TIMER: {
            if (_wifi_internet_is_connected() == 0) {
                ;
            } else {
                LOGI(TAG, "wifi connection: check ok");
            }
        } break;

        case EVENT_NTP_RETRY_TIMER:
            if (ntp_sync_time(NULL) == 0) {
                if (_wifi_internet_is_connected() == 0) {
                    /* 同步到时间,确认网络成功,提示音和升级只在第一次启动 */
                    _internet_set_connected(1);

                    if (!synced_before || wifi_pair_need_broadcast) {
#if defined(CONFIG_WIFI_SMARTLIVING) && CONFIG_WIFI_SMARTLIVING
                        if (wifi_pair_need_broadcast) {
                            LOGD(TAG, "wifi provisiong success");
                            wifi_prov_sl_set_connected(1);
#if !(defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO)
                            wifi_prov_sl_start_report();
#endif
                        }
#endif
                        event_publish(EVENT_NET_NTP_SUCCESS, NULL);
#if defined(CONFIG_GW_FOTA_EN) && CONFIG_GW_FOTA_EN
                        app_fota_start();
                        app_fota_rpt_gw_ver();
#endif

#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO
                        extern int smartliving_client_control(const int start);
                        smartliving_client_control(1);
#endif

                        synced_before            = 1;
                        wifi_pair_need_broadcast = 0;
                    }
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

static void _user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    if ((_wifi_is_pairing() == 0) && app_wifi_network_inited()) {
        _network_normal_handle(event_id, param);
        _network_reset_handle(event_id);
    } else {
        LOGE(TAG, "Critical network status callback %d", event_id);
    }
}

void app_wifi_network_deinit()
{
    LOGI(TAG, "Stop wifi network");

    netmgr_stop(app_netmgr_hdl);
    netmgr_dev_wifi_deinit(app_netmgr_hdl);
    netmgr_service_deinit();

    app_netmgr_hdl = NULL;
}

int app_wifi_network_inited()
{
    return (app_netmgr_hdl != NULL);
}

static netmgr_hdl_t _wifi_network_init(char *ssid, char *psk)
{
    LOGI(TAG, "Start wifi network");
    LOGD(TAG, "SSID=%s PASS=%s", ssid, psk);

    utask_t *task = utask_new("netmgr", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);

    netmgr_hdl_t hdl = netmgr_dev_wifi_init();
    netmgr_service_init(task);
    netmgr_config_wifi(hdl, ssid, strlen(ssid), psk, strlen(psk));
    rvm_dev_t *dev = netmgr_get_dev(hdl);
    rvm_hal_net_set_hostname(dev, "T-head");
    netmgr_start(hdl);
    app_netmgr_hdl = hdl;

    return hdl;
}

static void _wifi_pair_callback(uint32_t method_id, wifi_prov_event_t event, wifi_prov_result_t *result)
{
    if (event == WIFI_PROV_EVENT_TIMEOUT) {
        LOGD(TAG, "wifi pair timeout...");

        static char wifi_ssid[32 + 1];
        int         wifi_ssid_len = sizeof(wifi_ssid);
        static char wifi_psk[64 + 1];
        int         wifi_psk_len = sizeof(wifi_psk);

        aos_kv_get("wifi_ssid", wifi_ssid, &wifi_ssid_len);
        aos_kv_get("wifi_psk", wifi_psk, &wifi_psk_len);

        if (strlen(wifi_ssid) > 0)
            _wifi_network_init(wifi_ssid, wifi_psk);

#if defined(CONFIG_GW_SMARTLIVING_SUPPORT) && CONFIG_GW_SMARTLIVING_SUPPORT && defined(EN_COMBO_NET) && (EN_COMBO_NET)
        if (app_combo_is_breeze_start() == 1) {
            app_combo_set_init_state();
        }
#endif
        app_voice_play(VOICE_ID_PROVISIONING_FAIL, 90);

    } else if (event == WIFI_RPOV_EVENT_GOT_RESULT) {
        LOGD(TAG, "wifi pair got passwd...");

        _wifi_network_init(result->ssid, result->password);

        event_publish_delay(EVENT_NET_CHECK_TIMER, NULL, 30 * 1000);
        app_voice_play(VOICE_ID_PROVISIONING_SUCCESS, 90);
    }

    wifi_set_pairing(0);
}

static void _wifi_pair_thread(void *arg)
{
    pair_thread_running = 1;
    wifi_set_pairing(1);
    wifi_pair_need_broadcast = 1;

    LOGD(TAG, "start net config");
    // app_status_update();

#if defined(CONFIG_WIFI_SMARTLIVING) && CONFIG_WIFI_SMARTLIVING
    wifi_prov_sl_stop_report();
#endif
    wifi_prov_stop();

    _internet_set_connected(0);

    if (app_wifi_network_inited()) {
        app_wifi_network_deinit();
    }

    if (wifi_prov_method != WIFI_PROVISION_SL_BLE) {
        event_publish(EVENT_APP_INIT_STATE, (void *)&state);
    }

    switch (wifi_prov_method) {
        case WIFI_PROVISION_SOFTAP:
            wifi_prov_start(wifi_prov_get_method_id("softap"), _wifi_pair_callback, 120);
            break;
#if defined(CONFIG_WIFI_SMARTLIVING) && CONFIG_WIFI_SMARTLIVING
        extern rvm_dev_t *device_open_id(const char *name, int id);
        case WIFI_PROVISION_SL_DEV_AP:
            rvm_hal_wifi_init(
                device_open_id("wifi", 0)); // init wifi before get wifi mac addr since the ap ssid is built by mac
            wifi_prov_start(wifi_prov_get_method_id("sl_dev_ap"), _wifi_pair_callback, 120);
            break;

        case WIFI_PROVISION_SL_SMARTCONFIG:
            wifi_prov_start(wifi_prov_get_method_id("sl_smartconfig"), _wifi_pair_callback, 120);
            break;

        case WIFI_PROVISION_SL_BLE:
            printf("%s, %d\r\n", __FUNCTION__, __LINE__);
            wifi_prov_start(wifi_prov_get_method_id("sl_ble"), _wifi_pair_callback, 120);
            break;
#endif
        default:
            LOGE(TAG, "unsupported wifi provisioning method!");
            wifi_set_pairing(0);
            break;
    }

    pair_thread_running = 0;
}

int app_wifi_reset()
{
    aos_kv_del("wifi_ssid");
    aos_kv_del("wifi_psk");
    aos_kv_del("wifi_en");
    return 0;
}

void app_wifi_pair_start(void)
{
    aos_task_t task_handle;

    if (pair_thread_running) {
        return;
    }

    app_voice_play(VOICE_ID_PROVISIONING, 90);

    if (0 != aos_task_new_ext(&task_handle, "pair_start", _wifi_pair_thread, NULL, 3 * 1024, AOS_DEFAULT_APP_PRI)) {
        LOGE(TAG, "Create pair_start task failed.");
    }
}

void app_wifi_pair_stop(void)
{
    if (wifi_prov_get_status() != WIFI_PROV_STOPED) {
        wifi_prov_stop();
        wifi_set_pairing(0);
        // app_status_update();
        char wifi_ssid[32 + 1] = "\0";
        int  wifi_ssid_len     = sizeof(wifi_ssid);
        char wifi_psk[64 + 1]  = "\0";
        int  wifi_psk_len      = sizeof(wifi_psk);

        aos_kv_get("wifi_ssid", wifi_ssid, &wifi_ssid_len);
        aos_kv_get("wifi_psk", wifi_psk, &wifi_psk_len);

        if (strlen(wifi_ssid) > 0) {
            _wifi_network_init(wifi_ssid, wifi_psk);
        }
    }
}

static wifi_mode_e _app_net_init(void)
{
    char ssid[32 + 1] = { 0 };
    int  ssid_len     = sizeof(ssid);
    char psk[64 + 1]  = { 0 };
    int  psk_len      = sizeof(psk);

    /* 系统事件订阅 */
    event_subscribe(EVENT_NETMGR_GOT_IP, _user_local_event_cb, NULL);
    event_subscribe(EVENT_NETMGR_NET_DISCON, _user_local_event_cb, NULL);

    /* 使用系统事件的定时器 */
    event_subscribe(EVENT_NTP_RETRY_TIMER, _user_local_event_cb, NULL);
    event_subscribe(EVENT_NET_CHECK_TIMER, _user_local_event_cb, NULL);
    event_subscribe(EVENT_NET_LPM_RECONNECT, _user_local_event_cb, NULL);

    aos_kv_get("wifi_ssid", ssid, &ssid_len);
    aos_kv_get("wifi_psk", psk, &psk_len);

    if (strlen(ssid) == 0 || app_sys_get_boot_reason() == BOOT_REASON_WIFI_CONFIG) {
        if (app_sys_get_boot_reason() == BOOT_REASON_WIFI_CONFIG) {
            int last_method = 0;
            if (aos_kv_getint("wprov_method", &last_method) == 0) {
                wifi_prov_method = last_method;
                aos_kv_del("wprov_method");
            }
        }
        app_wifi_pair_start();
        app_sys_set_boot_reason(BOOT_REASON_SOFT_RESET);
        return MODE_WIFI_PAIRING;
    } else {
        event_publish(EVENT_APP_INIT_STATE, (void *)&state);
        _wifi_network_init(ssid, psk);
    }
    return MODE_WIFI_NORMAL;
}

wifi_mode_e app_network_init(void)
{
    int network_en;
    int ret;
    ret = aos_kv_getint("network_en", &network_en);
    if (ret < 0) {
        /* 若没有设置KV,默认使能 */
        network_en = 1;
    }

    if (network_en == 0) {
        return MODE_WIFI_CLOSE;
    }

#if defined(CONFIG_YOC_SOFTAP_PROV) && CONFIG_YOC_SOFTAP_PROV
    extern int wifi_prov_softap_register(char *ap_ssid);
    wifi_prov_softap_register("YoC");
#endif
#if defined(CONFIG_WIFI_SMARTLIVING) && CONFIG_WIFI_SMARTLIVING
    wifi_prov_sl_register();
#endif

    return _app_net_init();
}

wifi_mode_e app_network_reinit()
{
    int network_en;
    int ret;
    ret = aos_kv_getint("network_en", &network_en);
    if (ret < 0) {
        /* 若没有设置KV,默认使能 */
        network_en = 1;
    }

    if (network_en == 0) {
        return MODE_WIFI_CLOSE;
    }

    // app_status_update();
#if defined(CONFIG_WIFI_SMARTLIVING) && CONFIG_WIFI_SMARTLIVING
    wifi_prov_sl_stop_report();
#endif
    wifi_prov_stop();

    _internet_set_connected(0);

    if (app_wifi_network_inited()) {
        app_wifi_network_deinit();
    }
    return _app_net_init();
}
