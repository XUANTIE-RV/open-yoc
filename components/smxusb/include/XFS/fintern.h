/*
* fintern.h                                                 Version 2.07
*
* smxFS Internal Header File. Includes all header files needed internally
* by smxFS, in the proper order.
*
* Copyright (c) 2005-2009 Micro Digital Inc.
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

#ifndef SFS_FINTERN_H
#define SFS_FINTERN_H

#include "smxbase.h"
#include "fcfg.h"
#include "fport.h"
#include "fapi.h"
#include "fconst.h"
#include "fstruc.h"
#include "fcache.h"
#if SFS_CHKDSK_SUPPORT
#include "fchkdsk.h"
#endif
#if SFS_FINDFIRST_SUPPORT
#include "ffind.h"
#endif
#if SFS_FORMAT_SUPPORT
#include "fformat.h"
#endif
#include "fmount.h"
#include "fpath.h"
#if SFS_VFAT_SUPPORT
#include "fpathlfn.h"
#endif
#if SFS_FULL_FEATURES_SUPPORT
#include "fpathext.h"
#endif
#if SFS_VFAT_SUPPORT
#include "funicode.h"
#endif
#if SFS_FREECLUS_SUPPORT
#include "ffreeclu.h"
#endif

#endif /* SFS_FINTERN_H */
