/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <yoc/iot.h>

iot_channel_t *iot_channel_open(iot_t *iot, const char *ch_arg)
{
    iot_channel_t *ch = NULL;

    if (iot && iot->channel_open) {
        ch = aos_zalloc_check(sizeof(iot_channel_t));
        aos_mutex_new(&ch->ch_mutex);
        ch->uData = yoc_udata_new();
        ch->iot         = iot;
        ch->ch_open_arg = ch_arg;

        if (ch->uData != NULL) {
            if (iot->channel_open(ch) >= 0)
                return ch;
        } else
            yoc_udata_free(ch->uData);

        aos_mutex_free(&ch->ch_mutex);
        aos_free(ch);
    }

    return NULL;
}

int iot_channel_start(iot_channel_t *ch)
{
    int ret = -1;

    if (ch && ch->iot && ch->iot->channel_start) {
        ret = ch->iot->channel_start(ch);
    }

    return ret;
}

int iot_channel_close(iot_channel_t *ch)
{
    if (ch && ch->iot && ch->iot->channel_close) {
        ch->iot->channel_close(ch);

        yoc_udata_free(ch->uData);
        aos_mutex_free(&ch->ch_mutex);
        aos_free(ch);
        return 0;
    }

    return -1;
}

int iot_channel_config(iot_channel_t *ch, channel_set_t set, channel_get_t get, void *arg)
{
    if (ch) {
        ch->ch_set     = set;
        ch->ch_get     = get;
        ch->ch_cfg_arg = arg;
        return 0;
    }

    return -1;
}

int iot_channel_push(iot_channel_t *ch, uint8_t sync)
{
    int ret = -1;

    if (ch && ch->iot && ch->iot->channel_send) {
        if (ch->ch_get && sync) {
            ch->ch_get(ch->uData, ch->ch_cfg_arg);
        }

        ret = ch->iot->channel_send(ch);

        if(ret < 0) {
            event_publish(EVENT_IOT_PUSH_FAILED, NULL);
        }
    }

    return ret;
}

int iot_channel_raw_push(iot_channel_t *ch, void *payload, int len)
{
    if (ch && ch->iot && ch->iot->channel_raw_send) {
        return ch->iot->channel_raw_send(ch, payload, len);
    }

    return -1;
}

int iot_destroy(iot_t *iot)
{
    if (iot && iot->destroy) {
        iot->destroy(iot);
    } else {
        return -1;
    }

    return 0;
}
