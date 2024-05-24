# YoC

## 介绍

YoC是一个基于AliOS Things的基础软件平台。它为开发人员提供了统一的芯片CSI接口。还提供针对蓝牙、WiFi、语音、视觉应用等优化的组件。可以通过IDE(集成开发环境剑池CDK)进行开发，并使用系统性能分析工具来调试、开发和部署功能组件。它有助于SoC公司和开发人员快速交付定制的SDK，可以显著缩短产品上市时间。

## 架构

<img src="https://github.com/T-head-Semi/open-yoc/blob/v7.9.0/components/yoc/resources/yoc.png?raw=true">

- 内核与驱动： RVM CSI 层定义了 片上系统外设的统一接口，芯片对接完成RVM CSI接口，就可以支持 YoC 的软件系统。OSAL/POSIX接口提供了内核统一接口，集成了AliOS Things、FreeRTOS以及RT-Thread内核。RVM HAL为上层各类组件提供了统一的硬件抽象接口。
- 基础组件： 包含了设备管理框架、低功耗管理、高级安全可信计算（TEE）、网络协议、蓝牙协议栈、虚拟文件系统、网络管理器等。核心服务层采用独立模块化设计，用户可以根据应用需求，自由组装。
- 领域子系统： YoC 提供多种应用领域中软件框架，进一步简化应用方案的开发难度。目前YoC 平台中提供了智能语音框架、视觉 AI 框架、图形应用框架。

YoC 平台定义了芯片的统一接口，提供应用最基础的核心服务，提供了大量独立的应用组件，软件架构清晰、系统模块化并且可裁剪性非常好。针对芯片移植需求，只需要根据芯片驱动接口层（CSI）的定义，即可以将YoC 移植到该芯片上。针对资源受限的微控制器（MCU）系统，也可以裁剪出只需要几 KB 的 FLASH，几 KB的内存消耗的系统。对于资源丰富的物联网设备，YoC 提供可定制的核心服务，支持 OSAL API/POSIX API， 以及丰富的独立组件，提供更加面向领域的业务框架。

## 开发板

### RVB2601开发板

RVB2601是基于玄铁生态芯片CH2601的开发板，板载JTAG调试器，WiFi&BLE芯片W800，音频ADCES7210，音频DACES8156，128x64 OLED屏幕，RGB三色指示灯，用户按键，及兼容Arduino的扩展接口。

具体开发板硬件规格及信息请到[RVB2601开发板](https://occ.t-head.cn/vendor/detail/index?spm=a2cl5.14300867.0.0.681c1f9cxK233N&id=3886757103532519424&vendorId=3706716635429273600&module=4).

开发板快速上手手册请参考[RVB2601应用开发实战系列一: Helloworld最小系统](https://occ.t-head.cn/community/post/detail?spm=a2cl5.14300867.0.0.713b180fsACOZd&id=3887622217227972608)。

### d1_dock_pro开发板

Lichee D1 Dock Pro是一款功能集成度较高的RISC-V Linux开发套件。它板载128Mbit SPI FLASH及MIPI、RGB屏幕接口，拥有丰富的外设（2.4G Wi-Fi+BT模块、双数字麦克风和音频功放等），支持HDMI输出，同时还提供USB-JTAG和USB-UART功能。

具体开发板硬件规格及信息请到[d1_dock_pro开发板](https://occ.t-head.cn/vendor/detail/index?spm=a2cl5.26076654.0.0.1c7f180fYWbuLz&id=4030046623349878784).

开发板快速上手手册请参考[YoC RTOS 实战：lichee D1 dock 开发板快速上手教程](https://occ.t-head.cn/community/post/detail?spm=a2cl5.26076654.0.0.6dea180f5EqKus&id=4035432797019185152)

### bl606p开发板

BL606P-DVK专为用于评估音视频应用诞生，适用于智能面板、智能音箱、语音故事机等设备，适用于各类音视频和 AIoT 应用的开发。

具体开发板硬件规格及信息请到[BL606P-DVK开发板](https://occ.t-head.cn/vendor/detail/index?spm=a2cl5.26076654.0.0.4112180fes3RoI&id=4105634631926222848).

开发板快速上手手册请参考[BL606P 开发板上手手册](https://occ.t-head.cn/community/post/detail?spm=a2cl5.27438731.0.0.4e28180fT0sZG1&id=4106009187719385088)

### 华山派开发板

华山派 CV1812H开发板，是算能和生态伙伴硬件十万个为什么联合推出的开源生态开发板，为广大使用者提供基于RISC-V的开源开发环境，以视觉、AI场景开发为核心实现功能，芯片更集成第二代全自研深度学习张量处理器（TPU），全自研智能图像处理引擎（Smart ISP），硬件级别高度安全资料保护架构（Security），语音处理引擎及H.264/265智能编解码技术，同时还有配套的多媒体软件平台和IVE硬件加速接口，使AI部署与执行更加高效、快速、便捷。主流的深度学习框架，比如Caffe，Pytorch，ONNX ，MXNet和TensorFlow(Lite) 框架的主流网络可以轻松的移植。

具体开发板硬件规格及信息请到[华山派开发板](https://xuantie.t-head.cn/product/board/4212612498014146560?spm=a2cl5.26076654.0.0.347f4aa84husd3).

开发板快速上手手册请参考[华山派开发板上手手册](https://xuantie.t-head.cn/document?temp=huashan-school-development-board-handbook&slug=yocbook)

## 示例列表

使用之前请先安装[yoctools](https://xuantie.t-head.cn/document?temp=yoctools&slug=yocbook)编译构建工具。

试用示例之前请先使用以下命令进行下载:

```bash
git clone git@github.com:T-head-Semi/open-yoc.git -b v7.9.0
```

或者从GITEE下载：
```bash
mkdir yocworkspace
cd yocworkspace
yoc init
yoc install yoc -b v7.9.0
```

然后到`solutions`目录下，所有的示例都在那里，用户可以根据每个示例下的`README.md`文件进行操作。
通用示例如下：
| **分类** | **示例名称** | **说明** |
| --- | --- | --- |
| QEMU | bare_core_dsp | 一个简单的基于baremetal的玄铁exx系列P扩展使用示例。 |
|  | bare_core_ecc | 一个简单的基于baremetal的玄铁cpu ECC校验使用示例。 |
|  | bare_core_vector | 一个简单的基于baremetal的玄铁cpu vector使用示例。 |
|  | bare_core_matrix | 一个简单的基于baremetal的玄铁cpu matrix使用示例。 |
|  | bare_core_lockup | 一个简单的基于baremetal的玄铁exx cpu锁定lockup使用示例。 |
|  | bare_coremark | 一个简单的基于baremetal的coremark示例。coremark是一种基准测试套件，用于评估嵌入式系统处理器的性能，全面测试CPU、内存、编译器性能，不涵盖浮点运算、IO、网络等方面。 |
|  | bare_core_nmi | 一个简单的基于baremetal的玄铁exx cpu非屏蔽中断使用示例。 |
|  | bare_core_tcm | 一个简单的基于baremetal的玄铁cpu tcm功能使用示例。 |
|  | bare_core_vic | 一个简单的基于baremetal的玄铁cpu中断控制器使用示例. |
|  | bare_core_wfe | 一个简单的基于baremetal的玄铁exx cpu低功耗唤醒wfe功能使用示例。 |
|  | bare_core_wfi | 一个简单的基于baremetal的玄铁exx cpu低功耗唤醒wfi功能使用示例。 |
|  | bare_cpp_demo | 一个简单的基于baremetal的c++使用示例。 |
|  | bare_dhrystone | 一个简单的基于baremetal的dhrystone示例。dhrystone是一种基准测试程序，用于评估处理器的速度和效率。其主要用于测试整数运算速度和效率，不涵盖浮点运算、IO、网络等方面。 |
|  | bare_drv_timer | 一个简单的基于baremetal的定时器驱动裸驱示例(基于csi2.0规范). |
|  | bare_drv_uart | 一个简单的基于baremetal的串口驱动裸驱示例(基于csi2.0规范).  |
|  | bare_helloworld | 一个简单的基于baremetal的helloword demo，可运行在QEMU环境. |
|  | bare_linpack_dp | 一个简单的基于baremetal的linpack_dp示例。linpack_dp是一种用于评估计算机系统浮点运算性能的基准测试程序，用于测试双精度浮点运算性能。 |
|  | bare_linpack_sp | 一个简单的基于baremetal的linpack_sp示例。linpack_sp是一种用于评估计算机系统浮点运算性能的基准测试程序，用于测试单精度浮点运算性能。 |
|  | bare_whetstone | 一个简单的基于baremetal的whetstone示例。whetstone是一种基准测试程序，用于评估处理器的浮点运算性能，不涵盖整数运算、IO、网络等方面。 |
|  | mcu_freertos_helloworld | 一个面向mcu领域的基于freertos的helloword demo |
|  | mcu_freertos_event | 一个面向mcu领域的基于freertos的event使用示例 |
|  | mcu_freertos_message_q | 一个面向mcu领域的基于freertos的message queue 使用示例 |
|  | mcu_freertos_mutex | 一个面向mcu领域的基于freertos的mutex使用示例 |
|  | mcu_freertos_sem | 一个面向mcu领域的基于freertos的sem使用示例 |
|  | mcu_freertos_task | 一个面向mcu领域的基于freertos的task使用示例 |
|  | mcu_freertos_time | 一个面向mcu领域的基于freertos的time 使用示例  |
|  | mcu_freertos_timer | 一个面向mcu领域的基于freertos的timer 使用示例 |
|  | mcu_freertos_cpp | 一个面向mcu领域的基于freertos的cpp demo |
|  | mcu_rtthread_helloworld | 一个面向mcu领域的基于rtthread的helloword demo |
|  | mcu_rtthread_event | 一个面向mcu领域的基于rtthread的event demo |
|  | mcu_rtthread_message_q | 一个面向mcu领域的基于rtthread的message queue demo |
|  | mcu_rtthread_mutex | 一个面向mcu领域的基于rtthread的mutex demo |
|  | mcu_rtthread_sem | 一个面向mcu领域的基于rtthread的sem demo |
|  | mcu_rtthread_task | 一个面向mcu领域的基于rtthread的task demo |
|  | mcu_rtthread_time | 一个面向mcu领域的基于rtthread的time demo |
|  | mcu_rtthread_timer | 一个面向mcu领域的基于rtthread的timer demo |
|  | mcu_rtthread_cpp | 一个面向mcu领域的基于rtthread的cpp demo |
|  | mcu_rtthread_smp | 一个面向mcu领域的基于rtthread的smp demo |
|  | soc_helloworld | 一个面向soc领域(使用osal封装)的helloworld示例，可运行在玄铁QEMU环境. |
|  | soc_smp_demo | 一个面向soc领域(使用osal封装)的smp demo |
|  | soc_core_dsp | 一个面向soc领域(使用osal封装)的玄铁cpu dsp多线程使用示例 |
|  | soc_core_vector | 一个面向soc领域(使用osal封装)的玄铁cpu vector多线程使用示例 |
|  | soc_core_matrix | 一个面向soc领域(使用osal封装)的玄铁cpu matrix多线程使用示例 |
|  | soc_kernel_event | 一个面向soc领域(使用osal封装)的event demo |
|  | soc_kernel_message_q | 一个面向soc领域(使用osal封装)的message_q demo |
|  | soc_kernel_mutex | 一个面向soc领域(使用osal封装)的mutex demo |
|  | soc_kernel_sem | 一个面向soc领域(使用osal封装)的sem demo |
|  | soc_kernel_task | 一个面向soc领域(使用osal封装)的task demo |
|  | soc_kernel_time | 一个面向soc领域(使用osal封装)的time demo | 
|  | soc_kernel_timer | 一个面向soc领域(使用osal封装)的timer demo | 
|  | soc_cpp_demo | 一个面向soc领域(使用osal封装)的cpp demo | 
| 最小系统 | helloworld | 最小系统Helloworld例程，例程将创建一个任务实现周期性hello world日志输出。 |
| bootloader例程 | ch2601_boot | 基于CH2601平台的bootloader实例，完成下一级镜像的验签、跳转到下一级镜像和升级等功能。SDK中提供了其他已支持芯片平台的bootloader适配例程，可通过实例名字选择。比如：bl606p_boot -  BL606P平台、d1_boot_demo - D1平台、cv181xh_boot - CV181xH平台 |
| CLI命令行 | cli_demo | 串口命令行输入示例。串口命令行一般包括调试命令， 测试命令，获取系统信息命令，控制LOG信息打印等，用户也可以根据需要增加调测指令。 |
| 键值对 | kv_demo | KV文件系统是基于Flash的一种Key-Value 数据存储系统，支持断电保护、磨损均衡、坏块处理等功能。用户可通过CLI指令测试KV功能。 |
| 固件升级 | fota_demo | FOTA升级的DEMO。FOTA的云服务在OCC，包括固件的管理，许可证的管理，设备的管理等。 |
| 媒体播放 | xplayer_demo | 媒体播放器示例，用户可通过CLI指令测试播放器功能。 |
|  | codec_demo | Codec裸驱示例，用户可通过CLI指令测试Codec功能。 |
| 设备驱动 | devices_demo | devices_demo是devices组件下rvm_hal接口的使用示例。 |
| USB协议栈 | usb_demo | usb_demo展示了USB协议栈接口的使用方法。 |
|  | gprs_demo |  移远EC200A USB 4G模组适配实例，可参考实现RNDIS和USB Serial两类USB Class。 |
| 核间通信 | ipc_demo | 核间通信IPC在AP核的示例，主要实现了AP侧作为服务端日志和CLI指令代理功能。 |
|  | bl606p_c906_ipc_demo | 核间通信IPC在CP核的示例，主要实现了CP侧作为客户端日志和CLI指令功能。 |
| 图形引擎 | lvgl_demo | 基于LVGL开源图形库的示例程序，适配了官方提供的Stress/Widgets/Benchmark/Music四个示例程序。用户可修改代码选择不同示例程序运行。 |
| 网络连接 | wifi_demo | WiFi驱动示例，用户可通过CLI指令测试Scan、Station模式、AP模式等功能，已适配RTL8723 / BL606P / W800 等平台。 |
|  | httpclient_demo | httpclient组件使用示例，用户可通过CLI指令测试HTTP和HTTPS的访问以及HTTP Get功能。 |
|  | wifi_ble_provisioning_demo | 基于蓝牙协议栈实现蓝牙辅助配网功能的简单示例程序。 |
|  | bt_audio_demo | 基于蓝牙协议栈实现蓝牙音频播放功能的示例程序。 |
|  | ble_shell_demo | 蓝牙协议栈Shell测试程序，solution里带ble前缀的示例都是蓝牙BLE的开发例程，可以根据名字选择相应例程了解。比如：ble_hrs_demo - 展示HRS Profile功能、ble_hid_keyboard_demo - 展示HID设备基本功能、ble_scanner_demo - 中心设备开发实例、ble_uart_profile_client_demo - UART透传服务Client实例、ble_uart_profile_server_demo - UART透传服务Server实例  |
|  | mesh_shell | 蓝牙Mesh协议栈Shell测试程序，solution里带mesh前缀的示例都是蓝牙Mesh协议栈的开发例程，可以根据名字选择相应例程了解。比如：mesh_provisioner_demo - 蓝牙Mesh配网器实例、mesh_light_node_demo - Mesh智能灯设备实例、mesh_switch_node_demo - Mesh智能开关设备实例、mesh_switch_node_lpm_demo - Mesh低功耗设备实例、mesh_temperature_sensor_demo - Mesh低功耗设备实例、mesh_body_sensor_demo - Mesh低功耗设备实例 |

## 参考资料

- 玄铁开源社区: https://www.xrvm.cn/

- 玄铁开源社区文档中心：[文档中心](https://www.xrvm.cn/document-index?spm=a2cl5.14290816.0.0.b0b3mSsmmSsm3Z)

- YoC使用手册yocbook: [yocbook](https://www.xrvm.cn/document?temp=yoc-platform-overview&slug=yocbook)

## 许可证

YoC系统完全开源，代码版本遵循Apache License 2.0开源许可协议，可以免费在商业产品中使用，并且不需要公开私有代码。

## Release Note

### 2023.5.21
#### 新增特性
1. 新增RT-Thread内核，并支持SMP功能
2. FreerRTOS升级到v10.4.3 LTS版本
3. 增加CPU RTOS SDK，支持RV系列CPU
4. CSI CPU相关接口更新

#### 开发工具
|开发工具|版本|说明|
| --- | --- | --- |
|[CDK](https://www.xrvm.cn/community/download?id=4313363375687012352)|>=V2.24.2|面向基于RISC-V的MCU/AIoT领域的极简开发环境，帮助开发者从工程获取到配置、开发、下载、调试全链路的图形界面的支持。|
|[CDS](https://www.xrvm.cn/community/download?id=616214177698021376)|>=V5.2.18|面向基于玄铁CPU的集成开发环境，支持Bare metal、RTOS、Linux等各类程序的开发。|
|[QEMU](https://www.xrvm.cn/community/download?id=4300747789186437120)|>=V4.2.9|玄铁QEMU是一个以开源项目QEMU为基础，支持玄铁处理器的软件模拟器，提供了带基本外设的玄铁开发板模板。|
|[XuanTie Debug Server](https://www.xrvm.cn/community/download?id=4313368247333359616)|>=V5.18.1|XuanTie DebugServer是一个调试代理软件，它通过CKLINK，以JTAG的方式连接目标板，并支持“GDB Remote Protocol” 。开发者可以通过GDB对运行在玄铁800系列和玄铁900系列处理器上的软件程序进行在线调试。|
|[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)|>=2.0.76|Linux下命令行构建编译工具。|
|[玄铁工具链](https://www.xrvm.cn/community/download?id=4308603453615181824)|V2.10.0|riscv编译所需工具链，可通过yoc命令安装(yoc toolchain --riscv -f)；也可以自行下载安装，并在系统变量中设置对应的路径信息。

### 2023.8.15
#### 新增特性
1. 完善设备驱动
2. 新增CSI2D图像加速库
3. 支持MMU，虚拟地址映射
4. 支持S态运行，M/S态相互切换
5. 完善安全子系统
6. 完善语音子系统
7. 完善视频视觉子系统
8. 完善图形子系统
#### 新增开发板
1. 支持华山派开发板

### 2023.3.11
#### 新增特性
1. 设备驱动框架新增显示设备、块设备的支持
2. 支持CherryUSB协议栈
3. 支持eMMC存储及引导
4. 支持EXT4文件系统
5. 支持固件压缩打包功能
6. 支持基于IoT小程序的JS图形应用开发
7. 新增智慧面板解决方案，支持LVGL图形应用开发
#### 新增芯片
1. 支持晶视CV181x芯片
#### 其他更新
1. LVGL版本更新至v8.3.1
2. 接入移远EC200A USB 4G模组

### 2022.10.20
1. QEMU平台支持更多的RISC-V CPU， 具体请参考`riscv_dummy_demo`
2. 增加FOTA对AB分区升级的支持，具体可参考`fota_demo`
3. 提供更多通用示例供开发者快速上手
4. 硬件平台支持CH2601/PHY6220/W800/D1/BL606P
#### 开发工具
|开发工具|版本|说明|
| --- | --- | --- |
|[CDK](https://xuantie.t-head.cn/soft-tools/tools/4197790929093988352?spm=a2cl5.14290816.0.0.5911jOGIjOGIaJ)|>=V2.22.0|集成开发环境IDE|
|[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)|>=2.0.56|Linux下命令行构建编译工具|
|[玄铁工具链](https://occ.t-head.cn/community/download?id=4090445921563774976)|V2.6.1|riscv编译所需工具链，可通过yoc命令安装(yoc toolchain --riscv -f)；也可以自行下载安装，并在系统变量中设置对应的路径信息|
