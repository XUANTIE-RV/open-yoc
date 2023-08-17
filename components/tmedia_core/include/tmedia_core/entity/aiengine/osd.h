/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */
#ifndef OSD_H
#define OSD_H

#include <vector>

#include <tmedia_core/common/error.h>
#include <tmedia_core/common/frame.h>

class OSD
{
public:
    virtual ~OSD() {}

    virtual int Open(int idx) = 0;
    virtual int Close() = 0;

    virtual int Line(const TMVideoFrame &frame, const TMImageInfo::ImagePos_t &pt1, const TMImageInfo::ImagePos_t &pt2, const TMImageInfo::ColorScalar_t &color, int thickness = 1) = 0;
    virtual int Rectangle(const TMVideoFrame &frame, const TMImageInfo::ImageRect_t &rect, const TMImageInfo::ColorScalar_t &color, int thickness = 1) = 0;

    virtual int Line(const TMVideoFrame &frame_in, TMVideoFrame &frame_out, const TMImageInfo::ImagePos_t &pt1, const TMImageInfo::ImagePos_t &pt2, const TMImageInfo::ColorScalar_t &color, int thickness = 1) = 0;
    virtual int Rectangle(const TMVideoFrame &frame_in, TMVideoFrame &frame_out, const TMImageInfo::ImageRect_t &rect, const TMImageInfo::ColorScalar_t &color, int thickness = 1) = 0;
};

#endif  // OSD_H