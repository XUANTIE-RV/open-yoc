## 概述

AV框架是一个轻量级的多媒体开发框架，是AV组件的一部分。其采用典型的4层多媒体模型设计及面向对象的思想开发，使得用户在此基础上易于复用与扩展。若用户需要增加媒体取流格式、扩展媒体解封装格式或增加解码器类型，请参考AV组件适配对接文档。
AV框架主要抽象为四个层次：
媒体接入层：access层，负责媒体数据的来源，可能是file、http、fifo、mem等。
解复用层：demux层，负责把容器里的音视频数据剥离出来，然后分别送给audio/video decoder。
解码层：decoder层，将解码完成后的数据(yuv、pcm)送给audio/video output输出。
输出层：output层，负责将decoder过来的数据呈现/播放出来。

## 组件安装
```bash
yoc init
yoc install av
```

## 配置
无。

## 接口列表
### 音频服务接口media.h
| 函数 | 说明 |
| :--- | :--- |
| aui_player_init | 播放器初始化 |
| aui_player_play | 播放音频 |
| aui_player_seek_play | 播放音频 |
| aui_player_pause | 暂停播放 |
| aui_player_stop | 停止播放 |
| aui_player_seek | 跳转播放 |
| aui_player_vol_adjust | 调整音量 |
| aui_player_vol_set |调整音量到指定值 |
| aui_player_vol_gradual | 渐变调整音量到指定值 |
| aui_player_vol_get | 获取当前音量值 |
| aui_player_set_minvol | 设置播放时最小音量 |
| aui_player_get_state | 获取指定播放器的状态 |
| aui_player_resume_music | 恢复音乐播放 |
| aui_player_config_init | aui_player_config_init |
| aui_player_config | 恢复音乐播放 |
| aui_player_eq_config | 配置参数 |
| aui_player_key_config | 配置获取密钥回调 |
| aui_player_get_time | 获取播放时间 |
| aui_player_get_speed | 获取播放速度 |
| aui_player_set_speed | 设置播放速度 |

### 播放器接口player.h
| 函数 | 说明 |
| :--- | :--- |
| player_init | 播放器初始化 |
| player_conf_init | 播放器配置初始化 |
| player_new | 播放器生成 |
| player_ioctl | 播放器控制 |
| player_play | 播放器播放 |
| player_pause | 播放器暂停 |
| player_resume | 播放器恢复 |
| player_stop | 播放器停止 |
| player_free | 播放器释放 |
| player_seek | 播放器跳转播放 |
| player_get_cur_ptime | 获取播放器当前时间 |
| player_get_media_info | 获取播放器媒体信息 |
| player_get_vol | 获取播放器软件音量 |
| player_set_vol | 设置播放器软件音量 |
| player_get_speed | 获取播放器速度  |
| player_set_speed | 设置播放器速度 |

### 音频输出接口ao.h

| 函数 | 说明 |
| :--- | :--- |
| ao_ops_register | 音频输出操作函数注册 |
| ao_conf_init | 音频输出配置初始化 |
| ao_open | 音频输出打开 |
| ao_control | 音频输出控制 |
| ao_start | 音频输出开始 |
| ao_stop | 音频输出结束 |
| ao_write | 音频输出写入 |
| ao_drain | 音频输出drain |
| ao_close | 音频输出关闭 |

## 接口详细说明

### media_evt_t
`typedef void (*media_evt_t)(int type, aui_player_evtid_t evt_id);`

- 功能描述:
   - 播放器事件用户处理函数。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM）。
   - `evt_id`:  事件类型，取值如下：
      - AUI_PLAYER_EVENT_ERROR
      - AUI_PLAYER_EVENT_START
      - AUI_PLAYER_EVENT_FINISH

- 返回值:
   - 无。

### aui_player_init
`int aui_player_init(utask_t *task, media_evt_t evt_cb);`

- 功能描述:
   - 播放器初始化。

- 参数:
   - `task`: 指定播放器服务的utask，如果等于NULL，播放器自己创建utask。
   - `evt_cb`:  播放器事件回调函数。
   
- 返回值:
   - 0: 成功。
   - -1: 失败。
   
### aui_player_play
`int aui_player_play(int type, const char *url, int resume);`

- 功能描述:
   - 播放音频，强制停止已经在播的音频。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM）。
   - `url`:  媒体资源，url格式的定义形式如下:
      - file:///fatfs0/Music/1.mp3  SD卡中的音频文件
      - http://.../1.mp3            http音频
      - fifo://tts/1           播放云端反馈的tts流
      - mem://addr=%u&size=%u       播放存放在ram中资源
   - `resume`:  自动重启音乐(0：手动 1：自动)。
   
- 返回值:
   - 0: 成功。
   - -1: 失败。
   
### aui_player_seek_play
`int aui_player_seek_play(int type, const char *url, uint64_t seek_time, int resume);`

- 功能描述:
   - 跳转播放。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM）。
   - `url`:  媒体资源，url格式的定义形式如下:
      - file:///fatfs0/Music/1.mp3  SD卡中的音频文件
      - http://.../1.mp3            http音频
      - fifo://tts/1           播放云端反馈的tts流
      - mem://addr=%u&size=%u       播放存放在ram中资源
   - `seek_time`:  跳转的时间。	
   - `resume`:  自动重启音乐(0：手动 1：自动)。
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### aui_player_pause
`int aui_player_pause(int type);`

- 功能描述:
   - 暂停播放。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL）。
   
- 返回值:
   - 0: 成功。
   - 非0: 失败。
   
### aui_player_resume
`int aui_player_resume(int type);`

- 功能描述:
   - 暂停状态继续播放和静音状态恢复播放。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM）。
   
- 返回值:
   - 0: 成功。
   - 非0: 失败。

### aui_player_stop
`int aui_player_stop(int type);`

- 功能描述:
   - 停止播放器。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL）。
   
- 返回值:
   - 0: 成功。
   - 非0: 失败。

### aui_player_seek
`int aui_player_seek(int type, uint64_t seek_time);`

- 功能描述:
   - 播放器跳转播放。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL）。
   - `seek_time`: seek时间，单位ms。
   
- 返回值:
   - 0: 成功。
   - 非0: 失败。

### aui_player_mute
`int aui_player_mute(int type);`

- 功能描述:
   - 播放器静音。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL）。
   
- 返回值:
   - 0: 成功。
   - 非0: 失败。

### aui_player_vol_adjust
`int aui_player_vol_adjust(int type, int inc_volume);`

- 功能描述:
   - 调整音量。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL）。
   - `inc_volume`: 音量，正数加大，负数减小。
   
- 返回值:
   - 0: 成功。
   - 非0: 失败。

### aui_player_vol_set
`int aui_player_vol_set(int type, int volume);`

- 功能描述:
   - 调整音量到指定值。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL）。
   - `volume`: 音量。
   
- 返回值:
   - 0: 成功。
   - 非0: 失败。

### aui_player_vol_gradual
`int aui_player_vol_gradual(int type, int new_volume, int ms);`

- 功能描述:
   - 渐变调整音量到指定值。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM）。
   - `new_volume`: 音量值（0~100）。
   - `ms`: 渐变时间。   
   
- 返回值:
   - 0: 成功。
   - 非0: 失败。

### aui_player_vol_get
`int aui_player_vol_get(int type);`

- 功能描述:
   - 获取当前音量值。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM）。
   
- 返回值:
   - 音量值。
   
### aui_player_set_minvol
`int aui_player_set_minvol(int type, int volume);`

- 功能描述:
   - 设置播放时最小音量。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM,MEDIA_ALL）。
   - `new_volume`: 音量值（0~100）。
   
- 返回值:
   - 0: 成功。
   - 非0: 失败。

### aui_player_get_state
`aui_player_state_t aui_player_get_state(int type);`

- 功能描述:
   - 获取指定播放器的状态。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM）。
   
- 返回值:
   - 播放器的状态。

### aui_player_resume_music
`int aui_player_resume_music(void);`

- 功能描述:
   - 恢复音乐播放（有音量渐变效果）。

- 参数:
   - 无。
   
- 返回值:
   - 0: 成功。
   - 非0: 失败。

### aui_player_config_init
`int aui_player_config_init(aui_player_config_t *config);`

- 功能描述:
   - 播放器配置初始化。配置结构如下。
#### aui_player_config_t
| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| vol_en | uint8_t | 软件音量使能标记，0：未使能，1：使能 |
| vol_index | uint8_t | 软件音量大小，范围为0~255 |
| aef_conf | uint8_t * | AEF配置数据 |
| aef_conf_size | size_t | AEF配置数据大小 |
| resample_rate | uint32_t | 重采样速率 |
| web_cache_size | uint32_t | WEB CHACHE大小，0表示使用缺省值 |
| web_start_threshold | uint32_t | 开始读入的WEB CHACHE门限，范围：0~100，0表示使用缺省值 |
| snd_period_ms | uint32_t | 音频输出每消耗多少ms的数据量来一次中断，0表示使用缺省值 |
| snd_period_num | uint32_t | period_ms的个数 |
| speed | float | 播放速度，建议值范围：0.5~2.0 |
- 参数:
   - `config`: 配置数据指针。
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### aui_player_config
`int aui_player_config(const aui_player_config_t *config);`

- 功能描述:
   - 播放器配置。

- 参数:
   - `config`: 配置数据指针。
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### aui_player_eq_config
`int aui_player_eq_config(eqfp_t *eq_segments, int count);`

- 功能描述:
   - 播放器配置EQ参数。EQ参数结构如下。
#### eqfp_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| enable | uint8_t | 使能标记 |
| type | eqf_type | EQF类型 |
| gain | float | ？ |
| q | float | ？ |
| rate | uint32_t | 速率 |

- 参数:
   - `eq_segments`: EQ段指针。
   - `count`: 段个数。   
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### aui_player_key_config
`int aui_player_key_config(media_key_cb_t cb);`

- 功能描述:
   - 配置获取密钥回调函数。

- 参数:
   - `cb`: 密钥回调函数。函数定义：
   `typedef int (*media_key_cb_t)(const void *in, size_t ilen, void *out, size_t *olen);`
  
- 返回值:
   - 0: 成功。
   - -1: 失败。

### aui_player_get_time
`int aui_player_get_time(int type, aui_play_time_t *t);`

- 功能描述:     
  - 获取播放时间。
  
- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM）。
   - `t`: 播放时间指针。   
  
- 返回值:
   - 0: 成功。
   - -1: 失败。
   
#### aui_play_time_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| duration | uint64_t | 时长，单位:ms |
| curtime | uint64_t | 当前时间，单位:ms |

### aui_player_get_speed
`int aui_player_get_speed(int type, float *speed);`

- 功能描述:
   - 获取播放速度。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM）。
   - `speed`: 播放速度指针。   
  
- 返回值:
   - 0: 成功。
   - -1: 失败。

### aui_player_set_speed
`int aui_player_set_speed(int type, float speed);`

- 功能描述:
   - 设置播放速度。

- 参数:
   - `type`: 播放类型（支持MEDIA_MUSIC,MEDIA_SYSTEM）。
   - `speed`: 播放速度。   
  
- 返回值:
   - 0: 成功。
   - -1: 失败。

### player_init
`int player_init();`

- 功能描述:
   - 播放器模块初始化。
   在调用播放器其他接口之前，要初始化播放器模块。其主要功能为注册取流类型stream、解复用demux、解码器codec、音频输出ao等。
   对于stream，默认注册内存、文件、网络(http/https)、队列fifo取流类型。
   对于demux，默认注册wav、mp3、mp4、adts、flac、asf、amr等解复用格式。
   对于codec，默认注册了mp3、aac等解码器实现。用户可修改解决方案下package.yaml中的CONFIG_DECODER_XXX等配置项。如solutions/pangu_demo/package.yaml中，以下配置项默认可将pcm裸流、mp3解码器(pvmp3)、aac解码器(fdk-aac)注册进去
	```c
	  CONFIG_DECODER_PCM: 1
	  CONFIG_DECODER_PVMP3: 1
	  CONFIG_DECODER_FLAC: 0
	  CONFIG_DECODER_FDK: 1
	  CONFIG_DECODER_ADPCM_MS: 0
	  CONFIG_DECODER_AMRNB: 0
	  CONFIG_DECODER_AMRWB: 0
	  CONFIG_DECODER_IPC: 0
	```
    对于ao，默认注册了类似alsa音频输出实现。对于已上注册类型，用户均可根据实际需要进行裁剪。xx_register_xx系列相关的函数的说明请参考AV中的说明。

- 参数:  
  - 无。   
  
- 返回值:
   - 0: 成功。
   - -1: 失败。
   
### player_conf_init
`int player_conf_init(ply_conf_t *ply_cnf);`

- 功能描述:
   - 播放器配置初始化。初始化配置参数结构体，用于播放器的创建。用户通过该接口获取播放器默认的配置参数后，可根据需要修改相关参数。配置参数由结构体ply_conf_t表示，其包括重采样频率、软件音量、取流超时时间等，详细定义如下：

#### ply_conf_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| ao_name | uint8_t | 音频输出名，用于选择哪一个输出(如果存在多个)，默认为alsa |
| vol_en | uint8_t | 软件音量使能标记，0：未使能，1：使能 |
| vol_index | uint8_t | 软件音量大小(不同于硬件音量调节)，范围为0~255 |
| eq_segments | uint8_t | EQ段数(默认不支持，用户根据需要扩展) |
| aef_conf | uint8_t * | 音效配置数据(默认为索那音效，需商务合作或扩展) |
| aef_conf_size | size_t | 音效配置大小 |
| resample_rate | uint32_t | 非0表示重采样输出到该rate，否则原样输出 |
| rcv_timeout | uint32_t | 取流超时时间(ms)。AOS_WAIT_FOREVER表示一直等，0表示使用播放器默认配置 |
| cache_size | uint32_t | stream层取流缓存大小(byte) |
| cache_start_threshold | uint32_t | 当码流缓存到cache_size的指定百分比时才开始播放，防止网络状态不好时播放时断时续，优化播放效果。取值范围是0~100 |
| period_ms | uint32_t | 音频输出每消耗多少ms的数据量来一次中断 |
| period_num | uint32_t | period_ms的个数。通过period这两个参数可以计算出ao的缓存大小为(period_num * period_ms * (rate / 1000) * 2 * (16/8)) |
| get_dec_cb | get_decrypt_cb_t | 解密回调接口,用于从上层获取秘钥(若流是加密的) |
| event_cb | player_event_t | 播放器事件回调函数 |

对于vol_index，当配置过大时，对于某些码流可能播放存在爆音。用户需根据需要确定音频曲线范围。
对于event_cb，用户需实现下面类型的定义。其中data和len当前作为预留使用
`typedef void (*player_event_t)(player_t *player, uint8_t event, const void *data, uint32_t len);`
播放器当前主要提供了三种事件状态，见下表说明：

| 状态 | 描述 |
| :--- | :--- |
| PLAYER_EVENT_UNKNOWN | 未定义 |
| PLAYER_EVENT_ERROR | 播放出错 |
| PLAYER_EVENT_START | 开始播放 |
| PLAYER_EVENT_FINISH | 播放结束 |
对于get_dec_cb，请参考AV中的stream_conf_init接口说明。

- 参数:
   - `ply_cnf`: 配置数据。   
  
- 返回值:
   - 0: 成功。
   - -1: 失败。

### player_new
`player_t* player_new(const ply_conf_t *ply_cnf);`

- 功能描述:
   - 创建播放器实例。根据配置参数ply_cnf创建播放器实例。该接口与player_free对应。创建后的实例仅能调用player_play播放一个码流。如果需播放第二个码流，则需要调用player_stop停止播放后才能继续调用player_play。
   player_t结构体中的主要成员如下：

#### player_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| url | char * | URL |
| s | stream_cls_t *| 媒体层流指针 |
| demuxer | demux_cls_t * | 解复用器 |
| ad | ad_cls_t * | 音频解码器 |
| ao | ao_cls_t * | 音频输出 |
| ao_name| char * | AO名称 |
| start_time | uint64_t | 起始播放时间 |
| cache_size | uint32_t |  web缓存大小，同player_conf中定义 |
| cache_start_threshold | uint32_t | 缓存阈值，同player_conf中定义 |
| period_ms | uint32_t | 音频输出每消耗多少ms的数据量来一次中断，0表示使用缺省值 |
| period_num | uint32_t | period_ms的个数，同player_conf中定义 |
| resample_rate | uint32_t | 重采样频率，同player_conf中定义 |
| vol_en | uint8_t | 软件音量使能标记，0：未使能，1：使能 |
| vol_index | uint8_t | 软件音量大小，范围为0~255 |
| atempo_play_en | uint8_t | 音频倍数处理使能 |
| speed | float | 速度 |
| aef_conf | uint8_t * | 音效配置数据 |
| aef_conf_size | size_t | 音效配置大小 |
| eq_en | uint8_t | EQ使能 |
| eq_segments | uint8_t | EQ段数 |
| eq_params | eqfp_t | EQ配置参数数组，数组个数等于配置的EQ段数 |
| get_dec_cb | get_decrypt_cb_t | 解密回调，同player_conf中定义 |
| cur_pts | int64_t | 当前播放的present timestamp，用于获取当前播放时间 |
| status | uint8_t | 播放器运行状态 |
| evt | aos_event_t | 事件 |
| evt_status | uint8_t | 播放器事件状态 |
| event_cb | player_event_t | 播放器回调 |
| need_quit | uint8_t | 是否需要退出播放，用于通知stream层接口，防止中途退出播放时长时间阻塞 |
| lock | aos_mutex_t | 互斥锁 |
| stat | struct stat | 统计数据 |

- 参数:
   - `ply_cnf`: 配置数据。   
  
- 返回值:
   - 非空: 成功生成播放器对象。
   - NULL: 生成对象失败。
   
### player_ioctl
`int player_ioctl(player_t *player, int cmd, ...);`

- 功能描述:
   - 播放器控制。通过控制命令字cmd控制播放器的行为。具体的播放器控制命令字如下表所示。
| 状态 | 对应参数类型 | 描述 |
| :--- | :--- | :--- |
| PLAYER_CMD_UNKNOWN | 	—— | 未定义 |
| PLAYER_CMD_EQ_SET_PARAM | peq_setpa_t* | EQ参数配置 |
| PLAYER_CMD_EQ_ENABLE| peq_seten_t* | EQ使能配置 |
| PLAYER_CMD_SET_CACHE_SIZE | uint32_t | 取流缓存大小配置 |
| PLAYER_CMD_SET_RESAMPLE_RATE | uint32_t | 重采样输出配置 |
| PLAYER_CMD_SET_RCVTO | uint32_t | 取流超时时间配置 |
其中，对于PLAYER_CMD_EQ_SET_PARAM和PLAYER_CMD_EQ_ENABLE作为预留，需要自行扩展实现相应EQ接口。
对于PLAYER_CMD_SET_CACHE_SIZE、PLAYER_CMD_SET_RESAMPLE_RATE及PLAYER_CMD_SET_RCVTO配置，若当前播放器未处于停止状态，则配置后下次调用player_play时才会生效。

- 参数:
   - `player`: 播放器。   
   - `cmd`: 命令。
   - `...`: 命令参数。   
  
- 返回值:
   - 0: 成功。
   - -1: 失败。
   
### player_play
`int player_play(player_t *player, const char *url, uint64_t start_time);`

- 功能描述:
   - 码流播放。从指定时间start_time(ms)播放指定url的码流，创建本次播放所需的资源，与player_stop相对应。

- 参数:
   - `player`: 播放器。   
   - `url`:  媒体资源，url格式的定义形式如下:
      - file:///fatfs0/Music/1.mp3  SD卡中的音频文件
      - http://.../1.mp3            http音频
      - fifo://tts/1           播放云端反馈的tts流
      - mem://addr=%u&size=%u       播放存放在ram中资源
   - `start_time`: 开始时间。   
  
- 返回值:
   - 0: 成功。
   - -1: 失败。

- 注意事项：
     - 对于fifo流、直播流或某些索引信息不存在的码流，即使指定start_time，默认也会从起始处开始播放。
     
### player_pause
`int player_pause(player_t *player);`

- 功能描述:
   - 播放器暂停。该接口可暂停播放器的播放，当恢复播放时调用player_resume接口。

- 参数:
   - `player`: 播放器。   
  
- 返回值:
   - 0: 成功。
   - -1: 失败。
   
-  注意事项：
   - 对于fifo流，暂停后，因播放器不在从fifo中读取数据，当写端将fifo写满后会被阻塞住。
   - 通常情况下，对于fifo流，不建议用户调用暂停/恢复播放接口。
   - 若fifo写端是从网络拉取的流，恢复播放后需自行处理写端可能存在的网络重连问题。

### player_resume
`int player_resume(player_t *player);`

- 功能描述:
   - 播放器恢复播放。恢复播放器的播放，与player_pause相对应。对于http码流来说，若长时间暂停播放，设备不会从web服务器拉去流，web服务器通常会主动断开连接。当恢复播放时，取流发现断开连接后，会再次从当前读取位置处重新建立连接拉流。

- 参数:
   - `player`: 播放器。   
  
- 返回值:
   - 0: 成功。
   - -1: 失败。

### player_stop
`int player_stop(player_t *player);`
- 功能描述:
   - 播放器停止。停止播放器的播放，销毁本次播放所创建的资源，其与player_play相对应。当停止播放后可继续调用player_play播放下一个码流。

- 参数:
   - `player`: 播放器。   
  
- 返回值:
   - 0: 成功。
   - -1: 失败。

### player_free
`int player_free(player_t *player);`

- 功能描述:
   - 释放播放器实例。释放播放器相关资源，与player_new相对应。若播放器尚未停止播放，则在free之前会先停止播放。

- 参数:
   - `player`: 播放器。   
  
- 返回值:
   - 0: 成功。
   - -1: 失败。

### player_seek
`int player_seek(player_t *player, uint64_t timestamp);`

- 功能描述:
   - 跳转播放。当播放器处于播放状态或暂停状态时，可跳转到指定时间timestamp(ms)开始播放。跳转成功后，播放器状态不发生改变。

- 参数:
   - `player`: 播放器。   
   - `timestamp`: 时间戳。  
   
- 返回值:
   - 0: 成功。
   - -1: 失败。
   
- 注意事项：
     - 对于fifo流、直播流或某些索引信息不存在的码流，不支持跳转播放。

### player_get_cur_ptime
`int player_get_cur_ptime(player_t *player, play_time_t *ptime);`

- 功能描述:
  
  - 获取播放器当前时间。该接口可用于获取码流的当前播放时间和总时长，均为ms单位。当播放器处于播放或暂停状态时，才可正确调用。对于直播流或某些信息不完整的码流，总时长可能是动态的或无法准确获取到。
  
- 参数:
   - `player`: 播放器。   
   - `ptime`: 时间指针。  
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

#### play_time_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| duration | uint64_t | 时长，单位:ms |
| curtime | uint64_t | 当前时间，单位:ms |

### player_get_media_info
`int player_get_media_info(player_t *player, media_info_t *minfo);`

- 功能描述:  
   
  - 获取媒体信息。用于获取媒体的时长、大小、码率、编码格式、采样率等信息。当播放器处于播放或暂停状态时，才可正确调用。media_info_t结构中的tracks指针，上层应用无需释放。当停止播放时，该指针会被播放器释放掉。
  
- 参数:
   - `player`: 播放器。   
   - `minfo`: 媒体信息指针。  
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

#### media_info_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| tracks | track_info_t | track信息 |
| size | size_t | 大小 |
| bps | uint64_t | 码率 |
| duration | uint64_t | 时长，单位:ms |

### player_get_vol
`int player_get_vol(player_t *player, uint8_t *vol);`
- 功能描述:
   - 获取播放器软件音量。该软件音量仅属于当前播放器，播放器销毁后则不存在。该接口的前提是在创建播放器时使能软件音量配置。软件音量不同于系统音量(硬件音量)。音量的取值范围是0~255。当配置过大时，对于某些码流可能播放存在爆音。用户需根据需要确定音频曲线范围。

- 参数:
   - `player`: 播放器。   
   - `vol`: 音量指针。  
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### player_set_vol
`int player_set_vol(player_t *player, uint8_t vol);`

- 功能描述:
   - 设置播放器软件音量。该软件音量仅属于当前播放器，播放器销毁后则不存在。该接口的前提是在创建播放器时使能软件音量配置。软件音量不同于系统音量(硬件音量)。音量的取值范围是0~255。当配置过大时，对于某些码流可能播放存在爆音。用户需根据需要确定音频曲线范围。

- 参数:
   - `player`: 播放器。   
   - `vol`: 音量，取值范围：0~255。  
   
- 返回值:
   - 0: 成功。
   - -1: 失败。
   
### player_get_speed
`int player_get_speed(player_t *player, float *speed);`

- 功能描述:
   - 获取播放器速度。

- 参数:
   - `player`: 播放器。   
   - `speed`: 速度指针。  
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### player_set_speed
`int player_set_speed(player_t *player, float speed);` 

- 功能描述:
   - 设置播放器速度。

- 参数:
   - `player`: 播放器。   
   - `speed`: 速度。  
   
- 返回值:
   - 0: 成功。
   - -1: 失败。
     
### ao_ops_register
`int ao_ops_register(const struct ao_ops *ops);`

- 功能描述:
  
  - 音频输出操作接口注册。
  
- 参数:  
   - `ops`: 音频输出操作接口。   
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

#### ao_ops

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| name | const char * | 音频输出对象名称 |
| open | int (*open)(ao_cls_t *o, sf_t sf) | 打开函数 |
| start | int (*start)(ao_cls_t *o) | 开始函数 |
| stop | int (*stop)(ao_cls_t *o) | 停止函数 |
| drain | int (*drain)(ao_cls_t *o) | drain函数 |
| close | int (*close)(ao_cls_t *o) | 关闭函数 |
| write | int (*write)(ao_cls_t *o, const uint8_t *buf, size_t count) | 数据写入函数 |

### ao_conf_init
`int ao_conf_init(ao_conf_t *ao_cnf);`

- 功能描述:     
  - 音频输出配置初始化。
  
- 参数:  
   - `ao_cnf`: 音频输出配置指针。   
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

#### ao_conf_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| name | char * | 音频输出对象名称 |
| period_ms | uint32_t | 音频输出每消耗多少ms的数据量来一次中断，0表示使用缺省值 |
| period_num | uint32_t | period_ms的个数，0表示使用缺省值 |
| eq_segments | uint8_t | EQ段数，0表示不需要EQ |
| aef_conf | uint8_t * | AEF配置数据 |
| aef_conf_size | size_t | AEF配置数据大小 |
| resample_rate | uint32_t | 重采样速率 |
| vol_en | uint8_t | 软件音量使能标记，0：未使能，1：使能 |
| vol_index | uint8_t |  软件音量大小，范围为0~255 |
| atempo_play_en | uint8_t | 音频倍数处理使能 |
| speed | float | 速度，建议值范围：0.5~2.0 |

### ao_open
`ao_cls_t* ao_open(sf_t sf, const ao_conf_t *ao_cnf);`

- 功能描述:     
  - 音频输出打开。
  
- 参数:
   - `sf`: 音频采样格式。   
   - `ao_cnf`: 音频输出配置。 
   
- 返回值:
   - 非空: 成功，返回音频输出对象。
   - -空（NULL）: 失败。

#### ao_cls_t

| 成员 | 类型 | 说明 |
| :--- | :--- | :--- |
| sf | sf_t | ？ |
| priv | void * | 私有数据指针 |
| start | uint8_t | 开始标记 |
| interrupt | uint8_t |中断标记 |
| ori_sf | sf_t | ？ |
| period_ms | uint32_t | 音频输出每消耗多少ms的数据量来一次中断，0表示使用缺省值 |
| period_num | uint32_t | period_ms的个数，0表示使用缺省值 |
| resample_rate | uint32_t | 重采样速率 |
| avfc | avfilter_t * |  AVF链指针 |
| avf_vol | avfilter_t * | AVF软音量配置指针 |
| avf_atempo | avfilter_t * | AVF音频倍数处理配置指针 |
| vol_en | uint8_t | 软件音量使能标记，0：未使能，1：使能 |
| vol_index | uint8_t |  软件音量大小，范围为0~255 |
| atempo_play_en | uint8_t | 音频倍数处理使能 |
| speed | float | 速度，建议值范围：0.5~2.0 |
| aef_conf | uint8_t * | AEF配置数据 |
| aef_conf_size | uint32_t | AEF配置数据大小 |
| eq_en | uint8_t | EQ使能 |
| eq_segments | uint8_t | EQ段数，0表示不需要EQ |
| eq_params | eqfp_t * | EQ配置参数数组 |
| avf_eq | avfilter_t * | EQ配置指针 |
| oframe | avframe_t * |  AV帧指针 |
| cnl | mixer_cnl_t * | 合成器指针 |
| ops | struct ao_ops * | 操作函数指针 |
| lock | aos_mutex_t | 互斥锁 |

### ao_control
`int ao_control(ao_cls_t *o, int cmd, void *arg, size_t *arg_size);`

- 功能描述:
   - 音频输出控制。

- 参数:
   - `o`: 音频输出对象。   
   - `cmd`: 控制命令。   
   - `arg`: 命令参数指针。   
   - `arg_size`: 命令参数个数指针。      
   
- 返回值:
   - 0: 成功。
   - -1: 失败。

### ao_start
`int ao_start(ao_cls_t *o);`

- 功能描述:
   - 音频输出开始。

- 参数:
   - `o`: 音频输出对象。   

- 返回值:
   - 0: 成功。
   - -1: 失败。

### ao_stop
`int ao_stop(ao_cls_t *o);`

- 功能描述:
   - 音频输出停止。

- 参数:
   - `o`: 音频输出对象。   

- 返回值:
   - 0: 成功。
   - -1: 失败

### ao_write
`int ao_write(ao_cls_t *o, const uint8_t *buf, size_t count);`

- 功能描述:
   - 写入数据到音频输出。

- 参数:
   - `o`: 音频输出对象。   
   - `buf`: 数据缓冲区。  
   - `count`: 数据缓冲区长度。  

- 返回值:
   - 0: 成功。
   - -1: 失败

### ao_drain
`int ao_drain(ao_cls_t *o);`
- 功能描述:
   - 音频输出drain。？

- 参数:
   - `o`: 音频输出对象。   
  
- 返回值:
   - 0: 成功。
   - -1: 失败

### ao_close
`int ao_close(ao_cls_t *o);`

- 功能描述:
   - 音频输出关闭。

- 参数:
   - `o`: 音频输出对象。   
  
- 返回值:
   - 0: 成功。
   - -1: 失败
   
## 示例代码

### 播放器使用示例
#### 播放网络/SD卡mp3歌曲
```c
//核心代码片段
static player_t* g_player;

static void _player_event(player_t *player, uint8_t type, const void *data, uint32_t len)
{
    UNUSED(len);
    UNUSED(data);

    switch (type) {
    case PLAYER_EVENT_ERROR:
        player_stop(player);
        break;
    case PLAYER_EVENT_START:
        break;
    case PLAYER_EVENT_FINISH:
        player_stop(player);
        break;
    default:
        break;
    }
}

int app_main()
{
    ply_conf_t ply_cnf;

    player_conf_init(&ply_cnf);
    /* 配置音频重采样到48k */
    ply_cnf.resample_rate = 48000;
    /* 配置播放器状态回调函数 */
    ply_cnf.event_cb      = _player_event;
    /* 根据配置参数创建播放器示例 */
    g_player = player_new(&ply_cnf);

    /* 网络mp3歌曲 */
    char *url = "http://www.baidu.com/xx.mp3";
    /* 文件/sd卡mp3歌曲 */
    //char *url = "file:///fatfs0/test.MP3";
    /* 播放指定url的音频文件 */
    player_play(g_player, url, 0);
}
```

#### 播放内存中的pcm裸数据
```c
//核心代码片段
//按照上面示例中的代码，将url修改为mem://前缀并加入相关参数即可播放内存中的pcm裸数据
char url[128];
char *pcm_data = 0x123456;//pcm数据存放地址
size_t pcm_len = 1024;//pcm数据大小

snprintf(url, sizeof(url), "mem://addr=%u&size=%u&avformat=rawaudio&avcodec=pcm_s16le&channel=1&rate=16000", pcm_data, pcm_len);//pcm裸数据播放需要在url中指定avformat、avcodec、channel、rate参数
player_play(g_player, url, 0);
```

#### 语音合成(TTS）流播放
用户自行下载网络流，通过fifo传递给播放器播放。fifo可用于TTS的播放。
```c
//核心代码片段
static player_t* g_player;
static nsfifo_t* g_tts_fifo;

static void _ptask(void *arg)
{
    int fd;
    int cnt = 0, rc, wlen;
    char *val, *pos;
    uint8_t reof = 0;
    web_session_t *session;
    /* 创建一个http会话 */
    session = web_session_create();
    /* 向指定地址发起http请求 */
    rc = web_session_get(session, "http://www.srcbin.net/ai/result.mp3", 3);
    if (rc) {
        LOGE(TAG, "web_session_get fail. rc = %d, code = %d, phrase = %s", rc, session->code, session->phrase);
        goto err;
    }
    /* 获取内容的长度 */
    val = (char*)dict_get_val(&session->hdrs, "Content-Length");
    CHECK_RET_TAG_WITH_GOTO(val != NULL, err);
    fd = session->fd;
    LOGD(TAG, "content len = %d", atoi(val));

    for (;;) {
        /* 获取fifo的可写指针及长度 */
        wlen = nsfifo_get_wpos(g_tts_fifo, &pos, 8*1000);
        /* 获取播放器fifo读端是否退出(可能播放出错) */
        nsfifo_get_eof(g_tts_fifo, &reof, NULL);
        if (wlen <= 0 || reof) {
            LOGE(TAG, "get wpos err. wlen = %d, reof = %d", wlen, reof);
            break;
        }
        /* 从网络套接字中读取数据 */
        rc = sock_readn(fd, pos, wlen, 6*1000);
        if (rc <= 0) {
            LOGE(TAG, "readn err. rc = %d", rc);
            break;
        }
        /* 设置写指针 */
        nsfifo_set_wpos(g_tts_fifo, rc);
        cnt += rc;
    }
    LOGD(TAG, "rc = %8d, cnt = %8d", rc, cnt);

err:
    /* 销毁web会话资源 */
    web_session_destroy(session);
    return;
}
/* 创建fifo，指定fifo地址和大小 */
g_tts_fifo = nsfifo_open("fifo://tts/1", O_CREAT, 64*1024);
if (g_tts_fifo) {
    /* 创建 _ptask任务，用于通过网络获取码流数据 */
    aos_task_new("xx_task", _ptask, NULL, 6*1024);
    /* 设置播放器取流超时时间 */
    player_ioctl(g_player, PLAYER_CMD_RCVTIMEO, AOS_WAIT_FOREVER);
    /* 播放之前创建的fifo码流 */
    player_play(g_player, "fifo://tts/1", 0);
}
```

## 诊断错误码
无。

## 运行资源
无。

## 依赖资源
  - csi
  - aos
  - vfs
  - mbedtls
  - flac
  - amrnb
  - amrwb
  - pvmp3dec
  - opus
  - sonic
  - speex
  - speexdsp

## 组件参考
无。
