# 概述

W800是一颗适用于物联网领域的Wi-Fi/蓝牙双模 SoC 芯片，适用于智能家电、智能家居、智能玩具、无线音视频、工业控制、医疗监护等广泛的物联网领域。

本文介绍基于W800芯片的CB6201开发板的环境搭建及如何使用开发板进行网络连接并连接到生活物联网平台。通过本文的指引，开发者可以学会开发环境的搭建、SDK 的编译与烧录、基本调试方法，快速上手 W800 的开发。

# 环境搭建

windows及linux下环境搭建参考[W800快速上手手册](https://occ-oss-prod.oss-cn-hangzhou.aliyuncs.com/userFiles/3717897501090217984/resource/3717897501090217984jKGhhefnxh.pdf)

# 编译

在linux下编译执行

```bash
make clean;make
```

使用CDK则直接点击编译按钮。

注：CONFIG_GW_FOTA_EN 为 FOTA 开关宏，默认为关，如需开启请将其置为 1。

# 烧录

**使用CK-Link烧录**

在linux环境下烧录执行

```bash
make flashall
```

烧录成功后，可以看到所有分区烧写进度都已至100%。

```
Program partition: prim         address: 0x80d0400, size 664816 byte
erasing...
program 08172400, 100%
Program partition: imtb         address: 0x81c0000, size 4096 byte
erasing...
program 0081c1000, 100%
```

使用CDK则直接点击烧录下载按钮。

**使用串口烧录**

CB6201开发板支持不通过CK-Link而是直接通过串口进行烧录。按下CB6201开发板上BOOT按键后同时按RST按键，此时UART0输出CCCCC，表明进入烧录模式。然后通过串口工具（如SecureCRT）选取Xmodem方式将位于out/smart_lighting_w800目录下生成的yoc_total.fls文件烧录到开发板。

#### 网关添加设备

##### 扫描设备

```
gw scan_dev 1
/*查找到设备*/
->[GATEWAY_MESH]<timer_task>Found mesh dev:11:11:11:11:11:11,addr_type:00,oob_info:00,beraer:01
```

##### 关闭扫描

```
gw scan_dev 0
```

##### 添加设备

```
gw add_dev dev_type dev_info
dev_type:为设备类型，当前只支持Mesh设备，类型码为0x00;
dev_info:设备信息，对于Mesh设备信息为:addr addr_type uuid oob_info bearer
如:
gw add_dev 0 11:11:11:11:11:11 0 11111111111111e886d15f1ce200de02 0 1
/*添加成功，设备编号为1*/
->Active check node add index:1 0
```

#### 开关设备

```
/*控制设备编号为dev_id的设备开关*/
gw subdev_onoff <dev_id> 0/1
```

# 启动

烧录完成之后按复位键，串口会有打印输出。

首次上电时，开发板没有配置 WiFi 用户名密码，无法连接无线路由器，此时，需要使用APP对设备进行配网。

以本SDK默认的BLE辅助配网为例，配网过程如下：

* 长按CB6201开发板User按键2s以上进入配网模式

* 打开**云智能APP**，扫描产品二维码（具体参考[W800快速上手手册](https://occ-oss-prod.oss-cn-hangzhou.aliyuncs.com/userFiles/3717897501090217984/resource/3717897501090217984jKGhhefnxh.pdf)）
* 输入待配网AP的SSID/KEY并开始配网

配网完成后，APP上会出现创建的产品。

开发板目前支持三种配网方式：

* BLE辅助配网（默认配网方式）

* 一键配网

* 设备热点配网。

其中，BLE辅助配网为默认配网方式。如需切换配网方式，则需在串口0手动输入CLI命令输入指令：

```
prov 0    //切换为一键配网
prov 1    //切换为设备热点配网
prov 2    //切换为BLE辅助配网
```

> 注意，切换配网方式后，在生活物联网平台对应产品的配网方式也要做相应切换，否则会因为配网方式不匹配导致配网失败。

> 另外需要注意的是，SDK中对配网设置了120s的超时，配网超时后设备会退出配网状态。

配网原理及开发部分可参考[验证蓝牙辅助配网](https://help.aliyun.com/document_detail/155235.html?spm=a2c4g.11186623.6.1072.7ba96ed9puZnjb#title-2e1-plf-5z9) 及[WIFI配网概述](https://help.aliyun.com/document_detail/97570.html?spm=a2c4g.11186623.6.631.5fe66525Dmbbv0) 。

##### 5.3.2 连接到网络

开发板获取到路由器的SSID/KEY之后，就会发起连接。连接成功后，我们可以观察到串口打印如下日志：

  ```
  [     7.801532][I][netmgr  ]start dhcp
  [     7.855701][I][netmgr  ]IP: 192.168.1.103
  ```

也可以通过命令`ifconfig`检查网络连接状态，当网络连接成功，会有如下信息输出：

  ```
  > ifconfig
  
  wifi0	Link encap:WiFi  HWaddr 28:6d:cd:54:49:c9
      	inet addr:192.168.20.100
  	GWaddr:192.168.20.254
  	Mask:255.255.255.0
  	DNS SERVER 0: 114.114.114.114
  	DNS SERVER 1: 192.168.20.254
  
  WiFi Connected to 3c:37:86:96:1b:c1 (on wifi0)
  	SSID: NETGEAR_HYJ
  	channel: 6
  	signal: -33 dBm
  ```

  此时可以ping通平头哥OCC社区

  ```
  > ping occ.t-head.cn
  	ping occ.t-head.cn(203.119.214.112)
  	from 203.119.214.112: icmp_seq=1 time=38 ms
  ```

  如果Wi-Fi连接失败，会显示“WiFi Not connected”：

  ```
  > ifconfig
  
  wifi0	Link encap:WiFi  HWaddr 28:6d:cd:54:49:c9
      	inet addr:0.0.0.0
  	GWaddr:0.0.0.0
  	Mask:0.0.0.0
  	DNS SERVER 0: 208.67.222.222
  
  WiFi Connected to 00:00:00:00:00:00 (on wifi0)
  	SSID: NETGEAR_HYJ
  	channel: 31
  	signal: -31 dBm
  
  	WiFi Not connected
  ```


#### 5.4 连接生活物联网平台

当通过上述步骤确认网络通畅后，开发板会连接到生活物联网平台，打印信息如下

```
[D][SL      ]smartliving client started

[prt] log level set as: [ 2 ]
.................................
   PK : a1AodaoqL84
   DN : TEST013
   DS : ca14e4d6c9
  PID : example.demo.partner-id
  MID : example.demo.module-id
   SM : TLS + Direct
   TS : 2524608000000
.................................
```

其中，PK/DN/DS/PS/PID信息为五元组信息，在创建产品时由生活物联网平台分配。

与云端连接成功后，会有如下的打印信息

```
[D][tls     ]LD CA root Cert
[D][tls     ]SSL/TLS struct
[I][tls     ]Conn /public.iot-as-mqtt.cn-shanghai.aliyuncs.com/1883
[D][tls     ]Handshake
[D][tls     ]Verify X.509
[I][tls     ]certverify ret 0x00
Device Initialized, Devid: 0
Cloud Connected
```

此时，在生活物联网平台**设备调试**->**测试设备**页面找到对应设备并点击**调试**按钮就可以看到对应的设备上线，并可以在网页上对其进行操作和控制。