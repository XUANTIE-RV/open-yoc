# 概述

MiniAlsa是一个在rtos上运行的ALSA(Advanced Linux Sound Architecture)，提供基本的pcm、mixer等接口。

# 示例代码

## A Minimal Playback Program

```C
void playback (void)
{
    int i;
    short buf[128];
    aos_pcm_t *playback_handle;
    aos_pcm_hw_params_t *hw_params;

    aos_pcm_open (&playback_handle, "pcmP0", AOS_PCM_STREAM_PLAYBACK, 0); //打开设备“pcmP0”

    aos_pcm_hw_params_alloca(&hw_params); //申请硬件参数内存空间

    aos_pcm_hw_params_any(playback_handle, hw_params); //初始化硬件参数
    aos_pcm_hw_params_set_access(playback_handle, hw_params, AOS_PCM_ACCESS_RW_INTERLEAVED); // 设置音频数据参数为交错模式
    aos_pcm_hw_params_set_format(playback_handle, hw_params, AOS_PCM_FORMAT_S16_LE); //设置音频数据参数为小端16bit
    aos_pcm_hw_params_set_rate_near(playback_handle, hw_params, 16000, 0); //设置音频数据参数采样率为16K
    aos_pcm_hw_params_set_channels(playback_handle, hw_params, 2); //设置音频数据参数为2通道

    aos_pcm_hw_params(playback_handle, hw_params); //设置硬件参数到具体硬件中

    aos_pcm_hw_params_free(hw_params); //释放硬件参数内存空间

    aos_pcm_prepare(playback_handle); //启动“pcmP0”设备

    for (i = 0; i < 10; ++i) {
        aos_pcm_writei(playback_handle, buf, 128);//发送交错音频数据
    }

    aos_pcm_close (playback_handle); //关闭设备
}

```

## A Minimal Capture Program

```c
void captrue(void)
{
    int i;
    int err;
    short buf[128];
    aos_pcm_t *capture_handle;
    aos_pcm_hw_params_t *hw_params;

    aos_pcm_open (&capture_handle, "pcmC0", AOS_PCM_STREAM_CAPTURE, 0);//打开设备“pcmC0”

    aos_pcm_hw_params_malloc (&hw_params);//申请硬件参数内存空间

    aos_pcm_hw_params_any (capture_handle, hw_params);//初始化硬件参数
    aos_pcm_hw_params_set_access (capture_handle, hw_params, AOS_PCM_ACCESS_RW_INTERLEAVED);// 设置音频数据参数为交错模式
    aos_pcm_hw_params_set_format (capture_handle, hw_params, AOS_PCM_FORMAT_S16_LE);//设置音频数据参数为小端16bit
    aos_pcm_hw_params_set_rate_near (capture_handle, hw_params, 16000, 0);//设置音频数据参数采样率为16K
    aos_pcm_hw_params_set_channels (capture_handle, hw_params, 2);//设置音频数据参数为2通道

    aos_pcm_hw_params (capture_handle, hw_params);//设置硬件参数到具体硬件中

    aos_pcm_hw_params_free (hw_params);//释放硬件参数内存空间

    aos_pcm_prepare (capture_handle);/启动“pcmC0”设备

    for (i = 0; i < 10; ++i) {
        aos_pcm_readi (capture_handle, buf, 128);//接收交错音频数据
    }

    aos_pcm_close (capture_handle);//关闭设备
}
```

## A Minimal Mixer

``` C
void mixer(void)
{
    aos_mixer_t *mixer_hdl;
    aos_mixer_elem_t *elem;

    aos_mixer_open(&mixer_hdl, 0);  //申请mixer
    aos_mixer_attach(mixer_hdl, "card0"); //查找声卡 ”card0“
    aos_mixer_load(mixer_hdl); // 装载mxier
    elem = aos_mixer_first_elem(mixer_hdl); //查找第一个元素

    aos_mixer_selem_set_playback_volume_all(elem, 60); //设置音量为60
}
```
