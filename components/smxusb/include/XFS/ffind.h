/*
* ffind.h                                                   Version 2.04
*
* smxFS findfirst/findnext Related Functions.
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

#ifndef SFS_FFIND_H
#define SFS_FFIND_H

#if SFS_FINDFIRST_SUPPORT

#ifdef __cplusplus
extern "C" {
#endif

int         ProcessFileSpec(FILEINFO* pFindDirEntry, char * filespec);
int         FindFile(PDEVICEHANDLE pDevHandle, int id, FILEINFO* fileinfo);

#ifdef __cplusplus
}
#endif

#endif /* SFS_FINDFIRST_SUPPORT */
#endif /* SFS_FFIND_H */
