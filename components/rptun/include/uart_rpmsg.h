/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     uart_rpmsg.h
 * @brief    uart rpmsg Driver
 * @version  V1.0
 * @date     20. Jan 2020
 ******************************************************************************/

#ifndef _UART_RPMSG_H
#define _UART_RPMSG_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdbool.h>

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

#ifdef CONFIG_SERIAL_USE_VFS
int uart_rpmsg_init(const char *cpu_name, const char *dev_name,
                    int buf_size, bool isconsole);
#else
int uart_rpmsg_init(const char *cpuname, const char *devname,
                    int uart_idx, int buf_size, bool isconsole);
#endif

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* _UART_RPMSG_H */
