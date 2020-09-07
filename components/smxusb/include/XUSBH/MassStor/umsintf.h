/*
* umsintf.h                                                 Version 3.00
*
* smxUSBH Mass Storage Class Driver File System Interface.
*
* Copyright (c) 2004-2018 Micro Digital Inc.
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

#ifndef SU_MS_INTF_H
#define SU_MS_INTF_H

/*============================================================================
                                   CONSTANTS
============================================================================*/
/* return error code */
#define        SU_MS_E_OK                0    /*no error*/
#define        SU_MS_E_PROTECTED         1    /*card is write protected*/
#define        SU_MS_E_INITIALISATION    2    /*error during initialisation*/
#define        SU_MS_E_TRANSFER          3    /*error during transfer*/

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#if defined(__cplusplus)
extern "C" {
#endif

void     su_MSRegDevEvtCallback(SU_PDEVEVTCBFUNC func);
int      su_MStorIO(uint iID, uint iLUN, u8* pRAMAddr, u32 nStartSector, u16 nSectors, BOOLEAN Reading);
BOOLEAN  su_MStorMediaInserted(uint iID, uint iLUN);
BOOLEAN  su_MStorMediaRemoved(uint iID, uint iLUN);
u32      su_MStorSectorNum(uint iID, uint iLUN);
u32      su_MStorSectorSize(uint iID, uint iLUN);
BOOLEAN  su_MStorMediaChanged(uint iID, uint iLUN, BOOLEAN nClearStatus);
BOOLEAN  su_MStorMediaProtected(uint iID, uint iLUN);
BOOLEAN  su_MStorTestReady(uint iID, uint iLUN);
int      su_MStorMaxLUN(uint iID);
char *   su_MStorGetManufactureInfo(uint iID);
char *   su_MStorGetProductInfo(uint iID);
char *   su_MStorGetSerialInfo(uint iID);

#if defined(__cplusplus)
}
#endif

#endif  /* SU_MS_INTF_H */

