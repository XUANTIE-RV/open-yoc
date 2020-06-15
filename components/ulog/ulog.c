/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-25     armink       the first version
 */

#include "ulog.h"
#include "ulog_defs.h"
#include <aos/log.h>
#include <aos/kernel.h>
#include <aos/ringblk_buf.h>
#include <aos/debug.h>
#include "serf/minilibc_stdio.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#define _ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

#define ULOG_ASYNC_OUTPUT_THREAD_STACK  (4 * 1024)
#ifndef ULOG_ASYNC_OUTPUT_BUF_SIZE
#define ULOG_ASYNC_OUTPUT_BUF_SIZE      (64 * 1024)
#endif
/* the number which is max stored line logs */
#ifndef ULOG_ASYNC_OUTPUT_STORE_LINES
#define ULOG_ASYNC_OUTPUT_STORE_LINES  (ULOG_ASYNC_OUTPUT_BUF_SIZE * 3 / 2 / ULOG_LINE_BUF_SIZE)
#endif
struct _ulog
{
    int         init_ok;
    aos_mutex_t output_locker;
    /* all backends */
    slist_t     backend_list;
    /* the thread log's line buffer */
    char        log_buf_th[ULOG_LINE_BUF_SIZE];

#ifdef ULOG_USING_ASYNC_OUTPUT
    rbb_t           async_rbb;
    aos_task_t      async_th;
    aos_sem_t       async_notice;
    aos_sem_t       async_sem_finish;
    int             async_th_run;
#endif
};
typedef struct _ulog ulog_t;
static ulog_t t_ulog = { 0 };

void ulog_output_to_all_backend(const char *log, int size)
{
    slist_t *node;
    ulog_backend_t backend;

    if (!t_ulog.init_ok)
        return;

    /* output for all backends */
    for (node = slist_first(&t_ulog.backend_list); node; node = slist_next(node))
    {
        backend = slist_entry(node, struct ulog_backend, list);
        if (backend && backend->output)
            backend->output(backend, log, size);
    }
}
static void output_lock(void)
{
    aos_mutex_lock(&t_ulog.output_locker, AOS_WAIT_FOREVER);
}

static void output_unlock(void)
{
    aos_mutex_unlock(&t_ulog.output_locker);
}

static void do_output(const char *log_buf, int log_len)
{
#ifdef ULOG_USING_ASYNC_OUTPUT
    rbb_blk_t log_blk;
    ulog_frame_t log_frame;

    /* allocate log frame */
    log_blk = rbb_blk_alloc(t_ulog.async_rbb, _ALIGN(sizeof(struct ulog_frame) + log_len, 4));
    if (log_blk)
    {
        /* package the log frame */
        log_frame = (ulog_frame_t) log_blk->buf;
        // FIXME:
        log_frame->magic = ULOG_FRAME_MAGIC;
        log_frame->log_len = log_len;
        log_frame->log = (const char *)log_blk->buf + sizeof(struct ulog_frame);
        /* copy log data */
        memcpy(log_blk->buf + sizeof(struct ulog_frame), log_buf, log_len);
        /* put the block */
        rbb_blk_put(log_blk);
        /* send a notice */
        aos_sem_signal(&t_ulog.async_notice);
    }
    else
    {
        static int already_output = 0;
        if (already_output == 0)
        {
            printf("Warning: There is no enough buffer for saving async log,"
                    " please increase the ULOG_ASYNC_OUTPUT_BUF_SIZE option.\n");
            already_output = 1;
        }
    }
#else
    ulog_output_to_all_backend(log_buf, log_len);
#endif /* ULOG_USING_ASYNC_OUTPUT */
}

extern int g_syslog_level;
extern const char const g_yoc_log_string[];
int aos_log_tag(const char *tag, int log_level, const char *fmt, ...)
{
    struct timespec ts;
    int ret;
    int offset = 0;
    char *log_buf;
    int log_len = 0;
    va_list args;

    if (log_level > g_syslog_level ) {
        return -1;
    }

    output_lock();

    log_buf = t_ulog.log_buf_th;

    if (tag != NULL) {
        /* Get the current time */
        ret = clock_gettime(CLOCK_MONOTONIC, &ts);

        if (ret == 0) {
            snprintf(log_buf, ULOG_LINE_BUF_SIZE, "[%6d.%06d][%c][%-8s]", 
                        ts.tv_sec, (int)ts.tv_nsec / 1000, g_yoc_log_string[log_level], tag);
        } else {
            snprintf(log_buf, ULOG_LINE_BUF_SIZE, "[%c][%-8s]", g_yoc_log_string[log_level], tag);
        }
        offset = strlen(log_buf);
    }

    va_start(args, fmt);
    int fmt_result = vsnprintf(&log_buf[offset], ULOG_LINE_BUF_SIZE - offset, fmt, args);
    va_end(args);
    if (fmt_result > -1 && fmt_result + offset < ULOG_LINE_BUF_SIZE) {
        log_len = fmt_result + offset;
    } else {
        log_len = ULOG_LINE_BUF_SIZE;
        log_buf[ULOG_LINE_BUF_SIZE - 1] = '\n';
    }
    /* do log output */
    do_output(log_buf, log_len);

    output_unlock();

    return 0;
}

int ulog_backend_register(ulog_backend_t backend, const char *name)
{
    aos_assert(backend);
    aos_assert(name);
    aos_assert(t_ulog.init_ok);
    aos_assert(backend->output);

    if (backend->init)
    {
        backend->init(backend);
    }

    memcpy(backend->name, name, ULOG_NAME_MAX);

    // slist_add_tail(&t_ulog.backend_list, &backend->list);
    slist_add_tail(&backend->list, &t_ulog.backend_list);

    return 0;
}

int ulog_backend_unregister(ulog_backend_t backend)
{
    aos_assert(backend);
    aos_assert(t_ulog.init_ok);

    if (backend->deinit)
    {
        backend->deinit(backend);
    }

    slist_remove(&t_ulog.backend_list, &backend->list);

    return 0;
}

#ifdef ULOG_USING_ASYNC_OUTPUT
/**
 * asynchronous output logs to all backends
 *
 * @note you must call this function when ULOG_ASYNC_OUTPUT_BY_THREAD is disable
 */
void ulog_async_output(void)
{
    rbb_blk_t log_blk;
    ulog_frame_t log_frame;

    while ((log_blk = rbb_blk_get(t_ulog.async_rbb)) != NULL)
    {
        log_frame = (ulog_frame_t) log_blk->buf;
        if (log_frame->magic == ULOG_FRAME_MAGIC)
        {
            /* output to all backends */
            ulog_output_to_all_backend(log_frame->log, log_frame->log_len);
        }
        rbb_blk_free(t_ulog.async_rbb, log_blk);
    }
}

static void async_output_thread_entry(void *param)
{
    while (t_ulog.async_th_run)
    {
        aos_sem_wait(&t_ulog.async_notice, AOS_WAIT_FOREVER);
        ulog_async_output();
    }
    aos_task_exit(0);
    aos_sem_free(&t_ulog.async_notice);
    aos_sem_signal(&t_ulog.async_sem_finish);
}
#endif /* ULOG_USING_ASYNC_OUTPUT */

/**
 * flush all backends's log
 */
void ulog_flush(void)
{
    slist_t *node;
    ulog_backend_t backend;

    if (!t_ulog.init_ok)
        return;

#ifdef ULOG_USING_ASYNC_OUTPUT
    ulog_async_output();
#endif

    /* flush all backends */
    for (node = slist_first(&t_ulog.backend_list); node; node = slist_next(node))
    {
        backend = slist_entry(node, struct ulog_backend, list);
        if (backend->flush)
        {
            backend->flush(backend);
        }
    }
}

int ulog_init(void)
{
    if (t_ulog.init_ok)
        return 0;

    aos_mutex_new(&t_ulog.output_locker);
    slist_init(&t_ulog.backend_list);

#ifdef ULOG_USING_ASYNC_OUTPUT
    aos_assert(ULOG_ASYNC_OUTPUT_STORE_LINES >= 2);
    /* async output ring block buffer */
    t_ulog.async_rbb = rbb_create(_ALIGN(ULOG_ASYNC_OUTPUT_BUF_SIZE, 4), ULOG_ASYNC_OUTPUT_STORE_LINES);
    if (t_ulog.async_rbb == NULL)
    {
        printf("Error: ulog init failed! No memory for async rbb.\n");
        aos_mutex_free(&t_ulog.output_locker);
        return -ENOMEM;
    }
    /* async output thread */
    t_ulog.async_th_run = 1;
    aos_task_new_ext(&t_ulog.async_th, "ulog_async", async_output_thread_entry, &t_ulog, ULOG_ASYNC_OUTPUT_THREAD_STACK, 40);
    if (t_ulog.async_th.hdl == NULL)
    {
        printf("Error: ulog init failed! No memory for async output thread.\n");
        aos_mutex_free(&t_ulog.output_locker);
        rbb_destroy(t_ulog.async_rbb);
        return -ENOMEM;
    }

    aos_sem_new(&t_ulog.async_notice, 0);
    aos_sem_new(&t_ulog.async_sem_finish, 0);
#endif /* ULOG_USING_ASYNC_OUTPUT */

    t_ulog.init_ok = 1;

    return 0;
}

void ulog_deinit(void)
{
    slist_t *node;
    ulog_backend_t backend;

    if (!t_ulog.init_ok)
        return;

    /* deinit all backends */
    for (node = slist_first(&t_ulog.backend_list); node; node = slist_next(node))
    {
        backend = slist_entry(node, struct ulog_backend, list);
        if (backend->deinit)
        {
            backend->deinit(backend);
        }
    }

    aos_mutex_free(&t_ulog.output_locker);

#ifdef ULOG_USING_ASYNC_OUTPUT
    aos_sem_signal(&t_ulog.async_notice);
    t_ulog.async_th_run = 0;
    aos_sem_wait(&t_ulog.async_sem_finish, AOS_WAIT_FOREVER);
    aos_sem_free(&t_ulog.async_sem_finish);
    rbb_destroy(t_ulog.async_rbb);
#endif

    t_ulog.init_ok = 0;
}