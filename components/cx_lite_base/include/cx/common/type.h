/**
 * @file record.h
 * @copyright Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef CX_COMMON_TYPE_H
#define CX_COMMON_TYPE_H

namespace cx {

enum PixelFormat : uint16_t {
    PIX_FORMAT_NONE = 0,    // any pixel format
    PIX_FORMAT_RGB888,
    PIX_FORMAT_RGB888P,
    PIX_FORMAT_BGR888,
    PIX_FORMAT_BGR888P,

    PIX_FORMAT_YUV422P,
	PIX_FORMAT_YUV420P,
	PIX_FORMAT_YUV444P,

    PIX_FORMAT_NV21,
};

enum VencoderType : uint16_t {
    VENC_NONE = 0,
    VENC_H264,
    VENC_H265,

    VENC_BMP,
    VENC_JPEG,
    VENC_PNG,
};

enum CropType : uint8_t {
    CROP_NONE = 0,
    CROP_AROUND,    // crop off around, left the middle part
    CROP_TOP,       // crop off top part
    CROP_BOTTOM,
    CROP_LEFT,
    CROP_RIGHT,
};

enum AudioEncoderType : uint16_t {
    AENC_NONE = 0,
    AENC_G711A,
    AENC_G711U,
    AENC_AAC,
    AENC_MAX,
};

enum AudioSampleBits : uint16_t {
    AUD_SAMPLE_BITS_NONE = 0,
    AUD_SAMPLE_BITS_8BIT,
    AUD_SAMPLE_BITS_16BIT,
};

enum AudioSampleChannels : uint16_t {
    AUD_SAMPLE_CHANNEL_NONE = 0,
    AUD_SAMPLE_CHANNEL_MONO,
    AUD_SAMPLE_CHANNEL_STEREO,
};

enum AudioPcmDataType : uint16_t {
    AUD_PCM_ACCESS_RW_NONE = 0,
    AUD_PCM_ACCESS_RW_INTERLEAVED,
    AUD_PCM_ACCESS_RW_NONINTERLEAVED,
};

enum CameraWorkMode : uint16_t {
    CAMERA_MODE_COLOR = 0,
    CAMERA_MODE_MONO,
};

struct ImageSize {
    uint32_t width;
    uint32_t height;

    bool operator==(const ImageSize &other)
    {
        if (width == other.width && height == other.height) {
                return true;
        }

        return false;
    }
};

struct ImageCoordinate {
    int coordinate_x;
    int coordinate_y;

    bool operator==(const ImageCoordinate &other)
    {
        if (coordinate_x == other.coordinate_x && coordinate_y == other.coordinate_y) {
            return true;
        }

        return false;
    }
};

}


#endif /* CX_COMMON_TYPE_H */

