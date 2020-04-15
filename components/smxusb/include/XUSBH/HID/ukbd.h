/*
* ukbd.h                                                    Version 3.00
*
* smxUSBH Keyboard Class Driver.
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

#ifndef SU_KBD_H
#define SU_KBD_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

#define SU_KBD_BYPASS  0   /* If 1, keyboard data is not parsed. It is forwarded to callback set by su_KbdSetRawCallback(). */

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

typedef void (* PKBDFUNC)(u32 key);
typedef void (* PKBDRAWFUNC)(u8 *key);

int     su_KbdInit(void);
void    su_KbdRelease(void);
void    su_KbdRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

BOOLEAN su_KbdInserted(void);
void    su_KbdSetCallback(PKBDFUNC handler);

#if SU_KBD_BYPASS
/* The following function should only be called in the bypass mode */
void    su_KbdSetLED(u8 bLED);
void    su_KbdSetRawCallback(PKBDRAWFUNC handler);
#endif

#if !SB_MULTITASKING
void    su_KbdCheckRepeatKey(void);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* SU_KBD_H */

