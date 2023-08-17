/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_PAD_CVI_H
#define TM_PAD_CVI_H

#include <typeinfo>

#include <tmedia_core/bind/pad.h>
#include "cvi_common.h"


class TMEntity;
class TMSinkPadCvi;

class TMSrcPadCvi : public TMSrcPad
{
    friend class TMSinkPadCvi;

public:
    TMSrcPadCvi(TMEntity *entity, TMPad::Param_s *param);
    ~TMSrcPadCvi() {}

    int Bind(TMPad *pad) override;  // Bind with peer TMPad
    int UnBind() override;                   // UnBind with peer TMPad
    void SetConfig(MMF_CHN_S chn);
    MMF_CHN_S GetConfig(void);
    void DumpInfo() override;

private:
    MMF_CHN_S cviSrcChan;
    TMSinkPadCvi *cviPeerPad;
};

class TMSinkPadCvi : public TMSinkPad
{
    friend class TMSrcPadCvi;

public:
    TMSinkPadCvi(TMEntity *entity, TMPad::Param_s *param);
    ~TMSinkPadCvi() {}

    void SetConfig(MMF_CHN_S chn);
    MMF_CHN_S GetConfig(void);
    void DumpInfo() override;

private:
    MMF_CHN_S cviSinkChan;
    TMSrcPadCvi *cviPeerPad;
};

#endif  // TM_PAD_CVI_H
