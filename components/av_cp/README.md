## 概述

av_cp是一个轻量级的多核异构核间多媒体AV库，与av(运行在应用程序处理器侧)配合使用。该库屏蔽了底层核间通信的细节，应用开发人员可直接基于该库使用芯片的多核异构解码、重采样、fft等运算能力。

## 组件安装
```bash
yoc init
yoc install av_cp
```

## 配置

## 接口列表
| 函数 | 说明 |
| :--- | :--- |
| ad_icore_cp_init | 协处理器音频解码器初始化 |
| aef_icore_cp_init | 协处理器音效初始化 |
| atempo_icore_cp_init | 协处理器atempo初始化 |
| eq_icore_cp_init | 协处理器音频均衡器初始化 |
| fft_icore_cp_init | 协处理器FFT初始化 |
| mca_icore_cp_init | 协处理器MCA初始化 |
| res_icore_cp.h | 协处理器重采样初始化 |


## 接口详细说明

### ad_icore_cp_init
`int ad_icore_cp_init();`

- 功能描述:
   -  协处理器音频解码器初始化。

- 参数:
   - 无。

- 返回值:
   - 0: 成功。
   - -1: 失败。

### aef_icore_cp_init
`int aef_icore_cp_init();`

- 功能描述:
   -  协处理器音效初始化。

- 参数:
   - 无。

- 返回值:
   - 0: 成功。
   - -1: 失败。
   
### atempo_icore_cp_init
`int atempo_icore_cp_init();`

- 功能描述:
   -  协处理器atempo初始化。

- 参数:
   - 无。

- 返回值:
   - 0: 成功。
   - -1: 失败。   

### eq_icore_cp_init
`int eq_icore_cp_init();`

- 功能描述:
   -  协处理器均衡器初始化。

- 参数:
   - 无。

- 返回值:
   - 0: 成功。
   - -1: 失败。

### fft_icore_cp_init
`int fft_icore_cp_init();`

- 功能描述:
   -  协处理器FFT初始化。

- 参数:
   - 无。

- 返回值:
   - 0: 成功。
   - -1: 失败。
   
### mca_icore_cp_init
`int mca_icore_cp_init();`

- 功能描述:
   -  协处理器MCA初始化。

- 参数:
   - 无。

- 返回值:
   - 0: 成功。
   - -1: 失败。
 
### res_icore_cp.h
`int res_icore_cp.h();`

- 功能描述:
   -  协处理重采样初始化。

- 参数:
   - 无。

- 返回值:
   - 0: 成功。
   - -1: 失败。

## 示例


## 诊断错误码
无。

## 运行资源
无。

## 依赖资源
  - csi
  - aos
  - av

## 组件参考
无。




