/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_IMAGE_PROC_OPENCV_H
#define TM_IMAGE_PROC_OPENCV_H

#include <string>

#ifdef __linux__
#include <tmedia_config.h>
#endif

#include <tmedia_core/entity/aiengine/image_proc.h>

using namespace std;

class ImageProcOpenCV : public ImageProc
{
public:
    ImageProcOpenCV();
    ~ImageProcOpenCV();

    int Open(int idx);
    int Close();

    int CvtColor(const TMVideoFrame &src, TMImageInfo::PixelFormat dst_format, TMVideoFrame &dst);
    int Resize(const TMVideoFrame &src, const TMImageInfo::ImageSize_t &dst_size, TMVideoFrame &dst);
    int CvtResize(const TMVideoFrame &src, const TMImageInfo::ImageSize_t &dst_size, TMImageInfo::PixelFormat dst_format, TMVideoFrame &dst);
    int Crop(const TMVideoFrame &src, const TMImageInfo::ImageRect_t &dst_rect, TMVideoFrame &dst);
    int BatchCropResize(const TMVideoFrame &src, const TMImageInfo::ImageRect_t *crop_rect, const TMImageInfo::ImageSize_t *dst_size, TMImageInfo::PixelFormat dst_format, TMVideoFrame **dst, int count);
    int CropResize(const TMVideoFrame &src, const TMImageInfo::ImageRect_t &crop_rect, const TMImageInfo::ImageSize_t &dst_size,
                      TMImageInfo::PixelFormat dst_format, TMVideoFrame &dst) override;
};

#endif  /* TM_IMAGE_PROC_OPENCV_H */