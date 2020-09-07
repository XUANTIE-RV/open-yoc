/*
* uftdi232.h                                                Version 3.00
*
* smxUSBH FTDI 232B/LC/R USB to Serial Converter Driver.
*
* Copyright (c) 2006-2018 Micro Digital Inc.
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

#ifndef SU_FTDI232_H
#define SU_FTDI232_H

/*============================================================================
                                   CONSTANTS
============================================================================*/
/* Line Coding Value */
#define SU_FTDI232_PARITY_NONE           (0x0 << 8 )
#define SU_FTDI232_PARITY_ODD            (0x1 << 8 )
#define SU_FTDI232_PARITY_EVEN           (0x2 << 8 )
#define SU_FTDI232_PARITY_MARK           (0x3 << 8 )
#define SU_FTDI232_PARITY_SPACE          (0x4 << 8 )
#define SU_FTDI232_STOP_BITS_1           (0x0 << 11 )
#define SU_FTDI232_STOP_BITS_15          (0x1 << 11 )
#define SU_FTDI232_STOP_BITS_2           (0x2 << 11 )

/* Flow Control Settings */
#define SU_FTDI232_FLOWCTRL_NONE         0x0 
#define SU_FTDI232_FLOWCTRL_RTSCTS       (0x1 << 8)
#define SU_FTDI232_FLOWCTRL_DTRDSR       (0x2 << 8)
#define SU_FTDI232_FLOWCTRL_XONXOFF      (0x4 << 8)

/* Modem Control, DTR and RTS */
#define SU_FTDI232_DTR                   (1 | ( 0x1  << 8))
#define SU_FTDI232_RTS                   (2 | ( 0x2 << 8 ))

/* wValue for MODEMSTATUS */
#define SU_FTDI232_CTS                   0x10
#define SU_FTDI232_DSR                   0x20
#define SU_FTDI232_RI                    0x40
#define SU_FTDI232_RLSD                  0x80

/* Line status */
#define SU_FTDI232_DR                    0x01
#define SU_FTDI232_OE                    0x02
#define SU_FTDI232_PE                    0x04
#define SU_FTDI232_FE                    0x08
#define SU_FTDI232_BI                    0x10
#define SU_FTDI232_THRE                  0x20
#define SU_FTDI232_TEMT                  0x40
#define SU_FTDI232_FIFOE                 0x80

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int  su_FTDIInit(void);
void su_FTDIRelease(void);
void su_FTDIRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

BOOLEAN  su_FTDIInserted(uint iID);

int  su_FTDIOpen(uint iID);
int  su_FTDIClose(uint iID);
int  su_FTDIWriteData(uint iID, u8 *pData, int len);
int  su_FTDIReadData(uint iID, u8 *pData, int len);
int  su_FTDISetModemCtrl(uint iID, uint Data);
int  su_FTDISetFlowCtrl(uint iID, uint Data);
int  su_FTDISetLineCoding(uint iID, u32 dwDTERate, u16 wParityType,
                          u16 wDataBits, u16 wStopBits);
int  su_FTDIGetModemStatus(uint iID);
int  su_FTDIGetStatus(uint iID, u8 *pModemStatus, u8 *pLineStatus);
int  su_FTDISetEventChar(uint iID, u8 cData);
int  su_FTDISetErrorChar(uint iID, u8 cData);
int  su_FTDISetLatencyTimer(uint iID, uint iMS);
int  su_FTDIGetLatencyTimer(uint iID);


#if defined(__cplusplus)
}
#endif


/*==========================================================================*/
#endif /* SU_FTDI232_H */
