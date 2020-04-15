/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/byte_rw.h"
#include "avutil/bio.h"
#include "avformat/flac_rw.h"

static const int _rate_table[16] = {
    0, 88200, 176400, 192000, 8000, 16000, 22050, 24000,
    32000, 44100, 48000, 96000, 0, 0, 0, 0
};

static const int32_t _blocksize_table[16] = {
    0, 192, 576<<0, 576<<1, 576<<2, 576<<3, 0, 0,
    256<<0, 256<<1, 256<<2, 256<<3, 256<<4, 256<<5, 256<<6, 256<<7
};

static const int8_t _bits_table[] = {
    0, 8, 12, 0, 16, 20, 24, 0
};

/**
 * @brief  get the header from the buf
 * @param  [in] buf
 * @param  [in] hinfo
 * @return 0/-1
 */
int flac_hdr_get(const uint8_t buf[FLAC_HDR_SIZE_MAX], struct flac_hdr *hinfo)
{
    bio_t bio;
    uint32_t val;
    int64_t number;
    int bs_code, ch_code, sr_code, bits_code, t;

    CHECK_PARAM(buf && hinfo, -1);
    bio_init(&bio, (uint8_t*)buf, FLAC_HDR_SIZE_MAX);
    val = bio_r16be(&bio);
    /* syncword */
    if ((val & 0xfff8) != 0xfff8) {
        return -1;
    }

    memset(hinfo, 0, sizeof(struct flac_hdr));
    hinfo->is_var = val & 0x1;
    val = bio_r8(&bio);
    bs_code = (val >> 4) & 0xf;
    if (bs_code == 0)
        return -1;
    sr_code = val & 0xf;

    val = bio_r8(&bio);
    ch_code = (val >> 4) & 0xf;
    if (ch_code < 8) {
        hinfo->channels = ch_code + 1;
    } else if (ch_code < 11) {
        hinfo->channels = 2;
    } else
        return -1;

    bits_code = (val >> 1) & 0x7;
    if (bits_code == 3 || bits_code == 7)
        return -1;
    else
        hinfo->bits = _bits_table[bits_code];

    if (val & 0x1)
        return -1;

    /* nb_of frame or sample, utf-8 -> unicode */
    number = bio_r8(&bio);
    val = (number & 128) >> 1;
    if ((val & 0xc0) == 0x80 || val >= 0xFE)
        return -1;
    while (val & number) {
        t = bio_r8(&bio) - 128;
        if (t >> 6)
            return -1;
        number = (number << 6) + t;
        val =  val << 5;
    }
    number &= (val << 1) - 1;
    if (number < 0)
        return -1;
    hinfo->nb_frame_or_sample = number;

    if (bs_code == 0)
        return -1;
    else if (bs_code == 6)
        hinfo->block_size = bio_r8(&bio) + 1;
    else if (bs_code == 7)
        hinfo->block_size = bio_r16be(&bio) + 1;
    else
        hinfo->block_size = _blocksize_table[bs_code];

    if (sr_code < 12)
        hinfo->rate = _rate_table[sr_code];
    else if (sr_code == 12)
        hinfo->rate = bio_r8(&bio) * 1000;
    else if (sr_code == 13)
        hinfo->rate = bio_r16be(&bio);
    else if (sr_code == 14)
        hinfo->rate = bio_r16be(&bio) * 10;
    else
        return -1;

    return 0;
}

/**
 * @brief  parse the stream info of flac
 * @param  [in] buf
 * @param  [in] si
 * @return 0/-1
 */
int flac_parse_si(const uint8_t buf[FLAC_STREAMINFO_SIZE], struct flac_stream_info *si)
{
    bio_t bio;
    uint64_t val;

    memset(si, 0, sizeof(struct flac_stream_info));
    bio_init(&bio, (uint8_t*)buf, FLAC_STREAMINFO_SIZE);
    si->bsize_min = bio_r16be(&bio);
    si->bsize_max = bio_r16be(&bio);
    if (!(si->bsize_min && si->bsize_max && si->bsize_max >= si->bsize_min)) {
        return -1;
    }
    si->fsize_min  = bio_r24be(&bio);
    si->fsize_max  = bio_r24be(&bio);
    val            = bio_r24be(&bio);
    si->rate       = val >> 4;
    si->channels   = ((val >> 1) & 0x7) + 1;
    si->bits       = (val & 0x1);
    val            = bio_r64be(&bio);
    si->bits       = ((si->bits << 4) | (val >> 60)) + 1;
    if (!(si->rate && si->channels && si->bits)) {
        return -1;
    }
    si->nb_samples = (val >> 24) & ((1ULL << 36) - 1);

    return 0;
}

/**
 * @brief  sync the flac
 * @param  [in] rcb      : read byte callback
 * @param  [in] opaque   : in param of the callback
 * @param  [in] sync_max : max of the sync count
 * @param  [in/out] hdr  : hdr of the flac
 * @param  [in/out] hinfo
 * @return -1 on error or read byte count for sync
 */
int flac_sync(read_bytes_t rcb, void *opaque, size_t sync_max, uint8_t hdr[FLAC_HDR_SIZE_MAX], struct flac_hdr *hinfo)
{
    int rc = -1;
    int sync_cnt = 0, max;

    CHECK_PARAM(rcb && opaque && hdr && hinfo, -1);
    max = sync_max ? sync_max : INT32_MAX;
resync:
    while (sync_cnt < max) {
        rc = flac_hdr_get(hdr, hinfo);
        if (rc < 0) {
            memmove(&hdr[0], &hdr[1], FLAC_HDR_SIZE_MAX - 1);
            if (rcb(opaque, &hdr[FLAC_HDR_SIZE_MAX - 1], 1) <= 0)
                break;
            sync_cnt++;
            goto resync;
        }
        break;
    }

    return rc < 0 ? rc : sync_cnt;
}


