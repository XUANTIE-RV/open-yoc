/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <string>
#include <vector>
#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/format/camera.h>
#include "tmedia_backend_cvitek/cvi_pad/pad_cvi.h"
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"
#include "cvi_vi.h"
#include "cvi_isp.h"
#include "cvi_awb_comm.h"
#include "cvi_ae.h"
#include "cvi_awb.h"

class  TMViCvi: public TMCamera 
{
public:

    TMViCvi();
    virtual ~TMViCvi();

    // TMSrcEntity interface
    TMSrcPad  *GetSrcPad(int padID = 0) final override;

    // TMVideoInput interface
    int Open(string deviceName, TMPropertyList *propList = NULL) final override;
    int Close()                                                  final override;
    int SetConfig(TMPropertyList &propList)                      final override;
    int GetConfig(TMPropertyList &propList)                      final override;
    int Start()                                                  final override;
    int Stop()                                                   final override;
    int RecvFrame(TMVideoFrame &frame, int timeout)              final override;

    //TMcamera interface
    int GetModes(TMCameraModes_s &modes)                               final override;
    int SetMode(TMCameraModeCfg_s &mode)                               final override;
    int OpenChannel(int chnID, TMPropertyList *propList = NULL)       final override;
    int CloseChannel(int chnID)                                       final override;
    int SetChannelConfig(int chnID, TMPropertyList &propList)         final override;
    int GetChannelConfig(int chnID, TMPropertyList &propList)         final override;
    int StartChannel(int chnID)                                       final override;
    int StopChannel(int chnID)                                        final override;
    int RecvChannelFrame(int chnID, TMVideoFrame &frame, int timeout) final override;
    int ReleaseChannelFrame(int chnID, TMVideoFrame &frame)           final override;

private:
    map<int, TMSrcPadCvi *>mCviSrcPad;
    int mDeviceID;
    string cameraName;
    TMCameraWorkMode_e mCameraMode;
};

struct vi_cvi_fastconverge_param {
    struct param {
        short luma[5];
        short bv[5];
    };

    std::vector<param>  params;
};
