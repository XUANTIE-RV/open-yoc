/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_BUFFER_POOL_H
#define TM_BUFFER_POOL_H


#include "buffer.h"

#if __cplusplus
extern "C" {
#endif

    typedef struct _TMBufferPool TMBufferPool;
#define MAX_BUFFER_POOL_NAME_LEN  (32)

    typedef struct _TMBufferPoolConfig
    {
        TMBufferType bufferType;                   /** buffer memory type */
        uint32_t     bufferFlags;                  /** buffer flags */
        int          bufferSize;                   /** buffer memory size */
        uint32_t     bufferCnt;                    /** buffer count */
        char poolName[MAX_BUFFER_POOL_NAME_LEN + 1]; /** buffer pool name */

        TMBufferAllocator *allocator;              /**
                                                    *  buffer custom allocator, NULL will use system default allocator,
                                                    *  reference TMDefaultBufferAllocator.h
                                                    **/

        void *ctxAllocator;                        /** allocator context */
    } TMBufferPoolConfig;


    /**
     * Buffer Pool creation
     * @param cfg
     * @return
     */
    TMBufferPool *TMBufferPool_New(const TMBufferPoolConfig *cfg);

    /**
     * Get buffer pool configuration
     *
     * @param cfg
     */
    int TMBufferPool_GetConfig(TMBufferPool *pool, TMBufferPoolConfig *cfg);


    /**
     * buffer pool release
     *
     * @param pool
     */
    void TMBufferPool_Release(TMBufferPool *pool);


    /**
     * Number of available buffers in the buffer pool
     */
    int TMBufferPool_FreeCount(TMBufferPool *pool);


    /**
     * acquire buffer from  pool
     *
     * @param pool   buffer pool
     * @param waitMS Maximum milliseconds to wait, 0 does not wait
     *
     * @return TMBuffer for success,NULL for failure
     */
    TMBuffer *TMBufferPool_AcquireBuffer(TMBufferPool *pool, uint32_t waitMS);


    /*****************************   Common Buffer Pool  ***************************/
    typedef uint32_t  CommonPoolID;
#define MAX_COMMON_POOL_COUNT     16

    typedef struct _TMCommonBufferPoolConfig
    {
        uint32_t count;
        TMBufferPoolConfig cfg[MAX_COMMON_POOL_COUNT];
    } TMCommonBufferPoolConfig;

    int  TMCommonPool_GetConfig(TMCommonBufferPoolConfig *cfg);
    int  TMCommonPool_Init(const TMCommonBufferPoolConfig *cfg);
    int  TMCommonPool_Setup();
    void TMCommonPool_UnInit();

    /**
    * Number of available buffers in the common buffer pool
    */
    int TMCommonPool_FreeCount(TMBufferType bufferType,
                               uint32_t  bufferFlags,
                               uint32_t  bufferSize);

    /**
     * Get the buffer from common buffer pool
     *
     * @param bufferType   buffer type
     * @param bufferFlags  buffer flag
     * @param bufferSize   buffer size
     * @param waitMS       Maximum milliseconds to wait, 0 does not wait
     *
     * @return
     */

    TMBuffer *TMCommonPool_AcquireBuffer(TMBufferType bufferType,
                                         uint32_t  bufferFlags,
                                         uint32_t  bufferSize,
                                         uint32_t  waitMS);


#if __cplusplus
}
#endif  /* __cplusplus */

#endif  /* TM_BUFFER_POOL_H */
