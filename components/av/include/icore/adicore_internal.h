/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __ADICORE_INTERNAL_H__
#define __ADICORE_INTERNAL_H__

#include "avutil/sf.h"
#include "avutil/avframe.h"
#include "avutil/avpacket.h"
#include "icore/icore_msg.h"
#include "adicore_typedef.h"

__BEGIN_DECLS__

#ifdef CONFIG_CHIP_PANGU
#define ADICORE_AP_IDX          (0)
#define ADICORE_CP_IDX          (1)
#else
#define ADICORE_AP_IDX          (1)
#define ADICORE_CP_IDX          (2)
#endif
#define IPC_CMD_ADICORE  (55)
#define ADICORE_IPC_SERIVCE_ID 0x10

enum {
    ICORE_CMD_AD_INVALID,
    ICORE_CMD_AD_OPEN,
    ICORE_CMD_AD_DECODE,
    ICORE_CMD_AD_RESET,
    ICORE_CMD_AD_CLOSE,
};

typedef struct {
    avcodec_id_t  id;
    adi_conf_t    adi_cnf;      // req

    void          *ad;          // resp: ad_cls_t
    sf_t          sf;           // resp:
} adicore_open_t;

typedef struct {
    void          *ad;          // ad_cls_t
} adicore_reset_t;

typedef struct {
    void          *ad;          // ad_cls_t
} adicore_close_t;

typedef struct {
    void              *ad;      // ad_cls_t
    uint8_t           *es_data;
    int32_t           es_len;
    avframe_t         frame;
    int               got_frame;
} adicore_decode_t;

__END_DECLS__

#endif /* __ADICORE_INTERNAL_H__ */

