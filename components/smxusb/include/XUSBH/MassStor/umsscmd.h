/*
* umsscmd.h                                                 Version 3.00
*
* smxUSBH Mass Storage Class Driver SCSI Command.
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

#ifndef SU_MS_SENDCMD_H
#define SU_MS_SENDCMD_H

/*============================================================================
                                     ENUMS
============================================================================*/

#define SU_MS_SEND_CMD_BULK_SUCCESS    0  
#define SU_MS_SEND_CMD_BULK_LESS       1
#define SU_MS_SEND_CMD_BULK_FAILED     2
#define SU_MS_SEND_CMD_BULK_ABORTED    3
#define SU_MS_SEND_CMD_BULK_STALLED    4

#define SU_MS_SEND_CMD_RESULT_SUCCESS  0
#define SU_MS_SEND_CMD_RESULT_FAILED   1
#define SU_MS_SEND_CMD_RESULT_ERROR    2
#define SU_MS_SEND_CMD_RESULT_ABORTED  3

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#if defined(__cplusplus)
extern "C" {
#endif

int  su_MSSendCmdByBulk(SU_MS_REQ_INFO_T*, SU_MS_DRV_INFO_T*);
int  su_MSSendReqByBulk(SU_MS_DRV_INFO_T *pDrvInfo,
                          void *pCmdData, u32 transportPipe,
                          uint cmdLength, uint * sentLength);
int  su_MSSendReqByControl(SU_MS_DRV_INFO_T *pDrvInfo,
                             u32 transportPipe, u8 ReqData,
                             u8 ReqType, u16 ReqValue,
                             u16 ReqIndex, void *pCmdData,
                             u16 cmdLength);
void su_MSSendAllReqData(SU_MS_REQ_INFO_T *pMSReqInfo,
                           SU_MS_DRV_INFO_T* pDrvInfo);

int  su_MSSendReqErrorDo(SU_MS_DRV_INFO_T *pDrvInfo, u32 transportPipe);
u32  su_MSGetCmdLen(SU_MS_REQ_INFO_T *pMSReqInfo);

#if defined(__cplusplus)
}
#endif

/*==========================================================================*/
#endif /* SU_MS_SENDCMD_H */
