/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#ifndef __AOICORE_TYPEDEF_H__
#define __AOICORE_TYPEDEF_H__

#include <aos/aos.h>
#include "avutil/sf.h"

__BEGIN_DECLS__

enum {
    ICORE_AO_ID_UNKNOWN = 100,
    ICORE_AO_ID_ALSA,
};

typedef struct {
    int         id;                // ICORE_AO_ID_XX
    sf_t        sf;
} aoih_t;

__END_DECLS__

#endif /* __AOICORE_TYPEDEF_H__ */

