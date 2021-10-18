

# 概述

`ch2601_marquee_demo` 示例程序通过控制一颗RGB LED灯珠实现三色跑马灯效果。该程序基于CH2601开发板，用于介绍如何在CH2601上开发外设应用程序以及如何使用LED灯。

三色LED通过PIN引脚 PA7、PA25、PA4与Ch2601主芯片GPIO/PWM控制器连通，该程序软件通过GPIO及PWM两种方式控制LED灯。使用前确认CH2601开发板LED pin位置三个跳线帽已插入，三个跳线帽分别连接J3 pin3-pin4， J3 pin5-pin6， J3 pin7-pin8。

GPIO方式下，控制器以拉高拉低电压方式控制LED灯开关，通过每间隔固定的时间分别打开某色LED灯，并相应关闭其他两色LED灯，从而实现跑马灯效果。在控制GPIO前需要设置PA7/PA25/PA4引脚PINMUX复用为GPIO方式。

PWM方式下，可以实现效果更加丰富的呼吸灯效果。通过三路PWM通路分别控制RBG三色灯，实现有呼吸效果的跑马灯程序。在控制PWM通路前需要设置PA7/PA25/PA4引脚PINMUX复用为PWM方式。



# 下载

首先安装yoc工具，参考YoCBook [《CDK开发快速上手》](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html) 


# 配置

无

# 接口

该demo程序使用csi组件 gpio、pwm接口。请参考csi组件。
程序中主要函数如下：
初始化pinmux函数：

```c
led_pinmux_init();
```


定时刷新函数：

```c
void led_refresh()
```

开发者可修改以上函数实现不同的跑马灯效果。如刷新时间、灯光组合、呼吸效果等。

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

无。


# 启动

烧录完成之后按复位键，LED灯会有三色跑马灯效果。

## 组件参考

无。