/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <ctype.h>
#include "avutil/misc.h"

static struct {
    const char   *name;
    avcodec_id_t id;
} _codec_maps[] = {
    { "unknown",          AVCODEC_ID_UNKNOWN   },
    { "mp3",              AVCODEC_ID_MP3       },
    { "aac",              AVCODEC_ID_AAC       },
    { "raw pcm",          AVCODEC_ID_RAWAUDIO  },
    { "adpcm_ms",         AVCODEC_ID_ADPCM_MS  },
};

/**
 * @brief  get codec id by tag val
 * @param  [in] tags
 * @param  [in] tag
 * @return
 */
avcodec_id_t get_codec_id(const avcodec_tag_t *tags, uint32_t tag)
{
    int i;

    for (i = 0; tags[i].id != AVCODEC_ID_UNKNOWN; i++) {
        if (tag == tags[i].tag)
            return tags[i].id;
    }

    return AVCODEC_ID_UNKNOWN;
}

/**
 * @brief  get codec name
 * @param  [in] codec id
 * @return "unknown" on error
 */
const char* get_codec_name(avcodec_id_t id)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(_codec_maps); i++) {
        if (id == _codec_maps[i].id)
            return _codec_maps[i].name;
    }

    return "unknown";
}

/**
 * @brief  convert hex string to bytes
 * @param  [in] strhex : hex string
 * @param  [in] bytes
 * @param  [in] ibytes : length of the bytes
 * @return 0/-1
 */
int bytes_from_hex(const char *strhex, uint8_t *bytes, size_t ibytes)
{
    int i, slen;
    uint8_t m = 0, n, c;

    CHECK_PARAM(strhex && bytes, -1);
    slen = strlen(strhex);
    CHECK_PARAM(2 * ibytes >= slen, -1);
    memset(bytes, 0, ibytes);
    for (i = 0; i < slen; i++) {
        c = tolower(strhex[i]);
        if (c >= '0' && c <= '9') {
            n = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            n = c - 'a' + 10;
        } else {
            return -1;
        }

        if (i % 2) {
            bytes[i / 2] = m + n;
        } else {
            m = 16 * n;
        }
    }

    return 0;
}

/**
* @brief  clip a signed integer value into the -32768,32767(short)
* @param  [in] v
* @return
*/
int16_t clip_int16(int v)
{
    if ((v + 0x8000U) & ~0xFFFF)
        return (v >> 31) ^ 0x7FFF;
    else
        return v;
}

/**
* @brief  extend the val to sign int
* @param  [in] val
* @param  [in] bits: bits of the val
* @return
*/
int sign_extend(int val, unsigned bits)
{
    unsigned shift = 8 * sizeof(int) - bits;
    int v = (unsigned) val << shift;
    return v >> shift;
}

/**
 * @brief  four byte to str
 * @param  [in] val
 * @return not null
 */
char* four_2_str(uint32_t val)
{
    int i, rc = 0;
    static char buf[5];
    uint8_t *ptr = (uint8_t*)&val;

    for (i = 0; i < sizeof(val); i++) {
        rc += snprintf(buf + rc, sizeof(buf) -rc, "%c", ptr[i]);
    }

    return buf;
}

