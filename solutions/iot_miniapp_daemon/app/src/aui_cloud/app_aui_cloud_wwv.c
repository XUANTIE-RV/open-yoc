/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <yoc/aui_cloud.h>
#include <yoc/mic.h>

#include "app_aui_cloud.h"

#define TAG "auiwwv"

int app_aui_cloud_wwv(int confirmed)
{
    aui_mic_send_wakeup_check(confirmed);

    if (confirmed) {
        LOGD(TAG, "WWV confirmed");
    } else {
        LOGD(TAG, "WWV rejected");
        aui_mic_control(MIC_CTRL_STOP_PCM);
        app_aui_cloud_stop(1);
    }

    return 0;
}
