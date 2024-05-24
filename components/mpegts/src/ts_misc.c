/*
* Copyright (C) 2018-2023 Alibaba Group Holding Limited
*/
#ifndef TSMISC_HPP
#define TSMISC_HPP
#include <mpegts/ts_misc.h>

static struct
{
    ES_TYPE_ID   es_type;
    uint8_t      id;
    uint8_t      type;
} _es_maps[] =
{
    {AUDIO_MPEG1, STREAM_ID_AUDIO_STREAM},
    {AUDIO_AC3, STREAM_ID_AUDIO_STREAM},
    {AUDIO_ACC, STREAM_ID_AUDIO_STREAM},
    {VIDEO_H264, STREAM_ID_VIDEO_STREAM},
    {VIDEO_H265, STREAM_ID_VIDEO_STREAM},
    {VIDEO_MPEG2, STREAM_ID_VIDEO_STREAM},
    {AUDIO_G711, STREAM_ID_AUDIO_STREAM},
};

/**
* @brief  get stream ID by es type
* @param  [in] type
* @return 0 on error
*/
uint8_t ts_get_stream_id(ES_TYPE_ID type)
{
    for (int i = 0; i < ARRAY_SIZE(_es_maps); i++)
    {
        if (type == _es_maps[i].es_type)
            return _es_maps[i].id;
    }

    return 0;
}

/**
* @brief  crc32(x32+x26+x23+x22+x16+x12+x11+x10+x8+x7+x5+x4+x2+x+1), CRC_32_IEEE
* @param  [in] data
* @param  [in] len
* @return
*/
uint32_t ts_crc32(const uint8_t *data, size_t len)
{
    int i;
    uint32_t crc = 0xffffffff;

    while (len--)
    {
        crc ^= (uint32_t)(*data++) << 24;

        for (i = 0; i < 8; ++i)
        {
            if (crc & 0x80000000)
                crc = (crc << 1) ^ 0x04C11DB7;
            else
                crc <<= 1;
        }
    }
    return crc;
}
#endif

/**
 * @brief  read pts or dts
 * @param  [in] data[5]
 * @return
 */
uint64_t ts_read_pts(const uint8_t data[5])
{
    uint32_t val = 0;
    uint64_t pts = 0;

    val = data[0];
    pts |= ((val >> 1) & 0x07) << 30;

    val = (data[1] << 8) | data[2];
    pts |= ((val >> 1) & 0x7fff) << 15;

    val = (data[3] << 8) | data[4];
    pts |= ((val >> 1) & 0x7fff);

    return pts;
}

/**
* @brief  write pts or dts
* @param  [in] data[5]
* @param  [in] pts
* @param  [in] fill_flag
* @return
*/
void ts_write_pts(uint8_t data[5], uint64_t pts, uint32_t fill_flag)
{
    uint32_t val;

    data[0] = fill_flag << 4 | (((pts >> 30) & 0x07) << 1) | 1;

    val = (((pts >> 15) & 0x7fff) << 1) | 1;
    data[1] = (val >> 8) & 0xff;
    data[2] = val & 0xff;

    val = ((pts & 0x7fff) << 1) | 1;
    data[3] = (val >> 8) & 0xff;
    data[4] = val & 0xff;
}

/**
* @brief  write pcr adaption filed
* @param  [in] data[6]
* @param  [in] pcr
* @return
*/
void ts_write_pcr(uint8_t data[6], uint64_t pcr)
{
    data[0] = (int8_t)(pcr >> 25);
    data[1] = (int8_t)(pcr >> 17);
    data[2] = (int8_t)(pcr >> 9);
    data[3] = (int8_t)(pcr >> 1);
    data[4] = (int8_t)(pcr << 7 | 0x7e);
    data[5] = 0;
}

