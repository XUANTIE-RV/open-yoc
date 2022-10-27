# YoC

## 介绍

YoC是一个基于AliOS Things的基础软件平台。它为开发人员提供了统一的芯片CSI接口。还提供针对蓝牙、WiFi、语音、视觉应用等优化的组件。可以通过集成开发环境(IDE)剑驰CDK进行开发，并使用系统性能分析工具来调试、开发和部署功能组件。它有助于SoC公司和开发人员快速交付定制的SDK，可以显著缩短产品上市时间。


## 开发板

### RVB2601开发板

RVB2601是基于平头哥生态芯片CH2601的开发板，板载JTAG调试器，WiFi&BLE芯片W800，音频ADCES7210，音频DACES8156，128x64 OLED屏幕，RGB三色指示灯，用户按键，及兼容Arduino的扩展接口。

具体开发板硬件规格及信息请到[RVB2601开发板](https://occ.t-head.cn/vendor/detail/index?spm=a2cl5.14300867.0.0.681c1f9cxK233N&id=3886757103532519424&vendorId=3706716635429273600&module=4).

开发板快速上手手册请参考[RVB2601应用开发实战系列一: Helloworld最小系统](https://occ.t-head.cn/community/post/detail?spm=a2cl5.14300867.0.0.713b180fsACOZd&id=3887622217227972608)。

### PHY6220开发板

PHY6220是奉加微电子最新推出的超低功耗系统级蓝牙芯片，搭载平头哥玄铁CK802 CPU，可配置128KB-8MB Flash；超低功耗，RX/TX峰值电流低至4.6/4mA@3.3V；拥有全套自主知识产权协议栈，支持多通信标准协议，可以满足客户的定制化需求；异构多对多的蓝牙Mesh网络，实现基于BLE的大规模物联网；接收灵敏度-97dBm@1Mbps/-103dBm@125Kbps；支持高速OTA升级和AoA/AoD定位功能。

具体开发板硬件规格及信息请到[PHY6220开发板](https://occ.t-head.cn/vendor/detail/index?spm=a2cl5.26076654.0.0.75c9180fBd3xmo&id=3844141476569686016).

开发板快速上手手册请参考[PHY6220开发板快速上手手册](https://occ-oss-prod.oss-cn-hangzhou.aliyuncs.com/userFiles/3712904037927702528/resource/3712904037927702528hfGSSiepmB.pdf)


### W800开发板

W800开发板是一款基于IoT Wi-Fi/蓝牙SoC芯片W800的调试板卡，兼容Arduino接口。支持作为Wi-Fi/蓝牙模块与其他MCU主板今天对接调试，也支持作为主控设备进行开发调试。

具体开发板硬件规格及信息请到[W800开发板](https://occ.t-head.cn/vendor/detail/index?spm=a2cl5.26076654.0.0.47b8180fQDHEWX&id=3819785372310183936).

开发板快速上手手册请参考[CB6201开发板快速上手手册](https://occ-oss-prod.oss-cn-hangzhou.aliyuncs.com/userFiles/3717897501090217984/resource/371789750109021798416243697382898DSDaM3TmQ.pdf);
[蓝⽛MESH⽹关开发板快速上⼿⼿册](https://occ-oss-prod.oss-cn-hangzhou.aliyuncs.com/userFiles/3814675761722499072/resource/3814675761722499072/1636114574507/%E8%93%9D%E7%89%99MESH%E7%BD%91%E5%85%B3%E5%BC%80%E5%8F%91%E6%9D%BF%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%89%8B%E5%86%8C.pdf)

### d1_dock_pro开发板

Lichee D1 Dock Pro是一款功能集成度较高的RISC-V Linux开发套件。它板载128Mbit SPI FLASH及MIPI、RGB屏幕接口，拥有丰富的外设（2.4G Wi-Fi+BT模块、双数字麦克风和音频功放等），支持HDMI输出，同时还提供USB-JTAG和USB-UART功能。

具体开发板硬件规格及信息请到[d1_dock_pro开发板](https://occ.t-head.cn/vendor/detail/index?spm=a2cl5.26076654.0.0.1c7f180fYWbuLz&id=4030046623349878784).

开发板快速上手手册请参考[YoC RTOS 实战：lichee D1 dock 开发板快速上手教程](https://occ.t-head.cn/community/post/detail?spm=a2cl5.26076654.0.0.6dea180f5EqKus&id=4035432797019185152)

### bl606p开发板

BL606P-DVK专为用于评估音视频应用诞生，适用于智能面板、智能音箱、语音故事机等设备，适用于各类音视频和 AIoT 应用的开发。

具体开发板硬件规格及信息请到[BL606P-DVK开发板](https://occ.t-head.cn/vendor/detail/index?spm=a2cl5.26076654.0.0.4112180fes3RoI&id=4105634631926222848).

开发板快速上手手册请参考[BL606P 开发板上手手册](https://occ.t-head.cn/community/post/detail?spm=a2cl5.27438731.0.0.4e28180fT0sZG1&id=4106009187719385088)


## 示例列表

使用之前请先安装[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)编译构建工具。

试用示例之前请先使用以下命令进行下载:

```bash
git clone git@github.com:T-head-Semi/open-yoc.git -b v7.6.0
```

或者从GITEE下载：
```bash
mkdir yocworkspace
cd yocworkspace
yoc init
yoc install yocopen -b v7.6.0
```

然后到`solutions`目录下，所有的示例都在那里，用户可以根据每个示例下的`README.md`文件进行操作。
通用示例如下：
| 名称 | 说明 |
| --- | --- |
| helloworld | 最小系统Helloworld例程，其完成了AliOS Things的内核移植，最后在一个任务是实现周期性打印hello world的字符串 | 
| cli_demo | 串口命令行输入示例。串口命令行一般包括调试命令， 测试命令，获取系统信息命令，控制LOG信息打印等，其允许用户可以根据自己的需要增加命令，从而达到快速开发测试的命令。在本示例里主要集成了以下CLI命令，通过help命令可以List出所有支持的CLI命令。 |
| kv_demo | 最小KV文件系统功能的操作例程。KV文件系统是基于Flash的一种Key-Value 数据存储系统，该系统采用极小的代码及内存开销（最小资源 rom：3K bytes，ram：100bytes），在小规模的Flash上实现数据的存储管理能力，支持断电保护、磨损均衡、坏块处理等功能。KV文件系统存储系统支持只读模式与读写模式共存，只读模式可以用于工厂生产数据，读写模式可用于运行时的数据存存储。 |
| fota_demo | FOTA升级的DEMO。FOTA的云服务在OCC，包括固件的管理，许可证的管理，设备的管理等。 |
| wifi_demo | 一个简单的WiFi连接示例。 |
| httpclient_demo | 一个简单的httpclient组件使用示例。httpclient是一个开源的http客户端，支持HTTP和HTTPS的访问。 |
| xplayer_demo | 一个xplayer播放器框架命令行使用示例。 |
| aoshal_demo | aoshal组件下驱动的使用示例。 |
| codec_demo | csi codec 测试demo, 只测试minialsa所需相关csi codec api。 |
| ipc_demo | 是核间通信ipc在AP侧的demo，主要实现了AP侧作为ipc的服务端、客户端、输出和ipc的cli的功能。 |
| arm_dummy_demo | 一个简单的helloword demo，运行在QEMU环境。支持ARM的CPU。|
| csky_dummy_demo | 一个简单的helloword demo，运行在QEMU环境。支持CSKY的CPU。 |
| riscv_dummy_demo | 一个简单的helloword demo，运行在QEMU环境。支持RISC-V的CPU。 |

## 参考资料

- 平头哥开源社区: https://occ.t-head.cn/

- yoc使用手册yocbook: [yocbook](https://yoc.docs.t-head.cn/yocbook/Chapter1-YoC%E6%A6%82%E8%BF%B0/)

## Release Note

### 2022.10.20
1. QEMU平台支持更多的RISC-V CPU， 具体请参考`riscv_dummy_demo`
2. 增加FOTA对AB分区升级的支持，具体可参考`fota_demo`
3. 提供更多通用示例供开发者快速上手
4. 硬件平台支持CH2601/PHY6220/W800/D1/BL606P
#### 开发工具
|开发工具|版本|说明|
| --- | --- | --- |
|[CDK](https://occ.t-head.cn/community/download?id=575997419775328256)|>=V2.16.2|集成开发环境IDE|
|[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)|>=2.0.42|Linux下命令行构建编译工具|
|[玄铁工具链](https://occ.t-head.cn/community/download?id=4090445921563774976)|V2.6.1|riscv编译所需工具链，可通过yoc命令安装(yoc toolchain --riscv -f)，也可以自行下载安装，并集成到系统变量|
