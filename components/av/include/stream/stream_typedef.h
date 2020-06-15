/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __STREAM_TYPEDEF_H__
#define __STREAM_TYPEDEF_H__

#include <aos/aos.h>

__BEGIN_DECLS__

#define STREAM_RECV_TO_MS_DEFAULT (6*1000)
#define STREAM_RECV_TO_UNLIMITED  (AOS_WAIT_FOREVER)

enum stream_mode {
    STREAM_CREATE  = (1<<0),
    STREAM_READ    = (1<<1),
    STREAM_WRITE   = (1<<2),
};

typedef enum stream_type {
    STREAM_TYPE_UNKNOWN,
    STREAM_TYPE_FILE,
    STREAM_TYPE_HTTP,
    STREAM_TYPE_MEM,
    STREAM_TYPE_FIFO,
    STREAM_TYPE_CRYPTO,
} stream_type_t;

__END_DECLS__

#endif /* __STREAM_TYPEDEF_H__ */

