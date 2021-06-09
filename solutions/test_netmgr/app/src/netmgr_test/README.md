## 简介

Netmgr模块测试用例分为接口测试和功能测试，接口测试case在文件netmgr_api.c中，功能测试case在netmgr_function.c中。

## 接口测试执行方法

编译程序烧写进板子后，执行test netmgrapi指令，查看返回结果，执行通过的case不会打印，执行失败的case返回失败的接口信息。

注意：NETMGR组件依赖于底层的网卡，当前，只支持wifi。

## 功能测试执行方法

注意：NETMGR组件依赖于底层的网卡，当前，只支持wifi。
程序编译通过后，烧写进板子，在串口执行如下指令：

指令说明如下：

（1）初始化

指令：wifi init

功能：初始化netmgr服务，和wifi网卡

参数说明：

wifi：netmgr模块的wifi配置；

init：初始化操作；

返回值：无，根据log打印判断是否初始化成功。

注：必须先初始化，才能做后续测试。

（2）配置wifi ssid和psk

指令：wifi config ssid psk

功能：配置wifi ssid和psk

参数说明：

wifi：netmgr模块的wifi配置；

config：进行ssid和psk的配置；

ssid：要配置的ssid 字符串；

psk： 要配置的psk字符串；

返回值：无；若有result=0的log打印，表示配置成功； 

（3）配置IP相关参数

指令：wifi ipconfig dhcp | ip mask gw

功能：配置ip相关参数

参数说明：

wifi：netmgr模块的wifi配置；

ipconfig：进行IP地址相关参数的配置；

dhcp：表示以dhcp的方式获取IP地址信息，如果选择该参数，后续参数将被忽略；

ip： ip地址；没有选择dhcp参数时，可以直接配置IP地址

mask：子网掩码； 

gw：缺省网关；

返回值：无。 若有result=0的log打印，表示配置成功； 

注：如果不执行该命令，缺省为dhcp模式。

（4）连接wifi

指令：wifi connect

功能： 连接wifi。注意，在连接之前需要配置wifi ssid和psk

参数说明：

wifi：netmgr模块的wifi配置；

connect：进行wifi连接；

返回值： 无。

（5）查看接口状态

指令：wifi info

功能： 查看接口状态

参数说明：

wifi：netmgr模块的wifi配置；

info：打印wifi接口信息；

返回值： 无。可以根据打印，判断wifi的连接状态，以及接口的IP地址相关信息。

（6）断开wifi连接

指令：wifi disconnect

功能： 断开wifi连接。

参数说明：

wifi：netmgr模块的wifi配置；

connect：断开wifi连接；

返回值： 无。

（7）重置wifi设备

指令：wifi reset

功能： 重置wifi设备。

参数说明：

wifi：netmgr模块的wifi配置；

reset：重置wifi设备；

返回值： 无。

（8）去初始化

指令：wifi deinit

功能：去初始化netmgr服务，和wifi网卡

参数说明：

wifi：netmgr模块的wifi配置；

deinit：去初始化操作；

返回值：无，根据log打印判断是否初始化成功。

注：去初始化后，必须再次初始化，才能做其他测试。

（9）验证网络连通性

指令：ping ip

功能：探测到某个ip的连通性

参数说明：

ping：ping命令；

ip：要探测的ip地址；

返回值：无，根据打印判断到目的地址的连通性。

