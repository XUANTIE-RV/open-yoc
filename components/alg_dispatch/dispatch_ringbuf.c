/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <ulog/ulog.h>
#include <aos/ringbuffer.h>
#include <aos/aos.h>

#include "dispatch_ringbuf.h"

#define TAG "RINGBUF_GROUP"

#define EVENT_CAN_WRITE 0x0F0F0000
#define EVENT_CAN_READ  0x00000F0F

typedef struct _ringbuf_group_ {
    dev_ringbuf_t rb;
    aos_event_t   event;
    char         *buffer;
} ringbuf_group_t;

static ringbuf_group_t g_rbgroup[TYPE_MAX];

/**********************************************************
 * Dispatch API
 *********************************************************/
int voice_get_pcm_data(void *data, int len)
{
    uint8_t *temp     = (uint8_t *)data;
    int      read_len = len;

    read_len = ringbuffer_read(&g_rbgroup[TYPE_PCM].rb, (uint8_t *)temp, len);

    return read_len;
}

int voice_get_kws_data(void *data, int len)
{
    uint8_t *temp     = (uint8_t *)data;
    int      read_len = len;

    read_len = ringbuffer_read(&g_rbgroup[TYPE_KWS].rb, (uint8_t *)temp, len);

    return read_len;
}

int voice_get_feaec_data(void *data, int len, int timeout)
{
    uint8_t     *temp       = (uint8_t *)data;
    int          read_len   = len;

    if (!aos_event_is_valid(g_rbgroup[TYPE_FEAEC].event)) {
        return -1;
    }

    if (ringbuffer_available_read_space(&g_rbgroup[TYPE_FEAEC].rb) >= len) {
         read_len = ringbuffer_read(&g_rbgroup[TYPE_FEAEC].rb, (uint8_t *)temp, len);
    } else {
        unsigned int actl_flags = 0;
        aos_event_get(&g_rbgroup[TYPE_FEAEC].event, EVENT_CAN_READ, AOS_EVENT_OR_CLEAR, &actl_flags, timeout);
        read_len = ringbuffer_read(&g_rbgroup[TYPE_FEAEC].rb, (uint8_t *)temp, len);
    }
    return read_len;
}

/**********************************************************
 * Ringbuffer Group
 *********************************************************/
int dispatch_ringbuffer_create(data_type_e type, int buf_len)
{
    if (g_rbgroup[type].buffer == NULL) {

        int ret = aos_event_new(&g_rbgroup[type].event, 0);
        if (ret != 0) {
            return -1;
        }

        g_rbgroup[type].buffer = (char *)malloc(buf_len);
        ringbuffer_create(&g_rbgroup[type].rb, g_rbgroup[type].buffer, buf_len);
    }
    return 0;
}

int dispatch_ringbuffer_destory(data_type_e type)
{
    if (g_rbgroup[type].buffer) {
        ringbuffer_destroy(&g_rbgroup[type].rb);
        aos_event_free(g_rbgroup[type].event);
    }
    return 0;
}

int dispatch_ringbuffer_write(data_type_e type, void *data, int data_len)
{
    if (ringbuffer_available_write_space(&g_rbgroup[type].rb) < data_len) {
        // LOGE(TAG, "pcm ringbuffer is full");
        return 0;
    }

    aos_event_set(g_rbgroup[type].event, EVENT_CAN_READ, AOS_EVENT_OR);
    return ringbuffer_write(&g_rbgroup[type].rb, (uint8_t *)data, data_len);
}

int dispatch_ringbuffer_clear(data_type_e type)
{
    ringbuffer_clear(&g_rbgroup[type].rb);
    return 0;
}

int dispatch_ringbuffer_init()
{
    memset(&g_rbgroup, 0, sizeof(g_rbgroup));
    return 0;
}
