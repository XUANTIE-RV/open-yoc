#ifndef __WIFI_MGMR_PROFILE_H__
#define __WIFI_MGMR_PROFILE_H__
#include "wifi_mgmr.h"

int wifi_mgmr_profile_add(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg, int index);
int wifi_mgmr_profile_del(wifi_mgmr_t *mgmr, char *ssid, int len);
int wifi_mgmr_profile_get(wifi_mgmr_t *mgmr, wifi_mgmr_profile_msg_t *profile_msg);
int wifi_mgmr_profile_autoreconnect_is_enabled(wifi_mgmr_t *mgmr, int index);
int wifi_mgmr_profile_autoreconnect_disable(wifi_mgmr_t *mgmr, int index);
int wifi_mgmr_profile_autoreconnect_enable(wifi_mgmr_t *mgmr, int index);
#endif
