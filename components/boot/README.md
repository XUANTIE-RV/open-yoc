## 概述

系统升级组件。包括以下功能：

- 引导启动
- 镜像验签
- 镜像升级

## 配置

### C库相关配置

bootloader自行实现一个非常小的C库，无需minilibc或者newlib组件。
当使用ARM或者RV工具链时，需要定义这个宏为1。否则无需在应用重新定义。

CONFIG_NEWLIB_STUB: 1

### 无需升级功能

CONFIG_NO_OTA_UPGRADE: 1

### 无需差分升级功能

CONFIG_OTA_NO_DIFF: 1

### 无需验签功能

CONFIG_PARITION_NO_VERIFY: 1

### 安全区域设置支持

是否支持设置安全区域。
CONFIG_NOT_SUPORRT_SASC: 1

### KEY获取

是否从eFuse中获取KEY。
CONFIG_TB_KP: 1

## 接口列表

| 函数 | 说明 |
| :--- | :--- |
| boot_main | boot入口函数 |
| boot_load_and_jump | 跳转到下一级镜像  |
| boot_sys_reboot | 系统复位接口 |

## 接口详细说明

### boot_main

`int boot_main(int argc, char **argv);`

- 功能描述:
   - bootloader的入口函数。

- 参数:
   - `argc`: 参数个数。
   - `argv`: 参数数值。

- 返回值:
   - 0: 成功。
   - < 0: 失败。

### boot_load_and_jump

`void boot_load_and_jump(void);`

- 功能描述:
   - 跳转到下一级镜像。是一个weak类型的函数，用户可以自己实现。

- 参数:
   - 无

- 返回值:
   - 无

### boot_sys_reboot

`void boot_sys_reboot(void);`

- 功能描述:
   - 系统复位的接口。是一个weak类型的函数，用户可以自己实现。

- 参数:
   - 无

- 返回值:
   - 无

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

  - partition

## 组件参考

无。

