/*
 * Copyright (C) 2022-2023 Alibaba Group Holding Limited
 */

#include <string.h>
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/common/image_info.h>

using namespace std;

#define RGB2YUV_SHIFT 15
#define BY ( (int) (0.114 * 219 / 255 * (1 << RGB2YUV_SHIFT) + 0.5))
#define BV (-(int) (0.081 * 224 / 255 * (1 << RGB2YUV_SHIFT) + 0.5))
#define BU ( (int) (0.500 * 224 / 255 * (1 << RGB2YUV_SHIFT) + 0.5))
#define GY ( (int) (0.587 * 219 / 255 * (1 << RGB2YUV_SHIFT) + 0.5))
#define GV (-(int) (0.419 * 224 / 255 * (1 << RGB2YUV_SHIFT) + 0.5))
#define GU (-(int) (0.331 * 224 / 255 * (1 << RGB2YUV_SHIFT) + 0.5))
#define RY ( (int) (0.299 * 219 / 255 * (1 << RGB2YUV_SHIFT) + 0.5))
#define RV ( (int) (0.500 * 224 / 255 * (1 << RGB2YUV_SHIFT) + 0.5))
#define RU (-(int) (0.169 * 224 / 255 * (1 << RGB2YUV_SHIFT) + 0.5))


int TMImageInfo::getPlaneInfo(TMImageInfo::PixelFormat pixelFormat,
                              int width,
                              int height,
                              PlaneInfo_t *info,
                              int addrAlign,
                              int strideAlign)
{
    int planeNum         = 1;
    int bitWidth         = 8;
    int alignHeight      = 0;
    uint32_t cStride     = 0;
    uint32_t mainStride  = 0;
    uint32_t mainSize    = 0;
    uint32_t ySize       = 0;
    uint32_t cSize       = 0;
    bool bCompressFormat = false;

    if (info == NULL)
    {
        return TMResult::TM_EINVAL;
    }

    if (strideAlign == 0)
    {
        strideAlign = IMAGE_ALIGN_DEFAULT;
    }
    else if (strideAlign > IMAGE_ALIGN_MAX)
    {
        strideAlign = IMAGE_ALIGN_MAX;
    }
    else
    {
        strideAlign = IMAGE_ALIGN(strideAlign, IMAGE_ALIGN_DEFAULT);
    }

    if (addrAlign == 0)
    {
        addrAlign = IMAGE_ALIGN_DEFAULT;
    }
    else
    {
        addrAlign = IMAGE_ALIGN(addrAlign, IMAGE_ALIGN_DEFAULT);
    }


    if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P || pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
    {
        alignHeight = IMAGE_ALIGN(height, 2);
    }
    else
    {
        alignHeight = height;
    }

    mainStride = IMAGE_ALIGN((width * bitWidth + 7) >> 3, strideAlign);
    ySize = IMAGE_ALIGN(mainStride * alignHeight, addrAlign);

    if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)   ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    {
        planeNum = 3;

        cStride = IMAGE_ALIGN(((width >> 1) * bitWidth + 7) >> 3, strideAlign);
        cSize = IMAGE_ALIGN((cStride * alignHeight) >> 1, addrAlign);

        mainStride = cStride * 2;
        ySize = IMAGE_ALIGN(mainStride * alignHeight, addrAlign);

        mainSize = ySize + (cSize << 1);

    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)     ///< semi planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    {
        planeNum = 2;

        cStride = IMAGE_ALIGN((width * bitWidth + 7) >> 3, strideAlign);
        cSize = IMAGE_ALIGN((cStride * alignHeight) >> 1, addrAlign);

        mainSize = ySize + cSize;

    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV16)     ///< semi planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    {
        planeNum = 2;

        cStride = IMAGE_ALIGN((width * bitWidth + 7) >> 3, strideAlign);
        cSize = IMAGE_ALIGN(cStride * alignHeight, addrAlign);

        mainSize = ySize + cSize;

    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV422P)     ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    {
        planeNum = 3;

        cStride = IMAGE_ALIGN(((width >> 1) * bitWidth + 7) >> 3, strideAlign);
        cSize = IMAGE_ALIGN(cStride * alignHeight, addrAlign);

        mainSize = ySize + (cSize << 1);


    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUYV422)     ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
    {
        planeNum = 1;

        mainStride =  IMAGE_ALIGN(((width << 1) * bitWidth + 7) >> 3, strideAlign);
        ySize = IMAGE_ALIGN(mainStride * alignHeight, addrAlign);
        mainSize = ySize;
    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888 ||   ///< packed RGB 8:8:8, 24bpp, RGBRGB...
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888)   ///< packed BGR 8:8:8, 24bpp, BGRBGR...
    {

        planeNum = 1;

        mainStride = IMAGE_ALIGN((width * 24 + 7) >> 3, strideAlign);
        ySize = IMAGE_ALIGN(mainStride * alignHeight, addrAlign);
        mainSize = ySize;
    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888P ||   ///< planar RGB 8:8:8
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888P)   ///< planar BGR 8:8:8
    {
        planeNum = 3;

        cStride = mainStride;
        cSize = ySize;

        mainSize = ySize + (cSize << 1);
    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_ARGB8888 ||   ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGBA8888 ||   ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_ABGR8888 ||   ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGRA8888)     ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...
    {

        planeNum = 1;

        mainStride = IMAGE_ALIGN((width * 32 + 7) >> 3, strideAlign);
        ySize = IMAGE_ALIGN(mainStride * alignHeight, addrAlign);
        mainSize = ySize;
    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_ARGB8888P ||   ///< planar ARGB 8:8:8:8
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGBA8888P ||   ///< planar RGBA 8:8:8:8
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_ABGR8888P ||   ///< planar ABGR 8:8:8:8
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGRA8888P)     ///< planar BGRA 8:8:8:8
    {
        planeNum = 4;

        cStride = mainStride;
        cSize = ySize;

        mainSize = ySize + cSize*3;
    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_GRAY)     //< GRAY, 8bpp
    {
        planeNum = 1;

        mainStride =  IMAGE_ALIGN((width * bitWidth + 7) >> 3, strideAlign);
        ySize = IMAGE_ALIGN(mainStride * alignHeight, addrAlign);
        mainSize = ySize;

    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_JPEG)     ///< JPEG
    {
        //as a compress format
        planeNum = 1;
        bCompressFormat = true;

        mainStride = 0;
        mainSize = 0;

    }
    else     // packed format
    {
        planeNum = 1;
        mainStride = IMAGE_ALIGN(((width * bitWidth + 7) >> 3) * 3, strideAlign);
        ySize = IMAGE_ALIGN(mainStride * alignHeight, addrAlign);
        mainSize = ySize;
    }

    memset(info, 0, sizeof(*info));

    info->planeNum = planeNum;
    info->bCompress = bCompressFormat;
    info->totalSize = mainSize;

    if (!bCompressFormat)
    {
        //fill output information
        if (planeNum > 0)
        {
            info->planeSizes[0] = ySize;
            info->stride[0] = mainStride;
        }

        for (int i = 1; i < planeNum; i++)
        {
            info->planeSizes[i] = cSize;
            info->stride[i] = cStride;
        }
    }

    return TMResult::TM_OK;
}

int TMImageInfo::getPlaneInfo(TMImageInfo::PixelFormat pixelFormat,
                              int width,
                              int height,
                              int stride[3],
                              PlaneInfo_t *info)
{
    int planeNum         = 1;
    int alignHeight      = 0;
    uint32_t cStride     = 0;
    uint32_t mainStride  = 0;
    uint32_t mainSize    = 0;
    uint32_t ySize       = 0;
    uint32_t cSize       = 0;
    bool bCompressFormat = false;

    if (info == NULL)
    {
        return TMResult::TM_EINVAL;
    }

    if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P || pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
    {
        alignHeight = IMAGE_ALIGN(height, 2);
    }
    else
    {
        alignHeight = height;
    }

    mainStride = stride[0];
    ySize = mainStride * alignHeight;

    if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)   ///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    {
        planeNum = 3;

        cStride = stride[1];
        cSize = (cStride * alignHeight) >> 1;

        mainStride = cStride * 2;
        ySize = mainStride * alignHeight;

        mainSize = ySize + (cSize << 1);

    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)     ///< semi planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    {
        planeNum = 2;

        cStride = stride[1];
        cSize = (cStride * alignHeight) >> 1;

        mainSize = ySize + cSize;

    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV16)     ///< semi planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    {
        planeNum = 2;

        cStride = stride[1];
        cSize = cStride * alignHeight;

        mainSize = ySize + cSize;

    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV422P)     ///< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    {
        planeNum = 3;

        cStride = stride[1];
        cSize = cStride * alignHeight;

        mainSize = ySize + (cSize << 1);


    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUYV422)     ///< packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
    {
        planeNum = 1;

        mainStride =  stride[0];
        ySize = mainStride * alignHeight;
        mainSize = ySize;
    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888 ||   ///< packed RGB 8:8:8, 24bpp, RGBRGB...
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888)   ///< packed BGR 8:8:8, 24bpp, BGRBGR...
    {

        planeNum = 1;

        mainStride = stride[0];
        ySize = mainStride * alignHeight;
        mainSize = ySize;
    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888P ||   ///< planar RGB 8:8:8
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888P)   ///< planar BGR 8:8:8
    {
        planeNum = 3;

        cStride = mainStride;
        cSize = ySize;

        mainSize = ySize + (cSize << 1);
    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_ARGB8888 ||   ///< packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGBA8888 ||   ///< packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_ABGR8888 ||   ///< packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGRA8888)     ///< packed BGRA 8:8:8:8, 32bpp, BGRABGRA...
    {

        planeNum = 1;

        mainStride = stride[0];
        ySize = mainStride * alignHeight;
        mainSize = ySize;
    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_ARGB8888P ||   ///< planar ARGB 8:8:8:8
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGBA8888P ||   ///< planar RGBA 8:8:8:8
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_ABGR8888P ||   ///< planar ABGR 8:8:8:8
             pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGRA8888P)   ///< planar BGRA 8:8:8:8
    {
        planeNum = 4;

        cStride = mainStride;
        cSize = ySize;

        mainSize = ySize + cSize*3;
    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_GRAY)     //< GRAY, 8bpp
    {
        planeNum = 1;

        mainStride = stride[0];
        ySize = mainStride * alignHeight;
        mainSize = ySize;

    }
    else if (pixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_JPEG)     ///< JPEG
    {
        //as a compress format
        planeNum = 1;
        bCompressFormat = true;

        mainStride = 0;
        mainSize = 0;

    }
    else     // packed format
    {
        planeNum = 1;
        mainStride = stride[0];
        ySize = mainStride * alignHeight;
        mainSize = ySize;
    }

    memset(info, 0, sizeof(*info));

    info->planeNum = planeNum;
    info->bCompress = bCompressFormat;
    info->totalSize = mainSize;

    if (!bCompressFormat)
    {
        //fill output information
        if (planeNum > 0)
        {
            info->planeSizes[0] = ySize;
            info->stride[0] = mainStride;
        }

        for (int i = 1; i < planeNum; i++)
        {
            info->planeSizes[i] = cSize;
            info->stride[i] = cStride;
        }
    }

    return TMResult::TM_OK;
}

const string TMImageInfo::Name(TMImageInfo::PixelFormat pixelFmt)
{
    switch(pixelFmt)
    {
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_UNKNOW:
        return "PIXEL_FORMAT_UNKNOW";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P:
        return "PIXEL_FORMAT_YUV420P";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12:
        return "PIXEL_FORMAT_NV12";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV422P:
        return "PIXEL_FORMAT_YUV422P";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_YUYV422:
        return "PIXEL_FORMAT_YUYV422";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888:
        return "PIXEL_FORMAT_RGB888";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888P:
        return "PIXEL_FORMAT_RGB888P";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_RGBA8888:
        return "PIXEL_FORMAT_RGBA8888";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_RGBA8888P:
        return "PIXEL_FORMAT_RGBA8888P";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_ARGB8888:
        return "PIXEL_FORMAT_ARGB8888";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_ARGB8888P:
        return "PIXEL_FORMAT_ARGB8888P";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888 :
        return "PIXEL_FORMAT_BGR888";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888P:
        return "PIXEL_FORMAT_BGR888P";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_BGRA8888:
        return "PIXEL_FORMAT_BGRA8888";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_BGRA8888P:
        return "PIXEL_FORMAT_BGRA8888P";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_ABGR8888:
        return "PIXEL_FORMAT_ABGR8888";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_ABGR8888P:
        return "PIXEL_FORMAT_ABGR8888P";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_GRAY:
        return "PIXEL_FORMAT_GRAY";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_JPEG:
        return "PIXEL_FORMAT_JPEG";
    case TMImageInfo::PixelFormat::PIXEL_FORMAT_BINARY:
        return "PIXEL_FORMAT_BINARY";
    default:
        return "UNKNOWN";
    }
}


int TMImageInfo::MapColor(TMImageInfo::YUVColorValue_t &yuv,
                          TMImageInfo::RGBColorValue_t rgb,
                          TMImageInfo::RGBColorDepth_t rgb_depth)
{
    uint8_t r8, g8, b8;
    if ((rgb_depth.R > 8 || rgb_depth.G > 8 || rgb_depth.B > 8) ||
        (rgb_depth.R == 0 || rgb_depth.G == 0 || rgb_depth.B == 0))
        return TMResult::TM_EINVAL;

    r8 = (rgb_depth.R == 8) ? rgb.R : (uint8_t)(rgb.R * (1 << 8) / (1 << rgb_depth.R));
    g8 = (rgb_depth.G == 8) ? rgb.G : (uint8_t)(rgb.G * (1 << 8) / (1 << rgb_depth.G));
    b8 = (rgb_depth.B == 8) ? rgb.B : (uint8_t)(rgb.B * (1 << 8) / (1 << rgb_depth.B));

    yuv.Y = (uint8_t)((RY * r8 + GY * g8 + BY * b8 + (33 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT);
    yuv.U = (uint8_t)((RU * r8 + GU * g8 + BU * b8 + (257 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT);
    yuv.V = (uint8_t)((RV * r8 + GV * g8 + BV * b8 + (257 << (RGB2YUV_SHIFT - 1))) >> RGB2YUV_SHIFT);

    return TMResult::TM_OK;
}

