/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <iostream>
#ifdef __linux__
#include <tmedia_config.h>
#endif
#ifdef CONFIG_TMEDIA_CAMERA_OPENCV
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/util/util_inc.h>
#include <tmedia_core/entity/format/format_factory.h>
#include <tmedia_backend_seno/format/camera_opencv.h>

using namespace std;

TMCameraOpenCV::TMCameraOpenCV()
{
}

TMCameraOpenCV::~TMCameraOpenCV()
{
}

// TMSrcEntity interface
TMSrcPad* TMCameraOpenCV::GetSrcPad(int padID)
{
	return NULL;
}

// TMFormatInput interface
int TMCameraOpenCV::Open(string deviceName, TMPropertyList *propList)
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::Close()
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::SetConfig(TMPropertyList &propList)
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::GetConfig(TMPropertyList &propList)
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::Start()
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::Stop()
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::RecvFrame(TMVideoFrame &frame, int timeout)
{
    return TMResult::TM_OK;
}

// TMCamera interface
int TMCameraOpenCV::GetModes(TMCameraModes_s &modes)
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::SetMode(TMCameraModeCfg_s &mode)
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::OpenChannel(int chnID, TMPropertyList *propList)
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::CloseChannel(int chnID)
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::SetChannelConfig(int chnID, TMPropertyList &propList)
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::GetChannelConfig(int chnID, TMPropertyList &propList)
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::StartChannel(int chnID)
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::StopChannel(int chnID)
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::RecvChannelFrame(int chnID, TMVideoFrame &frame, int timeout)
{
    return TMResult::TM_OK;
}

int TMCameraOpenCV::ReleaseChannelFrame(int chnID, TMVideoFrame &frame)
{
    return TMResult::TM_OK;
}

REGISTER_VIDEO_INPUT_CLASS(TMMediaInfo::DeviceID::CAMERA, TMCameraOpenCV);
#endif