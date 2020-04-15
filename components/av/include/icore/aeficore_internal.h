/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AEFICORE_INTERNAL_H__
#define __AEFICORE_INTERNAL_H__

#include "icore/icore_msg.h"
#include "aef/aef_cls.h"

__BEGIN_DECLS__

#ifdef CONFIG_CHIP_PANGU
#define AEFICORE_AP_IDX          (0)
#define AEFICORE_CP_IDX          (1)
#else
#define AEFICORE_AP_IDX          (1)
#define AEFICORE_CP_IDX          (2)
#endif

#define IPC_CMD_AEFICORE         (59)
#define AEFICORE_IPC_SERIVCE_ID  (0x14)

enum {
    ICORE_CMD_AEF_INVALID,
    ICORE_CMD_AEF_NEW,
    ICORE_CMD_AEF_PROCESS,
    ICORE_CMD_AEF_FREE,
};

typedef struct {
    uint32_t          rate;
    uint8_t           *conf;
    size_t            conf_size;
    size_t            nsamples_max;
    void              *aef;      // resp: aefx_t
} aeficore_new_t;

typedef struct {
    void              *aef;
    int16_t           *in;
    int16_t           *out;
    size_t            nb_samples;
} aeficore_process_t;

typedef struct {
    void      *aef;
} aeficore_free_t;

__END_DECLS__

#endif /* __AEFICORE_INTERNAL_H__ */

