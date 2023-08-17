/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#ifndef TM_VPM_PARAMS_H
#define TM_VPM_PARAMS_H

#include <string>

#include <tmedia_core/common/common_inc.h>

using namespace std;

class TMVpmParams
{
public:
    uint32_t                 mCanvasWidth;
    uint32_t                 mCanvasHeight;
    TMImageInfo::PixelFormat mCanvasPixelFormat;

    TMVpmParams()  {Reset();}
    ~TMVpmParams() {}

    void Reset() {
        mCanvasWidth = 0;
        mCanvasHeight = 0;
        mCanvasPixelFormat = TMImageInfo::PixelFormat::PIXEL_FORMAT_UNKNOW;
    }

    void Dump()  {}
};

#endif  /* TM_VPM_PARAMS_H */
