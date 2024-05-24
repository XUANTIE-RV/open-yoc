/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */
#ifndef OSD_SENO_H
#define OSD_SENO_H

#include <string>

#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <tmedia_core/entity/aiengine/osd.h>

using namespace std;

class TMOsdSeno : public TMOsd
{
public:
    TMOsdSeno();
    ~TMOsdSeno();

    int Open(int idx);
    int Close();

    int Line(const TMVideoFrame &frame, const TMImageInfo::ImagePos_t &pt1, const TMImageInfo::ImagePos_t &pt2, const TMImageInfo::ColorScalar_t &color, int thickness = 1);
    int Rectangle(const TMVideoFrame &frame, const TMImageInfo::ImageRect_t &rect, const TMImageInfo::ColorScalar_t &color, int thickness = 1);
    int BatchRectangle(const TMVideoFrame &frame, const TMImageInfo::ImageRect_t *rect, const TMImageInfo::ColorScalar_t &color, int thickness, int count);
    int Line(const TMVideoFrame &frame_in, TMVideoFrame &frame_out, const TMImageInfo::ImagePos_t &pt1, const TMImageInfo::ImagePos_t &pt2, const TMImageInfo::ColorScalar_t &color, int thickness = 1);
    int Rectangle(const TMVideoFrame &frame_in, TMVideoFrame &frame_out, const TMImageInfo::ImageRect_t &rect, const TMImageInfo::ColorScalar_t &color, int thickness = 1);

private:
    static void drawHorizontalLine(uint8_t* nv12Data, int width, int height, int stride, int x1, int y1, int x2, int y2, TMImageInfo::YUVColorValue_t yuv_value, int thickness);
    static void drawVirticalLine(uint8_t* nv12Data, int width, int height, int stride, int x1, int y1, int x2, int y2, TMImageInfo::YUVColorValue_t yuv_value, int thickness);
};

#endif