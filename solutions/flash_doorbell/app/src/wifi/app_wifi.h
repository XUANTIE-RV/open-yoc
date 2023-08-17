/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef _APP_WIFI_INIT_
#define _APP_WIFI_INIT_

#include "wifi/wifi_prov/wifi_prov.h"
#include "wifi/multi_ssid/wifi_config.h"

typedef enum {
    MODE_WIFI_TEST    = -2,
    MODE_WIFI_CLOSE   = -1,
    MODE_WIFI_NORMAL  = 0,
    MODE_WIFI_PAIRING = 1
} wifi_mode_e;

/**
 * wifi driver init
 *
 * @return 0: success <0: fail
 */
int app_wifi_driver_init(void);

/**
 * set wifi low power
 *
 * @param [in] lpm_on 1:lpm on, 0:lpm off
 */
void app_wifi_set_lpm(int lpm_on);

/**
 * Scan signal to obtain the best connection hotspot
 * 
 * @return void
 */
void app_wifi_network_init_list();

/**
 * get wifi mac address
 * 
 * @param [out] mac address buffer
 * 
 * @return 0: success
 */
int app_wifi_getmac(uint8_t mac[6]);

/**
 * get BT mac address
 *
 * @param [out] mac address buffer
 *
 * @return 0: success
 */
int app_bt_getmac(char *mac);

/**
 * Scan signal to obtain the best connection hotspot
 * 
 * @return void
 */
void app_wifi_network_init_list();

#endif
