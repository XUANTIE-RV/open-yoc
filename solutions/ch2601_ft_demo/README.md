

# 概述

`ch2601_ft_demo` 是工厂测试的demo程序，用于演示工程测试程序的开发，功能包含LED亮灯、LED关灯、亮屏测试等。本文档描述支持CH2601开发板基本功能测试需求。功能测试通过AT指令执行测试.



# 下载

首先安装yoc工具，参考YoCBook CDK开发快速上手 章节， 链接 https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html

下载ch2601_ft_demo代码命令如下：

```bash
yoc init

yoc install ch2601_ft_demo
```

下载后代码位于solutions/ch2601_ft_demo



# 配置

无



# 接口

AT+BTN命令处理函数：

```c
void cmd_ft_btn_handler()
```

AT+LCD命令处理函数：

```c
void cmd_ft_lcd_handler()
```
AT+WIFI命令处理函数：

```c
void cmd_ft_wifi_handler()
```
AT+MIC命令处理函数：

```c
void cmd_ft_mic_handler()
```



## 诊断错误码

无。



## 运行资源

无。



## 依赖资源

无。



# 编译

目录solutions/ch2601_ft_demo下执行

```bash
make
```



# 烧录

目录solutions/ch2601_ft_demo下执行

```bash
make flashall
```



# 启动

烧录完成之后按复位键，串口会有打印输出



# 命令

## BUTTON测试
发送以下AT命令
```bash
AT+BTN
```
之后按下第一个键, RGB LED灯交替亮。按下另一个键，RGB LED灯熄灭。

## TFT LCD 亮屏测试
发送以下AT命令后屏幕全亮。
```bash
AT+LCD
```

## WIFI测试

发送以下AT命令后开始wifi测试。

```bash
AT+WIFI
```

## mic测试。

发送以下AT命令后开始麦克风左声道测试。

```bash
AT+MICL
```
发送以下AT命令后开始麦克风右声道测试。

```bash
AT+MICR
```


## 组件参考

无。