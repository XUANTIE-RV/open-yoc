/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include "avutil/byte_rw.h"
#include "avformat/amr_rw.h"

const uint32_t _nb_fsizes[] = { 12, 13, 15, 17, 19, 20, 26, 31 };
const uint32_t _wb_fsizes[] = { 17, 23, 32, 36, 40, 46, 50, 58, 60 };

/**
 * @brief  get the header from the buf
 * @param  [in] buf
 * @param  [in] amr_type : AMR_TYPE_XXX
 * @param  [in] hinfo
 * @return 0/-1
 */
int amr_hdr_get(const uint8_t* buf, uint8_t amr_type, struct amr_hdr *hinfo)
{
    int rc = -1;
    uint8_t mode;

    CHECK_PARAM(buf && hinfo, -1);
    memset(hinfo, 0, sizeof(struct amr_hdr));
    mode = (buf[0] >> 3) & 0x0f;
    if (amr_type == AMR_TYPE_NB) {
        if (mode < 8) {
            hinfo->scode     = buf[0];
            hinfo->mode      = mode;
            hinfo->framesize = _nb_fsizes[mode] + 1;
            rc               = 0;
        }
    } else if (amr_type == AMR_TYPE_WB) {
        if (mode < 9) {
            hinfo->scode     = buf[0];
            hinfo->mode      = mode;
            hinfo->framesize = _wb_fsizes[mode] + 1;
            rc               = 0;
        }
    }

    return rc;
}

/**
 * @brief  sync the amr
 * @param  [in] rcb      : read byte callback
 * @param  [in] opaque   : in param of the callback
 * @param  [in] sync_max : max of the sync count
 * @param  [in/out] hdr  : hdr of the amr
 * @param  [in] amr_type : AMR_TYPE_XXX
 * @param  [in/out] hinfo
 * @return -1 on error or read byte count for sync
 */
int amr_sync(read_bytes_t rcb, void *opaque, size_t sync_max, uint8_t hdr[AMR_HDR_SIZE], uint8_t amr_type, struct amr_hdr *hinfo)
{
    int rc = -1;
    int sync_cnt = 0, max;

    CHECK_PARAM(rcb && opaque && hdr && hinfo, -1);
    CHECK_PARAM(amr_type == AMR_TYPE_NB || amr_type == AMR_TYPE_WB, -1);
    max = sync_max ? sync_max : INT32_MAX;
resync:
    while (sync_cnt < max) {
        rc = amr_hdr_get(hdr, amr_type, hinfo);
        if (rc < 0) {
            if (rcb(opaque, &hdr[AMR_HDR_SIZE - 1], 1) <= 0)
                break;
            sync_cnt++;
            goto resync;
        }
        break;
    }

    return rc < 0 ? rc : sync_cnt;
}


