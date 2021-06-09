# BLE SHELL用户手册

# 1. 概述
## 1.1 目的
本文主要介绍了ble协议栈支持的shell指令集，为ble协议栈及应用开发/测试人员提供帮助。
## 1.2 名词解释
下面介绍本文中涉及的一些专有名词

| 名词  | 说明                                |
| ----- | ----------------------------------- |
| BLE   | bluetooth Low energy，低功耗蓝牙    |
| smp   | 安全管理协议                        |
| gatt  | 通用属性规范                        |
| l2cap | 逻辑链路控制和适配协议              |
| wl    | 白名单                              |
| oob   | Out of band，带外数据，用于安全认证 |
| bas   | ble 标准电池服务                    |

## 1.3 介绍
本文将会涉及到的如下几部分按章节介绍：

- ble指令语法
- ble指令介绍
- ble指令使用指南

主要介绍了ble adv、ble scan、ble smp、ble wl等几个常见应用场景的ble指令使用指南.

# 2. ble指令语法
## 2.1.  ble指令请求消息格式
ble 指令请求消息格式：ble+< CMD>+[para-1,para-2,……para-n]<\r><br />AT 请求消息格式

| 域               | 说明                                                         |
| ---------------- | ------------------------------------------------------------ |
| ble+             | 命令消息前缀                                                 |
| CMD              | 指令操作符                                                   |
| para-1,para-2,…… | 命令参数值，注意第三章各指令说明中，<>标识的参数是必选参数，[]标识的参数为非必选参数 |
| \\r              | 回车结束符                                                   |

## 2.2 ble 响应消息格式
参见下一章各指令说明
# 3. 指令说明
### 3.1 ble init
基本功能：蓝牙协议栈初始化

| 参数说明   | [mac] - 设备mac地址，格式为xx:xx:xx:xx:xx:xx <br />[mac_type]-设备mac地址类型，public(公有地址)/random(私有地址) |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Bluetooth stack init success -成功<br />Bluetooth init failed (err num)-失败，num为错误码 |
| 示例       | ble init 01:02:03:04:05:06 public                            |
| 注意事项   | 无                                                           |

### 3.2 ble name
基本功能：设置蓝牙设备名称

| 参数说明   | < name> - 设备名称 |
| ---------- | ------------------ |
| 返回值说明 | 无                 |
| 示例       | ble name test      |
| 注意事项   | 无                 |

### 3.3 ble scan
基本功能：扫描蓝牙设备

| 参数说明   | <value: active>  - active(主动扫描)/passive(被动扫描)/off(关闭扫描)<br />[dup filter] - dups（过滤重复使能）/nodups（关闭过滤重复）.默认关闭<br />[scan interval]- 扫描间隔,默认60ms<br />[scan window]- 扫描窗口,默认30ms<br />[ad] -需扫描的ad数据，格式为len&#124;adtype&#124;addata，默认为空<br />[sd]- 需扫描的sd数据，格式为len&#124;adtype&#124;addta，默认为空 |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | [DEVICE]: (address)(mac_type), adv type (type), rssi (rssi), Raw data:(ad_data)-成功<br />address - 设备mac地址,格式为xx:xx:xx:xx:xx:xx<br />mac_type - mac设备地址类型，public(公有地址)/random(私有地址)<br />type - 广播类型<br />rssi-rssi强度<br />ad_data - 扫描到的ad数据，格式为len&#124;adtype&#124;addata |
| 示例       | ble scan passive dups 96 48 02010603030f180809596f4320424153 |
| 注意事项   | 无                                                           |

### 3.4 ble scan_filter
基本功能：蓝牙扫描过滤

| 参数说明   | <filter_policy> - 0（关闭过滤）/1（白名单过滤）              |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Set scan filter (type)-设置成功<br />type - 是否开启扫描过滤，SCAN_FILTER_POLICY_ANY_ADV（关闭过滤）/SCAN_FILTER_POLICY_WHITE_LIST（打开过滤） |
| 示例       | ble scan_filter 1<br />Set scan filter SCAN_FILTER_POLICY_WHITE_LIST |
| 注意事项   | 打开扫描过滤后，只会上报白名单中的设备                       |

### 3.5 ble adv
基本功能：蓝牙广播

| 参数说明   | < type> - stop(停止)/conn(可连接广播)/nconn(不可连接广播)<br />< ad>- 广播的ad数据，格式为len&#124;adtype&#124;addata<br />< sd>- 广播的sd数据，格式为len&#124;adtype&#124;addata |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Advertising started-成功<br />Failed to start advertising (err num)-失败 |
| 示例       | ble adv conn 02010603030f180809596f4320424153                |
| 注意事项   | 无                                                           |

### 3.6 ble connect
基本功能：蓝牙发起连接

| 参数说明   | < adreess> - 设备mac地址，格式为xx:xx:xx:xx:xx:xx<br />< type> - 设备mac地址类型，public(公有地址)/random(私有地址)<br />[interval_min] - 设备连接最小间隔,默认 为30ms<br />[interval_max]- 设备连接最大间隔,默认为50ms<br />[latency] - 设备连接lantency,默认为0<br />< timeout>- 设备连接超时时间，默认为400ms |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Connected (conn_handle):adreess (type)  -   conn_handle为连接句柄， 连接成功，adreess为设备mac地址，格式为xx:xx:xx:xx:xx:xx，type为设备mac地址类型，public(公有地址)/random(私有地址) |
| 示例       | ble connect C0:BA:88:E3:3B:CC random<br />Connected (0): C0:BA:88:E3:3B:CC (random) |
| 注意事项   | 无                                                           |

### 3.7 ble disconnect
基本功能：蓝牙设备断开连接

| 参数说明   | [conn_handle] - 连接句柄                                     |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Disconected (conn_handle): adreess (type) err num   - 断开成功<br />conn_handle为连接句柄<br />adreess为设备mac地址，格式为xx:xx:xx:xx:xx:xx<br />type为设备mac地址类型,public(公有地址)/random(私有地址)<br />num为错误码 |
| 示例       | ble disconnect 0<br />Disconected (0): C0:BA:88:E3:3B:CC (random) err 22 |
| 注意事项   | 无                                                           |

### 3.8 ble auto-conn
基本功能：设置蓝牙自动连接

| 参数说明   | < adreess> - 设备mac地址，格式为xx:xx:xx:xx:xx:xx<br />< type> - 设备mac地址类型，public(公有地址)/random(私有地址)<br />< action> - on(打开)/off（关闭） |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | 无                                                           |
| 示例       | ble auto-conn  C0:BA:88:E3:3B:CC random on                   |
| 注意事项   | 无                                                           |

### 3.9 ble conn-update
基本功能：蓝牙更新 连接

| 参数说明   | < min>- 连接最小间隔<br /> < max> -连接最大间隔<br />< latency> -连接应答延时次数<br />< timeout>-连接超时时间 |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | LE conn param updated: int (interival) lat (latency) to (timeout) - 更新成功<br />interival -更新后的 连接间隔<br />latency - 更新后的连接应答延时次数<br />timeout - 更新后的连接超时时间<br />conn update failed (err num)-更新失败<br />num-错误码 |
| 示例       | ble conn-update 96 96 1 500<br />LE conn param updated: int 0x0096 lat 1 to 1280 |
| 注意事项   | 无                                                           |

### 3.10 ble clear
基本功能:清除配对设备

| 参数说明   | < dst> - 清除目标地址，all（清除所有）/address(特定mac地址，格式为xx:xx:xx:xx:xx:xx)<br />< type>- 设备mac地址类型，若前一个参数为all，则无需输入该参数，public(公有地址)/random(私有地址) |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Pairings successfully cleared-清除成功                       |
| 示例       | ble clear<br />Pairings successfully cleared                 |
| 注意事项   | 无                                                           |

### 3.11 ble security
基本功能：设置蓝牙安全等级

| 参数说明   | < security level> - 蓝牙安全等级，可为以下参数0（无加密无鉴权）/1（有加密无鉴权）/2（有加密有鉴权）/ 3（采用FIPS加密算法） |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | 无                                                           |
| 示例       | ble security 1                                               |
| 注意事项   | 需在连接建立后更新                                           |

### 3.12 ble io-capability
基本功能：设置蓝牙i/o能力

| 参数说明   | < io input>- io输入能力，NONE（无）/YESNO（1/0二值）/ KEYBOARD（键盘）<br /> < io output>- io输出能力，NONE(无)/DISPLAY(显示) |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | 无                                                           |
| 示例       | ble io-capability KEYBOARD DISPLAY                           |
| 注意事项   | 无                                                           |

### 3.13 ble auth-cancel
基本功能：蓝牙安全认证取消

| 参数说明   | 无              |
| ---------- | --------------- |
| 返回值说明 | 无              |
| 示例       | ble auth-cancel |
| 注意事项   | 无              |

### 3.14 ble auth-passkey
基本功能：蓝牙认证密钥输入

| 参数说明   | < passkey>- 认证密钥 |
| ---------- | -------------------- |
| 返回值说明 | 无                   |
| 示例       | ble auth-passkey     |
| 注意事项   | 无                   |

### 3.15 ble auth-passkey-confirm
基本功能：蓝牙认证密钥确认

| 参数说明   | 无                       |
| ---------- | ------------------------ |
| 返回值说明 | 无                       |
| 示例       | ble auth-passkey-confirm |
| 注意事项   | 无                       |

### 3.16 ble auth-pairing-confirm
基本功能：蓝牙安全认证配对确认

| 参数说明   | 无                                                           |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Bonded with （address） (type)-成功<br />adreess为设备mac地址，格式为xx:xx:xx:xx:xx:xx，type为设备mac地址类型，public(公有地址)/random(私有地址) |
| 示例       | ble auth-pairing-confirm                                     |
| 注意事项   | 当responder io-capability为KeyboardOnly或包含Display时，收到配对请求需要用ble auth-pairing-confirm确认 |

### 3.17 ble fixed-passkey
基本功能：蓝牙设置固定配对密钥

| 参数说明   | < passkey>- 配对密钥   |
| ---------- | ---------------------- |
| 返回值说明 | 无                     |
| 示例       | ble fixed-passkey 1234 |
| 注意事项   | 无                     |

### 3.18 ble gatt-exchange-mtu
基本功能：交换MTU

| 参数说明   | 无                      |
| ---------- | ----------------------- |
| 返回值说明 | Exchange pending - 成功 |
| 示例       | ble gatt-exchange-mtu   |
| 注意事项   | 无                      |

### 3.19 ble gatt-discover-primary
基本功能：蓝牙发现主要服务

| 参数说明   | < UUID>- 服务UUID<br /> [ start handle] -开始数据<br />[ end handle]-结束句柄 |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Service (uuid) found: start handle  (s), end_handle (e)-发现成功<br />uuid - 服务uuid<br />s- 服务开始句柄（十六进制）<br />e-服务结束句柄（十六进制） |
| 示例       | ble gatt-discover-primary 180f 0001 FFFF（发现BAS主服务)<br />Service 180f found: start handle c, end_handle f |
| 注意事项   | 无                                                           |

### 3.20 ble gatt-discover-characteristic
基本功能：蓝牙发现服务特性

| 参数说明   | < UUID>- 服务UUID<br /> [ start handle] -开始数据<br />[ end handle]-结束句柄 |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Characteristic (uuid) found: handle (handle)  Properties: (Pro)- 发现成功<br />uuid - 发现的服务uuid<br />handle - 服务句柄<br />Pro - 支持属性，如read、notify等 |
| 示例       | ble gatt-discover-characteristic 2a19 c f（发现BAS电量属性）<br />Characteristic 2a19 found: handle d |
| 注意事项   | 无                                                           |

### 3.21 ble gatt-discover-descriptor
基本功能：蓝牙发现服务特性描述符

| 参数说明   | < UUID>- 服务UUID<br /> [ start handle] -开始数据<br />[ end handle]-结束句柄 |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Descriptor (uuid) found:(handle)- 发现成功<br />uuid - 发现的服务uuid<br />handle - 服务句柄 |
| 示例       | ble gatt-discover-descriptor 2902 d f（发现BAS电量属性特性描述符）<br />Characteristic 2902 found: handle f |
| 注意事项   | 无                                                           |

### 3.22 ble gatt-read
基本功能：蓝牙服务特性读

| 参数说明   | < handle>-句柄 <br />[offset] -偏移                          |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Read complete: err (num) length (len) data - 读取成功<br />num-错误码<br />len-数据长度<br />data - 接收到的数据 |
| 示例       | ble gatt-read d<br />00000000 12 0E 00 19 2A                 |
| 注意事项   | 无                                                           |

### 3.23 ble gatt-read-multiple
基本功能：蓝牙服务特性读多组

| 参数说明   | <handle 1> -句柄1<br /><handle 2>-句柄2        |
| ---------- | ---------------------------------------------- |
| 返回值说明 |                                                |
| 示例       | 无                                             |
| 注意事项   | 读多组特性可能长度不够，最好先发起连接参数更新 |

### 3.24 ble gatt-write
基本功能：蓝牙发现服务特性写（需应答）

| 参数说明   | < handle> - 句柄<br />< offset> -偏移<br />< data>-单字节数据<br /> [length]-data重复次数 |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Write complete: err (num)-写成功<br />num-错误码             |
| 示例       | ble gatt-write f 0 1 1(使能BAS notify，也可通过gatt-subscribe实现） |
| 注意事项   | offset仅支持为0                                              |

### 3.25 ble gatt-write-without-response
基本功能：蓝牙发现服务特性写（无需应答）

| 参数说明   | < handle> - 句柄<br />< offset> -偏移<br />< data>-单字节数据<br /> [length]-data重复次数 |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Write Complete (err (num)) <br />num-错误码                  |
| 示例       | ble gatt-write-without-response f 0 01 1                     |
| 注意事项   | offset仅支持为0                                              |

### 3.26 ble gatt-write-signed
基本功能：蓝牙发现服务特性写（需应答）

| 参数说明   | < handle> - 句柄<br />< data>-单字节数据<br /> [length]-data重复次数，默认为1<br />[reapt]-发包行为重复次数，默认为1 |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Write Complete (err (num)) <br />num-错误码                  |
| 示例       | ble gatt-write-signed f 01 2 2                               |
| 注意事项   | 因为需携带签名数据，有效数据载荷有限， 使用前最好进行MTU协商 |

### 3.27 ble gatt-subscribe
基本功能：蓝牙发现服务特性订阅

| 参数说明   | < CCC handle>- CCC句柄<br /> < ind>-indicate使能，若未输入ind,则默认使用notify |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Subscribed-成功                                              |
| 示例       | ble gatt-subscribe f ind                                     |
| 注意事项   | 无                                                           |

### 3.28 ble gatt-unsubscribe
基本功能：蓝牙发现服务特性取消订阅

| 参数说明   | < CCC handle>- CCC句柄   |
| ---------- | ------------------------ |
| 返回值说明 | Unsubscribe success-成功 |
| 示例       | ble gatt-unsubscribe f   |
| 注意事项   | 无                       |

### 3.29 ble gatt-show-db
基本功能：蓝牙服务属性显示

| 参数说明   | < handle> - 句柄<br />< offset> -偏移<br />< data>-数据<br /> [length]-数据长度 |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 |                                                              |
| 示例       |                                                              |
| 注意事项   |                                                              |

### 3.30 ble gatt-register-service
基本功能：蓝牙注册gatt服务

| 参数说明   | 无                                                           |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | 无                                                           |
| 示例       | ble gatt-register-service                                    |
| 注意事项   | 用于client设备注册gatt服务等，发现server设备服务前需调用该接口 |

### 3.31 ble gatt-register-service2
基本功能：蓝牙注册gatt服务2

| 参数说明   | 无                         |
| ---------- | -------------------------- |
| 返回值说明 | 无                         |
| 示例       | ble gatt-register-service2 |
| 注意事项   |                            |

### 3.32 ble gatt-transport-test-config
基本功能：蓝牙传输服务测试

| 参数说明   | < type> -0（server）/1(client)<br />< mode>-0(loop)/1(single)<br /> < server tx mode > - 0(notify)/1(indicate)<br />< server rx handle> - 接收句柄<br />< client tx mode>-0(write)/ 1(write_withoutresponse) |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 |                                                              |
| 示例       | ble gatt-transport-test-config 0 0 c 0                       |
| 注意事项   |                                                              |

### 3.33 ble gatt-transport-test-op
基本功能：蓝牙传输服务测试操作

| 参数说明   | < op>-操作码，0(stop)/ 1(start)/ 2（show result）/ 3（reset） |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 |                                                              |
| 示例       |                                                              |
| 注意事项   |                                                              |

### 3.34 ble wl-size
基本功能：蓝牙获取白名单大小

| 参数说明   | 无                                                |
| ---------- | ------------------------------------------------- |
| 返回值说明 | white list size is (num)-成功<br />num-白名单大小 |
| 示例       | ble wl-size<br />white list size is 8             |
| 注意事项   | 无                                                |

### 3.35 ble wl-add
基本功能：蓝牙获取白名单添加

| 参数说明   | < adreess> - 设备mac地址，格式为xx:xx:xx:xx:xx:xx<br />< type> - 设备mac地址类型，public(公有地址)/random(私有地址) |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Add (address) (type) to white list- 成功<br />adreess为设备mac地址，格式为xx:xx:xx:xx:xx:xx，type为设备mac地址类型，public(公有地址)/random(私有地址) |
| 示例       | ble wl-add C0:BA:88:E3:3B:CC random<br />Add C0:BA:88:E3:3B:CC (random) to white list |
| 注意事项   | 无                                                           |

### 3.36 ble wl-remove
基本功能：蓝牙获取白名单删除

| 参数说明   | < adreess> - 设备mac地址，格式为xx:xx:xx:xx:xx:xx<br />< type> - 设备mac地址类型，public(公有地址)/random(私有地址) |
| ---------- | ------------------------------------------------------------ |
| 返回值说明 | Remove (address) (type) to white list- 成功<br />adreess为设备mac地址，格式为xx:xx:xx:xx:xx:xx，type为设备mac地址类型，public(公有地址)/random(私有地址) |
| 示例       | ble wl-remove C0:BA:88:E3:3B:CC random<br />Remove C0:BA:88:E3:3B:CC (random) to white list |
| 注意事项   | 无                                                           |

### 3.37 ble wl-clear
基本功能：蓝牙获取白名单清空

| 参数说明   | 无                                 |
| ---------- | ---------------------------------- |
| 返回值说明 | Clear white list-成功              |
| 示例       | ble wl-clear<br />Clear white list |
| 注意事项   | 无                                 |

# 4. ble指令使用指南
## 4.1 ble adv使用场景
### 4.1.1 场景说明
这里以使用shell命令注册gatt服务为例，说明ble shell slave设备主要使用流程
### 4.1.2 使用步骤
使用开发板A烧录shell应用，开发板A输入如下命令：
```shell
/*初始化ble协议栈*/
ble init
->Bluetooth stack init success
/*注册gatt test服务*/
ble gatt-register-service
->Registering test services
/*开始adv广播，设备名称为“Test”*/
ble adv conn 020106050954657374
->adv_type:0;adv_interval_min:160 (*0.625)ms;adv_interval_max:240 (*0.625)ms      
Advertising started
```
- 使用nrf connect查找到广播名为Test的设备，并点击CONNECT按钮
- 连接成功后，可以对对端设备进行服务发现

![](assets\service_find.png)

## 4.2 ble scan使用场景
### 4.2.1 场景说明
这里以使用shell命令扫描、连接、发现以及读取bas设备为例，说明ble shell master设备主要使用流程
### 4.2.2 使用步骤
使用开发板A烧录solutions下面的bas应用，开发板B烧录shell应用，开发板B输入如下命令：
```shell
/*初始化ble协议栈*/
ble init
->Bluetooth stack init success
/*注册gatt服务，用于上报服务发现数据*/
ble gatt-register-service
->Registering test services 
/*启动扫描*/
ble scan passive
/*扫描到bas设备*/
->[DEVICE]: C0:BA:88:E3:3B:CC (random), adv type 0, rssi -64, Raw data:02010603030
f180809596f4320424153                                                           
->[DEVICE]: F8:A7:63:41:B8:09 (public), adv type 0, rssi -57, Raw data:02010603032
8180c162818006ea3c590be709f5d
/*关闭扫描*/
ble scan off
/*发起连接*/
ble connect C0:BA:88:E3:3B:CC random
/*连接成功*/
->Connected (0): C0:BA:88:E3:3B:CC (random)
/*发现bas主要服务，180f为bas服务uuid*/
ble gatt-discover-primary 180f 0001 FFFF
/*找到bas服务，服务开始句柄为0x0c,结束句柄为0x0f*/
->Service 180f found: start handle c, end_handle f
/*发现bas服务电量属性，电量属性uuid为2a19*/
ble gatt-discover-characteristic 2a19 c f
/*找到电量属性*/
->Characteristic 2a19 found: handle d
/*发现BAS电量属性特性描述符，电量属性uuid为2902*/
ble gatt-discover-descriptor 2902 d f
/*找到电量属性描述符*/
->Characteristic 2902 found: handle f
/*使能电量属性notify*/
ble gatt-subscribe f notify
/*电量上报*/
->Notification: char handle 14 length 1                                           
00000000 52                                                                     
->Notification: char handle 14 length 1                                           
00000000 53                                                                     
->Notification: char handle 14 length 1                                           
00000000 54                                                                     
->Notification: char handle 14 length 1                                           
00000000 55                                                                     
->Notification: char handle 14 length 1                                           
00000000 56
```
## 4.3 ble wl使用场景
### 4.3.1 场景说明
这里以scanner为例，说明白名单机制使用方法
### 4.3.2 使用步骤
使用开发板A烧录solutions下面的bas应用，开发板B烧录shell应用，开发板B输入如下命令：
```shell
/*初始化ble协议栈*/
ble init
->Bluetooth stack init success
/*获取白名单大小*/
ble wl-size
->white list size is 8
/*添加白名单，只扫描BAS设备*/
ble wl-add C0:BA:88:E3:3B:CC random
->Add C0:BA:88:E3:3B:CC (random) to white list 
/*开启扫描过滤*/
ble scan_filter 1
->Set scan filter SCAN_FILTER_POLICY_WHITE_LIST
/*开启扫描*/
ble scan passive
/*可以看到只上报了白名单设备*/
->[DEVICE]: C0:BA:88:E3:3B:CC (random), adv type 0, rssi -63, Raw data:02010603030
f180809596f4320424153
->[DEVICE]: C0:BA:88:E3:3B:CC (random), adv type 0, rssi -63, Raw data:02010603030
f180809596f4320424153
->[DEVICE]: C0:BA:88:E3:3B:CC (random), adv type 0, rssi -63, Raw data:02010603030
f180809596f4320424153
```