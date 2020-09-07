/*
* udriver.h                                                 Version 3.00
*
* smxUSBH Device Framework Driver.
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

#ifndef SU_DRV_H
#define SU_DRV_H

/*============================================================================
                                   CONSTANTS
============================================================================*/

/* Flags for check USB device */
#define SU_CHK_DEV_INTFCLASS       0x0001
#define SU_CHK_DEV_INTFSUBCLASS    0x0002
#define SU_CHK_DEV_INTFPROTOCOL    0x0004
#define SU_CHK_DEV_DEVCLASS        0x0008
#define SU_CHK_DEV_DEVSUBCLASS     0x0010
#define SU_CHK_DEV_DEVPROTOCOL     0x0020
#define SU_CHK_DEV_VID             0x0040
#define SU_CHK_DEV_PID             0x0080
#define SU_CHK_DEV_INTFALL         (SU_CHK_DEV_INTFCLASS | SU_CHK_DEV_INTFSUBCLASS | SU_CHK_DEV_INTFPROTOCOL)
#define SU_CHK_DEV_DEVALL          (SU_CHK_DEV_DEVCLASS | SU_CHK_DEV_DEVSUBCLASS | SU_CHK_DEV_DEVPROTOCOL)
#define SU_CHK_DEV_VIDPID          (SU_CHK_DEV_VID | SU_CHK_DEV_PID)

/* USB command transfer flags */
#define SU_REQ_TF_DISABLESPD       0x0001
#define SU_REQ_TF_ASAP             0x0002
#define SU_REQ_TF_UNLINK           0x0008
#define SU_REQ_TF_QUEUEBULK        0x0010
#define SU_REQ_TF_NOFSBR           0x0020
#define SU_REQ_TF_ZEROPKT          0x0040
#define SU_REQ_TF_NOINTERRUPT      0x0080
#define SU_REQ_TF_TIMEOUT          0x1000

/* Status of request */
#define SU_REQ_STAT_SUCCESS        0
#define SU_REQ_STAT_QUEUED         (-6)
#define SU_REQ_STAT_CRC            (-11)
#define SU_REQ_STAT_BITSTUFF       (-12)
#define SU_REQ_STAT_STALL          (-13)
#define SU_REQ_STAT_TIMEOUT        (-14)
#define SU_REQ_STAT_DATAOVERRUN    (-15)
#define SU_REQ_STAT_DATAUNDERRUN   (-16)
#define SU_REQ_STAT_BUFOVERRUN     (-17)
#define SU_REQ_STAT_BUFUNDERRUN    (-19)
#define SU_REQ_STAT_PENDING        (-20)
#define SU_REQ_STAT_XDEV           (-21)
#define SU_REQ_STAT_NOMEM          (-22)
#define SU_REQ_STAT_INVAL          (-23)
#define SU_REQ_STAT_PIPE           (-24)
#define SU_REQ_STAT_REMOVED        (-25)
#define SU_REQ_STAT_CONNRESET      (-31)
#define SU_REQ_STAT_KILLED         (-32) /* Disable auto INT INs by HCD */
#define SU_REQ_STAT_AGAIN          (-34)
#define SU_REQ_STAT_PROTO          (-35)
#define SU_REQ_STAT_NAK            (-36)
#define SU_REQ_STAT_NYET           (-37)
#define SU_REQ_STAT_ERROR          (-38) /* General error */

#define SU_MEM_PAGE_SIZE           4096
#define SU_SPEED_UNKNOWN           0
#define SU_SPEED_LOW               1
#define SU_SPEED_FULL              2
#define SU_SPEED_HIGH              3

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

void su_DevMutexInit(void);
void su_DevMutexRelease(void);
int  su_AddDrvInfoToList(SU_CLS_INFO *pDrvInfo);
int  su_RemoveDrvInfoFromList(SU_CLS_INFO *pDrvInfo);
SU_HC_INFO *su_AllocHost(SU_HC_OP *pHostOp, const char *pHostBusName);
SU_HC_INFO *su_GetHost(int index);
void su_FreeHost(SU_HC_INFO *pHostInfo);
void su_ApplyUSBHost(SU_HC_INFO *pHostInfo);
void su_ReleaseUSBHost(SU_HC_INFO *pHostInfo);
SU_DEV_INFO *su_AllocDev(SU_HC_INFO *pDevBusInfo);
void su_FreeDev(SU_DEV_INFO *pDevInfo);
SU_REQ_INFO * su_BuildReqInfo(uint NumISOC);
void su_RelReqInfo(SU_REQ_INFO *pReqInfo);
int  su_SendReq(SU_REQ_INFO *pReqInfo);
int  su_UnlinkReq(SU_REQ_INFO *pReqInfo);
                                 
SU_SDP *su_BldSuPkt(uint req, uint type, uint val, uint idx, uint len);
int     su_SendCtrlReqSP(SU_DEV_INFO *pdev, u32 pipe, void *pBuf, uint tmo, SU_SDP *pSuPkt);
#define su_SendCtrlReq(dev, pipe, req, type, val, idx, buf, len, tmo) su_SendCtrlReqSP(dev, pipe, buf, tmo, su_BldSuPkt(req, type, val, idx, len))

int  su_ClearStall(SU_DEV_INFO *pDevInfo, u16 ep);

void su_AllocDevAddr(SU_DEV_INFO *pDevInfo);
void su_RemDev(SU_DEV_INFO **ppDevInfo);
int  su_AssignClassDrv(SU_DEV_INFO *pDevInfo);
SU_PSTACKEVTCBFUNC su_GetStackCallback(void);

#if SU_OTG
void su_SetOTGPortNum(int iPortNum);
int  su_EnableOTGHNP();
#endif

#if defined(__cplusplus)
}
#endif

#endif /* SU_DRV_H */

