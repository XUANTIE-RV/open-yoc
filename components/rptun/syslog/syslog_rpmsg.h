/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */
/******************************************************************************
 * @file     syslog_rpmsg.h
 * @brief    CSI Header File for syslog rpmsg server
 * @version  V1.0
 * @date     20. Jan 2020
 ******************************************************************************/

#ifndef _SYSLOG_RPMSG_H
#define _SYSLOG_RPMSG_H

/****************************************************************************
 * Pre-processor definitions
 ****************************************************************************/

#define SYSLOG_RPMSG_EPT_NAME           "rpmsg-syslog"

#define SYSLOG_RPMSG_TRANSFER           0
#define SYSLOG_RPMSG_TRANSFER_DONE      1
#define SYSLOG_RPMSG_SUSPEND            2
#define SYSLOG_RPMSG_RESUME             3

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct syslog_rpmsg_header_s
{
    uint32_t command;
    int32_t  result;
} __attribute__((__packed__));

struct syslog_rpmsg_transfer_s
{
    struct syslog_rpmsg_header_s header;
    int32_t                      count;
    char                         data[0];
} __attribute__((__packed__));

#endif /* _SYSLOG_RPMSG_H */
