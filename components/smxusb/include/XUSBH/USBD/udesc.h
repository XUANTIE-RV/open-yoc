/*
* udesc.h                                                   Version 3.00
*
* smxUSBH Device Framework Enumeration.
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

#ifndef SU_DESC_H
#define SU_DESC_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#if defined(__cplusplus)
extern "C" {
#endif

int  su_SetDevAddr(SU_DEV_INFO *pDevInfo);
int  su_GetDescString(SU_DEV_INFO *pDevInfo, 
                              u16 ReqIndex, u8 ReqValue, 
                              void *pDataBuf, uint ReqSize);
int  su_GetDesc(SU_DEV_INFO * pDevInfo, u8 descType, u8 ReqValue, 
                              void * pDataBuf, uint ReqSize);
int  su_SetDevCfgByIndex(SU_DEV_INFO * pDevInfo, int configIndex);
int  su_SetDevIntfByNum(SU_DEV_INFO * pDevInfo, uint inum, uint activeSetting);
void su_FreeEnumInfo(SU_DEV_INFO * pDevInfo);
void su_SetMaxPktSizes(SU_DEV_INFO * pDevInfo);
uint su_PktMaxLen(SU_DEV_INFO * pDevInfo, u32 pipe);
int  su_DupDescriptionString(SU_DEV_INFO * pDevInfo, int position, 
                               u8 * pStringBuf, uint stringLen);
int  su_EnumNewDev(SU_DEV_INFO * pDevInfo);

void su_PackSetupReq(SU_SDP *pReq, void *pMem);
void su_UnpackSetupReq(SU_SDP *pReq, void *pMem);
void su_UnpackDescHdr(SU_DESC_HDR *pDesc, void *pMem);
void su_UnpackClassDescHdr(SU_CLASS_DESCHDR_T *pDesc, void *pMem);
void su_UnpackEPDesc(SU_EP_DESC *pDesc, void *pMem, uint length);

#if SU_SAFETY_CHECKS
int  su_CheckDevDesc(SU_DEV_DESC *pDesc);
int  su_CheckCfgDesc(SU_CFG_DESC *pDesc);
int  su_CheckIntfDesc(SU_INTF_DESC *pDesc);
int  su_CheckEPDesc(SU_EP_DESC *pDesc);
#endif

#if defined(__cplusplus)
}
#endif

#endif /* SU_DESC_H */

