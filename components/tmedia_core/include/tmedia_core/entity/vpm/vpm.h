/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifndef TM_VPM_H
#define TM_VPM_H

#include <string>
#include <mutex>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/entity.h>
#include <tmedia_core/entity/vpm/vpm_params.h>

using namespace std;

/*
 *  VPM: Video Process Matrix
 *    Input  channels including: Camera, Video decoder, OSD overlay, etc
 *    Output channels including: Video output, Video Encoder, Interpreter, etc
 *
 *                              @---------------- [VPM] ----------------@
 *    ________                  |________                               |
 *   |        |     SendFrame() |        |                              |
 *   |in Frame|    -----------> |InChn[i]|                     _________|               ___________
 *   |________|                 |________|  ________________  |         |  RecvFrame() |           |
 *                              |          |                | |OutChn[x]| -----------> | out frame |
 *   ____________               |________  |                | |_________|              |           |
 *  |            |  SendFrame() |        | |                |           |              |___________|
 *  |  in frame  | -----------> |InChn[j]| |  Canvas Frame  |           |
 *  |            |              |________| |                |           |
 *  |____________|              |          |                |  _________|               ___________
 *                              |________  |________________| |         |  RecvFrame() |           |
 *    ________                  |        |                    |OutChn[y]| -----------> | out frame |
 *   |in Frame|     endFrame()  |InChn[k]|                    |_________|              |___________|
 *   |________|    -----------> |________|                              |
 *                              |                                       |
 *                              @---------------------------------------@
 *
 *  VPM processes:
 *    1. Input channels process: Crop -> Scale -> Rotate/Mirror/Flip;
 *    2. Composite from input channels with indicated positions and target pixel format;
 *    3. Output channels process: scale -> pixel format convert;
 */
#define TMVPM_MAX_FPS 60

class TMVpmInputChannel;
class TMVpmOutputChannel;

typedef int (*TMVpmInputDataConsumedCallback)(TMVideoFrame *);

class TMVpm : public TMFilterEntity
{
public:
    enum class PropID : uint32_t
    {
        // PropID            Data Type & Value range
        FRC_ENABLE,       // bool, frame rate control enable. Default is false.
                          //    true : when timer up, blend all input channels to canvas and
                          //           do post-process from canvas and push to output channels
                          //    false: Input:  blend to canvas when send from any input channel,
                          //           Output: do post-process from canvas
        FRC_BLEND_FPS,    // uint32_t, default is 25. unit fps.
        CANVAS_BG_COLOR,  // uint32_t, default is black. In RGB24 format no matter what pixel format is.
    };

    enum class ColorKey : uint32_t
    {
        BLACK = 0x000000, // Y=0x10, U=0x80, V=0x80
        WHITE = 0xFFFFFF, // Y=0xEB, U=0x80, V=0x80
        PINK  = 0xFF00FF, // Y=0x6A, U=0xCA, V=0xDE
        CYAN  = 0x00FFFF, // Y=0xAA, U=0xA6, V=0x10
        PURPLE= 0x800080, // Y=0x3D, U=0xA5, V=0xAF
    };

    TMVpm()  {InitDefaultPropertyList();}
    virtual ~TMVpm() {};

    // TMVpm interface
    virtual int Open(TMVpmParams &param, TMPropertyList *propList = NULL) = 0;
    virtual int SetConfig(TMPropertyList &propList) = 0;
    virtual int GetConfig(TMPropertyList &propList) = 0;
    virtual int Start() = 0;
    virtual int Flush() = 0;
    virtual int Stop()  = 0;
    virtual int Close() = 0;

    // Input channel, TMPropertyList refer to: TMVpmInputChannel::PropID
    virtual int CreateInputChannel(TMVpmInputChannel  **inChn, TMPropertyList *propList = NULL) = 0;
    virtual int ConfigInputChannel(TMVpmInputChannel   *inChn, TMPropertyList &propList) = 0;
    virtual int DestoryInputChannel(TMVpmInputChannel **inChn) = 0;
    virtual int DestoryInputChannels() = 0;

    // Output channel
    virtual int CreateOutputChannel(TMVpmOutputChannel  **outChn, TMPropertyList *propList = NULL) = 0;
    virtual int ConfigOutputChannel(TMVpmOutputChannel   *outChn, TMPropertyList &propList) = 0;
    virtual int DestoryOutputChannel(TMVpmOutputChannel **outChn) = 0;
    virtual int DestoryOutputChannels() = 0;

    // Data operations
    // when user not call RegisterDataConsumedCallback, VPM will UnRef frame automatically
    virtual int RegisterDataConsumedCallback(TMVpmInputChannel *inChn,
                                             TMVpmInputDataConsumedCallback callback) = 0;
    virtual int SendFrame(TMVpmInputChannel *inChn, TMVideoFrame &frame, int timeout = -1) = 0;
    virtual int RecvFrame(TMVpmOutputChannel *outChn, TMVideoFrame &frame, int timeout = -1) = 0;

    TMVpmParams mParams;
    TMPropertyList mDefaultPropertyList;
    TMPropertyList mCurrentPropertyList;

protected:
    list<TMVpmInputChannel *>  mInChnList;
    list<TMVpmOutputChannel *> mOutChnList;


    virtual void InitDefaultPropertyList()
    {
        TMPropertyList *pList;
        for (int loop = 0; loop < 2; loop++)
        {
            pList = (loop == 0) ? &mDefaultPropertyList : &mCurrentPropertyList;
            pList->Add(TMProperty((int)PropID::FRC_ENABLE, false, "frc enable"));
            pList->Add(TMProperty((int)PropID::FRC_BLEND_FPS, (uint32_t)25, "frc blend fps"));
            pList->Add(TMProperty((int)PropID::CANVAS_BG_COLOR,
                                  (uint32_t)ColorKey::BLACK, "canvas bg color"));
        }
    }
};

class TMVpmInputChannel final
{
public:
    enum class PropID : uint32_t
    {
        // PropID                  Data Type & Value range
        // source input frame info (only for input channel)
        SRC_WIDTH,              // uint32_t, Origin input width. default 0.
        SRC_HEIGHT,             // uint32_t, Origin input height.default 0
        SRC_PIXEL_FORMAT,       // TMImageInfo::PixelFormat, input pixel format. default UNKNOWN

        // blend to canvas positon
        CANVAS_POS_X,           // uint32_t, 0~VPM's canvas width
        CANVAS_POS_Y,           // uint32_t, 0~VPM's canvas height

        // blend color key
        COLOR_KEY_ENABLE,       // bool,    enable color key to transparent to frame. default is false
        COLOR_KEY_VALUE,        // uint32_t color key in RGB8888 format(prefer TMVideoBlende::ColorKey). default is BLACK

        // crop property
        CROP_ENABLE,            // bool,     crop from origin frame, default is disabled
        CROP_RECT_X,            // uint32_t, 0 ~ Origin picture width-1.  default 0.
        CROP_RECT_Y,            // uint32_t, 0 ~ Origin picture height-1. default 0.
        CROP_RECT_W,            // uint32_t, 0 ~ Origin picture width.    default -1 means input's width
        CROP_RECT_H,            // uint32_t, 0 ~ Origin picture height.   default -1 menas input's height

        // scale property
        SCALE_ENABLE,           // bool,     scale from origin frame, default is disabled
        SCALE_RECT_W,           // uint32_t, 0 ~ Origin picture width
        SCALE_RECT_H,           // uint32_t, 0 ~ Origin picture height

        // rotate property
        ROTATE_ENABLE,          // bool,     srotate from origin frame, default is disabled
        ROTATE_ANGLE,           // uint32_t, degree, 90/180/270

        // mirror property
        MIRROR_ENABLE,          // bool,    left-right mirror from origin frame, default is disabled

        // flip property
        FLIP_ENABLE,            // bool,    up-down flip origin frame, default is disabled
    };

public:
    TMVpmInputChannel()
    {
        mVpm = NULL;
        mOpaque = NULL;
        mHoldingFrameIsReady = false;
        mConsumedCallback = DefaultInputDataConsumedCallback;
        InitDefaultPropertyList();
    }
    ~TMVpmInputChannel()
    {
        if (mHoldingFrameIsReady && mConsumedCallback != NULL)
        {
            mConsumedCallback(&mHoldingFrame);
            mHoldingFrameIsReady = false;
        }
    }
    // input channel keeps last video frame for compositing until channel destoried

    TMVpm *mVpm;
    TMPropertyList mDefaultPropertyList;
    TMPropertyList mCurrentPropertyList;
    void *mOpaque;  // to stall platform private data

    TMVpmInputDataConsumedCallback mConsumedCallback;
    TMVideoFrame mHoldingFrame;
    bool         mHoldingFrameIsReady;
    std::mutex   mHoldingFrameMutex;   // to protect mHoldingFrame

protected:
    static int DefaultInputDataConsumedCallback(TMVideoFrame *frame)
    {
        LOG_D("VideoFrame(mPTS=%" PRIu64 ") is consumed.\n", frame->mPTS.Get().timestamp);
        return frame->UnRef();
    }

    void InitDefaultPropertyList()
    {
        TMPropertyList *pList;
        for (int loop = 0; loop < 2; loop++)
        {
            pList = (loop == 0) ? &mDefaultPropertyList : &mCurrentPropertyList;
            pList->Add(TMProperty((int)PropID::SRC_WIDTH,        0,     "src width"));
            pList->Add(TMProperty((int)PropID::SRC_HEIGHT,       0,     "src height"));
            pList->Add(TMProperty((int)PropID::SRC_PIXEL_FORMAT,
                                  (int)(TMImageInfo::PixelFormat::PIXEL_FORMAT_UNKNOW),
                                                                        "src pixel format"));

            pList->Add(TMProperty((int)PropID::CANVAS_POS_X,     0,     "canvas_pos_x"));
            pList->Add(TMProperty((int)PropID::CANVAS_POS_Y,     0,     "canvas_pos_y"));

            pList->Add(TMProperty((int)PropID::COLOR_KEY_ENABLE, false, "color key enable"));
            pList->Add(TMProperty((int)PropID::COLOR_KEY_VALUE,
                                  (uint32_t)TMVpm::ColorKey::BLACK,
                                                                        "color key value"));

            pList->Add(TMProperty((int)PropID::CROP_ENABLE,      false, "crop enable"));
            pList->Add(TMProperty((int)PropID::CROP_RECT_X,      0,     "crop_x"));
            pList->Add(TMProperty((int)PropID::CROP_RECT_Y,      0,     "crop_y"));
            pList->Add(TMProperty((int)PropID::CROP_RECT_W,      0,     "crop_w"));
            pList->Add(TMProperty((int)PropID::CROP_RECT_H,      0,     "crop_h"));

            pList->Add(TMProperty((int)PropID::SCALE_ENABLE,     false, "scale enable"));
            pList->Add(TMProperty((int)PropID::SCALE_RECT_W,     0,     "scale_w"));
            pList->Add(TMProperty((int)PropID::SCALE_RECT_H,     0,     "scale_h"));

            pList->Add(TMProperty((int)PropID::ROTATE_ENABLE,    false, "rotate enable"));
            pList->Add(TMProperty((int)PropID::ROTATE_ANGLE,     0,     "rotate angle"));

            pList->Add(TMProperty((int)PropID::MIRROR_ENABLE,    false, "mirror enable"));
            pList->Add(TMProperty((int)PropID::FLIP_ENABLE,      false, "mirror enable"));
        }
    }
};

class TMVpmOutputChannel final
{
public:
    enum class PropID : uint32_t
    {
        // PropID                  Data Type & Value range
        // output video frame
        POOL_INIT_COUNT,        // int32_t,  <=0 means use platform default count(prefer >=2)

        DEST_WIDTH,             // uint32_t, output frame width.  default 0, same as Canvas.
        DEST_HEIGHT,            // uint32_t, output frame height. default 0, same as Canvas.
        DEST_PIXEL_FORMAT,      // TMImageInfo::PixelFormat, output pixel format. default UNKNOWN.
        DEST_BG_COLOR,          // uint32_t, default is black. value should according to pixel format.

        // crop property
        CROP_ENABLE,            // bool,     crop from origin frame. default is disabled
        CROP_RECT_X,            // uint32_t, 0 ~ Origin picture width-1.  default 0.
        CROP_RECT_Y,            // uint32_t, 0 ~ Origin picture height-1. default 0.
        CROP_RECT_W,            // uint32_t, 0 ~ Origin picture width.    default -1 means canvas' width(maybe cut off).
        CROP_RECT_H,            // uint32_t, 0 ~ Origin picture height.   default -1 means canvas' height(maybe cut off).

        // scale property
        SCALE_ENABLE,           // bool,     scale from origin frame, default is disabled
        SCALE_RECT_W,           // uint32_t, 0 ~ Origin picture width.  default -1 means output dest width
        SCALE_RECT_H,           // uint32_t, 0 ~ Origin picture height. default -1 means output dest height

        // rotate property
        ROTATE_ENABLE,          // bool,     rotate from origin frame, default is disabled
        ROTATE_ANGLE,           // uint32_t, degree, 90/180/270.  default 0 means no rotate.

        // mirror property
        MIRROR_ENABLE,          // bool,     left-right mirror from origin frame. default is disabled

        // flip property
        FLIP_ENABLE,            // bool,     up-down flip origin frame. default is disabled

        // blit to dest output video frame position
        DEST_POS_X,             // uint32_t, dest video frame's x position. default is 0.
        DEST_POS_Y,             // uint32_t, dest video frame's y position. default is 0.
    };

public:
    TMVpmOutputChannel()
    {
        mVpm = NULL;
        mOpaque = NULL;
        InitDefaultPropertyList();
    }
    ~TMVpmOutputChannel() {}
    // TODO: attach OSD on output channel in next version

    TMVpm *mVpm;
    void *mOpaque;  // to stall platform private data
    TMPropertyList mDefaultPropertyList;
    TMPropertyList mCurrentPropertyList;

protected:
    void InitDefaultPropertyList()
    {
        TMPropertyList *pList;
        for (int loop = 0; loop < 2; loop++)
        {
            pList = (loop == 0) ? &mDefaultPropertyList : &mCurrentPropertyList;

            pList->Add(TMProperty((int)PropID::POOL_INIT_COUNT, -1,  "pool frame count"));

            pList->Add(TMProperty((int)PropID::DEST_WIDTH,    0,     "dest width"));
            pList->Add(TMProperty((int)PropID::DEST_HEIGHT,   0,     "dest height"));
            pList->Add(TMProperty((int)PropID::DEST_PIXEL_FORMAT,
                (int)(TMImageInfo::PixelFormat::PIXEL_FORMAT_UNKNOW),"pixel format"));

            pList->Add(TMProperty((int)PropID::DEST_BG_COLOR, 0,     "dest gb color"));

            pList->Add(TMProperty((int)PropID::CROP_ENABLE,   false, "crop enable"));
            pList->Add(TMProperty((int)PropID::CROP_RECT_X,   0,     "crop_x"));
            pList->Add(TMProperty((int)PropID::CROP_RECT_Y,   0,     "crop_y"));
            pList->Add(TMProperty((int)PropID::CROP_RECT_W,   -1,    "crop_w"));
            pList->Add(TMProperty((int)PropID::CROP_RECT_H,   -1,    "crop_h"));

            pList->Add(TMProperty((int)PropID::SCALE_ENABLE,  false, "scale enable"));
            pList->Add(TMProperty((int)PropID::SCALE_RECT_W,  0,     "scale_w"));
            pList->Add(TMProperty((int)PropID::SCALE_RECT_H,  0,     "scale_h"));

            pList->Add(TMProperty((int)PropID::ROTATE_ENABLE, false, "rotate enable"));
            pList->Add(TMProperty((int)PropID::ROTATE_ANGLE,  0,     "rotate angle"));

            pList->Add(TMProperty((int)PropID::MIRROR_ENABLE, false, "mirror enable"));
            pList->Add(TMProperty((int)PropID::FLIP_ENABLE,   false, "mirror enable"));

            pList->Add(TMProperty((int)PropID::DEST_POS_X,    0,     "dest_pos_x"));
            pList->Add(TMProperty((int)PropID::DEST_POS_Y,    0,     "dest_pos_y"));
        }
    }
};

#endif  /* TM_VPM_H */
