/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */
#ifndef OSD_H
#define OSD_H

#include <vector>

#include <tmedia_core/common/error.h>
#include <tmedia_core/common/frame.h>

class TMPaint
{
public:
    enum class Style : int32_t
    {
        Fill,
        Stroke,
        StrokeAndFill,
    };

    TMPaint::Style             mStyle;
    TMImageInfo::ColorScalar_t mColor;
    float                      mTextSize;

    TMPaint()
    {
        mStyle       = TMPaint::Style::Fill;
        mColor.alpha = 0xFF;
        mColor.red   = 0xFF;
        mColor.green = 0xFF;
        mColor.blue  = 0xFF;
        mTextSize    = 16;
    }

    TMPaint(TMPaint::Style style, TMImageInfo::ColorScalar_t color, float textSize)
    {
        mStyle = style;
        mColor = color;
        mTextSize = textSize;
    }
};

class TMOsd
{
public:
    virtual ~TMOsd() {}

    virtual int Open(int idx = 0) = 0;
    virtual int Close() = 0;

    virtual int Line(const TMVideoFrame &frame, const TMImageInfo::ImagePos_t &pt1, const TMImageInfo::ImagePos_t &pt2, const TMImageInfo::ColorScalar_t &color, int thickness = 1) = 0;
    virtual int Rectangle(const TMVideoFrame &frame, const TMImageInfo::ImageRect_t &rect, const TMImageInfo::ColorScalar_t &color, int thickness = 1) = 0;
    virtual int BatchRectangle(const TMVideoFrame &frame, const TMImageInfo::ImageRect_t *rect, const TMImageInfo::ColorScalar_t &color, int thickness, int count) { return TMResult::TM_NOT_SUPPORT; }

    virtual int Line(const TMVideoFrame &frame_in, TMVideoFrame &frame_out, const TMImageInfo::ImagePos_t &pt1, const TMImageInfo::ImagePos_t &pt2, const TMImageInfo::ColorScalar_t &color, int thickness = 1) = 0;
    virtual int Rectangle(const TMVideoFrame &frame_in, TMVideoFrame &frame_out, const TMImageInfo::ImageRect_t &rect, const TMImageInfo::ColorScalar_t &color, int thickness = 1) = 0;
    virtual int BatchRectangle(const TMVideoFrame &frame_in, TMVideoFrame &frame_out, const TMImageInfo::ImageRect_t *rect, const TMImageInfo::ColorScalar_t &color, int thickness, int count) { return TMResult::TM_NOT_SUPPORT; }

    virtual int String(const TMVideoFrame &frame, const TMImageInfo::ImagePos_t &pt, const std::string text, const TMPaint &paint) { return TMResult::TM_NOT_SUPPORT;}
};

#endif  // OSD_H