/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
/*
 * USB Video Class definitions.
 *
 * Copyright (C) 2009 Laurent Pinchart <laurent.pinchart@skynet.be>
 *
 * This file holds USB constants and structures defined by the USB Device
 * Class Definition for Video Devices. Unless otherwise stated, comments
 * below reference relevant sections of the USB Video Class 1.1 specification
 * available at
 *
 * http://www.usb.org/developers/devclass_docs/USB_Video_Class_1_1.zip
 */

#ifndef __AV_COMP_DESCRIPTOR_H__
#define __AV_COMP_DESCRIPTOR_H__

#include <stdint.h>
#include "rtos_types.h"
#include "usbd_core.h"
#include "usbd_video.h"

/* --------------------------------------------------------------------------
 * UVC constants
 */

/* A.2. Video Interface Subclass Codes */
#define UVC_SC_UNDEFINED				0x00
#define UVC_SC_VIDEOCONTROL				0x01
#define UVC_SC_VIDEOSTREAMING				0x02
#define UVC_SC_VIDEO_INTERFACE_COLLECTION		0x03

/* A.3. Video Interface Protocol Codes */
#define UVC_PC_PROTOCOL_UNDEFINED			0x00
#define UVC_PC_PROTOCOL_15				0x01

/* A.5. Video Class-Specific VC Interface Descriptor Subtypes */
#define UVC_VC_DESCRIPTOR_UNDEFINED			0x00
#define UVC_VC_HEADER					0x01
#define UVC_VC_INPUT_TERMINAL				0x02
#define UVC_VC_OUTPUT_TERMINAL				0x03
#define UVC_VC_SELECTOR_UNIT				0x04
#define UVC_VC_PROCESSING_UNIT				0x05
#define UVC_VC_EXTENSION_UNIT				0x06

/* A.6. Video Class-Specific VS Interface Descriptor Subtypes */
#define UVC_VS_UNDEFINED				0x00
#define UVC_VS_INPUT_HEADER				0x01
#define UVC_VS_OUTPUT_HEADER				0x02
#define UVC_VS_STILL_IMAGE_FRAME			0x03
#define UVC_VS_FORMAT_UNCOMPRESSED			0x04
#define UVC_VS_FRAME_UNCOMPRESSED			0x05
#define UVC_VS_FORMAT_MJPEG				0x06
#define UVC_VS_FRAME_MJPEG				0x07
#define UVC_VS_FORMAT_MPEG2TS				0x0a
#define UVC_VS_FORMAT_DV				0x0c
#define UVC_VS_COLORFORMAT				0x0d
#define UVC_VS_FORMAT_FRAME_BASED			0x10
#define UVC_VS_FRAME_FRAME_BASED			0x11
#define UVC_VS_FORMAT_STREAM_BASED			0x12

/* A.7. Video Class-Specific Endpoint Descriptor Subtypes */
#define UVC_EP_UNDEFINED				0x00
#define UVC_EP_GENERAL					0x01
#define UVC_EP_ENDPOINT					0x02
#define UVC_EP_INTERRUPT				0x03

/* A.8. Video Class-Specific Request Codes */
#define UVC_RC_UNDEFINED				0x00
#define UVC_SET_CUR					0x01
#define UVC_GET_CUR					0x81
#define UVC_GET_MIN					0x82
#define UVC_GET_MAX					0x83
#define UVC_GET_RES					0x84
#define UVC_GET_LEN					0x85
#define UVC_GET_INFO					0x86
#define UVC_GET_DEF					0x87

/* A.9.1. VideoControl Interface Control Selectors */
#define UVC_VC_CONTROL_UNDEFINED			0x00
#define UVC_VC_VIDEO_POWER_MODE_CONTROL			0x01
#define UVC_VC_REQUEST_ERROR_CODE_CONTROL		0x02

/* A.9.2. Terminal Control Selectors */
#define UVC_TE_CONTROL_UNDEFINED			0x00

/* A.9.3. Selector Unit Control Selectors */
#define UVC_SU_CONTROL_UNDEFINED			0x00
#define UVC_SU_INPUT_SELECT_CONTROL			0x01

/* A.9.4. Camera Terminal Control Selectors */
#define UVC_CT_CONTROL_UNDEFINED			0x00
#define UVC_CT_SCANNING_MODE_CONTROL			0x01
#define UVC_CT_AE_MODE_CONTROL				0x02
#define UVC_CT_AE_PRIORITY_CONTROL			0x03
#define UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL		0x04
#define UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL		0x05
#define UVC_CT_FOCUS_ABSOLUTE_CONTROL			0x06
#define UVC_CT_FOCUS_RELATIVE_CONTROL			0x07
#define UVC_CT_FOCUS_AUTO_CONTROL			0x08
#define UVC_CT_IRIS_ABSOLUTE_CONTROL			0x09
#define UVC_CT_IRIS_RELATIVE_CONTROL			0x0a
#define UVC_CT_ZOOM_ABSOLUTE_CONTROL			0x0b
#define UVC_CT_ZOOM_RELATIVE_CONTROL			0x0c
#define UVC_CT_PANTILT_ABSOLUTE_CONTROL			0x0d
#define UVC_CT_PANTILT_RELATIVE_CONTROL			0x0e
#define UVC_CT_ROLL_ABSOLUTE_CONTROL			0x0f
#define UVC_CT_ROLL_RELATIVE_CONTROL			0x10
#define UVC_CT_PRIVACY_CONTROL				0x11

/* A.9.5. Processing Unit Control Selectors */
#define UVC_PU_CONTROL_UNDEFINED			0x00
#define UVC_PU_BACKLIGHT_COMPENSATION_CONTROL		0x01
#define UVC_PU_BRIGHTNESS_CONTROL			0x02
#define UVC_PU_CONTRAST_CONTROL				0x03
#define UVC_PU_GAIN_CONTROL				0x04
#define UVC_PU_POWER_LINE_FREQUENCY_CONTROL		0x05
#define UVC_PU_HUE_CONTROL				0x06
#define UVC_PU_SATURATION_CONTROL			0x07
#define UVC_PU_SHARPNESS_CONTROL			0x08
#define UVC_PU_GAMMA_CONTROL				0x09
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL	0x0a
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL	0x0b
#define UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL		0x0c
#define UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL	0x0d
#define UVC_PU_DIGITAL_MULTIPLIER_CONTROL		0x0e
#define UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL		0x0f
#define UVC_PU_HUE_AUTO_CONTROL				0x10
#define UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL		0x11
#define UVC_PU_ANALOG_LOCK_STATUS_CONTROL		0x12

/* A.9.7. VideoStreaming Interface Control Selectors */
#define UVC_VS_CONTROL_UNDEFINED			0x00
#define UVC_VS_PROBE_CONTROL				0x01
#define UVC_VS_COMMIT_CONTROL				0x02
#define UVC_VS_STILL_PROBE_CONTROL			0x03
#define UVC_VS_STILL_COMMIT_CONTROL			0x04
#define UVC_VS_STILL_IMAGE_TRIGGER_CONTROL		0x05
#define UVC_VS_STREAM_ERROR_CODE_CONTROL		0x06
#define UVC_VS_GENERATE_KEY_FRAME_CONTROL		0x07
#define UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL		0x08
#define UVC_VS_SYNC_DELAY_CONTROL			0x09

/* B.1. USB Terminal Types */
#define UVC_TT_VENDOR_SPECIFIC				0x0100
#define UVC_TT_STREAMING				0x0101

/* B.2. Input Terminal Types */
#define UVC_ITT_VENDOR_SPECIFIC				0x0200
#define UVC_ITT_CAMERA					0x0201
#define UVC_ITT_MEDIA_TRANSPORT_INPUT			0x0202

/* B.3. Output Terminal Types */
#define UVC_OTT_VENDOR_SPECIFIC				0x0300
#define UVC_OTT_DISPLAY					0x0301
#define UVC_OTT_MEDIA_TRANSPORT_OUTPUT			0x0302

/* B.4. External Terminal Types */
#define UVC_EXTERNAL_VENDOR_SPECIFIC			0x0400
#define UVC_COMPOSITE_CONNECTOR				0x0401
#define UVC_SVIDEO_CONNECTOR				0x0402
#define UVC_COMPONENT_CONNECTOR				0x0403

/* 2.4.2.2. Status Packet Type */
#define UVC_STATUS_TYPE_CONTROL				1
#define UVC_STATUS_TYPE_STREAMING			2

/* 2.4.3.3. Payload Header Information */
#define UVC_STREAM_EOH					(1 << 7)
#define UVC_STREAM_ERR					(1 << 6)
#define UVC_STREAM_STI					(1 << 5)
#define UVC_STREAM_RES					(1 << 4)
#define UVC_STREAM_SCR					(1 << 3)
#define UVC_STREAM_PTS					(1 << 2)
#define UVC_STREAM_EOF					(1 << 1)
#define UVC_STREAM_FID					(1 << 0)

/* 4.1.2. Control Capabilities */
#define UVC_CONTROL_CAP_GET				(1 << 0)
#define UVC_CONTROL_CAP_SET				(1 << 1)
#define UVC_CONTROL_CAP_DISABLED			(1 << 2)
#define UVC_CONTROL_CAP_AUTOUPDATE			(1 << 3)
#define UVC_CONTROL_CAP_ASYNCHRONOUS			(1 << 4)

/* ------------------------------------------------------------------------
 * UVC structures
 */

/* All UVC descriptors have these 3 fields at the beginning */
struct uvc_descriptor_header {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
} __attribute__((packed));

/*-------------------------------------------------------------------------*/

/* USB_DT_INTERFACE_ASSOCIATION: groups interfaces */
struct usb_interface_assoc_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;

	__u8  bFirstInterface;
	__u8  bInterfaceCount;
	__u8  bFunctionClass;
	__u8  bFunctionSubClass;
	__u8  bFunctionProtocol;
	__u8  iFunction;
} __attribute__ ((packed));

#define USB_DT_INTERFACE_ASSOCIATION_SIZE	8


/* 3.7.2. Video Control Interface Header Descriptor */
struct uvc_header_descriptor {
	__u8   bLength;
	__u8   bDescriptorType;
	__u8   bDescriptorSubType;
	__le16 bcdUVC;
	__le16 wTotalLength;
	__le32 dwClockFrequency;
	__u8   bInCollection;
	__u8   baInterfaceNr[];
} __attribute__((__packed__));

#define UVC_DT_HEADER_SIZE(n)				(12+(n))

#define UVC_HEADER_DESCRIPTOR(n) \
	uvc_header_descriptor_##n

#define DECLARE_UVC_HEADER_DESCRIPTOR(n)		\
struct UVC_HEADER_DESCRIPTOR(n) {			\
	__u8   bLength;					\
	__u8   bDescriptorType;				\
	__u8   bDescriptorSubType;			\
	__le16 bcdUVC;					\
	__le16 wTotalLength;				\
	__le32 dwClockFrequency;			\
	__u8   bInCollection;				\
	__u8   baInterfaceNr[n];			\
} __attribute__ ((packed))

/* 3.7.2.1. Input Terminal Descriptor */
struct uvc_input_terminal_descriptor {
	__u8   bLength;
	__u8   bDescriptorType;
	__u8   bDescriptorSubType;
	__u8   bTerminalID;
	__le16 wTerminalType;
	__u8   bAssocTerminal;
	__u8   iTerminal;
} __attribute__((__packed__));

#define UVC_DT_INPUT_TERMINAL_SIZE			8

/* 3.7.2.2. Output Terminal Descriptor */
struct uvc_output_terminal_descriptor {
	__u8   bLength;
	__u8   bDescriptorType;
	__u8   bDescriptorSubType;
	__u8   bTerminalID;
	__le16 wTerminalType;
	__u8   bAssocTerminal;
	__u8   bSourceID;
	__u8   iTerminal;
} __attribute__((__packed__));

#define UVC_DT_OUTPUT_TERMINAL_SIZE			9

/* 3.7.2.3. Camera Terminal Descriptor */
struct uvc_camera_terminal_descriptor {
	__u8   bLength;
	__u8   bDescriptorType;
	__u8   bDescriptorSubType;
	__u8   bTerminalID;
	__le16 wTerminalType;
	__u8   bAssocTerminal;
	__u8   iTerminal;
	__le16 wObjectiveFocalLengthMin;
	__le16 wObjectiveFocalLengthMax;
	__le16 wOcularFocalLength;
	__u8   bControlSize;
	__u8   bmControls[3];
} __attribute__((__packed__));

#define UVC_DT_CAMERA_TERMINAL_SIZE(n)			(15+(n))

/* 3.7.2.4. Selector Unit Descriptor */
struct uvc_selector_unit_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bUnitID;
	__u8  bNrInPins;
	__u8  baSourceID[0];
	__u8  iSelector;
} __attribute__((__packed__));

#define UVC_DT_SELECTOR_UNIT_SIZE(n)			(6+(n))

#define UVC_SELECTOR_UNIT_DESCRIPTOR(n)	\
	uvc_selector_unit_descriptor_##n

#define DECLARE_UVC_SELECTOR_UNIT_DESCRIPTOR(n)	\
struct UVC_SELECTOR_UNIT_DESCRIPTOR(n) {		\
	__u8  bLength;					\
	__u8  bDescriptorType;				\
	__u8  bDescriptorSubType;			\
	__u8  bUnitID;					\
	__u8  bNrInPins;				\
	__u8  baSourceID[n];				\
	__u8  iSelector;				\
} __attribute__ ((packed))

/* 3.7.2.5. Processing Unit Descriptor */
struct uvc_processing_unit_descriptor {
	__u8   bLength;
	__u8   bDescriptorType;
	__u8   bDescriptorSubType;
	__u8   bUnitID;
	__u8   bSourceID;
	__le16 wMaxMultiplier;
	__u8   bControlSize;
	__u8   bmControls[2];
	__u8   iProcessing;
	__u8   bmVideoStandards;
} __attribute__((__packed__));

#define UVC_DT_PROCESSING_UNIT_SIZE(n)			(10+(n))

/* 3.7.2.6. Extension Unit Descriptor */
struct uvc_extension_unit_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bUnitID;
	__u8  guidExtensionCode[16];
	__u8  bNumControls;
	__u8  bNrInPins;
	__u8  baSourceID[0];
	__u8  bControlSize;
	__u8  bmControls[0];
	__u8  iExtension;
} __attribute__((__packed__));

#define UVC_DT_EXTENSION_UNIT_SIZE(p, n)		(24+(p)+(n))

#define UVC_EXTENSION_UNIT_DESCRIPTOR(p, n) \
	uvc_extension_unit_descriptor_##p_##n

#define DECLARE_UVC_EXTENSION_UNIT_DESCRIPTOR(p, n)	\
struct UVC_EXTENSION_UNIT_DESCRIPTOR(p, n) {		\
	__u8  bLength;					\
	__u8  bDescriptorType;				\
	__u8  bDescriptorSubType;			\
	__u8  bUnitID;					\
	__u8  guidExtensionCode[16];			\
	__u8  bNumControls;				\
	__u8  bNrInPins;				\
	__u8  baSourceID[p];				\
	__u8  bControlSize;				\
	__u8  bmControls[n];				\
	__u8  iExtension;				\
} __attribute__ ((packed))

/* 3.8.2.2. Video Control Interrupt Endpoint Descriptor */
struct uvc_control_endpoint_descriptor {
	__u8   bLength;
	__u8   bDescriptorType;
	__u8   bDescriptorSubType;
	__le16 wMaxTransferSize;
} __attribute__((__packed__));

#define UVC_DT_CONTROL_ENDPOINT_SIZE			5

/* 3.9.2.1. Input Header Descriptor */
struct uvc_input_header_descriptor {
	__u8   bLength;
	__u8   bDescriptorType;
	__u8   bDescriptorSubType;
	__u8   bNumFormats;
	__le16 wTotalLength;
	__u8   bEndpointAddress;
	__u8   bmInfo;
	__u8   bTerminalLink;
	__u8   bStillCaptureMethod;
	__u8   bTriggerSupport;
	__u8   bTriggerUsage;
	__u8   bControlSize;
	__u8   bmaControls[];
} __attribute__((__packed__));

#define UVC_DT_INPUT_HEADER_SIZE(n, p)			(13+(n*p))

#define UVC_INPUT_HEADER_DESCRIPTOR(n, p) \
	uvc_input_header_descriptor_##n_##p

#define DECLARE_UVC_INPUT_HEADER_DESCRIPTOR(n, p)	\
struct UVC_INPUT_HEADER_DESCRIPTOR(n, p) {		\
	__u8   bLength;					\
	__u8   bDescriptorType;				\
	__u8   bDescriptorSubType;			\
	__u8   bNumFormats;				\
	__le16 wTotalLength;				\
	__u8   bEndpointAddress;			\
	__u8   bmInfo;					\
	__u8   bTerminalLink;				\
	__u8   bStillCaptureMethod;			\
	__u8   bTriggerSupport;				\
	__u8   bTriggerUsage;				\
	__u8   bControlSize;				\
	__u8   bmaControls[p][n];			\
} __attribute__ ((packed))

/* 3.9.2.2. Output Header Descriptor */
struct uvc_output_header_descriptor {
	__u8   bLength;
	__u8   bDescriptorType;
	__u8   bDescriptorSubType;
	__u8   bNumFormats;
	__le16 wTotalLength;
	__u8   bEndpointAddress;
	__u8   bTerminalLink;
	__u8   bControlSize;
	__u8   bmaControls[];
} __attribute__((__packed__));

#define UVC_DT_OUTPUT_HEADER_SIZE(n, p)			(9+(n*p))

#define UVC_OUTPUT_HEADER_DESCRIPTOR(n, p) \
	uvc_output_header_descriptor_##n_##p

#define DECLARE_UVC_OUTPUT_HEADER_DESCRIPTOR(n, p)	\
struct UVC_OUTPUT_HEADER_DESCRIPTOR(n, p) {		\
	__u8   bLength;					\
	__u8   bDescriptorType;				\
	__u8   bDescriptorSubType;			\
	__u8   bNumFormats;				\
	__le16 wTotalLength;				\
	__u8   bEndpointAddress;			\
	__u8   bTerminalLink;				\
	__u8   bControlSize;				\
	__u8   bmaControls[p][n];			\
} __attribute__ ((packed))

/* 3.9.2.6. Color matching descriptor */
struct uvc_color_matching_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bColorPrimaries;
	__u8  bTransferCharacteristics;
	__u8  bMatrixCoefficients;
} __attribute__((__packed__));

#define UVC_DT_COLOR_MATCHING_SIZE			6

/* 4.3.1.1. Video Probe and Commit Controls */
struct uvc_streaming_control {
	__u16 bmHint;
	__u8  bFormatIndex;
	__u8  bFrameIndex;
	__u32 dwFrameInterval;
	__u16 wKeyFrameRate;
	__u16 wPFrameRate;
	__u16 wCompQuality;
	__u16 wCompWindowSize;
	__u16 wDelay;
	__u32 dwMaxVideoFrameSize;
	__u32 dwMaxPayloadTransferSize;
	__u32 dwClockFrequency;
	__u8  bmFramingInfo;
	__u8  bPreferedVersion;
	__u8  bMinVersion;
	__u8  bMaxVersion;
} __attribute__((__packed__));

struct uvc_format_descriptor_header {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bFormatIndex;
	__u8  bNumFrameDescriptors;
} __attribute__((packed));

/* Uncompressed Payload - 3.1.1. Uncompressed Video Format Descriptor */
struct uvc_format_uncompressed {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bFormatIndex;
	__u8  bNumFrameDescriptors;
	__u8  guidFormat[16];
	__u8  bBitsPerPixel;
	__u8  bDefaultFrameIndex;
	__u8  bAspectRatioX;
	__u8  bAspectRatioY;
	__u8  bmInterfaceFlags;
	__u8  bCopyProtect;
} __attribute__((__packed__));

#define UVC_DT_FORMAT_UNCOMPRESSED_SIZE			27

/* Uncompressed Payload - 3.1.2. Uncompressed Video Frame Descriptor */
struct uvc_frame_uncompressed {
	__u8   bLength;
	__u8   bDescriptorType;
	__u8   bDescriptorSubType;
	__u8   bFrameIndex;
	__u8   bmCapabilities;
	__le16 wWidth;
	__le16 wHeight;
	__le32 dwMinBitRate;
	__le32 dwMaxBitRate;
	__le32 dwMaxVideoFrameBufferSize;
	__le32 dwDefaultFrameInterval;
	__u8   bFrameIntervalType;
	__le32 dwFrameInterval[];
} __attribute__((__packed__));

#define UVC_DT_FRAME_UNCOMPRESSED_SIZE(n)		(26+4*(n))

#define UVC_FRAME_UNCOMPRESSED(n) \
	uvc_frame_uncompressed_##n

#define DECLARE_UVC_FRAME_UNCOMPRESSED(n)		\
struct UVC_FRAME_UNCOMPRESSED(n) {			\
	__u8   bLength;					\
	__u8   bDescriptorType;				\
	__u8   bDescriptorSubType;			\
	__u8   bFrameIndex;				\
	__u8   bmCapabilities;				\
	__le16 wWidth;					\
	__le16 wHeight;					\
	__le32 dwMinBitRate;				\
	__le32 dwMaxBitRate;				\
	__le32 dwMaxVideoFrameBufferSize;		\
	__le32 dwDefaultFrameInterval;			\
	__u8   bFrameIntervalType;			\
	__le32 dwFrameInterval[n];			\
} __attribute__ ((packed))

/* MJPEG Payload - 3.1.1. MJPEG Video Format Descriptor */
struct uvc_format_mjpeg {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bFormatIndex;
	__u8  bNumFrameDescriptors;
	__u8  bmFlags;
	__u8  bDefaultFrameIndex;
	__u8  bAspectRatioX;
	__u8  bAspectRatioY;
	__u8  bmInterfaceFlags;
	__u8  bCopyProtect;
} __attribute__((__packed__));

#define UVC_DT_FORMAT_MJPEG_SIZE			11

/* MJPEG Payload - 3.1.2. MJPEG Video Frame Descriptor */
struct uvc_frame_mjpeg {
	__u8   bLength;
	__u8   bDescriptorType;
	__u8   bDescriptorSubType;
	__u8   bFrameIndex;
	__u8   bmCapabilities;
	__le16 wWidth;
	__le16 wHeight;
	__le32 dwMinBitRate;
	__le32 dwMaxBitRate;
	__le32 dwMaxVideoFrameBufferSize;
	__le32 dwDefaultFrameInterval;
	__u8   bFrameIntervalType;
	__le32 dwFrameInterval[];
} __attribute__((__packed__));

#define UVC_DT_FRAME_MJPEG_SIZE(n)			(26+4*(n))

#define UVC_FRAME_MJPEG(n) \
	uvc_frame_mjpeg_##n

#define DECLARE_UVC_FRAME_MJPEG(n)			\
struct UVC_FRAME_MJPEG(n) {				\
	__u8   bLength;					\
	__u8   bDescriptorType;				\
	__u8   bDescriptorSubType;			\
	__u8   bFrameIndex;				\
	__u8   bmCapabilities;				\
	__le16 wWidth;					\
	__le16 wHeight;					\
	__le32 dwMinBitRate;				\
	__le32 dwMaxBitRate;				\
	__le32 dwMaxVideoFrameBufferSize;		\
	__le32 dwDefaultFrameInterval;			\
	__u8   bFrameIntervalType;			\
	__le32 dwFrameInterval[n];			\
} __attribute__ ((packed))

/* Frame Based Payload - 3.1.1. Frame Based Video Format Descriptor */
struct uvc_format_framebased {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bFormatIndex;
	__u8  bNumFrameDescriptors;
	__u8  guidFormat[16];
	__u8  bBitsPerPixel;
	__u8  bDefaultFrameIndex;
	__u8  bAspectRatioX;
	__u8  bAspectRatioY;
	__u8  bmInterfaceFlags;
	__u8  bCopyProtect;
	__u8  bVariableSize;
} __attribute__((__packed__));

#define UVC_DT_FORMAT_FRAMEBASED_SIZE			28

/* Frame Based Payload - 3.1.2. Frame Based Video Frame Descriptor */
struct uvc_frame_framebased {
	__u8  bLength;
	__u8  bDescriptorType;
	__u8  bDescriptorSubType;
	__u8  bFrameIndex;
	__u8  bmCapabilities;
	__u16 wWidth;
	__u16 wHeight;
	__u32 dwMinBitRate;
	__u32 dwMaxBitRate;
	__u32 dwDefaultFrameInterval;
	__u8  bFrameIntervalType;
	__u32 dwBytesPerLine;
	__u32 dwFrameInterval[];
} __attribute__((__packed__));

#define UVC_DT_FRAME_FRAMEBASED_SIZE(n)                        (26+4*(n))

#define UVC_FRAME_FRAMEBASED(n) \
	uvc_frame_framebased_##n

#define DECLARE_UVC_FRAME_FRAMEBASED(n)		\
struct UVC_FRAME_FRAMEBASED(n) {		\
	__u8  bLength;				\
	__u8  bDescriptorType;			\
	__u8  bDescriptorSubType;		\
	__u8  bFrameIndex;			\
	__u8  bmCapabilities;			\
	__u16 wWidth;				\
	__u16 wHeight;				\
	__u32 dwMinBitRate;			\
	__u32 dwMaxBitRate;			\
	__u32 dwDefaultFrameInterval;		\
	__u8  bFrameIntervalType;		\
	__u32 dwBytesPerLine;			\
	__u32 dwFrameInterval[n];		\
} __attribute__ ((packed))

/*-------------------------------------------------------------------------*/

/* CONTROL REQUEST SUPPORT */

/*
 * USB directions
 *
 * This bit flag is used in endpoint descriptors' bEndpointAddress field.
 * It's also one of three fields in control requests bRequestType.
 */
#define USB_DIR_OUT			0		/* to device */
#define USB_DIR_IN			0x80		/* to host */
/*
 * USB types, the second of three bRequestType fields
 */
#define USB_TYPE_MASK			(0x03 << 5)
#define USB_TYPE_STANDARD		(0x00 << 5)
#define USB_TYPE_CLASS			(0x01 << 5)
#define USB_TYPE_VENDOR			(0x02 << 5)
#define USB_TYPE_RESERVED		(0x03 << 5)

/*
 * USB recipients, the third of three bRequestType fields
 */
#define USB_RECIP_MASK			0x1f
#define USB_RECIP_DEVICE		0x00
#define USB_RECIP_INTERFACE		0x01
#define USB_RECIP_ENDPOINT		0x02
#define USB_RECIP_OTHER			0x03
/* From Wireless USB 1.0 */
#define USB_RECIP_PORT			0x04
#define USB_RECIP_RPIPE		0x05

/*
 * Standard requests, for the bRequest field of a SETUP packet.
 *
 * These are qualified by the bRequestType field, so that for example
 * TYPE_CLASS or TYPE_VENDOR specific feature flags could be retrieved
 * by a GET_STATUS request.
 */
#define USB_REQ_GET_STATUS		0x00
#define USB_REQ_CLEAR_FEATURE		0x01
#define USB_REQ_SET_FEATURE		0x03
#define USB_REQ_SET_ADDRESS		0x05
#define USB_REQ_GET_DESCRIPTOR		0x06
#define USB_REQ_SET_DESCRIPTOR		0x07
#define USB_REQ_GET_CONFIGURATION	0x08
#define USB_REQ_SET_CONFIGURATION	0x09
#define USB_REQ_GET_INTERFACE		0x0A
#define USB_REQ_SET_INTERFACE		0x0B
#define USB_REQ_SYNCH_FRAME		0x0C
#define USB_REQ_SET_SEL			0x30
#define USB_REQ_SET_ISOCH_DELAY		0x31

#define USB_REQ_SET_ENCRYPTION		0x0D	/* Wireless USB */
#define USB_REQ_GET_ENCRYPTION		0x0E
#define USB_REQ_RPIPE_ABORT		0x0E
#define USB_REQ_SET_HANDSHAKE		0x0F
#define USB_REQ_RPIPE_RESET		0x0F
#define USB_REQ_GET_HANDSHAKE		0x10
#define USB_REQ_SET_CONNECTION		0x11
#define USB_REQ_SET_SECURITY_DATA	0x12
#define USB_REQ_GET_SECURITY_DATA	0x13
#define USB_REQ_SET_WUSB_DATA		0x14
#define USB_REQ_LOOPBACK_DATA_WRITE	0x15
#define USB_REQ_LOOPBACK_DATA_READ	0x16
#define USB_REQ_SET_INTERFACE_DS	0x17

/* specific requests for USB Power Delivery */
#define USB_REQ_GET_PARTNER_PDO		20
#define USB_REQ_GET_BATTERY_STATUS	21
#define USB_REQ_SET_PDO			22
#define USB_REQ_GET_VDM			23
#define USB_REQ_SEND_VDM		24

/* The Link Power Management (LPM) ECN defines USB_REQ_TEST_AND_SET command,
 * used by hubs to put ports into a new L1 suspend state, except that it
 * forgot to define its number ...
 */

/*
 * USB feature flags are written using USB_REQ_{CLEAR,SET}_FEATURE, and
 * are read as a bit array returned by USB_REQ_GET_STATUS.  (So there
 * are at most sixteen features of each type.)  Hubs may also support a
 * new USB_REQ_TEST_AND_SET_FEATURE to put ports into L1 suspend.
 */
#define USB_DEVICE_SELF_POWERED		0	/* (read only) */
#define USB_DEVICE_REMOTE_WAKEUP	1	/* dev may initiate wakeup */
#define USB_DEVICE_TEST_MODE		2	/* (wired high speed only) */
#define USB_DEVICE_BATTERY		2	/* (wireless) */
#define USB_DEVICE_B_HNP_ENABLE		3	/* (otg) dev may initiate HNP */
#define USB_DEVICE_WUSB_DEVICE		3	/* (wireless)*/
#define USB_DEVICE_A_HNP_SUPPORT	4	/* (otg) RH port supports HNP */
#define USB_DEVICE_A_ALT_HNP_SUPPORT	5	/* (otg) other RH port does */
#define USB_DEVICE_DEBUG_MODE		6	/* (special devices only) */

/*
 * Test Mode Selectors
 * See USB 2.0 spec Table 9-7
 */
#define	USB_TEST_J		1
#define	USB_TEST_K		2
#define	USB_TEST_SE0_NAK	3
#define	USB_TEST_PACKET		4
#define	USB_TEST_FORCE_ENABLE	5

/* Status Type */
#define USB_STATUS_TYPE_STANDARD	0
#define USB_STATUS_TYPE_PTM		1

/*
 * New Feature Selectors as added by USB 3.0
 * See USB 3.0 spec Table 9-7
 */
#define USB_DEVICE_U1_ENABLE	48	/* dev may initiate U1 transition */
#define USB_DEVICE_U2_ENABLE	49	/* dev may initiate U2 transition */
#define USB_DEVICE_LTM_ENABLE	50	/* dev may send LTM */
#define USB_INTRF_FUNC_SUSPEND	0	/* function suspend */

#define USB_INTR_FUNC_SUSPEND_OPT_MASK	0xFF00
/*
 * Suspend Options, Table 9-8 USB 3.0 spec
 */
#define USB_INTRF_FUNC_SUSPEND_LP	(1 << (8 + 0))
#define USB_INTRF_FUNC_SUSPEND_RW	(1 << (8 + 1))

/*
 * Interface status, Figure 9-5 USB 3.0 spec
 */
#define USB_INTRF_STAT_FUNC_RW_CAP     1
#define USB_INTRF_STAT_FUNC_RW         2

#define USB_ENDPOINT_HALT		0	/* IN/OUT will STALL */

/* Bit array elements as returned by the USB_REQ_GET_STATUS request. */
#define USB_DEV_STAT_U1_ENABLED		2	/* transition into U1 state */
#define USB_DEV_STAT_U2_ENABLED		3	/* transition into U2 state */
#define USB_DEV_STAT_LTM_ENABLED	4	/* Latency tolerance messages */

/*
 * Feature selectors from Table 9-8 USB Power Delivery spec
 */
#define USB_DEVICE_BATTERY_WAKE_MASK	40
#define USB_DEVICE_OS_IS_PD_AWARE	41
#define USB_DEVICE_POLICY_MODE		42
#define USB_PORT_PR_SWAP		43
#define USB_PORT_GOTO_MIN		44
#define USB_PORT_RETURN_POWER		45
#define USB_PORT_ACCEPT_PD_REQUEST	46
#define USB_PORT_REJECT_PD_REQUEST	47
#define USB_PORT_PORT_PD_RESET		48
#define USB_PORT_C_PORT_PD_CHANGE	49
#define USB_PORT_CABLE_PD_RESET		50
#define USB_DEVICE_CHARGING_POLICY	54

/**
 * struct usb_ctrlrequest - SETUP data for a USB device control request
 * @bRequestType: matches the USB bmRequestType field
 * @bRequest: matches the USB bRequest field
 * @wValue: matches the USB wValue field (le16 byte order)
 * @wIndex: matches the USB wIndex field (le16 byte order)
 * @wLength: matches the USB wLength field (le16 byte order)
 *
 * This structure is used to send control requests to a USB device.  It matches
 * the different fields of the USB 2.0 Spec section 9.3, table 9-2.  See the
 * USB spec for a fuller description of the different fields, and what they are
 * used for.
 *
 * Note that the driver for any interface can issue control requests.
 * For most devices, interfaces don't coordinate with each other, so
 * such requests may be made at any time.
 */
struct usb_ctrlrequest {
	__u8 bRequestType;
	__u8 bRequest;
	__le16 wValue;
	__le16 wIndex;
	__le16 wLength;
} __attribute__ ((packed));

/*-------------------------------------------------------------------------*/

/*
 * STANDARD DESCRIPTORS ... as returned by GET_DESCRIPTOR, or
 * (rarely) accepted by SET_DESCRIPTOR.
 *
 * Note that all multi-byte values here are encoded in little endian
 * byte order "on the wire".  Within the kernel and when exposed
 * through the Linux-USB APIs, they are not converted to cpu byte
 * order; it is the responsibility of the client code to do this.
 * The single exception is when device and configuration descriptors (but
 * not other descriptors) are read from character devices
 * (i.e. /dev/bus/usb/BBB/DDD);
 * in this case the fields are converted to host endianness by the kernel.
 */

/*
 * Descriptor types ... USB 2.0 spec table 9.5
 */
#define USB_DT_DEVICE			0x01
#define USB_DT_CONFIG			0x02
#define USB_DT_STRING			0x03
#define USB_DT_INTERFACE		0x04
#define USB_DT_ENDPOINT			0x05
#define USB_DT_DEVICE_QUALIFIER		0x06
#define USB_DT_OTHER_SPEED_CONFIG	0x07
#define USB_DT_INTERFACE_POWER		0x08
/* these are from a minor usb 2.0 revision (ECN) */
#define USB_DT_OTG			0x09
#define USB_DT_DEBUG			0x0a
#define USB_DT_INTERFACE_ASSOCIATION	0x0b
/* these are from the Wireless USB spec */
#define USB_DT_SECURITY			0x0c
#define USB_DT_KEY			0x0d
#define USB_DT_ENCRYPTION_TYPE		0x0e
#define USB_DT_BOS			0x0f
#define USB_DT_DEVICE_CAPABILITY	0x10
#define USB_DT_WIRELESS_ENDPOINT_COMP	0x11
#define USB_DT_WIRE_ADAPTER		0x21
#define USB_DT_RPIPE			0x22
#define USB_DT_CS_RADIO_CONTROL		0x23
/* From the T10 UAS specification */
#define USB_DT_PIPE_USAGE		0x24
/* From the USB 3.0 spec */
#define	USB_DT_SS_ENDPOINT_COMP		0x30
/* From the USB 3.1 spec */
#define	USB_DT_SSP_ISOC_ENDPOINT_COMP	0x31

/* Conventional codes for class-specific descriptors.  The convention is
 * defined in the USB "Common Class" Spec (3.11).  Individual class specs
 * are authoritative for their usage, not the "common class" writeup.
 */
#define USB_DT_CS_DEVICE		(USB_TYPE_CLASS | USB_DT_DEVICE)
#define USB_DT_CS_CONFIG		(USB_TYPE_CLASS | USB_DT_CONFIG)
#define USB_DT_CS_STRING		(USB_TYPE_CLASS | USB_DT_STRING)
#define USB_DT_CS_INTERFACE		(USB_TYPE_CLASS | USB_DT_INTERFACE)
#define USB_DT_CS_ENDPOINT		(USB_TYPE_CLASS | USB_DT_ENDPOINT)

/* All standard descriptors have these 2 fields at the beginning */
struct usb_descriptor_header {
	__u8  bLength;
	__u8  bDescriptorType;
} __attribute__ ((packed));

/*-------------------------------------------------------------------------*/

// /* USB_DT_DEVICE: Device descriptor */
// struct usb_device_descriptor {
// 	__u8  bLength;
// 	__u8  bDescriptorType;

// 	__le16 bcdUSB;
// 	__u8  bDeviceClass;
// 	__u8  bDeviceSubClass;
// 	__u8  bDeviceProtocol;
// 	__u8  bMaxPacketSize0;
// 	__le16 idVendor;
// 	__le16 idProduct;
// 	__le16 bcdDevice;
// 	__u8  iManufacturer;
// 	__u8  iProduct;
// 	__u8  iSerialNumber;
// 	__u8  bNumConfigurations;
// } __attribute__ ((packed));

#define USB_DT_DEVICE_SIZE		18


/*
 * Device and/or Interface Class codes
 * as found in bDeviceClass or bInterfaceClass
 * and defined by www.usb.org documents
 */
#define USB_CLASS_PER_INTERFACE		0	/* for DeviceClass */
#define USB_CLASS_AUDIO			1
#define USB_CLASS_COMM			2
#define USB_CLASS_HID			3
#define USB_CLASS_PHYSICAL		5
#define USB_CLASS_STILL_IMAGE		6
#define USB_CLASS_PRINTER		7
#define USB_CLASS_MASS_STORAGE		8
#define USB_CLASS_HUB			9
#define USB_CLASS_CDC_DATA		0x0a
#define USB_CLASS_CSCID			0x0b	/* chip+ smart card */
#define USB_CLASS_CONTENT_SEC		0x0d	/* content security */
#define USB_CLASS_VIDEO			0x0e
#define USB_CLASS_WIRELESS_CONTROLLER	0xe0
#define USB_CLASS_PERSONAL_HEALTHCARE	0x0f
#define USB_CLASS_AUDIO_VIDEO		0x10
#define USB_CLASS_BILLBOARD		0x11
#define USB_CLASS_USB_TYPE_C_BRIDGE	0x12
#define USB_CLASS_MISC			0xef
#define USB_CLASS_APP_SPEC		0xfe
#define USB_CLASS_VENDOR_SPEC		0xff

#define USB_SUBCLASS_VENDOR_SPEC	0xff

/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/

/* USB_DT_CONFIG: Configuration descriptor information.
 *
 * USB_DT_OTHER_SPEED_CONFIG is the same descriptor, except that the
 * descriptor type is different.  Highspeed-capable devices can look
 * different depending on what speed they're currently running.  Only
 * devices with a USB_DT_DEVICE_QUALIFIER have any OTHER_SPEED_CONFIG
 * descriptors.
 */
struct usb_config_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;

	__le16 wTotalLength;
	__u8  bNumInterfaces;
	__u8  bConfigurationValue;
	__u8  iConfiguration;
	__u8  bmAttributes;
	__u8  bMaxPower;
} __attribute__ ((packed));

#define USB_DT_CONFIG_SIZE		9

/* from config descriptor bmAttributes */
#define USB_CONFIG_ATT_ONE		(1 << 7)	/* must be set */
#define USB_CONFIG_ATT_SELFPOWER	(1 << 6)	/* self powered */
#define USB_CONFIG_ATT_WAKEUP		(1 << 5)	/* can wakeup */
#define USB_CONFIG_ATT_BATTERY		(1 << 4)	/* battery powered */

/*-------------------------------------------------------------------------*/

/* USB String descriptors can contain at most 126 characters. */
#define USB_MAX_STRING_LEN	126

// /* USB_DT_STRING: String descriptor */
// struct usb_string_descriptor {
// 	__u8  bLength;
// 	__u8  bDescriptorType;

// 	__le16 wData[1];		/* UTF-16LE encoded */
// } __attribute__ ((packed));

/* note that "string" zero is special, it holds language codes that
 * the device supports, not Unicode characters.
 */

#define UVC_STRING_DESCRIPTOR_SIZE(n)           (2+2*(n))

#define UVC_STRING_DESCRIPTOR(n) \
	uvc_string_descriptor_##n

#define DECLARE_UVC_STRING_DESCRIPTOR(n) \
struct UVC_STRING_DESCRIPTOR(n) {       \
    __u8  bLength;                      \
    __u8  bDescriptorType;              \
    __le16 wData[n];                    \
} __attribute__ ((packed)) 

/*-------------------------------------------------------------------------*/

// /* USB_DT_INTERFACE: Interface descriptor */
// struct usb_interface_descriptor {
// 	__u8  bLength;
// 	__u8  bDescriptorType;

// 	__u8  bInterfaceNumber;
// 	__u8  bAlternateSetting;
// 	__u8  bNumEndpoints;
// 	__u8  bInterfaceClass;
// 	__u8  bInterfaceSubClass;
// 	__u8  bInterfaceProtocol;
// 	__u8  iInterface;
// } __attribute__ ((packed));

#define USB_DT_INTERFACE_SIZE		9

/*-------------------------------------------------------------------------*/



/* USB_DT_ENDPOINT: Endpoint descriptor */
// struct usb_endpoint_descriptor {
// 	__u8  bLength;
// 	__u8  bDescriptorType;

// 	__u8  bEndpointAddress;
// 	__u8  bmAttributes;
// 	__le16 wMaxPacketSize;
// 	__u8  bInterval;

// 	/* NOTE:  these two are _only_ in audio endpoints. */
// 	/* use USB_DT_ENDPOINT*_SIZE in bLength, not sizeof. */
// 	__u8  bRefresh;
// 	__u8  bSynchAddress;
// } __attribute__ ((packed));

#define USB_DT_ENDPOINT_SIZE		7
#define USB_DT_ENDPOINT_AUDIO_SIZE	9	/* Audio extension */


/*
 * Endpoints
 */
#define USB_ENDPOINT_NUMBER_MASK	0x0f	/* in bEndpointAddress */
#define USB_ENDPOINT_DIR_MASK		0x80

#define USB_ENDPOINT_XFERTYPE_MASK	0x03	/* in bmAttributes */
#define USB_ENDPOINT_XFER_CONTROL	0
#define USB_ENDPOINT_XFER_ISOC		1
#define USB_ENDPOINT_XFER_BULK		2
#define USB_ENDPOINT_XFER_INT		3
// #define USB_ENDPOINT_MAX_ADJUSTABLE	0x80

#define USB_ENDPOINT_MAXP_MASK	0x07ff
#define USB_EP_MAXP_MULT_SHIFT	11
#define USB_EP_MAXP_MULT_MASK	(3 << USB_EP_MAXP_MULT_SHIFT)
#define USB_EP_MAXP_MULT(m) \
	(((m) & USB_EP_MAXP_MULT_MASK) >> USB_EP_MAXP_MULT_SHIFT)

/* The USB 3.0 spec redefines bits 5:4 of bmAttributes as interrupt ep type. */
#define USB_ENDPOINT_INTRTYPE		0x30
#define USB_ENDPOINT_INTR_PERIODIC	(0 << 4)
#define USB_ENDPOINT_INTR_NOTIFICATION	(1 << 4)

#define USB_ENDPOINT_SYNCTYPE		0x0c
#define USB_ENDPOINT_SYNC_NONE		(0 << 2)
#define USB_ENDPOINT_SYNC_ASYNC		(1 << 2)
// #define USB_ENDPOINT_SYNC_ADAPTIVE	(2 << 2)
#define USB_ENDPOINT_SYNC_SYNC		(3 << 2)

// #define USB_ENDPOINT_USAGE_MASK		0x30
// #define USB_ENDPOINT_USAGE_DATA		0x00
// #define USB_ENDPOINT_USAGE_FEEDBACK	0x10
#define USB_ENDPOINT_USAGE_IMPLICIT_FB	0x20	/* Implicit feedback Data endpoint */

/*-------------------------------------------------------------------------*/

/* USB_DT_DEVICE_QUALIFIER: Device Qualifier descriptor */
struct usb_qualifier_descriptor {
	__u8  bLength;
	__u8  bDescriptorType;

	__le16 bcdUSB;
	__u8  bDeviceClass;
	__u8  bDeviceSubClass;
	__u8  bDeviceProtocol;
	__u8  bMaxPacketSize0;
	__u8  bNumConfigurations;
	__u8  bRESERVED;
} __attribute__ ((packed));

#define USB_DT_QUALIFIER_SIZE       10

/*-------------------------------------------------------------------------*/

#define MAX_PAYLOAD_SIZE  2048 // for high speed with three transcations every one micro frame
#define VIDEO_PACKET_SIZE (unsigned int)(((MAX_PAYLOAD_SIZE / 2)) | (0x01 << 11))

// Frame interval in 100 ns units.
#define FRAME_INTERVAL_FPS(N)   (1000 * 1000 * 10 / N)

/*-------------------------------------------------------------------------*/

typedef enum _UVC_FORMAT_E {
    UVC_FORMAT_MJPEG = 0,
    UVC_FORMAT_H264,
    UVC_FORMAT_YUY2,
    UVC_FORMAT_NV21
} UVC_FORMAT_E;

struct uvc_frame_info_st
{
    uint32_t frame_index;
    uint32_t width;
    uint32_t height;
    uint32_t fps;
    uint32_t rotate_flag;   //0:0, 1:90, 2:180, 3:270
} __attribute__((__packed__));


struct uvc_format_info_st
{
    uint32_t format_index;
    UVC_FORMAT_E format_type;
    uint32_t default_frame_index;
    uint32_t frame_cnt;
    struct uvc_frame_info_st *frames;
} __attribute__((__packed__));


void uvc_set_video_format_info(const struct uvc_format_info_st *video_format_info);
void uvc_get_video_format_info(struct uvc_format_info_st *video_format_info);
void uvc_set_video_frame_info(const struct uvc_frame_info_st *video_frame_info);
void uvc_get_video_frame_info(struct uvc_frame_info_st *video_frame_info);

uint8_t *av_comp_build_descriptors(struct uvc_format_info_st *format_info, uint32_t num_formats);
void av_comp_destroy_descriptors(uint8_t *descriptors);

/*-------------------------------------------------------------------------*/

#endif /* __LINUX_USB_VIDEO_H */

