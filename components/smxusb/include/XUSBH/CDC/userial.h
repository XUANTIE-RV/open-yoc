/*
* userial.h                                                 Version 2.55
*
* smxUSBH Serial Class Driver.
*
* Copyright (c) 2006-2014 Micro Digital Inc.
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

#ifndef SU_SERIAL_H
#define SU_SERIAL_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

/* Output and Input control lines and line errors. */
#define SU_SERIAL_LINE_IN_DCD                 0x01
#define SU_SERIAL_LINE_IN_DSR                 0x02
#define SU_SERIAL_LINE_IN_BRK                 0x04
#define SU_SERIAL_LINE_IN_RI                  0x08
#define SU_SERIAL_LINE_IN_FRAMING             0x10
#define SU_SERIAL_LINE_IN_PARITY              0x20
#define SU_SERIAL_LINE_IN_OVERRUN             0x40

/* Line coding for parity */
#define SU_SERIAL_PARITY_NONE                 0
#define SU_SERIAL_PARITY_ODD                  1
#define SU_SERIAL_PARITY_EVEN                 2
#define SU_SERIAL_PARITY_MARK                 3
#define SU_SERIAL_PARITY_SPACE                4

/* Line coding for stop bits */
#define SU_SERIAL_STOP_BITS_1                 0
#define SU_SERIAL_STOP_BITS_1_5               1
#define SU_SERIAL_STOP_BITS_2                 2


/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int  su_SerialInit(void);
void su_SerialRelease(void);
void su_SerialRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

typedef void (* PHOSTSERIALFUNC)(uint iID, u16 wNewLineState);

typedef void (* PSERIALDATAREADYFUNC)(uint iID, u8 *pData, uint iLen);

BOOLEAN  su_SerialInserted(uint iID);

int su_SerialOpen(uint iID);
int su_SerialClose(uint iID);

int su_SerialWrite(uint iID, u8 * pData, int len);
int su_SerialRead(uint iID, u8 *pData, int count, uint iTimeout);

int su_SerialGetLineState(uint iID, uint *piState);
int su_SerialGetLineCoding(uint iID, u32 *pdwDTERate, u8 *pbParityType,
                           u8 *pbDataBits, u8 *pbStopBits);

int su_SerialSendLineBreak(uint iID, u16 ms);
int su_SerialSetLineCoding(uint iID, u32 dwBaudrate, u8 iParity, u8 iDataBits, u8 iStopBit);

void su_SerialLineStateChangeNotify(PHOSTSERIALFUNC handler);

#if SB_MULTITASKING
void su_SerialDataReadyNotify(PSERIALDATAREADYFUNC handler);
int  su_SerialStartPollingTask(uint iID);
int  su_SerialStopPollingTask(uint iID);
BOOLEAN  su_SerialPollingTaskStarted(uint iID);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* SU_SERIAL_H */

