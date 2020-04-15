/*
* unet.h                                                    Version 3.00
*
* smxUSBH USB to Ethernet Adapter Driver.
*
* Copyright (c) 2007-2018 Micro Digital Inc.
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

#ifndef SU_NET_H
#define SU_NET_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

typedef int (* PNETFUNC)(int port, u8 *pBuf, uint len);

/* for internal use only */
int  su_NetInit(void);
void su_NetRelease(void);
void su_NetRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

int  su_NetSendCtrl(uint iID, u16 wReq, u16 wValue, u16 wIndex, u8 * pData, int len);
int  su_NetRecvCtrl(uint iID, u16 wReq, u16 wValue, u16 wIndex, u8 * pData, int len);

/* called by application */
BOOLEAN  su_NetInserted(uint iID);
int  su_NetOpen(uint iID);
int  su_NetClose(uint iID);

int  su_NetWriteData(uint iID, u8 *pData, int len);
int  su_NetGetNodeID(int port, u8 *pData);
void su_NetRegisterPortNotify(int port, PNETFUNC handler);

#if !SB_MULTITASKING
void su_CheckNetData(uint iID);
#endif


#if defined(__cplusplus)
}
#endif


/*==========================================================================*/
#endif /* SU_NET_H */
