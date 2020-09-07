/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-25     armink       the first version
 */
#ifndef __ULOG_CONSOLE_H__
#define __ULOG_CONSOLE_H__

#ifdef __cplusplus
extern "C"
{
#endif

int ulog_console_init(void);

int ulog_console_deinit(void);

#ifdef __cplusplus
}
#endif

#endif