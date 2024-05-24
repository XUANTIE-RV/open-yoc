/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <tmedia_core/memory/buffer.h>
#include <tmedia_core/memory/default_buffer_allocator.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdatomic.h>
#include <stdio.h>
#if defined(__linux__) && !defined(PLATFORM_X86_64)
#include <sys/mman.h>
#endif
#define LOG_LEVEL 2
#define LOG_PREFIX "TMBuffer"
#include "tmedia_core/common/syslog.h"
#include "buffer_privete.h"


TMBuffer *TMBuffer_New(int size)
{
    return TMBuffer_NewEx(size, TMBUFFER_TYPE_USER_MALLOC, 0, NULL, NULL);
}

TMBuffer *TMBuffer_NewEx(int size,
                         TMBufferType bufType,
                         uint32_t flags,
                         TMBufferAllocator *allocator,
                         void *ctxAllocator)
{
    TMBufferImpl *thiz;

    if (size <= 0 && bufType != TMBUFFER_TYPE_CUSTOM)
    {
        printf("buffer new fail,buffer size <=0\n");
        return NULL;
    }

    thiz = (TMBufferImpl *)malloc(sizeof(TMBufferImpl));
    if (thiz == NULL)
    {
        printf("malloc TMBufferImpl fail\n");
        return NULL;
    }
    memset(thiz, 0, sizeof(TMBufferImpl));
    thiz->bufInfo.thiz = (TMBuffer *)thiz;
    thiz->bufInfo.type = bufType;
    thiz->bufInfo.flags = flags;
    thiz->bufInfo.size = size;
    thiz->bufInfo.fd = -1;

    if (allocator == NULL)
    {
        allocator = DefaultBufferAllocator_Get();
        if (allocator == NULL)
        {
            printf("get default allocator failure.\n");
            free(thiz);
            return NULL;
        }
    }

    if (allocator->alloc(ctxAllocator, &thiz->bufInfo) != 0)
    {
        printf("allocator->alloc failure,bufType:%d size:%d\n", thiz->bufInfo.type, thiz->bufInfo.size);
        free(thiz);
        return NULL;
    }

    if (allocator != NULL)   //clone allocator
    {
        thiz->allocator = (TMBufferAllocator *)malloc(sizeof(TMBufferAllocator));
        memcpy(thiz->allocator, allocator, sizeof(TMBufferAllocator));
    }

    thiz->ctxAllocator = ctxAllocator;
    thiz->refCount = ATOMIC_VAR_INIT(1);

    return (TMBuffer *)thiz;
}


TMBuffer *TMBuffer_NewFromData(void *data,
                               int size,
                               TMBufferAllocator *allocator,
                               void *ctxAllocator)
{
    TMBufferImpl *thiz;

    if (data == NULL || size <= 0)
    {
        printf("bad parameter: data is NULL or size <= 0\n");
        return NULL;
    }

    thiz = (TMBufferImpl *)malloc(sizeof(TMBufferImpl));
    if (thiz == NULL)
    {
        printf("malloc TMBufferImpl fail\n");
        return NULL;
    }
    memset(thiz, 0, sizeof(TMBufferImpl));

    thiz->bExternalMemory = true;
    thiz->bufInfo.thiz = (TMBuffer *)thiz;
    thiz->bufInfo.data = data;
    thiz->bufInfo.size = size;
    thiz->bufInfo.fd = -1;
    thiz->refCount =  ATOMIC_VAR_INIT(1);

    if (allocator != NULL)
    {
        thiz->allocator = (TMBufferAllocator *)malloc(sizeof(TMBufferAllocator));
        memcpy(thiz->allocator, allocator, sizeof(TMBufferAllocator));
        thiz->ctxAllocator = ctxAllocator;
    }

    return (TMBuffer *)thiz;
}


TMBuffer *TMBuffer_NewFromFD(int fd, int size, TMBufferAllocator *allocator, void *ctxAllocator)
{
    TMBufferImpl *thiz;

    if (fd < 0)
    {
        printf("bad parameter: invalid fd %d \n", fd);

        return NULL;
    }

    thiz = (TMBufferImpl *)malloc(sizeof(TMBufferImpl));
    if (thiz == NULL)
    {
        printf("malloc TMBufferImpl fail\n");
        return NULL;
    }

    memset(thiz, 0, sizeof(TMBufferImpl));
    thiz->bufInfo.thiz = (TMBuffer *)thiz;
    thiz->bufInfo.data = NULL;
    thiz->bufInfo.size = size;
    thiz->bufInfo.fd = fd;
    thiz->bExternalMemory = true;
    thiz->refCount = ATOMIC_VAR_INIT(1);

    if (allocator != NULL)
    {
        thiz->allocator = (TMBufferAllocator *)malloc(sizeof(TMBufferAllocator));
        memcpy(thiz->allocator, allocator, sizeof(TMBufferAllocator));
        thiz->ctxAllocator = ctxAllocator;
    }

    return (TMBuffer *)thiz;
}


/* Use external file descriptor & external virtual address */
TMBuffer *TMBuffer_NewFromFDWithVirAddr(int fd, void *virAddr, int size)
{
    TMBufferImpl *thiz;

    if (fd <= 0)
    {
        printf("bad parameter: invalid fd %d \n", fd);
        return NULL;
    }
    if (size <= 0)
    {
        printf("bad parameter: invalid size %d \n", size);
        return NULL;
    }

    thiz = (TMBufferImpl *)malloc(sizeof(TMBufferImpl));
    if (thiz == NULL)
    {
        printf("malloc TMBufferImpl fail\n");
        return NULL;
    }

    memset(thiz, 0, sizeof(TMBufferImpl));
    thiz->bufInfo.thiz = (TMBuffer *)thiz;
    thiz->bufInfo.fd = fd;
    thiz->bExternalMemory = true;
    thiz->refCount = ATOMIC_VAR_INIT(1);
    if (virAddr != NULL)
    {
        thiz->bufInfo.size = size;
        thiz->bufInfo.data = virAddr;
    }
#if defined(__linux__) && !defined(PLATFORM_X86_64)
    else
    {
        thiz->bufInfo.size = (size + 4095) & ~4095;
        thiz->bufInfo.data = mmap(NULL, thiz->bufInfo.size, PROT_READ | PROT_WRITE, MAP_SHARED, thiz->bufInfo.fd, 0);
        thiz->bMapped = true;
    }
#endif
    return (TMBuffer *)thiz;
}


TMBuffer *TMBuffer_AddRef(TMBuffer *buf)
{
    if (buf == NULL)
    {
        printf("%s: buffer is NULL\n", __func__);
        return NULL;
    }

    TMBufferImpl *thiz = (TMBufferImpl *)buf;
    atomic_fetch_add(&thiz->refCount, 1);

    return buf;
}


void *TMBuffer_Data(TMBuffer *buf)
{
    if (buf == NULL)
    {
        printf("%s: buffer is NULL\n", __func__);
        return NULL;
    }

    TMBufferImpl *thiz = (TMBufferImpl *)buf;

    if (thiz->bufInfo.data == NULL && !thiz->bMapped)
    {
        if (thiz->allocator != NULL && thiz->allocator->mmap != NULL) {
            if (thiz->allocator->mmap(thiz->ctxAllocator, &thiz->bufInfo) == 0) {
                thiz->bMapped = true;
            } else {
                printf("mmap fail\n");
                return NULL;
            }
        }
    }

    return thiz->bufInfo.data;
}

int TMBuffer_Size(TMBuffer *buf)
{
    if (buf == NULL)
    {
        printf("%s: buffer is NULL\n", __func__);
        return 0;
    }

    TMBufferImpl *thiz = (TMBufferImpl *)buf;

    return thiz->bufInfo.size;
}

int TMBuffer_SetUserData(TMBuffer *buf, void *userData)
{
    if (buf == NULL)
    {
        printf("%s: buffer is NULL\n", __func__);
        return -1;
    }

    TMBufferImpl *thiz = (TMBufferImpl *)buf;
    thiz->bufInfo.userData = userData;
    return 0;
}

void *TMBuffer_UserData(TMBuffer *buf)
{
    if (buf == NULL)
    {
        printf("%s: buffer is NULL\n", __func__);
        return NULL;
    }

    TMBufferImpl *thiz = (TMBufferImpl *)buf;
    return thiz->bufInfo.userData;
}

uint64_t TMBuffer_ExtId(TMBuffer *buf)
{
    if (buf == NULL)
    {
        printf("%s: buffer is NULL\n", __func__);
        return 0;
    }

    TMBufferImpl  *thiz = (TMBufferImpl *)buf;
    return thiz->bufInfo.extId;
}


/**
 * Returns the physical address of TMBuffer
 * 0 Indicates invalid
 * @return
 */
unsigned long TMBuffer_PhyAddr(TMBuffer *buf)
{
    if (buf == NULL)
    {
        printf("%s: buffer is NULL\n", __func__);
        return 0;
    }

    TMBufferImpl *thiz = (TMBufferImpl *)buf;

    return thiz->bufInfo.phyAddr;
}


int TMBuffer_DmaBuf(TMBuffer *buf)
{
    if (buf == NULL)
    {
        printf("%s: buffer is NULL\n", __func__);
        return -1;
    }
    TMBufferImpl *thiz = (TMBufferImpl *)buf;

    return thiz->bufInfo.fd;
}

int TMBuffer_UnRef(TMBuffer *buf)
{
    int ret;
    if (buf == NULL)
    {
        printf("%s: buffer is NULL\n", __func__);
        return -1;
    }

    TMBufferImpl *thiz = (TMBufferImpl *)buf;
    if (atomic_fetch_sub(&thiz->refCount, 1) == 1)   //need release
    {
        if (thiz->cbRefZero != NULL)
        {
            thiz->cbRefZero(buf);
        }

        if (thiz->allocator != NULL && thiz->bMapped == true) //unmap first
        {
            if (thiz->allocator->munmap == NULL)
            {
                printf("munmap function is nil in allocator\n");

                return -1;
            }
            ret = thiz->allocator->munmap(thiz->ctxAllocator, &thiz->bufInfo);
            if(ret == 0)
            {
                thiz->bufInfo.data = NULL;
                thiz->bMapped = false;
            }
        }
#if defined(__linux__) && !defined(PLATFORM_X86_64)
        if (thiz->bExternalMemory == true && thiz->bMapped == true)
        {
            if (thiz->bufInfo.data != NULL && thiz->bufInfo.size > 0)
            {
                munmap(thiz->bufInfo.data, thiz->bufInfo.size);
                thiz->bufInfo.data = NULL;
            }
        }
#endif
        if (thiz->allocator != NULL && thiz->allocator->free != NULL)
        {
            ret = thiz->allocator->free(thiz->ctxAllocator, &thiz->bufInfo);
            if (ret == FAKE_FREE_BUFFER)
            {
                return 0; //Think it's successful to release here
            }
        }

        if (thiz->allocator)
        {
            free(thiz->allocator);
        }
        free(thiz);
        thiz = NULL;
    }

    return 0;
}

int TMBuffer_RefCount(TMBuffer *buf)
{
    int refCount = 0;
    if (buf == NULL)
    {
        LOG_D("%s: buffer is NULL\n", __func__);
        return 0;
    }

    TMBufferImpl *thiz = (TMBufferImpl *)buf;
    refCount = atomic_load(&thiz->refCount);

    return refCount;
}

int TMBuffer_RegRefZeroCallBack(TMBuffer *buf, TMBufferRefZeroCallBack callback)
{
    if (buf == NULL)
    {
        return -1;
    }

    TMBufferImpl *thiz = (TMBufferImpl *)buf;
    thiz->cbRefZero =  callback;

    return 0;
}
