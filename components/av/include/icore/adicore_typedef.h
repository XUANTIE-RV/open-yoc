/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __ADICORE_TYPEDEF_H__
#define __ADICORE_TYPEDEF_H__

#include <aos/aos.h>

__BEGIN_DECLS__

enum {
    ICORE_CODEC_ID_UNKNOWN = 100,
    ICORE_CODEC_ID_MP3,
    ICORE_CODEC_ID_AAC,
};

typedef struct {
    int         id;                // ICORE_CODEC_ID
    //sf_t        sf;              // if needed
    uint8_t     *extradata;        // extradata or frame header, if needed
    size_t      extradata_size;    // size of the spec data if exist
} adih_t;

__END_DECLS__

#endif /* __ADICORE_TYPEDEF_H__ */

