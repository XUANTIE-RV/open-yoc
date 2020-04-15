# 概述

OMA是一家国际组织，最初定义了一套 OMA-DM的协议，用来远程管理移动终端设备，比如手机开户，版本升级，等等。OMA-DM有着非常广泛的应用，很多运营生比如Verizon Wireless, Sprint都有自己的OMA-DM服务并要求手机/模块入网的时候通过自定义的OMA-DM入网测试。因为物联网的兴起， OMA在传统的OMA-DM协议基础之上，提出了LWM2M协议。2013年底，OMA发布了LWM2M规范。

OMA Lightweight M2M 主要动机是定义一组轻量级的协议适用于各种物联网设备，因为M2M设备通常是资源非常有限的嵌入式终端，无UI, 计算能力和网络通信能力都有限。同时也因为物联网终端的巨大数量，节约网络资源变得很重要。

LWM2M 定义了三个逻辑实体:
- LWM2M Server 服务器
- LWM2M client 客户端 负责执行服务器 的命令和上报执行结果
- LWM2M 引导服务器 Bootstrap server 负责 配置LWM2M客户端.

# 参考资料

[wakaama](https://github.com/eclipse/wakaama)

