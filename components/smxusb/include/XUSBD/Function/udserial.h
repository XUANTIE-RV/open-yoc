/*
* udserial.h                                                Version 2.57
*
* smxUSBD Serial Port Function Driver.
*
* Copyright (c) 2005-2016 Micro Digital Inc.
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

#ifndef SUD_SERIAL_H
#define SUD_SERIAL_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#define SUD_CDC_NOTIFY_DATA_READY           0
#define SUD_CDC_NOTIFY_LINE_STATE_CHANGE    1
#define SUD_CDC_NOTIFY_SET_LINE_CODING      2

/* Output and Input control lines and line errors. */
#define SUD_CDC_LINE_OUT_DTR                0x01
#define SUD_CDC_LINE_OUT_RTS                0x02
#define SUD_CDC_LINE_IN_DCD                 0x01
#define SUD_CDC_LINE_IN_DSR                 0x02
#define SUD_CDC_LINE_IN_BRK                 0x04
#define SUD_CDC_LINE_IN_RI                  0x08
#define SUD_CDC_LINE_IN_FRAMING             0x10
#define SUD_CDC_LINE_IN_PARITY              0x20
#define SUD_CDC_LINE_IN_OVERRUN             0x40

/* Line coding for parity */
#define SUD_CDC_PARITY_NONE                 0
#define SUD_CDC_PARITY_ODD                  1
#define SUD_CDC_PARITY_EVEN                 2
#define SUD_CDC_PARITY_MARK                 3
#define SUD_CDC_PARITY_SPACE                4

/* Line coding for stop bits */
#define SUD_CDC_STOP_BITS_1                 0
#define SUD_CDC_STOP_BITS_1_5               1
#define SUD_CDC_STOP_BITS_2                 2

typedef void (* PSERIALFUNC)(int port, int notification);

/* used by smxUSB internally */
int  sud_SerialInit(void);
void sud_SerialRelease(void);

#if SUD_COMPOSITE
/* for composite device only */
void *sud_SerialGetOps(void);
void *sud_SerialGetInterface(void);
#if SUD_SERIAL_SUPPORT_ACM
int sud_SerialAdjustUnionFuncDesc(uint i);
#endif
#endif

/* used by high level application */
int  sud_SerialIsPortConnected(int port);
int  sud_SerialWriteData(int port, u8 *pBuf, uint len);
int  sud_SerialDataLen(int port);
int  sud_SerialReadData(int port, u8 *pBuf, uint len);
int  sud_SerialSetLineState(int port, uint iState);
int  sud_SerialGetLineState(int port, uint *piState);
int  sud_SerialGetLineCoding(int port, u32 *pdwDTERate, u8 *pbParityType,
                             u8 *pbDataBits, u8 *pbStopBits);
void sud_SerialRegisterPortNotify(int port, PSERIALFUNC handler);

#ifdef __cplusplus
}
#endif

#endif  /* SUD_SERIAL_H */

