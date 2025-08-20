 /*
 * Copyright (C) 2017-2024 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

