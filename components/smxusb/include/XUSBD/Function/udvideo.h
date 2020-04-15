/*
* udvideo.h                                                 Version 2.51
*
* smxUSBD Video Function Driver. Based on UVC 1.0.
*
* Copyright (c) 2011 Micro Digital Inc.
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

#ifndef SUD_VIDEO_H
#define SUD_VIDEO_H

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/* notification */
#define SUD_VIDEO_NOTIFY_ISOCDATAREADY          1 /* Received ISOC data, for video stream */
#define SUD_VIDEO_NOTIFY_IN_START_STOP          2
#define SUD_VIDEO_NOTIFY_OUT_START_STOP         3

#define SUD_VIDEO_NOTIFY_INIT_DEF_SETTINGS      10
#define SUD_VIDEO_NOTIFY_INIT_MIN_SETTINGS      11
#define SUD_VIDEO_NOTIFY_INIT_MAX_SETTINGS      12
#define SUD_VIDEO_NOTIFY_INIT_INFO_SETTINGS     13
#define SUD_VIDEO_NOTIFY_SET_SETTINGS           14
#define SUD_VIDEO_NOTIFY_SET_PROBE_FORMAT       15
#define SUD_VIDEO_NOTIFY_SET_STILL_FORMAT       16

#define SUD_VIDEO_FORMAT_UNCOMPRESSED           1
#define SUD_VIDEO_FORMAT_MJPEG                  2
#define SUD_VIDEO_FORMAT_MPEG2TS                3

typedef void (* SUD_PVIDEOFUNC)(int port, int notification, u32 parameter);

typedef struct {
    uint iFormat;
    u16  wWidth;
    u16  wHeight;
    u32  dwFrameInterval;
    u32  dwMaxVideoFrameSize;
    u32  dwMaxPayloadTransferSize;
} SUD_VIDEO_FORMAT;

typedef struct {
    u8  bDevicePowerMode;
    /* Camera Control */
    u8  bScanningMode;
    u8  bAutoExposureMode;
    u8  bAutoExposurePriority;
    u32 dwExposureTimeAbsolute;
    u8  bExposureTimeRelative;
    u16 wFocusAbsolute;
    u8  bFocusRelative;
    u8  bFocusRelativeSpeed;
    u8  bFocusAuto;
    u16 wIrisAbsolute;
    u8  bIrisRelative;
    u16 wObjectiveFocalLength;
    s8  bZoom;
    u8  bDigitalZoom;
    u8  bZoomSpeed;
    s32 dwPanAbsolute;
    s32 dwTiltAbsolute;
    s8  bPanRelative;
    u8  bPanSpeed;
    s8  bTiltRelative;
    u8  bTiltSpeed;
    u16 wRollAbsolute;
    u8  bRollRelative;
    u8  bRollRelativeSpeed;
    u8  bPrivacy;
    /* Select Unit Control */
    u8 bSelector;
    /* Processing Unit Control */
    u16 wBacklightCompensation;
    s16 wBrightness;
    u16 wContrast;
    u16 wGain;
    u8  bPowerLineFrequency;
    s16 wHue;
    u8  bHueAuto;
    u16 wSaturation;
    u16 wSharpness;
    u16 wGamma;
    u16 wWhiteBalanceTemperature;
    u8  bWhiteBalanceTemperatureAuto;
    u16 wWhiteBalanceBlue;
    u16 wWhiteBalanceRed;
    u8  bWhiteBalanceComponentAuto;
    u16 wMultiplierStep;
    u16 wMultiplierLimit;
    u8  bVideoStandard;
    u8  bAnalogVideoLockStatus;
    u16 wSyncDelay;
} SUD_VIDEO_SETTINGS;

typedef struct {
    /* Camera Control */
    u32 dwExposureTimeAbsolute;
    u16 wFocusAbsolute;
    u8  bFocusRelative;
    u8  bFocusRelativeSpeed;
    u16 wIrisAbsolute;
    u16 wObjectiveFocalLength;
    s8  bZoom;
    u8  bDigitalZoom;
    u8  bZoomSpeed;
    s32 dwPanAbsolute;
    s32 dwTiltAbsolute;
    s8  bPanRelative;
    u8  bPanSpeed;
    s8  bTiltRelative;
    u8  bTiltSpeed;
    u16 wRollAbsolute;
    u8  bRollRelative;
    u8  bRollRelativeSpeed;
    /* Processing Unit Control */
    u16 wBacklightCompensation;
    s16 wBrightness;
    u16 wContrast;
    u16 wGain;
    s16 wHue;
    u16 wSaturation;
    u16 wSharpness;
    u16 wGamma;
    u16 wWhiteBalanceTemperature;
    u16 wWhiteBalanceBlue;
    u16 wWhiteBalanceRed;
    u16 wMultiplierStep;
    u16 wMultiplierLimit;
    u16 wSyncDelay;
} SUD_VIDEO_MIN_MAX;

typedef struct {
    u8  bDevicePowerMode;
    u8  bRequestErrorCode;
    /* Camera Control */
    u8  bScanningMode;
    u8  bAutoExposureMode;
    u8  bAutoExposurePriority;
    u8  dwExposureTimeAbsolute;
    u8  bExposureTimeRelative;
    u8  wFocusAbsolute;
    u8  bFocusRelative;
    u8  bFocusRelativeSpeed;
    u8  bFocusAuto;
    u8  wIrisAbsolute;
    u8  bIrisRelative;
    u8  wObjectiveFocalLength;
    u8  bZoom;
    u8  bDigitalZoom;
    u8  bZoomSpeed;
    u8  dwPanAbsolute;
    u8  dwTiltAbsolute;
    u8  bPanRelative;
    u8  bPanSpeed;
    u8  bTiltRelative;
    u8  bTiltSpeed;
    u8  wRollAbsolute;
    u8  bRollRelative;
    u8  bRollRelativeSpeed;
    u8  bPrivacy;
    /* Select Unit Control */
    u8  bSelector;
    /* Processing Unit Control */
    u8  wBacklightCompensation;
    u8  wBrightness;
    u8  wContrast;
    u8  wGain;
    u8  bPowerLineFrequency;
    u8  wHue;
    u8  bHueAuto;
    u8  wSaturation;
    u8  wSharpness;
    u8  wGamma;
    u8  wWhiteBalanceTemperature;
    u8  bWhiteBalanceTemperatureAuto;
    u8  wWhiteBalanceBlue;
    u8  wWhiteBalanceRed;
    u8  bWhiteBalanceComponentAuto;
    u8  wMultiplierStep;
    u8  wMultiplierLimit;
    u8  bVideoStandard;
    u8  bAnalogVideoLockStatus;
    u8  wSyncDelay;
    u8  ProbeCommit;
} SUD_VIDEO_INFO;

/* used by smxUSBD internally */
int  sud_VideoInit(void);
void sud_VideoRelease(void);

#if SUD_COMPOSITE
/* for composite device only */
void *sud_VideoGetOps(void);
void *sud_VideoGetInterface(void);
#endif

int  sud_VideoIsConnected(int port);

/* send Video stream data */
int  sud_VideoSendVideoData(int port, u8 *pData, int iLen);
/* Get the received video stream data */
int  sud_VideoGetVideoData(int port, u8 *pData, int iLen);

/* register callback function */
void sud_VideoRegisterNotify(int port, SUD_PVIDEOFUNC handler);

#ifdef __cplusplus
}
#endif

#endif  /* SUD_VIDEO_H */

