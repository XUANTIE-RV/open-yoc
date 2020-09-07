/*
* umouse.h                                                  Version 2.51
*
* smxUSBH Mouse Class Driver.
*
* Copyright (c) 2004-2012 Micro Digital Inc.
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

#ifndef SU_MOUSE_H
#define SU_MOUSE_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

#define    SU_MOUSE_BTN_LEFT     0x01
#define    SU_MOUSE_BTN_RIGHT    0x02
#define    SU_MOUSE_BTN_MIDDLE   0x04
#define    SU_MOUSE_BTN_SIDE     0x08
#define    SU_MOUSE_BTN_EXTRA    0x10

#define    SU_MOUSE_WHEEL_UP     1
#define    SU_MOUSE_WHEEL_DOWN   -1

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct
{
    u16 button;
    s16 x;
    s16 y;
    s8  wheel;
    u8  x_resolution;
    u8  y_resolution;
}SU_MOUSE_MSG;

typedef void (* PMOUSEFUNC)(SU_MOUSE_MSG * mouseMsg);

int     su_MouseInit(void);
void    su_MouseRelease(void);
void    su_MouseRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

BOOLEAN su_MouseInserted(void);

void    su_MouseSetCallback(PMOUSEFUNC handler);

#if defined(__cplusplus)
}
#endif

#endif /* SU_MOUSE_H */

