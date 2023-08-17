/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#ifndef TM_VPSS_PARAMS_H
#define TM_VPSS_PARAMS_H

#include <string>

#include <tmedia_core/common/common_inc.h>
#include <tmedia_core/entity/vpss/vpss_property.h>

using namespace std;
using namespace vpss_prop;

class TMVpssParams
{
public:
    uint32_t                 mMaxWidth;
    uint32_t                 mMaxHeight;
    TMImageInfo::PixelFormat mPixelFormat;
    DynamicRange_e           mDynamicRange; // TODO: Later
    FrameRateCtrl_s          mFrameRate;

    TMVpssParams();
    ~TMVpssParams() {}
    void Reset();
    void Dump();
};

#endif  /* TM_VPSS_PARAMS_H */
