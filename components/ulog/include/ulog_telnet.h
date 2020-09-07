/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-25     armink       the first version
 */

#ifndef __ULOG_TELNET_H__
#define __ULOG_TELNET_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef void (*ulog_telnet_event)(int connected);

int ulog_telnet_init(ulog_telnet_event event);

int ulog_telnet_deinit(void);

#ifdef __cplusplus
}
#endif

#endif