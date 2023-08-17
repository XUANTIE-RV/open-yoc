/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_AUDIO_INFO_H
#define TM_AUDIO_INFO_H


#define AUDIO_ALIGN_DEFAULT   32
#define ADUIO_ALIGN_MAX       1024
#define AUDIO_ALIGN(x, a)     (((x) + ((a)-1)) & ~((a)-1))

typedef enum {
    AUDIO_SAMPLE_BITS_UNKNOW    = 0, 
    AUDIO_SAMPLE_BITS_8BIT,
    AUDIO_SAMPLE_BITS_16BIT,
} AudioSampleBits_e;

typedef enum {
    AUDIO_SAMPLE_CHANNE_UNKNOW    = 0, 
    AUDIO_SAMPLE_CHANNEL_MONO,
    AUDIO_SAMPLE_CHANNEL_STEREO,
} AudioSampleChannels_e;

typedef enum {
    AUDIO_PCM_ACCESS_RW_UNKNOW    = 0,
    AUDIO_PCM_ACCESS_RW_INTERLEAVED,
    AUDIO_PCM_ACCESS_RW_NONINTERLEAVED,
} AudioPcmDataType_e;

typedef enum {
    G711_TYPE_UNKNOW    = 0,
    G711_TYPE_A,
    G711_TYPE_U,
} G711CodecType_e;

#endif  /* TM_AUDIO_INFO_H */
