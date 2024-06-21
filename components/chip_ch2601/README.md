## 概述

CH2601芯片组件，主要提供CH2601芯片外设驱动代码。

### 功能

包含芯片外设基地址、中断号等信息定义

包含UART、IIC、SPI、Flash等外设CSI驱动代码

包含CPU启动文件

包含芯片初始化功能

包含芯片异常处理

包含芯片复位功能

## 组件安装

```bash
yoc init
yoc install chip_ch2601
```

## 接口

见[CSI接口描述](https://www.xrvm.cn/document?temp=csi&slug=csi-chn)
