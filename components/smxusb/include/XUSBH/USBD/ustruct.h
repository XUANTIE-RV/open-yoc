/*
* ustruct.h                                                 Version 3.00
*
* smxUSBH Internal Control Structures for Stack and Class Drivers.
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
* Author: Yingbo Hu, Ralph Moore, David Moore
*
* Portable to any ANSI compliant C compiler.
*
*****************************************************************************/

#ifndef SU_STRUCT_H
#define SU_STRUCT_H

/* Constants needed by structures */
#define SU_MAXCHILDDEVNUM           16
#define SU_MAXIAD                    4
#define SU_MAX_DEV_NUM             128

#if (SU_MAX_DEV_NUM % 8)
#define SU_DEV_MAP_LEN             (SU_MAX_DEV_NUM / 8) + 1
#else
#define SU_DEV_MAP_LEN             (SU_MAX_DEV_NUM / 8)
#endif

#define SU_MAX_EP_NUM               16  /* Requires 12*2*4 = 96 bytes in every DEV_INFO struct */

/* USB descriptor and request sizes */
#define SU_SZ_CFG_DESC               9
#define SU_SZ_DESC_HDR               2
#define SU_SZ_DEV_DESC              18
#define SU_SZ_EP_DESC                7
#define SU_SZ_IAD_DESC               8
#define SU_SZ_INTF_DESC              9
#define SU_SZ_OTG_DESC               3
#define SU_SZ_REQ_HDR                8

/* Forward References */
typedef struct SU_CFG_DESC  *SU_CFG_DESC_PTR;
typedef struct SU_DEV_INFO  *SU_DEV_INFO_PTR;
typedef struct SU_REQ_INFO  *SU_REQ_INFO_PTR;
typedef struct SU_HC_OP     *SU_HC_OP_PTR;
typedef struct SU_HC_INFO   *SU_HC_INFO_PTR;
typedef struct SU_INTF_DESC *SU_INTF_DESC_PTR;
typedef struct SU_PKT_DESC  *SU_PKT_DESC_PTR;
typedef struct SU_HUB_TT    *SU_HUB_TT_PTR;


/*============================================================================
                        smxUSBH UNPACKED CONTROL STRUCTURES
============================================================================*/

/* Class Driver Check structure */
/* checkMask specifies which of the following fields to compare in the device
   descriptor or in its interface descriptors to find the class driver for the
   device. */
typedef struct
{
    u16   checkMask;
    u16   vendor;
    u16   product;
    u8    devClass;
    u8    devSubClass;
    u8    devProtocol;
    u8    intfClass;
    u8    intfSubClass;
    u8    intfProtocol;
}SU_CHK_DEV;


/* Class Driver Info */
typedef struct SU_CLS_INFO
{
    SU_LIST       drvList;        /* Links into class driver list */
    const char   *devName;
    const SU_CHK_DEV *devArray;   /* -> CD check struct array */
    SB_OS_MUTEX_HANDLE mutexDrv;  /* CLS_INFO access mutex */
    /* Generic class driver API functions to connect and to close */
    void *(*devConn)(SU_DEV_INFO_PTR pDevInfo, uint intf, const SU_CHK_DEV *id);
    void (*devClose)(SU_DEV_INFO_PTR pDevInfo, void *);
}SU_CLS_INFO;


/* Device Descriptor (must precede SU_DEV_INFO) */
typedef __packed struct SU_DEV_DESC
{
    u8  bLength;
    u8  bDescType;
    u16 bcdUSB;
    u8  bDevClass;
    u8  bDevSubClass;
    u8  bDevProtocol;
    u8  bMaxPktSize0;
    u16 idVendor;
    u16 idProduct;
    u16 bcdDev;
    u8  iManufacturer;
    u8  iProduct;
    u8  iSerialNumber;
    u8  bNumCfg;
}__packed_gnu SU_DEV_DESC;


/* Device Info */
/* One per connected device or hub. Contains info obtained by enumeration. */
typedef struct SU_DEV_INFO
{
    u8            speed;
    u8            hubPortNum;        /* hub port number (1-based) */
    u8            hubAddr;
    u8            childDevNum;
    uint          devAddr;
    u32           toggle[2];
    u16           inEPMaxPkt[SU_MAX_EP_NUM];
    u16           outEPMaxPkt[SU_MAX_EP_NUM];
    SU_HUB_TT_PTR    pTT;
    SU_HC_INFO_PTR   pHostInfo;      /* ->Host Ctrl info for this device */
    SU_DEV_DESC      desc;           /* Device descriptor structure */
    SU_CFG_DESC_PTR  pCfgDescArr;    /* ->Config descriptor array */
    SU_CFG_DESC_PTR  pActCfg;        /* ->Currently active configuration */
    char        **ppRawDesc;         /* ->Raw configuration descriptor set */
    void         *pPrivateData;      /* ->HC driver device info */
    #if SU_OTG
    SU_OTG_DESC   OTGDesc;
    #endif
    SU_DEV_INFO_PTR *pChildDev;      /* -> array of DEV_INFO pointers of devices connected to hub */
    SU_CLS_INFO  *pDrvInfo;          /* -> Class Driver info */
    SB_OS_MUTEX_HANDLE mutexDrv;     /* DEV_INFO access mutex */
}SU_DEV_INFO;


/* Host Controller Info */
/* Created by HC driver init function. */
typedef struct SU_HC_INFO
{
    const char   *pHostBusName;      /* ->HC name, e.g. "OHCI" */
    SU_HC_OP_PTR  pHostOp;           /* ->HC operations */
    SU_DEV_INFO  *pRootHubDev;       /* ->Root hub info */
    void         *pPrivateData;      /* ->HCtypeHostInfo e.g. HCtype = OHCI */
    u8            devMap[SU_DEV_MAP_LEN]; /* Bitmap of connected devices -- 
                                             used to assign unique addresses */
    uint nextAddr;   /* Trial next device address if SU_DEV_ADDR_INCREASE */
    int  index;
    int  iHCIndex;   /* Index number for this same type of Host Controller */
#if SU_OTG
    int  otgPortNum; /* port number (1-based) */
#endif
}SU_HC_INFO;


/* Host Controller Operations (API Functions) */
typedef struct SU_HC_OP
{
    int (*AllocDev)   (SU_DEV_INFO_PTR pDevInfo);
    int (*FreeDev)    (SU_DEV_INFO_PTR pDevInfo);
    int (*TransferCmd)(SU_REQ_INFO_PTR pReqInfo);
    int (*RemoveCmd)  (SU_REQ_INFO_PTR pReqInfo);
    int (*RoothubCmd) (uint iHCIndex, uint portNum, uint Req, u32 *pData);
}SU_HC_OP;


/* Interface Info */
/* Specifies the active interface and manages alternate interfaces, if any.
   All alternate interfaces have the same interface number; only one can be
   active at a time. */
typedef struct SU_INTF_INFO
{
    SU_INTF_DESC_PTR  pAltIntfDescArr;   /* ->Array of alternate IntfDesc */
    int           pActAltIntf;       /* Currently active alt interface index */
    int           NumAltIntf;        /* Number of alt interfaces */
    int           MaxAltIntf;        /* Maximum number of alt interfaces */
    SU_CLS_INFO  *pClassDrv;         /* ->Class driver for the active interface */
    void         *pDataBuf;          /* ->Data for the class driver */
}SU_INTF_INFO;


/* Request Info */
/* Controls a transfer request from a class driver or hub task to a device
   via a pipe. */
typedef struct SU_REQ_INFO
{
    SU_LIST       reqCmdList;        /* Links this request into the request command list */
    u32           pipe;              /* Pipe command -- see ubase.h */
    u32           TransferFlags;
    int           status;
    uint          TransferBufLen;    /* Length of TransferBuf */
    uint          actualLength;      /* Actual length sent or received */
    uint          startFrame;
    uint          pktNum;
    uint          interval;
    uint          errNum;
    u8           *pSetupPkt;         /* ->SETUP data packet for this request */
    void         *pTransferBuf;      /* ->Data buffer for this request */
    void         *pPrivateData;      /* ->ReqCmdInfo struct in HC driver */
    void         *pContext;          /* ->Class driver info */
    SU_REQ_INFO_PTR  next;           /* ->Next request for same endpoint */
    SU_REQ_INFO_PTR  isoNext;        /* Same used by HC */
    SB_OS_SEM_HANDLE pReqDone;       /* Request done semaphore */
    SU_DEV_INFO_PTR  pDev;           /* ->Device CB */
    SU_PKT_DESC_PTR  pktDesc;        /* ->Packet descriptor for this request */
    void (*completeFunc)(SU_REQ_INFO_PTR pReq); /* Request done callback fct */
}SU_REQ_INFO;


/* Packet Descriptor */
typedef struct SU_PKT_DESC
{
    uint offset;
    uint length;
    uint actualLength;
    int  status;
}SU_PKT_DESC;


/* Hub Transaction Translator info */
typedef struct SU_HUB_TT
{
    SU_DEV_INFO *pHubDevInfo;
    int  multiFlag;
}SU_HUB_TT; 


/*============================================================================
                        smxUSBH PACKED CONTROL STRUCTURES
============================================================================*/

/* Some compilers require a pack pragma instead of the __packed keyword */
#if __packed_pragma
#if defined(__RENESAS__)
#pragma pack 1  /* pack structures (byte alignment) */
#else
#pragma pack(1) /* pack structures (byte alignment) */
#endif
#endif

/* Interface Association Descriptor (must precede SU_CFG_DESC) */
typedef __packed struct SU_IA_DESC
{
    u8  bLength;
    u8  bDescType;
    u8  bFirstIntf;
    u8  bIntfCount;
    u8  bFuncClass;
    u8  bFuncSubClass;
    u8  bFuncProtocol;
    u8  iFunc;
} __packed_gnu SU_IA_DESC; 

/* Configuration Descriptor plus control fields */
typedef __packed struct SU_CFG_DESC
{
    /* Configuration Descriptor */
    u8  bLength;
    u8  bDescType;
    u16 wTotalLength;
    u8  bNumIntf;                /* Number of active interfaces */
    u8  bConfigValue;
    u8  iConfig;
    u8  bmAttr;
    u8  bMaxPower;
    /* Configuration Control aligned on 32-bit boundary */
    u8  unused[3];
    SU_INTF_INFO  *pIntfInfoArr; /* ->Interface info array */
    int            CSDLen;       /* Class-specific descriptor buffer length */
    u8            *pCSDesc;      /* ->Class-Specific descriptor buffer */
    int            IADNum;       /* Number of IADs found */
    SU_IA_DESC     IADArr[SU_MAXIAD]; /* IAD array */
}__packed_gnu SU_CFG_DESC;


/* Device Descriptor Header */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
}__packed_gnu SU_DESC_HDR;


/* Endpoint Descriptor plus control fields */
typedef __packed struct
{
    /* Endpoint Descriptor */
    u8  bLength;
    u8  bDescType;
    u8  bEPAddr;
    u8  bmAttr;
    u16 wMaxPktSize;
    u8  bInterval;
    /* Endpoint Control aligned on 32-bit boundary. */
    u8  unused[1];
    int CSDLen;   /* Length of class-specific descriptor buffer */
    u8 *pCSDesc;  /* ->Class-Specific descriptor buffer */
}__packed_gnu SU_EP_DESC;


/* Interface Descriptor plus control fields */
typedef __packed struct SU_INTF_DESC
{
    /* Interface Descriptor */
    u8  bLength;
    u8  bDescType;
    u8  bIntfNum;
    u8  bAltSetting;
    u8  bNumEP;
    u8  bIntfClass;
    u8  bIntfSubClass;
    u8  bIntfProtocol;
    u8  iIntf;
    /* Interface Control aligned on 32-bit boundary */
    u8          unused[3];
    SU_EP_DESC *pEPDescArr; /* ->Array of endpoints for this interface */
    int         CSDLen;     /* Class-specific descriptor buffer length */
    u8         *pCSDesc;    /* ->Class-Specific descriptor buffer */
}__packed_gnu SU_INTF_DESC;


/* OTG Descriptor */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bmAttr;
}__packed_gnu SU_OTG_DESC;


/* SETUP Data Packet */
typedef __packed struct
{
    u8   bmReqType;
    u8   bReq;
    u16  wValue;
    u16  wIndex;
    u16  wLength;
}__packed_gnu SU_SDP;


/*============================================================================
                          CLASS DESCRIPTOR STRUCTURES
============================================================================*/

typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
}__packed_gnu SU_CLASS_DESCHDR_T;

#define SU_SZ_CLASS_DESC_HDR     3

/* Hub Class Device Descriptor, USB 1.1 p264 */
#define SU_HUB_DEVNUM    ((SU_MAXCHILDDEVNUM + 1 + 7) / 8)

typedef __packed struct
{
    u8  bDescLength;
    u8  bDescType;
    u8  bNumPorts;
    u16 wHubCharacteristics;
    u8  bPwrOn2PwrGood;
    u8  bHubContrCurrent;
    u8  DevRemovable[SU_HUB_DEVNUM];
    u8  PortPwrCtrlMask[SU_HUB_DEVNUM];
}__packed_gnu SU_HUB_DESCINFO_T;

#define SU_SZ_HUB_DESC (7+2*SU_HUB_DEVNUM)

/* HID class descriptor structures. HID 6.2.1 */

typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u16 bcdHID;
    u8  bCountryCode;
    u8  bNumDesc;
    u8  bDescType0;
    u16 wDescLength;
    /* optional descriptors are not supported. */
} __packed_gnu SU_CLASS_HID_DESC_T;

#define SU_SZ_CLASS_HID_DESC 9

typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u16 wLength;
    u8  bData[2];
} __packed_gnu SU_CLASS_REPORT_DESC_T;

/* Communications class descriptor structures. CDC 5.2 Table 25c */

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;
}__packed_gnu SU_CLASS_FUNCTION_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bmCapabilities;
} __packed_gnu SU_CLASS_FUNCTION_GENERIC_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x00 */
    u16 bcdCDC;
} __packed_gnu SU_CLASS_HEADER_FUNCTION_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x01 */
    u8  bmCapabilities;
    u8  bDataIntf;
} __packed_gnu SU_CLASS_CALL_MANAGEMENT_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x02 */
    u8  bmCapabilities;
} __packed_gnu SU_CLASS_ACM_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x03 */
} __packed_gnu SU_CLASS_DIRECT_LINE_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x04 */
    u8  bRingerVolSeps;
    u8  bNumRingerPatterns;
} __packed_gnu SU_CLASS_TELE_RINGER_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x05 */
    u8  bmCapabilities;
} __packed_gnu SU_CLASS_TELE_CALL_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x06 */
    u8  bMasterIntf;
    u8  bSlaveIntf0[1];
} __packed_gnu SU_CLASS_UNION_FUNCTION_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x07 */
    u8  iCountryCodeRelDate;
    u16 wCountryCode0[1];
} __packed_gnu SU_CLASS_COUNTRY_SELECT_DESC_T;


typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x08 */
    u8  bmCapabilities;
} __packed_gnu SU_CLASS_TELE_OPER_DESC_T;


typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x09 */
    u8  bEntityId;
    u8  bIntfNo;
    u8  bOutIntfNo;
    u8  bmOptions;
    u8  bChild0[1];
} __packed_gnu SU_CLASS_TERMINAL_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x0a */
    u8  bEntityId;
    u8  iName;
    u8  bChannelIndex;
    u8  bPhysicalIntf;
} __packed_gnu SU_CLASS_NET_CHANNEL_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x0b */
    u8  bEntityId;
    u8  bProtocol;
    u8  bChild0[1];
} __packed_gnu SU_CLASS_PROTO_UNIT_FUNCTION_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x0c */
    u8  bEntityId;
    u8  bExtensionCode;
    u8  iName;
    u8  bChild0[1];
} __packed_gnu SU_CLASS_EXTENSION_UNIT_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x0d */
    u8  bmCapabilities;
} __packed_gnu SU_CLASS_MULTI_CHANNEL_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x0e */
    u8  bmCapabilities;
} __packed_gnu SU_CLASS_CAPI_CONTROL_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x0f */
    u8  iMACAddr;
    u32 bmEthernetStatistics;
    u16 wMaxSegmentSize;
    u16 wNumberMCFilters;
    u8  bNumberPowerFilters;
} __packed_gnu SU_CLASS_ETHERNET_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x10 */
    u8  iEndSystermIdentifier;
    u8  bmDataCapabilities;
    u8  bmATMDeviceStatistics;
    u16 wType2MaxSegmentSize;
    u16 wType3MaxSegmentSize;
    u16 wMaxVC;
} __packed_gnu SU_CLASS_ATM_DESC_T;


typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x12 */
    u16 bcdVersion;
    u8  bGUID[16];
} __packed_gnu SU_CLASS_MDLM_DESC_T;

typedef __packed struct
{
    u8  bFuncLength;
    u8  bDescType;
    u8  bDescSubtype;    /* 0x13 */
    u8  bGuidDescType;
    u8  bDetailData[1];
} __packed_gnu SU_CLASS_MDLMD_DESC_T;

/* Line Coding Structure from CDC spec 6.2.13 */
typedef __packed  struct
{
    u32 dwDTERate;
    u8  bCharFormat;
    u8  bParityType;
    u8  bDataBits;
} __packed_gnu SU_CLASS_CDC_LINE_CODING_T;

#define SU_SZ_CDC_LINE_CODING 7

/* Audio class descriptor structures */
/* AC descriptor */
typedef __packed struct /* Table 4-2 */
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u16 bcdADC;
    u16 wTotalLength;
    u8  bInCollection;
    u8  baIntfNr[1];
}__packed_gnu SU_CLASS_AC_HDR_DESC_T;

typedef __packed struct /* Table 4-3 */
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u8  bNrChannels;
    u16 wChannelConfig;
    u8  iChannelNames;
    u8  iTerminal;
}__packed_gnu SU_CLASS_AC_IT_DESC_T;

typedef __packed struct /* Table 4-4 */
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u8  bSourceID;
    u8  iTerminal;
}__packed_gnu SU_CLASS_AC_OT_DESC_T;

/* Audio Feature Unit Descriptor */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bUnitID;
    u8  bSourceID;
    u8  bControlSize;
    u8  bmaControls[2];
    /*
    u8  iFeature;
    */
}__packed_gnu SU_CLASS_AC_FU_DESC_T;

#define SU_SZ_CLASS_AC_FU_DESC 8
/* Audio Mixer Unit Descriptor */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bUnitID;
    u8  bNrInPins;
    u8  bSourceID[1]; 
    /*
    u8  bNrChannels;
    u16 wChannelConfig;
    u8  iChannelNames;
    u8  bmControls[];
    u8  iMixer;
    */
}__packed_gnu SU_CLASS_AC_MIXER_DESC_T;

#define SU_SZ_CLASS_AC_MIXER_DESC 6

/* Audio Selector Unit Descriptor */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bUnitID;
    u8  bNrInPins;
    u8  baSourceID[1];
    /*
    u8  iSelector;
    */
}__packed_gnu SU_CLASS_AC_SELECTOR_DESC_T;

#define SU_SZ_CLASS_AC_SELECTOR_DESC 6

/* AS descriptor */
typedef __packed struct  /* Table 4-19 */
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bTerminalLink;
    u8  bDelay;
    u16 wFormatTag;
}__packed_gnu SU_CLASS_AS_GENERAL_DESC_T;

/* Type I format descriptor, Table 2-1 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bFormatType;
    u8  bNrChannels;
    u8  bSubFrameSize;
    u8  bBitResolution;
    u8  bSamFreqType;
    u8  tSamFreq[1][3];
}__packed_gnu SU_CLASS_AS_FORMAT_TYPEI_DESC_T;

/* Audio Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bmAttr;
    u8  bLockDelayUnits;
    u16 wLockDelay;
}__packed_gnu SU_CLASS_AS_EP_DESC_T;

typedef __packed struct /* Table 4-2 */
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u16 bcdADC;
    u16 wTotalLength;
}__packed_gnu SU_CLASS_MS_HDR_DESC_T;

/* Audio Class-Specific MIDI Stream MIDI IN Jack Descriptor */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bJackType;
    u8  bJackID;
    u8  iJack;
}__packed_gnu SU_CLASS_MS_INJACK_DESC_T;

/* Audio Class-Specific MIDI Stream MIDI OUT Jack Descriptor */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bJackType;
    u8  bJackID;
    u8  bNrInputPins;
    u8  BaSourceID[1]; //TODO: support multiple sources
    u8  BaSourcePin[1];
    u8  iJack;
}__packed_gnu SU_CLASS_MS_OUTJACK_DESC_T;

/* Audio Class-Specific MIDI Stream BULK Data Endpoint Descriptor */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bNumEmbMIDIJack;
    u8  baAssocJackID[1]; //TODO: support multiple ID
}__packed_gnu SU_CLASS_MS_BULK_DESC_T;


/* Video class descriptor structures */
#define SU_VIDEO_USE_11 0
/* VC descriptor */
typedef __packed struct /* Table 3-3 */
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u16 bcdUVC;
    u16 wTotalLength;
    u32 dwClockFrequency;
    u8  bInCollection;
    u8  baIntfNr[1]; /* real size is sizeof(SU_CLASS_VC_HDR_DESC_T) + (bInCollection - 1) */
}__packed_gnu SU_CLASS_VC_HDR_DESC_T;

#define SU_SZ_VC_HDR_DESC 13

typedef __packed struct /* Table 3-4 */
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u8  iTerminal;
}__packed_gnu SU_CLASS_VC_IT_DESC_T;

#define SU_SZ_VC_IT_DESC 8

typedef __packed struct /* Table 3-5 */
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u8  bSourceID;
    u8  iTerminal;
}__packed_gnu SU_CLASS_VC_OT_DESC_T;

#define SU_SZ_VC_OT_DESC 9

typedef __packed struct /* Table 3-6 */
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u8  iTerminal;
    u16 wObjectiveFocalLengthMin;
    u16 wObjectiveFocalLengthMax;
    u16 wOcularFocalLength;
    u8  bControlSize;
    u8  bmControls[1]; /* real size is sizeof(SU_CLASS_VC_CAMERA_DESC_T) + bControlSize - 1 */
}__packed_gnu SU_CLASS_VC_CAMERA_DESC_T;

#define SU_SZ_VC_CAMERA_DESC 14

/* Video Selector Unit Descriptor, Table 3-7 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bUnitID;
    u8  bNrInPins;
    u8  baSourceID[1];
    /* u8  iSelector; */ /* real size is sizeof(SU_CLASS_VC_SU_DESC_T) + bNrInPins */
}__packed_gnu SU_CLASS_VC_SU_DESC_T;

#define SU_SZ_VC_SU_DESC 6

/* Video Processing Unit Descriptor, Table 3-8 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bUnitID;
    u8  bSourceID;
    u16 wMaxMultiplier;
    u8  bControlSize;
    u8  bmControls[2]; 
    /*
    u8  iProcessing;
    u8  bmVideoStandards;
    */
}__packed_gnu SU_CLASS_VC_PU_DESC_T;/* real size is sizeof(SU_CLASS_VC_PU_DESC_T) + 2 + bControlSize - 2 */

#define SU_SZ_VC_PU_DESC 10

/* Video Extension Unit Descriptor, Table 3-9 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bUnitID;
    u8  guidExtensionCode[16];
    u8  bNumControls;
    u8  bNrInPins;
    u8  baSourceID[1]; 
    /*
    u8  bControlSize;
    u8  bmControls[1];
    u8  iExtension;
    */
}__packed_gnu SU_CLASS_VC_EU_DESC_T;/* real size is sizeof(SU_CLASS_VC_EU_DESC_T) + bNrInPins - 1 */

#define SU_SZ_VC_EU_DESC 23

/* Video Class-Specific VC Interrupt Endpoint Descriptor, Table 3-11 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u16 wMaxTransferSize;
}__packed_gnu SU_CLASS_VC_INT_EP_DESC_T;

#define SU_SZ_VC_INT_EP_DESC 5

/* Video Class-Specific VS Input Header Descriptor, Table 3-13 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bNumFormats;
    u16 wTotalLength;
    u8  bEPAddr;
    u8  bmInfo;
    u8  bTerminalLink;
    u8  bStillCaptureMethod;
    u8  bTriggerSupport;
    u8  bTriggerUsage;
    u8  bControlSize;
    u8  bmaControls[1];
}__packed_gnu SU_CLASS_VS_INPUT_HDR_DESC_T;

#define SU_SZ_VS_INPUT_HDR_DESC 14

/* Video Class-Specific VS Output Header Descriptor, Table 3-14 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bNumFormats;
    u16 wTotalLength;
    u8  bEPAddr;
    u8  bTerminalLink;
#if SU_VIDEO_USE_11
    u8  bControlSize;
    u8  bmaControls[1];
#endif
}__packed_gnu SU_CLASS_VS_OUTPUT_HDR_DESC_T;

#define SU_SZ_VS_OUTPUT_HDR_DESC 10

/* Video Class-Specific Still Image Frame Descriptor, Table 3-17 */

typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bEPAddr;
    u8  bNumImageSizePatterns;
    u16 wWidth[2];
    /*
    u8  bNumCompressionPattern;
    u8  bCompression[0];
    */
}__packed_gnu SU_CLASS_VS_STILL_IMAGE_FRAME_DESC_T;/* real size is sizeof(SU_CLASS_VS_STILL_IMAGE_FRAME_DESC_T) + (bNumNumImageSizePatterns - 1)*sizeof(u32) + 1 + bNumCompressionPattern */

#define SU_SZ_VS_STILL_IMAGE_FRAME_DESC 9

/* Video Class-Specific VS Format Descriptor, Uncompressed, Table 3-1 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bFormatIndex;
    u8  bNumFrameDesc;
    u8  guidFormat[16];
    u8  bBitsPerPixel;
    u8  bDefaultFrameIndex;
    u8  bAspectRatioX;
    u8  bAspectRatioY;
    u8  bmInterlaceFlags;
    u8  bCopyProtect;
}__packed_gnu SU_CLASS_VS_FORMAT_UNCOMPRESSED_DESC_T;

#define SU_SZ_VS_FORMAT_UNCOMPRESSED_DESC 27

/* Video Class-Specific VS Frame Descriptor, Uncompressed, Table 3-2 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bFrameIndex;
    u8  bmCapabilities;
    u16 wWidth;
    u16 wHeight;
    u32 dwMinBitRate;
    u32 dwMaxBitRate;
    u32 dwMaxVideoFrameBufSize;
    u32 dwDefaultFrameInterval;
    u8  bFrameIntervalType;
    u32 dwFrameInterval[1];
}__packed_gnu SU_CLASS_VS_FRAME_UNCOMPRESSED_DESC_T;

#define SU_SZ_VS_FRAME_UNCOMPRESSED_DESC 30

/* Video Class-Specific VS Format Descriptor, MJPEG, Table 3-1 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bFormatIndex;
    u8  bNumFrameDesc;
    u8  bmFlags;
    u8  bDefaultFrameIndex;
    u8  bAspectRatioX;
    u8  bAspectRatioY;
    u8  bmInterlaceFlags;
    u8  bCopyProtect;
}__packed_gnu SU_CLASS_VS_FORMAT_MJPEG_DESC_T;

#define SU_SZ_VS_FORMAT_MJPEG_DESC 11

/* Video Class-Specific VS Frame Descriptor, MJPEG, Table 3-2 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bFrameIndex;
    u8  bmCapabilities;
    u16 wWidth;
    u16 wHeight;
    u32 dwMinBitRate;
    u32 dwMaxBitRate;
    u32 dwMaxVideoFrameBufSize;
    u32 dwDefaultFrameInterval;
    u8  bFrameIntervalType;
    u32 dwFrameInterval[1];
}__packed_gnu SU_CLASS_VS_FRAME_MJPEG_DESC_T;

#define SU_SZ_VS_FRAME_MJPEG_DESC 30

/* Video Class-Specific VS Format Descriptor, MPEG2-TS, Table 3-1 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bFormatIndex;
    u8  bDataOffset;
    u8  bPktLength;
    u8  bStrideLength;
    u8  guidStrideFormat[16];
}__packed_gnu SU_CLASS_VS_FORMAT_MPEG2TS_DESC_T;

#define SU_SZ_VS_FORMAT_MPEG2TS_DESC 23

/* Video Class-Specific VS Format Descriptor, DV, Table 3-1 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bFormatIndex;
    u32 dwMaxVideoFrameBufSize;
    u8  bFormatType;
}__packed_gnu SU_CLASS_VS_FORMAT_DV_DESC_T;

#define SU_SZ_VS_FORMAT_DV_DESC 9

/* Video Class-Specific VS Color Format Descriptor, Table 3-18 */
typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u8  bDescSubtype;
    u8  bColorPrimaries;
    u8  bTransferCharacteristics;
    u8  bMatrixCoefficients;
}__packed_gnu SU_CLASS_VS_COLOR_FORMAT_DESC_T;

#define SU_SZ_VS_COLOR_FORMAT_DESC 6

/* Video Class-Specific VS Probe and Commit Request, Table 4-47 */
typedef __packed struct
{
    u16 bmHint;
    u8  bFormatIndex;
    u8  bFrameIndex;
    u32 dwFrameInterval;
    u16 wKeyFrameRate;
    u16 wPFrameRate;
    u16 wCompQuality;
    u16 wCompWindowSize;
    u16 wDelay;
    u32 dwMaxVideoFrameSize;
    u32 dwMaxPayloadTransferSize;
#if SU_VIDEO_USE_11
    u32 dwClockFrequency;
    u8  bmFramingInfo;
    u8  bPreferedVersion;
    u8  bMinVersion;
    u8  bMaxVersion;
#endif
}__packed_gnu SU_CLASS_VS_PROBE_COMMIT_REQ_T;

#if SU_VIDEO_USE_11
#define SU_SZ_PROBE_COMMIT_REQ 34
#else
#define SU_SZ_PROBE_COMMIT_REQ 26
#endif

/* Video Class-Specific VS Still Probe and Commit Request, Table 4-47 */
typedef __packed struct
{
    u8  bFormatIndex;
    u8  bFrameIndex;
    u8  bCompressionIndex;
    u32 dwMaxVideoFrameSize;
    u32 dwMaxPayloadTransferSize;
}__packed_gnu SU_CLASS_VS_STILL_PROBE_COMMIT_REQ_T;

#define SU_SZ_STILL_PROBE_COMMIT_REQ 11

/* Payload header */
typedef __packed struct
{
    u8  bHLE;
    u8  bBFH;
    u8  bPTS[4];
    u8  bSCR[4];
    u8  bFN[2];
}__packed_gnu SU_VIDEO_PAYLOAD_HEADER;

#define SU_SZ_PAYLOAD_HEADER 12

/* CCID class */

typedef __packed struct
{
    u8  bLength;
    u8  bDescType;
    u16 bcdCCID;
    u8  bMaxSlotIndex;
    u8  bVoltageSupport;
    u32 dwProtocols;
    u32 dwDefaultClock;
    u32 dwMaximumClock;
    u8  bNumClockSupported;
    u32 dwDataRate;
    u32 dwMaxDataRate;
    u8  bNumDataRatesSupported;
    u32 dwMaxIFSD;
    u32 dwSynchProtocols;
    u32 dwMachanical;
    u32 dwFeatures;
    u32 dwMaxCCIDMessageLength;
    u8  bClassGetResponse;
    u8  bClassEnvelope;
    u16 wLcdLayout;
    u8  bPINSupport;
    u8  bMaxCCIDBusySlots;
}__packed_gnu SU_CLASS_CCID_DESCRIPTOR;

#define SU_SZ_CLASS_CCID_DESCRIPTOR 0x36

typedef __packed struct
{
    u8  bMessageType;
    u32 dwLength;
    u8  bSlot;
    u8  bSeq;
    u8  Reserved[3];
}__packed_gnu SU_CCID_MSG;

typedef __packed struct
{
    u8  bMessageType;
    u32 dwLength;
    u8  bSlot;
    u8  bSeq;
    u8  bStatus;
    u8  bError;
    u8  bReserved;
}__packed_gnu SU_CCID_RESP;

typedef __packed struct
{
    u8  bMessageType;
    u8  bmSlotICCState[1];
}__packed_gnu SU_CCID_NOTIFY_SLOT_CHANGE;

typedef __packed struct
{
    u8  bMessageType;
    u8  bSlot;
    u8  bSeq;
    u8  bError;
}__packed_gnu SU_CCID_NOTIFY_HARDWARE_ERROR;

typedef __packed struct
{
    u8  bmFindexDindex;
    u8  bmTCCKST0;
    u8  bGuardTimeT0;
    u8  bWaitingIntegerT0;
    u8  bClockStop;
}__packed_gnu SU_CCID_PROTOCOL_T0;

typedef __packed struct
{
    u8  bmFindexDindex;
    u8  bmTCCKST1;
    u8  bGuardTimeT1;
    u8  bWaitingIntegerT1;
    u8  bClockStop;
    u8  bIFSC;
    u8  bNadValue;
}__packed_gnu SU_CCID_PROTOCOL_T1;

#if __packed_pragma
#if defined(__RENESAS__)
#pragma unpack /* restore default field alignment */
#else
#pragma pack() /* no par means restore default field alignment */
#endif
#endif

#endif /* SU_STRUCT_H */
