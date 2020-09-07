/*
* fpath.h                                                   Version 2.21
*
* smxFS Directory Related Functions.
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

#ifndef SFS_FPATH_H
#define SFS_FPATH_H

#ifdef __cplusplus
extern "C" {
#endif 

void        ScanFreeClus(PDEVICEHANDLE pDevHandle);
void        SetTimeDate(DIRENTRY* pDir, uint iCreate);
void        GetTimeDate(DIRENTRY* pDir, DATETIME* pWrtTime, DATETIME* pCrtTime, DATETIME* pAccTime);
int         IsValidFilenameChar(char ch);
void        GetMode(FINDDIRENTRY* pFindDirEntry);
void *      GetDirEntry(PDEVICEHANDLE pDevHandle, u32 dwStartSector, uint dwOffset);
u8 *        GetDirEntryPtr(PDEVICEHANDLE pDevHandle, u32 dwStartSector, uint *pdwOffset);
int         FindFileByFullName(PDEVICEHANDLE pDevHandle, const char * pszFileName, FINDDIRENTRY* pFindDirEntry);
int         FindDirEntryByName(PDEVICEHANDLE pDevHandle, FINDDIRENTRY * pFindDirEntry);
uint        GetDirEntryName(PDEVICEHANDLE pDevHandle, DIRENTRY * pDirEntry, 
#if SFS_VFAT_SUPPORT
            char * pszLongName, 
#endif /* SFS_VFAT_SUPPORT */
            char * pszShortName, uint dwOffset, u32 dwDirStartSector, uint dwEntryNumber, u32 dwEntryCluster);
void        ParseShortFileName(DIRENTRY*   pDirEntry, char * pszShortName);
u32         GetPreviousDirSector(PDEVICEHANDLE pDevHandle, u32 dwPathCluster, u32 dwEntryCluster);
int         FreeFileHandle(PDEVICEHANDLE pDevHandle, FILEHANDLE filehandle);

#if !SFS_READONLY
int         SetDirEntry(PDEVICEHANDLE pDevHandle, u32 dwStartSector, uint dwOffset);
u32         AllocateFreeCluster(PDEVICEHANDLE pDevHandle);   
uint        TryAllocateFreeCluster(PDEVICEHANDLE pDevHandle, int iClusterNum);   
FILEHANDLE  CreateNewFile(PDEVICEHANDLE pDevHandle, FINDDIRENTRY *pFindDirEntry , uint iVolume);

int         EmptyFileByName(const char * filename, PDEVICEHANDLE pDevHandle, FINDDIRENTRY *pFindDirEntry, uint iFlush);
int         DiscardOneCluster(PDEVICEHANDLE pDevHandle,  u32 dwFileCluster);
int         CreateNewFileEntry(PDEVICEHANDLE pDevHandle, FINDDIRENTRY *pFindDirEntry, uint dwDirEntryNum, 
                               uint iLongDirCounter, u32 dwDirSector, u8 ReservedByte, uint iVolume);
int         CollectFreeDirEntry(PDEVICEHANDLE pDevHandle, FINDDIRENTRY* pFindDirEntry, uint iFlush);
int         ClearShortFileEntry(PDEVICEHANDLE pDevHandle,  u32 dwOffset, u32 dwDirSector, u32 dwEntryCluster, uint iRootDir);

#if SFS_MKDIR_SUPPORT
int         CreateDotDir(PDEVICEHANDLE pDevHandle, FINDDIRENTRY* pFindDirEntry);              
int         ClearDirCluster(PDEVICEHANDLE pDevHandle, u32 dwCluster);
int         MkDir(PDEVICEHANDLE pDevHandle, char *szFullPath);
#endif
       
#endif /* !SFS_READONLY */ 
FILEHANDLE  OpenExistedFile(PDEVICEHANDLE pDevHandle, FINDDIRENTRY *pFindDirEntry , const char * mode);
int         MoveToNextCluster(PDEVICEHANDLE pDevHandle, u32 *pdwDirCluster, u32 *pdwStartSector);
int         IsFAT1216Root(PDEVICEHANDLE pDevHandle, u32 dwPathCluster);

#ifdef __cplusplus
}
#endif

#endif /* SFS_FPATH_H */

