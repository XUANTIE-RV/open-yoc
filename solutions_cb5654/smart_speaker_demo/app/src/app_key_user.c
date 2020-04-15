/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc/adc_key_srv.h>
#include "app_main.h"
#include "app_lpm.h"

#define TAG "keyusr"

#define KEY_ID1 (1<<0)
#define KEY_ID2 (1<<1)
#define KEY_ID3 (1<<2)
#define KEY_ID4 (1<<3)
#define KEY_ID5 (1<<4)
#define KEY_ID6 (1<<5)
#define KEY_ID7 (1<<6)
#define KEY_ID8 (1<<7)
#define KEY_ID9 (1<<8)
#define KEY_ID_DBG (1<<5)

static int key_event_process(key_event_type_t event_type, int key_id, int rt_value)
{
    LOGD(TAG, "event_type=%d key_id=%x value=%d", event_type, key_id, rt_value);

    /* 状态可能会和按键同时发生 */
    if (event_type == KEY_SWITCH_CHANGE) {
    
    }

    /* 按键处理 */
    if (event_type == KEY_COMBO_CLICK) {
        /* 组合键处理 */
    } else {
        /* 单键处理 */
        switch (key_id) {
            case KEY_ID1:
                if (event_type == KEY_S_CLICK) {
                    app_volume_inc(0);
                }
                break;
            case KEY_ID2:
                if (event_type == KEY_S_CLICK) {
                    app_player_reverse();
                } else if (event_type == KEY_LONG_PRESS) {
                    app_player_stop();
                    wifi_pair_start();
                }
                break;
            case KEY_ID3:
                if (event_type == KEY_S_CLICK) {
                    app_volume_dec(0);
                }
                break;
            case KEY_ID4:
#if defined(SIMULATED_POWER_KEY) && SIMULATED_POWER_KEY
                if (event_type == KEY_LONG_PRESS) {
                    LOGD(TAG, "power key shutdown");
                    app_lpm_sleep(LPM_POLICY_DEEP_SLEEP, 1);
                    aos_msleep(10000);
                }
#endif
                break;
            case KEY_ID5:
                app_volume_mute();
                break;
            default:
                ;
        }
    }

    return 0;
}

static void key_scan_cb(void)
{
#ifdef CONFIG_BATTERY_ADC
    battery_update_avg();

    app_charger_check();
#endif
}

void app_button_init(void)
{
    key_srv_init(key_event_process);

    key_srv_config(0, 0, 0, KEY_AD_VAL_OFFSET);
    key_srv_add_keyid(PIN_ADC_KEY, KEY_TYPE_ADC, KEY_ADC_VAL1, KEY_ID1);
    key_srv_add_keyid(PIN_ADC_KEY, KEY_TYPE_ADC, KEY_ADC_VAL2, KEY_ID2);
    key_srv_add_keyid(PIN_ADC_KEY, KEY_TYPE_ADC, KEY_ADC_VAL3, KEY_ID3);

#if defined(BOARD_CB5654)
    key_srv_add_keyid(PIN_ADC_KEY, KEY_TYPE_ADC, KEY_ADC_VAL4, KEY_ID4);
    key_srv_add_keyid(PIN_ADC_KEY, KEY_TYPE_ADC, KEY_ADC_VAL5, KEY_ID5);
#elif defined(PIN_ADC_KEY2) 
    key_srv_add_keyid(PIN_ADC_KEY, KEY_TYPE_ADC, KEY_ADC_VAL4, KEY_ID6);
    key_srv_add_keyid(PIN_ADC_KEY2, KEY_TYPE_ADC, KEY_ADC_VAL5, KEY_ID5);
    key_srv_add_keyid(PIN_ADC_KEY2, KEY_TYPE_ADC, KEY_ADC_VAL6, KEY_ID4);
    key_srv_add_keyid(PIN_ADC_KEY2, KEY_TYPE_ADC, KEY_ADC_VAL7, KEY_ID7);
    key_srv_add_keyid(PIN_ADC_KEY2, KEY_TYPE_ADC, KEY_ADC_VAL8, KEY_ID8);
#endif

#if defined(PIN_TOUCH_PAD)
    key_srv_add_keyid(PIN_TOUCH_PAD, KEY_TYPE_GPIO, 0, KEY_ID9);
#endif

    extern int key_srv_scan_register(key_scan_cb_t cb);
    key_srv_scan_register(key_scan_cb);

    key_srv_start();
}

