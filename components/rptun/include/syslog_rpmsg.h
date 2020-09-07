/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

/******************************************************************************
 * @file     song_rptun.h
 * @brief    head file for ck rptun
 * @version  V1.0
 * @date     20. Jan 2020
 ******************************************************************************/

#ifndef _INC_SONG_RPTUN_H
#define _INC_SONG_RPTUN_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

int syslog_rpmsg_server_init(void);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* _INC_SONG_RPTUN_H */
