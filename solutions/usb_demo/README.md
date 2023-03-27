# 概述
该Demo展示了USB协议栈的使用方法

# 启动

```
(cli-uart)# ###YoC###[Dec  5 2022,16:49:19]
[   1.108]<D>main app_main.c[345]: build time: Dec  6 2022, 09:29:16
```

设备启动后没有初始化USB协议栈，需要输入CLI命令才能进行

## 调试

### USB Host 协议栈

#### 初始化

通过 `usbtest usbh` 可以初始化USB Host协议栈
```
(cli-uart)# usbtest usbh

(cli-uart)# 
(cli-uart)# 
(cli-uart)# 
(cli-uart)# [ 162.188]<D>USB usbh_hub.c[382]: Port change:0x02

[ 162.192]<D>USB usbh_hub.c[388]: Port 1 change

[ 162.196]<D>USB usbh_hub.c[400]: port 1, status:0x101, change:0x01

[ 162.228]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[ 162.260]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[ 162.292]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[ 162.324]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[ 162.356]<D>USB usbh_hub.c[436]: Port 1, status:0x101, change:0x00

[ 162.672]<I>USB usbh_hub.c[496]: New full-speed device on Hub 1, Port 1 connected

[ 162.680]<I>USB usbh_core.c[528]: New device found,idVendor:058f,idProduct:6254,bcdDevice:0100

[ 162.688]<I>USB usbh_core.c[568]: The device has 1 interfaces

[ 162.692]<I>USB usbh_core.c[635]: Enumeration success, start loading class driver

[ 162.700]<I>USB usbh_core.c[651]: Loading hub class driver

Hub Descriptor:
bLength: 0x09             
bDescriptorType: 0x29     
bNbrPorts: 0x04           
wHubCharacteristics: 0x0080 
bPwrOn2PwrGood: 0x32      
bHubContrCurrent: 0x64    
DeviceRemovable: 0x00     
PortPwrCtrlMask: 0xff     
[ 162.724]<I>USB usbh_core.c[429]: Ep=81 Attr=03 Mps=1 Interval=255 Mult=00

[ 162.732]<I>USB usbh_hub.c[296]: port 1, status:0x100, change:0x00

[ 162.736]<I>USB usbh_hub.c[296]: port 2, status:0x100, change:0x00

[ 162.744]<I>USB usbh_hub.c[296]: port 3, status:0x100, change:0x00

[ 162.748]<I>USB usbh_hub.c[296]: port 4, status:0x100, change:0x00

[ 162.756]<I>USB usbh_hub.c[305]: Register HUB Class:/dev/hub2

[ 162.760]<D>main app_main.c[223]: usbh_device_mount_done_callback Class:0x09,Subclass:0x00,Protocl:0x00
[ 162.768]<D>main app_main.c[228]: class driver: hub
```

#### 查看USB设备
```
(cli-uart)# lsusb -t
/: Hub 01, ports=1, is roothub
    |__Port 1,Port addr:0x02, ID 058f:6254, If 0,ClassDriver=hub
```
还可以使用 
```
lsusb -v
```
命令来得到更详细的信息

##### 插入U盘

插入U盘后可以通过`lsusb`命令看到多了一个msc设备

```
lsusb -t
/: Hub 01, ports=1, is roothub
    |__Port 1,Port addr:0x02, ID 058f:6254, If 0,ClassDriver=hub
/: Hub 02, ports=4, mounted on Hub 01:Port 1
    |__Port 1,Port addr:0x03, ID 05e3:0736, If 0,ClassDriver=msc
```


### USB Device 协议栈
测试完Host协议栈后，重启设备，才能进行Devices协议栈测试

#### UAC设备

初始化
通过CLI命令`usbtest usbd uac`可以将设备初始化成UAC设备
```
usbtest usbd uac
```

这是设备在等待PC等Host设备的连接。
接入电脑后USB口后可以看到打印

```
(cli-uart)# usbtest usbd uac
usbd uac inited, please use Audacity to test it!
CLOSE
CLOSE
CLOSE
CLOSE
```
说明设备已经枚举成功

可以通过 Audacity 等录音软件进行录音测试

设备会发送固定数字给Host。

#### UVC设备

初始化
通过CLI命令`usbtest usbd uvc`可以将设备初始化成UAC设备
```
usbtest usbd uvc
```

这是设备在等待PC等Host设备的连接。
接入电脑后USB口后可以看到打印

```
usbtest usbd uvc
usbd uvc inited, please goto https://webcam-test.com/zh-CN to test it!
CLOSE
CLOSE
CLOSE
CLOSE

```
说明设备已经枚举成功

可以通过 https://webcam-test.com/zh-CN 网站来产看UVC设备的录像

设备会发送一张特定的图片


#### RNDIS Device

初始化
通过CLI命令`usbtest usbd rndis`可以将设备初始化成UAC设备
```
usbtest usbd rndis
```

设备接入电脑后会收到电脑的一个DHCP报文，并且能在电脑上看到网卡设备

```
rndis test start, please plug the USB on pc
(cli-uart)# 
(cli-uart)# 
USBD_EVENT_CONFIGURED
USBD_EVENT_CONFIGURED

_read_cb len 342

USBD_EVENT_CONFIGURED
USBD_EVENT_CONFIGURED
```
