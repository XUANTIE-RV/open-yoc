/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <aos/aos.h>
#include <aos/kv.h>
#include <yoc/atserver.h>
#include <uservice/eventid.h>
#include "devices/wifi.h"
#include "devices/driver.h"
#include "devices/netdrv.h"
#include "at_internal.h"
#include <board.h>
#include <at_cmd.h>

#define TAG "app_factory"

void app_ftmode_entry(void)
{
    LOGD(TAG, "===>factory mode");

    /* AT cmd related */
    // app_at_server_init(NULL, CONFIG_AT_UART_NAME);
    // app_at_cmd_init();

    while (1) {
        aos_msleep(10000);
    }
}

void app_ftsetting_restore()
{
    extern int ble_mesh_node_reset();
    ble_mesh_node_reset(); /* RST mesh */

    extern int app_fota_occ_url_reset();

#if defined(CONFIG_GW_FOTA_EN) && CONFIG_GW_FOTA_EN
    app_fota_occ_url_reset(); /* RST occ url */
#endif

#if defined(CONFIG_SMARTLIVING_DEMO) && CONFIG_SMARTLIVING_DEMO
    extern int app_gateway_ut_reset();
    app_gateway_ut_reset(); /* RST gateway ut */
#endif

#if defined(CONFIG_GW_NETWORK_SUPPORT) && CONFIG_GW_NETWORK_SUPPORT
    extern int app_wifi_reset();
    app_wifi_reset(); /* RST wifi info */
#endif

    extern int iotx_guider_clear_dynamic_url();
    iotx_guider_clear_dynamic_url(); /* Clear mqtt url */

    aos_reboot();
}

/********************************************************************************/
/* for fct test start
********************************************************************************/
typedef struct {
    uint8_t ssid[MAX_SSID_SIZE + 1];
    int     max_rssi;
    int     min_rssi;
} wscan_param_st;

static wscan_param_st _g_wscan_param = { 0 };

static void wifi_cb_sta_connect_fail(rvm_dev_t *dev, rvm_hal_wifi_err_code_t err, void *arg)
{
    LOGI(TAG, "%s\n", __FUNCTION__);
}

static void wifi_cb_status_change(rvm_dev_t *dev, rvm_hal_wifi_event_id_t stat, void *arg)
{
    LOGI(TAG, "%s\n", __FUNCTION__);
}

static void print_encrypt_mode(rvm_hal_wifi_encrypt_type_t encryptmode)
{
    switch (encryptmode) {
        case WIFI_ENC_TKIP:
            printf("TKIP");
            break;

        case WIFI_ENC_AES:
            printf("CCMP");
            break;

        case WIFI_ENC_TKIP_AES_MIX:
            printf("TKIP+CCMP");
            break;

        default:
            printf("ERR");
            break;
    }
}

static void wifi_cb_scan_compeleted(rvm_dev_t *dev, uint16_t number, rvm_hal_wifi_ap_record_t *ap_records)
{
    int i;
    int ret = -1;
    printf("\nbssid / channel / signal dbm / flags / ssid\n\n");

    for (i = 0; i < number; i++) {
        rvm_hal_wifi_ap_record_t *ap_record = &ap_records[i];

        printf("%02x:%02x:%02x:%02x:%02x:%02x %02d %d ", ap_record->bssid[0], ap_record->bssid[1], ap_record->bssid[2],
               ap_record->bssid[3], ap_record->bssid[4], ap_record->bssid[5], ap_record->channel, ap_record->rssi);

        switch (ap_record->authmode) {
            case WIFI_AUTH_OPEN:
                printf("[OPEN]");
                break;

            case WIFI_AUTH_WEP:
                printf("[WEP]");
                break;

            case WIFI_AUTH_WPA_PSK:
                printf("[WPA-PSK-");
                print_encrypt_mode(ap_record->encryptmode);
                printf("]");
                break;

            case WIFI_AUTH_WPA2_PSK:
                printf("[WPA2-PSK-");
                print_encrypt_mode(ap_record->encryptmode);
                printf("]");
                break;

            case WIFI_AUTH_WPA_WPA2_PSK:
                printf("[WPA-PSK-");
                print_encrypt_mode(ap_record->encryptmode);
                printf("]");
                printf("[WPA2-PSK-");
                print_encrypt_mode(ap_record->encryptmode);
                printf("]");
                break;

            default:
                printf("[NOT SUPPORT]");
                break;
        }

        printf("    %s\n", ap_record->ssid);

        if (0 == strcmp((const char *)ap_record->ssid, (const char *)_g_wscan_param.ssid)) {
            ret = 1;

            if (_g_wscan_param.max_rssi >= ap_record->rssi && _g_wscan_param.min_rssi <= ap_record->rssi) {
                ret = 0;
            }
        }
    }

    if (ret) {
        AT_BACK_ERRNO(ret);
    } else {
        AT_BACK_OK();
    }
}

static void wifi_cb_fatal_err(rvm_dev_t *dev, void *arg) {}

static rvm_hal_wifi_event_func evt_cb = { .sta_connect_fail = wifi_cb_sta_connect_fail,
                                  .status_change    = wifi_cb_status_change,
                                  .scan_compeleted  = wifi_cb_scan_compeleted,
                                  .fatal_err        = wifi_cb_fatal_err };

static void wifihal_scan()
{
    rvm_dev_t *wifi_dev = rvm_hal_device_find("wifi", 0);

    if (wifi_dev == NULL) {
        LOGE(TAG, "NO wifi driver");
        return;
    }

    rvm_dev_t *        dev = wifi_dev;
    wifi_scan_config_t scan_config;

    memset(scan_config.ssid, 0, sizeof(scan_config.ssid));
    memset(scan_config.bssid, 0, sizeof(scan_config.bssid));
    scan_config.channel              = 0;
    scan_config.show_hidden          = 0;
    scan_config.scan_type            = WIFI_SCAN_TYPE_ACTIVE;
    scan_config.scan_time.active.max = 200;
    scan_config.scan_time.active.min = 100;
    scan_config.scan_time.passive    = 150;

    rvm_hal_wifi_install_event_cb(dev, &evt_cb);
    LOGI(TAG, "wifi block scan");
    rvm_hal_wifi_start_scan(dev, NULL, 1);
    LOGI(TAG, "wifi block scan done");
}

static void wifihal_connect_to_ap(const char *ssid, const char *psk)
{
    rvm_dev_t *wifi_dev = rvm_hal_device_find("wifi", 0);

    if (wifi_dev == NULL) {
        LOGE(TAG, "NO wifi driver");
        AT_BACK_ERRNO(-1);
        return;
    }

    rvm_dev_t *dev = wifi_dev;

    aos_kv_setstring("wifi_ssid", ssid);
    aos_kv_setstring("wifi_psk", psk);

    if (rvm_hal_wifi_reset(dev) < 0) {
        event_publish(EVENT_NETMGR_NET_DISCON, NULL);
        AT_BACK_ERRNO(-1);
        return;
    }

    rvm_hal_wifi_config_t *wifi_config = aos_zalloc(sizeof(rvm_hal_wifi_config_t));

    if (wifi_config == NULL) {
        AT_BACK_ERRNO(-1);
        return;
    }

    wifi_config->mode = WIFI_MODE_STA;
    strcpy(wifi_config->ssid, ssid);
    strcpy(wifi_config->password, psk);
    int ret = rvm_hal_wifi_start(dev, wifi_config);
    aos_free(wifi_config);

    if (ret == 0) {
        LOGI(TAG, "ssid{%s}, psk{%s}\n", ssid, psk);
    } else {
        LOGW(TAG, "no ap info");
        AT_BACK_ERRNO(-1);
    }
}

void user_at_wscan_handler(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        int   max_rssi, min_rssi;
        char *ssid  = data;
        char *token = strchr(data, ',');

        if (token) {
            *token = '\0';
            token++;
        } else {
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        data     = token;
        max_rssi = atoi(data);
        token    = strchr(data, ',');

        if (token) {
            *token = '\0';
            token++;
        } else {
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        data     = token;
        min_rssi = atoi(data);
        LOGI(TAG, "ssid: %s, max_rssi %d, min_rssi %d\n", ssid, max_rssi, min_rssi);
        _g_wscan_param.max_rssi = max_rssi;
        _g_wscan_param.min_rssi = min_rssi;
        strcpy((char *)_g_wscan_param.ssid, ssid);
        wifihal_scan();
    } else {
        AT_BACK_ERRNO(AT_ERR_INVAL);
    }
}

void event_app_connect_handler(uint32_t event_id, const void *param, void *context)
{
    LOGI(TAG, "===>connect success");

    atserver_send("+EVENT=%s,%s\r\n", "NET", "LINK_UP");
    AT_BACK_OK();
}

void user_at_wjoin_handler(char *cmd, int type, char *data)
{
    if (type == WRITE_CMD) {
        char *ssid  = data;
        char *token = strchr(data, ',');

        if (token) {
            *token = '\0';
            token++;
        } else {
            AT_BACK_ERRNO(AT_ERR_INVAL);
            return;
        }

        event_subscribe(EVENT_WIFI_LINK_UP, event_app_connect_handler, NULL);

        wifihal_connect_to_ap(ssid, token);
    } else {
        AT_BACK_ERRNO(AT_ERR_INVAL);
    }
}

__attribute__((weak)) int board_ftmode_check(void)
{
    return 0;
}

__attribute__((weak)) int board_gpio_test(void)
{
    return 0;
}

__attribute__((weak)) int board_ex_sram_test(void)
{
    return 0;
}

void user_at_psram_handler(char *cmd, int type, char *data)
{
    int ret = 0;

    if (type == EXECUTE_CMD) {
        ret = board_ex_sram_test();

        if (ret == 0) {
            AT_BACK_OK();
        } else {
            AT_BACK_ERRNO(AT_ERR_STATUS);
        }
    } else {
        AT_BACK_ERRNO(AT_ERR_INVAL);
    }
}

void user_at_gpio_handler(char *cmd, int type, char *data)
{
    int ret = 0;

    if (type == EXECUTE_CMD) {
        ret = board_gpio_test();

        if (ret == 0) {
            AT_BACK_OK();
        } else {
            AT_BACK_ERRNO(AT_ERR_STATUS);
        }
    } else {
        AT_BACK_ERRNO(AT_ERR_INVAL);
    }
}

void user_at_z_handler(char *cmd, int type, char *data)
{
    if (type == EXECUTE_CMD) {
        AT_BACK_OK();
        aos_msleep(100);
        aos_reboot();
    } else {
        AT_BACK_ERRNO(AT_ERR_INVAL);
    }
}

/********************************************************************************/
/* for fct test end
********************************************************************************/
