/*
* fcache.h                                                  Version 2.22
*
* smxFS Cache Functions.
*
* Copyright (c) 2004-2012 Micro Digital Inc.
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

#ifndef SFS_FCACHE_H
#define SFS_FCACHE_H

#ifdef __cplusplus
extern "C" {
#endif

int  DeviceCacheInit(PDEVICEHANDLE pDevHandle, u32 dwFATSecNum);
void DeviceCacheRelease(PDEVICEHANDLE pDevHandle);

#if !SFS_READONLY
int  WriteDataToFile(FILEHANDLE filehandle, PDEVICEHANDLE pDevHandle, u8 *pDataBuf);
int  SetFATNode(PDEVICEHANDLE pDevHandle, u32 dwCluster , u32 dwFatEntry);
int  UpdateFileEntry(FILEHANDLE filehandle, PDEVICEHANDLE pDevHandle);
int  FlushFATandDir(PDEVICEHANDLE pDevHandle, uint iFlushDriver);
int  FlushDataCache(PDEVICEHANDLE pDevHandle);
#endif
int  InvalidDataCache(PDEVICEHANDLE pDevHandle);

int  ReadDataFromFile(FILEHANDLE filehandle, PDEVICEHANDLE pDevHandle, u8 *pDataBuf);
u32  GetFATNode(PDEVICEHANDLE pDevHandle, u32  dwCluster);
int  ReadDirSectorFromDevice(PDEVICEHANDLE pDevHandle, u32 dwSector);
u32  GetDirSector(PDEVICEHANDLE pDevHandle, u32 dwPathCluster, u32 dwEntryCluster);

#ifdef __cplusplus
}
#endif

#endif /* SFS_FCACHE_H */
