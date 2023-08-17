/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#include <stdint.h>
#include <devices/wifi.h>
#include <yoc/netmgr.h>
#include <yoc/netmgr_service.h>

#include "app_wifi.h"

#define TAG "appwifi"

extern netmgr_hdl_t     app_netmgr_hdl;

static aos_sem_t g_wifi_config_sem;

static void scan_compeleted(rvm_dev_t *dev, uint16_t number, rvm_hal_wifi_ap_record_t *ap_records)
{
    rvm_hal_wifi_ap_record_t wifiApRecord;
    for (int j = 0; j < number; ++j) {
        for (int i = j + 1; i < number; ++i) {
            if (ap_records[i].rssi > ap_records[j].rssi) {
                wifiApRecord  = ap_records[i];
                ap_records[i] = ap_records[j];
                ap_records[j] = wifiApRecord;
            }
        }
    }

    for (int j = 0; j < number; ++j) {
        app_wifi_config_add_ap((char *)ap_records[j].ssid);
    }

    aos_sem_signal(&g_wifi_config_sem);
}

static rvm_hal_wifi_event_func wifi_event = {NULL, NULL, scan_compeleted, NULL};
void app_wifi_network_init_list()
{
    char *ssid, *psk;

    LOGI(TAG, "Start wifi network");
    rvm_dev_t *dev = netmgr_get_dev(app_netmgr_hdl);
    rvm_hal_net_set_hostname(dev, "T-head");

    if (app_wifi_config_get_ssid_num() != 1) {

        rvm_hal_wifi_install_event_cb(dev, &wifi_event);
        aos_check(!aos_sem_new(&g_wifi_config_sem, 0), EIO);

        rvm_hal_wifi_start_scan(dev, NULL, 1);

        aos_sem_wait(&g_wifi_config_sem, AOS_WAIT_FOREVER);
        rvm_hal_wifi_install_event_cb(dev, NULL);

        aos_sem_free(&g_wifi_config_sem);

        int ret = app_wifi_config_get_last_ssid_psk(&ssid, &psk);

        if (ret < 0) {
            return;
        }
    } else {
        int ret = app_wifi_config_get_last_ssid_psk(&ssid, &psk);

        if (ret < 0) {
            //return;
            ssid = "Yoc";
            psk = "testtest";
        }
    }

    netmgr_config_wifi(app_netmgr_hdl, ssid, strlen(ssid), psk, strlen(psk));

    LOGI(TAG, "SSID=%s PASS=%s", ssid, psk);
    netmgr_start(app_netmgr_hdl);

    return;
}

int app_wifi_getmac(uint8_t mac[6])
{
    int ret = -1;

    rvm_dev_t *dev = netmgr_get_dev(app_netmgr_hdl);

    if (dev) {
        rvm_hal_wifi_set_mode(dev, WIFI_MODE_STA);
        ret = rvm_hal_wifi_get_mac_addr(dev, mac);
    } else {
        LOGE(TAG, "get mac failed! Device not found!");
    }

    return ret;
}

int app_bt_getmac(char* mac)
{
    int ret = -1;

    rvm_dev_t *dev = netmgr_get_dev(app_netmgr_hdl);

    if (dev) {
        rvm_hal_wifi_set_mode(dev, WIFI_MODE_STA);
        extern int bt_get_mac_address(char *mac);
        ret = bt_get_mac_address(mac);
    } else {
        LOGE(TAG, "get mac failed! Device not found!");
    }

    return ret;
}

void app_wifi_set_lpm(int lpm_on)
{
    if (app_netmgr_hdl) {
        rvm_dev_t *dev = netmgr_get_dev(app_netmgr_hdl);
        rvm_hal_wifi_set_lpm(dev, lpm_on ? WIFI_LPM_KEEP_LINK : WIFI_LPM_NONE);
    }
}

