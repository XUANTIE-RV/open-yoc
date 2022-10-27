/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef __RECORD_PROCESS_H__
#define __RECORD_PROCESS_H__

#include <aos/aos.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TYPE_PCM  = 0,
    TYPE_KWS,
    TYPE_MAX
} data_type_e;

  void dispatch_ringbuffer_clear(data_type_e type);

  void dispatch_ringbuffer_write(data_type_e type, void *data, int data_len);

  void dispatch_ringbuffer_destory(data_type_e type);

  void dispatch_ringbuffer_create(data_type_e type, char *buf, int buf_len);


#ifdef __cplusplus
}
#endif

#endif