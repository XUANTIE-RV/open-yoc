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
#include <uservice/eventid.h>
#include <uservice/uservice.h>
#include <yoc/init.h>
#include <ntp.h>

#include "board_config.h"
#include "app_main.h"
#include <yoc/iot.h>


#define TAG "app_net"

#define NET_RESET_DELAY_TIME 15          /*second*/
#define NET_LPM_RECONNECT_DELAY_TIME 180 /*second*/
#define MAX_NET_RESET_ERR_TIMES -1
#define MAX_NET_RETRY_TIMES 5

netmgr_hdl_t app_netmgr_hdl;
static int   ntp_sync_flag;

#define DATA_PUSH_INTERVAL (5 * 1000)

static iot_t *        iot_handle      = NULL;
static iot_channel_t *iot_ch_post     = NULL;
static int            g_iot_connected = 0;
static int            g_led_status    = 1;
//static netmgr_hdl_t app_netmgr_hdl;

static void push_action(void)
{
    if (!g_iot_connected) {
        LOGD(TAG, "iot connect fail");
        return;
    }

    if (iot_channel_push(iot_ch_post, 1) != 0) {
        LOGE(TAG, "push err");
    }

    LOGI(TAG, "push action");
}

static void channel_set(uData *udata, void *arg)
{
    uData *node;
    slist_for_each_entry(&udata->head, node, uData, head)
    {
        if (node->value.updated) {
            if (strcmp(node->key.v_str, "led") == 0) {
                if (node->value.type == TYPE_INT) {
                    //LOGI(TAG, "change led:%x", node->value.v_int);
                    g_led_status = node->value.v_int;
                } else {
                    LOGE(TAG, "led type");
                }
            } else {
                if (node->value.type == TYPE_STR) {
                    LOGI(TAG, "ch set s (%s,%s)", node->key.v_str, node->value.v_str);
                } else {
                    LOGI(TAG, "ch set i (%s,%d)", node->key.v_str, node->value.v_int);
                }
            }
        }
    }
}

static void channel_get(uData *udata, void *arg)
{
    //LOGI(TAG, "channel_get\r\n");
    yoc_udata_set(udata, value_s("temp"), value_i(20), 1);
    yoc_udata_set(udata, value_s("humi"), value_i(20), 1);
    yoc_udata_set(udata, value_s("deviceName"), value_s(CONFIG_DEVICE_NAME), 1);
}

static void init_channel_udata(uData *udata)
{
    LOGI(TAG, "init_channel_udata\r\n");
    yoc_udata_set(udata, value_s("temp"), value_i(20), 1);
    yoc_udata_set(udata, value_s("humi"), value_i(20), 1);
    yoc_udata_set(udata, value_s("led"), value_i(1), 1);
    yoc_udata_set(udata, value_s("deviceName"), value_s(CONFIG_DEVICE_NAME), 1);
}

static int channel_open(void)
{
    /* one channel include two topic(publish&subscribe)
    publish data to topic /[product_key]/[device_name]/update
    subscribe topic /[product_key]/[device_name]/get */
    iot_ch_post =
        iot_channel_open(iot_handle, "thing/event/property/post,thing/service/property/set");
    if (iot_ch_post == NULL) {
        LOGE(TAG, "iot ch open");
        return -1;
    }
    iot_channel_config(iot_ch_post, channel_set, channel_get, NULL);

    /* init data node */
    init_channel_udata(iot_ch_post->uData);

    iot_channel_start(iot_ch_post);

    return 0;
}

void channel_close(void)
{
    if (iot_ch_post) {
        iot_channel_close(iot_ch_post);
        iot_ch_post = NULL;
    }
}

static void iot_event(uint32_t event_id, const void *param, void *context)
{
    if (iot_ch_post == NULL) {
        return;
    }

    switch (event_id) {
        case EVENT_IOT_CONNECT_SUCCESS:
            LOGD(TAG, "CONNNECT SUCCESS");
            g_iot_connected = 1;
            break;

        case EVENT_IOT_CONNECT_FAILED:
        case EVENT_IOT_DISCONNECTED:
            g_iot_connected = 0;
            channel_close();
            break;

        case EVENT_IOT_PUSH_SUCCESS:
            LOGD(TAG, "PUSH_SUCCESS");
            break;

        case EVENT_IOT_PUSH_FAILED:
            break;

        default:;
    }
}

int wifi_internet_is_connected()
{
    return (ntp_sync_flag == 1);
}

static void user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (event_id == EVENT_DATA_REPORT) {
        push_action();
        /* restart timer */
        //event_publish_delay(EVENT_DATA_REPORT, NULL, DATA_PUSH_INTERVAL);
    } else if (event_id == EVENT_NETMGR_GOT_IP) {
        if (g_iot_connected == 0) {
            channel_open();
        } else {
            LOGD(TAG, "channel already open");
        }

    } else if (event_id == EVENT_NETMGR_NET_DISCON) {
        channel_close();
        g_iot_connected = 0;
        //event_id = EVENT_IOT_CONNECT_FAILED;
    }
}

static netmgr_hdl_t wifi_network_init(char *ssid, char *psk)
{
    LOGI(TAG, "Start wifi network");
    LOGD(TAG, "SSID=%s PASS=%s", ssid, psk);

    utask_t *task = utask_new("netmgr", 4 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);

    netmgr_hdl_t hdl = netmgr_dev_wifi_init();
    netmgr_service_init(task);
    netmgr_config_wifi(hdl, ssid, strlen(ssid), psk, strlen(psk));
    netmgr_start(hdl);

    return hdl;
}

static wifi_mode_e app_net_init(void)
{
    /* system event */
    event_subscribe(EVENT_NETMGR_GOT_IP, user_local_event_cb, NULL);
    event_subscribe(EVENT_NETMGR_NET_DISCON, user_local_event_cb, NULL);

    event_subscribe(EVENT_DATA_REPORT, user_local_event_cb, NULL);
    event_subscribe(EVENT_CHANNEL_CHECK, user_local_event_cb, NULL);
    event_subscribe(EVENT_CHANNEL_CLOSE, user_local_event_cb, NULL);

    /* Subscribe IOT Event */
    event_subscribe(EVENT_IOT_CONNECT_SUCCESS, iot_event, NULL);
    event_subscribe(EVENT_IOT_CONNECT_FAILED, iot_event, NULL);
    event_subscribe(EVENT_IOT_DISCONNECTED, iot_event, NULL);
    event_subscribe(EVENT_IOT_PUSH_SUCCESS, iot_event, NULL);
    event_subscribe(EVENT_IOT_PUSH_FAILED, iot_event, NULL);
    /* start push Timer */
    event_publish_delay(EVENT_DATA_REPORT, NULL, DATA_PUSH_INTERVAL * 2);

    wifi_network_init(CONFIG_WIFI_SSID, CONFIG_WIFI_PSK);
    /* [product_key].iot-as-coap.cn-shanghai.aliyuncs.com:1883 */
    iot_alimqtt_config_t cfg = {.server_url_suffix = "iot-as-mqtt.cn-shanghai.aliyuncs.com:1883"};

    iot_handle = iot_new_alimqtt(&cfg);
    return MODE_WIFI_NORMAL;
}

wifi_mode_e app_network_init(void)
{
    /* init wifi driver and network */
    rtl8723ds_gpio_pin pin = {
        .wl_en = WLAN_ENABLE_PIN,
        .power = WLAN_POWER_PIN,
    };
    wifi_rtl8723ds_register(&pin);

    return app_net_init();
}

void sec_alimqtt_demo_main(uint32_t cmd)
{
#ifdef CONFIG_SECURITY_DEMO_SEC_ALIMQTT
    if (cmd == ALIMQTT_CMD_NET) {
        wifi_mode_e mode = app_network_init();
        if (mode != MODE_WIFI_TEST) {
            printf("===%s, %d, %x\n", __FUNCTION__, __LINE__, 0);
        }
    } else if (cmd == ALIMQTT_CMD_PUSH) {
        push_action();
    }

#endif
}
