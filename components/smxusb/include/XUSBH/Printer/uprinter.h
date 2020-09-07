/*
* uprinter.h                                                Version 2.40
*
* smxUSBH Printer Class Driver.
*
* Copyright (c) 2004-2010 Micro Digital Inc.
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

#ifndef SU_PRINTER_H
#define SU_PRINTER_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

#define SU_PRN_STATUS_PAPEREMPTY   0x20
#define SU_PRN_STATUS_SELECTED     0x10
#define SU_PRN_STATUS_NOERROR      0x08

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int  su_PrnID(uint iID, u8 * pData, int len);    
int  su_PrnInit(void);
void su_PrnRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

BOOLEAN  su_PrnInserted(uint iID);
int  su_PrnRead(uint iID, u8 * pData, int len);
void su_PrnRelease(void);
int  su_PrnReset(uint iID);    
u8   su_PrnStatus(uint iID);    
int  su_PrnWrite(uint iID, u8 * pData, int len);

#if defined(__cplusplus)
}
#endif

#endif /* SU_PRINTER_H */

