/*
* udmouse.h                                                 Version 2.53
*
* smxUSBD Mouse Function Driver.
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

#ifndef SUD_MOUSE_H
#define SUD_MOUSE_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#define    SUD_MOUSE_BTN_LEFT     0x01
#define    SUD_MOUSE_BTN_RIGHT    0x02
#define    SUD_MOUSE_BTN_MIDDLE   0x04
#define    SUD_MOUSE_BTN_SIDE     0x08
#define    SUD_MOUSE_BTN_EXTRA    0x10

#define    SUD_MOUSE_WHEEL_UP     (1)
#define    SUD_MOUSE_WHEEL_DOWN   (-1)

/* used by smxUSB internally */
int  sud_MouseInit(void);
void sud_MouseRelease(void);

#if SUD_COMPOSITE
/* for composite device only */
void *sud_MouseGetOps(void);
void *sud_MouseGetInterface(void);
#endif

/* used by high level application */
int  sud_MouseInput(u16 button, s16 x, s16 y, s8 wheel);

#ifdef __cplusplus
}
#endif

#endif  /* SUD_MOUSE_H */

