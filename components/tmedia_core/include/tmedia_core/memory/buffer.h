/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_BUFFER_H
#define TM_BUFFER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


typedef enum
{
    TMBUFFER_TYPE_USER_MALLOC  = 0, /* use malloc alloc */
    TMBUFFER_TYPE_DMA          = 1, /* system dma-buf */
    TMBUFFER_TYPE_CARVEOUT     = 2, /* system carveout memory */
    TMBUFFER_TYPE_CUSTOM       = 3  /* custom buffer type */
} TMBufferType;

#define TMBUFFER_FLAGS_CACHED       0x00000000  /* cpu cache type  */
#define TMBUFFER_FLAGS_UNCACHED     0x00000001  /* uncache type */
//#define TMBUFFER_FLAGS_WC         0x00000002  /* write-combine */
#define TMBUFFER_FLAGS_CONTIG       0x00000004  /* continuous physical memory */
#define TMBUFFER_FLAGS_CACHE_MASK   0x00000001  /* cache type mask */

typedef struct _TMBuffer TMBuffer;

typedef struct _BufferInfo
{
    TMBuffer *thiz;
    TMBufferType type;
    uint8_t *data;
    int size;
    union
    {
        unsigned long phyAddr;
        int fd;
    };

    /*
    *  The combination of memory flags
    *
    *  The lower 16 bits are TMBUFFER_FLAG_*,
    *  The high 16 is the parameter defined by bufType itself. For example,
    *  the high 16 is the  Carveout ID when bufType is TMBUFFER_TYPE_CARVEOUT
    *
    *  such as:
    *     flags = TMBUFFER_FLAGS_UNCACHED
    *     flags = TMBUFFR_FLAGS_UNCACHED | TMBUFFER_FLAGS_CONTIG
    *     flags =  carveout_id << 16 | TMBUFFER_FLAGS_CONTIG
    */
    uint32_t flags;

    uint64_t extId; //extern id of buffer
    void *userData; // user private data
} TMBufferInfo;

/**
 *
 * The allocation function defination that needs to fill the elements in TMBufferInfo structure
 *
 * @param ctx      the context of Allocator
 * @param bufInfo  TMBufferInfo structure
 *
 * @return  0 for success, -1 for failure
 */
typedef int (*TMBufferAllocFunc)(void *ctx, TMBufferInfo *bufInfo);

#define  FAKE_FREE_BUFFER  -99

/**
 * The release function defination
 *
 * @param ctx     the context of Allocator
 * @param bufInfo TMBufferInfo structure
 *
 * @return   0: success
 *           -1: failure
 *           FAKE_FREE_BUFFER: fake free operation
 */


typedef int (*TMBufferFreeFunc)(void *ctx, TMBufferInfo *bufInfo);

/**
 * the mmap function defination
 *
 * @param ctx the context of Allocator
 * @param TMBufferInfo structure
 *
 * @return 0:success  !0:failure
 */
typedef int (*TMBufferMMapFunc)(void *ctx, TMBufferInfo *bufInfo);

/**
 * the munmap function defination
 *
 * @param ctx the context of Allocator
 * @param TBufferInfo structure
 *
 * @return 0:success  !0:failure
 */
typedef int (*TMBufferMUnmapFunc)(void *ctx, TMBufferInfo *bufInfo);


/**
 * the reference zero call back function defination
 *
 * @param buf
 *
 * @return void
 */
typedef void (*TMBufferRefZeroCallBack)(TMBuffer* buf);

typedef struct _TMBufferAllocator
{
    TMBufferAllocFunc  alloc;
    TMBufferFreeFunc   free;
    TMBufferMMapFunc   mmap;
    TMBufferMUnmapFunc munmap;
} TMBufferAllocator;


/**
 * Create a TMBuffer of the specified size
 *
 * @param size          memory size to allocator
 *
 * @return  TMBuffer for success,NULL for failure
 */
TMBuffer *TMBuffer_New(int size);


/**
 * Create a TMBuffer of the specified size and others parameters
 *
 * @param size          memory size to allocator
 * @param bufType       buffer type
 * @param flags         reference TMBufferInfo::flags
 *
 * @param allocator     if the allocator is empty, the default system allocator will be used
 * @param ctxAllocator  the context of allocator
 * @return  TMBuffer for success,NULL for failure
 */
TMBuffer *TMBuffer_NewEx(int size,
                       TMBufferType bufType,
                       uint32_t flags,
                       TMBufferAllocator *allocator,
                       void *ctxAllocator);


/**
 * Create TMBuffer from existing external data
 *
 * @param data         external data pointer
 * @param size         external data size
 * @param allocator    buffer allocator
 * @param ctxAllocator the context of allocator
 *
 * @return TMBuffer for success,NULL for failure
 */
TMBuffer *TMBuffer_NewFromData(void *data,
                               int size,
                               TMBufferAllocator *allocator,
                               void *ctxAllocator);

/**
 * Create TMBuffer from existing external fd
 *
 * @param fd           external fd
 * @param allocator    buffer allocator
 * @param ctxAllocator the context of allocator
 *
 * @return TMBuffer for success,NULL for failure
 */
TMBuffer *TMBuffer_NewFromFD(int fd,
                             int size,
                             TMBufferAllocator *allocator,
                             void* ctxAllocator);

/**
 * Create TMBuffer from existing external fd
 *
 * @param fd           external fd
 * @param data         external data pointer
 * @param size         external data size
 *
 * @return TMBuffer for success,NULL for failure
 */
TMBuffer *TMBuffer_NewFromFDWithVirAddr(int fd,
                        void *data,
                        int size);

/**
 * AddRef  operation messages increment the reference count
 *
 * @param  buf
 * @return TMBuffer for success,NULL for failure
 */
TMBuffer *TMBuffer_AddRef(TMBuffer *buf);

/**
 *  the reference count of this TMBuffer is decrement,
 *  if the reference count reaches 0, it is released
 */
int TMBuffer_UnRef(TMBuffer *buf);


/**
 *   if the reference count reaches 0, the callback function will be invoke
 *  @param buf
 *  @param cb  callback function
 */
int TMBuffer_RegRefZeroCallBack(TMBuffer* buf,TMBufferRefZeroCallBack callback);

/**
 * virtual memory address
 *
 * @return
 */
void *TMBuffer_Data(TMBuffer *buf);

/**
 * Set UserData of TMBuffer
 *
 * @param buf
 * @param userData
 * @return 0:success  !0:failure
 */
int TMBuffer_SetUserData(TMBuffer *buf, void *userData);

/**
 * UserData of TMBuffer
 *
 * @param buf
 * @return User Data for success,NULL for failure
 */
void *TMBuffer_UserData(TMBuffer *buf);

/**
 * ExternId of TMBuffer
 *
 * @param buf
 * @return
 */
uint64_t TMBuffer_ExtId(TMBuffer *buf);

/**
 * Physics address of TMBuffer
 *
 * @return
 */
unsigned long TMBuffer_PhyAddr(TMBuffer *buf);

/**
 * memory size
 *
 * @return
 */
int TMBuffer_Size(TMBuffer *buf);

/**
 * get TMBuffer dma-buf fd
 *
 * @return  >=0 for success,-1 for failure or  fd is invalid
 */
int TMBuffer_DmaBuf(TMBuffer *buf);

/**
 * get reference count
 *
 * @param buf
 * @return
 */
int TMBuffer_RefCount(TMBuffer *buf);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* TM_BUFFER_H */
