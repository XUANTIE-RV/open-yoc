/*
* uddesc.h                                                  Version 2.53
*
* smxUSBD Descriptor Definition.
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

#ifndef SUD_DESC_H
#define SUD_DESC_H

#ifdef __cplusplus
extern "C" {
#endif

#if __packed_pragma
#pragma pack(1)  /* pack structures (byte alignment) */
#endif

typedef __packed struct {
    u8  bmRequestType;
    u8  bRequest;
    u16 wValue;
    u16 wIndex;
    u16 wLength;
} __packed_gnu SUD_DEV_REQUESTSTRU_T;

#define SUD_SIZEOF_REQUEST_HEAD     8

/* standard usb descriptor structures */

#if SUD_COMPOSITE || SUD_AUDIO || SUD_VIDEO
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;    /* 0x0B */
    u8  bFirstInterface;
    u8  bInterfaceCount;
    u8  bFunctionClass;
    u8  bFunctionSubClass;
    u8  bFunctionProtocol;
    u8  iFunction;
} __packed_gnu SUD_IAD_DESC_T; /* Interface Association Descriptor */

#define SUD_SIZEOF_IAD_DESC     8

#endif /* SUD_COMPOSITE */

typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;    /* 0x9 */
    u8  bmAttributes;
} __packed_gnu SUD_OTG_DESC_T;

#define SUD_SIZEOF_OTG_DESC     3

typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;    /* 0x06 */
    u16 bcdUSB;
    u8  bDeviceClass;
    u8  bDeviceSubClass;
    u8  bDeviceProtocol;
    u8  bMaxPacketSize0;
    u8  bNumConfigurations;
    u8  bReserved;
} __packed_gnu SUD_DEV_QUALIFIER_DESC_T;

#define SUD_SIZEOF_DEV_QUALIFIER_DESC     10

typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;    /* 0x5 */
    u8  bEndpointAddress;
    u8  bmAttributes;
    u16 wMaxPacketSize;
    u8  bInterval;
} __packed_gnu SUD_ENDPOINT_DESC_T;

#define SUD_SIZEOF_ENDPOINT_DESC     7

typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;    /* 0x04 */
    u8  bInterfaceNumber;
    u8  bAlternateSetting;
    u8  bNumEndpoints;
    u8  bInterfaceClass;
    u8  bInterfaceSubClass;
    u8  bInterfaceProtocol;
    u8  iInterface;
} __packed_gnu SUD_INTERFACE_DESC_T;

#define SUD_SIZEOF_INTERFACE_DESC     9

typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;    /* 0x2 */
    u16 wTotalLength;
    u8  bNumInterfaces;
    u8  bConfigurationValue;
    u8  iConfiguration;
    u8  bmAttributes;
    u8  bMaxPower;
} __packed_gnu SUD_CONFIGURATION_DESC_T;

#define SUD_SIZEOF_CONFIGURATION_DESC     9

typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;    /* 0x01 */
    u16 bcdUSB;
    u8  bDeviceClass;
    u8  bDeviceSubClass;
    u8  bDeviceProtocol;
    u8  bMaxPacketSize0;
    u16 idVendor;
    u16 idProduct;
    u16 bcdDevice;
    u8  iManufacturer;
    u8  iProduct;
    u8  iSerialNumber;
    u8  bNumConfigurations;
} __packed_gnu SUD_DEVICE_DESC_T;

#define SUD_SIZEOF_DEVICE_DESC     18

typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDataItem[2];
} __packed_gnu SUD_STRING_DESC_T;

#define SUD_SIZEOF_STRING_DESC     4

typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
} __packed_gnu SUD_GENERIC_DESC_T;

typedef __packed struct {
    union {
        SUD_GENERIC_DESC_T GenericDesc;
        SUD_ENDPOINT_DESC_T EndpointDesc;
        SUD_INTERFACE_DESC_T InterfaceDesc;
        SUD_CONFIGURATION_DESC_T ConfigurationDesc;
        SUD_DEVICE_DESC_T DeviceDesc;
        SUD_STRING_DESC_T StringDesc;
        SUD_DEV_QUALIFIER_DESC_T DevQualifierDesc;
#if SUD_COMPOSITE || SUD_AUDIO || SUD_VIDEO
        SUD_IAD_DESC_T IADDesc;
#endif
    } __packed_gnu Descriptor;

} __packed_gnu SUD_DESCRIPTOR_T;


/* communications class descriptor structures. CDC 5.2 Table 25c */

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
} __packed_gnu SUD_CLASS_FUNCTION_DESC_T;

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bmCapabilities;
} __packed_gnu SUD_CLASS_FUNCTION_GENERIC_DESC_T;

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x00 */
    u16 bcdCDC;
} __packed_gnu SUD_CLASS_HEADER_FUNCTION_DESC_T;

#define SUD_SIZEOF_CDC_HEADER_DESC 5

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x01 */
    u8  bmCapabilities;
    u8  bDataInterface;
} __packed_gnu SUD_CLASS_CALL_MANAGEMENT_DESC_T;

#define SUD_SIZEOF_CDC_CM_DESC 5

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x02 */
    u8  bmCapabilities;
} __packed_gnu SUD_CLASS_ACM_DESC_T;

#define SUD_SIZEOF_CDC_ACM_DESC 4

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x03 */
} __packed_gnu SUD_CLASS_DIRECT_LINE_DESC_T;

#define SUD_SIZEOF_CDC_DIRECT_LINE_DESC 3

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x04 */
    u8  bRingerVolSeps;
    u8  bNumRingerPatterns;
} __packed_gnu SUD_CLASS_TELE_RINGER_DESC_T;

#define SUD_SIZEOF_CDC_TELE_RINGER_DESC 5

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x05 */
    u8  bmCapabilities;
} __packed_gnu SUD_CLASS_TELE_CALL_DESC_T;

#define SUD_SIZEOF_CDC_TELE_CALL_DESC 4

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x06 */
    u8  bMasterInterface;
    u8  bSlaveInterface0[1];
} __packed_gnu SUD_CLASS_UNION_FUNCTION_DESC_T;

#define SUD_SIZEOF_CDC_UF_DESC 5

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x07 */
    u8  iCountryCodeRelDate;
    u16 wCountryCode0[1];
} __packed_gnu SUD_CLASS_COUNTRY_SELECT_DESC_T;

#define SUD_SIZEOF_CDC_COUNTRY_SELECT_DESC 6

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x08 */
    u8  bmCapabilities;
} __packed_gnu SUD_CLASS_TELE_OPER_DESC_T;

#define SUD_SIZEOF_CDC_TELE_OPER_DESC 4

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x09 */
    u8  bEntityId;
    u8  bInterfaceNo;
    u8  bOutInterfaceNo;
    u8  bmOptions;
    u8  bChild0[1];
} __packed_gnu SUD_CLASS_TERMINAL_DESC_T;

#define SUD_SIZEOF_CDC_TERMINAL_DESC 8

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x0a */
    u8  bEntityId;
    u8  iName;
    u8  bChannelIndex;
    u8  bPhysicalInterface;
} __packed_gnu SUD_CLASS_NET_CHANNEL_DESC_T;

#define SUD_SIZEOF_CDC_NET_CHANNEL_DESC 7

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x0b */
    u8  bEntityId;
    u8  bProtocol;
    u8  bChild0[1];
} __packed_gnu SUD_CLASS_PROTO_UNIT_FUNCTION_DESC_T;

#define SUD_SIZEOF_CDC_PROTO_UF_DESC 6

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x0c */
    u8  bEntityId;
    u8  bExtensionCode;
    u8  iName;
    u8  bChild0[1];
} __packed_gnu SUD_CLASS_EXTENSION_UNIT_DESC_T;

#define SUD_SIZEOF_CDC_EXTENSION_UNIT_DESC 7

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x0d */
    u8  bmCapabilities;
} __packed_gnu SUD_CLASS_MULTI_CHANNEL_DESC_T;

#define SUD_SIZEOF_CDC_MULTI_CHANNEL_DESC 4

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x0e */
    u8  bmCapabilities;
} __packed_gnu SUD_CLASS_CAPI_CONTROL_DESC_T;

#define SUD_SIZEOF_CDC_CAPI_CONTROL_DESC 4

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x0f */
    u8  iMACAddress;
    u32 bmEthernetStatistics;
    u16 wMaxSegmentSize;
    u16 wNumberMCFilters;
    u8  bNumberPowerFilters;
} __packed_gnu SUD_CLASS_ETHERNET_DESC_T;

#define SUD_SIZEOF_CDC_ETHERNET_DESC 13

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x10 */
    u8  iEndSystermIdentifier;
    u8  bmDataCapabilities;
    u8  bmATMDeviceStatistics;
    u16 wType2MaxSegmentSize;
    u16 wType3MaxSegmentSize;
    u16 wMaxVC;
} __packed_gnu SUD_CLASS_ATM_DESC_T;

#define SUD_SIZEOF_CDC_ATM_DESC 12

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x12 */
    u16 bcdVersion;
    u8  bGUID[16];
} __packed_gnu SUD_CLASS_MDLM_DESC_T;

#define SUD_SIZEOF_CDC_MDLM_DESC 21

typedef __packed struct {
    u8  bFunctionLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;    /* 0x13 */
    u8  bGuidDescriptorType;
    u8  bDetailData[1];
} __packed_gnu SUD_CLASS_MDLMD_DESC_T;

#define SUD_SIZEOF_CDC_MDLMD_DESC 5

/* HID class descriptor structures. HID 6.2.1 */

typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u16 bcdVersion;
    u8  bCountryCode;
    u8  bNumDescriptors;    /* 0x01 */
    u8  bDescriptorType0;
    u8  bDescriptorLength0;
    u8  bDescriptorLength1;
    /* optional descriptors are not supported. */
} __packed_gnu SUD_CLASS_HID_DESC_T;

#define SUD_SIZEOF_HID_DESC    9

typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u16 wLength;
    u8  bData[2];
} __packed_gnu SUD_CLASS_REPORT_DESC_T;

#define SUD_SIZEOF_REPORT_DESC    6

/* Audio class descriptor structures. */
/* AC descriptor */
typedef __packed struct { /* Table 4-2 */
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u16 bcdADC;
    u16 wTotalLength;
    u8  bInCollection;
    u8  baInterfaceNr[1]; /* real size is sizeof(SUD_CLASS_AC_HEADER_DESC_T) + (bInCollection - 1) */
} __packed_gnu SUD_CLASS_AC_HEADER_DESC_T;

#define SUD_SIZEOF_AC_HEADER_DESC 9

typedef __packed struct { /* Table 4-3 */
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u8  bNrChannels;
    u16 wChannelConfig;
    u8  iChannelNames;
    u8  iTerminal;
} __packed_gnu SUD_CLASS_AC_IT_DESC_T;

#define SUD_SIZEOF_AC_IT_DESC 12

typedef __packed struct { /* Table 4-4 */
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u8  bSourceID;
    u8  iTerminal;
} __packed_gnu SUD_CLASS_AC_OT_DESC_T;

#define SUD_SIZEOF_AC_OT_DESC 9

/* Audio Feature Unit Descriptor */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bUnitID;
    u8  bSourceID;
    u8  bControlSize;
    u8  bmaControls[1];
    /* u8  iFeature; */ /* real size is sizeof(SUD_CLASS_AC_FU_DESC_T) + (Channel*bControlSize) */
} __packed_gnu SUD_CLASS_AC_FU_DESC_T;

#define SUD_SIZEOF_AC_FU_DESC 7

typedef __packed struct {
    u8  bNrChannels;
    u16 wChannelConfig;
    u8  iChannelNames;
    u8  bmControls[1];
    /*u8  iMixer;*/
} __packed_gnu SUD_CLASS_AC_MIXER_CHANNEL_DESC_T;

#define SUD_SIZEOF_AC_MIXER_CHANNEL_DESC 5

/* Audio Mixer Unit Descriptor */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
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
} __packed_gnu SUD_CLASS_AC_MIXER_DESC_T; /* real size is sizeof(SUD_CLASS_AC_MIXER_DESC_T) + 2*bNrInPins - 1 + sizeof(SUD_CLASS_AC_MIXER_CHANNEL_DESC_T) */

#define SUD_SIZEOF_AC_MIXER_DESC 6

/* Audio Selector Unit Descriptor */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bUnitID;
    u8  bNrInPins;
    u8  baSourceID[1];
    /*u8  iSelector; */ /* real size is sizeof(SUD_CLASS_AC_SELECTOR_DESC_T) + (bNrInPins) */
} __packed_gnu SUD_CLASS_AC_SELECTOR_DESC_T;

#define SUD_SIZEOF_AC_SELECTOR_DESC 6

/* AS descriptor */
typedef __packed struct { /* Table 4-19 */
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bTerminalLink;
    u8  bDelay;
    u16 wFormatTag;
} __packed_gnu SUD_CLASS_AS_GENERAL_DESC_T;

#define SUD_SIZEOF_AS_GENERAL_DESC 7

/* Type I format descriptor, Table 2-1 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bFormatType;
    u8  bNrChannels;
    u8  bSubFrameSize;
    u8  bBitResolution;
    u8  bSamFreqType;
    u8  tSamFreq[1][3];/* real size is sizeof(SUD_CLASS_AC_SELECTOR_DESC_T) + (bSamFreqType -1)*3 */
} __packed_gnu SUD_CLASS_AS_FORMAT_TYPEI_DESC_T;

#define SUD_SIZEOF_AS_FORMAT_TYPEI_DESC 11

/* Audio Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bmAttributes;
    u8  bLockDelayUnits;
    u16 wLockDelay;
} __packed_gnu SUD_CLASS_AS_ENDPOINT_DESC_T;

#define SUD_SIZEOF_AS_ENDPOINT_DESC 7

/* Audio Endpoint Descriptor, Two bytes extension */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;    /* 0x5 */
    u8  bEndpointAddress;
    u8  bmAttributes;
    u16 wMaxPacketSize;
    u8  bInterval;
    u8  bFresh;
    u8  bSynchAddress;
} __packed_gnu SUD_ENDPOINT_AUDIO_DESC_T;


/* MS descriptor */
/* MS Interface Header Descriptor */
typedef __packed struct { /* Table 6-2 */
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u16 bcdMSC;
    u16 wTotalLength;
} __packed_gnu SUD_CLASS_MS_HEADER_DESC_T;

#define SUD_SIZEOF_MS_HEADER_DESC 7

/* MIDI IN Jack Descriptor */
typedef __packed struct { /* Table 6-3 */
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bJackType;
    u8  bJackID;
    u8  iJack;
} __packed_gnu SUD_CLASS_MIDI_IN_DESC_T;

#define SUD_SIZEOF_MIDI_IN_DESC 6

/* MIDI OUT Jack Descriptor */
typedef __packed struct { /* Table 6-4 */
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bJackType;
    u8  bJackID;
    u8  bNrInputPins;
    u8  baSourceID[1];
    u8  baSourcePin[1];
    u8  iJack;
} __packed_gnu SUD_CLASS_MIDI_OUT_DESC_T; /* real size is sizeof(SUD_CLASS_MIDI_OUT_DESC_T) + (bNrInputPins -1)*2 */

#define SUD_SIZEOF_MIDI_OUT_DESC 9

/* Audio Class-Specific MS Isochronous Audio Data Endpoint Descriptor */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bNumEmbMIDIJack;
    u8  baAssocJackID[1];
} __packed_gnu SUD_CLASS_MS_ENDPOINT_DESC_T; /* real size is sizeof(SUD_CLASS_MS_ENDPOINT_DESC_T) + (bNumEmbMIDIJack-1) */

#define SUD_SIZEOF_MS_ENDPOINT_DESC 5

/* Video class descriptor structures. */
/* VC descriptor */
typedef __packed struct { /* Table 3-3 */
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u16 bcdUVC;
    u16 wTotalLength;
    u32 dwClockFrequency;
    u8  bInCollection;
    u8  baInterfaceNr[1]; /* real size is sizeof(SUD_CLASS_VC_HEADER_DESC_T) + (bInCollection - 1) */
} __packed_gnu SUD_CLASS_VC_HEADER_DESC_T;

#define SUD_SIZEOF_VC_HEADER_DESC 13

typedef __packed struct { /* Table 3-4 */
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u8  iTerminal;
} __packed_gnu SUD_CLASS_VC_IT_DESC_T;

#define SUD_SIZEOF_VC_IT_DESC 8

typedef __packed struct { /* Table 3-5 */
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u8  bSourceID;
    u8  iTerminal;
} __packed_gnu SUD_CLASS_VC_OT_DESC_T;

#define SUD_SIZEOF_VC_OT_DESC 9

typedef __packed struct { /* Table 3-6 */
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u8  iTerminal;
    u16 wObjectiveFocalLengthMin;
    u16 wObjectiveFocalLengthMax;
    u16 wOcularFocalLength;
    u8  bControlSize;
    u8  bmControls[1]; /* real size is sizeof(SUD_CLASS_VC_CAMERA_DESC_T) + bControlSize - 1 */
} __packed_gnu SUD_CLASS_VC_CAMERA_DESC_T;

#define SUD_SIZEOF_VC_CAMERA_DESC 14

/* Video Selector Unit Descriptor, Table 3-7 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bUnitID;
    u8  bNrInPins;
    u8  baSourceID[1];
    /* u8  iSelector; */ /* real size is sizeof(SUD_CLASS_VC_SU_DESC_T) + bNrInPins */
} __packed_gnu SUD_CLASS_VC_SU_DESC_T;

#define SUD_SIZEOF_VC_SU_DESC 6

/* Video Processing Unit Descriptor, Table 3-8 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bUnitID;
    u8  bSourceID;
    u16 wMaxMultiplier;
    u8  bControlSize;
    u8  bmControls[2];
    /*
    u8  iProcessing;
    u8  bmVideoStandards;
    */
} __packed_gnu SUD_CLASS_VC_PU_DESC_T; /* real size is sizeof(SUD_CLASS_VC_PU_DESC_T) + 2 + bControlSize - 2 */

#define SUD_SIZEOF_VC_PU_DESC 10

/* Video Extension Unit Descriptor, Table 3-9 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
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
} __packed_gnu SUD_CLASS_VC_EU_DESC_T; /* real size is sizeof(SUD_CLASS_VC_EU_DESC_T) + bNrInPins - 1 */

#define SUD_SIZEOF_VC_EU_DESC 23

/* Video Class-Specific VC Interrupt Endpoint Descriptor, Table 3-11 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u16 wMaxTransferSize;
} __packed_gnu SUD_CLASS_VC_INT_ENDPOINT_DESC_T;

#define SUD_SIZEOF_VC_INT_ENDPOINT_DESC 5

/* Video Class-Specific VS Input Header Descriptor, Table 3-13 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bNumFormats;
    u16 wTotalLength;
    u8  bEndpointAddress;
    u8  bmInfo;
    u8  bTerminalLink;
    u8  bStillCaptureMethod;
    u8  bTriggerSupport;
    u8  bTriggerUsage;
    u8  bControlSize;
    u8  bmaControls[1];
} __packed_gnu SUD_CLASS_VS_INPUT_HEADER_DESC_T;

#define SUD_SIZEOF_VS_INPUT_HEADER_DESC 14

/* Video Class-Specific VS Output Header Descriptor, Table 3-14 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bNumFormats;
    u16 wTotalLength;
    u8  bEndpointAddress;
    u8  bTerminalLink;
#if SUD_VIDEO_USE_11
    u8  bControlSize;
    u8  bmaControls[1];
#endif
} __packed_gnu SUD_CLASS_VS_OUTPUT_HEADER_DESC_T;

#if SUD_VIDEO_USE_11
#define SUD_SIZEOF_VS_OUTPUT_HEADER_DESC 10
#else
#define SUD_SIZEOF_VS_OUTPUT_HEADER_DESC 8
#endif

/* Video Class-Specific Still Image Frame Descriptor, Table 3-17 */

typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bEndpointAddress;
    u8  bNumImageSizePatterns;
    u16 wWidth[1];
    u16 wHeight[1];
    /*
    u8  bNumCompressionPattern;
    u8  bCompression[0];
    */
} __packed_gnu SUD_CLASS_VS_STILL_IMAGE_FRAME_DESC_T; /* real size is sizeof(SUD_CLASS_VS_STILL_IMAGE_FRAME_DESC_T) + (bNumNumImageSizePatterns - 1)*sizeof(u32) + 1 + bNumCompressionPattern */

#define SUD_SIZEOF_VS_STILL_IMAGE_FRAME_DESC 9

/* Video Class-Specific VS Format Descriptor, Uncompressed, Table 3-1 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bFormatIndex;
    u8  bNumFrameDescriptors;
    u8  guidFormat[16];
    u8  bBitsPerPixel;
    u8  bDefaultFrameIndex;
    u8  bAspectRatioX;
    u8  bAspectRatioY;
    u8  bmInterlaceFlags;
    u8  bCopyProtect;
} __packed_gnu SUD_CLASS_VS_FORMAT_UNCOMPRESSED_DESC_T;

#define SUD_SIZEOF_VS_FORMAT_UNCOMPRESSED_DESC 27

/* Video Class-Specific VS Frame Descriptor, Uncompressed, Table 3-2 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bFrameIndex;
    u8  bmCapabilities;
    u16 wWidth;
    u16 wHeight;
    u32 dwMinBitRate;
    u32 dwMaxBitRate;
    u32 dwMaxVideoFrameBufferSize;
    u32 dwDefaultFrameInterval;
    u8  bFrameIntervalType;
    u32 dwFrameInterval[1];
} __packed_gnu SUD_CLASS_VS_FRAME_UNCOMPRESSED_DESC_T;

#define SUD_SIZEOF_VS_FRAME_UNCOMPRESSED_DESC 30

/* Video Class-Specific VS Format Descriptor, MJPEG, Table 3-1 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bFormatIndex;
    u8  bNumFrameDescriptors;
    u8  bmFlags;
    u8  bDefaultFrameIndex;
    u8  bAspectRatioX;
    u8  bAspectRatioY;
    u8  bmInterlaceFlags;
    u8  bCopyProtect;
} __packed_gnu SUD_CLASS_VS_FORMAT_MJPEG_DESC_T;

#define SUD_SIZEOF_VS_FORMAT_MJPEG_DESC 11

/* Video Class-Specific VS Frame Descriptor, MJPEG, Table 3-2 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bFrameIndex;
    u8  bmCapabilities;
    u16 wWidth;
    u16 wHeight;
    u32 dwMinBitRate;
    u32 dwMaxBitRate;
    u32 dwMaxVideoFrameBufferSize;
    u32 dwDefaultFrameInterval;
    u8  bFrameIntervalType;
    u32 dwFrameInterval[1];
} __packed_gnu SUD_CLASS_VS_FRAME_MJPEG_DESC_T;

#define SUD_SIZEOF_VS_FRAME_MJPEG_DESC 30

/* Video Class-Specific VS Format Descriptor, MPEG2-TS, Table 3-1 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bFormatIndex;
    u8  bDataOffset;
    u8  bPacketLength;
    u8  bStrideLength;
    u8  guidStrideFormat[16];
} __packed_gnu SUD_CLASS_VS_FORMAT_MPEG2TS_DESC_T;

#define SUD_SIZEOF_VS_FORMAT_MPEG2TS_DESC 23

/* Video Class-Specific VS Format Descriptor, DV, Table 3-1 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bFormatIndex;
    u32 dwMaxVideoFrameBufferSize;
    u8  bFormatType;
} __packed_gnu SUD_CLASS_VS_FORMAT_DV_DESC_T;

#define SUD_SIZEOF_VS_FORMAT_DV_DESC 9

/* Video Class-Specific VS Color Format Descriptor, Table 3-18 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bDescriptorSubtype;
    u8  bColorPrimaries;
    u8  bTransferCharacteristics;
    u8  bMatrixCoefficients;
} __packed_gnu SUD_CLASS_VS_COLOR_FORMAT_DESC_T;

#define SUD_SIZEOF_VS_COLOR_FORMAT_DESC 6

/* DFU Class-Specific Function Descriptor, Table 3-18 */
typedef __packed struct {
    u8  bLength;
    u8  bDescriptorType;
    u8  bmAttributes;
    u16 wDetachTimeOut;
    u16 wTransferSize;
    u16 bcdDFUVersion;
} __packed_gnu SUD_CLASS_DFU_FUNC_DESC_T;

#define SUD_SIZEOF_DFU_FUNC_DESC 9

typedef __packed struct SUD_Class_Desc_T {
    union {
        SUD_CLASS_FUNCTION_DESC_T FunctionDesc;
        SUD_CLASS_FUNCTION_GENERIC_DESC_T GenericDesc;
#if SUD_CDC
        SUD_CLASS_HEADER_FUNCTION_DESC_T HeaderFuncDesc;
        SUD_CLASS_CALL_MANAGEMENT_DESC_T CallManagementDesc;
        SUD_CLASS_ACM_DESC_T ACMDesc;
        SUD_CLASS_DIRECT_LINE_DESC_T DirectLineDesc;
        SUD_CLASS_TELE_RINGER_DESC_T TeleRingerDesc;
        SUD_CLASS_TELE_OPER_DESC_T TeleOperDesc;
        SUD_CLASS_TELE_CALL_DESC_T TeleCallDesc;
        SUD_CLASS_UNION_FUNCTION_DESC_T UnionFuncDesc;
        SUD_CLASS_COUNTRY_SELECT_DESC_T CountrySelectionDesc;
        SUD_CLASS_TERMINAL_DESC_T TerminalDesc;
        SUD_CLASS_NET_CHANNEL_DESC_T NetChannelDesc;
        SUD_CLASS_EXTENSION_UNIT_DESC_T ExtensionUnitDesc;
        SUD_CLASS_MULTI_CHANNEL_DESC_T MultiChannelDesc;
        SUD_CLASS_CAPI_CONTROL_DESC_T CAPIControlDesc;
        SUD_CLASS_ETHERNET_DESC_T EthernetDesc;
        SUD_CLASS_ATM_DESC_T ATMDesc;
        SUD_CLASS_MDLM_DESC_T MDLMDesc;
        SUD_CLASS_MDLMD_DESC_T MDLMDDesc;
#endif
#if SUD_HID
        SUD_CLASS_HID_DESC_T HIDDesc;
#endif
#if SUD_AUDIO
        SUD_CLASS_AC_HEADER_DESC_T ACHeaderDesc;
        SUD_CLASS_AC_IT_DESC_T ACITDesc;
        SUD_CLASS_AC_OT_DESC_T ACOTDesc;
        SUD_CLASS_AC_FU_DESC_T ACFeatureUnitDesc;
        SUD_CLASS_AC_MIXER_DESC_T ACMixerDesc;
        SUD_CLASS_AC_SELECTOR_DESC_T ACSelectorDesc;
        SUD_CLASS_AS_GENERAL_DESC_T ASGeneralDesc;
        SUD_CLASS_AS_FORMAT_TYPEI_DESC_T ASFormatTypeIDesc;
        SUD_CLASS_AS_ENDPOINT_DESC_T AudioEndpointDesc;
        SUD_CLASS_MS_HEADER_DESC_T MSHeaderDesc;
        SUD_CLASS_MIDI_IN_DESC_T MSINDesc;
        SUD_CLASS_MIDI_OUT_DESC_T MSOUTDesc;
        SUD_CLASS_MS_ENDPOINT_DESC_T MSEndpointDesc;
#endif
#if SUD_VIDEO
        SUD_CLASS_VC_HEADER_DESC_T VCHeaderDesc;
        SUD_CLASS_VC_IT_DESC_T VCITDesc;
        SUD_CLASS_VC_OT_DESC_T VCOTDesc;
        SUD_CLASS_VC_CAMERA_DESC_T VCCameraDesc;
        SUD_CLASS_VC_SU_DESC_T VCSUDesc;
        SUD_CLASS_VC_PU_DESC_T VCPUDesc;
        SUD_CLASS_VC_EU_DESC_T VCEUDesc;
        SUD_CLASS_VC_INT_ENDPOINT_DESC_T VCIntEndpointDesc;
        SUD_CLASS_VS_INPUT_HEADER_DESC_T VSInputHeaderDesc;
        SUD_CLASS_VS_OUTPUT_HEADER_DESC_T VSOutputHeaderDesc;
        SUD_CLASS_VS_STILL_IMAGE_FRAME_DESC_T VSStillImageFrameDesc;
        SUD_CLASS_VS_FORMAT_UNCOMPRESSED_DESC_T VSFormatUncompressedDesc;
        SUD_CLASS_VS_FRAME_UNCOMPRESSED_DESC_T VSFrameUncompressedDesc;
        SUD_CLASS_VS_FORMAT_MJPEG_DESC_T VSFormatMJPEGDesc;
        SUD_CLASS_VS_FRAME_MJPEG_DESC_T VSFrameMJPEGDesc;
        SUD_CLASS_VS_FORMAT_MPEG2TS_DESC_T VSFormatMPEG2TSDesc;
        SUD_CLASS_VS_FORMAT_DV_DESC_T VSFormatDVDesc;
        SUD_CLASS_VS_COLOR_FORMAT_DESC_T VSColorFormatDesc;
#endif
    } __packed_gnu Descriptor;

} __packed_gnu SUD_CLASS_DESC_T;

#if __packed_pragma
#pragma pack()  /* no par means restore default field alignment */
#endif

typedef struct {
    u8  bDescriptorSubtype;
    u16 bcdCDC;
} SUD_HEADER_INFO_T;

typedef struct {
    u8  bmCapabilities;
    u8  bDataInterface;
} SUD_CALL_MANAGEMENT_INFO_T;

typedef struct {
    u8  bmCapabilities;
} SUD_ACM_INFO_T;

typedef struct {
    u8  bMasterInterface;
    u8  bSlaveInterface[1];
} SUD_UNION_FUNCTION_INFO_T;

typedef struct {
    char *iMACAddress;
    u8  bmEthernetStatistics;
    u16 wMaxSegmentSize;
    u16 wNumberMCFilters;
    u8  bNumberPowerFilters;
} SUD_ETHERNET_INFO_T;

typedef struct {
    u16 bcdVersion;
    u8  bGUID[16];
} SUD_MDLM_INFO_T;

typedef struct {
    u8 bGuidDescriptorType;
    u8 bDetailData[2];
} SUD_MDLMD_INFO_T;


/* HID class types. USB Device Class Definitions for Human Interface Devices (HID) */

typedef struct {
    u16 bcdVersion;
    u8  bCountryCode;
    u8  bDescriptorType;
    u16 wDescriptorLength;
    u8 *pReportDesc;
} SUD_HID_INFO_T;

/* Audio Class */

typedef struct {
    u16 bcdADC;
    u8  bInCollection;
} SUD_AC_HEADER_INFO_T;

typedef struct {
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u8  bNrChannels;
    u16 wChannelConfig;
} SUD_AC_IT_INFO_T;

typedef struct {
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u8  bSourceID;
} SUD_AC_OT_INFO_T;

typedef struct {
    u8  bUnitID;
    u8  bSourceID;
    u8  bControlSize;
    u8  bChannels;
    u16 bmaControls[6];
} SUD_AC_FU_INFO_T;

typedef struct {
    u8  bUnitID;
    u8  bNrInPins;
    u8  bNrChannels;
    u8  bSourceID[13];
    u16 wChannelConfig;
    u8  bmControls[13];
} SUD_AC_MIXER_INFO_T;

typedef struct {
    u8  bUnitID;
    u8  bNrInPins;
    u8  baSourceID[12];
} SUD_AC_SELECTOR_INFO_T;

typedef struct {
    u8  bTerminalLink;
    u8  bDelay;
    u16 wFormatTag;
} SUD_AS_GENERAL_INFO_T;

typedef struct {
    u8  bFormatType;
    u8  bNrChannels;
    u8  bSubFrameSize;
    u8  bBitResolution;
    u8  bSamFreqType;
    u32 tSamFreq[11];
} SUD_AS_FORMAT_TYPEI_INFO_T;

typedef struct {
    u8  bmAttributes;
    u8  bLockDelayUnits;
    u16 wLockDelay;
} SUD_AS_ENDPOINT_INFO_T;

typedef struct {
    u16 bcdMSC;
} SUD_MS_HEADER_INFO_T;

typedef struct {
    u8  bJackType;
    u8  bJackID;
} SUD_MIDI_IN_INFO_T;

typedef struct {
    u8  bJackType;
    u8  bJackID;
    u8  bNrInputPins;
    u8  baSourceID[8];
    u8  baSourcePin[8];
} SUD_MIDI_OUT_INFO_T;

typedef struct {
    u8  bNumEmbMIDIJack;
    u8  baAssocJackID[8];
} SUD_MS_ENDPOINT_INFO_T;

/* Video Class */
typedef struct {
    u16 bcdUVC;
    u16 wTotalLength;
    u32 dwClockFrequency;
    u8  bInCollection;
} SUD_VC_HEADER_INFO_T;

typedef struct {
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
} SUD_VC_IT_INFO_T;

typedef struct {
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u8  bSourceID;
} SUD_VC_OT_INFO_T;

typedef struct {
    u8  bTerminalID;
    u16 wTerminalType;
    u8  bAssocTerminal;
    u16 wObjectiveFocalLengthMin;
    u16 wObjectiveFocalLengthMax;
    u16 wOcularFocalLength;
    u8  bControlSize;
    u8  bmControls[3];
} SUD_VC_CAMERA_INFO_T;

typedef struct {
    u8  bUnitID;
    u8  bNrInPins;
    u8  baSourceID[6];
} SUD_VC_SU_INFO_T;

typedef struct {
    u8  bUnitID;
    u8  bSourceID;
    u16 wMaxMultiplier;
    u8  bControlSize;
    u8  bmControls[4];
    u8  bmVideoStandards;
} SUD_VC_PU_INFO_T;

typedef struct {
    u8  bUnitID;
    u8  guidExtensionCode[16];
    u8  bNumControls;
    u8  bNrInPins;
    u8  baSourceID[5];
    u8  bControlSize;
    u8  bmControls[7];
} SUD_VC_EU_INFO_T;

typedef struct {
    u16 wMaxTransferSize;
} SUD_VC_INT_ENDPOINT_INFO_T;

typedef struct {
    u8  bNumFormats;
    u16 wTotalLength;
    u8  bEndpointAddress;
    u8  bmInfo;
    u8  bTerminalLink;
    u8  bStillCaptureMethod;
    u8  bTriggerSupport;
    u8  bTriggerUsage;
    u8  bControlSize;
    u8  bmaControls[8];
} SUD_VS_INPUT_HEADER_INFO_T;

typedef struct {
    u8  bNumFormats;
    u16 wTotalLength;
    u8  bEndpointAddress;
    u8  bTerminalLink;
    u8  bControlSize;
    u8  bmaControls[8];
} SUD_VS_OUTPUT_HEADER_INFO_T;

typedef struct {
    u8  bEndpointAddress;
    u8  bNumImageSizePatterns;
    u16 wWidth[8];
    u16 wHeight[8];
    u8  bNumCompressionPattern;
    u8  bCompression[8];
} SUD_VS_STILL_IMAGE_FRAME_INFO_T;

typedef struct {
    u8  bFormatIndex;
    u8  bNumFrameDescriptors;
    u8  guidFormat[16];
    u8  bBitsPerPixel;
    u8  bDefaultFrameIndex;
    u8  bAspectRatioX;
    u8  bAspectRatioY;
    u8  bmInterlaceFlags;
    u8  bCopyProtect;
} SUD_VS_FORMAT_UNCOMPRESSED_INFO_T;

typedef struct {
    u8  bFrameIndex;
    u8  bmCapabilities;
    u16 wWidth;
    u16 wHeight;
    u32 dwMinBitRate;
    u32 dwMaxBitRate;
    u32 dwMaxVideoFrameBufferSize;
    u32 dwDefaultFrameInterval;
    u8  bFrameIntervalType;
    u32 dwFrameInterval[7];
} SUD_VS_FRAME_UNCOMPRESSED_INFO_T;

typedef struct {
    u8  bFormatIndex;
    u8  bNumFrameDescriptors;
    u8  bmFlags;
    u8  bDefaultFrameIndex;
    u8  bAspectRatioX;
    u8  bAspectRatioY;
    u8  bmInterlaceFlags;
    u8  bCopyProtect;
} SUD_VS_FORMAT_MJPEG_INFO_T;

typedef struct {
    u8  bFrameIndex;
    u8  bmCapabilities;
    u16 wWidth;
    u16 wHeight;
    u32 dwMinBitRate;
    u32 dwMaxBitRate;
    u32 dwMaxVideoFrameBufferSize;
    u32 dwDefaultFrameInterval;
    u8  bFrameIntervalType;
    u32 dwFrameInterval[7];
} SUD_VS_FRAME_MJPEG_INFO_T;

typedef struct {
    u8  bFormatIndex;
    u8  bDataOffset;
    u8  bPacketLength;
    u8  bStrideLength;
    u8  guidStrideFormat[16];
} SUD_VS_FORMAT_MPEG2TS_INFO_T;

typedef struct {
    u8  bFormatIndex;
    u32 dwMaxVideoFrameBufferSize;
    u8  bFormatType;
} SUD_VS_FORMAT_DV_INFO_T;

typedef struct {
    u8  bColorPrimaries;
    u8  bTransferCharacteristics;
    u8  bMatrixCoefficients;
} SUD_VS_COLOR_FORMAT_INFO_T;

typedef struct {
    u8 bDescriptorType;
    u8 bDescriptorSubtype;
    u8 bElements;
    union {
        SUD_HEADER_INFO_T HeaderInfo;
#if SUD_CDC
        SUD_CALL_MANAGEMENT_INFO_T CallManagementInfo;
        SUD_ACM_INFO_T ACMInfo;
        SUD_UNION_FUNCTION_INFO_T UnionFuncInfo;
        SUD_ETHERNET_INFO_T EthernetInfo;
        SUD_MDLM_INFO_T MDLMInfo;
        SUD_MDLMD_INFO_T MDLMDInfo;
#endif
#if SUD_HID
        SUD_HID_INFO_T HIDInfo;
#endif
#if SUD_AUDIO
        SUD_AC_HEADER_INFO_T ACHeaderInfo;
        SUD_AC_IT_INFO_T ACITInfo;
        SUD_AC_OT_INFO_T ACOTInfo;
        SUD_AC_FU_INFO_T ACFeatureUnitInfo;
        SUD_AC_MIXER_INFO_T ACMixerInfo;
        SUD_AC_SELECTOR_INFO_T ACSelectorInfo;
        SUD_AS_GENERAL_INFO_T ASGeneralInfo;
        SUD_AS_FORMAT_TYPEI_INFO_T ASFormatTypeIInfo;
        SUD_AS_ENDPOINT_INFO_T AudioEndpointInfo;
        SUD_MS_HEADER_INFO_T MSHeaderInfo;
        SUD_MIDI_IN_INFO_T MSINInfo;
        SUD_MIDI_OUT_INFO_T MSOUTInfo;
        SUD_MS_ENDPOINT_INFO_T MSEndpointInfo;
#endif
#if SUD_VIDEO
        SUD_VC_HEADER_INFO_T VCHeaderInfo;
        SUD_VC_IT_INFO_T VCITInfo;
        SUD_VC_OT_INFO_T VCOTInfo;
        SUD_VC_CAMERA_INFO_T VCCameraInfo;
        SUD_VC_SU_INFO_T VCSUInfo;
        SUD_VC_PU_INFO_T VCPUInfo;
        SUD_VC_EU_INFO_T VCEUInfo;
        SUD_VC_INT_ENDPOINT_INFO_T VCIntEndpointInfo;
        SUD_VS_INPUT_HEADER_INFO_T VSInputHeaderInfo;
        SUD_VS_OUTPUT_HEADER_INFO_T VSOutputHeaderInfo;
        SUD_VS_STILL_IMAGE_FRAME_INFO_T VSStillImageFrameInfo;
        SUD_VS_FORMAT_UNCOMPRESSED_INFO_T VSFormatUncompressedInfo;
        SUD_VS_FRAME_UNCOMPRESSED_INFO_T VSFrameUncompressedInfo;
        SUD_VS_FORMAT_MJPEG_INFO_T VSFormatMJPEGInfo;
        SUD_VS_FRAME_MJPEG_INFO_T VSFrameMJPEGInfo;
        SUD_VS_FORMAT_MPEG2TS_INFO_T VSFormatMPEG2TSInfo;
        SUD_VS_FORMAT_DV_INFO_T VSFormatDVInfo;
        SUD_VS_COLOR_FORMAT_INFO_T VSColorFormatInfo;
#endif
    } Info;
} SUD_CLASS_INFO_T;

#ifdef __cplusplus
}
#endif

#endif /* SUD_DESC_H */

