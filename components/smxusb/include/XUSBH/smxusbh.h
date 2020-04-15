/*
* smxusbh.h                                                 Version 3.00
*
* smxUSBH Main Header File. Includes all necessary header files for
* using smxUSBH, in the proper order.
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

#ifndef SMX_USB_H
#define SMX_USB_H

#include "smxbase.h"
#include "ucfg.h"
#include "uinit.h"

#if SU_MSTOR
#include "MassStor/umsintf.h"
#endif

#if SU_MOUSE
#include "HID/umouse.h"
#endif

#if SU_KBD
#include "HID/ukbd.h"
#endif

#if SU_PRINTER
#include "Printer/uprinter.h"
#endif

#if SU_SERIAL
#include "CDC/userial.h"
#endif

#if SU_CDCACM
#include "CDC/ucdcacm.h"
#endif

#if SU_FTDI232
#include "CDC/uftdi232.h"
#endif

#if SU_AUDIO
#include "Audio/uaudio.h"
#endif

#if SU_HID
#include "HID/uhid.h"
#endif

#if SU_K4510
#include "CDC/uk4510.h"
#endif

#if SU_NET
#include "CDC/unet.h"
#endif

#if SU_NOVATEL
#include "CDC/unovatel.h"
#endif

#if SU_UF8000
#include "Printer/uuf8000.h"
#endif

#if SU_RT250X
#include "CDC/urt250x.h"
#endif

#if SU_PL2303
#include "CDC/upl2303.h"
#endif

#if SU_OBID
#include "CDC/uobid.h"
#endif

#if SU_WCESERIAL
#include "CDC/uwceser.h"
#endif

#if SU_SIERRA
#include "CDC/usierra.h"
#endif

#if SU_RT2870
#include "CDC/urt2870.h"
#endif

#if SU_RT3070
#include "CDC/urt3070.h"
#endif

#if SU_RT3572
#include "CDC/urt3572.h"
#endif

#if SU_RT5370
#include "CDC/urt5370.h"
#endif

#if SU_RT5572
#include "CDC/urt5572.h"
#endif

#if SU_RTL81XX
#include "CDC/urtl81xx.h"
#endif

#if SU_V20K13
#include "Video/uv20k13.h"
#endif

#if SU_VIDEO
#include "Video/uvideo.h"
#endif

#if SU_CCID
#include "CCID/uccid.h"
#endif

#if SU_MT7601
#include "CDC/umt7601.h"
#endif

#if SU_CTEMPL
#include "CTempl/uctempl.h"
#endif

#if SU_TUSBH
#include "TUSBH/tusbh.h"
#endif

#endif /* SMX_USB_H */

