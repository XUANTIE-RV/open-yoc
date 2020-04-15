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

int uart_rpmsg_init(const char *cpu_name, const char *dev_name,
                    int buf_size, bool isconsole);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* _UART_RPMSG_H */
