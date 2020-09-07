/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __ALI_COAP_INTERNAL_H__
#define __ALI_COAP_INTERNAL_H__

typedef int (*subscribe_cb_t)(const char *topic, void *payload, int len, void *arg);

int alicoap_usrv_init(const char *server_url_suffix);
int alicoap_usrv_connect(void);
int alicoap_usrv_yield(void);
int alicoap_usrv_send(const char *topic, void *payload, int len);
//int alicoap_usrv_subscribe(const char *topic, subscribe_cb_t cb, void *arg);
int alicoap_usrv_disconnect(void);
void alicoap_usrv_deinit(void);

#endif
