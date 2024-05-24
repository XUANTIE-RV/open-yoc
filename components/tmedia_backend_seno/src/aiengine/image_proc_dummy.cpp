#include <tmedia_backend_seno/aiengine/image_proc_dummy.h>

ImageProcDummy::ImageProcDummy()
{
    
}

ImageProcDummy::~ImageProcDummy()
{
    
}


int ImageProcDummy::Open(int idx)
{
    return TMResult::TM_OK;
}


int ImageProcDummy::Close()
{
    return TMResult::TM_OK;
}


int ImageProcDummy::CvtColor(const TMVideoFrame &src, TMImageInfo::PixelFormat dst_format, TMVideoFrame &dst)
{
    return TMResult::TM_OK;
}


int ImageProcDummy::Resize(const TMVideoFrame &src, const TMImageInfo::ImageSize_t &dst_size, TMVideoFrame &dst)
{
    return TMResult::TM_OK;
}


int ImageProcDummy::CvtResize(const TMVideoFrame &src, const TMImageInfo::ImageSize_t &dst_size, TMImageInfo::PixelFormat dst_format, TMVideoFrame &dst)
{
    return TMResult::TM_OK;
}


int ImageProcDummy::Crop(const TMVideoFrame &src, const TMImageInfo::ImageRect_t &dst_rect, TMVideoFrame &dst)
{
    return TMResult::TM_OK;
}


int ImageProcDummy::CropResize(const TMVideoFrame &src, const TMImageInfo::ImageRect_t &crop_rect, const TMImageInfo::ImageSize_t &dst_size,
                      TMImageInfo::PixelFormat dst_format, TMVideoFrame &dst)
{
    return TMResult::TM_OK;
}

int ImageProcDummy::BatchCropResize(const TMVideoFrame &src, const TMImageInfo::ImageRect_t *crop_rect, const TMImageInfo::ImageSize_t *dst_size, TMImageInfo::PixelFormat dst_format, TMVideoFrame **dst, int count)
{
    return TMResult::TM_NOT_SUPPORT;
}