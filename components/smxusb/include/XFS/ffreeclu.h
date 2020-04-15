/*
* ffreeclu.h                                                Version 2.07
*
* smxFS Free Cluster Cache Function.
*
* Copyright (c) 2009 Micro Digital Inc.
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

#ifndef SFS_FFREECLU_H
#define SFS_FFREECLU_H

#if SFS_FREECLUS_SUPPORT

#ifdef __cplusplus
extern "C" {
#endif

int  FindMoreFreeClusters(PDEVICEHANDLE pDevHandle, uint iScanSectorNum);
int  PutFreeCluster(PDEVICEHANDLE pDevHandle, u32 dwFirstCluster);
u32  GetFreeCluster(PDEVICEHANDLE pDevHandle);
u32  GetFreeClustersNum(PDEVICEHANDLE pDevHandle);

#ifdef __cplusplus
}
#endif

#endif /* SFS_FREECLUS_SUPPORT */
#endif /* SFS_FFREECLU_H */
