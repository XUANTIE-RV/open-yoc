/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __STREAM_IOCTL_H__
#define __STREAM_IOCTL_H__

#include "stream/stream_typedef.h"

__BEGIN_DECLS__

typedef enum stream_cmd {
    STREAM_CMD_UNKNOWN,
    STREAM_CMD_GET_SIZE,
    STREAM_CMD_GET_FORMAT,
    STREAM_CMD_GET_CODEC,
    STREAM_CMD_GET_CHANNEL,
    STREAM_CMD_GET_RATE,
} stream_cmd_t;


__END_DECLS__

#endif /* __STREAM_IOCTL_H__ */

