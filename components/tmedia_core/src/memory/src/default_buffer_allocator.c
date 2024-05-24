/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#if defined(__linux__) && !defined(PLATFORM_X86_64) // PC Linux does NOT need dmabuf support
#include <sys/mman.h>
#include "vdmabuf.h"
#include <tmedia_config.h>
#endif

#include <stdatomic.h>
#include <stdbool.h>
#include <tmedia_core/memory/default_buffer_allocator.h>
#include <stdio.h>

static int Default_Buffer_Alloc(void *ctx, TMBufferInfo *buf);
static int Default_Buffer_Free(void *ctx, TMBufferInfo *buf);

#if defined(__linux__) && !defined(PLATFORM_X86_64)
static int Default_Buffer_Mmap(void *ctx, TMBufferInfo *bufInfo);
static int Default_Buffer_Munmap(void *ctx, TMBufferInfo *buf);
#endif

typedef struct
{
    atomic_bool inited;
#if defined(__linux__) && !defined(PLATFORM_X86_64)
    int vd_fd;
#endif
    TMBufferAllocator *allocatorFuncTbl;
} DefaultAllocatorCtx;

static DefaultAllocatorCtx _defaultAllocatorCtx =
{
    false,
#if defined(__linux__) && !defined(PLATFORM_X86_64)
    -1,
#endif
    NULL
};

static TMBufferAllocator defaultBufferAllocator =
{
    .alloc  = Default_Buffer_Alloc,
    .free   = Default_Buffer_Free,
#if defined(__linux__) && !defined(PLATFORM_X86_64)
    .mmap   = Default_Buffer_Mmap,
    .munmap = Default_Buffer_Munmap
#endif
};


int DefaultAllocator_Init()
{
    if (atomic_load(&_defaultAllocatorCtx.inited))
    {
        printf(" default allocator has already init");
        return 0;
    }

    memset(&_defaultAllocatorCtx, 0, sizeof(DefaultAllocatorCtx));
#if defined(__linux__) && !defined(PLATFORM_X86_64)
    _defaultAllocatorCtx.vd_fd = -1;

    int vdFD = vd_dev_open();  //open virtio-dmabuf device handle
    if (vdFD < 0)
    {
        vdFD = -1;
    }

    _defaultAllocatorCtx.vd_fd = vdFD;
#endif
    _defaultAllocatorCtx.allocatorFuncTbl = &defaultBufferAllocator;
    atomic_store(&_defaultAllocatorCtx.inited, true);

    return 0;
}

void DefaultAllocator_DeInit()
{
    if (!atomic_load(&_defaultAllocatorCtx.inited))
    {
        printf(" default allocator without init");
        return;
    }

#if defined(__linux__) && !defined(PLATFORM_X86_64)
    if (_defaultAllocatorCtx.vd_fd >= 0)
    {
        vd_dev_close(_defaultAllocatorCtx.vd_fd);
        _defaultAllocatorCtx.vd_fd = -1;
    }
#endif
    _defaultAllocatorCtx.allocatorFuncTbl = NULL;
    atomic_store(&_defaultAllocatorCtx.inited, false);
}

TMBufferAllocator *DefaultBufferAllocator_Get()
{
    if (!atomic_load(&_defaultAllocatorCtx.inited))
    {
        int ret = DefaultAllocator_Init();
        if(ret != 0)
        {
            printf("Default Allocator Init Failed!\n");
            return NULL;
        }
    }

    return _defaultAllocatorCtx.allocatorFuncTbl;
}


int Default_Buffer_Alloc(void *ctx, TMBufferInfo *bufInfo)
{
    if (bufInfo->type == TMBUFFER_TYPE_USER_MALLOC)
    {
        bufInfo->data = (uint8_t *)malloc(bufInfo->size);
        if (!bufInfo->data)
        {
            printf(" bufInfo->data malloc fail");
            return -1;
        }
    }
#if defined(__linux__) && !defined(PLATFORM_X86_64)
    else if (bufInfo->type == TMBUFFER_TYPE_DMA ||
             bufInfo->type == TMBUFFER_TYPE_CARVEOUT)
    {

        uint32_t flags = 0;
        int dma_fd;
        uint64_t vd_id;
        dma_buf_heap_type_e dmaHeapType;
        int ret;
        int carvout_id;

        if (_defaultAllocatorCtx.vd_fd == -1)
        {
            printf("virtio-dmabuf device fd is -1 \n");
            return -1;
        }

        carvout_id = 0;

        if (bufInfo->type == TMBUFFER_TYPE_CARVEOUT)
        {
            dmaHeapType = DMA_BUF_HEAP_TYPE_CARVEOUT;

            carvout_id = (bufInfo->flags & 0xFFFF0000) >> 16;
        }
        else     /* TMBUFFER_TYPE_DMA */
        {
            if (bufInfo->flags & TMBUFFER_FLAGS_CONTIG)
            {
                dmaHeapType = DMA_BUF_HEAP_TYPE_SYSTEM_CONTIG;
            }
            else
            {
                dmaHeapType = DMA_BUF_HEAP_TYPE_SYSTEM;
            }
        }


        if ((bufInfo->flags & TMBUFFER_FLAGS_CACHE_MASK)  == TMBUFFER_FLAGS_UNCACHED)   /* with uncached flag */
        {
            flags |= DMA_BUF_HEAP_FLAG_NON_CACHED;
        }

#ifdef PLATFORM_LIGHT
        /* Hardware on Light such as vpu,g2d,ISP does not support memory > 4GB */
        flags |= DMA_BUF_HEAP_FLAG_DMA32;
#endif

        //if (bufInfo->flags & TMBUFFER_FLAGS_WC){
        //}


        ret = vd_dma_buf_alloc(_defaultAllocatorCtx.vd_fd, bufInfo->size,
                               1, dmaHeapType, flags, carvout_id, &dma_fd, &vd_id);
        if (ret != 0)
        {
            printf(" vd alloc fail,ret:%d\n", ret);
            return -1;
        }

        bufInfo->fd = dma_fd;
        bufInfo->extId = vd_id;
    }
#endif
    else     //not support
    {
        return -1;
    }

    return 0;
}

int Default_Buffer_Free(void *ctx, TMBufferInfo *bufInfo)
{
    if (bufInfo->type  == TMBUFFER_TYPE_USER_MALLOC)
    {
        free(bufInfo->data);
    }
#if defined(__linux__) && !defined(PLATFORM_X86_64)
    else if (bufInfo->type == TMBUFFER_TYPE_DMA ||
             bufInfo->type == TMBUFFER_TYPE_CARVEOUT)
    {
        if (_defaultAllocatorCtx.vd_fd == -1)
        {
            printf("virtio-dmabuf device fd is -1 \n");
            return -1;
        }

        close(bufInfo->fd);
        bufInfo->fd = -1;
    }
#endif

    bufInfo->data = NULL;

    return 0;
}

#if defined(__linux__) && !defined(PLATFORM_X86_64)
int Default_Buffer_Mmap(void *ctx, TMBufferInfo *bufInfo)
{
    void *virAddr = NULL;

    if (bufInfo->fd >= 0)
    {
        virAddr = mmap(NULL, bufInfo->size, PROT_READ | PROT_WRITE, MAP_SHARED, bufInfo->fd, 0);
    }
    else
    {
        printf("bufInfo->fd is invalid \n");

        return -1;
    }

    bufInfo->data = virAddr;

    return 0;
}

int Default_Buffer_Munmap(void *ctx, TMBufferInfo *buf)
{

    if (buf->data)
    {
        munmap(buf->data, buf->size);
        return 0;
    }

    printf("munmap fail.bufInfo->data is NULL \n");

    return -1;
}
#endif
