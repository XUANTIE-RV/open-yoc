# 概述

专为IoT场景打造的麦克风服务，为用户提供本地语音识别各类事件及各类语音控制命令，从而解决传统硬件设备智能化的痛点。

# 实例代码

```c
void mic_evt_cb(int source, mic_event_id_t evt_id, void *data, int size)
{

    switch (evt_id) {
        case MIC_EVENT_PCM_DATA:
            LOGD(TAG, "mic pcm evt");
            break;
        case MIC_EVENT_SESSION_START:
            LOGD(TAG, "mic start evt");
            break;
        case MIC_EVENT_SESSION_STOP:
            LOGD(TAG, "mic stop evt");
            break;
        default:;
    }
}

/* mic init */
int app_mic_init(utask_t *task)
{
    int ret;

    voice_mic_register();

    ret = aui_mic_start(task, mic_evt_cb);

    return ret;
}
```