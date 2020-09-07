/*
* udintern.h                                                Version 2.34
*
* smxUSBD Internal Header File. Includes all header files needed internally
* by smxUSBD, in the proper order.
*
* Copyright (c) 2005-2008 Micro Digital Inc.
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

#ifndef SUD_INTERN_H
#define SUD_INTERN_H
#include "smxbase.h"
#include "udcfg.h"
#include "udport.h"
#include "udheap.h"
#include "udinit.h"
#include "udutil.h"
#include "uddesc.h"
#include "uddevice.h"
#include "udfunc.h"
#include "uddcd.h"
#include "udep0.h"

#if SUD_OTG
#include "smxusbo.h"
#endif

#endif /* SUD_INTERN_H */

