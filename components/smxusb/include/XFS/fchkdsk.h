/*
* fchkdsk.h                                                 Version 2.00
*
* smxFS Check Disk function.
*
* Copyright (c) 2007-2008 Micro Digital Inc.
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

#ifndef SFS_FCHKDSK_H
#define SFS_FCHKDSK_H

#if SFS_CHKDSK_SUPPORT

#ifdef __cplusplus
extern "C" {
#endif

int CheckDisk(PDEVICEHANDLE pDevHandle, uint iFixFlag, char *pErrDescBuf, uint iBufLen);

#ifdef __cplusplus
}
#endif

#endif /* SFS_CHKDSK_SUPPORT */
#endif /* SFS_FCHKDSK_H */
