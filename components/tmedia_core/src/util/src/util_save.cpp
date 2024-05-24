/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <string.h>
#include <unistd.h>
#include <iostream>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/common/syslog.h>
#include <tmedia_core/util/util_save.h>

using namespace std;

int TMUtilSave::PgmPicture(TMVideoFrame &frame, string &filename)
{
    FILE *f;
    int i;

    f = fopen(filename.c_str(), "wb");
    fprintf(f, "P5\n%d %d\n%d\n", frame.mWidth, frame.mHeight, 255);

    for (i = 0; i < frame.mHeight; i++)
        fwrite(frame.mData[0] + i * frame.mStride[0], 1, frame.mWidth, f);
    fclose(f);
    return 0;
}

int TMUtilSave::Picture(TMVideoFrame &frame, const char *filename, bool append)
{
    FILE *f;
    int i;

    if (filename == NULL || strlen(filename) == 0)
        return TMResult::TM_EINVAL;

    const char *openMode = (append && (access(filename, F_OK) == 0)) ? "a+" : "wb";

    f = fopen(filename, openMode);
    if (f == NULL)
    {
        LOG_E("Can't create file '%s' to write\n", filename);
        return TMResult::TM_EACCES;
    }

    int plane_width[TMFRAME_MAX_PLANE] = {0};
    int plane_height[TMFRAME_MAX_PLANE] = {0};

    switch (frame.mPixelFormat)
    {
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P:
        plane_height[0] = frame.mHeight;
        plane_height[1] = frame.mHeight / 2;
        plane_height[2] = frame.mHeight / 2;

        plane_width[0] = frame.mWidth;
        plane_width[1] = frame.mWidth / 2;
        plane_width[2] = frame.mWidth / 2;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12:
        plane_height[0] = frame.mHeight;
        plane_height[1] = frame.mHeight / 2;

        plane_width[0] = frame.mWidth;
        plane_width[1] = frame.mWidth;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV422P:
        plane_height[0] = frame.mHeight;
        plane_height[1] = frame.mHeight;
        plane_height[2] = frame.mHeight;

        plane_width[0] = frame.mWidth;
        plane_width[1] = frame.mWidth / 2;
        plane_width[2] = frame.mWidth / 2;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888:
        plane_height[0] = frame.mHeight;
        plane_width[0] = frame.mWidth * 3;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_ARGB8888:
        plane_height[0] = frame.mHeight;
        plane_width[0] = frame.mWidth * 4;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_NV16:
        plane_height[0] = frame.mHeight;
        plane_height[1] = frame.mHeight;

        plane_width[0] = frame.mWidth;
        plane_width[1] = frame.mWidth;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_YUYV422:
        plane_height[0] = frame.mHeight;
        plane_width[0] = frame.mWidth * 2;
        break;
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888P:
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888P:
        plane_height[0] = frame.mHeight;
        plane_height[1] = frame.mHeight;
        plane_height[2] = frame.mHeight;

        plane_width[0] = frame.mWidth;
        plane_width[1] = frame.mWidth;
        plane_width[2] = frame.mWidth;
        break;
    default:
        LOG_E("frame->mPixelFormat(%s) not support yet", TMImageInfo::Name(frame.mPixelFormat).c_str());
        fclose(f);
        return TMResult::TM_EINVAL;
    }

    for (uint32_t plane = 0; plane < frame.mPlanes; plane++)
    {
        for (i = 0; i < plane_height[plane]; i++)
            fwrite(frame.mData[plane] + i * frame.mStride[plane], 1, plane_width[plane], f);
    }

    fclose(f);
    return TMResult::TM_OK;
}

int TMUtilSave::Picture(TMVideoFrame &frame, string &filename, bool append)
{
    return TMUtilSave::Picture(frame, filename.c_str(), append);
}

int TMUtilSave::Packet(TMPacket &pkt, string &filename)
{
    FILE *f = fopen(filename.c_str(), "wb");
    fwrite(pkt.mData, 1, pkt.mDataLength, f);
    fclose(f);

    return TMResult::TM_OK;
}

