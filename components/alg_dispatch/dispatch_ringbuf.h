/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef __RINGBUFFER_GROUP_H__
#define __RINGBUFFER_GROUP_H__

#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TYPE_PCM  = 0,
    TYPE_KWS,
    TYPE_FEAEC,
    TYPE_MAX
} data_type_e;

int dispatch_ringbuffer_init();
int dispatch_ringbuffer_clear(data_type_e type);
int dispatch_ringbuffer_write(data_type_e type, void *data, int data_len);
int dispatch_ringbuffer_destory(data_type_e type);
int dispatch_ringbuffer_create(data_type_e type, int buf_len);


#ifdef __cplusplus
}
#endif

#endif