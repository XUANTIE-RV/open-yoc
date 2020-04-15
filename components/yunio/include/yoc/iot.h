/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef YOC_IOT_H
#define YOC_IOT_H

#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <aos/kernel.h>
#include <yoc/eventid.h>
#include <yoc/uservice.h>

#include <yoc/udata.h>

typedef struct _iot_channel iot_channel_t;
typedef struct _iot         iot_t;

typedef void (*channel_set_t)(uData *udata, void *arg);
typedef void (*channel_get_t)(uData *udata, void *arg);
typedef void (*iot_event_t)(iot_t *ch, int iot_event_id, void *arg);

struct _iot {
    int (*channel_open)(iot_channel_t *ch);
    int (*channel_start)(iot_channel_t *ch);
    int (*channel_send)(iot_channel_t *ch);
    int (*channel_raw_send)(iot_channel_t *ch, void *payload, int len);
    int (*channel_recv)(iot_channel_t *ch);
    void (*channel_close)(iot_channel_t *ch);
    int (*destroy)(iot_t *iot);
    void *user_cfg;
};

struct _iot_channel {
    slist_t node;

    iot_t      *iot;
    uData      *uData;
    const char *ch_open_arg; /* channel open arg */

    channel_set_t ch_set; /* recv data from cloud, do user action */
    channel_get_t ch_get; /* before push data to cloud, user update uData from hardware */

    void       *ch_cfg_arg; /* iot_channel_config arg */
    void       *priv;
    aos_mutex_t ch_mutex;
};

typedef struct iot_alicoap_config {
    const char *server_url_suffix;
} iot_alicoap_config_t;

typedef struct iot_alimqtt_config {
    const char *server_url_suffix;
} iot_alimqtt_config_t;

typedef struct iot_onenet_config {
    int32_t lifetime; /* unit second */
} iot_onenet_config_t;

typedef struct iot_oceancon_config {
    char server_addr[30];
    int  lifetime;
    int (*udata_to_buffer)(uData *data, void *buffer);
    int (*buffer_to_udata)(uData *data, void *buffer, int len);
    int buffer_size;
} iot_oceancon_config_t;

iot_t *iot_new_oceancon(iot_oceancon_config_t *config);
iot_t *iot_new_alicoap(iot_alicoap_config_t *config);
iot_t *iot_new_alimqtt(iot_alimqtt_config_t *config);
iot_t *iot_new_onenet(iot_onenet_config_t *config);
int iot_destroy(iot_t *iot);

iot_channel_t *iot_channel_open(iot_t *iot, const char *ch_arg);
int iot_channel_start(iot_channel_t *ch);
int iot_channel_close(iot_channel_t *ch);
int iot_channel_config(iot_channel_t *ch, channel_set_t set, channel_get_t get, void *arg);
int iot_channel_push(iot_channel_t *ch, uint8_t sync);
int iot_channel_raw_push(iot_channel_t *ch, void *payload, int len);

#endif
