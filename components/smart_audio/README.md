## 概述

在智能语音控制系统中，语音播放是一个多种操作混杂在一起的复杂场景。具体表现如下：

* 声音来源：可能是本地或云端，未来也可以扩展到蓝牙等其他播放源
* 声音类型：包括本地提示音，云端下发TTS（Text-To-Speech）,云端下发音乐链接等类型；每种声音类型的处理都略有不同。
* 行为多样：用户会发起多种播放控制请求，例如音量控制，暂停/继续播放，静音控制
* 仲裁管理：当多个播放请求同时产生时，决定播放优先顺序

因此需要一个播放器控制组件，在原始播放器的基础上进行更高层的抽象，一方面根据应用场景实现更复杂的逻辑控制，另一方面也对播放器接口进一步抽象，增加开发者的开发效率。

### 播控协议
播控组件包括两种声音类型：

* 系统音：包括系统提示音，以及云端的TTS（Text-To-Speech）声音
* 音乐：包括本地/云端播放的歌曲

系统音比音乐具有更高的优先级，体现在：

* 正在播放音乐：如果此时要播放系统声音，会”打断“音乐（暂停音乐播放），优先播放系统声音；系统声音播放完成后，在根据情况决定是否恢复音乐播放；
* 正在播放系统声音：如果此时要播放音乐，只能等系统声音播放完成后，才能播放音乐。音乐不能打断系统声音播放。

对于同优先级的声音（系统音vs.系统音，音乐vs.音乐），新的声音会打断当前播放声音，并且被打断的声音没法恢复播放。

播控组件定义了四种播放控制指令：

* Play：开始播放
* Stop：停止播放
* Pause：暂停播放
* Resume：恢复播放

针对不同的声音类型，控制指令的行为也有所不同。如下表：

|            | 系统音                                                       | 音乐                                                         |
| ---------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| **Play**   | 立即开始播放：<br>- 如果当前有系统声音正在播放，会打断正在播放的系统声音，被打断的系统声音不能恢复<br>- 如果当前有音乐正在播放，会打断正在播放的音乐，被打断的音乐看情况可以恢复 | - 如果当前有系统声音正在播放，不能打断，等系统声音播放完之后开始播放<br>- 如果当前有音乐正在播放，会打断正在播放的音乐，被打断的音乐不能恢复 |
| **Stop**   | 停止播放                                                     | 停止播放                                                     |
| **Pause**  | 不支持                                                       | 暂停播放                                                     |
| **Resume** | 不支持                                                       | 恢复播放                                                     |

### 最低播放音量

播控组件包含了最低播放音量的设计。这是考虑到，当用户使用唤醒词唤醒设备时，表示用户希望和设备进行语音交互。因此不论当前音量是否为0，都应该让用户听到回复的声音。应用层在初始化播控组件时，需要将最低播放音量传入播控组件；播控组件在播放声音时，会检测当前音量是否为0 -- 如果当前音量为0，使用最低播放音量进行播放，保证用户可以听到声音。
## 组件安装
```bash
yoc init
yoc install smart_audio
```

## 配置
无。

## 接口列表

| 函数 | 说明 |
| :--- | :--- |
| smtaudio_init | 播控初始化 |
| smtaudio_vol_up | 增加音量 |
| smtaudio_vol_down | 减少音量              |
| smtaudio_vol_set | 设置音量到固定值 |
| smtaudio_vol_get | 获取当前音量          |
| smtaudio_pause | 暂停音乐播放          |
| smtaudio_resume | 恢复音乐播放          |
| smtaudio_mute | 进入/退出静音禁麦状态 |
| smtaudio_start      | 开始播放一段音频      |
| smtaudio_stop | 停止当前音频播放      |
| smtaudio_set_config | 设置播控配置参数 |
| smtaudio_get_config | 获取播控配置参数      |
| smtaudio_set_speed | 设置播放速率 |
| smtaudio_get_state | 查看当前播控状态 |
| smtaudio_lpm | 进入/退出低功耗状态 |

## 接口详细说明

### media_evt

`typedef void (*media_evt)(int type, aui_player_evtid_t evt_id);`

- 功能描述:
   - 播放器事件处理回调函数。

- 参数:
   - `type`: 音频类型，音乐或系统音。
   - `evt_id`: 播放器事件

- 返回值:
   - 无。

### smtaudio_init

`int8_t smtaudio_init(size_t stack_size, media_evt_t media_evt, uint8_t min_vol, uint8_t *aef_conf, size_t aef_conf_size, float speed, int resample);`

- 功能描述:
   - 初始化播控组件。

- 参数:
   - `stack_size`: 创建media任务时分配的堆栈空间大小。
   - `media_evt`: 播放器事件处理回调函数。
   - `min_vol`: 最小播放音量。
   - `aef_conf`: 音效配置参数。
   - `aef_conf_size`: 音效配置参数大小。
   - `speed`: 初始播放速度。
   - `resample`: 重采样率。

- 返回值:
   - 0: 初始化成功。
   - -1: 初始化失败。

### smtaudio_vol_up
`int8_t smtaudio_vol_up(int16_t vol);`

- 功能描述:
   - 增加音量。

- 参数:
   - `vol`: 增加音量大小。

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

### smtaudio_vol_down
`int8_t smtaudio_vol_down(int16_t vol);`

- 功能描述:
   - 减少音量。

- 参数:
   - `vol`: 减少音量大小。

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

### smtaudio_vol_set
`int8_t smtaudio_vol_set(int16_t vol);`

- 功能描述:
   - 设置音量到固定值。

- 参数:
   - `vol`: 设置音量大小。

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

### smtaudio_vol_get
`int8_t smtaudio_vol_get(void);`

- 功能描述:
   - 设置音量到固定值。

- 参数:
   - 无。

- 返回值:
   - 获取到的音量值，范围在0~100。

### smtaudio_pause
`int8_t smtaudio_pause(void);`

- 功能描述:
   - 暂停当前音乐播放，对系统声音不生效（系统声不能被暂停）。

- 参数:
  - 无。

- 返回值:
   - 0: 暂停成功。
   - -1: 暂停失败。

### smtaudio_resume
`int8_t smtaudio_resume(void);`

- 功能描述:
   - 恢复被暂停的音乐，对系统声音不生效。如果当前没有音乐被暂停，则直接返回。

- 参数:
  - 无。

- 返回值:
   - 0: 恢复成功。
   - -1: 恢复失败。

### smtaudio_mute
`int8_t smtaudio_mute(void);`

- 功能描述:
   - 进入/退出静音禁麦状态。第一次调用进入静音禁麦状态，第二次调用退出静音禁麦状态。

- 参数:
  - 无。

- 返回值:
   - 0: 操作成功。
   - -1: 操作失败。

### smtaudio_start
`int8_t smtaudio_start(int type, char *url, uint64_t seek_time, uint8_t resume);`

- 功能描述:
   - 开始播放一段音频。

- 参数:
   - `type`: 音频类型，音乐或系统音。
   - `url`：音频内容地址。
   - `seek_time`：播放起始位置，0表示从头播放。
   - `resume`：播放结束后是否恢复播放被打断音乐（只对系统音有效）。

- 返回值:
   - 0: 开始播放成功。
   - -1: 开始播放失败。

### smtaudio_stop
`int8_t smtaudio_stop(int type);`

- 功能描述:
   - 停止当前音频播放。

- 参数:
   - `type`: 音频类型，音乐或系统音。

- 返回值:
   - 0: 停止播放成功。
   - -1: 停止播放失败。

### smtaudio_set_config
`int8_t smtaudio_set_config(aui_player_config_t *config);`

- 功能描述:
   - 设置播控配置参数。

- 参数:
   - `config`: 配置参数。

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

### smtaudio_get_config
`int8_t smtaudio_get_config(aui_player_config_t *config);`

- 功能描述:
   - 获取播控配置参数。

- 参数:
   - `config`: 配置参数。

- 返回值:
   - 0: 获取成功。
   - -1: 获取失败。

### smtaudio_set_speed
`int smtaudio_set_speed(float speed)`

- 功能描述:
   - 设置播放速率。

- 参数:
   - `speed`: 播放速率，范围0.5 ~ 2.0。

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

### smtaudio_get_state
`smtaudio_state_t smtaudio_get_state(void);`

- 功能描述:
   - 获取播控状态。

- 参数:
   - 无。

- 返回值:
   - 当前播控状态，具体说明如下：
```c
// SYSTEM状态变化不会改变SMTAUDIO_STATE，只有MUSIC状态会改变
typedef enum {
    SMTAUDIO_STATE_PLAYING,      // 正在播放状态
    SMTAUDIO_STATE_PAUSE,        // 暂停状态
    SMTAUDIO_STATE_ZERO_VOLUME,  // 零音量状态
    SMTAUDIO_STATE_MUTE,         // 静音禁麦状态，除了禁麦键外，其他按键和语音指令都不响应
    SMTAUDIO_STATE_STOP,         // 停止状态
    SMTAUDIO_STATE_NUM
} smtaudio_state_t;
```

### smtaudio_lpm
`int8_t smtaudio_lpm(uint8_t state);`

- 功能描述:
   - 进入/退出低功耗状态。

- 参数:
   - `state`: 低功耗状态，1 - 进入低功耗，0 - 退出低功耗

- 返回值:
   - 0: 设置成功。
   - -1: 设置失败。

## 示例

```c
// 回调函数，会注册到播控组件中，用于接收播放事件
static void media_evt(int type, aui_player_evtid_t evt_id)
{
  switch (evt_id) {
        case AUI_PLAYER_EVENT_START:
            /* 开始播放事件 */
            ...
            break;
        case AUI_PLAYER_EVENT_ERROR:
            /* 播放错误事件 */
            ...
            break;
        case AUI_PLAYER_EVENT_FINISH:
            /* 播放完成事件 */
            ...
            break;
        case AUI_PLAYER_EVENT_RESUME:
            /* 恢复播放事件 */
            ...
            break;
        case AUI_PLAYER_EVENT_UNDER_RUN:
            /* 播放缓冲数据不足事件 */
            ...
            break;
        case AUI_PLAYER_EVENT_OVER_RUN:
            /* 播放缓冲数据溢出事件 */
            ...
            break;
        default:
            break;
    }
}

void main()
{
    ...;
    /* 初始化播控组件 */
    smtaudio_init(4 * 1024, media_evt, 0,  (uint8_t *)eqcfg, eqcfg_size, 1.0f, 44100);
    ...
    /* 使用系统声音播放网络流 */
    smtaudio_start(MEDIA_SYSTEM,
                  "https://www.kozco.com/tech/LRMonoPhase4.mp3", 0, 1);
    ...
    /* 获取当前播控组件状态，并控制音量 */
    if (SMTAUDIO_STATE_MUTE != smtaudio_get_state()) {
        smtaudio_vol_up(10);
    }
}
```

## 诊断错误码
无。

## 运行资源

无。

## 依赖资源
  - csi
  - rhino
  - aos
  - kv
  - av

## 组件参考
无。