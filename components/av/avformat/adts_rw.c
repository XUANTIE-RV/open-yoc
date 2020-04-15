/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/byte_rw.h"
#include "avformat/adts_rw.h"

static int aac_freqs[] = { 96000, 88200, 64000, 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 8000, 7350 };

/**
 * @brief  get the header from the buf
 * @param  [in] buf
 * @param  [in] hinfo
 * @return 0/-1
 */
int adts_hdr_get(const uint8_t* buf, struct adts_hdr *hinfo)
{
    uint8_t id, profile, channel, rdb;
    uint32_t value, ifreq, flen, freq;

    CHECK_PARAM(buf && hinfo, -1);
    value = byte_r16be(buf);
    /* syncword */
    if ((value & 0xfff0) != 0xfff0) {
        return -1;
    }
    /* layer */
    if ((value & 0x6) != 0x0) {
        return -1;
    }
    id      = value & 0x8;
    profile = (buf[2] >> 6) & 0x3;
    ifreq   = (buf[2] >> 2) & 0xf;
    if (ifreq >= ARRAY_SIZE(aac_freqs)) {
        return -1;
    }
    freq    = aac_freqs[ifreq];
    value   = byte_r16be(buf + 2);
    channel = (value >> 6) & 0x7;
    if (channel > 8) {
        return -1;
    }

    value = byte_r32be(buf + 3);
    flen  = (value >> 13) & 0x1fff;
    if (flen <= ADTS_HDR_SIZE) {
        return -1;
    }
    rdb = value & 0x3;

    hinfo->id         = id;
    hinfo->flen       = flen;
    hinfo->freq       = freq;
    hinfo->profile    = profile;
    hinfo->channel    = channel;
    hinfo->nb_samples = (rdb + 1) * 1024;
    hinfo->bps        = flen * 8 / (1.0 * hinfo->nb_samples / freq);

    return 0;
}

/**
 * @brief  sync the adts
 * @param  [in] rcb      : read byte callback
 * @param  [in] opaque   : in param of the callback
 * @param  [in] sync_max : max of the sync count
 * @param  [in/out] hdr  : hdr of the adts
 * @param  [in/out] hinfo
 * @return -1 on error or read byte count for sync
 */
int adts_sync(read_bytes_t rcb, void *opaque, size_t sync_max, uint8_t hdr[ADTS_HDR_SIZE], struct adts_hdr *hinfo)
{
    int rc = -1;
    int sync_cnt = 0, max;

    CHECK_PARAM(rcb && opaque && hdr && hinfo, -1);
    max = sync_max ? sync_max : INT32_MAX;
resync:
    while (sync_cnt < max) {
        rc = adts_hdr_get(hdr, hinfo);
        if (rc < 0) {
            memmove(&hdr[0], &hdr[1], ADTS_HDR_SIZE - 1);
            if (rcb(opaque, &hdr[ADTS_HDR_SIZE - 1], 1) <= 0)
                break;
            sync_cnt++;
            goto resync;
        }
        break;
    }

    return rc < 0 ? rc : sync_cnt;
}



