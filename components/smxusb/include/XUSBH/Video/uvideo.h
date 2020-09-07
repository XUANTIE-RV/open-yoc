/*
* uvideo.h                                                  Version 3.00
*
* smxUSBH Video Class Driver.
*
* Copyright (c) 2013-2018 Micro Digital Inc.
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

#ifndef SU_VIDEO_H
#define SU_VIDEO_H

/*============================================================================
                                   CONSTANTS
============================================================================*/
#define SU_VIDEO_FORMAT_UNCOMPRESSED           0
#define SU_VIDEO_FORMAT_MJPEG                  1
#define SU_VIDEO_FORMAT_MPEG2TS                2

#define SU_VIDEO_FORMAT_UNCOMPRESSED_UNKNOWN   0
#define SU_VIDEO_FORMAT_UNCOMPRESSED_YUV422    1
#define SU_VIDEO_FORMAT_UNCOMPRESSED_YUV420    2

#define SU_VIDEO_MAX_FORMAT                    4
#define SU_VIDEO_MAX_FRAME                     24
#define SU_VIDEO_MAX_FRAME_INTERVAL            16
#define SU_VIDEO_MAX_STILL_IMAGE_PATTERN       24

typedef struct
{
    u16  wWidth[SU_VIDEO_MAX_STILL_IMAGE_PATTERN];
    u16  wHeight[SU_VIDEO_MAX_STILL_IMAGE_PATTERN];
    u8   bColorPrimaries;
    u8   bTransferCharacteristics;
    u8   bMatrixCoefficients;
    uint iPatternNum;
    uint iFormat;
}SU_VIDEO_STILL_IMAGE_FORMAT;

typedef struct
{
    u16  wWidth;
    u16  wHeight;
    u32  dwFrameInterval[SU_VIDEO_MAX_FRAME_INTERVAL];
    uint iNumFrameInterval;
}SU_VIDEO_CAPTURE_FRAME;

typedef struct
{
    uint iFormat;
    uint iYUVFormat;
    u8   bBitsPerPixel;
    u8   bDefaultFrameIndex;
    
    SU_VIDEO_CAPTURE_FRAME Frame[SU_VIDEO_MAX_FRAME];
    uint  bNumFrame;
}SU_VIDEO_CAPTURE_FORMAT;

typedef struct
{
    u8  bDevPowerMode;
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
}SU_VIDEO_SETTINGS;

typedef struct
{
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
}SU_VIDEO_MIN_MAX;

typedef struct
{
    u8  bDevPowerMode;
    u8  bReqErrorCode;
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
}SU_VIDEO_INFO;

/*============================================================================
                          GLOBAL FUNCTION PROTOTYPES
============================================================================*/

#if defined(__cplusplus)
extern "C" {
#endif

int  su_VideoInit(void);
void su_VideoRelease(void);
void su_VideoRegDevEvtCallback(SU_PDEVEVTCBFUNC func);

int  su_VideoCameraGetStillImageFormatNum(uint iID);
int  su_VideoCameraGetStillImageFormat(uint iID, uint iIndex, SU_VIDEO_STILL_IMAGE_FORMAT *pFormat);
int  su_VideoCameraGetCaptureFormatNum(uint iID);
int  su_VideoCameraGetCaptureFormat(uint iID, uint iIndex, SU_VIDEO_CAPTURE_FORMAT *pFormat);
int  su_VideoCameraGetCurrent(uint iID, SU_VIDEO_SETTINGS *pCurrent);
int  su_VideoCameraGetDefault(uint iID, SU_VIDEO_SETTINGS *pDefault);
int  su_VideoCameraGetMin(uint iID, SU_VIDEO_MIN_MAX *pMin);
int  su_VideoCameraGetMax(uint iID, SU_VIDEO_MIN_MAX *pMax);
int  su_VideoCameraGetInfo(uint iID, SU_VIDEO_INFO *pInfo);
int  su_VideoCameraSetCaptureFrame(uint iID, uint iFormat, u16 wWidth, u16 wHeight, u32 dwFrameInterval);
int  su_VideoCameraSetStillImageFormat(uint iID, uint iFormat, u16 wWidth, u16 wHeight);
int  su_VideoCameraSetCurrent(uint iID, SU_VIDEO_SETTINGS *pNewCurrent);
int  su_VideoCameraGetCaptureSize(uint iID, u32 *pMaxVideoFrameSize, u32 *pMaxPayloadBufSize);

int  su_VideoCameraOpen(uint iID);
int  su_VideoCameraClose(uint iID);
int  su_VideoCameraCapture(uint iID, u8 *pData, uint iLen, BOOLEAN *pbIncludeNewFrame, uint *piNewFrameOffset);

BOOLEAN  su_VideoInserted(uint iID);

#if defined(__cplusplus)
}
#endif


/*==========================================================================*/
#endif /* SU_VIDEO_H */
