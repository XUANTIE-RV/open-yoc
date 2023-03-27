/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <yoc/netmgr.h>
#include <ulog/ulog.h>
#include <aos/kv.h>
#include <softap_prov.h>
#include <wifi_provisioning.h>

#include "app_net.h"
#include "app_player.h"
#include "event_mgr/app_event.h"

#include "wifi_prov.h"
#include "app_main.h"

#define TAG "wifiprov"

extern netmgr_hdl_t app_netmgr_hdl;
static int          g_wifi_pairing;
int g_wifi_prov_method = WIFI_PROVISION_SOFTAP;


int wifi_is_pairing()
{
    return (g_wifi_pairing);
}

void wifi_pair_set_prov_type(int type)
{
    g_wifi_prov_method = type;
}

int wifi_pair_get_prov_type()
{
    return g_wifi_prov_method;
}

static void wifi_set_pairing(int pairing)
{
    g_wifi_pairing = pairing;
}

static void wifi_network_init(char *ssid, char *psk)
{
    LOGI(TAG, "Start wifi network");
    LOGD(TAG, "SSID=%s PASS=%s", ssid, psk);

    netmgr_config_wifi(app_netmgr_hdl, ssid, strlen(ssid), psk, strlen(psk));
    rvm_dev_t *dev = netmgr_get_dev(app_netmgr_hdl);
    rvm_hal_net_set_hostname(dev, "T-head");
    netmgr_start(app_netmgr_hdl);

    return;
}

static void wifi_pair_callback(uint32_t method_id, wifi_prov_event_t event,
                               wifi_prov_result_t *result)
{
    if (event == WIFI_PROV_EVENT_TIMEOUT) {
        LOGD(TAG, "wifi pair timeout...");
        app_event_update(EVENT_STATUS_WIFI_PROV_TIMEOUT);

        static char wifi_ssid[32 + 1];
        int         wifi_ssid_len = sizeof(wifi_ssid);
        static char wifi_psk[64 + 1];
        int         wifi_psk_len = sizeof(wifi_psk);

        aos_kv_get("wifi_ssid", wifi_ssid, &wifi_ssid_len);
        aos_kv_get("wifi_psk", wifi_psk, &wifi_psk_len);

        if (!app_wifi_config_is_empty())
            app_wifi_network_init_list();

    } else if (event == WIFI_RPOV_EVENT_GOT_RESULT) {
        LOGD(TAG, "wifi pair got passwd...");
        app_event_update(EVENT_STATUS_WIFI_PROV_RECVED);

        app_wifi_config_add(result->ssid, result->password);
        wifi_network_init(result->ssid, result->password);

        event_publish_delay(EVENT_NET_CHECK_TIMER, NULL, WIFI_STA_CONNNECT_TIMEOUT * 1000);
    }

    wifi_set_pairing(0);
}

static void _wifi_pair_thread(void *arg)
{
    int ret = 0;
    wifi_set_pairing(1);

    LOGD(TAG, "start net config, method[%d]", g_wifi_prov_method);
    smtaudio_stop(MEDIA_ALL); // stop play when start wifi pair

    wifi_prov_stop();

    app_network_internet_set_connected(0);

    switch (g_wifi_prov_method) {
    case WIFI_PROVISION_SOFTAP:
        ret = wifi_prov_start(wifi_prov_get_method_id("softap"), wifi_pair_callback, WIFI_PROV_TIMEOUT);
        break;
    default:
        LOGE(TAG, "unsupported wifi provisioning method!");
        wifi_set_pairing(0);
        ret = -1;
        break;
    }

    if (0 == ret) {
        /* 进入配网模式 */
        app_event_update(EVENT_STATUS_WIFI_PROV_START);
    } else {
        /* 配网失败 */
        app_event_update(EVENT_STATUS_WIFI_PROV_FAILED);
    }
}

void wifi_pair_start(void)
{
    aos_task_t task_handle;

    if (wifi_is_pairing()) {
        /* 正在配置网络，请稍后 */
        app_event_update(EVENT_STATUS_WIFI_PROV_ALREADY_START);
        return;
    }

    if (0 != aos_task_new_ext(&task_handle, "pair_start", _wifi_pair_thread, NULL, 10 * 1024,
                              AOS_DEFAULT_APP_PRI)) {
        LOGE(TAG, "Create pair_start task failed.");
    }
}

void wifi_pair_stop(void)
{
    if (wifi_prov_get_status() != WIFI_PROV_STOPED) {
        wifi_prov_stop();
        wifi_set_pairing(0);

        if (!app_wifi_config_is_empty())
            app_wifi_network_init_list();
    }
}
