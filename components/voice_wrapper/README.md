# 概述
提供离在线语音标准接口，供不同的算法和云端适配。

# 麦克风服务

## 概述

专为IoT场景打造的麦克风服务，为用户提供本地语音识别各类事件及各类语音控制命令，从而解决传统硬件设备智能化的痛点，并提供麦克风适配层，方便用户适配不同的语音框架。

## 组件安装

```bash
yoc init
yoc install mic
```

## 接口列表

| 函数                    | 说明               |
| :---------------------- | :----------------- |
| aui_mic_evt_t           | 麦克风事件回调     |
| aui_mic_init            | 麦克风初始化       |
| aui_mic_deinit          | 关闭指定麦克风     |
| aui_mic_start           | 启动麦克风服务     |
| aui_mic_stop            | 停止麦克风服务     |
| aui_mic_set_param       | 设置mic相关参数    |
| aui_mic_control         | 麦克风控制命令     |
| aui_mic_get_state       | 获取麦克风状态     |

## 接口详细说明

### aui_mic_evt_t

`int aui_mic_start(utask_t *task, aui_mic_evt_t evt_cb);`

- 功能描述: 
  - 麦克风事件回调。

- 参数: 
  - `task`: utask_t 指针。
  - `evt_cb`: 用户事件回掉。

- 返回值: 
  - 0: 成功。
  - -1: 失败。

### aui_mic_init

`int aui_mic_init(utask_t *task, aui_mic_evt_t evt_cb);`

- 功能描述: 
  - 语音模块初始化。

- 参数: 
  - `task`: utask_t 指针。
  - `evt_cb`: 用户事件回掉。

- 返回值: 
  - 0: 成功。
  - -1: 失败。

### aui_mic_deinit

`int aui_mic_deinit(void);`

- 功能描述: 
  - 去初始化语音模块。

- 参数: 
  - 无。

- 返回值: 
  - 0: 成功。
  - -1: 失败。

### aui_mic_start

`int aui_mic_start(utask_t *task, aui_mic_evt_t evt_cb);`

- 功能描述: 
  - 启动语音服务。

- 参数: 
  - `task`: utask_t 指针。
  - `evt_cb`: 用户事件回掉。

- 返回值: 
  - 0: 成功。
  - -1: 失败。

### aui_mic_stop

`int aui_mic_stop(void);`

- 功能描述: 
  - 停止语音服务。

- 参数: 
  - 无。

- 返回值: 
  - 0: 成功。
  - -1: 失败。

### aui_mic_set_param

`int aui_mic_set_param(void *param);`

- 功能描述: 
  - 设置语音算法参数。语音离线算法使用

- 参数: 
  - `param`: 算法参数。

- 返回值: 
  - 0: 成功。
  - -1: 失败。

### aui_mic_control

`int aui_mic_control(mic_ctrl_cmd_t cmd);`

- 功能描述: 
  - 语音数据控制命令。

- 参数: 
  - `cmd`: 控制命令
    - MIC_CTRL_START_PCM 启动音频数据传输
    - MIC_CTRL_STOP_PCM 停止音频数据传输
    - MIC_CTRL_START_SESSION 强制进入对话模式

- 返回值: 
  - 0: 成功。
  - -1: 失败。

### aui_mic_get_state

`int aui_mic_get_state(mic_state_t *state);`

- 功能描述: 
  - 获取语音算法状态。

- 参数: 
  - `state`: 麦克风状态。

- 返回值: 
  - 0: 成功。
  - -1: 失败。

## 示例

```c
/* mic event*/
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
        default: ;
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

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

- csi
- aos
- record

## 组件参考

无。

# 语音云服务

## 概述

提供了语音合成、语义理解、本地语义端文本理解等接口。

## 配置

无。

## 接口列表

| 函数                                                | 说明                                     |
| :------------------------------------------------- | :--------------------------------------- |
| aui_cloud_init                                     | AUI系统初始化                             |
| aui_cloud_deinit                                   | AUI系统去初始化                           |
| aui_cloud_stop                                     | 停止云端交互所有流程                       |
| aui_cloud_nlp_register / aui_cloud_nlp_unregister  | 向AUI注册（取消）nlp方法                  |
| aui_cloud_tts_register / aui_cloud_tts_unregister  | 向AUI注册（取消）tts方法                  |
| aui_cloud_start_audio                              | 启动语音数据交互                          |
| aui_cloud_push_audio                               | 向云端推送语音数据，回调语义解析结果       |
| aui_cloud_stop_audio                               | 结束语音数据输入                          |
| aui_cloud_push_text                                | 向云端推送文本信息，回调语义解析结果      |
| aui_cloud_start_tts                                | 启动文本转化语音(TTS)                   |
| aui_cloud_req_tts                                  | 向云端推送TTS请求的文本数据，回调TTS音频数据 |
| aui_cloud_stop_tts                                 | 结束TTS请求                              |

## 接口详细说明

### AUI系统初始化

```C
aui_t *aui_cloud_init(aui_config_t *config);
```

* 参数
  * config:  aui cloud配置参数
* 返回值
  * 不等于 NULL: 成功
  * 等于 NULL: 失败



```c
typedef struct aui_config {
    char *per;                      // 发音人选择
    int   vol;                      // 音量，取值0-100，50标准音量
    int   pit;                      // 音调，预留配置
    int   spd;                      // 语速，-500~500，默认0标准语速
    int   asr_fmt;                  // asr push audio时的编码格式， 1：pcm 2：mp3
    int   tts_fmt;                  // tts 成功时的audio编码格式，  1：pcm 2：mp3
    int   nlp_fmt;                  // tts 成功时的audio编码格式，  1：pcm 2：mp3
    int   srate;                    // 采样率，预留
    int   cloud_vad;                // 云端VAD功能使能，预留
    char *tts_cache_path;	        // TTS内部缓存路径，预留
    const char *js_account;         // json格式的账号信息，包含aui系统的所有账号信息
} aui_config_t;
```



### AUI系统去初始化

```CQL
int aui_cloud_deinit(aui_t *aui);
```

- 参数
  - aui: `aui_t`指针

- 返回值
  - 0: 成功
  - < 0: 失败

### 停止AUI系统

```c
int aui_cloud_stop(aui_t *aui);
```

- 参数
  - aui: `aui_t`指针

- 返回值
  - 0: 成功
  - < 0: 失败



### AUI系统（取消）注册asr、nlp、tts

```c
int aui_cloud_nlp_register(aui_t *aui, aui_nlp_cls_t *ops, aui_nlp_cb_t cb, void *priv);
int aui_cloud_tts_register(aui_t *aui, aui_tts_cls_t *ops, aui_tts_cb_t cb, void *priv);
int aui_cloud_nlp_unregister(aui_t *aui);
int aui_cloud_tts_unregister(aui_t *aui);
```

- 参数
  - aui: `aui_t`指针
  - ops： nlp、tts的实现
  - cb：  nlp、tts的事件回调
  - priv：nlp、tts的用户参数

- 返回值
  - 0: 成功
  - < 0: 失败



```c
typedef void (*aui_nlp_cb_t)(void *data, int len, void* nlp_priv);
typedef void (*aui_tts_cb_t)(void *data, int len, void* tts_priv);
```

- data: 各项对应服务的data，默认为json数据包
- len：数据长度
- xxx_priv：nlp、tts的用户参数



### 启动语音数据交互

```C
int aui_cloud_start_audio(aui_t *aui, aui_audio_req_type_e type);
```

* 参数
  * aui: `aui_t`指针
  * type：请求类型
          AUI_AUDIO_REQ_TYPE_NLP: 请求语义
          AUI_AUDIO_REQ_TYPE_WWV: 请求唤醒二次确认
* 返回值
  * 0: 成功
  * < 0: 失败



### 向云端推送语音数据，回调语义解析结果

```C
int aui_cloud_push_audio(aui_t *aui, void *data, size_t size);
```

* 参数
  * aui: `aui_t`指针
  * data: 数据指针
  * size: 数据大小
* 返回值
  * 0: 成功
  * < 0: 失败

### 结束语音数据输入

```C
int aui_cloud_stop_audio(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 0: 成功
  * < 0: 失败


### 向云端推送文本信息，回调语义解析结果

```C
int aui_cloud_push_text(aui_t *aui, char *text);
```

* 参数
  * aui: `aui_t`指针
  * text: 文本字符串
* 返回值
  * 0: 成功
  * < 0: 失败

### 启动文本转化语音(TTS)

```C
int aui_cloud_start_tts(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 0: 成功
  * < 0: 失败

### 向云端推送TTS请求的文本数据，回调TTS音频数据

```C
int aui_cloud_req_tts(aui_t *aui, const char *text);
```

* 参数
  * aui: `aui_t`指针
  * text: 需要转换的文本
* 返回值
  * 0: 成功
  * < 0: 失败

### 结束TTS请求

```C
int aui_cloud_stop_tts(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 0: 成功
  * < 0: 失败

### 停止云端交互所有流程

```C
int aui_cloud_stop(aui_t *aui);
```

* 参数
  * aui: `aui_t`指针
* 返回值
  * 0: 成功
  * < 0: 失败


# 示例代码
无。

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

minilibc: v7.4.0及以上。
aos: v7.4.0及以上。

## 组件参考

无。