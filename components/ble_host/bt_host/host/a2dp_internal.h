/** @file
 * @brief Advance Audio Distribution Profile Internal header.
 */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 * Copyright (c) 2022 Alibaba Group Holding Limited
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __A2DP_INTERNAL_H__
#define __A2DP_INTERNAL_H__

#define BT_A2DP_VERSION  0x103

#define BT_A2DP_SBC_MAX_CHANNELS 2
#define BT_A2DP_SBC_MAX_BANDS    8
#define BT_A2DP_SBC_MAX_BLOCKS   16

/** @brief SBC media payload header
 * revert struct member in byte for memory order.
 * so that the memory in packet could map to the structure.
 */
struct bt_a2dp_sbc_payload_hdr {
    /** Number of frames */
    u8_t nof : 4;
    /** last packet of a fragmented frame flag */
    u8_t last : 1;
    /** first packet of a fragmented frame flag */
    u8_t first : 1;
    /** frame is fragmented flag */
    u8_t frag : 1;
} __packed;

struct bt_a2dp {
    struct bt_avdtp session;
};

#endif /* __A2DP_INTERNAL_H__ */

