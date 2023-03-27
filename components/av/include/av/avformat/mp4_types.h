/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __MP4_TYPES_H__
#define __MP4_TYPES_H__

#include "av/avutil/common.h"

__BEGIN_DECLS__

#define ATOMID(a, b, c, d)     ((a) | ((b) << 8) | ((c) << 16) | ((d) << 24))
#define ATOM_FTYP_SIZE_MAX     (32)

#define MP4_ES_DESCR_TAG                    (0x03)
#define MP4_DEC_CONFIG_DESCR_TAG            (0x04)
#define MP4_DEC_SPECIFIC_DESCR_TAG          (0x05)

typedef struct {
    uint32_t type;
    int64_t  size;
} mp4_atom_t;

typedef struct {
    int32_t count;
    int32_t duration;
} mp4_stts_t;

typedef struct {
    int32_t first;
    int32_t count;
    int32_t id;
} mp4_stsc_t;

__END_DECLS__

#endif /* __MP4_TYPES_H__ */

