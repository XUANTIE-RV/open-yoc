/*
 * Copyright (C) 2022-2023 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/time.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <tmedia_core/memory/default_buffer_allocator.h>
#include <tmedia_core/memory/buffer_pool.h>
#include "queue.h"
#include "buffer_privete.h"

typedef struct _TMBufferPoolImpl
{
    TMBufferPoolConfig  cfg;

    TAILQ_HEAD(stailhead, PoolItem) freeListHead;
    TAILQ_HEAD(tailhead, PoolItem)  usedLisHead;
    int freeCount;
    int usedCount;
    bool activated;

    pthread_mutex_t itemsLock;
    pthread_mutex_t semMutex;
    pthread_cond_t  semCond;
} TMBufferPoolImpl;

struct PoolItem
{
    TAILQ_ENTRY(PoolItem) entries;

    TMBufferAllocator origAllocator;
    void *origAllocatorCtx;

    TMBuffer *buf;
    TMBufferPoolImpl *pool;
    atomic_bool poolMapedFlag;
    atomic_bool poolRemovedFlag;
};


typedef struct _CommonPool
{
    TMCommonBufferPoolConfig  poolsCfg;
    TMBufferPool *pools[MAX_COMMON_POOL_COUNT];
    atomic_bool initFlag;
    atomic_bool setupFlag;
} CommonPool;

static CommonPool g_commonPool = {.initFlag = ATOMIC_VAR_INIT(false), .setupFlag = ATOMIC_VAR_INIT(false) };


static int PoolBufferAlloc(void *ctx, TMBufferInfo *bufInfo)
{
    struct PoolItem *poolItem = (struct PoolItem *)ctx;
    int ret;

    ret = poolItem->origAllocator.alloc(poolItem->origAllocatorCtx, bufInfo);
    return ret;
}


static int PoolBufferFree(void *ctx, TMBufferInfo *bufInfo)
{
    struct PoolItem *poolItem = (struct PoolItem *)ctx;
    TMBufferPoolImpl *pool = poolItem->pool;
    int ret;

    if (!atomic_load(&poolItem->poolRemovedFlag))
    {
        TMBuffer *buf = bufInfo->thiz;
        TMBuffer_AddRef(buf); /* Reference count increase */

        pthread_mutex_lock(&pool->itemsLock);
        TAILQ_REMOVE(&pool->usedLisHead, poolItem, entries); /* Remove from used list */
        TAILQ_INSERT_TAIL(&pool->freeListHead, poolItem, entries); /* Add to available list */
        pool->freeCount++;
        pool->usedCount--;
        pthread_mutex_unlock(&pool->itemsLock);

        ret = FAKE_FREE_BUFFER;

        pthread_mutex_lock(&pool->semMutex);
        pthread_cond_signal(&pool->semCond);
        pthread_mutex_unlock(&pool->semMutex);

    }
    else     /* The pool has been released and needs to release buffer and release pool item */
    {
        ret = poolItem->origAllocator.free(poolItem->origAllocatorCtx, bufInfo);
        TAILQ_REMOVE(&pool->freeListHead, poolItem, entries);
        free(poolItem);
        pool->freeCount--;
        ret = 0;
    }

    return ret;
}

static int PoolBufferMMap(void *ctx, TMBufferInfo *bufInfo)
{
    int ret = -1;
    struct PoolItem *poolItem = (struct PoolItem *)ctx;
    if(!atomic_load(&poolItem->poolMapedFlag))
    {
        ret = poolItem->origAllocator.mmap(poolItem->origAllocatorCtx, bufInfo);
        poolItem->poolMapedFlag = ATOMIC_VAR_INIT(true);
    }
    return ret;
}


static int PoolBufferMUnmap(void *ctx, TMBufferInfo *bufInfo)
{
    int ret = -1;
    struct PoolItem *poolItem = (struct PoolItem *)ctx;
    if (atomic_load(&poolItem->poolRemovedFlag))
    {
        ret = poolItem->origAllocator.munmap(poolItem->origAllocatorCtx, bufInfo);
        poolItem->poolMapedFlag = ATOMIC_VAR_INIT(false);
        return ret;
    }

    return ret;
}

int  TMCommonPool_Init(const TMCommonBufferPoolConfig *cfg)
{
    int i, j;
    int sortIdx[MAX_COMMON_POOL_COUNT];
    int sortCnt;
    int minBlock;
    int minIdx;

    if (atomic_load(&g_commonPool.initFlag))
    {
        printf("common pool has already init.\n");
        return 0;
    }

    sortCnt = 0;

    //sort from small to large
    do
    {
        minBlock = INT_MAX;
        minIdx = -1;

        for (i = 0; i < cfg->count; i++)
        {
            for (j = 0; j < sortCnt; j++)
            {
                if (i == sortIdx[j])
                {
                    break;
                }
            }

            if (j < sortCnt)
            {
                continue;
            }


            if (cfg->cfg[i].bufferSize <= minBlock)
            {
                minBlock = cfg->cfg[i].bufferSize;
                minIdx = i;
            }
        }

        if (minIdx != -1)
        {
            sortIdx[sortCnt] = minIdx;
            sortCnt++;
        }
    }
    while (sortCnt < cfg->count);

    for (i = 0; i < sortCnt; i++)
    {
        memcpy(&g_commonPool.poolsCfg.cfg[i], &cfg->cfg[sortIdx[i]], sizeof(TMBufferPoolConfig));
        if (cfg->cfg[sortIdx[i]].allocator != NULL)   //clone allocator
        {
            g_commonPool.poolsCfg.cfg[i].allocator = (TMBufferAllocator *) malloc(sizeof(TMBufferAllocator));
            memcpy(g_commonPool.poolsCfg.cfg[i].allocator, cfg->cfg[sortIdx[i]].allocator, sizeof(TMBufferAllocator));
        }
    }

    g_commonPool.poolsCfg.count = sortCnt;

    atomic_store(&g_commonPool.initFlag, true);
    atomic_store(&g_commonPool.setupFlag, false);

    return 0;
}

int TMCommonPool_GetConfig(TMCommonBufferPoolConfig *cfg)
{
    if (cfg == NULL)
    {
        printf("cfg parameter is NULL\n");
        return -1;
    }

    memcpy(cfg, &g_commonPool.poolsCfg, sizeof(*cfg));
    return 0;
}


void TMCommonPool_UnInit()
{
    TMBufferPool *pool;
    int i;

    for (i = 0; i < g_commonPool.poolsCfg.count; i++)
    {
        pool = g_commonPool.pools[i];
        TMBufferPool_Release(pool);

        if (g_commonPool.poolsCfg.cfg[i].allocator)
        {
            free(g_commonPool.poolsCfg.cfg[i].allocator);
        }
    }

    atomic_store(&g_commonPool.initFlag, false);
    atomic_store(&g_commonPool.setupFlag, false);
}

int TMCommonPool_Setup()
{
    TMBufferPool *bufferPool;
    int i, j;

    if (!atomic_load(&g_commonPool.initFlag))
    {
        printf("common pool need init first.\n");
        return 0;
    }

    if (atomic_load(&g_commonPool.setupFlag))
    {
        printf("common pool has already setup.\n");
        return 0;
    }

    for (i = 0; i < g_commonPool.poolsCfg.count; i++)
    {
        TMBufferPoolConfig *cfg = &g_commonPool.poolsCfg.cfg[i];

        bufferPool = TMBufferPool_New(cfg);
        if (!bufferPool)
        {
            for (j = 0; j < i; j++)
            {
                TMBufferPool_Release(g_commonPool.pools[j]);
            }
            return -1;
        }
        g_commonPool.pools[i] = bufferPool;
    }

    atomic_store(&g_commonPool.setupFlag, true);

    return 0;
}


int TMCommonPool_FreeCount(TMBufferType bufferType,
                           uint32_t  bufferFlags,
                           uint32_t  blkSize){
    TMBufferPoolConfig *cfg;
    int i;

    /**
    *    The strategy is to find the first pool whose size is greater than or equal to blksize and whose memory type and flag are consistent,
    * then get FreeCount buffer form pool
    */
    for (i = 0; i < g_commonPool.poolsCfg.count; i++)
    {
        cfg = &g_commonPool.poolsCfg.cfg[i];

        if (bufferType == cfg->bufferType &&
            bufferFlags == cfg->bufferFlags)
        {
            if (g_commonPool.poolsCfg.cfg[i].bufferSize >= blkSize)
            {
                return  TMBufferPool_FreeCount(g_commonPool.pools[i]);
            }
        }
    }

    return 0;
}

TMBuffer *TMCommonPool_AcquireBuffer(TMBufferType bufferType,
                                     uint32_t  bufferFlags,
                                     uint32_t blkSize,
                                     uint32_t waitMS)
{
    TMBuffer *buffer;
    TMBufferPoolConfig *cfg;
    int i;

    /**
    *    The strategy is to find the first pool whose size is greater than or equal to blksize and whose memory type and flag are consistent,
    * then acquire buffer form pool
    */
    for (i = 0; i < g_commonPool.poolsCfg.count; i++)
    {
        cfg = &g_commonPool.poolsCfg.cfg[i];

        if (bufferType == cfg->bufferType &&
                bufferFlags == cfg->bufferFlags)
        {
            if (g_commonPool.poolsCfg.cfg[i].bufferSize >= blkSize)
            {
                buffer = TMBufferPool_AcquireBuffer(g_commonPool.pools[i], waitMS);
                return buffer;
            }
        }
    }

    return NULL;
}

TMBufferPool *TMBufferPool_New(const TMBufferPoolConfig *cfg)
{
    TMBufferPoolImpl *pool;
    TMBuffer *buf;
    TMBufferAllocator poolAllocator;
    struct PoolItem *poolItem;
    pthread_condattr_t  cond_attr;
    int i;

    if (cfg == NULL)
    {
        printf("cfg parameter is NULL\n");
        return NULL;
    }


    if (cfg->bufferCnt == 0 || (cfg->bufferType != TMBUFFER_TYPE_CUSTOM && cfg->bufferSize == 0))
    {
        printf("cfg->bufferCnt or cfg->bufferSize is 0\n");
        return NULL;
    }

    pool = (TMBufferPoolImpl *)malloc(sizeof(TMBufferPoolImpl));
    if (pool == NULL)
    {
        return NULL;
    }
    memset(pool, 0, sizeof(TMBufferPoolImpl));
    memcpy(&pool->cfg, cfg, sizeof(TMBufferPoolConfig));
    if (cfg->allocator)
    {
        pool->cfg.allocator = (TMBufferAllocator *)malloc(sizeof(TMBufferAllocator));
        memcpy(pool->cfg.allocator, cfg->allocator, sizeof(TMBufferAllocator));
    }

    TAILQ_INIT(&pool->freeListHead);
    TAILQ_INIT(&pool->usedLisHead);

    pthread_condattr_init(&cond_attr);
    pthread_cond_init(&pool->semCond, &cond_attr);
    pthread_mutex_init(&pool->semMutex, 0);
    pthread_mutex_init(&pool->itemsLock, 0);
    pthread_condattr_destroy(&cond_attr);


    TMBufferType  bufType = cfg->bufferType;
    uint32_t bufFlags = cfg->bufferFlags;

    poolAllocator.alloc = PoolBufferAlloc;
    poolAllocator.free = PoolBufferFree;
    poolAllocator.mmap = PoolBufferMMap;
    poolAllocator.munmap = PoolBufferMUnmap;

    for (i = 0; i < pool->cfg.bufferCnt; i++)
    {
        poolItem = (struct PoolItem *)malloc(sizeof(struct PoolItem));
        if (poolItem == NULL)
        {
            break;
        }
        memset(poolItem, 0, sizeof(*poolItem));
        poolItem->pool =  pool;
        poolItem->poolRemovedFlag = ATOMIC_VAR_INIT(false);

        if (pool->cfg.allocator)
        {
            memcpy(&poolItem->origAllocator, pool->cfg.allocator, sizeof(TMBufferAllocator));
            poolItem->origAllocatorCtx = pool->cfg.ctxAllocator;
        }
        else
        {
            memcpy(&poolItem->origAllocator, DefaultBufferAllocator_Get(), sizeof(TMBufferAllocator));
            poolItem->origAllocatorCtx = NULL;
        }
        buf = TMBuffer_NewEx(pool->cfg.bufferSize, bufType, bufFlags, &poolAllocator, poolItem);
        if (buf == NULL)
        {
            break;
        }
        poolItem->buf = buf;
        TAILQ_INSERT_TAIL(&pool->freeListHead, poolItem, entries);
    }

    if (i != pool->cfg.bufferCnt)
    {
        TMBufferPool_Release((TMBufferPool *)pool);
        printf("create buffer pool fail\n");
        return NULL;
    }

    pool->freeCount = cfg->bufferCnt;
    pool->usedCount = 0;
    pool->activated = true;

    return (TMBufferPool *)pool;
}


int TMBufferPool_FreeCount(TMBufferPool *pool)
{
    TMBufferPoolImpl *poolImpl;
    int freeCnt;

    if (pool == NULL) return -1;
    poolImpl = (TMBufferPoolImpl *) pool;

    pthread_mutex_lock(&poolImpl->itemsLock);
    freeCnt = poolImpl->freeCount;
    pthread_mutex_unlock(&poolImpl->itemsLock);

    return freeCnt;
}


int TMBufferPool_Release(TMBufferPool *pool)
{
    TMBufferPoolImpl *poolImpl;
    struct PoolItem *item1, *item2;
    TMBuffer *buf;
    int freeCnt;
    int usedCnt;
    int totalCnt;

    if (pool == NULL)
    {
        printf("pool parameter is NULL\n");
        return -1;
    }
    poolImpl = (TMBufferPoolImpl *)pool;

    pthread_mutex_lock(&poolImpl->itemsLock);

    freeCnt = poolImpl->freeCount;
    usedCnt = poolImpl->usedCount;
    totalCnt = freeCnt + usedCnt;

    if (poolImpl->activated == false)
    {
        if (totalCnt > 0)
            printf("warning: there are still %d buffers have not been unref!\n", totalCnt);
        printf("delete buffer pool\n");
        goto UNREF_OUTSIDE_POOL;
    }

    item1 = TAILQ_FIRST(&poolImpl->freeListHead);
    while (item1 != NULL)
    {
        item2 = TAILQ_NEXT(item1, entries);
        buf = item1->buf;

        atomic_store(&item1->poolRemovedFlag, true);

        TMBuffer_UnRef(buf);
        item1 = item2;
    }

    // buffers are not allowed to be free outside bufferpool
    TAILQ_FOREACH(item1, &poolImpl->usedLisHead, entries)
    {
        atomic_store(&item1->poolRemovedFlag,
                     true); /* Because these used buffers have been separated from the pool and have been used externally */
        /* make a pool destruction mark on the pool item */
    }
    poolImpl->activated = false;

    freeCnt = poolImpl->freeCount;
    usedCnt = poolImpl->usedCount;
    totalCnt = freeCnt + usedCnt;

    if (totalCnt > 0)
    {
        printf("%d buffers have not been UnRefed. Please try latter.\n", (totalCnt));
        pthread_mutex_unlock(&poolImpl->itemsLock);
        return (totalCnt);
    }

UNREF_OUTSIDE_POOL:
    pthread_mutex_unlock(&poolImpl->itemsLock);

    TAILQ_FOREACH(item1, &poolImpl->usedLisHead, entries)
    {
        TMBuffer *usedBuf = item1->buf;
        TMBufferImpl *buff = (TMBufferImpl *)usedBuf;
        TMBufferAllocator *allocator = DefaultBufferAllocator_Get();
        if (allocator == NULL || buff->allocator == NULL)
        {
            printf("get default allocator failure.\n");
            return -1;
        }
        memcpy(buff->allocator, allocator, sizeof(TMBufferAllocator));
    }

    if (poolImpl->cfg.allocator != NULL)
    {
        free(poolImpl->cfg.allocator);
    }

    pthread_cond_destroy(&poolImpl->semCond);
    pthread_mutex_destroy(&poolImpl->semMutex);
    pthread_mutex_destroy(&poolImpl->itemsLock);

    free(poolImpl);
    return totalCnt;
}

static int BufferPool_Wait_PoolItem(TMBufferPoolImpl *poolImpl, uint32_t waitMS)
{
    int res;
    struct timespec tspec;
    struct timeval now;

    pthread_mutex_lock(&poolImpl->semMutex);
    gettimeofday(&now, NULL);
    int nsec = now.tv_usec * 1000 + (waitMS % 1000) * 1000000;
    tspec.tv_nsec = nsec % 1000000000;
    tspec.tv_sec = now.tv_sec + nsec / 1000000000 + waitMS / 1000;
    res = pthread_cond_timedwait(&poolImpl->semCond, &poolImpl->semMutex, &tspec);
    pthread_mutex_unlock(&poolImpl->semMutex);

    //ETIMEDOUT == res  timeout

    return res == 0 ? 0 : 1;
}


TMBuffer *TMBufferPool_AcquireBuffer(TMBufferPool *pool, uint32_t waitMS)
{
    TMBufferPoolImpl *poolImpl = (TMBufferPoolImpl *)pool;
    struct PoolItem *item;

    if (poolImpl == NULL)
    {
        printf("pool parameter is NULL\n");
        return NULL;
    }
    if (poolImpl->activated != true)
    {
        printf("Pool is unactivated. Can't operate pool buffers!\n");
        return NULL;
    }

    pthread_mutex_lock(&poolImpl->itemsLock);
    item = TAILQ_FIRST(&poolImpl->freeListHead);
    if (item != NULL)
    {
        TAILQ_REMOVE(&poolImpl->freeListHead, item, entries);
        TAILQ_INSERT_TAIL(&poolImpl->usedLisHead, item, entries);
        poolImpl->freeCount --;
        poolImpl->usedCount ++;
        pthread_mutex_unlock(&poolImpl->itemsLock);
    }
    else
    {
        pthread_mutex_unlock(&poolImpl->itemsLock);
        if (waitMS > 0)   /* Wait for a free buffer */
        {

            if (BufferPool_Wait_PoolItem(poolImpl, waitMS) == 0)
            {
                pthread_mutex_lock(&poolImpl->itemsLock);
                item = TAILQ_FIRST(&poolImpl->freeListHead);
                if (item != NULL)
                {
                    TAILQ_REMOVE(&poolImpl->freeListHead, item, entries);
                    TAILQ_INSERT_TAIL(&poolImpl->usedLisHead, item, entries);

                    poolImpl->freeCount--;
                    poolImpl->usedCount++;
                }
                pthread_mutex_unlock(&poolImpl->itemsLock);
            }
        }
    }

    return item ? item->buf : NULL;
}

int TMBufferPool_GetConfig(TMBufferPool *pool, TMBufferPoolConfig *cfg)
{
    TMBufferPoolImpl *poolImpl = (TMBufferPoolImpl *)pool;

    if (poolImpl == NULL)
    {
        printf("pool parameter is NULL\n");
        return -1;
    }
    if (poolImpl->activated != true)
    {
        printf("Pool is unactivated. Can't operate pool buffers!\n");
        return -1;
    }

    if (cfg == NULL)
    {
        printf("cfg parameter is NULL\n");
        return -1;
    }

    memcpy(cfg, &poolImpl->cfg, sizeof(TMBufferPoolConfig));

    return 0;
}