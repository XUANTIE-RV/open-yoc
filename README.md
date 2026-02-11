# XuanTie RTOS SDK

## 介绍

RTOS-SDK是基于CSI接口规范开发的一套软件SDK。其中，CSI（chip software interface）是玄铁自研的嵌入式软件开发接口，包含CSI-Core、CSI-kernel、CSI-DSP、CSI-Driver，接口与ARM CMSIS兼容，可实现从ARM到玄铁的快速软件迁移。软件SDK基于上述CSI接口，对接上述的Smart硬件平台，进一步实现了对常见RTOS系统的支持以及与CPU相关的开发实例程序。客户可以参考SDK中集成的示例程序进行应用开发，可大幅提高开发效率；进一步的，SDK支持多种工程编译环境，包括CDK IDE 工程，Makefile工程等。


## 参考资料

- 玄铁官方网站: https://www.xrvm.cn/community/download?id=4455469730437861376

## 许可证

RTOS SDK开源代码遵循Apache License 2.0开源许可协议，可以免费在商业产品中使用，并且不需要公开私有代码。

## Release Note

### v2.4.3
#### 主要特性更新
1. 修复 E906/E907 运行 FreeRTOS 时偶现卡住的问题

#### 依赖工具版本
| **工具** | **版本号** |
| --- | --- |
| CDK | V2.24.14 |
| CDS | V5.2.31 |
| Debug Server | V5.18.5 |
| Xuantie-900-gcc-elf-newlib编译工具链 | V3.2.0 |
| Xuantie-900-llvm-elf-newlib编译工具链 | V2.2.1 |
| Xuantie QEMU | V5.2.8 |


### v2.4.2
#### 主要特性更新
1. C908X VLEN512/1024/4096支持
2. boot from itcm示例增加对E907系列处理器的支持
3. 增加对E907/E906系列处理器非矢量中断中断咬尾功能的支持
4. E902/E906/E907系列处理器，freeRTOS的port.c中补充vPortSuppressTicksAndSleep函数实现，以满足cpusleep功能的实现
5. csi组件增加csi_rv_encoding.h文件，包含了标准CSR以及玄铁扩展CSR的描述

#### 依赖工具版本
| **工具** | **版本号** |
| --- | --- |
| CDK | V2.24.14 |
| CDS | V5.2.31 |
| Debug Server | V5.18.5 |
| Xuantie-900-gcc-elf-newlib编译工具链 | V3.2.0 |
| Xuantie-900-llvm-elf-newlib编译工具链 | V2.2.1 |
| Xuantie QEMU | V5.2.8 |


### v2.4.1
#### 主要特性更新
1. 解决 E 系列处理器在频繁中断嵌套下会出现异常的问题
2. 解决 E902 系列处理器运行 perf record 功能时会卡住的问题

#### 依赖工具版本
| **工具** | **版本号** |
| --- | --- |
| CDK | V2.24.14 |
| CDS | V5.2.31 |
| Debug Server | V5.18.5 |
| Xuantie-900-gcc-elf-newlib编译工具链 | V3.2.0 |
| Xuantie-900-llvm-elf-newlib编译工具链 | V2.2.1 |
| Xuantie QEMU | V5.2.8 |


### v2.4.0
#### 主要特性更新
1. 增加 E901/E901Plus 系列处理器型号支持
2. 支持 Codesize 优化的 minilibc 库
3. 增加非矢量中断中断咬尾示例
4. 增加 boot from itcm 示例

#### 依赖工具版本
| **工具** | **版本号** |
| --- | --- |
| CDK | V2.24.13 |
| CDS | V5.2.29 |
| Debug Server | V5.18.5 |
| Xuantie-900-gcc-elf-newlib编译工具链 | V3.2.0 |
| Xuantie-900-llvm-elf-newlib编译工具链 | V2.2.1 |
| Xuantie QEMU | V5.2.7 |


### v2.3.0
#### 主要特性更新
1. freeRTOS升级至V11.2.0版本，支持SMP功能
2. perf功能增加对freeRTOS系统的支持
3. perf功能新增对E906/E907处理器的支持
4. rtthread_helloworld 增加rt-thread utest/testcases/kernel下所有case的支持
5. 改用Makefile构建方式，移除对package.yaml文件和yoctools构建工具的依赖
6. mcu/soc类型sample重命名，mcu前缀直接去掉，soc前缀改为osal
7. 增加r908-cp-xt/r908fd-cp-xt/r908fdv-cp-xt/c910v3-cp-xt/c920v3-cp-xt型号支持
8. 增加bare_pie_demo地址无关示例，支持c908/c908i/c908v处理器
9. 裸系统内存管理模块从libc_bare组件中剥离出来，成为单独组件mm，并解决某些情况下内存申请越界问题
10. 支持使用llvm工具链编译
11. 《玄铁RTOS SDK用户手册》更新至v2.3.0版本

#### 依赖工具版本
| **工具** | **版本号** |
| --- | --- |
| CDK | V2.24.11 |
| CDS | V5.2.27 |
| Debug Server | V5.18.5 |
| Xuantie-900-gcc-elf-newlib编译工具链 | V3.1.0 |
| Xuantie-900-llvm-elf-newlib编译工具链 | V2.1.0 |
| Xuantie QEMU | V5.2.6 |


### v2.2.0
#### 主要特性更新
1. perf stat功能支持，支持玄铁所有PMU事件的统计
2. perf record功能支持，统计信息支持文本和火焰图展示
3. 优化中断和任务上下文切换时保存寄存器的时间
4. 增加RT-Thread栈回溯的调试打印输出
5. 更新CDK/CDS工程，简化用户操作步骤
6. 《玄铁RTOS SDK用户手册》更新至v2.2.0版本

#### 依赖工具版本
| **工具** | **版本号** |
| --- | --- |
| CDK | V2.24.5 |
| CDS | V5.2.18 |
| Debug Server | V5.18.3 |
| Xuantie-900-gcc-elf-newlib编译工具链 | V3.0.1 |
| Xuantie QEMU | V5.0.5 |
| yoctools构建工具 | 2.0.81 |


### v2.1.0
#### 主要特性更新
1. 新增玄铁处理器R908、C920V3的支持
2. 增加TCM ECC 2BIT校验相关CASE
3. 修复使用玄铁GCC14工具链编译的一些警告问题
4. 用户使用文档更新至v2.1.0版本

#### 依赖工具版本
| **工具** | **版本号** |
| --- | --- |
| CDK | V2.24.4 |
| CDS | V5.2.20 |
| Debug Server | V5.18.3 |
| Xuantie-900-gcc-elf-newlib编译工具链 | V3.0.1 |
| Xuantie QEMU | V5.0.4 |
| yoctools构建工具 | 2.0.95 |
