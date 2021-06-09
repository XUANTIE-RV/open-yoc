# 概述

基于`ch2601`芯片的离线音频播放DEMO。音频文件存储在设备的FLASH中，由于存储介质空间有限，存储的音频不是很长。

# 使用

## 通过CDK

在CDK的首页，通过搜索ch2601，可以找到CH2601_PLAYER_DEMO，然后创建工程。

CDK的使用可以参考YoCBook 《CDK开发快速上手》 章节， 链接 https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html

具体步骤参考OCC上的博文《RVB2601应用开发实战系列五: 网络播放器设计(一)》
## 通过命令行

### 下载

```bash
mkdir workspace
cd workspace
yoc init
yoc install ch2601_player_demo
```

### 编译

```bash
make clean;make
```

### 烧录

```bash
make flashall
```

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit_debug
```


# 运行

烧写完毕之后按下复位键即可看到打印信息。

```cli
Welcome boot2.0!
build: Mar  5 2021 15:34:41
load img & jump to [prim]
load&jump 0x18017000,0x18017000,247380
xip...
j 0x18017044
[   0.020]<I>INIT [YoC]Build:Mar  8 2021,17:42:45
[   0.020]<I>INIT find 6 partitions
[   0.590]<D>atempo regist affecter, name = atempo_sonic
[   0.600]<I>player player_new, 161 enter.
[   0.620]<I>player player_new, 201 leave. player = 2000CC48
[   0.640]<I>player player_play, 569 enter. player = 2000CC48
[   0.650]<D>demux_mp3 mp3 probe, sync_cnt = 0, rc = 0, score = 100
[   0.660]<I>player player_play, 589 leave. player = 2000CC48
[   0.660]<I>demux find a demux, name = mp3, url = mem://addr=402955324&size=23220
[   0.670]<D>avparser get avparser ops err, id = 1
[   0.680]<I>ad find a decode, name = pvmp3dec, id = 1
[   0.690]<D>filter_vol open a avfilter, name = vol
[   0.700]<D>ao_alsa  ao open
[   0.710]<D>ao ao ref: openref =  1, startref =  0, fun = __ao_open
[   0.710]<D>ao ori sf ==> sf = 32768146, rate = 16000, ch = 1, bits = 16, siged = 1, float = 0, endian = 0
[   0.720]<D>ao ao  sf ==> sf = 32768146, rate = 16000, ch = 1, bits = 16, siged = 1, float = 0, endian = 0
[   1.000]<D>ao ao ref: openref =  1, startref =  1, fun = __ao_start
[   1.000]<D>player_demo =====_player_event, 19, type = 2
[   1.010]<D>player player_get_media_info, 809 enter. player = 2000CC48
[   1.020]<D>player player_get_media_info, 821 leave. player = 2000CC48
[   1.020]<D>player_demo =====rc = 0, duration = 2902ms, bps = 64011, size = 23220
[   1.060]<I>player first frame output
[   4.070]<D>stream read ret = 0, may be read eof, len = 180. 'mem://addr=402955324&size=23220'
[   4.140]<D>stream read ret = 0, may be read eof, len = 0. 'mem://addr=402955324&size=23220'
[   4.150]<I>demux read packet may be eof. eof = 1, rc = 0, url = mem://addr=402955324&size=23220
[   4.160]<D>player cb run task quit
[   4.160]<D>player_demo =====_player_event, 19, type = 3
[   4.170]<I>player player_stop, 699 enter. player = 2000CC48
[   4.540]<D>ao ao ref: openref =  1, startref =  0, fun = __ao_stop
[   4.550]<D>ao ao ref: openref =  0, startref =  0, fun = __ao_close
[   4.560]<I>stream stream stat: to_4000ms = 0, to_2000ms = 0, to_1000ms = 0, to_500ms = 0, to_200ms = 0, to_100ms = 0, to_50ms = 0, to_20ms = 0, to_other = 193, cache_full = 0, upto_cnt = 0, rsize = 0, size = 23220, url = mem://addr=402955324&size=23220
[   4.600]<I>player player_stop, 710 leave. player = 2000CC48
```

# 播放控制

## 停止播放

通过串口终端输入：

```cli
player stop
```

## 播放本地音频

**开始播放之前需要先停止上一次的播放**

通过串口终端输入：

```cli
player play welcom
player play raw
```

# 音频文件

此demo播放的音频内容存储在app/include/welcome_mp3.h这个文件中。可以将这个mp3的数据内容进行修改。

