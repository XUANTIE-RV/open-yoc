/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifdef EN_COMBO_NET

#ifndef __COMBO_NET_H__
#define __COMBO_NET_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <wifi_provisioning.h>

#define COMBO_EVT_CODE_AP_INFO         0x0001
#define COMBO_EVT_CODE_RESTART_ADV     0x0002

int combo_net_init(wifi_prov_cb cb);
int combo_net_deinit(void);
uint8_t combo_ble_conn_state(void);

void combo_ap_conn_notify(void);
void combo_token_report_notify(void);

/* extern functions */
typedef enum {
    TOKEN_TYPE_NOT_CLOUD = 0x00,
    TOKEN_TYPE_CLOUD,
    TOKEN_TYPE_MAX,
    TOKEN_TYPE_INVALID = 0xFF
} bind_token_type_t;
#define RANDOM_MAX_LEN  (16)

int ieee80211_is_beacon(uint16_t fc);
int ieee80211_is_probe_resp(uint16_t fc);
int ieee80211_get_ssid(uint8_t *beacon_frame, uint16_t frame_len, uint8_t *ssid);
int aw_ieee80211_get_bssid(uint8_t *in, uint8_t *mac);
int awss_set_token(uint8_t token[RANDOM_MAX_LEN], bind_token_type_t token_type);
uint8_t aws_next_channel(void);

#endif
#endif