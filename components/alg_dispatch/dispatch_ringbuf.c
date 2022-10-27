/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <ulog/ulog.h>
#include <aos/ringbuffer.h>
#include <aos/aos.h>

#include "dispatch_internal.h"

#define TAG "Dispatch_buf"

static dev_ringbuf_t pcm_rb;
static dev_ringbuf_t kws_rb;

int voice_get_pcm_data(void *data, int len) {
  uint8_t *temp = (uint8_t *)data;
  int read_len = len;

  read_len = ringbuffer_read(&pcm_rb, (uint8_t *)temp, len);

  return read_len;
}

int voice_get_kws_data(void *data, int len) {
  uint8_t *temp = (uint8_t *)data;
  int read_len = len;

  read_len = ringbuffer_read(&kws_rb, (uint8_t *)temp, len);

  return read_len;
}


void dispatch_ringbuffer_create(data_type_e type, char *buf, int buf_len) {
    if (type == TYPE_PCM) {
        ringbuffer_create(&pcm_rb, buf, buf_len);
    } else if (type == TYPE_KWS) {
        ringbuffer_create(&kws_rb, buf, buf_len);
    }
}

void dispatch_ringbuffer_destory(data_type_e type) {
    if (type == TYPE_PCM) {
        ringbuffer_destroy(&pcm_rb);
    } else if (type == TYPE_KWS) {
        ringbuffer_destroy(&kws_rb);
    }
}

void dispatch_ringbuffer_write(data_type_e type, void *data, int data_len) {
    if (type == TYPE_PCM) {
        if (ringbuffer_full(&pcm_rb)) {
          ;//LOGE(TAG, "pcm ringbuffer is full");
        }
        ringbuffer_write(&pcm_rb, (uint8_t *)data, data_len);
    } else if (type == TYPE_KWS) {
        if (ringbuffer_full(&kws_rb)) {
          LOGE(TAG, "pcm ringbuffer is full");
        }
        ringbuffer_write(&kws_rb, (uint8_t *)data, data_len);
    }
}

void dispatch_ringbuffer_clear(data_type_e type) {
    if (type == TYPE_PCM) {
        ringbuffer_clear(&pcm_rb);
    } else if (type == TYPE_KWS) {
        ringbuffer_clear(&kws_rb);
    }
}