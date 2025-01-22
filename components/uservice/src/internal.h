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

#ifndef USERVICE_INTERNAL_H
#define USERVICE_INTERNAL_H

#include <stdint.h>
#include <uservice/uservice.h>
#include "ulog/ulog.h"
#include <aos/list.h>
#include <aos/kernel.h>
#include <aos/debug.h>
#ifdef __linux__
#include <mqueue.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct uservice {
    const char *name;
    void       *context;
    process_t   process_rpc;

    utask_t    *task;
    slist_t     next;
};

struct rpc_record {
    uint32_t cmd_id;
    uint32_t count;
    struct uservice *srv;
    slist_t next;
};

struct utask {
    aos_task_t  task;
    int8_t      running;

    uint8_t     queue_count;
#if defined(CONFIG_DEBUG) && defined(CONFIG_DEBUG_UTASK)
    uint8_t     queue_max_used;
    slist_t     rpc_reclist;
#endif
#ifdef __linux__
    mqd_t       queue;
    char        *qname;
#else
    void       *queue_buffer;
    aos_queue_t queue;
#endif
    aos_mutex_t mutex;
    rpc_t      *current_rpc;
    aos_sem_t   running_wait;

    slist_t     uservice_lists;
    slist_t     rpc_buffer_gc_cache;
    slist_t     node;
};

struct rpc_buffer {
    int       timeout_ms;
    uint8_t * buffer;
    uint16_t  buf_size;
    uint16_t  pos;
    aos_sem_t sem;

    slist_t  next;
};

#define TASK_LOCK(task) aos_mutex_lock(&(task->mutex), AOS_WAIT_FOREVER)
#define TASK_UNLOCK(task) aos_mutex_unlock(&(task->mutex))

void rpc_free(rpc_t *rpc);
int  rpc_wait(rpc_t *rpc);

typedef struct event_list {
    slist_t     events;
    aos_mutex_t mutex;
} event_list_t;

int  eventlist_init(event_list_t *evlist);
void eventlist_uninit(event_list_t *evlist);

int  eventlist_subscribe(event_list_t *evlist, uint32_t event_id, event_callback_t cb, void *context);
int  eventlist_unsubscribe(event_list_t *evlist, uint32_t event_id, event_callback_t cb, void *context);
int  eventlist_publish(event_list_t *evlist, uint32_t event_id, void *data);
int  eventlist_remove(event_list_t *evlist, uint32_t event_id);

int  eventlist_subscribe_fd(event_list_t *evlist, uint32_t fd, event_callback_t cb, void *context);
int  eventlist_unsubscribe_fd(event_list_t *evlist, uint32_t fd, event_callback_t cb, void *context);
int  eventlist_publish_fd(event_list_t *evlist, uint32_t fd, void *data);
int  eventlist_remove_fd(event_list_t *evlist, uint32_t fd);

int  eventlist_setfd(event_list_t *evlist, void *readfds);

#ifdef __cplusplus
}
#endif

#endif
