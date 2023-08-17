/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */
#ifndef IMAGE_PROC_H
#define IMAGE_PROC_H

#include <vector>

#include <tmedia_core/common/error.h>
#include <tmedia_core/common/frame.h>

typedef enum
{
    IMAGE_FLIP_VERT,
    IMAGE_FLIP_HORZ,
    IMAGE_FLIP_VERT_HORZ,
} ImageFlip_e;

typedef enum
{
    PADDING_CONST,
    PADDING_REPLICATE,
    PADDING_REFLECT101,
    PADDING_REFLECT,
} PaddingType_e;

typedef struct
{
    int top;
    int bottom;
    int left;
    int right;
    int constVal;
} PaddingInfo_t;

typedef struct
{
    float a11;
    float a12;
    float xt;
    float a21;
    float a22;
    float yt;
} AffineMatrix_t;

typedef struct
{
    float a11;
    float a12;
    float a13;
    float a21;
    float a22;
    float a23;
    float a31;
    float a32;
    float a33;
} PerspectiveMatrix_t;

typedef struct
{
    TMImageInfo::ImageRect_t crop_rect;
    TMImageInfo::ImageSize_t scale_size;
    TMImageInfo::PixelFormat dst_format;
} ImageProcConfig_t;

class ImageProc
{
public:
    virtual ~ImageProc() {}

    virtual int Open(int idx) = 0;
    virtual int Close() = 0;

    virtual int CvtColor(const TMVideoFrame &src, TMImageInfo::PixelFormat dst_format, TMVideoFrame &dst) = 0;
    virtual int Resize(const TMVideoFrame &src, const TMImageInfo::ImageSize_t &dst_size, TMVideoFrame &dst) = 0;
    virtual int CvtResize(const TMVideoFrame &src, const TMImageInfo::ImageSize_t &dst_size, TMImageInfo::PixelFormat dst_format, TMVideoFrame &dst) = 0;
    virtual int Crop(const TMVideoFrame &src, const TMImageInfo::ImageRect_t &dst_rect, TMVideoFrame &dst) = 0;
    virtual int CropResize(const TMVideoFrame &src, const TMImageInfo::ImageRect_t &crop_rect,
                              const TMImageInfo::ImageSize_t &dst_size, TMImageInfo::PixelFormat dst_format, TMVideoFrame &dst) = 0;
    virtual int BatchCropResize(const TMVideoFrame &src, ImageProcConfig_t *dst_config, int count, TMVideoFrame **dst) { return TMResult::TM_NOT_SUPPORT; }
};

#endif  // IMAGE_PROC_H