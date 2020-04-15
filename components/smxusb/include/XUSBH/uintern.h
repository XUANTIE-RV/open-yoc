/*
* uintern.h                                                 Version 3.00
*
* smxUSBH Internal Header File. Includes all header files needed internally
* by smxUSBH, in the proper order.
*
* Copyright (c) 2004-2018 Micro Digital Inc.
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

#ifndef SU_INTERN_H
#define SU_INTERN_H

#include "smxbase.h"
#include "ucfg.h"
#include "uport.h"
#include "uheap.h"
#include "uinit.h"
#include "ubase.h"
#include "ustruct.h"
#include "udevice.h"
#include "udriver.h"
#include "udesc.h"
#include "uhubbase.h"
#include "uhubthrd.h"
#if (SU_HUB & !SB_MULTITASKING)
#include "uhubdrv.h"
#endif

#if SU_OTG
#include "smxusbo.h"
#endif

#endif  /* SU_INTERN_H */

