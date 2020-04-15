/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <aos/log.h>
#include <aos/list.h>
#include <aos/debug.h>
#include <yoc/nvram.h>
#include <yoc/uservice.h>
#include <yoc/udata.h>

#include <yoc/iot.h>
#include "iot_alimqtt_uservice.h"
#define TAG "alimqtt"

static iot_t *        g_iot_alimqtt       = NULL;
static iot_channel_t *iot_alimqtt_channel = NULL;
static char g_pub_topic[64];
static char g_sub_topic[64];

static int  alimqtt_destroy(iot_t *iot);
static int  alimqtt_channel_open(iot_channel_t *ch);
static void alimqtt_channel_close(iot_channel_t *ch);
static int  alimqtt_channel_start(iot_channel_t *ch);
static int  alimqtt_channel_send(iot_channel_t *ch);
static int  alimqtt_channel_raw_send(iot_channel_t *ch, void *payload, int len);
static int  alimqtt_channel_recv(iot_channel_t *ch);

static int alimqtt_yio_recv(const char *topic, void *payload, int len, void *arg)
{
    iot_channel_t *ch = (iot_channel_t *)arg;

    LOGD(TAG, "recv %d", len);

    if (ch == NULL)
        return -1;

    aos_mutex_lock(&ch->ch_mutex, AOS_WAIT_FOREVER);

    /* update udata form cloud */
    yoc_udata_from_json(ch->uData, (char *)payload);

    /* user update to hardware */
    if (ch->ch_set) {
        ch->ch_set(ch->uData, ch->ch_cfg_arg);
    }

    /* clear update flag */
    yoc_udata_clear_flag_all(ch->uData);

    aos_mutex_unlock(&ch->ch_mutex);

    return 0;
}


iot_t *iot_new_alimqtt(iot_alimqtt_config_t *config)
{
    if (g_iot_alimqtt != NULL) {
        return NULL;
    }

    // usrv task init
    alimqtt_usrv_init();

    g_iot_alimqtt = aos_malloc(sizeof(iot_t));
    if (g_iot_alimqtt == NULL) {
        return NULL;
    }

    g_iot_alimqtt->user_cfg = aos_malloc(sizeof(iot_alimqtt_config_t));

    if (g_iot_alimqtt->user_cfg == NULL) {
        aos_free(g_iot_alimqtt);
        return NULL;
    }

    memcpy(g_iot_alimqtt->user_cfg, config, sizeof(iot_alimqtt_config_t));

    g_iot_alimqtt->channel_open  = alimqtt_channel_open;
    g_iot_alimqtt->channel_start = alimqtt_channel_start;
    g_iot_alimqtt->channel_send  = alimqtt_channel_send;
    g_iot_alimqtt->channel_raw_send  = alimqtt_channel_raw_send;
    g_iot_alimqtt->channel_recv  = alimqtt_channel_recv;
    g_iot_alimqtt->channel_close = alimqtt_channel_close;
    g_iot_alimqtt->destroy       = alimqtt_destroy;

    return g_iot_alimqtt;
}

static int alimqtt_destroy(iot_t *iot)
{
    if (g_iot_alimqtt != iot) {
        return -EBADFD;
    }

    if (iot == NULL) {
        return -EINVAL;
    }

    if (iot->user_cfg) {
        aos_free(iot->user_cfg);
    }

    alimqtt_usrv_deinit();
    aos_free(g_iot_alimqtt);
    g_iot_alimqtt = NULL;

    return 0;
}

static int alimqtt_channel_open(iot_channel_t *ch)
{
    char *ptr;

    if (iot_alimqtt_channel != NULL) {
        return -1;
    }

    iot_alimqtt_channel = ch;
    if (ch->ch_open_arg) {
        strcpy(g_pub_topic, ch->ch_open_arg);
        ptr = strchr(g_pub_topic, ',');
        if (ptr) {
            *ptr = '\0';
            ptr++;
            strcpy(g_sub_topic, ptr);
        }
    }

    LOGD(TAG, "pub topic = %s, sub topic=%s", g_pub_topic, g_sub_topic);

    return 0;
}

static void alimqtt_channel_close(iot_channel_t *ch)
{
    alimqtt_usrv_disconnect();
    iot_alimqtt_channel = NULL;
    return;
}

static int alimqtt_channel_start(iot_channel_t *ch)
{
    int ret;

    if (ch->uData == NULL) {
        return -EINVAL;
    }

    ret = alimqtt_usrv_connect();
    if (ret < 0) {
        return -1;
    }

    ret = alimqtt_usrv_subscribe(g_sub_topic, alimqtt_yio_recv, (void *)ch);
    if (ret < 0) {
        LOGE(TAG, "subscribe %s", g_sub_topic);
    }

    return 0;
}

static int alimqtt_channel_send(iot_channel_t *ch)
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
        ret = alimqtt_usrv_send(g_pub_topic, str, ret);
    } else {
        LOGD("IOT", "nothing to send");
    }

    return ret;
}

static int  alimqtt_channel_raw_send(iot_channel_t *ch, void *payload, int len)
{
    int ret;

    if (payload == NULL || len <= 0) {
        return -EINVAL;
    }

    ret = alimqtt_usrv_send(g_pub_topic, payload, len);

    return ret >= 0 ? 0 : -1;
}

static int alimqtt_channel_recv(iot_channel_t *ch)
{
    //TODO: cloud data convert to uData
    return 0;
}
