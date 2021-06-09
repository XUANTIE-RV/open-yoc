/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avcodec/ad.h"
#include "avcodec/avcodec.h"
#include "avcodec/avcodec_all.h"

/**
 * @brief  regist ad for pcm
 * @return 0/-1
 */
int ad_register_pcm()
{
    extern struct ad_ops ad_ops_pcm;
    return ad_ops_register(&ad_ops_pcm);
}

/**
 * @brief  regist ad for ipc
 * @return 0/-1
 */
int ad_register_ipc()
{
    extern struct ad_ops ad_ops_ipc;
    return ad_ops_register(&ad_ops_ipc);
}

/**
 * @brief  regist ad for tmall
 * @return 0/-1
 */
int ad_register_tmall()
{
    extern struct ad_ops ad_ops_tmall;
    return ad_ops_register(&ad_ops_tmall);
}

/**
 * @brief  regist ad for pvmp3
 * @return 0/-1
 */
int ad_register_pvmp3()
{
    extern struct ad_ops ad_ops_pvmp3;
    return ad_ops_register(&ad_ops_pvmp3);
}

/**
 * @brief  regist ad for adpcm_ms
 * @return 0/-1
 */
int ad_register_adpcm_ms()
{
    extern struct ad_ops ad_ops_adpcm_ms;
    return ad_ops_register(&ad_ops_adpcm_ms);
}

/**
 * @brief  regist ad for flac
 * @return 0/-1
 */
int ad_register_flac()
{
    extern struct ad_ops ad_ops_flac;
    return ad_ops_register(&ad_ops_flac);
}

/**
 * @brief  regist ad for amrnb
 * @return 0/-1
 */
int ad_register_amrnb()
{
    extern struct ad_ops ad_ops_amrnb;
    return ad_ops_register(&ad_ops_amrnb);
}

/**
 * @brief  regist ad for amrwb
 * @return 0/-1
 */
int ad_register_amrwb()
{
    extern struct ad_ops ad_ops_amrwb;
    return ad_ops_register(&ad_ops_amrwb);
}

/**
 * @brief  regist ad for opus
 * @return 0/-1
 */
int ad_register_opus()
{
    extern struct ad_ops ad_ops_opus;
    return ad_ops_register(&ad_ops_opus);
}

/**
 * @brief  regist ad for speex
 * @return 0/-1
 */
int ad_register_speex()
{
    extern struct ad_ops ad_ops_speex;
    return ad_ops_register(&ad_ops_speex);
}

/**
 * @brief  regist ad for alaw
 * @return 0/-1
 */
int ad_register_alaw()
{
    extern struct ad_ops ad_ops_alaw;
    return ad_ops_register(&ad_ops_alaw);
}

/**
 * @brief  regist ad for mulaw
 * @return 0/-1
 */
int ad_register_mulaw()
{
    extern struct ad_ops ad_ops_mulaw;
    return ad_ops_register(&ad_ops_mulaw);
}

