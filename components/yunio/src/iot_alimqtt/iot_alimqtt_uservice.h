/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __ALI_MQTT_INTERNAL_H__
#define __ALI_MQTT_INTERNAL_H__

typedef int (*subscribe_cb_t)(const char *topic, void *payload, int len, void *arg);

int alimqtt_usrv_init(void);
int alimqtt_usrv_connect(void);
int alimqtt_usrv_yield(void);
int alimqtt_usrv_send(const char *topic, void *payload, int len);
int alimqtt_usrv_subscribe(const char *topic, subscribe_cb_t cb, void *arg);
int alimqtt_usrv_disconnect(void);
void alimqtt_usrv_deinit(void);

#endif
