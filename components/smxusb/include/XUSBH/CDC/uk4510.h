/*
* uk4510.h                                                  Version 2.55
*
* smxUSBH Huawei K4510 3G Wireless Dongle Driver.
*
* Copyright (c) 2012-2014 Micro Digital Inc.
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

#ifndef SU_K4510_H
#define SU_K4510_H

#define SU_K4510_NS_REG_NOT            0
#define SU_K4510_NS_REG_LOCAL          1
#define SU_K4510_NS_SEARCHING          2
#define SU_K4510_NS_REG_REJECTED       3
#define SU_K4510_NS_REG_UNKNOWN        4
#define SU_K4510_NS_REG_ROAMING        5

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int  su_K4510Init(void);
void su_K4510Release(void);
void su_K4510RegDevEvtCallback(SU_PDEVEVTCBFUNC func);

BOOLEAN  su_K4510Inserted(uint iID);
BOOLEAN  su_K4510Connected(uint iID);

int su_K4510CheckNetworkState(uint iID);
BOOLEAN su_K4510GetNetworkName(uint iID, char *pName);

int su_K4510Open(uint iID);
int su_K4510Close(uint iID);

int su_K4510SendATCmd(uint iID, u8 * pData, int len, int timeout);
int su_K4510SendATCmdAndWait(uint iID, u8 * pData, int len);

int su_K4510Write(uint iID, u8 * pData, int len, int timeout);
int su_K4510Read(uint iID, u8 *pData, int len, int timeout);
int su_K4510PollRead(uint iID, u8 *pData, int len, int timeout);

#if defined(__cplusplus)
}
#endif

#endif /* SU_K4510_H */

