/*
* uhubdrv.h                                                 Version 3.00
*
* smxUSBH External Hub Driver.
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

#ifndef SU_HUB_DRV_H
#define SU_HUB_DRV_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

/*============================================================================
                          STRUCTURES AND OTHER TYPEDEFS
============================================================================*/

typedef struct
{
    SU_LIST threadEventList;
    SU_DEV_INFO * pDev;
    SU_REQ_INFO * pReqInfo;   /* the INT EP for polling the hub status */   
    u8  buffer[SU_HUB_DEVNUM]; 
    int error;
    int errCount;
    SU_HUB_DESCINFO_T desc;
    SU_HUB_TT TT;
}SU_HUB_DEVINFO_T;

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

#if SU_HUB

void su_HubExtProcess(void);
int  su_HubSetPortPower(SU_DEV_INFO *pParentDevInfo, uint iHubAddr, uint portNum, BOOLEAN bEnable);
int  su_HubResetPort(SU_DEV_INFO *pParentDevInfo, uint iHubAddr, uint portNum, BOOLEAN bReset);
int  su_HubSetPortTest(SU_DEV_INFO *pParentDevInfo, uint iHubAddr, uint portNum, uint test);

#if !SB_MULTITASKING
/* external hub support */
int  su_HubGetExtFlag(void);
void su_HubClearExtFlag(void);
#endif

int  su_HubInit(void);
void su_HubRelease(void);

#endif

#if defined(__cplusplus)
}
#endif

#endif /* SU_HUB_DRV_H */

