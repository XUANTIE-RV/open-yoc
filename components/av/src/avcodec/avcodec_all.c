/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "av/avcodec/ad.h"
#include "av/avcodec/avcodec.h"
#include "av/avcodec/avcodec_all.h"

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

/**
 * @brief  regist all decoder
 * attention: can rewrite this function by caller
 * @return 0/-1
 */
__attribute__((weak)) int ad_register_all()
{
#if defined(CONFIG_DECODER_PCM)
    REGISTER_DECODER(PCM, pcm);
#endif
#if defined(CONFIG_DECODER_PVMP3)
    REGISTER_DECODER(PVMP3, pvmp3);
#endif
#if defined(CONFIG_DECODER_ADPCM_MS)
    REGISTER_DECODER(ADPCM_MS, adpcm_ms);
#endif
#if defined(CONFIG_DECODER_FLAC)
    REGISTER_DECODER(FLAC, flac);
#endif
#if defined(CONFIG_DECODER_AMRNB)
    REGISTER_DECODER(AMRNB, amrnb);
#endif
#if defined(CONFIG_DECODER_AMRWB)
    REGISTER_DECODER(AMRWB, amrwb);
#endif
#if defined(CONFIG_DECODER_OPUS)
    REGISTER_DECODER(OPUS, opus);
#endif
#if defined(CONFIG_DECODER_SPEEX)
    REGISTER_DECODER(SPEEX, speex);
#endif
#if defined(CONFIG_DECODER_ALAW)
    REGISTER_DECODER(ALAW, alaw);
#endif
#if defined(CONFIG_DECODER_MULAW)
    REGISTER_DECODER(MULAW, mulaw);
#endif
#if defined(CONFIG_DECODER_IPC)
    REGISTER_DECODER(IPC, ipc);
#endif

    return 0;
}

