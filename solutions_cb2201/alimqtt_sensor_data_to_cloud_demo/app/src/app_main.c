/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */


#include <stdlib.h>
#include <string.h>

#include <aos/log.h>
#include <aos/list.h>
#include <aos/debug.h>

#include <yoc/uservice.h>
#include <yoc/eventid.h>
#include <yoc/iot.h>
#include <yoc/sysinfo.h>
#include <yoc/udata.h>
#include <pin.h>
#include <devices/led.h>
#include <devices/device.h>
#include <devices/devicelist.h>
#include <devices/hal/sensor_impl.h>
#include <devices/drv_sensor.h>
#include <devices/led_rgb.h>

#define SENSOR_NAME "simulate"
//#define SENSOR_NAME DHT11_DEV_NAME

#include "app_main.h"

#define DATA_PUSH_INTERVAL (5 * 1000)

#define LED_ON  1
#define LED_OFF 0
#define DEVICE_LED_NAME     "ledrgb"
static led_pin_config_t led_config = {EXAMPLE_LED_R_PIN, EXAMPLE_LED_G_PIN, EXAMPLE_LED_B_PIN, 1};
static const char *   TAG         = "app";
static iot_t *        iot_handle  = NULL;
static iot_channel_t *iot_ch_post = NULL;
static int  g_iot_connected = 0;
static int  g_led_status = 1;


static void push_action(void)
{
    if (!netmgr_is_gotip(app_netmgr_hdl)) {
        LOGD(TAG, "net down");
        return;
    }

    if (!g_iot_connected) {
        LOGD(TAG, "iot connect fail");
        return;
    }

    if (iot_channel_push(iot_ch_post, 1) != 0) {
        LOGE(TAG, "push err");
    }

    LOGI(TAG, "push action");
}

static void led_set_status(int status)
{
    static aos_dev_t *led_dev = NULL;

    if (led_dev == NULL) {
        led_dev = led_open_id(DEVICE_LED_NAME, 1);
    }

    if (led_dev == NULL) {
        LOGE(TAG, "led_dev open err!");
    }

    if (status == LED_ON) {
        led_control(led_dev, COLOR_WHITE, -1, -1);
    } else if (status == LED_OFF) {
        led_control(led_dev, COLOR_BLACK, -1, -1);
    }
    //led_close(led_dev);
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
                    led_set_status(g_led_status);
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
    int temp = 0, humi = 0;
	get_sensor_value(SENSOR_NAME, &temp, &humi);
    //LOGI(TAG, "channel_get\r\n");
    yoc_udata_set(udata, value_s("temp"), value_i(temp), 1);
    yoc_udata_set(udata, value_s("humi"), value_i(humi), 1);
    yoc_udata_set(udata, value_s("led"), value_i(g_led_status), 1);
    yoc_udata_set(udata, value_s("deviceName"), value_s(aos_get_device_name()), 1);
}

static void init_channel_udata(uData *udata)
{
    //LOGI(TAG, "init_channel_udata\r\n");
    yoc_udata_set(udata, value_s("temp"), value_i(0), 1);
    yoc_udata_set(udata, value_s("humi"), value_i(0), 1);
    yoc_udata_set(udata, value_s("led"), value_i(g_led_status), 1);
    yoc_udata_set(udata, value_s("deviceName"), value_s(aos_get_device_name()), 1);
}

static int channel_open(void)
{
    /* one channel include two topic(publish&subscribe)
    publish data to topic /[product_key]/[device_name]/update
    subscribe topic /[product_key]/[device_name]/get */
    iot_ch_post = iot_channel_open(iot_handle, "thing/event/property/post,thing/service/property/set");
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

    /*do exception process */
    app_exception_event(event_id);
}

static void network_event(uint32_t event_id, const void *param, void *context)
{
    switch(event_id) {
    case EVENT_NETMGR_GOT_IP:
        LOGD(TAG, "EVENT_NETMGR_GOT_IP");
        if (g_iot_connected == 0) {
            channel_open();
        }
        break;
    case EVENT_NETMGR_NET_DISCON:
        channel_close();
        g_iot_connected = 0;
        break;
    }

    /*do exception process */
    app_exception_event(event_id);
}

static void user_local_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (event_id == EVENT_DATA_REPORT){
        push_action();
        /* restart timer */
        event_publish_delay(EVENT_DATA_REPORT, NULL, DATA_PUSH_INTERVAL);
    } else if (event_id == EVENT_CHANNEL_CHECK) {
        if (g_iot_connected == 0) {
            int res = -1;

            if (netmgr_is_gotip(app_netmgr_hdl)) {
                res = channel_open();
            } else {
                LOGW(TAG, "net not ready");
            }

            /* error before do connnect, trans to EVENT_IOT_CONNECT_FAILED to retry */
            if (res != 0) {
                event_id = EVENT_IOT_CONNECT_FAILED;
            }
        } else {
            LOGD(TAG, "channel already open");
        }

    } else if (event_id == EVENT_CHANNEL_CLOSE) {
        channel_close();
        g_iot_connected = 0;
        event_id = EVENT_IOT_CONNECT_FAILED;
    }

    /*do exception process */
    app_exception_event(event_id);
}

void main()
{
    board_yoc_init();

    /* 模拟传感 */
    sensor_simulate_register(NULL, 0);

    /* DHT11 传感驱动 */
    //static sensor_pin_config_t dht11_config = {PA4};
    //sensor_dht11_register(&dht11_config, 0);  

    /* init the led for system status notify */
    led_rgb_register(&led_config, 1);

    led_set_status(LED_ON);
    /* Subscribe */
    event_subscribe(EVENT_NETMGR_GOT_IP, network_event, NULL);
    event_subscribe(EVENT_NETMGR_NET_DISCON, network_event, NULL);

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

    /* [product_key].iot-as-coap.cn-shanghai.aliyuncs.com:1883 */
    iot_alimqtt_config_t cfg = {.server_url_suffix =
                                    "iot-as-mqtt.cn-shanghai.aliyuncs.com:1883"};

    iot_handle = iot_new_alimqtt(&cfg);
}
