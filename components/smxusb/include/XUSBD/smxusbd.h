/*
* smxusbd.h                                                 Version 2.54
*
* smxUSBD Main Header File. Includes all necessary header files for
* using smxUSBD, in the proper order.
*
* Copyright (c) 2005-2012 Micro Digital Inc.
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

#ifndef SMX_USBD_H
#define SMX_USBD_H

#include "udcfg.h"
#include "udinit.h"

#if SUD_SERIAL
#include "Function/udserial.h"
#endif

#if SUD_MOUSE
#include "Function/udmouse.h"
#endif

#if SUD_MSTOR
#include "Function/udmstor.h"
#endif

#if SUD_RNDIS
#include "Function/udrndis.h"
#endif

#if SUD_AUDIO
#include "Function/udaudio.h"
#endif

#if SUD_KBD
#include "Function/udkbd.h"
#endif

#if SUD_VIDEO
#include "Function/udvideo.h"
#endif

#if SUD_MTP
#include "Function/udmtp.h"
#endif

#if SUD_DFU
#include "Function/uddfu.h"
#endif

#if SUD_HIDCOM
#include "Function/udhidcom.h"
#endif

#if SUD_FTEMPL
#include "Function/udftempl.h"
#endif

#endif /* SMX_USBD_H */

