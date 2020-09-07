/*
* fpathext.h                                                Version 2.21
*
* smxFS Directory Related Extension Functions.
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

#ifndef SFS_FPATHEXT_H
#define SFS_FPATHEXT_H

#ifdef __cplusplus
extern "C" {
#endif 


#if SFS_RENAME_SUPPORT
int  RenameFile(const char * newname, const char * oldname, PDEVICEHANDLE pDevHandle, 
                FINDDIRENTRY *  pFindDirEntryNew, u32 dwPathCluster, u32 dwEntryCluster, uint dwOffset);
#endif

#if SFS_CLN_SHUTDOWN_SUPPORT
int  SetNotClnShutDownFlag(PDEVICEHANDLE pDevHandle, uint iSet);
int  IsWritePendingFileExist(PDEVICEHANDLE pDevHandle, FILEHANDLE filehandle);
#endif

#if SFS_VOLUME_SUPPORT
int  FindVolumeDir(PDEVICEHANDLE pDevHandle, FINDDIRENTRY * pFindDirEntry);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SFS_FPATHEXT_H */
