## 概述
专为IoT场景打造的智能语音识别框架，为用户提供数据采集接口，LPM接口，语音识别算法接口等接口，方便后期接入云端语音识别等框架，从而解决传统硬件设备智能化的痛点。

## 组件安装

```bash
yoc init
yoc install voice
```

## 配置
无。

## 接口列表
| 函数 | 说明 |
| :--- | :--- |
| voice_init | 语音模块初始化 |
| voice_deinit | 语音模块注销 |
| voice_config | 语音模块配置 |
| voice_add_mic | 添加麦克风 |
| voice_add_ref | 添加REF |
| voice_start | 语音开始 |
| voice_stop | 语音停止 |
| voice_backflow_control | 语音回流控制 |
| voice_ai_init | 初始化 ai 算法模块 |
| voice_ai_deinit | 智能语音注销 |
| ipc_ch_get | 获取IPC通道 |
| voice_malloc | 语音节点数据内存申请 |
| voice_free | 语音节点数据释放 |
| pcm_init | PCM初始化 |
| pcm_deinit | PCM注销 |
| pcm_mic_config | PCM麦克风配置 |
| pcm_ref_config | PCM REF配置 |
| pcm_start | PCM 开始 |

## 接口详细说明

### voice_init 
`voice_t *voice_init(voice_evt_t cb, void *priv);`

- 功能描述:
   - 语音初始化。
   voice_evt_t定义：
`typedef void (*voice_evt_t)(void *priv, voice_evt_id_t evt_id, void *data, int len);`

#### voice_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| mic_param | voice_pcm_param_t * | 麦克风参数 |
| ref_param | voice_pcm_param_t * | REF参数 |
| param | voice_param_t | 语音参数 |
| cb | voice_evt_t | 事件回调函数 |
| priv | void * | 事件回调函数参数 |
| queue | aos_queue_t | 任务队列 |
| ipc_task | aos_task_t | IPC任务 |
| buffer | uint8_t | 缓冲区 |
| ch | voice_ch_t | 消息处理句柄 |
| ops | char * | 操作函数 |
| pcm | voice_pcm_t | PCM结构 |
| mute_flag | int | 静音标记 |
| mute_timeout | int | 静音时间 |

#### voice_pcm_param_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| pcm_name | char * | pcm设备名称 |
| rate | unsigned int | 采样率 |
| sample_bits | int | 采样位数 |
| access | int | 是否为交错模式，0：非交错 1：交错 |
| channles | int | 通道总数 |
| channles_sum | int | 通道总数 |
| period_bytes | int | pcm周期数据量（用户不必配置） |

#### voice_param_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| cts_ms | char * | CTS |
| ipc_mode | unsigned int | ipc模式 |

#### voice_ch_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| hdl | int | 句柄 |
| cb | voice_msg_evt_t | 回调函数 |
| priv | void * | 回调函数参数 |

#### voice_ch_io_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| init | voice_ch_t *(*init)(voice_msg_evt_t cb, void *priv) | 初始化函数 |
| msg_send | int (*msg_send)(voice_ch_t *ch, voice_msg_t *msg) | 消息发送函数 |

#### voice_pcm_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| priv | void * | 参数 |
| mic | voice_capture_t * | mic数据指针 |
| ref | voice_capture_t * | ref数据指针 |
| data | void * | 数据 |
| len | int | 数据长度 |
| pcm_send | voice_pcm_send | pcm发送 |

#### voice_capture_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| hdl | aos_pcm_t * | 句柄 |
| name | char * | 名称 |
| param | voice_pcm_param_t * | pcm参数 |
| data | void * | 数据 |
| len | int | 数据长度 |

- 参数:
   - `cb`: voice事件。
   - `priv`: 用户私有数据。   
   
- 返回值:
   - 非空: 语音句柄。
   - 空（NULL）: 失败。

### voice_deinit
`void voice_deinit(voice_t *v);`

- 功能描述:
   - 语音注销。

- 参数:
   - `v`: 语音句柄。
   
- 返回值:
   - 无。

### voice_config
`int voice_config(voice_t *v, voice_param_t *p);`

- 功能描述:
   - 语音配置。

- 参数:
   - `v`: 语音句柄。
   - `p`: 语音参数。   
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### voice_add_mic 
`int voice_add_mic(voice_t *v, voice_pcm_param_t *p);`

- 功能描述:
   - 添加麦克风。

- 参数:
   - `v`: 语音句柄。
   - `p`: 麦克风参数。   
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### voice_add_ref
`int voice_add_ref(voice_t *v, voice_pcm_param_t *p);`

- 功能描述:
   - 添加REF。？

- 参数:
   - `v`: 语音句柄。
   - `p`: REF参数。   
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### voice_start 
`int voice_start(voice_t *v);`

- 功能描述:
   - 语音开始。

- 参数:
   - `v`: 语音句柄。
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### voice_stop
`int voice_stop(voice_t *v);`

- 功能描述:
   - 语音停止。

- 参数:
   - `v`: 语音句柄。
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### voice_backflow_control
`int voice_backflow_control(voice_t *v, voice_backflow_id_t id, int flag);`

- 功能描述:
   - 语音回流控制。
   
#### voice_backflow_id_t
| 枚举 | 说明 |
| :--- | :--- |
| VOICE_MIC_DATA | MIC数据 |
| VOICE_REF_DATA | REF数据 |
| VOICE_VAD_DATA | VAD数据 |
| VOICE_AEC_DATA | AEC数据 |
| VOCIE_BACKFLOW_DATA | 回流数据 |

- 参数:
   - `v`: 语音句柄。
   - `id`: 数据类型。
   - `flag`: 标志，0：关闭回流，1：打开回流。   
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### voice_ai_init 
`voice_t *voice_ai_init(void *priv, voice_cts_ops_t *ops);`

- 功能描述:
   - 智能语音初始化。
#### voice_cts_ops_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| init | int (*init)(void *priv) | 初始化参数 |
| deinit | int (*deinit)(void *priv) | 注销函数 |
| aec | int (*aec)(void *priv, void *mic, void *ref, int ms, void *out) |AEC算法实现 |
| vad | int (*vad)(void *priv, void *mic, void *ref, int ms, void *out) | VAD算法实现 |
| kws | int (*kws)(void *priv, void *mic, void *ref, int ms, void *out) | KWS算法实现 |
| asr | int (*asr)(void *priv, void *vad_data, int ms) | ASR算法实现 |

#### voice_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| mic_param | voice_pcm_param_t * | 麦克风参数 |
| ref_param | voice_pcm_param_t * | REF参数？ |
| param | voice_param_t | 语音参数 |
| ai_ops | voice_evt_t | 事件回调函数 |
| priv | void * | 事件回调函数参数 |
| task | aos_task_t | 任务 |
| sem | aos_sem_t | 信号量 |
| ipc | ipc_t * | IPC |
| aec_time_stamp | long long | AEC时间戳 |
| vad_time_stamp | long long | VAD时间戳 |
| asr_time_stamp | long long | ASR时间戳 |
| state | int | 状态 |
| task_running | volatile int | 任务运行标记 |
| pcm_data | voice_data_t | PCM数据 |
| backflow_data | voice_data_t | 回流数据 |
| backflow_enable | int | 回流使能标记 |
| cts_ms | long int | CTS |
| silence_tmout | int| 断句时间 |
| silence_start | int | 断句开始标记 |
| ch | voice_ch_t | 操作函数 |
| ops | voice_ch_io_t | 操作io函数 |
| pcm | voice_pcm_t | PCM结构 |

#### voice_data_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| type | int | 类型 |
| seq | int | 序列号 |
| flag | int | 标记 |
| len | int | 数据长度 |
| data | 字符数组 |数据 |

- 参数:
   - `cb`: ai算法实现。
   - `priv`: 用户私有数据。   
   
- 返回值:
   - 非空: 语音句柄。
   - 空（NULL）: 失败。

###  voice_ai_deinit
`void voice_ai_deinit(voice_t *v);`

- 功能描述:
   - 智能语音注销。
   
- 参数:
   - `v`: 语音句柄。
   
- 返回值:
   - 无。   
   
### ipc_ch_get
`voice_ch_io_t *ipc_ch_get(void);`

- 功能描述:
   - 获取IPC通道。
   
- 参数:
   - 无。
   
- 返回值:
   - 无。

### voice_malloc
`void *voice_malloc(unsigned int size);`

- 功能描述:
   - 语音节点数据内存申请。
   
- 参数:
   - `size`: 节点数据大小。
   
- 返回值:
   - 非空：语音节点当前数据指针。
   - 空（NULL）：申请失败。

### voice_free
`void voice_free(void *data);`

- 功能描述:
   - 语音节点数据释放。
   
- 参数:
   - `data`: 数据指针。
   
- 返回值:
   - 无。

### pcm_init
`voice_pcm_t *pcm_init(voice_pcm_send send, void *priv);`

- 功能描述:
   - PCM初始化。
   
- 参数:
   - `send`: 发送函数。
   - `priv`: 发送函数参数。   
   
- 返回值:
   - 非空：PCM数据指针。
   - 空（NULL）：申请失败。
   
### pcm_deinit
`void pcm_deinit(voice_pcm_t *p);`

- 功能描述:
   - PCM注销。
   
- 参数:
   - `p`: PCM句柄。
   
- 返回值:
   - 无。

### pcm_mic_config
`void pcm_mic_config(voice_pcm_t *p, voice_pcm_param_t *param);`

- 功能描述:
   - PCM麦克风配置。
   
- 参数:
   - `p`: PCM句柄。
   - `param`: 配置参数指针。   
   
- 返回值:
   - 无。

### pcm_ref_config
`void pcm_ref_config(voice_pcm_t *p, voice_pcm_param_t *param);`

- 功能描述:
   - PCM  REF配置。
   
- 参数:
   - `p`: PCM句柄。
   - `param`: 配置参数指针。   
   
- 返回值:
   - 无。

### pcm_start
`int pcm_start(voice_pcm_t *p);`

- 功能描述:
   - PCM启动。
   
- 参数:
   - `p`: PCM句柄。
   
- 返回值:
   - 无。

## 示例代码

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

## 诊断错误码
无。

## 运行资源
无。

## 依赖资源
  - csi
  - aos
  - minialsa
  - ipc

## 组件参考
无。   
