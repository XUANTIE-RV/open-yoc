/*
* usierra.h                                                 Version 2.40
*
* smxUSBH Sierra Wireless Dongle Driver.
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

#ifndef SU_SIERRA_H
#define SU_SIERRA_H

/*============================================================================
                                   CONSTANTS
============================================================================*/
/* Output and Input control lines and line errors. */
#define SU_SIERRA_LINE_IN_DCD                 0x01
#define SU_SIERRA_LINE_IN_DSR                 0x02
#define SU_SIERRA_LINE_IN_BRK                 0x04
#define SU_SIERRA_LINE_IN_RI                  0x08
#define SU_SIERRA_LINE_IN_FRAMING             0x10
#define SU_SIERRA_LINE_IN_PARITY              0x20
#define SU_SIERRA_LINE_IN_OVERRUN             0x40

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int  su_SierraInit(void);
void su_SierraRelease(void);
void su_SierraRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

typedef void (* SU_PSIERRAFUNC)(uint iID, u16 wNewLineState);
void su_SierraLineStateChangeNotify(SU_PSIERRAFUNC handler);

BOOLEAN  su_SierraInserted(uint iID);

int su_SierraOpen(uint iID);
int su_SierraClose(uint iID);

int su_SierraWrite(uint iID, uint iPort, u8 * pData, int len, int timeout);
int su_SierraRead(uint iID, uint iPort, u8 *pData, int len, int timeout);


#if defined(__cplusplus)
}
#endif

#endif /* SU_SIERRA_H */

