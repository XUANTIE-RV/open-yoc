/*
* fport.h                                                   Version 2.25
*
* smxFS Porting Layer.
*
* Copyright (c) 2004-2018 Micro Digital Inc.
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

#ifndef SFS_FPORT_H
#define SFS_FPORT_H

#define SFS_VERSION 0x0225  /* version is XX.XX */

#ifdef __cplusplus
extern "C" unsigned int sfs_Version;
#else
extern unsigned int sfs_Version;
#endif

/********** OS Porting Section **********/

/* API ENTER/EXIT mutex */
#define SFS_MUTEX_HANDLE      SB_OS_MUTEX_HANDLE
#define SFS_MUTEX_CREATE      sb_OS_MUTEX_CREATE
#define SFS_MUTEX_DELETE      sb_OS_MUTEX_DELETE
#define SFS_API_ENTER         sb_OS_MUTEX_GET_INF
#define SFS_API_EXIT          sb_OS_MUTEX_RELEASE

/********** Heap Porting Section **********/

#if SFS_USE_C_HEAP
#define sfs_heapinit(uiSize)
#define sfs_heaprelease()
#if defined(__VISUALDSPVERSION__)
/* use user heap in the SDRAM */
#define sfs_malloc(size) heap_malloc(heap_lookup(1), size) 
#define sfs_free(p) heap_free(heap_lookup(1), p)
#else
#define sfs_malloc sb_OS_MEM_ALLOC
#define sfs_free sb_OS_MEM_FREE
#endif
#else
#ifdef __cplusplus
extern "C" {
#endif
  void *sfs_heapinit(u32 uiSize);
  void  sfs_heaprelease(void);
  void *sfs_malloc(u32 num_bytes);
  void  sfs_free(void *block);
#ifdef __cplusplus
}
#endif
#endif

/********* Hardware Porting Section *********/

#if SB_CPU_BIG_ENDIAN
#define SFS_INVERT_U16(v16)  SB_INVERT_U16(v16)
#define SFS_INVERT_U32(v32)  SB_INVERT_U32(v32)
#else
#define SFS_INVERT_U16(v16)  (u16)(v16)
#define SFS_INVERT_U32(v32)  (u32)(v32)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/********** Debug Output Porting Section **********/

#if SFS_DEBUG_LEVEL
extern void             sfs_DumpHexL(const char * pInfo, u32 value, int level);
extern void             sfs_DebugL(const char * pszDbgInfo, int level);
#else
#define sfs_DumpHexL(pInfo, value, level);
#define sfs_DebugL(pszDbgInfo, level);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SFS_FPORT_H */

