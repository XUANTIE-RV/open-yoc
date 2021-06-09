/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/byte_rw.h"
#include "avformat/ts_rw.h"

static const es_map_t _es_maps[] = {
    { 0x03, AVMEDIA_TYPE_AUDIO, AVCODEC_ID_MP3        },
    { 0x04, AVMEDIA_TYPE_AUDIO, AVCODEC_ID_MP3        },
    { 0x0f, AVMEDIA_TYPE_AUDIO, AVCODEC_ID_AAC        },
    { 0x1c, AVMEDIA_TYPE_AUDIO, AVCODEC_ID_AAC        },
};

/**
 * @brief  get codecid by es type
 * @param  [in] es_type
 * @return
 */
avcodec_id_t get_codecid_by_es(uint32_t es_type)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(_es_maps); i++) {
        if (es_type == _es_maps[i].es_type)
            return _es_maps[i].codec_id;
    }

    return AVCODEC_ID_UNKNOWN;
}

/**
 * @brief  get media type by es type
 * @param  [in] es_type
 * @return
 */
avmedia_type_t get_media_type_by_es(uint32_t es_type)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(_es_maps); i++) {
        if (es_type == _es_maps[i].es_type)
            return _es_maps[i].media_type;
    }

    return AVMEDIA_TYPE_UNKNOWN;
}

/**
 * @brief  get ts header info
 * @param  [in] buf
 * @param  [in] hdr
 * @return 0/-1
 */
int ts_hdr_get(const uint8_t* buf, struct ts_hdr *hdr)
{
    int rc = -1;

    CHECK_PARAM(buf && hdr, -1);
    memset(hdr, 0, sizeof(struct ts_hdr));
    if (buf[0] == 0x47) {
        hdr->sync_byte                    = buf[0];
        hdr->transport_error_indicator    = buf[1] >> 7;
        hdr->payload_unit_start_indicator = (buf[1] >> 6) & 0x01;
        hdr->transport_priority           = (buf[1] >> 5) & 0x01;
        hdr->pid                          = ((buf[1] & 0x1F) << 8) | buf[2];
        hdr->transport_scrambling_control = buf[3] >> 6;
        hdr->adaption_field_control       = (buf[3] >> 4) & 0x03;
        hdr->continuity_counter           = buf[3] & 0x0F;
        rc                                = 0;
    }

    return rc;
}


