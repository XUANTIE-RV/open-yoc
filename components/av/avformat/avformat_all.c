/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avformat/avformat.h"
#include "avformat/avformat_all.h"

/**
 * @brief  regist demux for wav
 * @return 0/-1
 */
int demux_register_wav()
{
    extern struct demux_ops demux_ops_wav;
    return demux_ops_register(&demux_ops_wav);
}

/**
 * @brief  regist demux for mp3
 * @return 0/-1
 */
int demux_register_mp3()
{
    extern struct demux_ops demux_ops_mp3;
    return demux_ops_register(&demux_ops_mp3);
}

/**
 * @brief  regist demux for mp4
 * @return 0/-1
 */
int demux_register_mp4()
{
    extern struct demux_ops demux_ops_mp4;
    return demux_ops_register(&demux_ops_mp4);
}

/**
 * @brief  regist demux for rawaudio
 * @return 0/-1
 */
int demux_register_rawaudio()
{
    extern struct demux_ops demux_ops_rawaudio;
    return demux_ops_register(&demux_ops_rawaudio);
}

/**
 * @brief  regist demux for adts
 * @return 0/-1
 */
int demux_register_adts()
{
    extern struct demux_ops demux_ops_adts;
    return demux_ops_register(&demux_ops_adts);
}

/**
 * @brief  regist demux for flac
 * @return 0/-1
 */
int demux_register_flac()
{
    extern struct demux_ops demux_ops_flac;
    return demux_ops_register(&demux_ops_flac);
}

/**
 * @brief  regist demux for asf
 * @return 0/-1
 */
int demux_register_asf()
{
    extern struct demux_ops demux_ops_asf;
    return demux_ops_register(&demux_ops_asf);
}

/**
 * @brief  regist demux for amr
 * @return 0/-1
 */
int demux_register_amr()
{
    extern struct demux_ops demux_ops_amr;
    return demux_ops_register(&demux_ops_amr);
}


