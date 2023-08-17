/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#ifndef TM_IMAGE_INFO_H
#define TM_IMAGE_INFO_H

#include <cstdint>
#include <string>

using namespace std;

#define IMAGE_ALIGN_DEFAULT   64    // {FFmpeg: 32}; {TH1520: VPU=32, G2D=64}
#define IMAGE_ALIGN_MAX       1024
#define IMAGE_ALIGN(x, a)     (((x) + ((a)-1)) & ~((a)-1))

class TMImageInfo
{
public:
    enum PixelFormat
    {
        PIXEL_FORMAT_UNKNOW    = 0,       // unknow
        PIXEL_FORMAT_YUV420P   = 1 << 0,  // planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
        PIXEL_FORMAT_NV12      = 1 << 1,  // semi planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
        PIXEL_FORMAT_YUV422P   = 1 << 2,  // planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
        PIXEL_FORMAT_YUYV422   = 1 << 3,  // packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
        PIXEL_FORMAT_RGB888    = 1 << 4,  // packed RGB 8:8:8, 24bpp, RGBRGB...
        PIXEL_FORMAT_RGB888P   = 1 << 5,  // planar RGB 8:8:8
        PIXEL_FORMAT_RGBA8888  = 1 << 6,  // packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
        PIXEL_FORMAT_RGBA8888P = 1 << 7,  // planar RGBA 8:8:8:8
        PIXEL_FORMAT_ARGB8888  = 1 << 8,  // packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
        PIXEL_FORMAT_ARGB8888P = 1 << 9,  // planar ARGB 8:8:8:8
        PIXEL_FORMAT_BGR888    = 1 << 10, // packed BGR 8:8:8, 24bpp, BGRBGR...
        PIXEL_FORMAT_BGR888P   = 1 << 11, // planar BGR 8:8:8
        PIXEL_FORMAT_BGRA8888  = 1 << 12, // packed BGRA 8:8:8:8, 32bpp, BGRABGRA...
        PIXEL_FORMAT_BGRA8888P = 1 << 13, // planar BGRA 8:8:8:8
        PIXEL_FORMAT_ABGR8888  = 1 << 14, // packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
        PIXEL_FORMAT_ABGR8888P = 1 << 15, // planar ABGR 8:8:8:8
        PIXEL_FORMAT_GRAY      = 1 << 16, // GRAY, 8bpp
        PIXEL_FORMAT_JPEG      = 1 << 17, // JPEG
        PIXEL_FORMAT_BINARY    = 1 << 18, // data format
        PIXEL_FORMAT_NV21      = 1 << 19,
        PIXEL_FORMAT_YV12      = 1 << 20,
        PIXEL_FORMAT_NV16      = 1 << 21  // semi planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    };

    enum Resolution
    {
        QVGA    = 1 << 0,   //320x240
        VGA     = 1 << 1,   //640x480
        XGA     = 1 << 2,   //1024x768
        HD720P  = 1 << 3,   //1280x720
        HD1080P = 1 << 4,   //1920x1080
    };

    typedef struct
    {
        uint32_t    width;
        uint32_t    height;
    } ImageSize_t;

    typedef struct
    {
        uint32_t    left;
        uint32_t    top;
        uint32_t    width;
        uint32_t    height;
    } ImageRect_t;

    typedef struct
    {
        uint32_t    left;
        uint32_t    top;
    } ImagePos_t;

    enum ImageRotation
    {
        IMAGE_ROTATION_0    = 1 << 0,
        IMAGE_ROTATION_90   = 1 << 1,
        IMAGE_ROTATION_180  = 1 << 2,
        IMAGE_ROTATION_270  = 1 << 3,
    };

    typedef struct
    {
        uint8_t alpha; // 0 - 255
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    } ColorScalar_t;

    typedef struct
    {
        bool bCompress;
        int  planeNum;
        int  planeSizes[3];
        int  totalSize;
        int  stride[3];
    } PlaneInfo_t;

    typedef struct
    {
        uint8_t R;
        uint8_t G;
        uint8_t B;
    } RGBColorValue_t;

    typedef struct
    {
        uint8_t R;
        uint8_t G;
        uint8_t B;
    } RGBColorDepth_t;

    typedef struct
    {
        uint8_t Y;
        uint8_t U;
        uint8_t V;
    } YUVColorValue_t;

    static int getPlaneInfo(TMImageInfo::PixelFormat pixelFormat,
                            int width,
                            int height,
                            PlaneInfo_t *info,
                            int strideAlign = 0,
                            int addrAlign = 0);

    static int getPlaneInfo(TMImageInfo::PixelFormat pixelFormat,
                             int width,
                             int height,
                             int stride[3],
                             PlaneInfo_t *info);

    static const string Name(TMImageInfo::PixelFormat pixelFmt);

    static int MapColor(YUVColorValue_t &yuv, RGBColorValue_t rgb, RGBColorDepth_t rgb_depth = {8,8,8});
};

#endif  /* TM_IMAGE_INFO_H */
