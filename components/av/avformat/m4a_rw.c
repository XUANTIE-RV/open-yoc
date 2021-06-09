/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/byte_rw.h"
#include "avformat/m4a_rw.h"


const static int _m4a_sample_rates[16] = {
    96000, 88200, 64000, 48000, 44100, 32000,
    24000, 22050, 16000, 12000, 11025, 8000, 7350
};

const static uint8_t _m4a_channels[8] = {
    0, 1, 2, 3, 4, 5, 6, 8
};

/**
* @brief  decode the Audio Specific Config
* @param  [in] m4ac
* @param  [in] asc : extradata, Audio Specific Config
* @param  [in] size
* @return 0/-1
*/
int m4a_decode_asc(m4a_cnf_t *m4ac, const uint8_t *asc, size_t size)
{
    //FIXME:
    CHECK_PARAM(m4ac && asc && size, -1);
    memset(m4ac, 0, sizeof(m4a_cnf_t));

    m4ac->object_type = asc[0] >> 3;
    m4ac->sample_idx  = ((asc[0] & 0x7) << 1) | (asc[1] >> 7);
    if (m4ac->sample_idx < ARRAY_SIZE(_m4a_sample_rates))
        m4ac->rate = _m4a_sample_rates[m4ac->sample_idx];
    m4ac->ch_conf     = (asc[1] & 0x78) >> 3;
    if (m4ac->ch_conf < ARRAY_SIZE(_m4a_channels))
        m4ac->channels = _m4a_channels[m4ac->ch_conf];

    if (!(m4ac->object_type && m4ac->rate && m4ac->channels))
        return -1;

    if (m4ac->object_type == AOT_SBR || m4ac->object_type == AOT_PS) {
        m4ac->ps = m4ac->object_type == AOT_PS ? 1 : 0;
        m4ac->sbr = 1;
        m4ac->ext_object_type = AOT_SBR;
        m4ac->ext_sample_idx = ((asc[1] & 0x7) << 1) | (asc[2] >> 7);
        if (m4ac->ext_sample_idx < ARRAY_SIZE(_m4a_sample_rates))
            m4ac->ext_rate = _m4a_sample_rates[m4ac->ext_sample_idx];
        m4ac->object_type = (asc[2] & 0x7c) >> 2;

        if (!(m4ac->ext_object_type && m4ac->object_type && m4ac->ext_rate))
            return -1;
    }


    return 0;
}



