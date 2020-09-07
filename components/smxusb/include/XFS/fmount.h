/*
* fmount.h                                                  Version 2.24
*
* smxFS Mount Related Functions.
*
* Copyright (c) 2004-2014 Micro Digital Inc.
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

#ifndef SFS_FMOUNT_H
#define SFS_FMOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

uint  CheckDevice(PDEVICEHANDLE pDevHandle);

FATCONSDEF *GetFATCons(uint iFatType);
BOOLEAN CheckJumpCode(u8 *pMem);
BOOLEAN CheckBootSignature(u8 *pMem);

#if SB_CPU_BIG_ENDIAN
void InvertPartitionTable(PARTITIONTABLE *pPartition);
void InvertFATBPB16(BPB16 *pBPB);
#if SFS_FAT32_SUPPORT
void InvertFATBPB32(BPB32 *pBPB);
#endif
#else
#define InvertFATBPB16(pBPB)
#define InvertFATBPB32(pBPB)
#define InvertPartitionTable(pPartition)
#endif /* SB_CPU_BIG_ENDIAN */

#if !SB_PACKED_STRUCT_SUPPORT
void UnpackPartitionTable(PARTITIONTABLE *pPartition, void *pMem);
void PackPartitionTable(PARTITIONTABLE *pPartition, void *pMem);
void UnpackFATBPB16(BPB16 *pBPB, void *pMem);
void PackFATBPB16(BPB16 *pBPB, void *pMem);
void UnpackFATBPB32(BPB32 *pBPB, void *pMem);
void PackFATBPB32(BPB32 *pBPB, void *pMem);
#else
#define UnpackPartitionTable(pPartition, pMem)  memcpy(pPartition, pMem, SIZEOFPARTITIONTABLE)
#define PackPartitionTable(pPartition, pMem)    memcpy(pMem, pPartition, SIZEOFPARTITIONTABLE)
#define UnpackFATBPB16(pBPB, pMem)              memcpy(pBPB, pMem, SIZEOFBPB16)
#define PackFATBPB16(pBPB, pMem)                memcpy(pMem, pBPB, SIZEOFBPB16)
#define UnpackFATBPB32(pBPB, pMem)              memcpy(pBPB, pMem, SIZEOFBPB32)
#define PackFATBPB32(pBPB, pMem)                memcpy(pMem, pBPB, SIZEOFBPB32)
#endif

int   MountDevice(PDEVICEHANDLE pDevHandle);
void  UnmountDevice(PDEVICEHANDLE pDevHandle);

#if SFS_USE_FAT32_FSINFO || SFS_FAT_FSINFO_SUPPORT
int   SetFSInfo(PDEVICEHANDLE pDevHandle, u32 dwFreeClus, u32 dwNxtFreeClus);
#endif /* SFS_FAT32_SUPPORT */

#ifdef __cplusplus
}
#endif

#endif /* SFS_FMOUNT_H */
