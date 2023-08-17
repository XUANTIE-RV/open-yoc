/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */

#ifndef __WIFI_PROV__
#define __WIFI_PROV__
#include <stdint.h>

#define WIFI_PROV_TIMEOUT         180 /*秒*/
#define WIFI_STA_CONNNECT_TIMEOUT 60  /*秒*/

#define WIFI_PROVISION_SOFTAP      1
#define WIFI_PROVISION_SOFTAP_BLE  2 /* 预留 */

typedef struct {
    char    ssid[33];
    char    password[65];
    uint8_t bssid[6];
    uint8_t channel;
    uint8_t auth_mode;
    char    auth[64];
} wifi_prov_res_t;

/**
 * set wifi prov type
 * 
 * @param type wifi prov type,  WIFI_PROVISION_XXX
 * @return void
 */
void wifi_pair_set_prov_type(int type);

/**
 * get et wifi prov type
 *
 * @return wifi prov type,  WIFI_PROVISION_XXX
 */
int  wifi_pair_get_prov_type();

/**
 * start wifi pair
 */
void wifi_pair_start(void);

/**
 * stop wifi pair
 */
void wifi_pair_stop(void);

/**
 * get wifi pairing status
 *
 * @return 1: pairing, 0: not pairing
 */
int wifi_is_pairing();

#endif
