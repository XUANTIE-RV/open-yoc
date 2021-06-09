/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc/button.h>
#include "app_main.h"
#include "app_lpm.h"
#include <aos/hal/adc.h>
#include "app_sys.h"

#define TAG "keyusr"

extern int wifi_prov_method;

typedef enum {
    button_id0 = 0,
    button_id1,
    button_id2,
    button_id3,
    button_id4,
    button_id5,
    button_id6,
    button_id7,
    button_id8,
    button_id9,
} button_id_t;

typedef enum {
    inc = 0,
    pause,
    dec,
    network,
    id4,
    id5
} button_event_t;

void button_evt(int event_id, void *priv)
{
    LOGD(TAG, "button(%s)\n", (char *)priv);
    switch (event_id) {
    case inc:
        app_volume_inc(0);
        break;
    
    case pause:
        app_player_reverse();
        break;

    case network:
        app_player_stop();
        //wifi_pair_start();
        aos_kv_setint("wprov_method", wifi_prov_method);
        printf("method: %d\r\n", wifi_prov_method);
        app_sys_set_boot_reason(BOOT_REASON_WIFI_CONFIG);
        aos_reboot();
        break;
    case dec:
        app_volume_dec(0);
        break;

    case id4: 
        #if defined(SIMULATED_POWER_KEY) && SIMULATED_POWER_KEY
        if (event_id == BUTTON_PRESS_LONG_DOWN) {
            LOGD(TAG, "power key shutdown");
            app_lpm_sleep(LPM_POLICY_DEEP_SLEEP, 1);
            aos_msleep(10000);
        }
        #endif
        
        break;
    case id5:
        app_volume_mute();
        break;
    default:
        break;
    }
}

void app_button_init(void)
{
    button_init();

    button_add_adc(button_id0, "adc0", drv_adc_pin2channel(PIN_ADC_KEY), KEY_ADC_VAL1, KEY_AD_VAL_OFFSET);
    button_add_adc(button_id1, "adc0", drv_adc_pin2channel(PIN_ADC_KEY), KEY_ADC_VAL2, KEY_AD_VAL_OFFSET);
    button_add_adc(button_id2, "adc0", drv_adc_pin2channel(PIN_ADC_KEY), KEY_ADC_VAL3, KEY_AD_VAL_OFFSET);
#if defined(BOARD_CB5654)
    button_add_adc(button_id4, "adc0", drv_adc_pin2channel(PIN_ADC_KEY), KEY_ADC_VAL4, KEY_AD_VAL_OFFSET);
    button_add_adc(button_id5, "adc0", drv_adc_pin2channel(PIN_ADC_KEY), KEY_ADC_VAL5, KEY_AD_VAL_OFFSET);
#elif defined(PIN_ADC_KEY2) 
    button_add_adc(button_id6, "adc0", drv_adc_pin2channel(PIN_ADC_KEY), KEY_ADC_VAL4, KEY_AD_VAL_OFFSET);
    button_add_adc(button_id5, "adc0", drv_adc_pin2channel(PIN_ADC_KEY2), KEY_ADC_VAL5, KEY_AD_VAL_OFFSET);
    button_add_adc(button_id4, "adc0", drv_adc_pin2channel(PIN_ADC_KEY2), KEY_ADC_VAL6, KEY_AD_VAL_OFFSET);
    button_add_adc(button_id7, "adc0", drv_adc_pin2channel(PIN_ADC_KEY2), KEY_ADC_VAL7, KEY_AD_VAL_OFFSET);
    button_add_adc(button_id8, "adc0", drv_adc_pin2channel(PIN_ADC_KEY2), KEY_ADC_VAL8, KEY_AD_VAL_OFFSET);
#endif

#if defined(PIN_TOUCH_PAD)
    button_add_gpio(button_id9, PIN_TOUCH_PAD, LOW_LEVEL);
#endif

    button_evt_t b_tbl[] = {
        {
            .button_id  = button_id0,
            .event_id   = BUTTON_PRESS_UP,
            .press_time = 0,
        }
    };
    button_add_event(inc, b_tbl, sizeof(b_tbl)/sizeof(button_evt_t), button_evt, "inc");
    
    b_tbl[0].button_id = button_id1;
    button_add_event(pause, b_tbl, sizeof(b_tbl)/sizeof(button_evt_t), button_evt, "pause");

    b_tbl[0].event_id = BUTTON_PRESS_LONG_DOWN;
    b_tbl[0].press_time = 3000;
    button_add_event(network, b_tbl, sizeof(b_tbl)/sizeof(button_evt_t), button_evt, "network");
    
    b_tbl[0].button_id = button_id2;
    b_tbl[0].event_id = BUTTON_PRESS_UP;
    button_add_event(dec, b_tbl, sizeof(b_tbl)/sizeof(button_evt_t), button_evt, "dec");
    
    b_tbl[0].button_id = button_id2;
    b_tbl[0].event_id = BUTTON_PRESS_LONG_DOWN;
    b_tbl[0].press_time = 5000;
    button_add_event(dec, b_tbl, sizeof(b_tbl)/sizeof(button_evt_t), button_evt, "mute_long");

#if defined(BOARD_CB5654)
    b_tbl[0].event_id  = BUTTON_PRESS_UP;
    b_tbl[0].button_id = button_id5;
    button_add_event(id5, b_tbl, sizeof(b_tbl)/sizeof(button_evt_t), button_evt, "id5");
#elif defined(PIN_ADC_KEY2)
    b_tbl[0].event_id  = BUTTON_PRESS_UP;
    b_tbl[0].button_id = button_id5;
    button_add_event(id5, b_tbl, sizeof(b_tbl)/sizeof(button_evt_t), button_evt, "id5");
#endif
}