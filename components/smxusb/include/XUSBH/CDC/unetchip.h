/*
* unetchip.h                                                Version 3.00
*
* smxUSBH USB to Ethernet Adapter Chip Driver.
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

#ifndef SU_NETCHIP_H
#define SU_NETCHIP_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

#define SU_NET_CHIP_ASIX    1
#define SU_NET_TOTAL_CHIP_NUM (SU_NET_CHIP_ASIX)
/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct
{
    int (*Init) (void);
    int (*Release) (void);
    int (*AddPktHdr) (SU_DEV_INFO *pDev, SU_REQ_INFO *pReqInfo, u8 *pData, uint len);
    int (*GetOnePkt) (SU_DEV_INFO *pDev, SU_REQ_INFO *pReqInfo, u8 *pData, uint len);
    int (*GetEthernetID) (u8 *pMACAddr);
    SU_CHK_DEV * (*GetCheckDevID) (uint *piSize);
}SU_NET_CHIP_OPER_T;

#if SU_NET_CHIP_ASIX
SU_NET_CHIP_OPER_T * su_GetNetASIXOper(void);
#endif

/* add more chip here */

#if defined(__cplusplus)
}
#endif


/*==========================================================================*/
#endif /* SU_NETCHIP_H */
