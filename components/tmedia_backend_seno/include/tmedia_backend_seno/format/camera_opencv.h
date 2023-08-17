/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_CAMERA_OPENCV_H
#define TM_CAMERA_OPENCV_H

#include <string>

#ifdef __linux__
#include <tmedia_config.h>
#endif
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/format/camera.h>

using namespace std;

class TMCameraOpenCV : public TMCamera
{
public:
    TMCameraOpenCV();
    virtual ~TMCameraOpenCV();

    // TMSrcEntity interface
    TMSrcPad *GetSrcPad(int padID = 0) final override;

    // TMFormatInput interface
    int Open(string deviceName, TMPropertyList *propList = NULL) final override;
    int Close()                                                  final override;
    int SetConfig(TMPropertyList &propList)                      final override;
    int GetConfig(TMPropertyList &propList)                      final override;
    int Start()                                                  final override;
    int Stop()                                                   final override;
    int RecvFrame(TMVideoFrame &frame, int timeout)              final override;

    // TMCamera interface
    int GetModes(TMCameraModes_s &modes)                              final override;
    int SetMode(TMCameraModeCfg_s &mode)                              final override;
    int OpenChannel(int chnID, TMPropertyList *propList = NULL)       final override;
    int CloseChannel(int chnID)                                       final override;
    int SetChannelConfig(int chnID, TMPropertyList &propList)         final override;
    int GetChannelConfig(int chnID, TMPropertyList &propList)         final override;
    int StartChannel(int chnID)                                       final override;
    int StopChannel(int chnID)                                        final override;
    int RecvChannelFrame(int chnID, TMVideoFrame &frame, int timeout) final override;
    int ReleaseChannelFrame(int chnID, TMVideoFrame &frame)           final override;

    // TMCameraOpenCV interface

private:

};

#endif  /* TM_CAMERA_OPENCV_H */
