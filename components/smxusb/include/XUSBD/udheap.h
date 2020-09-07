/*
* udheap.h                                                  Version 2.56
*
* smxUSBD Memory Management.
*
* Copyright (c) 2008-2013 Micro Digital Inc.
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

#ifndef SUD_HEAP_H
#define SUD_HEAP_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

void *sud_Alloc(uint n);
void sud_Free(void *p);
void *sud_MemoryCopy(void *pDest, uint iOffset, void *pSrc, uint len);

#if SUD_USE_NON_CACHEABLE_MEM
void sud_HeapInit(u32 dwStartAddr, uint iSize);
void sud_HeapRelease(void);
void *sud_AllocNC(uint n);
void sud_FreeNC(void *p);
#else
#define sud_HeapInit(dwStartAddr, iSize);
#define sud_HeapRelease();
#define sud_AllocNC sud_Alloc
#define sud_FreeNC sud_Free
#endif

#ifdef __cplusplus
}
#endif

#endif /* SUD_HEAP_H */
