/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "av/avutil/av_typedef.h"
#include "av/avutil/byte_rw.h"
#include "av/avutil/bio.h"
#include "av/avformat/ogg_rw.h"

static struct ogg_codec {
    const char            *magic;
    int                   magic_size;
    avcodec_id_t          id;
} _ogg_codecs[] = {
    { "Speex   ",    8,   AVCODEC_ID_SPEEX       },
    { "OpusHead",    8,   AVCODEC_ID_OPUS        },
};

/**
 * @brief  find codec support for ogg-format now
 * @param  [in] buf
 * @param  [in] size
 * @return AVCODEC_ID_UNKNOWN on error
 */
avcodec_id_t ogg_find_codec(const uint8_t *buf, size_t size)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(_ogg_codecs); i++) {
        if ((size >= _ogg_codecs[i].magic_size) &&
            (memcmp(buf, _ogg_codecs[i].magic, _ogg_codecs[i].magic_size) == 0))
            return _ogg_codecs[i].id;
    }

    return AVCODEC_ID_UNKNOWN;
}

/**
 * @brief  get ogg header info
 * @param  [in] buf
 * @param  [in] hinfo
 * @return 0/-1
 */
int page_hdr_get(const uint8_t buf[OGG_HDR_LEN], struct page_hdr *hinfo)
{
    bio_t bio;

    CHECK_PARAM(buf && hinfo, -1);
    memset(hinfo, 0, sizeof(struct page_hdr));
    if (strncmp((const char*)buf, "OggS", 4)) {
        return -1;
    }
    bio_init(&bio, (uint8_t*)buf + 4, OGG_HDR_LEN);

    hinfo->version   = bio_r8(&bio);
    hinfo->type      = bio_r8(&bio);
    hinfo->granule   = bio_r64le(&bio);
    hinfo->serial_nb = bio_r32le(&bio);
    hinfo->seq_nb    = bio_r32le(&bio);
    hinfo->crc       = bio_r32le(&bio);
    hinfo->nb_segs   = bio_r8(&bio);
    if (!hinfo->nb_segs) {
        return -1;
    }

    return 0;
}

/**
 * @brief  sync the ogg
 * @param  [in] rcb        : read byte callback
 * @param  [in] opaque     : in param of the callback
 * @param  [in] sync_max   : max of the sync count
 * @param  [in/out] hdr[OGG_HDR_LEN] : hdr of the ogg
 * @param  [in/out] hinfo
 * @return -1 on error or read byte count for sync
 */
int ogg_page_sync(read_bytes_t rcb, void *opaque, size_t sync_max, uint8_t hdr[OGG_HDR_LEN], struct page_hdr *hinfo)
{
    int rc = -1;
    int sync_cnt = 0, max;

    CHECK_PARAM(rcb && opaque && hdr && hinfo, -1);
    max = sync_max ? sync_max : INT32_MAX;
resync:
    while (sync_cnt < max) {
        rc = page_hdr_get(hdr, hinfo);
        if (rc < 0) {
            memmove(&hdr[0], &hdr[1], OGG_HDR_LEN - 1);
            if (rcb(opaque, &hdr[OGG_HDR_LEN - 1], 1) <= 0)
                break;
            sync_cnt++;
            goto resync;
        }
        break;
    }

    return rc < 0 ? rc : sync_cnt;
}


