/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <tmedia_core/entity/vpss/vpss_factory.h>

using namespace std;

TMVpss *TMVpssFactory::CreateEntity()
{
    return mVpssClass() ? (TMVpss*)mVpssClass()() : NULL;
}
