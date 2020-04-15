/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <devices/led_rgb.h>

#include "app_main.h"

#define TAG "app"

#define DEVICE_LED_NAME     "ledrgb"

static led_pin_config_t led_config = {PIN_LED_R, PIN_LED_G, PIN_LED_B, LED_FLIP_FLAG};

static aos_dev_t *led_dev = NULL;
static int g_app_led_en = 1;

void app_set_led_enabled(int en)
{
    g_app_led_en = en;
}

void app_set_led_state(int state)
{
    if (g_app_led_en == 0) {
        if (led_dev) {
            led_close(led_dev);
            led_dev = NULL;
        }
        return;
    }

    if (led_dev == NULL) {    
        led_rgb_register(&led_config, 1);

        if (led_dev == NULL) {
            led_dev = led_open_id(DEVICE_LED_NAME, 1);
        }

        if (led_dev == NULL) {
            LOGE(TAG, "led_dev open err!");
            return;
        }
    }

    switch (state)
    {
        case LED_LOADING:
            led_control(led_dev, COLOR_WHITE, 0, 0);
            break;
        case LED_NET_DOWN:
            led_control(led_dev, COLOR_WHITE, 50, 500);
            break;
        case LED_NET_READY:
            led_control(led_dev, COLOR_WHITE, 50, 3000);
            break;
        case LED_PLAYING:
            led_control(led_dev, COLOR_WHITE, 50, 1000);
            break;
        case LED_NET_CONFIG:
            led_control(led_dev, COLOR_WHITE, 50, 100);
            break;
        case LED_TALKING:
            led_control(led_dev, COLOR_WHITE, 0, 0);
            break;
        default:
            led_control(led_dev, COLOR_BLACK, 0, 0);
            break;
    }

    /* LED lights are working all the time,no need to close.*/
    //led_close(led_dev);
}

static void app_status_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (event_id == EVENT_UPDATE_STATUS) {
        //LOGD(TAG, ">>>  EVENT_UPDATE_STATUS");

        //网络状态
        if (wifi_is_pairing()) {
            app_set_led_state(LED_NET_CONFIG);
        } else if (wifi_internet_is_connected()) {
            app_set_led_state(LED_NET_READY);
        } else {
            app_set_led_state(LED_NET_DOWN);
        }

        //播放状态
        if (app_player_get_status()) {
            app_set_led_state(LED_PLAYING);
        }

        //MIC状态
        if (app_mic_is_busy()) {
            app_set_led_state(LED_TALKING);
        }
    }
}

void app_status_init()
{
    /* 事件订阅 */
    event_subscribe(EVENT_UPDATE_STATUS, app_status_event_cb, NULL);
}

void app_status_update(void)
{
    //event_publish(EVENT_UPDATE_STATUS, NULL);
    event_publish_delay(EVENT_UPDATE_STATUS, NULL, 50);
}
