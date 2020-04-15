/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/byte_rw.h"
#include "avutil/misc.h"
#include "avformat/riff_rw.h"

static const avcodec_tag_t g_codec_tags_wav[] = {
    { AVCODEC_ID_RAWAUDIO    , WAV_FORMAT_PCM          },
    { AVCODEC_ID_ADPCM_MS    , WAV_FORMAT_ADPCM_MS     },
    { AVCODEC_ID_RAWAUDIO    , WAV_FORMAT_FLOAT        },
    { AVCODEC_ID_WMAV1       , WAV_FORMAT_WMAV1        },
    { AVCODEC_ID_WMAV2       , WAV_FORMAT_WMAV2        },
    { AVCODEC_ID_UNKNOWN     , 0                       },
};

/**
 * @brief  get codec id by tag val
 * @param  [in] tag
 * @return
 */
avcodec_id_t wav_get_codec_id(uint32_t tag)
{
    return get_codec_id(g_codec_tags_wav, tag);
}


