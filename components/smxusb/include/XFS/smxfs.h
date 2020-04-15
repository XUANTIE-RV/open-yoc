/*
* smxfs.h                                                   Version 1.40
*
* smxFS Main Header File. Includes all necessary header files for
* using smxFS, in the proper order.
*
* Copyright (c) 2005-2007 Micro Digital Inc.
* All rights reserved. www.smxrtos.com
*
* This software is confidential and proprietary to Micro Digital Inc.
* It has been furnished under a license and may be used, copied, or
* disclosed only in accordance with the terms of that license and with
* the inclusion of this header. No title to nor ownership of this
* software is hereby transferred.
*
* Author: David Moore
*
* Portable to any ANSI compliant C compiler.
*
*****************************************************************************/

#ifndef SFS_SMXFS_H
#define SFS_SMXFS_H

#include "smxbase.h"
#include "fcfg.h"
#include "fport.h"
#include "fapi.h"

#if SFS_DRV_WINDISK
#include "fdwin.h"
#endif

#if SFS_DRV_RAMDISK
#include "fdram.h"
#endif

#if SFS_DRV_USB
#include "fdusb.h"
#endif

#if SFS_DRV_MMCSD
#include "fdmsd.h"
#endif

#if SFS_DRV_NANDFLASH
#include "fdnand.h"
#endif

#if SFS_DRV_NORFLASH
#include "fdnor.h"
#endif

#if SFS_DRV_ATA
#include "fdata.h"
#endif

#if SFS_DRV_CF
#include "fdcf.h"
#endif

#if SFS_DRV_DOC
#include "fddoc.h"
#endif

#endif /* SFS_SMXFS_H */
