## 概述

`sc5654`声卡驱动是YoC的多媒体技术中最基础的组成部分，是实现声波/数字信号转换的硬件驱动。

## 配置

无

## 接口说明

### PCM模块结构体

首先，先了解一下`PCM`相关的一个重要结构体`snd_pcm_ops_t`。相关的操作和接口都封装在`snd_pcm_ops_t`这个结构体中，相关定义在文件:`pcm.h`。

```c
typedef struct snd_pcm_ops {
    int (*hw_params_set)(snd_pcm_t *pcm, struct snd_pcm_hw_params *params);
    int (*hw_get_size)(snd_pcm_t *pcm);
    int (*hw_get_remain_size)(snd_pcm_t *pcm);
    int (*prepare)(snd_pcm_t *pcm);
    int (*pause)(snd_pcm_t *pcm, int enable);
    int (*write)(snd_pcm_t *pcm, void *buf, int size);
    int (*read)(snd_pcm_t *pcm, void *buf, int size);
    int (*set_event)(snd_pcm_t *pcm, pcm_event_cb cb, void *priv);
} snd_pcm_ops_t;
```

### PCM接口说明

- **hw_params_set**

  设置pcm参数

- **`hw_get_remain_size`**

  获取剩余未发送pcm数据

- **`prepare`**

  启动设备

- **`pause`**

  暂停pcm发送

- **`write、read`**

  读（写）pcm，注意**该接口操作的pcm均为非交错数据**

- **`set_event`**

  设置事件回调函数

  ```c
  #define PCM_EVT_WRITE (1 << 0)
  #define PCM_EVT_READ (1 << 1)
  #define PCM_EVT_XRUN (1 << 2)/** Stopped: underrun (playback) or overru
  ```

### MIXER模块结构体

`MIXER`相关的一个重要结构体`sm_elem_ops_t`。相关的操作和接口都封装在`sm_elem_ops_t`这个结构体中，相关定义在文件:`mixer.h`

```c
typedef struct sm_elem_ops {
    int (*set_dB)(snd_mixer_elem_t *elem, int l_dB, int r_dB);
    int (*volume_to_dB)(snd_mixer_elem_t *elem, int value);
} sm_elem_ops_t;
```

### MIXER接口说明

- set_dB

  设置增益

- volume_to_dB

  音量0-100转化为对应增益值

## 示例

### 驱动注册

```c
#include <devices/drv_snd_pangu.h>

void board_yoc_init()
{
    snd_card_register(6300);
}
```

### A Minimal Playback Program

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

### A Minimal Capture Program

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

### A Minimal Mixer

```C
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

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

minilibc: v7.2.0及以上。
aos: v7.2.0及以上。

## 组件参考

无。