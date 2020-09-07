/*
* fpathlfn.h                                                Version 2.04
*
* smxFS Directory VFAT (Long File Name) Related Functions.
*
* Copyright (c) 2004-2009 Micro Digital Inc.
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

#ifndef SFS_FPATHLFN_H
#define SFS_FPATHLFN_H

#ifdef __cplusplus
extern "C" {
#endif 

void *GetLongDirEntry(PDEVICEHANDLE pDevHandle, u32 dwStartSector, uint dwOffset);
void  SetLongDirEntry(PDEVICEHANDLE pDevHandle, u32 dwStartSector, uint dwOffset);
int   IsLongFileName(char * pszFileName);
uint  GenerateShortName(PDEVICEHANDLE pDevHandle, FINDDIRENTRY *pFindDirEntry, char * szShortName);
int   FillLongDirEntry(PDEVICEHANDLE pDevHandle, uint iLongDirCounter, FINDDIRENTRY *pFindDirEntry,
                       uint dwOffset, u32 dwDirStartSector, char *szShortNameBuf);
int   DeleteLongDirEntry(PDEVICEHANDLE pDevHandle, u32 dwPathCluster, u32 dwEntryCluster, u32 dwOffset, u32 dwDirSector);
uint  GetLongDirEntryName(PDEVICEHANDLE pDevHandle, DIRENTRY* pDirEntry, char * pszLongName, 
                          char * pszShortName, uint dwOffset, u32 dwDirStartSector,
                          uint dwEntryNumber, u32 dwEntryCluster);
u8    GetReservedByte(char * pszShortName, uint * piLongDirCounter);
u8    ChkSum(u8 * pFcbName);

#ifdef __cplusplus
}
#endif

#endif /* SFS_FPATHLFN_H */
