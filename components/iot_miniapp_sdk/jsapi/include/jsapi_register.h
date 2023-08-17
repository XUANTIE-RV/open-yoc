/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef _AIOT_JSAPI_H_
#define _AIOT_JSAPI_H_

#ifdef __cplusplus
extern "C" {
#endif

void register_aiot_wifi_jsapi();
void register_aiot_http_jsapi();
void register_aiot_power_jsapi();
void register_aiot_system_kv_jsapi();
void register_aiot_screen_jsapi();
void register_aiot_voice_jsapi();
void register_aiot_smta_jsapi();
void register_aiot_fota_jsapi();
void register_aiot_gateway_jsapi();

#ifdef __cplusplus
}
#endif

#endif  //_AIOT_JSAPI_H_
