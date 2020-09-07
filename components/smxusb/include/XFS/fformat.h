/*
* fformat.h                                                 Version 2.25
*
* smxFS Format Related Functions.
*
* Copyright (c) 2004-2016 Micro Digital Inc.
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

#ifndef SFS_FFORMAT_H
#define SFS_FFORMAT_H

#ifdef __cplusplus
extern "C" {
#endif

#if SFS_FORMAT_SUPPORT
int   FormatDevice(PDEVICEHANDLE pDevHandle, FORMATINFO * pFormatInfo);
int   PartitionDevice(PDEVICEHANDLE pDevHandle, PARTITIONINFO * pPartitionInfo);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SFS_FFORMAT_H */
