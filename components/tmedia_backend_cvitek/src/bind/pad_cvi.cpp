/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/util/util_inc.h>
#include <tmedia_core/bind/pad.h>
#include "tmedia_backend_cvitek/cvi_pad/pad_cvi.h"
#include "cvi_sys.h"
#include "tmedia_backend_cvitek/cvi_util/util_cvi.h"

using namespace std;

TMSrcPadCvi::TMSrcPadCvi(TMEntity *entity, TMPad::Param_s *param)
    : TMSrcPad(entity, param), cviPeerPad(NULL)
{
}

int TMSrcPadCvi::Bind(TMPad *pad)
{
    if (pad == NULL)
    {
        TMEDIA_PRINTF("Error: pad is NULL\n");
        return TMResult::TM_EINVAL;
    }

    TMSinkPadCvi *sinkPad = dynamic_cast<TMSinkPadCvi *>(pad);
    if (sinkPad == NULL)
    {
        TMEDIA_PRINTF("Error: pad is not TMSinkPadCvi\n");
        return TMResult::TM_EINVAL;
    }

    int ret = CVI_SYS_Bind(&cviSrcChan, &sinkPad->cviSinkChan);
    if (ret != 0) 
    {
        TMEDIA_PRINTF("Error: cvi bind failed\n");
        return TMResult::TM_EINVAL;
    }
    cviPeerPad = sinkPad;
    TMEDIA_PRINTF("## Bind ## src=%d:%d dst=%d:%d \n", cviSrcChan.s32DevId, cviSrcChan.s32ChnId, sinkPad->cviSinkChan.s32DevId, sinkPad->cviSinkChan.s32ChnId);
    this->DumpInfo();

    return TMResult::TM_OK;
}

int TMSrcPadCvi::UnBind()
{
    if (cviPeerPad == NULL) {
        TMEDIA_PRINTF("Error: no peer pad\n");
        return TMResult::TM_EINVAL;
    }
    TMEDIA_PRINTF("## Unbind ##\n");
    this->DumpInfo();
    int ret = CVI_SYS_UnBind(&cviSrcChan, &cviPeerPad->cviSinkChan);
    if (ret != 0) 
    {
        TMEDIA_PRINTF("Error: cvi unbind failed\n");
        return TMResult::TM_EINVAL;
    }
    cviPeerPad = NULL;

    return TMResult::TM_OK;
}

void TMSrcPadCvi::SetConfig(MMF_CHN_S chn)
{
    cviSrcChan.enModId = chn.enModId;
    cviSrcChan.s32DevId = chn.s32DevId;
    cviSrcChan.s32ChnId = chn.s32ChnId;
}

MMF_CHN_S TMSrcPadCvi::GetConfig(void)
{
    return cviSrcChan;
}

void TMSrcPadCvi::DumpInfo()
{
    TMEDIA_PRINTF(">>>> Dumping TMSrcPadCvi info begin >>>>\n");
    // TMPad::DumpInfo();
    if (cviPeerPad != NULL) 
    {
        TMEDIA_PRINTF("  Chan  : %d %d %d\n", cviSrcChan.enModId, cviSrcChan.s32DevId, cviSrcChan.s32ChnId);
        TMEDIA_PRINTF("  PeerChan  : %d %d %d\n",  cviPeerPad->cviSinkChan.enModId,  cviPeerPad->cviSinkChan.s32DevId, cviPeerPad->cviSinkChan.s32ChnId);
    }
    else
    {
        TMEDIA_PRINTF("  Peer Pad  : not exists\n");

    }
    TMEDIA_PRINTF("<<<< Dumping TMSrcPadCvi info end   <<<<\n");
}

TMSinkPadCvi::TMSinkPadCvi(TMEntity *entity, TMPad::Param_s *param)
    : TMSinkPad(entity, param), cviPeerPad(NULL)
{
}

void TMSinkPadCvi::SetConfig(MMF_CHN_S chn)
{
    cviSinkChan.enModId = chn.enModId;
    cviSinkChan.s32DevId = chn.s32DevId;
    cviSinkChan.s32ChnId = chn.s32ChnId;
}

MMF_CHN_S TMSinkPadCvi::GetConfig(void)
{
    return cviSinkChan;
}

void TMSinkPadCvi::DumpInfo()
{
    TMEDIA_PRINTF(">>>> Dumping TMSinkPadCvi info begin >>>>\n");
    // TMPad::DumpInfo();
    if (cviPeerPad != NULL) 
    {
        TMEDIA_PRINTF("  Chan  : %d %d %d\n", cviSinkChan.enModId, cviSinkChan.s32DevId, cviSinkChan.s32ChnId);
        TMEDIA_PRINTF("  PeerChan  : %d %d %d\n", cviPeerPad->cviSrcChan.enModId, cviPeerPad->cviSrcChan.s32DevId, cviPeerPad->cviSrcChan.s32ChnId);
    }
    else
    {
        TMEDIA_PRINTF("  Peer Pad  : not exists\n");

    }
    TMEDIA_PRINTF("<<<< Dumping TMSinkPadCvi info end   <<<<\n");
}
