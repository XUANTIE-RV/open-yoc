/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include <stdbool.h>
#include "aos/yloop.h"

typedef void (*eventloop_sock_cb)(void);
typedef int (*reader_poll_t)(void *reader, bool setup, void *sem);

typedef struct {
    int          event;
    reader_poll_t poll;
    eventloop_sock_cb cb;
} eventloop_sock_t;

/**
 * This function will init per-loop event service
 * @param[in]  void
 * @return  0 is OK, -1 is error
 */
int eventloop_local_event_init(void);

/**
 * This function will deinit per-loop event service
 * @param[in]  void
 * @return  0 is OK, -1 is error
 */
int eventloop_local_event_deinit(void);

#endif /* YLOOP_H */

