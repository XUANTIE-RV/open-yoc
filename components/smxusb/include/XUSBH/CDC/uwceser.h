/*
* uwceser.h                                                 Version 2.40
*
* smxUSBH WinCE non-CDC/ACM Serial Class Driver.
*
* Copyright (c) 2009-2010 Micro Digital Inc.
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

#ifndef SU_WCESERIAL_H
#define SU_WCESERIAL_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int  su_WCESerialInit(void);
void su_WCESerialRelease(void);
void su_WCESerialRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

BOOLEAN  su_WCESerialInserted(uint iID);

int su_WCESerialOpen(uint iID);
int su_WCESerialClose(uint iID);

int su_WCESerialWrite(uint iID, u8 * pData, int len);
int su_WCESerialRead(uint iID, u8 *pData, int count);

#if defined(__cplusplus)
}
#endif

#endif /* SU_WCESERIAL_H */

