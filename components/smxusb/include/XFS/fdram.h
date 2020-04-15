/*
* fdram.h                                                   Version 2.24
*
* smxFS RAM Disk Device Driver.
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

#ifndef SFS_FDRAM_H
#define SFS_FDRAM_H

#define RAMDISK_NUM    1   /* Specify 1 or 2 RAM disks. If 2 remember to add sfs_devreg() to initmods.c. */

#ifdef __cplusplus
extern "C" {
#endif

extern const SB_BD_IF * sfs_GetRAM0Interface(void); 
#if (RAMDISK_NUM == 2)
extern const SB_BD_IF * sfs_GetRAM1Interface(void); 
#endif

#ifdef __cplusplus
}
#endif

#endif /* SFS_FDRAM_H */

