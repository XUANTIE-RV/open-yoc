/*
* uheap.h                                                   Version 2.51
*
* smxUSBH Memory Management.
*
* Copyright (c) 2004-2011 Micro Digital Inc.
* All rights reserved. www.smxrtos.com
*
* This software is confidential and proprietary to Micro Digital Inc.
* It has been furnished under a license and may be used, copied, or
* disclosed only in accordance with the terms of that license and with
* the inclusion of this header. No title to nor ownership of this
* software is hereby transferred.
*
* Author: Yingbo Hu
*
* Portable to any ANSI compliant C compiler.
*
*****************************************************************************/

#ifndef SU_HEAP_H
#define SU_HEAP_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#if defined(__cplusplus)
extern "C" {
#endif

#if SU_NEED_NC_MEMORY

void *su_HeapInit(u32 uiSize);
void  su_HeapRelease(void);

/* For OHCI, UHCI, and EHCI, we need to allocate non-cacheable memory. */
void *su_AllocNC(uint iSize, uint iAlign);
void  su_FreeNC(void *pAddr);

/* These allocate normal memory. */
#if !SU_USE_C_HEAP
void *su_HeapMalloc(u32 num_bytes);
void  su_HeapFree(void *block);
#define su_Alloc           su_HeapMalloc
#define su_Free            su_HeapFree
#else
#define su_Alloc           sb_OS_MEM_ALLOC
#define su_Free            sb_OS_MEM_FREE
#endif

#else

/* For other controllers, we do not need non-cacheable memory. */
#if !SU_USE_C_HEAP
void *su_HeapInit(u32 uiSize);
void  su_HeapRelease(void);
void *su_HeapMalloc(u32 num_bytes);
void  su_HeapFree(void *block);
#define su_AllocNC(a, b)   su_HeapMalloc(a)
#define su_FreeNC          su_HeapFree
#define su_Alloc           su_HeapMalloc
#define su_Free            su_HeapFree
#else
#define su_HeapInit(uiSize) uiSize
#define su_HeapRelease()
#define su_AllocNC(a, b)   sb_OS_MEM_ALLOC(a)
#define su_FreeNC          sb_OS_MEM_FREE
#define su_Alloc           sb_OS_MEM_ALLOC
#define su_Free            sb_OS_MEM_FREE
#endif
#endif

#if SU_DEBUG_LEVEL
int   su_GetAllocatedSize(void);
void  su_HeapCheck(void);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* SU_HEAP_H */
