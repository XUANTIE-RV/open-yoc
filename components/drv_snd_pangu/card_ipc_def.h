/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef _CARD_IPC_DEF_H_
#define _CARD_IPC_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CARD_IPC_ID (0x40)
#define CARD_DCACHE_OFFSET (16)

typedef enum {
    CARD_INIT_CMD,
    CARD_DEINIT_CMD,
    CARD_OPEN_CMD,
    CARD_CLOSE_CMD,

    PCMP_INIT_CMD,
    PCMP_DEINIT_CMD,
    PCMP_OPEN_CMD,
    PCMP_CLOSE_CMD,
    PCMP_PARAM_SET_CMD,
    PCMP_WRITE_CMD,
    PCMP_REMAIN_SIZE_CMD,
    PCMP_PAUSE_CMD,

    MIXER_SET_GAIN_CMD,

    PCMP_WRITE_PERIOD_CMD,
    PCMP_WRITE_EMPTY_CMD,
    PCMP_READ_PERIOD_CMD,
    PCMP_READ_FULL_CMD,
} card_ipc_msg_id_t;


#ifdef __cplusplus
}
#endif

#endif