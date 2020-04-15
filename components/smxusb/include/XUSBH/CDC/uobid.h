/*
* uobid.c                                                   Version 2.40
*
* smxUSBH FEIG OBID-iscan RFID Reader Driver.
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

#ifndef SU_OBID_H
#define SU_OBID_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int  su_OBIDInit(void);
void su_OBIDRelease(void);
void su_OBIDRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

BOOLEAN  su_OBIDInserted(uint iID);

int su_OBIDSendCtrl(uint iID, u8 *pData, int len);
int su_OBIDRecvCtrl(uint iID, u8 *pData, int len);

#if defined(__cplusplus)
}
#endif


/*==========================================================================*/
#endif /* SU_OBID_H */
