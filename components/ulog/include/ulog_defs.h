/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-25     armink       the first version
 */

#ifndef __ULOG_DEFS_H__
#define __ULOG_DEFS_H__

#include <aos/list.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* buffer size for every line's log */
#ifndef ULOG_LINE_BUF_SIZE
#define ULOG_LINE_BUF_SIZE             256
#endif

#define ULOG_FRAME_MAGIC               0x10
#define ULOG_NAME_MAX 12

struct ulog_frame
{
    /* magic word is 0x10 ('lo') */
    uint32_t magic:8;
    uint32_t is_raw:1;
    uint32_t log_len:23;
    uint32_t level;
    const char *log;
    const char *tag;
};
typedef struct ulog_frame *ulog_frame_t;

struct ulog_backend
{
    char name[ULOG_NAME_MAX];
    void (*init)  (struct ulog_backend *backend);
    void (*output)(struct ulog_backend *backend, const char *log, int len);
    void (*flush) (struct ulog_backend *backend);
    void (*deinit)(struct ulog_backend *backend);
    slist_t list;
};
typedef struct ulog_backend *ulog_backend_t;

#ifdef __cplusplus
}
#endif

#endif /* __ULOG_DEFS_H__ */
