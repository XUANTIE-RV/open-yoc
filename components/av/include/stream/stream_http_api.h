/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __STREAM_HTTP_API_H__
#define __STREAM_HTTP_API_H__

#include <aos/aos.h>

__BEGIN_DECLS__

int stream_http_open(const char *url, int mode);

int stream_http_close(void);

int stream_http_read( uint8_t *buf, size_t count);

int stream_http_write(const uint8_t *buf, size_t count);

int stream_http_control(int cmd, void *arg, size_t *arg_size);

__END_DECLS__

#endif /* __STREAM_HTTP_API_H__ */

