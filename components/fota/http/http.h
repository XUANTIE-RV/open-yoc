/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __HTTP_H__
#define __HTTP_H__

#include "util/network.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct http {
    char *path;
    char *host;
    uint8_t *buffer;
    int buffer_offset;
    char *url;
    int port;
    network_t net;
} http_t;

// char *json_getvalue(char *body, char *key, int *len);

http_t *http_init(const char *path);
int http_head_sets(http_t *http, const char *key, const char *value);
int http_head_seti(http_t *http, const char *key, int value);
int http_post(http_t *http, char *playload, int timeoutms);
int http_get(http_t *http, int timeoutms);
int http_wait_resp(http_t *http, char **head_end, int timeoutms);
char *http_head_get(http_t *http, char *key, int *length);
char *http_read_data(http_t *http);
int http_deinit(http_t *http);
#ifdef __cplusplus
}
#endif

#endif
