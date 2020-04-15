/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AOICORE_INTERNAL_H__
#define __AOICORE_INTERNAL_H__

#include "avutil/sf.h"
#include "icore/icore_msg.h"
#include "aoicore_typedef.h"

__BEGIN_DECLS__

#ifdef CONFIG_CHIP_PANGU
#define AOICORE_AP_IDX          (0)
#define AOICORE_CP_IDX          (1)
#else
#define AOICORE_AP_IDX          (1)
#define AOICORE_CP_IDX          (2)
#endif
#define IPC_CMD_AOICORE  (56)
#define AOICORE_IPC_SERIVCE_ID (0x11)

enum {
    ICORE_CMD_AO_INVALID,
    ICORE_CMD_AO_OPEN,
    ICORE_CMD_AO_START,
    ICORE_CMD_AO_WRITE,
    ICORE_CMD_AO_DRAIN,
    ICORE_CMD_AO_STOP,
    ICORE_CMD_AO_CLOSE,

    //TODO
    ICORE_CMD_VOL_SET,
};

typedef struct {
    aoih_t    ash;      // req
    void      *ao;      // resp: ao_cls_t
} aoicore_open_t;

typedef struct {
    void      *ao;      // ao_cls_t
} aoicore_close_t;

typedef aoicore_close_t aoicore_start_t;
typedef aoicore_close_t aoicore_stop_t;
typedef aoicore_close_t aoicore_drain_t;

typedef struct {
    void              *ao;      // ao_cls_t
    uint8_t           *buf;
    size_t           count;
} aoicore_write_t;

typedef struct {
    size_t           vol; // 0-100
} volicore_set_t;

__END_DECLS__

#endif /* __AOICORE_INTERNAL_H__ */

