# 简介

av是一个轻量级的多媒体AV框架，方便用户在此基础上复用与扩展。其按照典型的播放器模型设计，主要抽象为四个层次:
**媒体接入层：**access层，负责媒体数据的来源，可能是file、http、rtp等。

**解复用层：**demux层，负责把容器里的音视频数据剥离出来，然后分别送给audio/video  decoder。

**解码层：**decoder层，将解码完成后的数据(yuv、pcm)送给audio/video output输出。

**输出层：**output层，负责将decoder过来的数据呈现/播放出来。
如果把数据想象成流水的话，每层的功能虽然不同，但是他们大致抽象的功能都是接收上个模块过来的数据,然后加工并把加工后的数据送到下一个模块。把上述这些层通过某种方式连接起来，就形成了一个音频播放器。

# 示例代码
## 媒体接入层

媒体接入层当前支持内存流、文件流、http(s)网络流、队列流(tts播放可基于此实现)

```c
/**
 * @brief  regist stream for memory
 * @return 0/-1
 */
int stream_register_mem();

/**
 * @brief  regist stream for file
 * @return 0/-1
 */
int stream_register_file();

/**
 * @brief  regist stream for http
 * @return 0/-1
 */
int stream_register_http();

/**
 * @brief  regist stream for fifo
 * @return 0/-1
 */
int stream_register_fifo(); 
```

## 解复用层

解复用层当前支持wav、mp3、mp4、rawaudio(裸流)、adts等格式。

```c
/**
 * @brief  regist demux for wav
 * @return 0/-1
 */
int demux_register_wav();

/**
 * @brief  regist demux for mp3
 * @return 0/-1
 */
int demux_register_mp3();

/**
 * @brief  regist demux for mp4
 * @return 0/-1
 */
int demux_register_mp4();

/**
 * @brief  regist demux for rawaudio
 * @return 0/-1
 */
int demux_register_rawaudio();

/**
 * @brief  regist demux for adts
 * @return 0/-1
 */
int demux_register_adts();
```

## 解码层

解码层支持pcm解码(裸流)、ipc解码(核间解码)、pvmp3解码(mp3)等编码格式的支持。

```c
/**
 * @brief  regist ad for pcm
 * @return 0/-1
 */
int ad_register_pcm();

/**
 * @brief  regist ad for ipc
 * @return 0/-1
 */
int ad_register_ipc();

/**
 * @brief  regist ad for pvmp3
 * @return 0/-1
 */
int ad_register_pvmp3();

```

## 输出层

输出层当前仅支持alsa(mini_alsa,轻量级alsa标准接口实现，用来屏蔽不同芯片codec的差异)输出

```c
/**
 * @brief  regist audio output for alsa
 * @return 0/-1
 */
int ao_register_alsa();
```
