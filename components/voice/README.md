# 概述

专为IoT场景打造的智能语音识别框架，为用户提供数据采集接口，LPM接口，语音识别算法接口等接口，方便后期接入云端语音识别等框架，从而解决传统硬件设备智能化的痛点。

# 示例代码

```c

void voice_event(void *priv, voice_evt_id_t evt_id, void *data, int len)
{
    char *p = data;
    int data_len = len;

    if (evt_id == VOICE_ASR_EVT) {
        LOGD(TAG, "voice asr evt");
    } else if (evt_id == VOICE_DATA_EVT) {
        voice_data_t *vdata = (voice_data_t *)data;
        LOGD(TAG, "voice date evt(%p)(%d)", vdata->data, vdata->len);
    } else if (evt_id == VOICE_SILENCE_EVT) {
        LOGD(TAG, "voice silence evt");
    }
}

void mic_adaptor_init(void)
{
    voice_pcm_param_t pcm_p;
    voice_param_t v_p;
    voice_t *v;

    v = voice_init(voice_event, NULL);

    pcm_p.access = 0;
    pcm_p.channles = 1;
    pcm_p.channles_sum = 1;
    pcm_p.rate = 16000;
    pcm_p.sample_bits = 16;
    pcm_p.pcm_name = "pcmC0";
    voice_add_mic(v, &pcm_p);
    pcm_p.pcm_name = "pcmC2";
    voice_add_ref(v, &pcm_p);

    v_p.cts_ms = 80;
    v_p.ipc_mode = 0;
    voice_config(v, &v_p);

    voice_start(v);

    voice_backflow_control(v, VOICE_AEC_DATA, 1);
}

```