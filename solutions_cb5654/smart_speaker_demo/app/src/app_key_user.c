/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc/button.h>
#include "app_main.h"
#include "app_lpm.h"

#define TAG "keyusr"
void button_evt(button_evt_id_t event_id, char *name, void *priv)
{
    LOGD(TAG, "%s evtid:%d", name, event_id);
    if (strcmp("inc", name) == 0) {
        if (event_id == BUTTON_PRESS_UP) {
            app_volume_inc(0);
        }
    } else if (strcmp("pause", name) == 0) {
        if (event_id == BUTTON_PRESS_UP) {
            app_player_reverse();
        } else if (event_id == BUTTON_PRESS_LONG_DOWN) {
            app_player_stop();
            wifi_pair_start();
        }
    } else if (strcmp("dec", name) == 0) {
        if (event_id == BUTTON_PRESS_UP) {
            app_volume_dec(0);
        }
    } else if (strcmp("id4", name) == 0) {
        #if defined(SIMULATED_POWER_KEY) && SIMULATED_POWER_KEY
        if (event_id == BUTTON_PRESS_LONG_DOWN) {
            LOGD(TAG, "power key shutdown");
            app_lpm_sleep(LPM_POLICY_DEEP_SLEEP, 1);
            aos_msleep(10000);
        }
        #endif
    } else if (strcmp("id5", name) == 0) {
        app_volume_mute();
    }
}

const static button_config_t button_table[] = {
    {PIN_ADC_KEY, PRESS_UP_FLAG, button_evt, NULL, BUTTON_TYPE_ADC,  "inc"},
    {PIN_ADC_KEY, EVT_ALL_FLAG,  button_evt, NULL, BUTTON_TYPE_ADC,  "pause"},
    {PIN_ADC_KEY, PRESS_UP_FLAG, button_evt, NULL, BUTTON_TYPE_ADC,  "dec"},
#if defined(BOARD_CB5654)
    {PIN_ADC_KEY, EVT_ALL_FLAG,  button_evt, NULL, BUTTON_TYPE_ADC,  "id4"},
    {PIN_ADC_KEY, PRESS_UP_FLAG, button_evt, NULL, BUTTON_TYPE_ADC,  "id5"},
#elif defined(PIN_ADC_KEY2) 
    {PIN_ADC_KEY,  PRESS_UP_FLAG, button_evt, NULL, BUTTON_TYPE_ADC,  "id6"},
    {PIN_ADC_KEY2, PRESS_UP_FLAG, button_evt, NULL, BUTTON_TYPE_ADC,  "id5"},
    {PIN_ADC_KEY2, EVT_ALL_FLAG,  button_evt, NULL, BUTTON_TYPE_ADC,  "id4"},
    {PIN_ADC_KEY2, PRESS_UP_FLAG, button_evt, NULL, BUTTON_TYPE_ADC,  "id7"},
    {PIN_ADC_KEY2, PRESS_UP_FLAG, button_evt, NULL, BUTTON_TYPE_ADC,  "id8"},
#endif
#if defined(PIN_TOUCH_PAD)
    {PIN_TOUCH_PAD, PRESS_UP_FLAG, button_evt, NULL, BUTTON_TYPE_GPIO,  "id9"},
#endif

    {0, 0, NULL, NULL},
};

void app_button_init(void)
{
    button_srv_init();
    button_init(button_table);

    button_param_t p;
    button_param_cur("inc", &p);
    p.adc_name  = "adc0";
    p.vref      = KEY_ADC_VAL1;
    p.range     = KEY_AD_VAL_OFFSET;
    button_param_set("inc", &p);

    p.vref      = KEY_ADC_VAL2;
    button_param_set("pause", &p);

    p.vref      = KEY_ADC_VAL3;
    button_param_set("dec", &p);

#if defined(BOARD_CB5654)
    p.vref      = KEY_ADC_VAL4;
    button_param_set("id4", &p);
    p.vref      = KEY_ADC_VAL5;
    button_param_set("id5", &p);
#elif defined(PIN_ADC_KEY2)
    p.vref      = KEY_ADC_VAL4;
    button_param_set("id6", &p);
    p.vref      = KEY_ADC_VAL5;
    button_param_set("id5", &p);
    p.vref      = KEY_ADC_VAL4;
    button_param_set("id4", &p);
    p.vref      = KEY_ADC_VAL5;
    button_param_set("id7", &p);
    p.vref      = KEY_ADC_VAL4;
    button_param_set("id8", &p);
#endif

#if defined(PIN_TOUCH_PAD)
    p.vref      = KEY_ADC_VAL4;
    button_param_set("id9", &p);
#endif

}