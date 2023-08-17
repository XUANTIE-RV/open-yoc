/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#ifndef TM_FORMAT_CAMERA_H
#define TM_FORMAT_CAMERA_H

#include <string>
#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/format/format_demuxer.h>
#include <tmedia_core/entity/format/format_info.h>
#include <tmedia_core/entity/format/format_io.h>

/*
 * Camera logical parts are below, including: input sensor and output channel
 *  @------------------------------@
 *  |    Camera Settings      _____|______   Channel Settings
 *  |  ===================   |            | =====================
 *  |      Exposure          | channel[0] | configuration: width/height, pix_fmt
 *  |      R/G/B Gain        |_____ ______| ---> Channel Events
 *  |      HDR mode           _____|______
 *  |        ......          |            |
 *  |                        | channel[1] | configuration: width/height, pix_fmt
 *  |                        |_____ ______| ---> Channel Events
 *  |                         _____|______
 *  |                        |            |
 *  |                        | channel[n] | configuration: width/height, pix_fmt
 *  |                        |_____ ______| ---> Channel Events
 *  |        CAMERA                |
 *  @------------------------------@        ===> Camera Events (sensor, error, warning...)
 */

using namespace std;

typedef struct {
    int count;
    struct {
        int mode_id;    // must support mode_id=0 as default mode
        char description[128];
    } modes[8];
} TMCameraModes_s;

typedef struct {
    int   mode_id;
    char *calibriation; /* set NULL to use default in system  */
    char *lib3a;        /* set NULL to use default in system  */
} TMCameraModeCfg_s;

typedef enum {
    TM_CAMERA_CHANNEL_0 = 0,
    TM_CAMERA_CHANNEL_1,
    TM_CAMERA_CHANNEL_2,
    TM_CAMERA_CHANNEL_3,
    TM_CAMERA_CHANNEL_4,
    TM_CAMERA_CHANNEL_5,
    TM_CAMERA_CHANNEL_6,
    TM_CAMERA_CHANNEL_7,
    TM_CAMERA_CHANNEL_MAX_COUNT
} TMCameraChannelID_e;

typedef enum {
    TMCAMERA_CAPTURE_NONE  = 0,
    TMCAMERA_CAPTURE_VIDEO = 1 << 0,
    TMCAMERA_CAPTURE_AUDIO = 1 << 1,
} TMCameraCaptureType_e;

typedef enum {
    TMCAMERA_MODE_NONE,
    TMCAMERA_COLOR_MODE,
    TMCAMERA_MONO_MODE,
} TMCameraWorkMode_e;

#define CAMERA_NAME_RGB0 "camera_rgb0"
#define CAMERA_NAME_IR0  "camera_ir0"

class TMCamera : public TMVideoInput
{
public:
    enum class PropID : uint32_t
    {
        // Camera property IDs                  Data Type
        CAMERA_HFLIP,                           // bool, false: no horizontal flip; true: horizontal flip
        CAMERA_VFLIP,                           // bool, false: no vertical flip; true: vertical flip
        CAMERA_ROTATE,                          // int, 0: no rotate; 90, 180, 270
        CAMERA_EXPOSURE_MODE,                   // enum, 0: MODE_AUTO; 1: MANUAL; 2: SHUTTER_PRIORITY; 3: APERTURE_PRIORITY
        CAMERA_EXPOSURE_BIAS,                   // int, Exposure Bias, min: -6, max: 6, step: 1, default: 0
        CAMERA_AUTO_PRESET_WHITE_BALANCE,       // enum, Auto White Balance, 0: MANUAL; 1: AUTO; 2: CUSTOM1; 3: CUSTOM2
        CAMERA_RED_GAIN,                        // uint32_t, Red Gain, min: 2^0, max: 2^31, step: 1, default: 256
        CAMERA_GREEN_GAIN,                      // uint32_t, Green Gain, min: 2^0, max: 2^31, step: 1, default: 256
        CAMERA_BLUE_GAIN,                       // uint32_t, Blue Gain, min: 2^0, max: 2^31, step: 1, default: 256
        CAMERA_WORK_MODE,                       // TMCameraWorkMode_e, 0: none; 1: COLOR_MODE; 2: MONO_MODE
        CAMERA_EXPOSURE_ABSOLUTE,               // int, sensor exposure time/100us, min: 0, max: 10000, step: 1, default: 100

        // Channel property IDs
        CHANNEL_CAPTURE_TYPE,           // int, bitmask by TMCameraCaptureType_e
        CHANNEL_OUTPUT_WIDTH,           // int, [0,1920], <0 means use camera origin width
        CHANNEL_OUTPUT_HEIGHT,          // int, [0,1088], <0 means use camera origin height
        CHANNEL_OUTPUT_PIXEL_FORMAT,    // TMImageInfo::PixelFormat
    };

    TMCamera() {InitDefaultPropertyList();}
    virtual ~TMCamera() {}

    // TMSrcEntity interface
    virtual TMSrcPad *GetSrcPad(int padID = 0) = 0;

    // TMVideoInput interface
    virtual int Open(string deviceName, TMPropertyList *propList = NULL) = 0;
    virtual int Close()                                                  = 0;
    virtual int SetConfig(TMPropertyList &propList)                      = 0;
    virtual int GetConfig(TMPropertyList &propList)                      = 0;
    virtual int Start()                                                  = 0;
    virtual int Stop()                                                   = 0;
    virtual int RecvFrame(TMVideoFrame &frame, int timeout)              = 0;

    // TMCamera interface
    virtual int GetModes(TMCameraModes_s &modes)                              = 0;
    virtual int SetMode(TMCameraModeCfg_s &mode)                              = 0;
    virtual int OpenChannel(int chnID, TMPropertyList *propList = NULL)       = 0;
    virtual int CloseChannel(int chnID)                                       = 0;
    virtual int SetChannelConfig(int chnID, TMPropertyList &propList)         = 0;
    virtual int GetChannelConfig(int chnID, TMPropertyList &propList)         = 0;
    virtual int StartChannel(int chnID)                                       = 0;
    virtual int StopChannel(int chnID)                                        = 0;
    virtual int RecvChannelFrame(int chnID, TMVideoFrame &frame, int timeout) = 0;
    virtual int ReleaseChannelFrame(int chnID, TMVideoFrame &frame)           = 0;

    // TODO: Add 3A_Ready event/function

protected:
    TMPropertyList mDefaultCameraPropertyList;
    TMPropertyList mCurrentCameraPropertyList;
    TMPropertyList mDefaultChannelPropertyList[TM_CAMERA_CHANNEL_MAX_COUNT];
    TMPropertyList mCurrentChannelPropertyList[TM_CAMERA_CHANNEL_MAX_COUNT];

    virtual void InitDefaultPropertyList()
    {
        TMPropertyList *pList;
        for (int loop = 0; loop < 2; loop++)
        {
            pList = (loop == 0) ? &mDefaultCameraPropertyList :  &mCurrentCameraPropertyList;
            pList->Add(TMProperty((int)PropID::CAMERA_HFLIP, false, "horizontal flip"));
            pList->Add(TMProperty((int)PropID::CAMERA_VFLIP, false, "vertical flip"));
            pList->Add(TMProperty((int)PropID::CAMERA_ROTATE, 0, "rotate"));
            pList->Add(TMProperty((int)PropID::CAMERA_EXPOSURE_MODE, 0, "exposure mode"));
            pList->Add(TMProperty((int)PropID::CAMERA_EXPOSURE_BIAS, 0, "exposure bias"));
            pList->Add(TMProperty((int)PropID::CAMERA_AUTO_PRESET_WHITE_BALANCE, 0, "auto preset white balance"));
            pList->Add(TMProperty((int)PropID::CAMERA_RED_GAIN, 256, "color red gain"));
            pList->Add(TMProperty((int)PropID::CAMERA_GREEN_GAIN, 256, "color green gain"));
            pList->Add(TMProperty((int)PropID::CAMERA_BLUE_GAIN, 256, "color blue gain"));
            // pList->Add(TMProperty((int)PropID::CAMERA_WORK_MODE, TMCAMERA_COLOR_MODE, "TMCameraWorkMode"));

            for (uint32_t i = 0; i < TM_CAMERA_CHANNEL_MAX_COUNT; i++)
            {
                pList = (loop == 0) ? &mDefaultChannelPropertyList[i] :  &mCurrentChannelPropertyList[i];
                pList->Add(TMProperty((int)PropID::CHANNEL_CAPTURE_TYPE, TMCAMERA_CAPTURE_VIDEO, "channel capture type"));
                pList->Add(TMProperty((int)PropID::CHANNEL_OUTPUT_WIDTH, -1, "channel output width"));
                pList->Add(TMProperty((int)PropID::CHANNEL_OUTPUT_HEIGHT, -1, "channel output height"));
                pList->Add(TMProperty((int)PropID::CHANNEL_OUTPUT_PIXEL_FORMAT, (int32_t)(TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12), "channel output pixel format"));
            }
        }
    }
};

#endif  /* TM_FORMAT_CAMERA_H */
