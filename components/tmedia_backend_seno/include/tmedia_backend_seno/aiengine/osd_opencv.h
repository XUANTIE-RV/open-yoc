/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifndef OSD_OPENCV_H
#define OSD_OPENCV_H

#include <string>

#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <tmedia_core/entity/aiengine/osd.h>

using namespace std;

class OSDOpenCV : public OSD
{
public:
    OSDOpenCV();
    ~OSDOpenCV();

    int Open(int idx);
    int Close();

    int Line(const TMVideoFrame &frame, const TMImageInfo::ImagePos_t &pt1, const TMImageInfo::ImagePos_t &pt2, const TMImageInfo::ColorScalar_t &color, int thickness = 1);
    int Rectangle(const TMVideoFrame &frame, const TMImageInfo::ImageRect_t &rect, const TMImageInfo::ColorScalar_t &color, int thickness = 1);

    int Line(const TMVideoFrame &frame_in, TMVideoFrame &frame_out, const TMImageInfo::ImagePos_t &pt1, const TMImageInfo::ImagePos_t &pt2, const TMImageInfo::ColorScalar_t &color, int thickness = 1);
    int Rectangle(const TMVideoFrame &frame_in, TMVideoFrame &frame_out, const TMImageInfo::ImageRect_t &rect, const TMImageInfo::ColorScalar_t &color, int thickness = 1);
};

#endif