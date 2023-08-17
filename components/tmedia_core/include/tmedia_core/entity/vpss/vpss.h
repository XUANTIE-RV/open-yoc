/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_VPSS_H
#define TM_VPSS_H

#include <string>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/vpss/vpss_params.h>
#include <tmedia_core/entity/vpss/vpss_property.h>
#include <tmedia_core/entity/entity.h>

using namespace std;
using namespace vpss_prop;

class TMVpss : public TMFilterEntity
{
public:
    enum class PropID : uint32_t
    {
        // ID                      Data Type & Value range
        CROP_ENABLE,            // bool, To crop new frame from origin frame
        CROP_COORDINATE,        // vpss_prop::CropCoordinate_e
        CROP_RECT_X,            // int32_t 0 ~ Origin picture width-1
        CROP_RECT_Y,            // int32_t 0 ~ Origin picture width-1
        CROP_RECT_W,            // int32_t 0 ~ Origin picture width
        CROP_RECT_H,            // int32_t 0 ~ Origin picture width

        FRC_ENABLE,             // bool, frame rate control. false=free run
        ROTATE_ENABLE,          // bool, rotate from origin frame
        SCALE_ENABLE,           // bool, scale from origin frame
        COVER_ENABLE,           // bool, cover colored region on origin frame
        OVERLAY_ENABLE,         // bool, add overlay and blend out.
        FORMAT_CONVERT_ENABLE,  // bool, convert pixel format from origin frame
        DATA_LINE_MODE,         // vpss_prop::DataLineMode_e, Data Online/Offline/Parallel mode

        OUTPUT_WIDTH,
        OUTPUT_HEIGHT,
        OUTPUT_PIXEL_FORMAT,

        START_FRAME_NUM,        // int32_t >= 0, the number of frames to skip when start
    };

    TMVpssParams mParams;

    TMVpss();
    virtual ~TMVpss();

    // TMFilterEntity interface
    virtual TMSrcPad *GetSrcPad(int padID = 0) = 0;
    virtual TMSinkPad *GetSinkPad(int padID = 0) = 0;

    // TMVpss interface
    virtual int Open(TMVpssParams &param, TMPropertyList *propList = NULL) = 0;
    virtual int SetConfig(TMPropertyList &propList) = 0;
    virtual int GetConfig(TMPropertyList &propList) = 0;
    virtual int Start() = 0;
    virtual int Flush() = 0;
    virtual int Stop()  = 0;
    virtual int Close() = 0;

    virtual int SendFrame(TMVideoFrame &frame, int timeout) = 0;
    virtual int RecvFrame(TMVideoFrame &frame, int timeout) = 0;
};

#endif  /* TM_VPSS_H */
