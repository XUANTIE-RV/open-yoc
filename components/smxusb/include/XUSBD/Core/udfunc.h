/*
* udfunc.h                                                  Version 2.53
*
* smxUSBD Function Layer.
*
* Copyright (c) 2005-2011 Micro Digital Inc.
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

#ifndef SUD_FUNC_H
#define SUD_FUNC_H

#if SUD_HIGH_SPEED
#define SUD_CLASS_BCD_VERSION              0x0200
#else
#define SUD_CLASS_BCD_VERSION              0x0110 /* only support USB 1.1 now */
#endif

/* CDC 4.3 Table 16 */
#define SUD_CDC_NO_SUBCLASS                0x00
#define SUD_CDC_DLCM_SUBCLASS              0x01
#define SUD_CDC_ACM_SUBCLASS               0x02
#define SUD_CDC_TCM_SUBCLASS               0x03
#define SUD_CDC_MCCM_SUBCLASS              0x04
#define SUD_CDC_CCM_SUBCLASS               0x05
#define SUD_CDC_ENCM_SUBCLASS              0x06
#define SUD_CDC_ANCM_SUBCLASS              0x07

/* WMCD 5.1 */
#define SUD_CDC_WHCM_SUBCLASS              0x08
#define SUD_CDC_DMM_SUBCLASS               0x09
#define SUD_CDC_MDLM_SUBCLASS              0x0a
#define SUD_CDC_OBEX_SUBCLASS              0x0b

/* CDC 4.7 Table 19 */
#define SUD_CDC_NO_PROTOCOL                0x00
#define SUD_CDC_PROTOCOL_AT                0x01
#define SUD_CDC_PROTOCOL_VENDOR            0xFF

/* CDC 5.2.3 Table 24 */
#define SUD_CDC_CS_INTERFACE               SUD_REQ_CS_INTERFACE
#define SUD_CDC_CS_ENDPOINT                SUD_REQ_CS_ENDPOINT

/* Descriptor Subtypes CDC 5.2.3 Table 25 */
/* WMCD 5.3 Table 5.3 */

#define SUD_CDC_ST_HEADER                  0x00
#define SUD_CDC_ST_CMF                     0x01
#define SUD_CDC_ST_ACMF                    0x02
#define SUD_CDC_ST_DLMF                    0x03
#define SUD_CDC_ST_TRF                     0x04
#define SUD_CDC_ST_TCLF                    0x05
#define SUD_CDC_ST_UF                      0x06
#define SUD_CDC_ST_CSF                     0x07
#define SUD_CDC_ST_TOMF                    0x08
#define SUD_CDC_ST_USBTF                   0x09
#define SUD_CDC_ST_NCT                     0x0a
#define SUD_CDC_ST_PUF                     0x0b
#define SUD_CDC_ST_EUF                     0x0c
#define SUD_CDC_ST_MCMF                    0x0d
#define SUD_CDC_ST_CCMF                    0x0e
#define SUD_CDC_ST_ENF                     0x0f
#define SUD_CDC_ST_ATMNF                   0x10

#define SUD_CDC_ST_WHCM                    0x11
#define SUD_CDC_ST_MDLM                    0x12
#define SUD_CDC_ST_MDLMD                   0x13
#define SUD_CDC_ST_DMM                     0x14
#define SUD_CDC_ST_OBEX                    0x15
#define SUD_CDC_ST_CS                      0x16
#define SUD_CDC_ST_CSD                     0x17
#define SUD_CDC_ST_TCM                     0x18

/* Audio Class */
/* Audio Interface Subclass Codes, Audio Spec A.2 */
#define SUD_AUDIO_SUBCLASS_UNDEF           0x00
#define SUD_AUDIO_SUBCLASS_AC              0x01
#define SUD_AUDIO_SUBCLASS_AS              0x02
#define SUD_AUDIO_SUBCLASS_MS              0x03

/* Audio Class-Specific AC Interface Descriptor Subtypes, Audio Spec A.5 */
#define SUD_AUDIO_AC_HEADER                0x01
#define SUD_AUDIO_AC_INPUT_TERMINAL        0x02
#define SUD_AUDIO_AC_OUTPUT_TERMINAL       0x03
#define SUD_AUDIO_AC_MIXER_UNIT            0x04
#define SUD_AUDIO_AC_SELECTOR_UNIT         0x05
#define SUD_AUDIO_AC_FEATURE_UNIT          0x06
#define SUD_AUDIO_AC_PROCESSING_UNIT       0x07
#define SUD_AUDIO_AC_EXTENSION_UNIT        0x08

/* Audio Class-Specific AS Interface Descriptor Subtypes, Audio Spec A.6 */
#define SUD_AUDIO_AS_GENERAL               0x01
#define SUD_AUDIO_AS_FORMAT_TYPE           0x02
#define SUD_AUDIO_AS_FORMAT_SPECIFIC       0x03

/* Audio Class Format Tag */
#define SUD_AUDIO_FORMAT_TYPE_I            0x01
#define SUD_AUDIO_FORMAT_TYPE_II           0x02
#define SUD_AUDIO_FORMAT_TYPE_III          0x03

/* Audio Class-Specific Endpoint Descriptor Subtypes, Audio Spec A.8 */
#define SUD_AUDIO_AS_EP_GENERAL            0x01

/* MS Class-Specific Interface Descriptor Subtypes, Midi Spec A.1 */
#define SUD_AUDIO_MS_HEADER                0x01
#define SUD_AUDIO_MIDI_IN_JACK             0x02
#define SUD_AUDIO_MIDI_OUT_JACK            0x03
#define SUD_AUDIO_MIDI_ELEMENT             0x04

/* MS Class-Specific Endpoint Descriptor Subtypes, Midi Spec A.2 */
#define SUD_AUDIO_MS_EP_GENERAL            0x01

/* HID subclass and protocol */
#define SUD_HID_KBDMOUSE_SUBCLASS          0x01
#define SUD_HID_PROTOCOL_KBD               0x01
#define SUD_HID_PROTOCOL_MOUSE             0x02


/* Mass Storage subclass and protocol */
#define SUD_MS_SCSI_SUBCLASS               0x06
#define SUD_MS_PROTOCOL_CTLBULKINT         0x00
#define SUD_MS_PROTOCOL_CTLBULK            0x01
#define SUD_MS_PROTOCOL_BULKONLY           0x50

/* Video Class */
/* Video Interface Subclass Codes, Videoo Spec A.2 */
#define SUD_VIDEO_SUBCLASS_UNDEF           0x00
#define SUD_VIDEO_SUBCLASS_VC              0x01
#define SUD_VIDEO_SUBCLASS_VS              0x02
#define SUD_VIDEO_SUBCLASS_IC              0x03

/* Video Class-Specific VC Interface Descriptor Subtypes, Video Spec A.5 */
#define SUD_VIDEO_VC_HEADER                0x01
#define SUD_VIDEO_VC_INPUT_TERMINAL        0x02
#define SUD_VIDEO_VC_OUTPUT_TERMINAL       0x03
#define SUD_VIDEO_VC_SELECTOR_UNIT         0x04
#define SUD_VIDEO_VC_PROCESSING_UNIT       0x05
#define SUD_VIDEO_VC_EXTENSION_UNIT        0x06

/* Video Class-Specific VS Interface Descriptor Subtypes, Video Spec A.6 */
#define SUD_VIDEO_VS_INPUT_HEADER          0x01
#define SUD_VIDEO_VS_OUTPUT_HEADER         0x02
#define SUD_VIDEO_VS_STILL_IMAGE_FRAME     0x03
#define SUD_VIDEO_VS_FORMAT_UNCOMPRESSED   0x04
#define SUD_VIDEO_VS_FRAME_UNCOMPRESSED    0x05
#define SUD_VIDEO_VS_FORMAT_MJPEG          0x06
#define SUD_VIDEO_VS_FRAME_MJPEG           0x07
#define SUD_VIDEO_VS_FORMAT_MPEG2TS        0x0A
#define SUD_VIDEO_VS_FORMAT_DV             0x0C
#define SUD_VIDEO_VS_COLORFORMAT           0x0D
#define SUD_VIDEO_VS_FORMAT_FRAME_BASED    0x10
#define SUD_VIDEO_VS_FRAME_FRAME_BASED     0x11
#define SUD_VIDEO_VS_FORMAT_STREAM_BASED   0x12

/* Video Class-Specific Endpoint Descriptor Subtypes, Video Spec A.7 */
#define SUD_VIDEO_EP_GENERAL               0x01
#define SUD_VIDEO_EP_ENDPOINT              0x02
#define SUD_VIDEO_EP_INTERRUPT             0x03

/* USB Terminal Types, Video Spec B.1 */
#define SUD_VIDEO_TT_VENDOR_SPECIFIC       0x0100
#define SUD_VIDEO_TT_STREAMING             0x0101

/* Input Terminal Types, Video Spec B.2 */
#define SUD_VIDEO_ITT_VENDOR_SPECIFIC      0x0200
#define SUD_VIDEO_ITT_CAMERA               0x0201
#define SUD_VIDEO_ITT_MEDIA_TRANSPORT      0x0202

/* Output Terminal Types, Video Spec B.3 */
#define SUD_VIDEO_OTT_VENDOR_SPECIFIC      0x0300
#define SUD_VIDEO_OTT_DISPLAY              0x0301
#define SUD_VIDEO_OTT_MEDIA_TRANSPORT      0x0302

/* External Terminal Types, Video Spec B.4 */
#define SUD_VIDEO_EXT_VENDOR_SPECIFIC      0x0400
#define SUD_VIDEO_COMPOSITE_CONNECTOR      0x0401
#define SUD_VIDEO_SVIDEO_CONNECTOR         0x0402
#define SUD_VIDEO_COMPONENT_CONNECTOR      0x0403

/* Still Image Class */
/* Still Image Interface Subclass Codes, Still Image Spec 6.1.3 */
#define SUD_STILLIMG_SUBCLASS_UNDEF        0x00
#define SUD_STILLIMG_SUBCLASS_CAPTURE      0x01

/* Still Image Interface Protocol Codes, Still Image Spec 6.1.3 */
#define SUD_STILLIMG_PROTOCOL_UNDEF        0x00
#define SUD_STILLIMG_PROTOCOL_PIMA15740    0x01

/* Device Firmware Upgrade Class */
/* Device Firmware Upgrade Codes, DFU Spec 4.1 */
#define SUD_DFU_SUBCLASS_UNDEF             0x00
#define SUD_DFU_SUBCLASS_DFU               0x01

/* Device Firmware Upgrade Codes, DFU Spec 4.1 */
#define SUD_DFU_PROTOCOL_UNDEF             0x00
#define SUD_DFU_PROTOCOL_RUNTIME           0x01
#define SUD_DFU_PROTOCOL_DFU               0x02

/* configuration modifiers */
#define SUD_BMATTRIBUTE_RESERVED           0x80

#if SUD_SELFPOWERED
#define SUD_BMATTRIBUTE_SELF_POWERED       0x40
#else
#define SUD_BMATTRIBUTE_SELF_POWERED       0x0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* function operation */
typedef struct {
    int (*DoEvent)(SUD_DEVICE_HANDLE_T *pDevice, uint event);
    int (*RequestSentDone)(SUD_REQUESTINFO_T *pRequestInfo, int status);
    int (*ReceiveRequest)(SUD_REQUESTINFO_T *pRequestInfo);
    int (*ReceiveSetup)(SUD_REQUESTINFO_T *pRequestInfo);
} SUD_FUNCTION_OPER_T;


/* function driver */
typedef struct SUD_Function_Driver_T {
    const char *pFunctionName;
    const SUD_FUNCTION_OPER_T *pOperation;

    /* device & configuration information */
    SUD_DEVICE_INFO_T *pDeviceInfo;
    SUD_CONFIGURATION_INFO_T *pConfigurationInfo;

    /* device and configuration descriptor */
    SUD_DEVICE_DESC_T *pDeviceDescriptor;
    SUD_CONFIGURATION_HANDLE_T *pConfigurationHandle;

    /* number of configuration */
    uint iConfigurations;

#if SUD_HIGH_SPEED
    SUD_DEV_QUALIFIER_DESC_T *pDevQualifierDescriptor;
#endif
} SUD_FUNCTION_DRIVER_T;

/*============================================================================
                               GLOBAL FUNCTIONS
============================================================================*/

/* called by function driver such as serial */
int  sud_RegisterFunction(struct SUD_Function_Driver_T *);
void sud_DeregisterFunction(struct SUD_Function_Driver_T *);

/* called by the register device function */
int  sud_FunctionInit(SUD_DCD_HANDLE_T *, SUD_DEVICE_HANDLE_T *);
void sud_FunctionRelease(void);

void *sud_PackSetupRequest(SUD_DEV_REQUESTSTRU_T *pRequest, void *pMem);
void sud_UnpackSetupRequest(SUD_DEV_REQUESTSTRU_T *pRequest, void *pMem);

void *sud_PackDeviceDesc(SUD_DEVICE_DESC_T *pDesc, void *pMem);

void *sud_PackConfigDesc(SUD_CONFIGURATION_DESC_T *pDesc, void *pMem);

void *sud_PackInterfaceDesc(SUD_INTERFACE_DESC_T *pDesc, void *pMem);

void *sud_PackEndpointDesc(SUD_ENDPOINT_DESC_T *pDesc, void *pMem);

void *sud_PackQualifierDesc(SUD_DEV_QUALIFIER_DESC_T *pDesc, void *pMem);

void *sud_PackOTGDesc(SUD_OTG_DESC_T *pDesc, void *pMem);

void *sud_PackStringDesc(SUD_STRING_DESC_T *pDesc, void *pMem);

#if SUD_COMPOSITE || SUD_AUDIO || SUD_VIDEO
void *sud_PackIADDesc(SUD_IAD_DESC_T *pDesc, void *pMem);
#endif

#if SUD_HID
void *sud_PackHIDDesc(SUD_CLASS_HID_DESC_T *pDesc, void *pMem);
void *sud_PackReportDesc(SUD_CLASS_REPORT_DESC_T *pDesc, void *pMem);
#endif

#if SUD_CDC
void *sud_PackCDCClassDesc(SUD_CLASS_FUNCTION_DESC_T *pDesc, void *pMem);
#endif

#if SUD_AUDIO
void *sud_PackAudioCtrlClassDesc(SUD_GENERIC_DESC_T *pDesc, void *pMem);
void *sud_PackAudioStreamClassDesc(SUD_GENERIC_DESC_T *pDesc, void *pMem);
void *sud_PackMIDIClassDesc(SUD_GENERIC_DESC_T *pDesc, void *pMem);
#endif
#ifdef __cplusplus
}
#endif

#endif /* SUD_FUNC_H */

