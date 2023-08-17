# 概述
使用移远EC200A 4G模组的网络使用示例。
本示例通过USB与EC200A模组通讯，使用了RNDIS和USB Serial两个USB class。

# 启动

```
(cli-uart)# ###YoC###[Nov 29 2022,16:31:06]
cpu clock is 0Hz
aos debug init ok.
[   1.112]<D>main app_main.c[54]: build time: Nov 29 2022, 16:31:06

[   1.372]<D>USB usbh_hub.c[382]: Port change:0x02

[   1.376]<D>USB usbh_hub.c[388]: Port 1 change

[   1.380]<D>USB usbh_hub.c[400]: port 1, status:0x101, change:0x01

[   1.412]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[   1.444]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[   1.476]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[   1.508]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[   1.540]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[   1.856]<I>USB usbh_hub.c[496]: New full-speed device on Hub 1, Port 1 connected

[   1.864]<I>USB usbh_core.c[528]: New device found,idVendor:058f,idProduct:6254,bcdDevice:0100

[   1.872]<I>USB usbh_core.c[568]: The device has 1 interfaces

[   1.876]<I>USB usbh_core.c[635]: Enumeration success, start loading class driver

[   1.884]<I>USB usbh_core.c[651]: Loading hub class driver
 
[   1.908]<I>USB usbh_core.c[429]: Ep=81 Attr=03 Mps=1 Interval=255 Mult=00

[   1.916]<I>USB usbh_hub.c[296]: port 1, status:0x100, change:0x00

[   1.920]<I>USB usbh_hub.c[296]: port 2, status:0x101, change:0x01

[   1.928]<I>USB usbh_hub.c[296]: port 3, status:0x100, change:0x00

[   1.932]<I>USB usbh_hub.c[296]: port 4, status:0x100, change:0x00

[   1.940]<I>USB usbh_hub.c[305]: Register HUB Class:/dev/hub2

[   1.944]<D>USB usbh_hub.c[382]: Port change:0x04

[   1.948]<D>USB usbh_hub.c[382]: Port change:0x04

[   1.952]<D>USB usbh_hub.c[388]: Port 2 change

[   1.956]<D>USB usbh_hub.c[400]: port 2, status:0x101, change:0x01

[   1.992]<D>USB usbh_hub.c[436]: Port 2, status:0x101, change:0x00

[   2.024]<D>USB usbh_hub.c[436]: Port 2, status:0x101, change:0x00

[   2.056]<D>USB usbh_hub.c[436]: Port 2, status:0x101, change:0x00

[   2.088]<D>USB usbh_hub.c[436]: Port 2, status:0x101, change:0x00

[   2.120]<D>USB usbh_hub.c[436]: Port 2, status:0x101, change:0x00

[   2.324]<I>USB usbh_hub.c[496]: New full-speed device on Hub 2, Port 2 connected

[   2.332]<I>USB usbh_core.c[528]: New device found,idVendor:05e3,idProduct:0610,bcdDevice:0654

[   2.340]<I>USB usbh_core.c[568]: The device has 1 interfaces

[   2.344]<I>USB usbh_core.c[635]: Enumeration success, start loading class driver

[   2.352]<I>USB usbh_core.c[651]: Loading hub class driver
 
[   2.376]<I>USB usbh_core.c[429]: Ep=81 Attr=03 Mps=1 Interval=255 Mult=00

[   2.384]<I>USB usbh_hub.c[296]: port 1, status:0x100, change:0x00

[   2.392]<I>USB usbh_hub.c[296]: port 2, status:0x101, change:0x01

[   2.396]<I>USB usbh_hub.c[296]: port 3, status:0x100, change:0x00

[   2.404]<I>USB usbh_hub.c[296]: port 4, status:0x100, change:0x00

[   2.408]<I>USB usbh_hub.c[305]: Register HUB Class:/dev/hub3

[   2.412]<D>USB usbh_hub.c[382]: Port change:0x04

[   2.420]<D>USB usbh_hub.c[382]: Port change:0x04

[   2.424]<D>USB usbh_hub.c[382]: Port change:0x04

[   2.428]<D>USB usbh_hub.c[382]: Port change:0x04

[   2.432]<D>USB usbh_hub.c[388]: Port 2 change

[   2.436]<D>USB usbh_hub.c[400]: port 2, status:0x101, change:0x01

[   2.468]<D>USB usbh_hub.c[436]: Port 2, status:0x101, change:0x00

[   2.500]<D>USB usbh_hub.c[436]: Port 2, status:0x101, change:0x00

[   2.532]<D>USB usbh_hub.c[436]: Port 2, status:0x101, change:0x00

[   2.564]<D>USB usbh_hub.c[436]: Port 2, status:0x101, change:0x00

[   2.596]<D>USB usbh_hub.c[436]: Port 2, status:0x101, change:0x00

[   2.824]<I>USB usbh_hub.c[496]: New full-speed device on Hub 3, Port 2 connected

[   2.832]<I>USB usbh_core.c[528]: New device found,idVendor:0bda,idProduct:8153,bcdDevice:3100

[   2.840]<I>USB usbh_core.c[568]: The device has 1 interfaces

[   2.844]<I>USB usbh_core.c[635]: Enumeration success, start loading class driver

[   2.852]<I>USB usbh_core.c[651]: Loading usb_serial class driver

[   2.856]<W>USB usbh_serial.c[18]: ignore intf:0

[   2.860]<D>USB usbh_hub.c[382]: Port change:0x04

[   2.864]<D>USB usbh_hub.c[382]: Port change:0x04

[   9.060]<D>USB usbh_hub.c[382]: Port change:0x02

[   9.064]<D>USB usbh_hub.c[388]: Port 1 change

[   9.068]<D>USB usbh_hub.c[400]: port 1, status:0x101, change:0x01

[   9.100]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[   9.132]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[   9.164]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[   9.196]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[   9.228]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[   9.456]<I>USB usbh_hub.c[496]: New full-speed device on Hub 3, Port 1 connected

[   9.464]<I>USB usbh_core.c[528]: New device found,idVendor:2c7c,idProduct:6005,bcdDevice:0318

[   9.472]<I>USB usbh_core.c[568]: The device has 5 interfaces

[   9.476]<I>USB usbh_core.c[635]: Enumeration success, start loading class driver

[   9.484]<I>USB usbh_core.c[651]: Loading rndis class driver

[   9.488]<I>USB usbh_core.c[429]: Ep=83 Attr=02 Mps=64 Interval=00 Mult=00

[   9.496]<I>USB usbh_core.c[429]: Ep=0c Attr=02 Mps=64 Interval=00 Mult=00

[   9.500]<I>USB usbh_rndis.c[263]: rndis init success

[   9.508]<I>USB usbh_rndis.c[270]: rndis query OID_GEN_SUPPORTED_LIST success,oid num :28

[   9.516]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:00010101

[   9.520]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:00010102

[   9.524]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:00010103

[   9.532]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:00010104

[   9.536]<I>USB usbh_rndis.c[323]: rndis query iod:00010106 success

[   9.544]<I>USB usbh_rndis.c[323]: rndis query iod:00010107 success

[   9.548]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:0001010a

[   9.556]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:0001010b

[   9.560]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:0001010c

[   9.568]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:0001010d

[   9.572]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:00010116

[   9.580]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:0001010e

[   9.584]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:00010111

[   9.592]<I>USB usbh_rndis.c[323]: rndis query iod:00010114 success

[   9.596]<I>USB usbh_rndis.c[323]: rndis query iod:00010202 success

[   9.604]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:00020101

[   9.608]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:00020102

[   9.616]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:00020103

[   9.620]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:00020104

[   9.624]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:00020105

[   9.632]<I>USB usbh_rndis.c[323]: rndis query iod:01010101 success

[   9.636]<I>USB usbh_rndis.c[323]: rndis query iod:01010102 success

[   9.644]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:01010103

[   9.648]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:01010105

[   9.656]<I>USB usbh_rndis.c[323]: rndis query iod:01010104 success

[   9.660]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:01020101

[   9.668]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:01020102

[   9.672]<W>USB usbh_rndis.c[320]: Ignore rndis query iod:01020103

[   9.680]<I>USB usbh_rndis.c[333]: rndis set OID_GEN_CURRENT_PACKET_FILTER success

[   9.688]<I>USB usbh_rndis.c[340]: rndis set OID_802_3_MULTICAST_LIST success

[   9.692]<I>USB usbh_rndis.c[344]: Register RNDIS Class:/dev/rndis

[   9.700]<I>USB usbh_core.c[651]: Loading usb_serial class driver

[   9.704]<W>USB usbh_serial.c[18]: ignore intf:1

[   9.708]<I>USB usbh_core.c[651]: Loading usb_serial class driver

[   9.716]<W>USB usbh_serial.c[18]: ignore intf:2

[   9.720]<I>USB usbh_core.c[651]: Loading usb_serial class driver

[   9.724]<I>USB usbh_core.c[429]: Ep=89 Attr=03 Mps=64 Interval=16 Mult=00

[   9.732]<I>USB usbh_core.c[429]: Ep=86 Attr=02 Mps=64 Interval=00 Mult=00

[   9.736]<I>USB usbh_core.c[429]: Ep=0f Attr=02 Mps=64 Interval=00 Mult=00

[   9.744]<I>USB usbh_serial.c[44]: Register serial Class:/dev/serial

[   9.752]<I>USB usbh_core.c[651]: Loading usb_serial class driver

[   9.756]<W>USB usbh_serial.c[18]: ignore intf:4

[   9.760]<D>USB usbh_hub.c[382]: Port change:0x02

[   9.764]<D>USB usbh_hub.c[382]: Port change:0x02

[   9.768]<D>USB usbh_hub.c[382]: Port change:0x02

[  11.120]<I>netmgr netmgr_service.c[316]: start gprs
[  11.124]<I>atparser atparser.c[587]: line(ATE0), format(OK%n), count=-1

[  11.132]<I>atparser atparser.c[587]: line(+CME ERROR: 14), format(+CPIN: READY%n), count=-1

[  12.140]<I>atparser atparser.c[587]: line(+CME ERROR: 13), format(+CPIN: READY%n), count=-1

[  13.148]<I>atparser atparser.c[587]: line(+CME ERROR: 14), format(+CPIN: READY%n), count=-1

[  14.156]<I>atparser atparser.c[587]: line(+CME ERROR: 14), format(+CPIN: READY%n), count=-1

[  17.164]<I>netmgr netmgr_service.c[187]: start dhcp
[  19.124]<I>netmgr netmgr_service.c[211]: IP: 192.168.43.200
[  19.128]<I>main app_main.c[30]: Got IP
[  19.132]<I>main app_main.c[37]: NTP Start
[  19.164]<D>NTP ntp.c[197]: ntp1.aliyun.com
[  19.212]<D>NTP ntp.c[259]: NTP sec: 1669711309 usec: 930362
[  19.216]<D>NTP ntp.c[288]: sync success
[  19.220]<I>main app_main.c[45]: NTP Success
```

设备启动以后usb协议栈会检测usb hub和usb device，检测到en200a模块后 会启动netmgr 进行联网，联网完成后会进行ntp时间同步


## RNDIS 网卡调试
默认开了CONFIG_RNDIS_DEVICE_ETH宏，是通过接入windows，作为windows的网卡，和设备进行通讯。
启动后可以usb插入pc后有如下打印

```
[10:38:01.816]收←◆(cli-uart)# ###YoC###[Jun 20 2023,02:35:46]
cpu clock is 0Hz
aos debug init ok.
[   0.024]<D>main app_main.c[62]: build time: Jun 20 2023, 02:35:47

[   0.032]<E>rndis v_rndis_device.c[237]: queue send error
[   0.036]<I>netmgr netmgr_service.c[316]: start eth

[10:38:01.929]收←◆DevEnumSpeed:0

[10:38:01.995]收←◆USBD_EVENT_CONFIGURED
USBD_EVENT_CONFIGURED
usbd_configure_done_callback
```

设备端输入ifconfig可以看到有一个 eth0的网卡，ip为 192.168.11.10。
```
ifconfig

eth0	Link encap:eth  HWaddr 00:00:00:00:00:00
    	inet addr:192.168.11.10
	GWaddr:192.168.11.1
	Mask:255.255.255.0
	DNS SERVER 0: 208.67.222.222
```

同时，windows电脑上也会有一个rndis网卡出现，通过修改 windows上的rndis网卡在的ip，就可以和设备进行通讯了。（由于通过usb连接，没有dhcpd功能，电脑和设备都需要设置静态ip）



通过去掉 宏 CONFIG_RNDIS_DEVICE_ETH 可以配置使用 移远的4G网卡。

## 4G 网卡调试

查看USB设备
```
(cli-uart)# lsusb -t
/: Hub 01, ports=1, is roothub
    |__Port 1,Port addr:0x02, ID 058f:6254, If 0,ClassDriver=hub
/: Hub 02, ports=4, mounted on Hub 01:Port 1
    |__Port 2,Port addr:0x03, ID 05e3:0610, If 0,ClassDriver=hub
/: Hub 03, ports=4, mounted on Hub 02:Port 2
    |__Port 1,Port addr:0x05, ID 2c7c:6005, If 0,ClassDriver=rndis
    |__Port 1,Port addr:0x05, ID 2c7c:6005, If 1,ClassDriver=usb_serial
    |__Port 1,Port addr:0x05, ID 2c7c:6005, If 2,ClassDriver=usb_serial
    |__Port 1,Port addr:0x05, ID 2c7c:6005, If 3,ClassDriver=usb_serial
    |__Port 1,Port addr:0x05, ID 2c7c:6005, If 4,ClassDriver=usb_serial
    |__Port 2,Port addr:0x04, ID 0bda:8153, If 0,ClassDriver=usb_serial
```

查看网络设备
```
(cli-uart)# ifconfig

gprs0	Link encap:GPRS
    	inet addr:192.168.43.200
    	Signal Quality(31): -52 dBm
    	Sim insert is OK
    	Sim ccid:898600E61xxxxxxxxxx
```

ping
```
(cli-uart)# ping www.baidu.com
	ping www.baidu.com(36.152.44.95)
	from 36.152.44.95: icmp_seq=1 time=40 ms
	from 36.152.44.95: icmp_seq=2 time=24 ms
	from 36.152.44.95: icmp_seq=3 time=28 ms
```
