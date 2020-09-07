/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AVFORMAT_ALL_H__
#define __AVFORMAT_ALL_H__

#include <aos/aos.h>

__BEGIN_DECLS__

/**
 * @brief  regist demux for wav
 * @return 0/-1
 */
int demux_register_wav();

/**
 * @brief  regist demux for mp3
 * @return 0/-1
 */
int demux_register_mp3();

/**
 * @brief  regist demux for mp4
 * @return 0/-1
 */
int demux_register_mp4();

/**
 * @brief  regist demux for rawaudio
 * @return 0/-1
 */
int demux_register_rawaudio();

/**
 * @brief  regist demux for adts
 * @return 0/-1
 */
int demux_register_adts();

/**
 * @brief  regist demux for flac
 * @return 0/-1
 */
int demux_register_flac();

/**
 * @brief  regist demux for asf
 * @return 0/-1
 */
int demux_register_asf();

/**
 * @brief  regist demux for amr
 * @return 0/-1
 */
int demux_register_amr();

/**
 * @brief  regist all demuxer
 * @return 0/-1
 */
static inline int demux_register_all()
{
    int rc = 0;

    rc |= demux_register_wav();
    rc |= demux_register_mp3();
    rc |= demux_register_mp4();
    rc |= demux_register_adts();
    rc |= demux_register_rawaudio();
    rc |= demux_register_flac();
    rc |= demux_register_asf();
    rc |= demux_register_amr();

    return rc;
}


__END_DECLS__

#endif /* __AVFORMAT_ALL_H__ */

