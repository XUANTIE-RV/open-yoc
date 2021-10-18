# 概述

`ch2601_freertos_demo`是基于RVB2601开发板的最小系统例程，其完成了FreeRTOS的内核移植，两个任务实现交替周期性打印字符串。

## CDK
在CDK的首页，通过搜索ch2601，可以找到ch2601_freertos_demo，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html) 章节


## 启动

烧录完成之后按复位键，串口会有打印输出

```cli
Welcome boot2.0!
build: Mar  5 2021 15:34:41
load img & jump to [prim]
load&jump 0x18017000,0x18017000,59392
xip...
j 0x18017044
[   0.020]<I>INIT Build:Oct 18 2021,17:45:45
[   0.020]<D>app e5

[   0.030]<D>app Hello task1! YoC
[   0.040]<D>app Hello task2! YoC
[   1.040]<D>app Hello task2! YoC
[   1.040]<D>app Hello task1! YoC
[   2.040]<D>app Hello task1! YoC
```
