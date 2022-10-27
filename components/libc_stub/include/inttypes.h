/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#ifndef _PRIV_INTTYPES_H_
#define _PRIV_INTTYPES_H_
#ifdef __cplusplus
extern "C" {
#endif

#include_next <inttypes.h>

#undef PRId32
#undef PRIi32
#undef PRIo32
#undef PRIu32
#undef PRIx32
#undef PRIX32

#undef SCNd32
#undef SCNi32
#undef SCNo32
#undef SCNu32
#undef SCNx32

#define PRId32		__STRINGIFY(d)
#define PRIi32		__STRINGIFY(i)
#define PRIo32		__STRINGIFY(o)
#define PRIu32		__STRINGIFY(u)
#define PRIx32		__STRINGIFY(x)
#define PRIX32		__STRINGIFY(X)

#define SCNd32		__STRINGIFY(d)
#define SCNi32		__STRINGIFY(i)
#define SCNo32		__STRINGIFY(o)
#define SCNu32		__STRINGIFY(u)
#define SCNx32		__STRINGIFY(x)

#ifdef __cplusplus
}
#endif

#endif
