/*
* uctempl.h                                                 Version 2.40
*
* smxUSBH Class Driver Template.
*
* Copyright (c) 2006-2010 Micro Digital Inc.
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

#ifndef SU_CTEMPLE_H
#define SU_CTEMPLE_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int  su_CTemplInit(void);
void su_CTemplRelease(void);
void su_CTemplRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

typedef void (* PCTEMPLFUNC)(void *pData, uint len);

BOOLEAN  su_CTemplInserted(uint iID);

int su_CTemplSendCtrl(uint iID, u8 * pData, int len);
int su_CTemplSendBulk(uint iID, u8 * pData, int len);
int su_CTemplSendINT(uint iID, u8 * pData, int len);

int su_CTemplRecvCtrl(uint iID, u8 * pData, int len);
int su_CTemplRecvBulk(uint iID, u8 *pData, int count);
int su_CTemplRecvINT(uint iID, u8 *pData, int count);

void su_CTemplSetCallback(PCTEMPLFUNC handler);


#if defined(__cplusplus)
}
#endif


/*==========================================================================*/
#endif /* SU_CTEMPLE_H */
