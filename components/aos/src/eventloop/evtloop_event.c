/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <aos/yloop.h>
#include <aos/list.h>
#include "evtloop_main.h"

#ifndef DEBUG_MALLOC
#define DEBUG_MALLOC malloc
#endif

#ifndef DEBUG_FREE
#define DEBUG_FREE free
#endif


extern int event_write(const void *buf, size_t len);
extern int event_read(void *buf, size_t len);
extern int event_poll(void *reader, bool setup, void *sem);
void * g_current_event_cb = NULL;

typedef struct {
    dlist_t       node;
    aos_event_cb  cb;
    void         *private_data;
    uint16_t      type_filter;
} local_event_list_node_t;

static dlist_t g_local_event_list = AOS_DLIST_INIT(g_local_event_list);

static int input_add_event(input_event_t *event)
{
    return event_write(event, sizeof(*event));
}

static void handle_events(input_event_t *event)
{
    if (event->type == EV_RPC) {
        aos_call_t handler = (aos_call_t)event->value;
        void *arg = (void *)event->extra;
        handler(arg);

        return;
    }

    if (event->type == EV_SYS && event->code == CODE_NULL && event->value == VALUE_NULL) {
        return;
    } else if (event->type == EV_ALL && event->code == CODE_NULL && event->value == VALUE_NULL) {
        //LOGD("loop", "loop run test");
        return;
    }

    local_event_list_node_t *event_node = NULL;
    dlist_for_each_entry(&g_local_event_list, event_node, local_event_list_node_t, node) {
        if (event_node->type_filter != EV_ALL
            && event_node->type_filter != event->type) {
            continue;
        }
        g_current_event_cb = event_node->cb;
        (event_node->cb)(event, event_node->private_data);
        g_current_event_cb = NULL;
    }
}

void event_read_cb(void)
{
    input_event_t event;
    int ret = event_read(&event, sizeof(event));

    if (ret == sizeof(event)) {
        handle_events(&event);
    }
}

int aos_post_event(uint16_t type, uint16_t code, unsigned long value)
{
    int ret;
    input_event_t event = {
        .type  = type,
        .code  = code,
        .value = value,
    };

    ret = input_add_event(&event);
    if (ret < 0) {
        return -EPERM;
    }

    return 0;
}

#if 0
int yoc_eloop_register_listener(aos_event_cb cb, void *priv)
{
    local_event_list_node_t *event_node = DEBUG_MALLOC(sizeof(local_event_list_node_t));

    if (NULL == event_node) {
        return -ENOMEM;
    }

    event_node->cb           = cb;
    event_node->private_data = priv;
    event_node->type_filter  = EV_ALL;
    dlist_add_tail(&event_node->node, &g_local_event_list);

    return YOC_OK;
}
#endif

void yoc_eloop_unregister_listener(aos_event_cb cb, void *priv)
{
    local_event_list_node_t *event_node = NULL;
    dlist_for_each_entry(&g_local_event_list, event_node, local_event_list_node_t, node) {
        if (event_node->cb == cb && event_node->private_data == priv) {
            dlist_del(&event_node->node);
            DEBUG_FREE(event_node);
            return;
        }
    }
}

int aos_register_event_filter(uint16_t type, aos_event_cb cb, void *priv)
{
    if (cb == NULL) {
        return -EINVAL;
    }

    local_event_list_node_t *event_node = DEBUG_MALLOC(sizeof(local_event_list_node_t));
    if (NULL == event_node) {
        return -ENOMEM;
    }

    event_node->cb           = cb;
    event_node->private_data = priv;
    event_node->type_filter  = type;

    dlist_add_tail(&event_node->node, &g_local_event_list);

    return 0;
}

int aos_unregister_event_filter(uint16_t type, aos_event_cb cb, void *priv)
{
    local_event_list_node_t *event_node = NULL;

    if (cb == NULL) {
        return -EINVAL;
    }

    dlist_for_each_entry(&g_local_event_list, event_node, local_event_list_node_t, node) {
        if (event_node->cb == cb && event_node->private_data == priv && event_node->type_filter == type) {
            dlist_del(&event_node->node);
            DEBUG_FREE(event_node);
            return 0;
        }
    }

    return -ENOMEM;
}
/*
 * schedule a callback in yos loop main thread
 */
int aos_schedule_call(aos_call_t fun, void *arg)
{
    int ret;

    input_event_t event = {
        .type = EV_RPC,
        .value = (unsigned long)fun,
        .extra = (unsigned long)arg,
    };

    if (fun == NULL) {
        return -EINVAL;
    }

    ret = input_add_event(&event);
    if (ret < 0) {
        return -EPERM;
    }

    return 0;
}

