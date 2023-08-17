# 概述
wifi_demo是一个简单的WiFi连接示例。

# 使用
## CDK
在CDK的首页，通过搜索wifi_demo，可以找到wifi_demo，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html) 章节

## 通过命令行
需要先安装[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install wifi_demo
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

#### cv181xh_huashanpi_evb 平台
1. 编译

```bash
make clean
make SDK=sdk_chip_cv181xh_bga
```

2. 烧写

```bash
make flashall SDK=sdk_chip_cv181xh_bga
```

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

# 运行
烧录完成之后按下复位按键，串口会有打印输出。

```
(cli-uart)# cpu clock is 1008000000Hz
[   1.360]<I>[init]<app_task>find 9 partitions
[   1.380]<D>[WIFI]<app_task>Init WLAN enable


[14:23:17.352]收←◆[   1.480]<D>[WIFI_IO]<app_task>__sdio_bus_probe
SD:mmc_card_create card:0x40245b90 id:1

[14:23:17.413]收←◆[ERR] SDC:__mci_irq_handler,879 raw_int:100 err!
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
[   1.610]<I>[netmgr]<netmgr>start wifi
[   1.610]<D>[WiFiCONF]<netmgr>WIFI is not running
[   1.620]<I>[netmgr_wifi]<netmgr>ssid{Xiaomi_HYJ}, psk{lab000000}

[   1.620]<D>[WiFiCONF]<wifi_start_sta_task>WIFI is not running
[   1.650]<D>[WiFiCONF]<wifi_start_sta_task>Initializing WIFI ...

[14:23:17.583]收←◆[   1.710]<D>[WIFI_IO]<sdio_irq>sdio_irq_thread enter IRQ routine
```

## WiFi配置

```cli
ifconfig ap <ssid> <password>
```

其中<ssid>指的是WiFi名称，<password>指的是WiFi密码。配置完成之后输入reboot命令进行复位，或者通过开发板的复位键进行复位。

```
[14:23:21.434]收←◆[   5.560]<D>[WIFI]<wifi_start_sta_task>@@@@@@@@@@@@@@ Connection Success @@@@@@@@@@@@@@

[   5.560]<I>[netmgr]<netmgr>start dhcp

[14:23:21.502]收←◆[   5.620]<I>[netmgr]<netmgr>IP: 192.168.31.61
[   5.630]<I>[app]<event_svr>Got IP
```

## PING测试

网络连接成功后，使用PING命令测试即为成功

```cli
ping www.baidu.com

[14:23:33.899]收←◆	from 112.80.248.76: icmp_seq=1 time=40 ms

[14:23:34.910]收←◆	from 112.80.248.76: icmp_seq=2 time=10 ms

[14:23:35.929]收←◆	from 112.80.248.76: icmp_seq=3 time=10 ms
```
