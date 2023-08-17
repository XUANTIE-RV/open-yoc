/*
 * Copyright (C) 2018-2022 Alibaba Group Holding Limited
 */

#include <string>
#include "ulog/ulog.h"
#include "aos/cli.h"
#include <tmedia_core/entity/parser/parser_inc.h>
#include <tmedia_core/entity/format/format_inc.h>
#include <tmedia_core/entity/vpss/vpss_inc.h>
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"

#define TAG "video_parser_demo"

using namespace std;

#define INBUF_SIZE 4096

extern "C" {
    void test_tmedia_vi_vpss(void);
    void test_VpssDump(VPSS_GRP Grp, VPSS_CHN Chn, CVI_U32 u32FrameCnt);
}

void test_tmedia_vi_vpss_origin()
{
    /*******************VI***********************/
    TMMediaInfo::DeviceID deviceID = TMMediaInfo::DeviceID::CAMERA;
    TMCamera *vi_ir = static_cast<TMCamera *>(TMFormatVideoInputFactory::CreateEntity(deviceID));
    TMPropertyList propList;

    propList.Reset();
    propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_WIDTH, 1920);
    propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_HEIGHT, 1080);
    propList.Assign(TMCamera::PropID::CAMERA_WORK_MODE, TMCAMERA_MONO_MODE);

    assert(vi_ir->Open("ir0", &propList) == TMResult::TM_OK);
    assert(vi_ir->Start() == TMResult::TM_OK);

    propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_PIXEL_FORMAT, static_cast<int32_t>(TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21));
    assert(vi_ir->OpenChannel(0, &propList) == TMResult::TM_OK);

    // TMMediaInfo::DeviceID deviceID = TMMediaInfo::DeviceID::CAMERA;
    // TMCamera *vi_rgb = static_cast<TMCamera *>(TMFormatVideoInputFactory::CreateEntity(deviceID));
    // // TMPropertyList propList;


    // propList.Reset();
    // propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_WIDTH, 1920);
    // propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_HEIGHT, 1080);
    // propList.Assign(TMCamera::PropID::CAMERA_WORK_MODE, TMCAMERA_COLOR_MODE);

    // assert(vi_rgb->Open("rgb1", &propList) == TMResult::TM_OK);
    // assert(vi_rgb->Start() == TMResult::TM_OK);

    // propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_PIXEL_FORMAT, static_cast<int32_t>(TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21));
    // assert(vi_rgb->OpenChannel(0, &propList) == TMResult::TM_OK);



    /*******************VPSS**********************/
    TMVpssParams vpssParam;
    TMVpss *vpss = TMVpssFactory::CreateEntity();

    vpssParam.mMaxWidth 	= 1920;
    vpssParam.mMaxHeight 	= 1080;
    vpssParam.mPixelFormat 	= TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21;

    // TMPropertyList propList;
    propList.Reset();

    /* for MPP, get group ID and channel ID from label
        label: [dev id (uint8)]<<16 | [group id (uint8)]<<8 | [channel id (uint8)] */
    // int label;
    // CXCHECK_RET_WITH_RET(Util_String2Int(vpssPtr->label, label) == 0, -1);

    /* 	VPSS1 can bind up to 3 VI devices, but VPSS0 can bind only 1 VI
        so put VPSS1 as the first VPSS in the row, e.g.,

        IR0 --> VPSS1 (online) -----------> VPSS0 (offline) --> AI Process
                                |
                                -----> VJPEG0

        IR1 --> VPSS1 (online) ------------> VENC0
    */

    propList.Assign(VPSS_ONLINE_MODE_ID, 0);
    propList.Assign(VPSS_DEVICE_ID, 1);
    propList.Assign(VPSS_GROUP_ID, 0);
    propList.Assign(VPSS_CHANNEL_ID, 0);
    
    propList.Assign(TMVpss::PropID::OUTPUT_WIDTH, 480);
    propList.Assign(TMVpss::PropID::OUTPUT_HEIGHT, 640);
    propList.Assign(TMVpss::PropID::OUTPUT_PIXEL_FORMAT, static_cast<int32_t>(TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21));
    // if(vpssPtr->rotationAngle != 0) {
    propList.Assign(TMVpss::PropID::ROTATE_ENABLE, 90);
    // }

    vpss->Open(vpssParam, &propList);
    vpss->Start();



    TMCamera *vi_rgb = static_cast<TMCamera *>(TMFormatVideoInputFactory::CreateEntity(deviceID));

    propList.Reset();
    propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_WIDTH, 1920);
    propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_HEIGHT, 1080);
    propList.Assign(TMCamera::PropID::CAMERA_WORK_MODE, TMCAMERA_COLOR_MODE);

    assert(vi_rgb->Open("rgb1", &propList) == TMResult::TM_OK);
    assert(vi_rgb->Start() == TMResult::TM_OK);

    propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_PIXEL_FORMAT, static_cast<int32_t>(TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21));
    assert(vi_rgb->OpenChannel(0, &propList) == TMResult::TM_OK);


    /*******************VPSS**********************/
    // TMVpssParams vpssParam;
    vpss = TMVpssFactory::CreateEntity();

    vpssParam.mMaxWidth 	= 1920;
    vpssParam.mMaxHeight 	= 1080;
    vpssParam.mPixelFormat 	= TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21;

    // TMPropertyList propList;
    propList.Reset();

    /* for MPP, get group ID and channel ID from label
        label: [dev id (uint8)]<<16 | [group id (uint8)]<<8 | [channel id (uint8)] */
    // int label;
    // CXCHECK_RET_WITH_RET(Util_String2Int(vpssPtr->label, label) == 0, -1);

    /* 	VPSS1 can bind up to 3 VI devices, but VPSS0 can bind only 1 VI
        so put VPSS1 as the first VPSS in the row, e.g.,

        IR0 --> VPSS1 (online) -----------> VPSS0 (offline) --> AI Process
                                |
                                -----> VJPEG0

        IR1 --> VPSS1 (online) ------------> VENC0
    */

    propList.Assign(VPSS_ONLINE_MODE_ID, 0);
    propList.Assign(VPSS_DEVICE_ID, 1);
    propList.Assign(VPSS_GROUP_ID, 1);
    propList.Assign(VPSS_CHANNEL_ID, 0);
    
    propList.Assign(TMVpss::PropID::OUTPUT_WIDTH, 480);
    propList.Assign(TMVpss::PropID::OUTPUT_HEIGHT, 640);
    propList.Assign(TMVpss::PropID::OUTPUT_PIXEL_FORMAT, static_cast<int32_t>(TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21));
    // if(vpssPtr->rotationAngle != 0) {
    propList.Assign(TMVpss::PropID::ROTATE_ENABLE, 90);
    // }

    vpss->Open(vpssParam, &propList);
    vpss->Start();


    test_VpssDump(0, 0, 10);
    
}



void test_tmedia_vi_vpss()
{
    // /*******************VI***********************/
    TMMediaInfo::DeviceID deviceID = TMMediaInfo::DeviceID::CAMERA;
    TMPropertyList propList;

    TMCamera *vi_ir = static_cast<TMCamera *>(TMFormatVideoInputFactory::CreateEntity(deviceID));
    propList.Reset();
    propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_WIDTH, 1920);
    propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_HEIGHT, 1080);
    propList.Assign(TMCamera::PropID::CAMERA_WORK_MODE, TMCAMERA_MONO_MODE);

    assert(vi_ir->Open("ir0", &propList) == TMResult::TM_OK);
    assert(vi_ir->Start() == TMResult::TM_OK);

    propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_PIXEL_FORMAT, static_cast<int32_t>(TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21));
    assert(vi_ir->OpenChannel(0, &propList) == TMResult::TM_OK);



    TMCamera *vi_rgb = static_cast<TMCamera *>(TMFormatVideoInputFactory::CreateEntity(deviceID));
    propList.Reset();
    propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_WIDTH, 1920);
    propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_HEIGHT, 1080);
    propList.Assign(TMCamera::PropID::CAMERA_WORK_MODE, TMCAMERA_COLOR_MODE);

    assert(vi_rgb->Open("rgb1", &propList) == TMResult::TM_OK);
    assert(vi_rgb->Start() == TMResult::TM_OK);

    propList.Assign(TMCamera::PropID::CHANNEL_OUTPUT_PIXEL_FORMAT, static_cast<int32_t>(TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21));
    assert(vi_rgb->OpenChannel(0, &propList) == TMResult::TM_OK);


    /*******************VPSS**********************/
    TMVpssParams vpssParam;
    TMVpss *vpss = TMVpssFactory::CreateEntity();

    vpssParam.mMaxWidth 	= 1920;
    vpssParam.mMaxHeight 	= 1080;
    vpssParam.mPixelFormat 	= TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21;

    // TMPropertyList propList;
    propList.Reset();

    /* for MPP, get group ID and channel ID from label
        label: [dev id (uint8)]<<16 | [group id (uint8)]<<8 | [channel id (uint8)] */
    // int label;
    // CXCHECK_RET_WITH_RET(Util_String2Int(vpssPtr->label, label) == 0, -1);

    /* 	VPSS1 can bind up to 3 VI devices, but VPSS0 can bind only 1 VI
        so put VPSS1 as the first VPSS in the row, e.g.,

        IR0 --> VPSS1 (online) -----------> VPSS0 (offline) --> AI Process
                                |
                                -----> VJPEG0

        IR1 --> VPSS1 (online) ------------> VENC0
    */

    propList.Assign(VPSS_ONLINE_MODE_ID, 0);
    propList.Assign(VPSS_DEVICE_ID, 1);
    propList.Assign(VPSS_GROUP_ID, 0);
    propList.Assign(VPSS_CHANNEL_ID, 0);
    
    propList.Assign(TMVpss::PropID::OUTPUT_WIDTH, 480);
    propList.Assign(TMVpss::PropID::OUTPUT_HEIGHT, 640);
    propList.Assign(TMVpss::PropID::OUTPUT_PIXEL_FORMAT, static_cast<int32_t>(TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21));
    // if(vpssPtr->rotationAngle != 0) {
    propList.Assign(TMVpss::PropID::ROTATE_ENABLE, 90);
    // }

    vpss->Open(vpssParam, &propList);
    vpss->Start();



    /*******************VPSS**********************/
    // TMVpssParams vpssParam;
    vpss = TMVpssFactory::CreateEntity();

    vpssParam.mMaxWidth 	= 1920;
    vpssParam.mMaxHeight 	= 1080;
    vpssParam.mPixelFormat 	= TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21;

    // TMPropertyList propList;
    propList.Reset();

    /* for MPP, get group ID and channel ID from label
        label: [dev id (uint8)]<<16 | [group id (uint8)]<<8 | [channel id (uint8)] */
    // int label;
    // CXCHECK_RET_WITH_RET(Util_String2Int(vpssPtr->label, label) == 0, -1);

    /* 	VPSS1 can bind up to 3 VI devices, but VPSS0 can bind only 1 VI
        so put VPSS1 as the first VPSS in the row, e.g.,

        IR0 --> VPSS1 (online) -----------> VPSS0 (offline) --> AI Process
                                |
                                -----> VJPEG0

        IR1 --> VPSS1 (online) ------------> VENC0
    */

    propList.Assign(VPSS_ONLINE_MODE_ID, 0);
    propList.Assign(VPSS_DEVICE_ID, 1);
    propList.Assign(VPSS_GROUP_ID, 1);
    propList.Assign(VPSS_CHANNEL_ID, 0);
    
    propList.Assign(TMVpss::PropID::OUTPUT_WIDTH, 480);
    propList.Assign(TMVpss::PropID::OUTPUT_HEIGHT, 640);
    propList.Assign(TMVpss::PropID::OUTPUT_PIXEL_FORMAT, static_cast<int32_t>(TMImageInfo::PixelFormat::PIXEL_FORMAT_NV21));
    // if(vpssPtr->rotationAngle != 0) {
    propList.Assign(TMVpss::PropID::ROTATE_ENABLE, 90);
    // }

    vpss->Open(vpssParam, &propList);
    vpss->Start();


    test_VpssDump(1, 0, 10);
    
}