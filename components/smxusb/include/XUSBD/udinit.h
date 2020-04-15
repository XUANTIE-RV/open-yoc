/*
* udinit.h                                                  Version 2.55
*
* smxUSBD Initialization.
*
* Copyright (c) 2005-2013 Micro Digital Inc.
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

#ifndef SUD_INIT_H
#define SUD_INIT_H

#define SUD_AUDIO_MASK      (0x1 << 0)
#define SUD_DFU_MASK        (0x1 << 1)
#define SUD_ECM_MASK        (0x1 << 2)
#define SUD_KBD_MASK        (0x1 << 3)
#define SUD_HID_MASK        (0x1 << 4)
#define SUD_MOUSE_MASK      (0x1 << 5)
#define SUD_MSTOR_MASK      (0x1 << 6)
#define SUD_MTP_MASK        (0x1 << 7)
#define SUD_PRINTER_MASK    (0x1 << 8)
#define SUD_RNDIS_MASK      (0x1 << 9)
#define SUD_SERIAL_MASK     (0x1 << 10)
#define SUD_SMARTCARD_MASK  (0x1 << 11)
#define SUD_VIDEO_MASK      (0x1 << 12)
#define SUD_COMPOSITE_MASK  (0x1 << 14)
#define SUD_FTEMPL_MASK     (0x1 << 15)

#define SUD_ALL_MASK        0xFFFF

#define SUD_CB_CONFIGURED   0x1
#define SUD_CB_SUSPENDED    0x2

typedef void (* SUD_PSTACKEVTCBFUNC)(uint iFlag);

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

int  sud_USBInited(void);
int  sud_Initialize(uint iConfig);
void sud_Release(void);

void sud_RegStackCallback(SUD_PSTACKEVTCBFUNC pCallback);

int  sud_Reconfig(uint iNewFunction);
int  sud_ReAttach(void);

#ifdef __cplusplus
}
#endif

#endif /* SUD_INIT_H */

