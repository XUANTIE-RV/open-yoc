/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/byte_rw.h"
#include "avformat/mp3_rw.h"

static const uint16_t tabsel_123[2][3][16] = {
    {
        {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,0},
        {0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,0},
        {0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,0}
    },

    {
        {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,0},
        {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0},
        {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}
    }
};

static const int freqs[9] = {
    44100, 48000, 32000,   // MPEG 1.0
    22050, 24000, 16000,   // MPEG 2.0
    11025, 12000,  8000
};  // MPEG 2.5

/**
 * @brief  get mp3 header info
 * @param  [in] buf
 * @param  [in] hinfo
 * @return 0/-1
 */
int mp3_hdr_get(const uint8_t* buf, struct mp3_hdr_info *hinfo)
{
    int bitrate;
    int layer, spf;
    int bits = 16;
    uint32_t header;
    int channel, lsf, framesize, padding;
    int bitrate_index,sampling_frequency, divisor;
    const int mult[3] = { 12000, 144000, 144000 };

    CHECK_PARAM(buf && hinfo, -1);
    memset(hinfo, 0, sizeof(struct mp3_hdr_info));
    header = byte_r32be(buf);
    if ((header & 0xffe00000) != 0xffe00000 ) {
        return -1;
    }

    layer = 4 - ((header >> 17) & 3);
    if (layer == 4) {
        return -1;
    }

    sampling_frequency = (header >> 10) & 0x3;
    if (sampling_frequency == 3) {
        return -1;
    }

    if (header & (1<<20)) {
        // MPEG 1.0 (lsf==0) or MPEG 2.0 (lsf==1)
        lsf = !(header & (1<<19));
        sampling_frequency += lsf * 3;
    } else {
        // MPEG 2.5
        lsf = 1;
        sampling_frequency += 6;
    }

    bitrate_index = (header >> 12) & 0xf;
    padding       = (header >> 9) & 0x1;
    channel       = (((header >> 6) & 0x3) == 3) ? 1 : 2;
    bitrate       = tabsel_123[lsf][layer-1][bitrate_index];
    framesize     = bitrate * mult[layer-1];
    if (!framesize) {
        return -1;
    }

    divisor    = layer == 3 ? (freqs[sampling_frequency] << lsf) : freqs[sampling_frequency];
    framesize /= divisor;
    framesize += padding;
    if (layer == 1)
        framesize *= 4;

    if (layer == 1)
        spf = 384;
    else if (layer == 2)
        spf = 1152;
    else if (sampling_frequency > 2) // not 1.0
        spf = 576;
    else
        spf = 1152;

    hinfo->spf       = spf;
    hinfo->lsf       = lsf;
    hinfo->layer     = layer;
    hinfo->bitrate   = bitrate;
    hinfo->framesize = framesize;
    hinfo->sf        = sf_make_channel(channel) | sf_make_rate(freqs[sampling_frequency]) | sf_make_bit(bits) | sf_make_signed(bits > 8);

    return 0;
}

/**
 * @brief  sync the mp3
 * @param  [in] rcb        : read byte callback
 * @param  [in] opaque     : in param of the callback
 * @param  [in] sync_max   : max of the sync count
 * @param  [in/out] hdr[4] : hdr of the mp3
 * @param  [in/out] hinfo
 * @return -1 on error or read byte count for sync
 */
int mp3_sync(read_bytes_t rcb, void *opaque, size_t sync_max, uint8_t hdr[4], struct mp3_hdr_info *hinfo)
{
    int rc = -1;
    int sync_cnt = 0, max;

    CHECK_PARAM(rcb && opaque && hdr && hinfo, -1);
    max = sync_max ? sync_max : INT32_MAX;
resync:
    while (sync_cnt < max) {
        rc = mp3_hdr_get(hdr, hinfo);
        if (rc < 0) {
            memmove(&hdr[0], &hdr[1], MP3_HDR_LEN - 1);
            if (rcb(opaque, &hdr[MP3_HDR_LEN - 1], 1) <= 0)
                break;
            sync_cnt++;
            goto resync;
        }
        break;
    }

    return rc < 0 ? rc : sync_cnt;
}


