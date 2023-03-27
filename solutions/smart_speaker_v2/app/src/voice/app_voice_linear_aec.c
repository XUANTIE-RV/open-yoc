/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */
#include <yoc/mic.h>
#include <ulog/ulog.h>
#include <dispatch_process.h>

#define TAG "LINEAR_AEC"

#define FRAME_DATA_BYTE (16 * 20 * 2 * 5) // 20ms * sizeof(short) * 5chn

static void linear_aec_process(void *arg)
{
    short * data = (short *)malloc(FRAME_DATA_BYTE);
    long long last_ms = aos_now_ms();
    long long read_byte = 0;

    while(1) {
        int ret = voice_get_feaec_data(data, FRAME_DATA_BYTE, 1000);
        if (ret < 0) {
            LOGD(TAG, "fe ace: unsupport");
            aos_msleep(2000);
            continue;
        }

        read_byte += ret;

        long long now = aos_now_ms();
        if (now - last_ms > 2000) {
            last_ms = now;
            LOGD(TAG, "read byte = %lld", read_byte);
        }
    }
}

void app_linear_aec_init()
{
    aui_mic_control(MIC_CTRL_ENABLE_LINEAR_AEC_DATA, 1);
    aos_task_t task_handle;
    aos_task_new_ext(&task_handle, "linearaec", linear_aec_process, NULL, 4096, AOS_DEFAULT_APP_PRI);
}
