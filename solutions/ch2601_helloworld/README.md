# 概述

`helloworld`是基于RVB2601开发板的最小系统Helloworld例程，其完成了AliOS Things的内核移植，最后在一个任务是实现周期性打印Helloworld的字符串。

# 使用

## CDK

双击打开当前目录下的`project.cdkproj`。
如何安装和使用CDK请参考YoCBook [《CDK开发快速上手》](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html) 章节。

# 启动

烧录完成之后按复位键，串口会有打印输出

```cli
Welcome boot2.0!
build: Mar  5 2021 15:34:41
load img & jump to [prim]
load&jump 0x18017000,0x18017000,73232
xip...
j 0x18017044
[   0.020]<I>[INIT]<app_task>Build:Oct 18 2021,16:01:56
[   0.020]<D>[app]<app_task>e5

[   0.040]<D>[app]<app_task>Hello world! YoC
[   1.040]<D>[app]<app_task>Hello world! YoC
[   2.040]<D>[app]<app_task>Hello world! YoC
[   3.040]<D>[app]<app_task>Hello world! YoC
[   4.040]<D>[app]<app_task>Hello world! YoC
[   5.040]<D>[app]<app_task>Hello world! YoC
```
