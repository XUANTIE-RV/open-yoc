/*
* ucdcacm.h                                                 Version 2.55
*
* smxUSBH CDC-ACM Class Driver.
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

#ifndef SU_CDCACM_H
#define SU_CDCACM_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

/* Output and Input control lines and line errors. */
#define SU_CDCACM_LINE_IN_DCD                 0x01
#define SU_CDCACM_LINE_IN_DSR                 0x02
#define SU_CDCACM_LINE_IN_BRK                 0x04
#define SU_CDCACM_LINE_IN_RI                  0x08
#define SU_CDCACM_LINE_IN_FRAMING             0x10
#define SU_CDCACM_LINE_IN_PARITY              0x20
#define SU_CDCACM_LINE_IN_OVERRUN             0x40

#define SU_CDCACM_LINE_OUT_DTR                0x01
#define SU_CDCACM_LINE_OUT_RTS                0x02

/* Line coding for parity */
#define SU_CDCACM_PARITY_NONE                 0
#define SU_CDCACM_PARITY_ODD                  1
#define SU_CDCACM_PARITY_EVEN                 2
#define SU_CDCACM_PARITY_MARK                 3
#define SU_CDCACM_PARITY_SPACE                4

/* Line coding for stop bits */
#define SU_CDCACM_STOP_BITS_1                 0
#define SU_CDCACM_STOP_BITS_1_5               1
#define SU_CDCACM_STOP_BITS_2                 2

/* Feature Selector */
#define SU_CDCACM_FEATURE_ABSTRACT_STATE      0x01
#define SU_CDCACM_FEATURE_COUNTRY_SETTING     0x02

/* Notify type */
#define SU_CDCACM_CONNECT_CHANGED             0x01
#define SU_CDCACM_LINE_STATE_CHANGED          0x02
#define SU_CDCACM_RESPONSE_READY              0x04

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int  su_CDCACMInit(void);
void su_CDCACMRelease(void);
void su_CDCACMRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

typedef void (* PHOSTCDCACMFUNC)(uint iID, uint wNotifyType, u16 wParam);

BOOLEAN  su_CDCACMInserted(uint iID);

int su_CDCACMOpen(uint iID);
int su_CDCACMClose(uint iID);

int su_CDCACMWrite(uint iID, u8 * pData, uint count);
int su_CDCACMRead(uint iID, u8 *pData, uint count, uint timeout);

int su_CDCACMSetLineState(uint iID, uint iState);

int su_CDCACMGetLineState(uint iID, uint *piState);
int su_CDCACMGetLineCoding(uint iID, u32 *pdwDTERate, u8 *pbParityType,
                           u8 *pbDataBits, u8 *pbStopBits);

int su_CDCACMSendLineBreak(uint iID, u16 ms);
int su_CDCACMSetLineCoding(uint iID, u32 dwBaudrate, u8 iParity, u8 iDataBits, u8 iStopBit);

int su_CDCACMSetCommFeature(uint iID, u16 wSelector, u16 wStatus);
int su_CDCACMGetCommFeature(uint iID, u16 wSelector, u16 *pwStatus);

void su_CDCACMRegisterStateChangeNotify(PHOSTCDCACMFUNC handler);

#if defined(__cplusplus)
}
#endif

#endif /* SU_CDCACM_H */

