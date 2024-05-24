/*
 * Copyright (C) 2023 Alibaba Group Holding Limited
 */

#include <tmedia_core/entity/vpm/vpm_factory.h>

using namespace std;

TMVpm *TMVpmFactory::CreateEntity()
{
    TMVpm *vpm = mVpmClass() ? (TMVpm*)mVpmClass()() : NULL;
    if (vpm != NULL)
    {
        vpm->mName = "Vpm";
    }

    return vpm;
}

