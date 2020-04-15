# 概述

`pangu_demo` 是一个基于`pangu`芯片，包含语音交互和媒体播放的语音demo

# 编译

```bash
make clean;make
```

# 烧录

```bash
make flashall
```

# 命令

## 网络配置

### 设置wifi ssid和密码

```
kv set wifi_ssid CSKY-T
```

```
kv set wifi_psk test1234
```

设置完毕重启开发板

### 设置mac地址

```
iwpriv wifi_debug set_mac 00e04c87A080
```

设置完毕重启开发板

## OTA配置

配置完成需要重启开发板

### FOTA服务器设置

```
kv set otaurl http://pre.occ.t-head.cn/api/image/ota/pull
```

### FOTA使能配置

```
kv setint fota_en 0/1
```

### FOTA检测间隔(MS)

```
kv setint fota_cycle 10000
```

### model设置

```
kv set model pangu
```

### deivce_id设置

```
kv set device_id b405d54804400000ba11a0ca8d90db0c
```

## ASR服务器配置

```
kv set asr_server 192.168.1.102:8090
```

## 测试命令

注意：**所有中文的输入要求时UTF8**

### ai tts text

读文本
text为文本信息，可以是英文和中文，有空格可以使用引号
```
ai tts hello，”从前有座山 山里有座庙 庙里有个小和尚说”
```

### ai talk music:singer+songname
交互的一个特殊功能，用户获取mp3资源，并播放
```
ai talk music:周杰伦+菊花台
```

### ai talk text
一般的交互信息

```
ai talk “我想听 邓紫棋 的光年之外”
```

### ai music url[http://]
播放http协议

```
ai music http://zhangmenshiting.qianqian.com/data2/music/b945b1d2be8542bd895a56a6b92e963c/594654945/3008959933600128.mp3?xcode=b2dff42de18e306e9824035d6041d0a0
```

### ai notify [0-6]
播放通知音
```
ai notify 0
ai notify 1
```

### ai vol +/-/[0-100]
音调整，自动识别当前场景

```
ai vol -   降低音量
ai vol +  增加音量
ai vol 40  调整到40%
```

### app micrec […]
录制mic采集的数据到指定的服务器，服务器软件在`../pangu_rec_demo/server`目录下，具体使用见readme.md

- 开始录制，会同时录制3路的pcm数据，1麦1参考1路处理后数据
```
app micrec start ws://192.168.1.102:9090 savename.pcm
```

- 停止录制
```
app micrec stop
```

### Wifi测试命令
具体见`../wifihalapp`例子
