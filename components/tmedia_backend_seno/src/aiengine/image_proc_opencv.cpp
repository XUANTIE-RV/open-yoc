/*
 * Copyright (C) 2022-2023 Alibaba Group Holding Limited
 */
#ifdef __linux__
#include <tmedia_config.h>
#endif
#ifdef CONFIG_TMEDIA_AIENGINE_IMAGE_PROC_OPENCV
#include <iostream>
#include <opencv2/opencv.hpp>
#include <tmedia_core/tracer/tracer.h>
#include <tmedia_core/entity/aiengine/aiengine_inc.h>
#include <tmedia_backend_seno/aiengine/image_proc_opencv.h>

TMImageProcOpenCV::TMImageProcOpenCV()
{
    TRACE_EVENT(CAT_AIE_IMAGEPROC, "TMImageProcOpenCV");
}

TMImageProcOpenCV::~TMImageProcOpenCV()
{
    TRACE_EVENT(CAT_AIE_IMAGEPROC, "~TMImageProcOpenCV");
}

int TMImageProcOpenCV::Open(int idx)
{
    TRACE_EVENT(CAT_AIE_IMAGEPROC, "Open", "idx", idx);

    return 0;
}

int TMImageProcOpenCV::Close()
{
    TRACE_EVENT(CAT_AIE_IMAGEPROC, "Close");

    return 0;
}

int TMImageProcOpenCV::CvtColor(const TMVideoFrame &src, TMImageInfo::PixelFormat dst_format, TMVideoFrame &dst)
{
    TRACE_EVENT(CAT_AIE_IMAGEPROC, "CvtColor", "dst_format", TMImageInfo::Name(dst_format));

    int width = src.mWidth;
    int height = src.mHeight;
    TMImageInfo::PixelFormat in_format = src.mPixelFormat;
    TMImageInfo::PixelFormat out_format = dst_format;
    void *data_src = src.mData[0];

    cv::Mat srcImage;
    if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
    {
        srcImage.create(height * 3 / 2, width, CV_8UC1);
        memcpy(srcImage.data, data_src, width * height * 3 / 2);
    }
    else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
    {
        srcImage.create(height * 3 / 2, width, CV_8UC1);
        memcpy(srcImage.data, data_src, width * height * 3 / 2);
    }
    else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUYV422)
    {
        srcImage.create(height, width, CV_8UC2);
        memcpy(srcImage.data, data_src, width * height * 2);
    }
    else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888)
    {
        srcImage.create(height, width, CV_8UC3);
        memcpy(srcImage.data, data_src, width * height * 3);
    }
    else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888)
    {
        srcImage.create(height, width, CV_8UC3);
        memcpy(srcImage.data, data_src, width * height * 3);
    }
    else
    {
        return TMResult::TM_NOT_SUPPORT;
    }

    cv::Mat dstImage;
    if (in_format != out_format)
    {
        switch (out_format)
        {
        case TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P:
            if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV422P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2RGB_Y422);
                cv::cvtColor(dstImage, srcImage, cv::COLOR_RGB2YUV_I420);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_RGB2YUV_I420);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_BGR2YUV_I420);
                dstImage.copyTo(srcImage);
            }
            else
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            break;
        case TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV422P:
            if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_RGB2YUV_I420);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_BGR2YUV_I420);
                dstImage.copyTo(srcImage);
            }
            else
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            break;
        case TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888:
            if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2RGB_I420);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2RGB_NV12);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV422P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2RGB_Y422);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUYV422)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2RGB_YUYV);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888)
            {
                ; // nothing
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_BGR2RGB);
                dstImage.copyTo(srcImage);
            }
            else
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            break;
        case TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888:
            if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_I420);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_NV12);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV422P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_Y422);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUYV422)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_YUYV);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_RGB2BGR);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888)
            {
                ; // nothing
            }
            else
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            break;
        case TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888P:
            if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_I420);
                std::vector<cv::Mat> channels;
                cv::Mat imageBlueChannel;
                cv::Mat imageGreenChannel;
                cv::Mat imageRedChannel;
                cv::split(dstImage, channels);
                imageBlueChannel = channels.at(0);
                imageGreenChannel = channels.at(1);
                imageRedChannel = channels.at(2);
                int stride = dstImage.size().width * dstImage.size().height;
                memcpy(dstImage.data + stride * 0, imageRedChannel.data, stride);
                memcpy(dstImage.data + stride * 1, imageGreenChannel.data, stride);
                memcpy(dstImage.data + stride * 2, imageBlueChannel.data, stride);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_NV12);
                std::vector<cv::Mat> channels;
                cv::Mat imageBlueChannel;
                cv::Mat imageGreenChannel;
                cv::Mat imageRedChannel;
                cv::split(dstImage, channels);
                imageBlueChannel = channels.at(0);
                imageGreenChannel = channels.at(1);
                imageRedChannel = channels.at(2);
                int stride = dstImage.size().width * dstImage.size().height;
                memcpy(dstImage.data + stride * 0, imageRedChannel.data, stride);
                memcpy(dstImage.data + stride * 1, imageGreenChannel.data, stride);
                memcpy(dstImage.data + stride * 2, imageBlueChannel.data, stride);
                dstImage.copyTo(srcImage);
            }
            else
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            break;
        case TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888P:
            if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_I420);
                std::vector<cv::Mat> channels;
                cv::Mat imageBlueChannel;
                cv::Mat imageGreenChannel;
                cv::Mat imageRedChannel;
                cv::split(dstImage, channels);
                imageBlueChannel = channels.at(0);
                imageGreenChannel = channels.at(1);
                imageRedChannel = channels.at(2);
                int stride = dstImage.size().width * dstImage.size().height;
                memcpy(dstImage.data + stride * 0, imageBlueChannel.data, stride);
                memcpy(dstImage.data + stride * 1, imageGreenChannel.data, stride);
                memcpy(dstImage.data + stride * 2, imageRedChannel.data, stride);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_NV12);
                std::vector<cv::Mat> channels;
                cv::Mat imageBlueChannel;
                cv::Mat imageGreenChannel;
                cv::Mat imageRedChannel;
                cv::split(dstImage, channels);
                imageBlueChannel = channels.at(0);
                imageGreenChannel = channels.at(1);
                imageRedChannel = channels.at(2);
                int stride = dstImage.size().width * dstImage.size().height;
                memcpy(dstImage.data + stride * 0, imageBlueChannel.data, stride);
                memcpy(dstImage.data + stride * 1, imageGreenChannel.data, stride);
                memcpy(dstImage.data + stride * 2, imageRedChannel.data, stride);
                dstImage.copyTo(srcImage);
            }
            else
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            break;
        default:
            return TMResult::TM_NOT_SUPPORT;
        }
    }
    else
    {
        return TMResult::TM_EINVAL;
    }

    dst.Init();
    dst.mPTS = src.mPTS;
    dst.mWidth = width;
    dst.mHeight = height;
    dst.mPixelFormat = out_format;
    dst.PrepareBuffer();
    memcpy(TMBuffer_Data(dst.GetBuffer()), srcImage.data, TMBuffer_Size(dst.GetBuffer()));

    return TMResult::TM_OK;
}

int TMImageProcOpenCV::Resize(const TMVideoFrame &src, const TMImageInfo::ImageSize_t &dst_size, TMVideoFrame &dst)
{
    TRACE_EVENT(CAT_AIE_IMAGEPROC, "Resize", "dst_size.width", dst_size.width, "dst_size.height", dst_size.height);

    int width = src.mWidth;
    int height = src.mHeight;
    TMImageInfo::PixelFormat in_format = src.mPixelFormat;
    void *data_src = src.mData[0];

    cv::Mat srcImage;
    cv::Mat dstImage;
    if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
    {
        srcImage.create(height * 3 / 2, width, CV_8UC1);
        memcpy(srcImage.data, data_src, width * height * 3 / 2);
        cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2RGB_I420);
        cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
        cv::cvtColor(dstImage, srcImage, cv::COLOR_RGB2YUV_I420);
    }
    else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888)
    {
        srcImage.create(height, width, CV_8UC3);
        memcpy(srcImage.data, data_src, width * height * 3);
        cv::cvtColor(srcImage, dstImage, cv::COLOR_RGB2BGR);
        cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
        cv::cvtColor(dstImage, srcImage, cv::COLOR_BGR2RGB);
    }
    else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888)
    {
        srcImage.create(height, width, CV_8UC3);
        memcpy(srcImage.data, data_src, width * height * 3);
        cv::resize(srcImage, srcImage, cv::Size(dst_size.width, dst_size.height));
    }
    else
    {
        return TMResult::TM_NOT_SUPPORT;
    }

    dst.Init();
    dst.mPTS = src.mPTS;
    dst.mWidth = dst_size.width;
    dst.mHeight = dst_size.height;
    dst.mPixelFormat = src.mPixelFormat;
    dst.PrepareBuffer();
    memcpy(TMBuffer_Data(dst.GetBuffer()), srcImage.data, TMBuffer_Size(dst.GetBuffer()));

    return TMResult::TM_OK;
}

int TMImageProcOpenCV::CvtResize(const TMVideoFrame &src, const TMImageInfo::ImageSize_t &dst_size, TMImageInfo::PixelFormat dst_format, TMVideoFrame &dst)
{
    TRACE_EVENT(CAT_AIE_IMAGEPROC, "CvtResize", "dst_size.width", dst_size.width, "dst_size.height", dst_size.height, "dst_format", TMImageInfo::Name(dst_format));

    int width = src.mWidth;
    int height = src.mHeight;
    TMImageInfo::PixelFormat in_format = src.mPixelFormat;
    TMImageInfo::PixelFormat out_format = dst_format;
    void *data_src = src.mData[0];

    cv::Mat srcImage;
    if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
    {
        srcImage.create(height * 3 / 2, width, CV_8UC1);
        memcpy(srcImage.data, data_src, width * height * 3 / 2);
    }
    else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
    {
        srcImage.create(height * 3 / 2, width, CV_8UC1);
        memcpy(srcImage.data, data_src, width * height * 3 / 2);
    }
    else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUYV422)
    {
        srcImage.create(height, width, CV_8UC2);
        memcpy(srcImage.data, data_src, width * height * 2);
    }
    else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888)
    {
        srcImage.create(height, width, CV_8UC3);
        memcpy(srcImage.data, data_src, width * height * 3);
    }
    else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888)
    {
        srcImage.create(height, width, CV_8UC3);
        memcpy(srcImage.data, data_src, width * height * 3);
    }
    else
    {
        return TMResult::TM_NOT_SUPPORT;
    }

    cv::Mat dstImage;
    if (in_format != out_format)
    {
        switch (out_format)
        {
        case TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P:
            if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV422P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2RGB_Y422);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                cv::cvtColor(dstImage, srcImage, cv::COLOR_RGB2YUV_I420);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888)
            {
                cv::resize(srcImage, srcImage, cv::Size(dst_size.width, dst_size.height));
                cv::cvtColor(srcImage, dstImage, cv::COLOR_RGB2YUV_I420);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888)
            {
                cv::resize(srcImage, srcImage, cv::Size(dst_size.width, dst_size.height));
                cv::cvtColor(srcImage, dstImage, cv::COLOR_BGR2YUV_I420);
                dstImage.copyTo(srcImage);
            }
            else
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            break;
        case TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV422P:
            if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888)
            {
                cv::resize(srcImage, srcImage, cv::Size(dst_size.width, dst_size.height));
                cv::cvtColor(srcImage, dstImage, cv::COLOR_RGB2YUV_I420);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888)
            {
                cv::resize(srcImage, srcImage, cv::Size(dst_size.width, dst_size.height));
                cv::cvtColor(srcImage, dstImage, cv::COLOR_BGR2YUV_I420);
                dstImage.copyTo(srcImage);
            }
            else
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            break;
        case TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888:
            if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2RGB_I420);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2RGB_NV12);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV422P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2RGB_Y422);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUYV422)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2RGB_YUYV);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888)
            {
                cv::resize(srcImage, srcImage, cv::Size(dst_size.width, dst_size.height));
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_BGR2RGB);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                dstImage.copyTo(srcImage);
            }
            else
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            break;
        case TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888:
            if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_I420);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_NV12);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV422P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_Y422);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUYV422)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_YUYV);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_RGB2BGR);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888)
            {
                cv::resize(srcImage, srcImage, cv::Size(dst_size.width, dst_size.height));
            }
            else
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            break;
        case TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888P:
            if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_I420);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                std::vector<cv::Mat> channels;
                cv::Mat imageBlueChannel;
                cv::Mat imageGreenChannel;
                cv::Mat imageRedChannel;
                cv::split(dstImage, channels);
                imageBlueChannel = channels.at(0);
                imageGreenChannel = channels.at(1);
                imageRedChannel = channels.at(2);
                int stride = dstImage.size().width * dstImage.size().height;
                memcpy(dstImage.data + stride * 0, imageRedChannel.data, stride);
                memcpy(dstImage.data + stride * 1, imageGreenChannel.data, stride);
                memcpy(dstImage.data + stride * 2, imageBlueChannel.data, stride);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_NV12);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                std::vector<cv::Mat> channels;
                cv::Mat imageBlueChannel;
                cv::Mat imageGreenChannel;
                cv::Mat imageRedChannel;
                cv::split(dstImage, channels);
                imageBlueChannel = channels.at(0);
                imageGreenChannel = channels.at(1);
                imageRedChannel = channels.at(2);
                int stride = dstImage.size().width * dstImage.size().height;
                memcpy(dstImage.data + stride * 0, imageRedChannel.data, stride);
                memcpy(dstImage.data + stride * 1, imageGreenChannel.data, stride);
                memcpy(dstImage.data + stride * 2, imageBlueChannel.data, stride);
                dstImage.copyTo(srcImage);
            }
            else
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            break;
        case TMImageInfo::PixelFormat::PIXEL_FORMAT_BGR888P:
            if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_I420);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                std::vector<cv::Mat> channels;
                cv::Mat imageBlueChannel;
                cv::Mat imageGreenChannel;
                cv::Mat imageRedChannel;
                cv::split(dstImage, channels);
                imageBlueChannel = channels.at(0);
                imageGreenChannel = channels.at(1);
                imageRedChannel = channels.at(2);
                int stride = dstImage.size().width * dstImage.size().height;
                memcpy(dstImage.data + stride * 0, imageBlueChannel.data, stride);
                memcpy(dstImage.data + stride * 1, imageGreenChannel.data, stride);
                memcpy(dstImage.data + stride * 2, imageRedChannel.data, stride);
                dstImage.copyTo(srcImage);
            }
            else if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_NV12)
            {
                cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_NV12);
                cv::resize(dstImage, dstImage, cv::Size(dst_size.width, dst_size.height));
                std::vector<cv::Mat> channels;
                cv::Mat imageBlueChannel;
                cv::Mat imageGreenChannel;
                cv::Mat imageRedChannel;
                cv::split(dstImage, channels);
                imageBlueChannel = channels.at(0);
                imageGreenChannel = channels.at(1);
                imageRedChannel = channels.at(2);
                int stride = dstImage.size().width * dstImage.size().height;
                memcpy(dstImage.data + stride * 0, imageBlueChannel.data, stride);
                memcpy(dstImage.data + stride * 1, imageGreenChannel.data, stride);
                memcpy(dstImage.data + stride * 2, imageRedChannel.data, stride);
                dstImage.copyTo(srcImage);
            }
            else
            {
                return TMResult::TM_NOT_SUPPORT;
            }
            break;
        default:
            return TMResult::TM_NOT_SUPPORT;
        }
    }
    else
    {
        return TMResult::TM_NOT_SUPPORT;
    }

    dst.Init();
    dst.mPTS = src.mPTS;
    dst.mWidth = dst_size.width;
    dst.mHeight = dst_size.height;
    dst.mPixelFormat = out_format;
    dst.PrepareBuffer();
    memcpy(TMBuffer_Data(dst.GetBuffer()), srcImage.data, TMBuffer_Size(dst.GetBuffer()));

    return TMResult::TM_OK;
}

int TMImageProcOpenCV::Crop(const TMVideoFrame &src, const TMImageInfo::ImageRect_t &dst_rect, TMVideoFrame &dst)
{
    TRACE_EVENT(CAT_AIE_IMAGEPROC, "Crop", "dst_rect.top", dst_rect.top, "dst_rect.left", dst_rect.left, "dst_rect.width", dst_rect.width, "dst_rect.height", dst_rect.height);

    int width = src.mWidth;
    int height = src.mHeight;
    TMImageInfo::PixelFormat in_format = src.mPixelFormat;
    void *data_src = src.mData[0];

    cv::Mat srcImage;
    cv::Mat dstImage;
    if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P)
    {
        srcImage.create(height * 3 / 2, width, CV_8UC1);
        memcpy(srcImage.data, data_src, width * height * 3 / 2);
        cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2RGB_I420);
    }
    else
    {
        return TMResult::TM_NOT_SUPPORT;
    }

    srcImage = dstImage(cv::Rect(dst_rect.left, dst_rect.top, dst_rect.width, dst_rect.height));
    cv::cvtColor(srcImage, srcImage, cv::COLOR_RGB2YUV_I420);

    dst.Init();
    dst.mPTS = src.mPTS;
    dst.mWidth = dst_rect.width;
    dst.mHeight = dst_rect.height;
    dst.mPixelFormat = src.mPixelFormat;
    dst.PrepareBuffer();
    memcpy(TMBuffer_Data(dst.GetBuffer()), srcImage.data, TMBuffer_Size(dst.GetBuffer()));

    return TMResult::TM_OK;
}

int TMImageProcOpenCV::BatchCropResize(const TMVideoFrame &src, const TMImageInfo::ImageRect_t *crop_rect, const TMImageInfo::ImageSize_t *dst_size, TMImageInfo::PixelFormat dst_format, TMVideoFrame **dst, int count)
{
    return TMResult::TM_NOT_SUPPORT;
}

int TMImageProcOpenCV::CropResize(const TMVideoFrame &src,
                                   const TMImageInfo::ImageRect_t &crop_rect,
                                   const TMImageInfo::ImageSize_t &dst_size,
                                   TMImageInfo::PixelFormat dst_format,
                                   TMVideoFrame &dst)
{
    TRACE_EVENT(CAT_AIE_IMAGEPROC, "CropResize",
        "crop_rect.top", crop_rect.top, "crop_rect.left", crop_rect.left, "crop_rect.width", crop_rect.width, "crop_rect.height", crop_rect.height,
        "dst_size.width", dst_size.width, "dst_size.height", dst_size.height, "dst_format", TMImageInfo::Name(dst_format));

    int width = src.mWidth;
    int height = src.mHeight;
    TMImageInfo::PixelFormat in_format = src.mPixelFormat;
    void *data_src = src.mData[0];

    if (dst_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_UNKNOW) {
        dst_format = in_format;
    }

    cv::Mat src_image;
    cv::Mat dst_image;
    if (in_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P && (dst_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P || dst_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888P)) {
        src_image.create(height * 3 / 2, width, CV_8UC1);
        memcpy(src_image.data, data_src, width * height * 3 / 2);
        cv::cvtColor(src_image, dst_image, cv::COLOR_YUV2RGB_I420);
    } else {
        return TMResult::TM_NOT_SUPPORT;
    }

    src_image = dst_image(cv::Rect(crop_rect.left, crop_rect.top, crop_rect.width, crop_rect.height));
    cv::resize(src_image, dst_image, cv::Size(dst_size.width, dst_size.height));

    dst.Init();
    dst.mPTS = src.mPTS;
    dst.mWidth = dst_size.width;
    dst.mHeight = dst_size.height;
    dst.mPixelFormat = dst_format;
    dst.PrepareBuffer();

    if (dst_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P) {
        cv::cvtColor(dst_image, src_image, cv::COLOR_RGB2YUV_I420);
        memcpy(TMBuffer_Data(dst.GetBuffer()), src_image.data, TMBuffer_Size(dst.GetBuffer()));
    } else if (dst_format == TMImageInfo::PixelFormat::PIXEL_FORMAT_RGB888P) {
        std::vector<cv::Mat> channels;
        cv::Mat r_channel;
        cv::Mat g_channel;
        cv::Mat b_channel;
        cv::split(dst_image, channels);
        b_channel = channels.at(0);
        g_channel = channels.at(1);
        r_channel = channels.at(2);
        const int stride = dst_image.size().width * dst_image.size().height;
        char *p = (char *)TMBuffer_Data(dst.GetBuffer());
        memcpy(p + stride * 0, r_channel.data, stride);
        memcpy(p + stride * 1, g_channel.data, stride);
        memcpy(p + stride * 2, b_channel.data, stride);
    }

    return TMResult::TM_OK;
}

REGISTER_IMAGE_PROC_CLASS(TMImageProcOpenCV);

#endif
