/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_VPSS_CVI_H
#define TM_VPSS_CVI_H

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/vpss/vpss.h>

#include <cvi_defines.h>
#include <cvi_common.h>
#include <cvi_vpss.h>
#include "cvi_sys.h"
#include "tmedia_backend_cvitek/cvi_pad/pad_cvi.h"

typedef struct {
    VPSS_GRP VpssGrpID;
    VPSS_CHN VpssChnID;
    int WidthIn;
    int HeightIn;
    int WidthOut;
    int HeightOut;
} grp_chn_combo_table_s;

class TMVpssCvi : public TMVpss
{
public:
    TMVpssCvi();
    virtual ~TMVpssCvi();

    // TMFilterEntity interface
    TMSrcPad  *GetSrcPad(int padID = 0)  final override;
    TMSinkPad *GetSinkPad(int padID = 0) final override;

    // TMVpss interface
    int Open(TMVpssParams &codecParam, TMPropertyList *propList = NULL) final override;
    int SetConfig(TMPropertyList &propList)         final override;
    int GetConfig(TMPropertyList &propList)         final override;
    int Start()                                     final override;
    int Flush()                                     final override;
    int Stop()                                      final override;
    int Close()                                     final override;

    int SendFrame(TMVideoFrame &frame, int timeout) final override;
    int RecvFrame(TMVideoFrame &frame, int timeout) final override;

private:
    static void VpssTableInfoPrint(void) {
        for(int i=0; i < (int)mGroupChannelTable.size(); i++) {
            cout << "VPSS: Group:"<< mGroupChannelTable[i].VpssGrpID << " Channel:" << mGroupChannelTable[i].VpssChnID \
            << " In: Width:"<< mGroupChannelTable[i].WidthIn << " Height:"<< mGroupChannelTable[i].HeightIn            \
            << " Out: Width:"<< mGroupChannelTable[i].WidthOut << " Height:"<< mGroupChannelTable[i].HeightOut << endl;
        }
    }
    static bool mCviCreatedVpssGrp[VPSS_MAX_GRP_NUM];
    static bool mCviStartedVpssGrp[VPSS_MAX_GRP_NUM];
    static bool mCviOpenedVpssChn[VPSS_MAX_CHN_NUM];
    static bool mCviUsedOnlineVpss[2];
    static vector<grp_chn_combo_table_s> mGroupChannelTable;
    int MapVpssGrpParam(VPSS_GRP_ATTR_S &outParam, TMVpssParams &inParam);
    int MapVpssChnParam(VPSS_CHN_ATTR_S &outParam, TMPropertyList &inParam);
    int MapVpssCropParam(VPSS_CROP_INFO_S &outParam, TMPropertyList &inParam);
    TMPropertyList mCurrentPropertyList;
    VPSS_GRP mCviVpssGrp;
    VPSS_CHN mCviVpssChn;
    map<int, TMSrcPadCvi *>mCviSrcPad;
    map<int, TMSinkPadCvi *>mCviSinkPad;
    bool mOnlineMode;
    bool mFilterFrameFlag;
    int  mStartFrame;

    static int ReleaseFrameInternal(TMVideoFrame &frame);
    int GetAvaliableVpssGrp(VPSS_GRP &vpssGrp);
    int GetAvaliableVpssChn(VPSS_CHN &vpssChn);
};

#endif  // TM_VPSS_CVI_H
