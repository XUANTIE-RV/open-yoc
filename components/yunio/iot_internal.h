/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __IOT_INTERNAL_H__
#define __IOT_INTERNAL_H__

#include <aos/list.h>
#include <aos/kernel.h>
#include <aos/log.h>

#include <yoc/udata.h>

typedef struct _iot_channel iot_channel_t;

typedef struct _iot {
    slist_t       udata_list;
    void         *user_cfg;
    void         *ch_cfg_arg; /* iot_channel_config arg */
    iot_ops_t     ops;
    iot_config_t *config; /* iot_channel_config arg */
    aos_mutex_t   ch_mutex;
    void         *priv;
} iot_t;

typedef struct _iot_ops {
    const char *name;
    int (*open)(iot_channel_t *ch);
    int (*start)(iot_channel_t *ch);
    int (*send)(iot_channel_t *ch);
    int (*raw_send)(iot_channel_t *ch, void *payload, int len);
    int (*recv)(iot_channel_t *ch);
    void (*close)(iot_channel_t *ch);
    int (*destroy)(iot_t *iot);
    slist_t udata_list;
} iot_ops_t;

typedef struct iot_config {
    void *data;
    void (*on_update)(iot_t *iot, uData *udata); /* recv data from cloud, do user action */
    void (*on_read)(iot_t *iot, uData *udata); /* before push data to cloud, user update uData from hardware */
    void (*on_pushed)(iot_t *iot, uData *udata);
    void (*on_push_fail)(iot_t *iot, uData *udata);
    void (*on_connected)(iot_t *iot);
    void (*on_connnect_fail)(iot_t *iot);
    void (*on_disconnect)(iot_t *iot);
} iot_config_t;

struct data {
    slist_t       node;
    iot_t        *iot;
    uData        *uData;
    const char   *ch_open_arg; /* channel open arg */
    aos_mutex_t   ch_mutex;
    void         *priv;
} udata_t;

iot_t *iot_new_oceancon(void *config);
iot_t *iot_new_alicoap(void *config);
iot_t *iot_new_alimqtt(void *config);
iot_t *iot_new_onenet(void *config);

void iot_register(iot_ops_t *ops);

int iot_destroy(iot_t *iot);

int iot_create(iot_t *iot, const char *name, iot_config_t *config);

int iot_start(iot_t *iot);
int iot_stop(iot_t *iot);
int iot_quit(iot_t *iot);

uData *iot_udata_new(iot_t *iot, const char *ch_arg);

int iot_push(uData *data, uint8_t sync);


#if 0

static void iot_user_set(iot_t *iot, uData *udata)
{
    uData *node;
    slist_for_each_entry(&udata->head, node, uData, head) {
        if (node->value.updated) {
            if (node->value.type == TYPE_STR) {
                LOGI(TAG, "ch set (%s,%s)", node->key.v_str, node->value.v_str);
            }
            else {
                LOGI(TAG, "ch set (%s,%d)", node->key.v_str, node->value.v_int);
            }
        }
    }
}

static void iot_user_get(iot_t *iot, uData *udata)
{
    yoc_udata_set(udata, value_s("smoke"), value_i(10), 1);

    uData *node;
    slist_for_each_entry(&udata->head, node, uData, head) {
        if (node->value.updated) {
            yoc_udata_set(udata, value_s("smoke"), value_i(10), 1);
        }
    }
}

static void iot_user_on_connected(iot_t *iot) {

}

static void iot_user_on_connnect_fail(iot_t *iot) {
    iot_start(iot);
}

static void iot_user_on_disconnect(iot_t *iot) {
    iot_start(iot);
    iot_quit(iot);
}

static void iot_user_on_pushed(iot_t *iot, uData *udata) {
    udate_reset(iot, udata);

}

static void iot_user_on_push_fail(iot_t *iot, uData *udata) {
    iot_push(iot, udata);
}

iot_config_t config = {
    .on_update = iot_user_set,
    .on_read = iot_user_get,
    .on_pushed = iot_user_on_pushed,
    .on_push_fail = iot_user_on_push_fail,

    .on_connected = iot_user_on_connected,
    .on_connnect_fail = iot_user_on_connnect_fail,
    .on_disconnect = iot_user_on_disconnect,
};

iot_register(&alimqtt_ops);
iot_register(&alicoap_ops);

iot_t iot;

iot_create(&iot, ALIMQTT, &config);

uData *udata1 = iot_udata_new(&iot, "thing/event/property/post,thing/service/property/set");
iot_udata_add(&iot, udata1);

uData *udata2 = iot_udata_new(&iot, "thing/event/property/post,thing/service/property/set");
iot_udata_add(&iot, udata2);
iot_udata_remove(&iot, udata2);

iot_start(&iot);

while (1) {
    udata_set(udata1, value_s("temp"), value_i(0), 1);
    udata_set(udata1, value_s("humi"), value_i(0), 1);
    udata_set(udata1, value_s("led"), value_i(g_led_status), 1);
    udata_set(udata1, value_s("deviceName"), value_s(aos_get_device_name()), 1);
    iot_push(&iot, udata1);

    udata_set(udata2, value_s("temp"), value_i(0), 1);
    udata_set(udata2, value_s("humi"), value_i(0), 1);
    udata_set(udata2, value_s("led"), value_i(g_led_status), 1);
    udata_set(udata2, value_s("deviceName"), value_s(aos_get_device_name()), 1);
    iot_push(&iot, udata2);

    sleep(1);
}

#endif


#endif
