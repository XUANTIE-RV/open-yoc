/*
 * Copyright (C) 2020 FishSemi Inc. All rights reserved.
 */

/******************************************************************************
 * @file     song_rptun.h
 * @brief    head file for ck rptun
 * @version  V1.0
 * @date     20. Jan 2020
 ******************************************************************************/

#ifndef _SONG_RPTUN_H
#define _SONG_RPTUN_H

/****************************************************************************
 * Included Files
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

struct song_rptun_config_s
{
    const char                   *cpuname;
    const char                   *firmware;

    const struct rptun_addrenv_s *addrenv;
    struct rptun_rsc_s           *rsc;

    bool                         nautostart;
    bool                         master;

    int32_t                      mailbox;

    int (*config)(const struct song_rptun_config_s *config, void *data);
    int (*start)(const struct song_rptun_config_s *config);
    int (*stop)(const struct song_rptun_config_s *config);
};

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

void song_rptun_initialize(const struct song_rptun_config_s *config);

#ifdef __cplusplus
}
#endif
#endif /* _SONG_RPTUN_H */
