/*
* upl2303.h                                                 Version 2.40
*
* smxUSBH Prolific 2303HXD USB to Serial Converter Driver.
*
* Copyright (c) 2008-2010 Micro Digital Inc.
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

#ifndef SU_PL2303_H
#define SU_PL2303_H

/*============================================================================
                                   CONSTANTS
============================================================================*/
/* Output and Input control lines and line errors. */
#define SU_PL2303_LINE_IN_DCD            0x01
#define SU_PL2303_LINE_IN_DSR            0x02
#define SU_PL2303_LINE_IN_BRK            0x04
#define SU_PL2303_LINE_IN_RI             0x08
#define SU_PL2303_LINE_IN_FRAMING        0x10
#define SU_PL2303_LINE_IN_PARITY         0x20
#define SU_PL2303_LINE_IN_OVERRUN        0x40
#define SU_PL2303_LINE_IN_CTS            0x80
#define SU_PL2303_LINE_OUT_DTR           0x1
#define SU_PL2303_LINE_OUT_RTS           0x2


/* Line coding for parity */
#define SU_PL2303_PARITY_NONE            0
#define SU_PL2303_PARITY_ODD             1
#define SU_PL2303_PARITY_EVEN            2
#define SU_PL2303_PARITY_MARK            3
#define SU_PL2303_PARITY_SPACE           4

/* Line coding for stop bits */
#define SU_PL2303_STOP_BITS_1            0
#define SU_PL2303_STOP_BITS_1_5          1
#define SU_PL2303_STOP_BITS_2            2

/* Flow Control Settings */
#define SU_PL2303_FLOWCTRL_NONE          0x0 
#define SU_PL2303_FLOWCTRL_RTSCTS        0x1

#define SU_PL2303_MTU                    512

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int  su_PL2303Init(void);
void su_PL2303Release(void);
void su_PL2303RegDevEvtCallback(SU_PDEVEVTCBFUNC func);

typedef void (* PHOSTPL2303FUNC)(uint iID, u16 wNewLineState);

BOOLEAN  su_PL2303Inserted(uint iID);

int  su_PL2303Open(uint iID);
int  su_PL2303Close(uint iID);
int  su_PL2303WriteData(uint iID, u8 *pData, uint len);
int  su_PL2303ReadData(uint iID, u8 *pData, uint len, uint timeout);
int  su_PL2303SetModemCtrl(uint iID, uint Data);
int  su_PL2303SetFlowCtrl(uint iID, uint Data);
int  su_PL2303SetLineCoding(uint iID, u32 dwDTERate, u16 wParityType,
                          u16 wDataBits, u16 wStopBits);
void su_PL2303LineStateChangeNotify(PHOSTPL2303FUNC handler);


#if defined(__cplusplus)
}
#endif


/*==========================================================================*/
#endif /* SU_PL2303_H */
