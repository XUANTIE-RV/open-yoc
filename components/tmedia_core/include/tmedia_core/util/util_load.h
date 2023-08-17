/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_UTIL_LOAD_H
#define TM_UTIL_LOAD_H

#include <string>
#include <tmedia_core/common/common_inc.h>

using namespace std;

class TMUtilLoad
{
public:
    typedef struct
    {
        string fileName;
        int width;
        int height;
        TMImageInfo::PixelFormat pixelFormat;
    } PictureParams_s;

    static int LoadPicture(TMVideoFrame &frame, PictureParams_s &params);
};

#endif  /* TM_UTIL_LOAD_H */
