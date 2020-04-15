/*
* uhubthrd.h                                                Version 2.53
*
* smxUSBH Hub Thread.
*
* Copyright (c) 2004-2013 Micro Digital Inc.
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

#ifndef SU_HUB_THREAD_H
#define SU_HUB_THREAD_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#if defined(__cplusplus)
extern "C" {
#endif

int  su_HubThreadInit(void);
void su_HubThreadRelease(void);
void su_HubEnableRoothubPorts(BOOLEAN bEnable);

void su_HubWakeupThread(int roothub, int index);

#if defined(__cplusplus)
}
#endif

#endif /* SU_HUB_THREAD_H */
