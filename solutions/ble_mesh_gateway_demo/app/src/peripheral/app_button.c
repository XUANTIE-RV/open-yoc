/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */
#include <yoc/button.h>
#include "app_main.h"
#include "app_sys.h"
#include "board.h"

#define TAG "keyusr"

extern int wifi_prov_method;

typedef enum {
    nothing = 1,
    network,
} button_event_t;

void button_evt(int event_id, void *priv)
{
    LOGD(TAG, "button(%s)\n", (char *)priv);
    switch (event_id) {
		case network:
			LOGE(TAG, "ble prov");
#if defined(CONFIG_GW_NETWORK_SUPPORT) && CONFIG_GW_NETWORK_SUPPORT
			aos_kv_setint("wprov_method", wifi_prov_method);
			if (wifi_prov_method == WIFI_PROVISION_SL_BLE) {
				printf("wifi_prov_method=WIFI_PROVISION_SL_BLE\n");
				aos_kv_del("AUTH_AC_AS");
				aos_kv_del("AUTH_KEY_PAIRS");
			}
			app_sys_set_boot_reason(BOOT_REASON_WIFI_CONFIG);//zz
			aos_reboot();
#else
           LOGE(TAG,"Unsupport");
#endif
			break;
		case nothing:
			//TODO
			break;
		default:
			break;
    }
}

void app_button_init(void)
{
#if defined(BOARD_BUTTON_ADC_NUM) && BOARD_BUTTON_ADC_NUM > 0
    button_init();
    button_add_gpio(BOARD_BUTTON0, BOARD_BUTTON0_GPIO_PIN, LOW_LEVEL);//添加gpio按键

    button_evt_t b_tbl[] = {
        {
            .button_id  = BOARD_BUTTON0,
            .press_type   = BUTTON_PRESS_UP,//1
            .press_time = 0,
        }
    };
	button_add_event(nothing, b_tbl, sizeof(b_tbl)/sizeof(button_evt_t), button_evt, "nothing");//为当前按键表参数 添加事件
	
	b_tbl[0].button_id = BOARD_BUTTON0;
	b_tbl[0].press_type = BUTTON_PRESS_LONG_DOWN;//2
	b_tbl[0].press_time = 2000;
	button_add_event(network, b_tbl, sizeof(b_tbl)/sizeof(button_evt_t), button_evt, "network");//为当前按键表参数 添加事件
#endif
}
