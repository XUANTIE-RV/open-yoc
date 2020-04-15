/*
* umsdrv.h                                                  Version 3.00
*
* smxUSBH Mass Storage Class Driver.
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

#ifndef SU_MS_DRV_H
#define SU_MS_DRV_H

/*============================================================================
                                      CONFIGURATION
============================================================================*/

#define SU_MSTOR_FULL_SUPPORT  0      /* set to 1 if you need to support QIC/UFI/ATAPI */

#if SU_MSTOR_FULL_SUPPORT             /* longer timeout needed for CD drives if encounter bad sector */
#define SU_MSTOR_REQ_TMO       15000  /* timeout for su_MSSendReqByBulk() and su_MSSendReqByControl() */
#else
#define SU_MSTOR_REQ_TMO       6000   /* timeout for su_MSSendReqByBulk() and su_MSSendReqByControl() */
#endif

/*============================================================================
                                        CONSTANTS
============================================================================*/

#define SU_CLASS_MAIN_MASSSTORAGE           8

#define SU_MS_MAX_CMD_LEN                   16
#define SU_MS_STRING_LEN                    32

/* command topic */
#define SU_MS_CMD_TEST_UNIT_READY           0x00
#define SU_MS_CMD_REQ_SENSE                 0x03
#define SU_MS_CMD_READ_6                    0x08
#define SU_MS_CMD_WRITE_6                   0x0A
#define SU_MS_CMD_REQ                       0x12
#define SU_MS_CMD_MODE_SELECT               0x15
#define SU_MS_CMD_MODE_SENSE                0x1A
#define SU_MS_CMD_START_STOP                0x1B
#define SU_MS_CMD_GET_CAPACITY              0x25
#define SU_MS_CMD_READ_10                   0x28
#define SU_MS_CMD_WRITE_10                  0x2A
#define SU_MS_CMD_LOG_SENSE                 0x4D
#define SU_MS_CMD_MODE_SENSE_10             0x5A

/* send command direction */
#define SU_MS_CMD_DIRECTION_UNKNOWN         0
#define SU_MS_CMD_DIRECTION_SEND            1
#define SU_MS_CMD_DIRECTION_RECV            2
#define SU_MS_CMD_DIRECTION_NONE            3

/* send command status */
#define SU_MS_CMD_RESULT_SUCCESS            0x00    /* success */
#define SU_MS_CMD_RESULT_FAIL               0x01    /* fail*/
#define SU_MS_CMD_RESULT_WRPROTECT          0x02    /* write protect*/
#define SU_MS_CMD_RESULT_NOT_READY          0x04    /* not ready */
#define SU_MS_CMD_RESULT_MEDIUM_ERROR       0x08    /* media error */
#define SU_MS_CMD_RESULT_HARDWARE_ERROR     0x10    /* hardware error */
#define SU_MS_CMD_RESULT_UNIT_ATTENTION     0x20    /* unit attention */
#define SU_MS_CMD_RESULT_ILLEGAL_REQ        0x40    /* illegal request */

/* fail detail */
#define SU_MS_CMD_RESULT_FAIL_ABORT         0x0100  /* abort command */
#define SU_MS_CMD_RESULT_FAIL_WRPROTECT     0x0200  /* write protect */
#define SU_MS_CMD_RESULT_FAIL_ERROR         0x0300  /* unknown error */

/* USB mass storage device type */
#define SU_MS_CMD_FORMAT_RBC                0x01    /* flash */
#define SU_MS_CMD_FORMAT_8020I              0x02    /* CD-ROM */
#define SU_MS_CMD_FORMAT_QIC157             0x03    /* QIC-157 Tapes */
#define SU_MS_CMD_FORMAT_UFI                0x04    /* Floppy */
#define SU_MS_CMD_FORMAT_8070I              0x05    /* Removable media */
#define SU_MS_CMD_FORMAT_SCSI               0x06    /* Transparent SCSI */

/* USB mass storage device transfer type */
#define SU_MS_CMD_SEND_CTRLBULKINT          0x00    /* Control/Bulk/Interrupt */
#define SU_MS_CMD_SEND_CTRLBULK             0x01    /* Control/Bulk */
#define SU_MS_CMD_SEND_BULK                 0x50    /* bulk only */

#define MS_CMD_LENGTH    12 

/* SENSE KEYS */
 
#define MS_NO_SENSE            0x00
#define MS_RECOVERED_ERROR     0x01
#define MS_NOT_READY           0x02
#define MS_MEDIUM_ERROR        0x03
#define MS_HARDWARE_ERROR      0x04
#define MS_ILLEGAL_REQUEST     0x05
#define MS_UNIT_ATTENTION      0x06
#define MS_DATA_PROTECT        0x07
#define MS_BLANK_CHECK         0x08
#define MS_COPY_ABORTED        0x0A
#define MS_ABORTED_COMMAND     0x0B
#define MS_VOLUME_OVERFLOW     0x0D
#define MS_MISCOMPARE          0x0E

/*============================================================================
                          STRUCTURES AND OTHER TYPEDEFS
============================================================================*/

/* command data structure */
typedef struct
{
    u8   *pBuf;
    uint  bufLen;
}SU_MS_CMD_DATA_BUF_T;

struct SU_MS_Drv_Info_T;

/* MS request command infomation structure */
typedef struct  
{
    struct SU_MS_Drv_Info_T *pDrvInfo;
    u8    cmdLength;
    u8    cmdDirection;
    u8    cmdData[SU_MS_MAX_CMD_LEN];
    void *pBuf;
    SU_MS_CMD_DATA_BUF_T *pReqBuf;
    u8    bufPrepare[64];
    uint  bufLength;
    uint  numScatter;
    uint  LUN;
    int   result;
}SU_MS_REQ_INFO_T;

typedef int  (*SENDCMDFUNC)(SU_MS_REQ_INFO_T *, struct SU_MS_Drv_Info_T*);
typedef void (*BUILDCMDFUNC)(SU_MS_REQ_INFO_T *, struct SU_MS_Drv_Info_T*);

/* USB mass storage device information */
typedef struct
{
    u8   flagCMDFormat;
    u8   flagSendCMD;
}SU_MS_DEV_DRV_T;

typedef struct
{
    u32       StorageSectorNum;
    u32       StorageSectorSize;
    uint      StorageChanged;
    uint      StorageProtected;
    uint      StorageRemoved;
    uint      StorageRemoveable;
}SU_MS_UNIT_INFO;

typedef struct SU_MS_Drv_Info_T
{
    /* Manufacturer information */
    u8        MfrInfo[SU_MS_STRING_LEN];
    u8        ProductInfo[SU_MS_STRING_LEN];
    u8        SerialNumInfo[SU_MS_STRING_LEN];
    u8        subclassInfo;
    u8        protocolInfo;
    u8        bulkInEPAddr;
    u8        bulkOutEPAddr;
    SU_DEV_INFO *pUSBDevInfo;
    SU_MS_UNIT_INFO *pLUNInfo;
    uint      StorageFound;
    uint      StorageID;
    uint      StorageMaxLUN;
    uint      IntfIdx;
    uint      iID;
    SENDCMDFUNC     SendCmdFunc;     /* Send command function */
    BUILDCMDFUNC    BuildCmdFunc;    /* Build command function */
    SU_REQ_INFO        *pMSDoReqInfo;
#if SU_MSTOR_FULL_SUPPORT
    SU_REQ_INFO        *pMSIntReqInfo;
    SU_EP_DESC         *pInterruptEPDesc;
    u8                 *pIntBuffer;
    SB_OS_SEM_HANDLE    signalIntTransport;
    uint                waitInterrupt;
#endif
#if SB_MULTITASKING
    SB_OS_MUTEX_HANDLE  mutexCmdDo;
#endif
    const SU_MS_DEV_DRV_T *drvFuncStruct;
}SU_MS_DRV_INFO_T;


/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int su_MSDevIsProbed(uint iID);
void * su_MSInitDev(SU_DEV_INFO * pDev, uint IntfIdx,
                             const SU_CHK_DEV * pDevInfo);
void su_MSReleaseDev(SU_DEV_INFO * pDev, void *ptr);
void su_MSClearDrvInfo(uint iID);
void su_MSSendReq(SU_MS_REQ_INFO_T*, SU_MS_DRV_INFO_T*);
void su_MSInitDrvInfo(void);
SU_MS_DRV_INFO_T * su_MSGetDrvInfo(uint iID);
SU_CHK_DEV * su_MSGetDevInfo(void);

#if defined(__cplusplus)
}
#endif

#endif /* SU_MS_DRV_H */

