# 概述
httpclient_demo是一个简单的httpclient组件使用示例。httpclient是一个开源的http客户端，支持HTTP和HTTPS的访问。

# 使用
## CDK
在CDK的首页，通过搜索httpclient_demo，可以找到httpclient_demo，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html) 章节

## 通过命令行
需要先安装[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install httpclient_demo
```

### 编译&烧录

注意：
    烧录时请注意当前目录下的`gdbinitflash`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的T-HeadDebugServer中显示的对应的内容。

#### D1平台

1. 编译

```bash
make clean
make SDK=sdk_chip_d1
```

2. 烧写

```bash
make flashall SDK=sdk_chip_d1
```

#### bl606P平台

1. 编译

```bash
make clean
make SDK=sdk_chip_bl606p_e907
```

2. 烧写

```bash
make flashall SDK=sdk_chip_bl606p_e907
```

#### ch2601平台

1. 编译

```bash
make clean
make SDK=sdk_chip_ch2601
```

2. 烧写

```bash
make flashall SDK=sdk_chip_ch2601
```

#### cv181x平台

1. 编译

```bash
make clean
make SDK=sdk_chip_cv181xh
```

2. 烧写

```bash
make flashall SDK=sdk_chip_cv181xh
```

#### f133平台

1. 编译

```bash
make clean
make SDK=sdk_chip_f133
```

2. 烧写

```bash
make flashall SDK=sdk_chip_f133
```

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

# 运行
重新上电或按下RST键，系统启动，串口会有以下打印信息，表示系统运行成功。

```
###YoC###[Aug 16 2022,03:03:24]
(cli-uart)# cpu clock is 1008000000Hz
[   0.350]<I>[init]<app_task>find 8 partitions
[   0.360]<D>[WIFI]<app_task>Init WLAN enable

[   0.470]<D>[WIFI_IO]<app_task>__sdio_bus_probe
SD:mmc_card_create card:0x4024f680 id:1
[ERR] SDC:__mci_irq_handler,879 raw_int:100 err!
[ERR] SDC:SDC err, cmd 8, [ERR] SDC:sdc 663 abnormal status: RespErr
SD:sd1.0 or mmc
SD:***** Try sdio *****
[WRN] SD:card claims to support voltages below the defined range.These will be ignored.
SD:sdio highspeed 
SD:mmc_sdio_init_card bus width type:2
SD:
============= card information ==============
SD:Card Type     : SDIO
SD:Card Spec Ver : 1.0
SD:Card RCA      : 0x0001 
SD:Card OCR      : 0x90ffffff
SD:    vol_window  : 0x00ffffff
SD:    to_1v8_acpt : 1
SD:    high_capac  : 1
SD:Card CSD      :
SD:    speed       : 50000 KHz
SD:    cmd class   : 0x0
SD:    capacity    : 0MB
SD:Card CUR_STA  :
SD:    speed_mode  : DS: 25 MHz
SD:    bus_width   : 2
SD:    speed_class : 0
SD:=============================================
SD:***** sdio init ok *****
[   0.600]<I>[netmgr]<netmgr>start wifi
[   0.600]<D>[WiFiCONF]<netmgr>WIFI is not running
[   0.610]<I>[netmgr_wifi]<netmgr>ssid{SSID_Undef}, psk{}

[   0.610]<D>[WiFiCONF]<wifi_start_sta_task>WIFI is not running
[   0.640]<D>[WiFiCONF]<wifi_start_sta_task>Initializing WIFI ...
[   0.670]<D>[WIFI_IO]<sdio_irq>sdio_irq_thread enter IRQ routine
[   3.240]<D>[WiFiCONF]<wifi_start_sta_task>WIFI initialized

[   3.240]<D>[WiFiCONF]<wifi_start_sta_task>a2dp_case_wifi_slot: 35
[   4.580]<E>[WIFI]<wifi_start_sta_task>ERROR: STA Task, wifi connect failed! try another
[   5.920]<D>[WIFI]<cmd_thread>scan done!

[   6.580]<E>[WIFI]<wifi_start_sta_task>Target AP not found

[   6.580]<I>[app]<event_svr>Net down
[   6.580]<D>[AppExp]<event_svr>EVENT_NETMGR_NET_DISCON
[   6.590]<D>[AppExp]<event_svr>Net Reset after 3 second
```

**WiFi配置**

```cli
ifconfig ap <ssid> <password>
```

其中<ssid>指的是WiFi名称，<password>指的是WiFi密码。配置完成之后输入reboot命令进行复位，或者通过开发板的复位键进行复位。

```
[11:25:53.497]收←◆[   7.560]<D>[WIFI]<wifi_start_sta_task>@@@@@@@@@@@@@@ Connection Success @@@@@@@@@@@@@@

[   7.560]<I>[netmgr]<netmgr>start dhcp

[11:25:55.167]收←◆[   9.230]<I>[netmgr]<netmgr>IP: 192.168.31.61
[   9.230]<I>[init]<event_svr>Got IP
[   9.230]<I>[init]<event_svr>NTP Start
[   9.260]<D>[NTP]<event_svr>ntp1.aliyun.com

[11:25:55.238]收←◆[   9.300]<D>[NTP]<event_svr>NTP sec: 1660620356 usec: 22204
[   9.300]<D>[NTP]<event_svr>sync success
[   9.300]<I>[init]<event_svr>NTP Success
Usage: web http|https
     : web http get <url>, such as web http get http://occ.t-head.cn
```

**HTTP测试**

通过串口CLI输入 `web http` 命令进行HTTP访问测试。

```cli
[11:28:04.012]发→◇web http
□
[11:28:04.021]收←◆web http
...(省略)...
[ 143.160]<I>[example]<cli-uart>++++++++++++++ HTTP HEAD TEST OK

[ 143.170]<D>[example]<cli-uart>HTTP_EVENT_DISCONNECTED
```

**HTTPS测试**

通过串口CLI输入 `web https` 命令进行HTTPS访问测试。

```cli
[11:30:53.596]发→◇web https
□
[11:30:53.607]收←◆web https
...(省略)...
[ 164.400]<I>[example]<cli-uart>++++++++++++++ HTTPS TEST OK
[ 164.410]<D>[example]<cli-uart>HTTP_EVENT_DISCONNECTED
```

**自定义URL测试**

通过串口CLI输入 `web http get <URL>` 命令进行HTTP GET访问测试。比如输入 `web http get http://occ.t-head.cn`。

```cli
[13:40:13.338]发→◇web http get http://occ.t-head.cn
□
[13:40:13.343]收←◆web http get http://occ.t-head.cn
...(省略)...

[8069.410]<I>[example]<cli-uart>++++++++++++++ HTTP GET TEST OK
```

