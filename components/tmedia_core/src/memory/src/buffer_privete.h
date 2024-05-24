/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifndef TM_BUFFER_PRIVATE_H
#define TM_BUFFER_PRIVATE_H

#include <stddef.h>
#include <stdint.h>
#include <tmedia_core/memory/buffer.h>
#include <tmedia_core/memory/default_buffer_allocator.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct
{
    TMBufferInfo bufInfo;
    TMBufferAllocator *allocator;
    void *ctxAllocator;
    atomic_int refCount;
    bool bExternalMemory;
    bool bMapped;
    TMBufferRefZeroCallBack cbRefZero;
} TMBufferImpl;

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* TM_BUFFER_PRIVATE_H */
