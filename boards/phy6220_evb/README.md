# 概述

奉加微PHY6220板级配置组件。包括PHY6220开发版本Pin脚定义，使用的固件生成脚本，flash烧录脚本，和默认鉴权密钥。

## 配置

```c
开发板RGB三色灯pin脚定义
#define PIN_LED_R  P23
#define PIN_LED_G  P31
#define PIN_LED_B  P32

开发板UART口定义
#define CONSOLE_UART_IDX  0
#define CONSOLE_TXD       P9
#define CONSOLE_RXD       P10
```

# 接口列表

无

# 示例

无

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

  - chip_phy6220
  - drv_bt_phy6220

## 组件参考

无。



