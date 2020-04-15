/*
* uddevice.h                                                Version 2.57
*
* smxUSBD Device Core Layer.
*
* Copyright (c) 2005-2017 Micro Digital Inc.
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

#ifndef SUD_DEVICE_H
#define SUD_DEVICE_H

/*============================================================================
                                   CONSTANTS
============================================================================*/


/* Device and/or Interface Class codes */
#define SUD_CLASS_PER_INTERFACE         0    /* for DeviceClass */
#define SUD_CLASS_AUDIO                 1
#define SUD_CLASS_COMM                  2
#define SUD_CLASS_HID                   3
#define SUD_CLASS_PHYSICAL              5
#define SUD_CLASS_STILL_IMAGE           6
#define SUD_CLASS_PRINTER               7
#define SUD_CLASS_MASS_STORAGE          8
#define SUD_CLASS_HUB                   9
#define SUD_CLASS_CDC_DATA              10
#define SUD_CLASS_CSCID                 11
#define SUD_CLASS_CONTENT_SEC           13
#define SUD_CLASS_VIDEO                 14
#define SUD_CLASS_COMPOSITE             0xEF
#define SUD_CLASS_APP_SPEC              0xFE
#define SUD_CLASS_VENDOR_SPEC           0xFF

/* USB request types */
#define SUD_TYPE_STANDARD               (0x00 << 5)
#define SUD_TYPE_CLASS                  (0x01 << 5)
#define SUD_TYPE_VENDOR                 (0x02 << 5)
#define SUD_TYPE_RESERVED               (0x03 << 5)

/* USB request recipients */
#define SUD_RECIP_DEVICE                0x00
#define SUD_RECIP_INTERFACE             0x01
#define SUD_RECIP_ENDPOINT              0x02
#define SUD_RECIP_OTHER                 0x03

/* USB request directions */
#define SUD_DIR_OUT                     0
#define SUD_DIR_IN                      0x80

/* Descriptor types */
#define SUD_DT_DEVICE                   0x01
#define SUD_DT_CONFIG                   0x02
#define SUD_DT_STRING                   0x03
#define SUD_DT_INTERFACE                0x04
#define SUD_DT_ENDPOINT                 0x05
#define SUD_DT_DEVICE_QUALIFIER         0x06  /* USB 2.0 */
#define SUD_DT_OTHER_SPEED_CONFIG       0x07  /* USB 2.0 */
#define SUD_DT_INTERFACE_POWER          0x08  /* USB 2.0 */
#define SUD_DT_OTG                      0x09  /* USB 2.0 */
#define SUD_DT_DEBUG                    0x0A  /* USB 2.0 */
#define SUD_DT_IAD                      0x0B  /* USB 2.0 */

#define SUD_DT_HID                      (SUD_TYPE_CLASS | 0x01)
#define SUD_DT_REPORT                   (SUD_TYPE_CLASS | 0x02)
#define SUD_DT_PHYSICAL                 (SUD_TYPE_CLASS | 0x03)

#define SUD_DT_HUB                      (SUD_TYPE_CLASS | 0x09)

#define SUD_DT_DFU                      (SUD_TYPE_CLASS | 0x01)

#define SUD_DT_CS_INTERFACE             (SUD_TYPE_CLASS|SUD_DT_INTERFACE)
#define SUD_DT_CS_ENDPOINT              (SUD_TYPE_CLASS|SUD_DT_ENDPOINT)

/* Descriptor sizes per descriptor type */
#define SUD_DT_DEVICE_SIZE              18
#define SUD_DT_CONFIG_SIZE              9
#define SUD_DT_INTERFACE_SIZE           9
#define SUD_DT_ENDPOINT_SIZE            7
#define SUD_DT_ENDPOINT_AUDIO_SIZE      9    /* Audio extension */
#define SUD_DT_HUB_NONVAR_SIZE          7
#define SUD_DT_HID_SIZE                 9
#define SUD_DT_IAD_SIZE                 7

#define SUD_OTG_SRP                     0x01
#define SUD_OTG_HNP                     0x02

/* Endpoints related */
#define SUD_ENDPOINT_NUMBER_MASK        0x0f    /* in bEndpointAddress */
#define SUD_ENDPOINT_DIR_MASK           0x80

#define SUD_ENDPOINT_XFERTYPE_MASK      0x03    /* in bmAttributes */
#define SUD_ENDPOINT_XFER_CONTROL       0
#define SUD_ENDPOINT_XFER_ISOC          1
#define SUD_ENDPOINT_XFER_BULK          2
#define SUD_ENDPOINT_XFER_INT           3
#define SUD_ENDPOINT_XFER_ASYNC         0x4

/* USB Packet IDs (PIDs) */
#define SUD_PID_UNDEF_0                 0xf0
#define SUD_PID_OUT                     0xe1
#define SUD_PID_ACK                     0xd2
#define SUD_PID_DATA0                   0xc3
#define SUD_PID_PING                    0xb4    /* USB 2.0 */
#define SUD_PID_SOF                     0xa5
#define SUD_PID_NYET                    0x96    /* USB 2.0 */
#define SUD_PID_DATA2                   0x87    /* USB 2.0 */
#define SUD_PID_SPLIT                   0x78    /* USB 2.0 */
#define SUD_PID_IN                      0x69
#define SUD_PID_NAK                     0x5a
#define SUD_PID_DATA1                   0x4b
#define SUD_PID_PREAMBLE                0x3c    /* Token mode */
#define SUD_PID_ERR                     0x3c    /* USB 2.0: handshake mode */
#define SUD_PID_SETUP                   0x2d
#define SUD_PID_STALL                   0x1e
#define SUD_PID_MDATA                   0x0f    /* USB 2.0 */

/* Standard requests */
#define SUD_REQ_GET_STATUS              0x00
#define SUD_REQ_CLEAR_FEATURE           0x01
#define SUD_REQ_SET_FEATURE             0x03
#define SUD_REQ_SET_ADDRESS             0x05
#define SUD_REQ_GET_DESCRIPTOR          0x06
#define SUD_REQ_SET_DESCRIPTOR          0x07
#define SUD_REQ_GET_CONFIGURATION       0x08
#define SUD_REQ_SET_CONFIGURATION       0x09
#define SUD_REQ_GET_INTERFACE           0x0A
#define SUD_REQ_SET_INTERFACE           0x0B
#define SUD_REQ_SYNCH_FRAME             0x0C

/* USB Spec Release number */
#define SUD_BCD_HS_VERSION              0x0200
#define SUD_BCD_VERSION                 0x0110

/* Device Requests */
#define SUD_REQ_DIRECTION_MASK          0x80
#define SUD_REQ_TYPE_MASK               0x60
#define SUD_REQ_RECIPIENT_MASK          0x1f

#define SUD_REQ_DEVICE2HOST             0x80
#define SUD_REQ_HOST2DEVICE             0x00

#define SUD_REQ_TYPE_STANDARD           0x00
#define SUD_REQ_TYPE_CLASS              0x20
#define SUD_REQ_TYPE_VENDOR             0x40

#define SUD_REQ_RECIPIENT_DEVICE        0x00
#define SUD_REQ_RECIPIENT_INTERFACE     0x01
#define SUD_REQ_RECIPIENT_ENDPOINT      0x02
#define SUD_REQ_RECIPIENT_OTHER         0x03

/* Get Status bits */
#define SUD_STATUS_SELFPOWERED          0x01
#define SUD_STATUS_REMOTEWAKEUP         0x02

#define SUD_STATUS_HALT                 0x01

/* Standard Feature Selectors */
#define SUD_ENDPOINT_HALT               0x00
#define SUD_DEVICE_REMOTE_WAKEUP        0x01
#define SUD_TEST_MODE                   0x02

/* OTG Feature Selectors */
#define SUD_OTG_B_HNP_ENABLE            0x03
#define SUD_OTG_A_HNP_SUPPORT           0x04
#define SUD_OTG_A_ALT_HNP_SUPPORT       0x05

/* Test Mode Feature Selectors */
#define SUD_TEST_MODE_TEST_J            0x01
#define SUD_TEST_MODE_TEST_K            0x02
#define SUD_TEST_MODE_TEST_SE0_NAK      0x03
#define SUD_TEST_MODE_TEST_PACKET       0x04
#define SUD_TEST_MODE_TEST_FORCE_ENABLE 0x05

/* USB Status */
#define SUD_SEND_IN_PROGRESS            0
#define SUD_SEND_FINISHED_OK            1
#define SUD_SEND_FINISHED_ERROR         2
#define SUD_RECV_READY                  3
#define SUD_RECV_OK                     4
#define SUD_RECV_ERROR                  5

/* Device Events */
#define SUD_DEVICE_UNKNOWN              0
#define SUD_DEVICE_INIT                 1
#define SUD_DEVICE_CREATE               2
#define SUD_DEVICE_HUB_CONFIGURED       3
#define SUD_DEVICE_RESET                4

#define SUD_DEVICE_ADDRESS_ASSIGNED     5
#define SUD_DEVICE_CONFIGURED           6
#define SUD_DEVICE_SET_INTERFACE        7

#define SUD_DEVICE_SET_FEATURE          8
#define SUD_DEVICE_CLEAR_FEATURE        9

#define SUD_DEVICE_DE_CONFIGURED        10

#define SUD_DEVICE_BUS_INACTIVE         11
#define SUD_DEVICE_BUS_ACTIVITY         12

#define SUD_DEVICE_POWER_INTERRUPTION   13
#define SUD_DEVICE_HUB_RESET            14
#define SUD_DEVICE_DESTROY              15


/* Device State */
#define SUD_STATE_INIT                  0
#define SUD_STATE_CREATED               1
#define SUD_STATE_ATTACHED              2
#define SUD_STATE_POWERED               3
#define SUD_STATE_DEFAULT               4
#define SUD_STATE_ADDRESSED             5
#define SUD_STATE_CONFIGURED            6
#define SUD_STATE_UNKNOWN               7

/* Device status */
#define SUD_OPENING                     0
#define SUD_OK                          1
#define SUD_SUSPENDED                   2
#define SUD_CLOSING                     3

/* MaxPacket Size*/
#define SUD_HS_BULK_MAXPACKET_SIZE      512    /* High speed maximum bulk transfer packet size */
#define SUD_FS_BULK_MAXPACKET_SIZE      64     /* Full speed maximum bulk transfer packet size */

#define SUD_MAX_ENDPOINT_NUM            16
#define SUD_MAX_IAD_DESC_NUM            4

struct SUD_Function_Driver_T;

struct SUD_Endpoint_Handle_T;
struct SUD_Device_Handle_T;
struct SUD_DCD_Handle_T;

/* USB Requests */
typedef struct {
    SUD_LISTHEADER_T RequestList;

    struct SUD_Endpoint_Handle_T *pEndpoint;

    struct SUD_Device_Handle_T *pDevice;

    SUD_DEV_REQUESTSTRU_T RequestStru;

    u8 *pBuffer;
    uint iBufLength;
    uint iDataLength;
    uint iActualLength;

    uint iStatus;
    uint iSendNotify;
} SUD_REQUESTINFO_T;

typedef struct {
    u8  bEndpointAddress;
    u8  bmAttributes;
    u16 wMaxPacketSize;
    u8  bInterval;
    u8  bDirection;
#if SUD_AUDIO
    u8  bFresh;
    u8  bSynchAddress;
#endif
} SUD_ENDPOINT_INFO_T;

typedef struct {
    const char *iInterface;
    uint bAlternateSetting;
    uint bClassNum;
    SUD_CLASS_INFO_T *pClassInfo;
    uint bEndpointNum;
    const SUD_ENDPOINT_INFO_T *pEndpointInfo;
} SUD_ALTERNATE_INFO_T;

typedef struct {
    const char *iInterface;
    u8 bInterfaceClass;
    u8 bInterfaceSubClass;
    u8 bInterfaceProtocol;
    u8 bPadding;
    uint iAlternateNum;
    const SUD_ALTERNATE_INFO_T *pAlternateInfo;
} SUD_INTERFACE_INFO_T;

#if SUD_COMPOSITE || SUD_AUDIO || SUD_VIDEO
typedef struct {
    const char *iFunction;
    u8 bFunctionClass;
    u8 bFunctionSubClass;
    u8 bFunctionProtocol;
    u8 bFirstInterface;
    u8 bInterfaceCount;
} SUD_IAD_INFO_T;
#endif /* SUD_COMPOSITE */

typedef struct {
    const char *iConfiguration;
    u8 bmAttributes;
    u8 bMaxPower;
    u16 wPadding;
    uint bInterfaceNum;
    const SUD_INTERFACE_INFO_T *pInterfaceInfo;
#if SUD_COMPOSITE || SUD_AUDIO || SUD_VIDEO
    uint bIADNum;
    SUD_IAD_INFO_T *pIADInfo;
#endif
} SUD_CONFIGURATION_INFO_T;

typedef struct {
    u8 bDeviceClass;
    u8 bDeviceSubClass;
    u8 bDeviceProtocol;
    u8 bPadding;

    u16 idVendor;
    u16 idProduct;

    const char *iManufacturer;
    const char *iProduct;
    const char *iSerialNumber;
} SUD_DEVICE_INFO_T;

/* Endpoint handle */
typedef struct SUD_Endpoint_Handle_T {
    uint iEndpointAddr;                 /* logical endpoint address  */

    SUD_LISTHEADER_T ReadyList;         /* empty Request for receiving for ready request for sending */
    SUD_REQUESTINFO_T *pCurRequestInfo; /* current RequestInfo pointer */
    u16 bmAttributes;                   /* copy of bmAttributes from endpoint descriptor */
    u16 wMaxPacketSize;                 /* maximum packet size from endpoint descriptor */

} SUD_ENDPOINT_HANDLE;

/* USB Device handle */

typedef struct SUD_Device_Handle_T {
    struct SUD_DCD_Handle_T *pDCD;

    struct SUD_Function_Driver_T *pFunctionDriver;

    uint iDeviceState;
    uint iDeviceStatus;
#if SUD_OTG
    uint iOTGEnabled;
#endif

    u8 bDeviceAddr;
    u8 bActiveConfig;
    u8 bActiveInterface;
    u8 bPadding;
    u8 baActiveAlternate[SUD_MAX_ENDPOINT_NUM];

    SUD_PSTACKEVTCBFUNC pStackCallback;


} SUD_DEVICE_HANDLE_T;

typedef struct {
    int (*DCInit)(void);
    int (*DCRelease)(void);
    int (*DCStartXmit)(SUD_ENDPOINT_HANDLE *pEndpoint);
    int (*DCStallEndpoint)(uint ep, uint stall);
    int (*DCSetAddress)(u8 address);
    int (*DCEndpointHalted)(uint ep);
    int (*DCSetupOneEndpoint)(SUD_DEVICE_HANDLE_T *pDevice, uint ep, SUD_ENDPOINT_HANDLE *pEndpoint);
    int (*DCSetupEndpointDone)(SUD_DEVICE_HANDLE_T *pDevice);
    int (*DCDisableEndpoint)(uint ep);
    int (*DCConnect)(void);
    int (*DCDisconnect)(void);
    int (*DCEnable)(SUD_DEVICE_HANDLE_T *pDevice);
    int (*DCDisable)(void);
    int (*DCEnableInt)(void);
    int (*DCDisableInt)(void);
    int (*DCEP0PacketSize)(void);
    int (*DCMaxEndpoints)(void);
    int (*DCStartup)(SUD_DEVICE_HANDLE_T *pDevice);
    int (*DCMapEndpoint)(uint ep);
    int (*DCFrameNum)(void);
} SUD_DC_OPERATION_T;

/* DCD handle */
typedef struct SUD_DCD_Handle_T {
    uint          bMaxEndpoints;
    uint          bMaxPacketSize;
    const char    *pDCName;
    char          *pSerialNum;
    SUD_DC_OPERATION_T  *pDCOperation;
    SUD_ENDPOINT_HANDLE *pEndpointArray;

} SUD_DCD_HANDLE_T;

typedef struct {
    SUD_INTERFACE_DESC_T *pInterfaceDesc;

    uint iClassNum;
    SUD_CLASS_DESC_T **ppClassDescArray;

    uint iEndpointNum;
    SUD_ENDPOINT_DESC_T **ppEndpointDescArray;
#if SUD_HID
    SUD_CLASS_REPORT_DESC_T **ppReportDescArray;
#endif

} SUD_ALTERNATE_HANDLE_T;

typedef struct SUD_Interface_Handle_T {
    uint iAlternateNum;
    SUD_ALTERNATE_HANDLE_T *pAlternateHandleArray;

} SUD_INTERFACE_HANDLE_T;

typedef struct {
    uint iInterfaceNum;
    SUD_CONFIGURATION_DESC_T *pConfigDesc;
    SUD_INTERFACE_HANDLE_T *pInterfaceHandleArray;
    struct SUD_Function_Driver_T *pFunctionDriver;
#if SUD_COMPOSITE || SUD_AUDIO || SUD_VIDEO
    uint iIADNum;
    SUD_IAD_DESC_T *pIADDesc[SUD_MAX_IAD_DESC_NUM];
#endif
} SUD_CONFIGURATION_HANDLE_T;


#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

SUD_DCD_HANDLE_T *sud_AllocateDCD(u8 bMaxEndpoints);

SUD_DEVICE_HANDLE_T *sud_RegisterDevice(SUD_DCD_HANDLE_T *pDCD);
void sud_DeregisterDevice(SUD_DEVICE_HANDLE_T *pDevice);

int  sud_DoDeviceEvent(SUD_DEVICE_HANDLE_T *pDevice, uint event);

int  sud_AllocateRequestInfoData(SUD_REQUESTINFO_T *pRequestInfo, uint len);
SUD_REQUESTINFO_T *sud_AllocateRequestInfo(SUD_DEVICE_HANDLE_T *pDevice, u8 iEndpointAddr, uint len);
void sud_FreeRequestInfo(SUD_REQUESTINFO_T *pRequestInfo);
int  sud_ResizeRequestInfoData(SUD_DEVICE_HANDLE_T *pDevice, u8 iEndpointAddr, uint size);

SUD_REQUESTINFO_T *sud_GetRequestInfo(SUD_LISTHEADER_T *pHeader);
void sud_ReuseRequestInfo(SUD_REQUESTINFO_T *pRequestInfo);

int sud_SendRequest(SUD_REQUESTINFO_T *pRequestInfo);
int sud_CancelRequest(SUD_REQUESTINFO_T *pRequestInfo);
int sud_ProcessSetupPacket(SUD_REQUESTINFO_T *pRequestInfo);
void sud_RecvCompleted(SUD_ENDPOINT_HANDLE *pEndpoint, uint len, uint bad);
void sud_TransmitCompleted(SUD_ENDPOINT_HANDLE *pEndpoint);
void sud_RequestSendDone(SUD_REQUESTINFO_T *pEndpoint, uint result);

SUD_CONFIGURATION_HANDLE_T *sud_GetConfigurationHandle(SUD_DEVICE_HANDLE_T *pDevice,
        uint configuration);
struct SUD_Interface_Handle_T *sud_GetInterfaceHandle(SUD_DEVICE_HANDLE_T *pDevice,
        uint configuration, uint interface);
SUD_ALTERNATE_HANDLE_T *sud_GetAlternateHandle(SUD_DEVICE_HANDLE_T *pDevice, uint configuration,
        uint interface, uint alternate);
#if SUD_COMPOSITE || SUD_AUDIO || SUD_VIDEO
SUD_IAD_DESC_T *sud_GetIADDescriptor(SUD_DEVICE_HANDLE_T *pDevice, uint configuration, uint index);
#endif
SUD_CONFIGURATION_DESC_T *sud_GetConfigurationDescriptor(SUD_DEVICE_HANDLE_T *pDevice, uint configuration);
SUD_INTERFACE_DESC_T *sud_GetInterfaceDescriptor(SUD_DEVICE_HANDLE_T *pDevice, uint configuration,
        uint interface, uint alternate);
SUD_ENDPOINT_DESC_T *sud_GetEndpointDescriptorByIndex(SUD_DEVICE_HANDLE_T *pDevice, uint configuration,
        uint interface, uint alternate, uint index);
SUD_CLASS_DESC_T *sud_GetClassDescriptor(SUD_DEVICE_HANDLE_T *pDevice, uint configuration,
        uint interface, uint alternate, uint index);
SUD_CLASS_REPORT_DESC_T *sud_GetReportDescriptor(SUD_DEVICE_HANDLE_T *pDevice, uint configuration,
        uint interface, uint alternate, uint index);
SUD_ENDPOINT_DESC_T *sud_GetEndpointDescriptor(SUD_DEVICE_HANDLE_T *pDevice, uint configuration,
        uint interface, uint alternate, uint ep);
SUD_STRING_DESC_T *sud_GetStringDescriptor(u8 index);
SUD_DEVICE_DESC_T *sud_GetDeviceDescriptor(SUD_DEVICE_HANDLE_T *pDevice);
#if SUD_HIGH_SPEED
SUD_DEV_QUALIFIER_DESC_T *sud_GetDevQualifierDescriptor(SUD_DEVICE_HANDLE_T *pDevice);
#endif

void sud_FillOutEndpointRequestInfo(SUD_DEVICE_HANDLE_T *pDevice, SUD_ENDPOINT_HANDLE *pEndpoint, uint num);
void sud_FlushEndpoint(SUD_ENDPOINT_HANDLE *pEndpoint);
void sud_DetachEndpoint(SUD_ENDPOINT_HANDLE *pEndpoint);
#if SUD_HIGH_SPEED
void sud_ChangeToHSConfig(SUD_DEVICE_HANDLE_T *pDevice);
void sud_ChangeToFSConfig(SUD_DEVICE_HANDLE_T *pDevice);
#endif
#ifdef __cplusplus
}
#endif

#endif /* SUD_DEVICE_H */

