/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */
#ifdef __linux__
#include <tmedia_config.h>
#endif
#define LOG_LEVEL 1
#include <tmedia_core/common/syslog.h>
#include <iostream>
#include <tmedia_backend_seno/aiengine/osd_seno.h>
#include <tmedia_core/entity/aiengine/aie_factory.h>

TMOsdSeno::TMOsdSeno()
{
}

TMOsdSeno::~TMOsdSeno()
{
}

int TMOsdSeno::Open(int idx)
{
    return TMResult::TM_OK;
}

int TMOsdSeno::Close()
{
    return TMResult::TM_OK;
}

int TMOsdSeno::Line(const TMVideoFrame &frame, const TMImageInfo::ImagePos_t &pt1, const TMImageInfo::ImagePos_t &pt2, const TMImageInfo::ColorScalar_t &color, int thickness)
{
    if(frame.mPixelFormat != TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
    {
        LOG_E("current only support NV12\n");
        return TMResult::TM_NOT_SUPPORT;
    }

    TMImageInfo::YUVColorValue_t yuv_value;
    TMImageInfo::RGBColorValue_t rgb_value = {color.red, color.green, color.blue};
    TMImageInfo::RGBColorDepth_t rgb_depth = {8, 8, 8};
    TMImageInfo::MapColor(yuv_value, rgb_value, rgb_depth);

    uint8_t* yPlane = frame.mData[0];
    uint8_t* uvPlane = frame.mData[0] + frame.mStride[0] * frame.mHeight;

    int x1 = pt1.left, y1 = pt1.top, x2 = pt2.left, y2 = pt2.top;
    // Calculate the distance between the two points
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    // Calculate the number of pixels to be covered
    int numPixels = dx > dy ? dx : dy;

    // Calculate the increment for each pixel
    float xInc = (float)(x2 - x1) / numPixels;
    float yInc = (float)(y2 - y1) / numPixels;

    for (int i = 0; i <= numPixels; i++) 
    {
        int x = x1 + (int)(i * xInc);
        int y = y1 + (int)(i * yInc);
        // Set Y value
        yPlane[y * frame.mStride[0] + x] = yuv_value.Y;
        // Set UV values
        uvPlane[(y / 2 * frame.mStride[1]) + x / 2 * 2] = yuv_value.U;
        uvPlane[(y / 2 * frame.mStride[1]) + x / 2 * 2 +1] = yuv_value.V;
    }

    return TMResult::TM_OK;
}

void TMOsdSeno::drawHorizontalLine(uint8_t* nv12Data, int width, int height, int stride, int x1, int y1, int x2, int y2, TMImageInfo::YUVColorValue_t yuv_value, int thickness)
{
    uint8_t *y_data = nv12Data;
    uint8_t *uv_data = nv12Data + stride * height;
    uint16_t uv_value = (yuv_value.V<<8)+yuv_value.U;

    x1 = x1 & ~1;
    x2 = x2 & ~1;
    y1 = y1 & ~1;
    y2 = y2 & ~1;

    if(thickness > (y1+1))
    {
        thickness = y1+1;
    }

    if((thickness+y2) > height)
    {
        thickness = height-y2;
    }

    for(int i=0; i<thickness; i++)
    {
        memset(y_data + (y1+i)*stride+x1, yuv_value.Y, x2-x1+1);
        if((y1+i)%2 == 0)
        {
            for (int j = x1; j<=x2; j=j+2) 
            {
                *(uint16_t *)(uv_data+(y1+i) * stride/2 + j) = uv_value;
            }
        }
    }
}

void TMOsdSeno::drawVirticalLine(uint8_t* nv12Data, int width, int height, int stride, int x1, int y1, int x2, int y2, TMImageInfo::YUVColorValue_t yuv_value, int thickness)
{
    uint8_t *y_data = nv12Data;
    uint8_t *uv_data = nv12Data + stride * height;
    uint16_t uv_value = (yuv_value.V<<8)+yuv_value.U;

    x1 = x1 & ~1;
    x2 = x2 & ~1;
    y1 = y1 & ~1;
    y2 = y2 & ~1;

    if(thickness > (x1+1))
    {
        thickness = x1+1;
    }

    if((thickness+x2) > width)
    {
        thickness = width-x2;
    }

    for(int i=0; i<thickness; i++)
    {
        for (int j = y1; j <= y2; j++) 
        {
            y_data[j * stride + x1+i] = yuv_value.Y;
            if((x1+i)%2 == 0 && j%2 == 0)
            {
                *(uint16_t *)(uv_data + j*stride/2 + x1+i) = uv_value;
            }
        }
    }
}

int TMOsdSeno::Rectangle(const TMVideoFrame &frame, const TMImageInfo::ImageRect_t &rect, const TMImageInfo::ColorScalar_t &color, int thickness)
{
    if(frame.mPixelFormat != TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
    {
        LOG_E("current only support NV12\n");
        return TMResult::TM_NOT_SUPPORT;
    }

    TMImageInfo::YUVColorValue_t yuv_value;
    TMImageInfo::RGBColorValue_t rgb_value = {color.red, color.green, color.blue};
    TMImageInfo::RGBColorDepth_t rgb_depth = {8, 8, 8};
    TMImageInfo::MapColor(yuv_value, rgb_value, rgb_depth);

    TMImageInfo::ImageRect_t rect_;

    rect_.left = rect.left & ~1;
    rect_.top = rect.top & ~1;
    rect_.width = rect.width & ~1;
    rect_.height = rect.height & ~1;

    int thickness_up = (thickness > (int)(rect_.top+1)) ? (rect_.top+1) : thickness;
    int thickness_down = ((int)(thickness+rect_.top+rect_.height) > frame.mHeight) ? (frame.mHeight-rect_.top-rect_.height) : thickness;
    int thickness_left = (thickness > (int)(rect_.left+1)) ? (rect_.left+1) : thickness;
    int thickness_right = ((int)(thickness+rect_.left +rect_.width) > frame.mWidth) ? (frame.mWidth-rect_.left-rect_.width) : thickness;

    drawHorizontalLine(frame.mData[0], frame.mWidth, frame.mHeight, frame.mStride[0], rect_.left-(thickness_left-1), rect_.top-(thickness_up-1), rect_.left+rect_.width+(thickness_right-1), rect_.top-(thickness_up-1), yuv_value, thickness_up);
    drawHorizontalLine(frame.mData[0], frame.mWidth, frame.mHeight, frame.mStride[0], rect_.left-(thickness_left-1), rect_.top+rect_.height, rect_.left+rect_.width+(thickness_right-1), rect_.top+rect_.height, yuv_value, thickness_down);
    drawVirticalLine(frame.mData[0], frame.mWidth, frame.mHeight, frame.mStride[0], rect_.left-(thickness_left-1), rect_.top-(thickness_up-1), rect_.left-(thickness_left-1), rect_.top+rect_.height+(thickness_down-1), yuv_value, thickness_left);
    drawVirticalLine(frame.mData[0], frame.mWidth, frame.mHeight, frame.mStride[0], rect_.left+rect_.width, rect_.top-(thickness_up-1), rect_.left+rect_.width, rect_.top+rect_.height+(thickness_down-1), yuv_value, thickness_right);

    return TMResult::TM_OK;
}

int TMOsdSeno::BatchRectangle(const TMVideoFrame &frame, const TMImageInfo::ImageRect_t *rect, const TMImageInfo::ColorScalar_t &color, int thickness, int count)
{
    if(frame.mPixelFormat != TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
    {
        LOG_E("current only support NV12\n");
        return TMResult::TM_NOT_SUPPORT;
    }
    TMImageInfo::YUVColorValue_t yuv_value;
    TMImageInfo::RGBColorValue_t rgb_value = {color.red, color.green, color.blue};
    TMImageInfo::RGBColorDepth_t rgb_depth = {8, 8, 8};
    TMImageInfo::MapColor(yuv_value, rgb_value, rgb_depth);

    for(int i=0; i<count; i++)
    {
        TMImageInfo::ImageRect_t rect_;

        rect_.left = rect[i].left & ~1;
        rect_.top = rect[i].top & ~1;
        rect_.width = rect[i].width & ~1;
        rect_.height = rect[i].height & ~1;

        int thickness_up = (thickness > (int)(rect_.top+1)) ? (rect_.top+1) : thickness;
        int thickness_down = ((int)(thickness+rect_.top+rect_.height) > frame.mHeight) ? (frame.mHeight-rect_.top-rect_.height) : thickness;
        int thickness_left = (thickness > (int)(rect_.left+1)) ? (rect_.left+1) : thickness;
        int thickness_right = ((int)(thickness+rect_.left +rect_.width) > frame.mWidth) ? (frame.mWidth-rect_.left-rect_.width) : thickness;

        drawHorizontalLine(frame.mData[0], frame.mWidth, frame.mHeight, frame.mStride[0], rect_.left-(thickness_left-1), rect_.top-(thickness_up-1), rect_.left+rect_.width+(thickness_right-1), rect_.top-(thickness_up-1), yuv_value, thickness_up);
        drawHorizontalLine(frame.mData[0], frame.mWidth, frame.mHeight, frame.mStride[0], rect_.left-(thickness_left-1), rect_.top+rect_.height, rect_.left+rect_.width+(thickness_right-1), rect_.top+rect_.height, yuv_value, thickness_down);
        drawVirticalLine(frame.mData[0], frame.mWidth, frame.mHeight, frame.mStride[0], rect_.left-(thickness_left-1), rect_.top-(thickness_up-1), rect_.left-(thickness_left-1), rect_.top+rect_.height+(thickness_down-1), yuv_value, thickness_left);
        drawVirticalLine(frame.mData[0], frame.mWidth, frame.mHeight, frame.mStride[0], rect_.left+rect_.width, rect_.top-(thickness_up-1), rect_.left+rect_.width, rect_.top+rect_.height+(thickness_down-1), yuv_value, thickness_right);
    }

    return TMResult::TM_OK;
}

int TMOsdSeno::Line(const TMVideoFrame &frame_in, TMVideoFrame &frame_out, const TMImageInfo::ImagePos_t &pt1, const TMImageInfo::ImagePos_t &pt2, const TMImageInfo::ColorScalar_t &color, int thickness)
{
    return TMResult::TM_NOT_SUPPORT;
}

int TMOsdSeno::Rectangle(const TMVideoFrame &frame_in, TMVideoFrame &frame_out, const TMImageInfo::ImageRect_t &rect, const TMImageInfo::ColorScalar_t &color, int thickness)
{
    return TMResult::TM_NOT_SUPPORT;
}

REGISTER_OSD_CLASS(TMOsdSeno);
