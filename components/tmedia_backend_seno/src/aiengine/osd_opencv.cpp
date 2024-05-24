/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#ifdef __linux__
#include <tmedia_config.h>
#endif
#ifdef CONFIG_TMEDIA_AIENGINE_OSD_OPENCV
#include <iostream>
#include <opencv2/opencv.hpp>
#include <tmedia_core/entity/aiengine/aiengine_inc.h>
#include <tmedia_backend_seno/aiengine/osd_opencv.h>

TMOsdOpenCV::TMOsdOpenCV()
{
}

TMOsdOpenCV::~TMOsdOpenCV()
{
}

int TMOsdOpenCV::Open(int idx)
{
    return TMResult::TM_OK;
}

int TMOsdOpenCV::Close()
{
    return TMResult::TM_OK;
}

int TMOsdOpenCV::Line(const TMVideoFrame &frame, const TMImageInfo::ImagePos_t &pt1, const TMImageInfo::ImagePos_t &pt2, const TMImageInfo::ColorScalar_t &color, int thickness)
{
    int width = frame.mWidth;
    int height = frame.mHeight;
    void *data_src = frame.mData[0];

    cv::Mat srcImage;
    cv::Mat dstImage;
    if (frame.mPixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P) {
        srcImage.create(height * 3 / 2, width, CV_8UC1);
        memcpy(srcImage.data, data_src, width * height * 3 / 2);
        cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_I420);
        cv::line(dstImage, cv::Point(pt1.left, pt1.top), cv::Point(pt2.left, pt2.top), cv::Scalar(color.blue, color.green, color.red, 0), thickness);
        cv::cvtColor(dstImage, srcImage, cv::COLOR_BGR2YUV_I420);
    } else {
        return TMResult::TM_NOT_SUPPORT;
    }

    memcpy(TMBuffer_Data(frame.GetBuffer()), srcImage.data, TMBuffer_Size(frame.GetBuffer()));

    return TMResult::TM_OK;
}

int TMOsdOpenCV::Rectangle(const TMVideoFrame &frame, const TMImageInfo::ImageRect_t &rect, const TMImageInfo::ColorScalar_t &color, int thickness)
{
    int width = frame.mWidth;
    int height = frame.mHeight;
    void *data_src = frame.mData[0];

    cv::Mat srcImage;
    cv::Mat dstImage;
    if (frame.mPixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P) {
        srcImage.create(height * 3 / 2, width, CV_8UC1);
        memcpy(srcImage.data, data_src, width * height * 3 / 2);
        cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_I420);
        cv::rectangle(dstImage, cv::Rect(rect.left, rect.top, rect.width, rect.height), cv::Scalar(color.blue, color.green, color.red, 0), thickness);
        cv::cvtColor(dstImage, srcImage, cv::COLOR_BGR2YUV_I420);
    } else {
        return TMResult::TM_NOT_SUPPORT;
    }

    memcpy(TMBuffer_Data(frame.GetBuffer()), srcImage.data, TMBuffer_Size(frame.GetBuffer()));

    return TMResult::TM_OK;
}

int TMOsdOpenCV::Line(const TMVideoFrame &frame_in, TMVideoFrame &frame_out, const TMImageInfo::ImagePos_t &pt1, const TMImageInfo::ImagePos_t &pt2, const TMImageInfo::ColorScalar_t &color, int thickness)
{
    int width = frame_in.mWidth;
    int height = frame_in.mHeight;
    void *data_src = frame_in.mData[0];

    cv::Mat srcImage;
    cv::Mat dstImage;
    if (frame_in.mPixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P) {
        srcImage.create(height * 3 / 2, width, CV_8UC1);
        memcpy(srcImage.data, data_src, width * height * 3 / 2);
        cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_I420);
        cv::line(dstImage, cv::Point(pt1.left, pt1.top), cv::Point(pt2.left, pt2.top), cv::Scalar(color.blue, color.green, color.red, 0), thickness);
        cv::cvtColor(dstImage, srcImage, cv::COLOR_BGR2YUV_I420);
    } else {
        return TMResult::TM_NOT_SUPPORT;
    }

    frame_out.Init();
    frame_out.mPTS = frame_in.mPTS;
    frame_out.mPixelFormat = frame_in.mPixelFormat;
    frame_out.mWidth = width;
    frame_out.mHeight = height;
    frame_out.PrepareBuffer();
    memcpy(TMBuffer_Data(frame_out.GetBuffer()), srcImage.data, TMBuffer_Size(frame_out.GetBuffer()));

    return TMResult::TM_OK;
}

int TMOsdOpenCV::Rectangle(const TMVideoFrame &frame_in, TMVideoFrame &frame_out, const TMImageInfo::ImageRect_t &rect, const TMImageInfo::ColorScalar_t &color, int thickness)
{
    int width = frame_in.mWidth;
    int height = frame_in.mHeight;
    void *data_src = frame_in.mData[0];

    cv::Mat srcImage;
    cv::Mat dstImage;
    if (frame_in.mPixelFormat == TMImageInfo::PixelFormat::PIXEL_FORMAT_YUV420P) {
        srcImage.create(height * 3 / 2, width, CV_8UC1);
        memcpy(srcImage.data, data_src, width * height * 3 / 2);
        cv::cvtColor(srcImage, dstImage, cv::COLOR_YUV2BGR_I420);
        cv::rectangle(dstImage, cv::Rect(rect.left, rect.top, rect.width, rect.height), cv::Scalar(color.blue, color.green, color.red, 0), thickness);
        cv::cvtColor(dstImage, srcImage, cv::COLOR_BGR2YUV_I420);
    } else {
        return TMResult::TM_NOT_SUPPORT;
    }

    frame_out.Init();
    frame_out.mPTS = frame_in.mPTS;
    frame_out.mPixelFormat = frame_in.mPixelFormat;
    frame_out.mWidth = width;
    frame_out.mHeight = height;
    frame_out.PrepareBuffer();
    memcpy(TMBuffer_Data(frame_out.GetBuffer()), srcImage.data, TMBuffer_Size(frame_out.GetBuffer()));

    return TMResult::TM_OK;
}

REGISTER_OSD_CLASS(TMOsdOpenCV);

#endif
