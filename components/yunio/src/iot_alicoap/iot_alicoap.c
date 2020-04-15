/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/log.h>
#include <aos/list.h>
#include <aos/debug.h>
#include <yoc/nvram.h>
#include <yoc/udata.h>

#include <yoc/iot.h>
#include <yoc/uservice.h>
#include <yoc/eventid.h>
#include "iot_alicoap_uservice.h"

#define TAG "alicoap"

static iot_t *        g_iot_alicoap       = NULL;
static iot_channel_t *iot_alicoap_channel = NULL;

static int  alicoap_destroy(iot_t *iot);
static int  alicoap_channel_open(iot_channel_t *ch);
static void alicoap_channel_close(iot_channel_t *ch);
static int  alicoap_channel_start(iot_channel_t *ch);
static int  alicoap_channel_send(iot_channel_t *ch);
static int alicoap_channel_raw_send(iot_channel_t *ch, void *payload, int len);
static int  alicoap_channel_recv(iot_channel_t *ch);

iot_t *iot_new_alicoap(iot_alicoap_config_t *config)
{
    if (g_iot_alicoap != NULL) {
        return NULL;
    }

    // usrv task init
    alicoap_usrv_init(config->server_url_suffix);

    g_iot_alicoap = aos_malloc(sizeof(iot_t));
    if (g_iot_alicoap == NULL) {
        return NULL;
    }

    g_iot_alicoap->user_cfg = aos_malloc(sizeof(iot_alicoap_config_t));

    if (g_iot_alicoap->user_cfg == NULL) {
        aos_free(g_iot_alicoap);
        return NULL;
    }

    memcpy(g_iot_alicoap->user_cfg, config, sizeof(iot_alicoap_config_t));

    g_iot_alicoap->channel_open  = alicoap_channel_open;
    g_iot_alicoap->channel_start = alicoap_channel_start;
    g_iot_alicoap->channel_send  = alicoap_channel_send;
    g_iot_alicoap->channel_raw_send  = alicoap_channel_raw_send;
    g_iot_alicoap->channel_recv  = alicoap_channel_recv;
    g_iot_alicoap->channel_close = alicoap_channel_close;
    g_iot_alicoap->destroy       = alicoap_destroy;

    return g_iot_alicoap;
}

static int alicoap_destroy(iot_t *iot)
{
    if (g_iot_alicoap != iot) {
        return -EBADFD;
    }

    if (iot == NULL) {
        return -EINVAL;
    }

    if (iot->user_cfg) {
        aos_free(iot->user_cfg);
    }

    alicoap_usrv_deinit();
    aos_free(g_iot_alicoap);
    g_iot_alicoap = NULL;

    return 0;
}

static int alicoap_channel_open(iot_channel_t *ch)
{
    if (iot_alicoap_channel != NULL) {
        return -1;
    }

    iot_alicoap_channel = ch;
    return 0;
}

static void alicoap_channel_close(iot_channel_t *ch)
{
    alicoap_usrv_disconnect();
    iot_alicoap_channel = NULL;
    return;
}

static int alicoap_channel_start(iot_channel_t *ch)
{
    int ret;
    if (ch->uData == NULL) {
        return -EINVAL;
    }

    ret = alicoap_usrv_connect();
    if (ret < 0) {
        return -1;
    }

    return 0;
}

static int alicoap_channel_send(iot_channel_t *ch)
{
    int  ret = 0;
    char str[128]; // = "{\"type\":1,\"val\":22}";


    aos_mutex_lock(&ch->ch_mutex, AOS_WAIT_FOREVER);

    ret = yoc_udata_to_json(ch->uData, str, sizeof(str), 1);

    /* clear key update flag */
    yoc_udata_clear_flag_all(ch->uData);

    aos_mutex_unlock(&ch->ch_mutex);

    if (ret > 0) {
        LOGD("IOT", "->%s", str);
        ret = alicoap_usrv_send((char *)ch->ch_open_arg, str, ret);
    } else {
        LOGD("IOT", "nothing to send");
    }

    return ret;
}

static int alicoap_channel_raw_send(iot_channel_t *ch, void *payload, int len)
{
    int ret;

    if (payload == NULL || len <= 0) {
        return -EINVAL;
    }

    ret = alicoap_usrv_send((char *)ch->ch_open_arg, payload, len);

    return ret >= 0 ? 0 : -1;
}


static int alicoap_channel_recv(iot_channel_t *ch)
{
    //TODO: cloud data convert to uData
    return 0;
}
