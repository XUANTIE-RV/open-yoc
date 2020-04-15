/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-25     armink       the first version
 */

#include "ulog_console.h"
#include "ulog.h"
#include <stddef.h>

extern int uart_putc(int ch);

static struct ulog_backend console;

static void ulog_console_backend_output(struct ulog_backend *backend, const char *log, int len)
{
    if (backend && log && len > 0) {
        for (int i = 0; i < len; i++)
            uart_putc(log[i]);        
    }
}

int ulog_console_init(void)
{
    ulog_init();
    console.output = ulog_console_backend_output;

    ulog_backend_register(&console, "console");

    return 0;
}

int ulog_console_deinit(void)
{
    ulog_backend_unregister(&console);
    return 0;
}