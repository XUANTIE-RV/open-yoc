## 概述

算法处理的最后一个节点，提供上云和关键词数据。

## 组件安装
```bash
yoc init
yoc install alg_dispatch
```
## 接口

### 获取算法处理后，准备上云处理做ASR或NLP的语音数据

```C
int voice_get_pcm_data(void *data, int len);
```
* 参数
  * data: 获取数据的缓冲，数据格式单路16bit，PCM数据
  * len: 读取的长度
* 返回值
  * 实际读取的长度

### 获取关键词PCM数据
```C
int voice_get_kws_data(void *data, int len);
```
* 参数
  * data: 获取数据的缓冲，数据格式单路16bit，PCM数据
  * len: 读取的长度
* 返回值
  * 实际读取的长度

## 示例
无

## 运行资源
无
