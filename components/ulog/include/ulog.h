/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-25     armink       the first version
 */

#ifndef __ULOG_H__
#define __ULOG_H__

#include "ulog_defs.h"

#ifdef __cplusplus
extern "C"
{
#endif

int ulog_init(void);
void ulog_deinit(void);

/**
 * flush all backends's log
 */
void ulog_flush(void);

int ulog_backend_register(ulog_backend_t backend, const char *name);
int ulog_backend_unregister(ulog_backend_t backend);

#ifdef ULOG_USING_ASYNC_OUTPUT
void ulog_async_output(void);
#endif

#ifdef __cplusplus
}
#endif

#endif