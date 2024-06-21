视频视觉应用平台（ChiXiao Lite）以 YoC（Yun On Chip）实时系统作为基座，针对轻量级应用，提供面向领域的多媒体及AI服务，使得用户可快速迭代，落地产品。典型应用场景包括：猫眼门锁、考勤机、消费类IPC等智能产品。

# 平台特性

1、基于轻量级高实时的RTOS系统
● YoC操作系统内核，丰富的基础组件
● 轻量全栈开销，典型视觉产品资源消耗：Flash < 16 MB，RAM < 64 MB
● 百毫秒级启动出图
2、轻量级视觉AI服务
● 支持离线目标识别算法，包括目标检测、Landmark检测、特征值计算、单目活检、双目活检等模型
● 支持AI算子CPU向量加速
● 支持算法扩展定制，可适配多方算法
3、上云服务
● 支持基于阿里云LinkVisual的远程直播、对讲功能
● 提供端云一体完整解决方案，支持移动终端远程控制
4、录像服务
● 支持音视频流录制和存储、图片抓拍
● 支持本地存储空间管理
● 支持录像文件检索

# 平台架构

![image.png](./resource/frame_chixiao.png)



平台架构如上图所示，图中各层释义如下：
● 解决方案：提供基于特定功能或特定领域的参考软件解决方案
● 视频视觉应用服务：多媒体及AI服务平台，提供录像、直播上云、目标识别等核心服务
● 多媒体库：高性能、跨平台多媒体库，对上提供编解码、音视频复用、模型推理等基础能力集
● 基础组件：提供基于操作系统的基础功能组件，如文件系统、网络驱动、命令行工具等
● 芯片平台：芯片底层驱动层，对接芯片硬件



# 平台手册

在线文档：[[在线链接](https://www.xrvm.cn/document?temp=stp6yg14re1q6wbb&slug=video-vision-application-platform)]



# 版本说明

[[Release Notes](https://www.xrvm.cn/document?temp=oypviaqc542rtakq&slug=video-vision-application-platform)]

