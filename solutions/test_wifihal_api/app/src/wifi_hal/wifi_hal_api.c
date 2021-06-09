/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include "../yunit.h"
#include <devices/wifi.h>
#include <ulog/ulog.h>

#define TAG "wifi_hal_test"

static aos_dev_t *wifi_dev = NULL;

static void wifi_cb_sta_connect_fail(aos_dev_t *dev, wifi_err_code_t err, void *arg)
{
    LOGI(TAG, "%s\n", __FUNCTION__);
}

static void wifi_cb_status_change(aos_dev_t *dev, wifi_event_id_t stat, void *arg)
{
    LOGI(TAG, "%s\n", __FUNCTION__);
}

static void monitor_data_handler(wifi_promiscuous_pkt_t *buf, wifi_promiscuous_pkt_type_t type)
{
    LOGI(TAG, "%s\n", __FUNCTION__);
}

static void print_encrypt_mode(wifi_encrypt_type_t encryptmode)
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

static void wifi_cb_scan_compeleted(aos_dev_t *dev, uint16_t number, wifi_ap_record_t *ap_records)
{
    int i;

    printf("\nbssid / channel / signal dbm / flags / ssid\n\n");

    for (i = 0; i < number; i++) {
        wifi_ap_record_t *ap_record = &ap_records[i];

        printf("%02x:%02x:%02x:%02x:%02x:%02x %02d %d ",
               ap_record->bssid[0], ap_record->bssid[1], ap_record->bssid[2],
               ap_record->bssid[3], ap_record->bssid[4], ap_record->bssid[5],
               ap_record->channel, ap_record->rssi);

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

        printf("    %s\n",  ap_record->ssid);
    }

}

static void wifi_cb_fatal_err(aos_dev_t *dev, void *arg){}

static wifi_event_func evt_cb = {
    .sta_connect_fail = wifi_cb_sta_connect_fail,
    .status_change = wifi_cb_status_change,
    .scan_compeleted = wifi_cb_scan_compeleted,
    .fatal_err = wifi_cb_fatal_err
};



static void test_wifi_hal_init(void)
{
    int ret;
    if (wifi_dev == NULL) {
        wifi_dev = device_open_id("wifi", 0);
    }

    ret = hal_wifi_init(wifi_dev);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_INIT_01");

    ret = hal_wifi_init(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_INIT_02");

}

static void test_wifi_hal_deinit(void)
{
    int ret;
    if (wifi_dev == NULL) {
        wifi_dev = device_open_id("wifi", 0);
    }

    ret = hal_wifi_deinit(wifi_dev);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_DEINIT_01");

    ret = hal_wifi_deinit(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_DEINIT_02");

    ret = hal_wifi_reset(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_RESET_01");

    ret = hal_wifi_reset(wifi_dev);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_RESET_02");
}

static void test_wifi_hal_mac(void)
{
    int ret;
    uint8_t mac[6] = {0};
    uint8_t new_mac[6] = {0};

    if (wifi_dev == NULL) {
        wifi_dev = device_open_id("wifi", 0);
    }

    hal_wifi_get_mac_addr(NULL, mac);
    LOGD(TAG, "MAC address: %02x-%02x-%02x-%02x-%02x-%02x",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    YUNIT_ASSERT(mac[0] == 0 && mac[1] == 0 && mac[2] == 0 && mac[3] == 0 && mac[4] == 0 && mac[5] == 0);

    hal_wifi_get_mac_addr(wifi_dev, mac);
    LOGD(TAG, "MAC address: %02x-%02x-%02x-%02x-%02x-%02x",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    YUNIT_ASSERT(!(mac[0] == 0 && mac[1] == 0 && mac[2] == 0 && mac[3] == 0 && mac[4] == 0 && mac[5] == 0));

    mac[3] = (mac[3]+2) % 0x100;

    hal_wifi_set_mac_addr(wifi_dev, mac);
    hal_wifi_get_mac_addr(wifi_dev, new_mac);
    LOGD(TAG, "new MAC address: %02x-%02x-%02x-%02x-%02x-%02x",
               mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    YUNIT_ASSERT(mac[0] == new_mac[0] && mac[1] == new_mac[1] && mac[2] == new_mac[2] && \
                 mac[3] == new_mac[3] && mac[4] == new_mac[4] && mac[5] == new_mac[5]);
}


static void test_wifi_mode() 
{
    int ret;

    wifi_mode_t mode;

    ret = hal_wifi_set_mode(wifi_dev, WIFI_MODE_STA);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_MODE_01");

    hal_wifi_get_mode(wifi_dev, &mode);
    YUNIT_ASSERT_MSG_QA(mode == WIFI_MODE_STA, "mode=%d", mode, "YOC_WIFI_HAL_MODE_02");

    ret = hal_wifi_set_mode(wifi_dev, WIFI_MODE_AP);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_MODE_03");

    hal_wifi_get_mode(wifi_dev, &mode);
    YUNIT_ASSERT_MSG_QA(mode == WIFI_MODE_AP, "mode=%d", mode, "YOC_WIFI_HAL_MODE_04");

    ret = hal_wifi_set_mode(wifi_dev, WIFI_MODE_APSTA);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_MODE_05");

    hal_wifi_get_mode(wifi_dev, &mode);
    YUNIT_ASSERT_MSG_QA(mode == WIFI_MODE_APSTA, "mode=%d", mode, "YOC_WIFI_HAL_MODE_06");

    ret = hal_wifi_set_mode(wifi_dev, WIFI_MODE_P2P);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_ARG, "ret=%d", ret, "YOC_WIFI_HAL_MODE_07");

    // default to be WIFI_MODE_STA
    hal_wifi_get_mode(wifi_dev, &mode);
    YUNIT_ASSERT_MSG_QA(mode == WIFI_MODE_APSTA, "mode=%d", mode, "YOC_WIFI_HAL_MODE_08");


    // NULL aos_dev
    ret = hal_wifi_set_mode(NULL, WIFI_MODE_APSTA);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_MODE_09");

    ret = hal_wifi_get_mode(NULL, &mode);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_MODE_10");

    // restore to STA mode
    ret = hal_wifi_set_mode(wifi_dev, WIFI_MODE_STA);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_MODE_11");
}

static void test_wifi_install_evt_cb()
{
    int ret;
    ret = hal_wifi_install_event_cb(NULL, &evt_cb);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_CB_INSTALL_01");

    ret = hal_wifi_install_event_cb(wifi_dev, NULL);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_CB_INSTALL_02");

    ret = hal_wifi_install_event_cb(wifi_dev, &evt_cb);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_CB_INSTALL_03");
}

static void test_wifi_proto()
{
    int ret;

    uint8_t protocol_bitmap;

    ret = hal_wifi_set_protocol(wifi_dev, WIFI_PROTOCOL_11B);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_PROTO_01");

    ret = hal_wifi_get_protocol(wifi_dev, &protocol_bitmap);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_PROTO_02");
    YUNIT_ASSERT_MSG_QA(protocol_bitmap == WIFI_PROTOCOL_11B, \
                        "bitmap=%d", protocol_bitmap, "YOC_WIFI_HAL_PROTO_03");

    ret = hal_wifi_set_protocol(wifi_dev, (WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G));
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_PROTO_04");

    ret = hal_wifi_get_protocol(wifi_dev, &protocol_bitmap);
    YUNIT_ASSERT_MSG_QA(protocol_bitmap == (WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G), \
                        "bitmap=%d", protocol_bitmap, "YOC_WIFI_HAL_PROTO_05");

    ret = hal_wifi_set_protocol(NULL, WIFI_PROTOCOL_11B);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_PROTO_06");

    ret = hal_wifi_get_protocol(NULL, &protocol_bitmap);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_PROTO_07");

}

static void test_wifi_country()
{
    int ret;

    wifi_country_t country;

    ret = hal_wifi_get_country(wifi_dev, &country);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_COUNTRY_01");
    YUNIT_ASSERT_MSG_QA(country >= WIFI_COUNTRY_CN, "country=%d", country, "YOC_WIFI_HAL_COUNTRY_02");

    ret = hal_wifi_set_country(wifi_dev, WIFI_COUNTRY_US);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_COUNTRY_03");

    hal_wifi_get_country(wifi_dev, &country);
    YUNIT_ASSERT_MSG_QA(country == WIFI_COUNTRY_US, "country=%d", country, "YOC_WIFI_HAL_COUNTRY_04");

    ret = hal_wifi_get_country(NULL, &country);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_COUNTRY_05");

    ret = hal_wifi_set_country(NULL, WIFI_COUNTRY_EU);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_COUNTRY_06");

    //set to default again
    ret = hal_wifi_set_country(wifi_dev, WIFI_COUNTRY_CN);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_COUNTRY_07");
}


static void test_wifi_reconnect()
{
    int ret;

    bool en;

    ret = hal_wifi_get_auto_reconnect(wifi_dev, &en);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_RECONN_01");
    YUNIT_ASSERT_MSG_QA(en == false, "en=%d", en, "YOC_WIFI_HAL_RECONN_02");

    ret = hal_wifi_set_auto_reconnect(wifi_dev, true);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_RECONN_03");

    hal_wifi_get_auto_reconnect(wifi_dev, &en);
    YUNIT_ASSERT_MSG_QA(en == true, "en=%d", en, "YOC_WIFI_HAL_RECONN_04");

    ret = hal_wifi_get_auto_reconnect(NULL, &en);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_RECONN_05");

    ret = hal_wifi_set_auto_reconnect(NULL, false);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_RECONN_06");
}


static void test_wifi_lpm()
{
    int ret;

    wifi_lpm_mode_t mode;

    ret = hal_wifi_get_lpm(wifi_dev, &mode);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_LPM_01");
    YUNIT_ASSERT_MSG_QA(mode == WIFI_LPM_NONE, "mode=%d", mode, "YOC_WIFI_HAL_LPM_02");

    ret = hal_wifi_set_lpm(wifi_dev, WIFI_LPM_POWEROFF);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_LPM_03");

    hal_wifi_get_lpm(wifi_dev, &mode);
    YUNIT_ASSERT_MSG_QA(mode == WIFI_LPM_POWEROFF, "mode=%d", mode, "YOC_WIFI_HAL_LPM_04");

    ret = hal_wifi_get_lpm(NULL, &mode);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_LPM_05");

    ret = hal_wifi_set_lpm(NULL, WIFI_LPM_KEEP_SOCKET);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_LPM_06");

    // close lpm
    ret = hal_wifi_set_lpm(wifi_dev, WIFI_LPM_NONE);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_LPM_07");
}


static void test_wifi_power()
{
    int ret;

    ret = hal_wifi_power_on(wifi_dev);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_POWER_01");

    ret = hal_wifi_power_off(wifi_dev);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_POWER_02");

    ret = hal_wifi_power_off(wifi_dev);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_POWER_03");

    ret = hal_wifi_power_on(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_POWER_04");

    ret = hal_wifi_power_off(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_POWER_05");
}


static void test_wifi_channel()
{
    int ret;

    uint8_t primary;
    wifi_second_chan_t second;

    hal_wifi_stop(wifi_dev);
    aos_msleep(2000);

    ret = hal_wifi_get_channel(wifi_dev, &primary, &second);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_CHAN_01");
    YUNIT_ASSERT_MSG_QA(primary == 6 || primary == 1, "primary=%d", primary, "YOC_WIFI_HAL_CHAN_02");

    ret = hal_wifi_set_channel(wifi_dev, 0, WIFI_SECOND_CHAN_NONE);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_ARG, "ret=%d", ret, "YOC_WIFI_HAL_CHAN_03");

    hal_wifi_get_channel(wifi_dev, &primary, &second);
    YUNIT_ASSERT_MSG_QA(primary == 6 || primary == 1, "primary=%d", primary, "YOC_WIFI_HAL_CHAN_04");


    ret = hal_wifi_set_channel(wifi_dev, 1, WIFI_SECOND_CHAN_NONE);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_CHAN_05");

    aos_msleep(1000);
    hal_wifi_get_channel(wifi_dev, &primary, &second);
    YUNIT_ASSERT_MSG_QA(primary == 1, "primary=%d", primary, "YOC_WIFI_HAL_CHAN_06");

    ret = hal_wifi_get_channel(NULL, &primary, &second);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_CHAN_07");

    ret = hal_wifi_set_channel(NULL, 1, WIFI_SECOND_CHAN_NONE);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_CHAN_08");
}

static void test_wifi_scan()
{
    int ret;

    wifi_scan_config_t scan_config;
    memset(scan_config.ssid, 0, sizeof(scan_config.ssid));
    memset(scan_config.bssid, 0, sizeof(scan_config.bssid));
    scan_config.channel = 0;
    scan_config.show_hidden = 0;
    scan_config.scan_type = WIFI_SCAN_TYPE_ACTIVE;
    scan_config.scan_time.active.max = 0;
    scan_config.scan_time.active.min = 0;
    scan_config.scan_time.passive = 0;

    ret = hal_wifi_start_scan(wifi_dev, NULL, true);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_SCAN_01");
    aos_msleep(2000);

    ret = hal_wifi_start_scan(wifi_dev, NULL, false);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_SCAN_02");
    aos_msleep(2000);

    ret = hal_wifi_start_scan(wifi_dev, &scan_config, true);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_SCAN_03");
    aos_msleep(2000);

    ret = hal_wifi_start_scan(wifi_dev, &scan_config, false);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_SCAN_04");
    aos_msleep(2000);

    ret = hal_wifi_start_scan(NULL, &scan_config, false);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_SCAN_05");
    aos_msleep(2000);
}

static void test_wifi_start_stop()
{
    int ret;

    wifi_config_t config;
    memset(&config, 0, sizeof(wifi_config_t));
    config.mode = WIFI_MODE_AP;
    strcpy(config.ssid, "wifihal_ap");
    strcpy(config.password, "12345678");

    ret = hal_wifi_stop(wifi_dev);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_STOP_01");

    ret = hal_wifi_start(wifi_dev, &config);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_START_01");

    ret = hal_wifi_stop(wifi_dev);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_STOP_02");

    config.mode = WIFI_MODE_P2P;
    ret = hal_wifi_start(wifi_dev, &config);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_ARG, "ret=%d", ret, "YOC_WIFI_HAL_START_02");

    config.mode = WIFI_MODE_STA;
    ret = hal_wifi_start(NULL, &config);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_START_03");

    ret = hal_wifi_stop(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_STOP_03");
}

static void test_wifi_link_stat()
{
    int ret;

    wifi_ap_record_t ap_info = {0};
    wifi_status_link_t link_status;

    wifi_config_t config;
    memset(&config, 0, sizeof(wifi_config_t));
    config.mode = WIFI_MODE_STA;
    strcpy(config.ssid, "NETGEAR_HYJ");
    strcpy(config.password, "lab000000");

    hal_wifi_stop(wifi_dev);
    aos_msleep(2000);
    ret = hal_wifi_sta_get_link_status(wifi_dev, &ap_info);
    link_status = ap_info.link_status;
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_LINK_STAT_01");
    YUNIT_ASSERT_MSG_QA(link_status == WIFI_STATUS_LINK_DOWN, "link_status=%d", \
                        link_status, "YOC_WIFI_HAL_LINK_STAT_02");

    hal_wifi_start(wifi_dev, &config);
    aos_msleep(2000);
    ret = hal_wifi_sta_get_link_status(wifi_dev, &ap_info);
    link_status = ap_info.link_status;
    YUNIT_ASSERT_MSG_QA(link_status == WIFI_STATUS_LINK_UP, "link_status=%d", \
                        link_status, "YOC_WIFI_HAL_LINK_STAT_03");

    ret = hal_wifi_sta_get_link_status(NULL, &ap_info);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_LINK_STAT_04");
}


static void test_wifi_sta_list()
{
    int ret;

    wifi_sta_list_t sta_list = {0};
    
    wifi_ap_record_t ap_info = {0};
    wifi_status_link_t link_status;

    wifi_config_t config;
    memset(&config, 0, sizeof(wifi_config_t));
    config.mode = WIFI_MODE_AP;
    strcpy(config.ssid, "Yoc_W800");
    strcpy(config.password, "123456");

    hal_wifi_stop(wifi_dev);
    aos_msleep(2000);
    ret = hal_wifi_ap_get_sta_list(wifi_dev, &sta_list);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_STA_LIST_01");
    YUNIT_ASSERT_MSG_QA(sta_list.num == 0, "num=%d", sta_list.num, "YOC_WIFI_HAL_STA_LIST_02");

    hal_wifi_start(wifi_dev, &config);
    aos_msleep(2000);
    ret = hal_wifi_ap_get_sta_list(wifi_dev, &sta_list);
    link_status = ap_info.link_status;
    YUNIT_ASSERT_MSG_QA(sta_list.num == 0, "num=%d", sta_list.num, "YOC_WIFI_HAL_STA_LIST_03");

    ret = hal_wifi_ap_get_sta_list(NULL, &sta_list);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_STA_LIST_04");
}

static void test_wifi_monitor()
{
    int ret;

    ret = hal_wifi_start_monitor(wifi_dev, NULL);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_ARG, "ret=%d", ret, "YOC_WIFI_HAL_MONITOR_01");

    ret = hal_wifi_start_monitor(wifi_dev, monitor_data_handler);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_MONITOR_02");

    ret = hal_wifi_stop_monitor(wifi_dev);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_MONITOR_03");

    ret = hal_wifi_stop_monitor(wifi_dev);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_MONITOR_04");

    ret = hal_wifi_start_monitor(NULL, monitor_data_handler);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_MONITOR_05");

    ret = hal_wifi_stop_monitor(NULL);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_MONITOR_06");
}

static void test_wifi_send_raw()
{
    int ret;

    uint8_t buffer[30] = {0x1};

    hal_wifi_start_monitor(wifi_dev, monitor_data_handler);

    // len > sizeof(buffer)
    ret = hal_wifi_send_80211_raw_frame(wifi_dev, buffer, 6);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_ARG, "ret=%d", ret, "YOC_WIFI_HAL_SEND_01");

    ret = hal_wifi_send_80211_raw_frame(wifi_dev, buffer, 4);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_ARG, "ret=%d", ret, "YOC_WIFI_HAL_SEND_02");

    ret = hal_wifi_send_80211_raw_frame(wifi_dev, buffer, 30);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_SEND_03");

    hal_wifi_stop_monitor(wifi_dev);
}

static void test_wifi_smart_cfg()
{
    int ret;
    ret = hal_wifi_set_smartcfg(wifi_dev, 1);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_CFG_01");

    ret = hal_wifi_set_smartcfg(wifi_dev, 3);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_CFG_02");

    ret = hal_wifi_set_smartcfg(wifi_dev, 4);
    YUNIT_ASSERT_MSG_QA(ret == WIFI_ERR_OK, "ret=%d", ret, "YOC_WIFI_HAL_CFG_03");

    ret = hal_wifi_set_smartcfg(NULL, 2);
    YUNIT_ASSERT_MSG_QA(ret == -1, "ret=%d", ret, "YOC_WIFI_HAL_CFG_04");
}

void api_wifi_hal_test_entry(yunit_test_suite_t *suite)
{
    yunit_add_test_case(suite, "wifi.hal.init", test_wifi_hal_init);
    yunit_add_test_case(suite, "wifi.hal.mac", test_wifi_hal_mac);

    yunit_add_test_case(suite, "wifi.mode", test_wifi_mode);
    yunit_add_test_case(suite, "wifi.install_cb", test_wifi_install_evt_cb);
    yunit_add_test_case(suite, "wifi.proto", test_wifi_proto);
    yunit_add_test_case(suite, "wifi.country", test_wifi_country);
    yunit_add_test_case(suite, "wifi.reconnect", test_wifi_reconnect);
    yunit_add_test_case(suite, "wifi.lpm", test_wifi_lpm);
    yunit_add_test_case(suite, "wifi.power", test_wifi_power);
    yunit_add_test_case(suite, "wifi.channel", test_wifi_channel);
    yunit_add_test_case(suite, "wifi.scan", test_wifi_scan);

    yunit_add_test_case(suite, "wifi.start_stop", test_wifi_start_stop);
    yunit_add_test_case(suite, "wifi.linkstat", test_wifi_link_stat);
    yunit_add_test_case(suite, "wifi.sta_list", test_wifi_sta_list);

    yunit_add_test_case(suite, "wifi.monitor", test_wifi_monitor);
    yunit_add_test_case(suite, "wifi.send_raw", test_wifi_send_raw);
    yunit_add_test_case(suite, "wifi.smart_cfg", test_wifi_smart_cfg);

    yunit_add_test_case(suite, "wifi.hal.deinit", test_wifi_hal_deinit);
}