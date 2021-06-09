/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

#include <aos/yloop.h>
#include <aos/kernel.h>
#include <aos/list.h>
#include <errno.h>
#include "evtloop_main.h"

//static const char *TAG = "eventloop";

#ifndef DEBUG_MALLOC
#define DEBUG_MALLOC malloc
#endif
#ifndef DEBUG_REALLOC
#define DEBUG_REALLOC realloc
#endif
#ifndef DEBUG_CALLOC
#define DEBUG_CALLOC calloc
#endif

#ifndef DEBUG_FREE
#define DEBUG_FREE free
#endif

#define LOOP_WDT_TIMER   (1000 * 10)

typedef void (*eventloop_timeout_cb)(void *private_data);

typedef struct eventloop_timeout_s {
    dlist_t          next;
    long long        timeout_ms;
    void             *private_data;
    eventloop_timeout_cb        cb;
    int              ms;
} eventloop_timeout_t;

typedef struct {
    dlist_t          timeouts;
    eventloop_sock_t    reader;
    uint8_t          max_sock;
    bool             pending_terminate;
    bool             terminate;
} eventloop_ctx_t;

static eventloop_ctx_t  g_main_ctx;

static inline eventloop_ctx_t *get_context(void)
{
    return &g_main_ctx;
}

extern int event_poll(void *reader, bool setup, void *sem);
extern void event_read_cb(void);

aos_loop_t aos_loop_init(void)
{
    memset(&g_main_ctx, 0, sizeof(eventloop_ctx_t));

    g_main_ctx.reader.poll = event_poll;
    g_main_ctx.reader.cb = event_read_cb;

    eventloop_local_event_init();

    dlist_init(&g_main_ctx.timeouts);

    return &g_main_ctx;
}

aos_loop_t aos_current_loop(void)
{
    return (void *)&g_main_ctx;
}

static int eventloop_poll(int timeout)
{
    eventloop_ctx_t *ctx = get_context();

    aos_sem_t sem;
    int       ret = 0;

    aos_sem_new(&sem, 0);

    /* clear reader event flag */
    ctx->reader.event = 0;

    /* poll readers */
    ctx->reader.poll(&ctx->reader, true, &sem);

    if (!timeout) {
        ret = 0;
        goto check_poll;
    }

    if (timeout == -1) {
        ret = aos_sem_wait(&sem, AOS_WAIT_FOREVER);
    } else {
        ret = aos_sem_wait(&sem, timeout);
    }

check_poll:

    ctx->reader.poll(&ctx->reader, false, &sem);

    if (ctx->reader.event) {
        ret++;
    }

    aos_sem_free(&sem);

    return ret < 0 ? 0 : ret;
}

void aos_loop_run(void)
{
    eventloop_ctx_t *ctx = get_context();

    while (!ctx->terminate) {

        int delayed_ms = -1;

        if (!dlist_empty(&ctx->timeouts)) {
            eventloop_timeout_t *tmo = dlist_first_entry(&ctx->timeouts, eventloop_timeout_t, next);
            long long now = aos_now_ms();

            if (now < tmo->timeout_ms) {
                delayed_ms = tmo->timeout_ms - now;
            } else {
                delayed_ms = 0;
            }
        }

        int res = eventloop_poll(delayed_ms);

        /* check if some registered timeouts have occurred */
        if (!dlist_empty(&ctx->timeouts)) {
            eventloop_timeout_t *tmo = dlist_first_entry(&ctx->timeouts, eventloop_timeout_t, next);
            long long now = aos_now_ms();

            if (now >= tmo->timeout_ms) {
                dlist_del(&tmo->next);
                tmo->cb(tmo->private_data);
                DEBUG_FREE(tmo);
            }
        }

        if (res <= 0) {
            continue;
        }

        if (ctx->reader.event) {
            ctx->reader.cb();
        }
    }

    ctx->terminate = 0;
}

void aos_loop_exit(void)
{
    eventloop_ctx_t *ctx = get_context();
    ctx->terminate = 1;

    aos_post_event(EV_SYS, CODE_SYS_LOOP_EXIT, VALUE_NULL);
}

void aos_loop_destroy(void)
{
    eventloop_ctx_t *ctx = get_context();

    eventloop_local_event_deinit();


    while (!dlist_empty(&ctx->timeouts)) {
        eventloop_timeout_t *timeout = dlist_first_entry(&ctx->timeouts, eventloop_timeout_t,
                                       next);
        dlist_del(&timeout->next);
        DEBUG_FREE(timeout);
    }
}

int aos_post_delayed_action(int ms, aos_call_t action, void *param)
{
    //int ret;
    eventloop_timeout_t *tmp;
    eventloop_timeout_t *timeout;
    eventloop_ctx_t *ctx = get_context();

    if (action == NULL || ms < 0) {
        return -EINVAL;
    }

    timeout = DEBUG_MALLOC(sizeof(*timeout));

    if (timeout == NULL) {
        return -ENOMEM;
    }

    timeout->timeout_ms = aos_now_ms() + ms;
    timeout->private_data = param;
    timeout->cb = action;
    timeout->ms = ms;

    dlist_for_each_entry(&ctx->timeouts, tmp, eventloop_timeout_t, next) {
        if (timeout->timeout_ms < tmp->timeout_ms) {
            break;
        }
    }
    dlist_add_tail(&timeout->next, &tmp->next);

    /* fix bug: if no timer, loop will wait forever, post event active loop */
    aos_post_event(EV_SYS, CODE_NULL, VALUE_NULL);

    return 0;
}

void aos_cancel_delayed_action(int ms, aos_call_t action, void *param)
{
    eventloop_ctx_t *ctx = get_context();
    eventloop_timeout_t *tmp;

    if (action == NULL) {
        return;
    }

    dlist_for_each_entry(&ctx->timeouts, tmp, eventloop_timeout_t, next) {
        if (ms >= 0 && tmp->ms != ms) {
            continue;
        }

        if (tmp->cb != action) {
            continue;
        }

        if (tmp->private_data != param) {
            continue;
        }

        dlist_del(&tmp->next);
        DEBUG_FREE(tmp);
        return;
    }
}
