## 1.概述
`genie_mesh_node_ctl` 是可连接天猫精灵音箱，同时支持天猫精灵APP控制的Mesh节点应用示例，可以用于开发支持开关、场景模式控制的灯产品；或者作为支持AT指令/串口协议的蓝牙Mesh模组，外接MCU通过串口与蓝牙Mesh模组通信来开发各品类智能家居产品；也可以作为基于单蓝牙Mesh芯片二次开发的基础代码。会用到SIG Model与阿里巴巴Vendor Model。
​
## 2.编译
### 2.1 编译TG7120B上运行的程序

```bash
cd solutions/genie_mesh_node_ctl/
make clean
make
```

生成固件：
完整的烧录固件：solutions/genie_mesh_node_ctl/generated/total_image.hexf
OTA固件：solutions/genie_mesh_node_ctl/generated/fota.bin
### 2.2 编译TG7121B上运行的程序

```bash
cd solutions/genie_mesh_node_ctl/
make clean
make SDK=sdk_chip_TG7121B
```

生成固件：
完整的烧录固件：solutions/genie_mesh_node_ctl/generated/total_image.hex
OTA固件：solutions/genie_mesh_node_ctl/generated/fota.bin

## 3.烧录
### 3.1 烧录TG7120B

参考boards/TG7120B_evb/README.md中的烧录方法。

### 3.2 烧录TG7121B

参考boards/TG7121B_evb/README.md中的烧录方法。

## 4.启动
烧录完成之后按复位键或者重新上电，启动设备
TG7120B 调试串口默认波特率为256000，连接串口工具后串口会有打印输出
TG7121B 调试串口默认波特率为115200，默认无打印输出

## 5.说明
### 5.1 对Genie Mesh SDK的接口调用说明
#### 5.1.1 初始化
函数名称：int genie_service_init(genie_service_ctx_t* p_ctx)
参数说明：
结构体genie_service_ctx_t的成员参数如下：

| 参数名称 | 参数类型 | 功能说明 | 备注 |
| --- | --- | --- | --- |
| p_mesh_elem | structbt_mesh_elem | Element的地址 |  |
| mesh_elem_counts | uint8_t | Element的数目 |  |
| event_cb | user_event_cb | SDK上传给应用的事件 | 用户事件回调，系统初始化和复位通知及接收Mesh数据通知，另外还有定时事件和用户自定义串口透传数据通知 |
| prov_timeout | uint32_t | 默认10分钟，超时之后进入静默广播状态 | 静默广播可以进行设备发现，但是不能直接配网 |
| lpm_conf | genie_lpm_conf_t | 低功耗相关参数配置 | 低功耗相关，由宏定义CONFIG_PM_SLEEP开关该功能
（芯片尚未适配，低功耗暂不推荐打开） |

结构体genie_lpm_conf_t的成员参数如下：

| 参数名称 | 参数类型 | 功能说明 | 备注 |
| --- | --- | --- | --- |
| lpm_wakeup_io | uint8_t | 是否支持GPIO唤醒，0：不支持，>0：支持 |  |
| lpm_wakeup_io_config | genie_lpm_wakeup_io_t | 配置多个唤醒GPIO | 多键开关可以使用 |
| is_auto_enable | uint8_t | 上电之后是否自动进入低功耗状态 | 大于0自动进入低功耗 |
| delay_sleep_time | uint32_t | 上电多久之后进入低功耗状态，默认是10秒 |  |
| sleep_ms | uint16_t | 低功耗睡眠时长 | GLP模式的时候1100ms |
| wakeup_ms | uint16_t | 低功耗唤醒时长 | GLP模式的时候60ms |
| genie_lpm_cb | genie_lpm_cb_t | 睡眠和唤醒状态通知用户的回调函数 |  |

#### 5.1.2 发送Mesh数据
SDK为Vendor Model的数据发送提供了两个API接口，一个需要element参数，另外一个不需要element参数，会直接使用primary element，两个函数的参数介绍如下：
函数名称：int genie_transport_send_model(genie_transport_model_param_t* p_vendor_msg)
参数说明：
结构体genie_transport_model_param_t的成员参数如下：

| 参数名称 | 参数类型 | 功能说明 | 备注 |
| --- | --- | --- | --- |
| p_elem | struct bt_mesh_elem* | 指定发送数据的Element，主要是获取源地址 |  |
| p_model | struct bt_mesh_model* | 指定发送数据的model，主要是获取app key |  |
| result_cb | transport_result_cb | 发送结果回调给应用 | 发送超时或者发送成功（D3和CF消息不会执行回调） |
| retry | uint8_t | 没有收到应答执行重发的次数，默认重发两次 | 如果是GLP设备默认重发五次 |
| opid | uint8_t | opcode id，例如：0xD4 | 取3字节opcode的第一个字节 |
| tid | uint8_t | 传输ID，通常传入值0，为0的时候SDK会自动生成TID | Mesh设备发送数据的TID从0x80到0xBF |
| dst_addr | uint16_t | 数据目的地址 |  |
| retry_period | uint16_t | 重传的时间间隔 |  |
| len | uint16_t | 发送数据的长度 |  |
| data | uint8_t*  | 发送数据的起始地址 |  |

函数名称：int genie_transport_send_payload(genie_transport_payload_param_t* payload_param)
结构体genie_transport_payload_param_t的成员参数如下：

| 参数名称 | 参数类型 | 功能说明 | 备注 |
| --- | --- | --- | --- |
| opid | uint8_t | opcode id，例如：0xD4 | 取3字节opcode的第一个字节 |
| tid | uint8_t | 传输ID，通常传入值0，为0的时候SDK会自动生成TID | Mesh设备发送数据的TID从0x80到0xBF |
| retry_cnt | uint8_t | 没有收到应答执行重发的次数，默认重发两次 | 如果是GLP设备默认重发五次 |
| dst_addr | uint16_t | 数据目的地址 |  |
| p_payload | uint8_t*  | 发送数据的起始地址 |  |
| payload_len | uint16_t | 发送数据的长度 |  |
| result_cb | transport_result_cb | 发送结果回调给应用 | 发送超时或者发送成功（D3和CF消息不会执行回调） |

#### 5.1.3 接收Mesh数据
应用程序接收Mesh数据是通过初始化的时候注册的用户回调函数接收的，如下两个Event分别接收SIG Model的数据和Genie的Vendor Model的数据：

- 接收SIG Model的数据：
```c
      case GENIE_EVT_SIG_MODEL_MSG:    {
        sig_model_msg *p_msg = (sig_model_msg *)p_arg;

        if (p_msg)
        {
            GENIE_LOG_INFO("SIG mesg ElemID(%d)", p_msg->element_id);
        }
    }
    break;
```


- 接收Vendor Model的数据
```c
    case GENIE_EVT_VENDOR_MODEL_MSG:
    {
        genie_transport_model_param_t *p_msg = (genie_transport_model_param_t *)p_arg;

        if (p_msg && p_msg->p_model && p_msg->p_model->user_data)
        {
            sig_model_element_state_t *p_elem_state = (sig_model_element_state_t *)p_msg->p_model->user_data;
            GENIE_LOG_INFO("ElemID(%d) TID(%02X)", p_elem_state->element_id, p_msg->tid);
            (void)p_elem_state;
        }
    }
    break;
```

#### 5.1.4 通过命令接收和发送Mesh数据
参考5.5或者5.6的部分，可以使用二进制串口协议或者AT指令协议，也可以使用用户自定义的协议


### 5.2 串口说明
#### 5.2.1 调试串口命令说明（UART0)
配置代码参考：
```c

void console_init(int idx, uint32_t baud, uint16_t buf_size)
{
    int rc;
    aos_mutex_new(&g_console_mutex_handle);

    _uart.port                = idx;
    _uart.config.baud_rate    = baud;
    _uart.config.mode         = MODE_TX_RX;
    _uart.config.flow_control = FLOW_CONTROL_DISABLED;
    _uart.config.stop_bits    = STOP_BITS_1;
    _uart.config.parity       = NO_PARITY;
    _uart.config.data_width   = DATA_WIDTH_8BIT;

    rc = hal_uart_init(&_uart);
    if (rc == 0) {
        g_console_handle = &_uart;
        g_console_buf_size = buf_size;
    }
}
```
调试串口主要命令说明：

| 命令名称 | 命令说明 | 使用参考（示例） |
| --- | --- | --- |
| set_tt | 设备蓝牙Mesh设备三元组 | set_tt 5297793 0c51b11c6ec78b52b803b3bbaae64fba 486e704a5bf6 |
| get_tt | 查看蓝牙Mesh设备三元组 | 无参数 |
| get_info | 查看版本和MAC等信息 | 无参数 |
| reboot | 系统重启 | 无参数 |
| reset | 设备复位 | 无参数 |
| mesg | 通过MESH发送数据 | mesg d4 1 f000 010203 |

mesg命令参数说明：

1. 第一个参数d4就是indication发送，其他有D3、CE及CF等；
2. 第二个参数是发送模式和重发次数参数
- 0表示不重发
- 1-252表示重发次数
- 253表示使用payload的第一个字节作为时间间隔参数，单位是100ms，例如：mesg d4 253 f000 030201  表示300毫秒发一次0201，mesg d4 253 f000 1e0201是3秒一次0201
- 254表示收到回复或者发送超时就再次发送
- 255表示每秒自动发送一次
3. 第三个参数是接收者地址，必须是四个字符如果设置为0000会使用音箱默认组播地址F000；
4. 第四个参数是发送的内容，例如010203就是发送0x01，0x02，0x03因此必须是偶数个0-f之间的字符；

#### 5.2.2 数据串口说明（UART1)
数据串口配置参考代码：
##### 5.2.2.1 数据串口初始化
```c

int genie_sal_uart_init(void)
{
     memset(frame_buff, 0, FRAME_BUFFER_SIZE);

     mcu_uart.port = GENIE_MCU_UART_PORT;
     mcu_uart.config.baud_rate = GENIE_MCU_UART_BAUDRATE;
     mcu_uart.config.mode = MODE_TX_RX;
     mcu_uart.config.flow_control = FLOW_CONTROL_DISABLED;
     mcu_uart.config.stop_bits = STOP_BITS_1;
     mcu_uart.config.parity = NO_PARITY;
     mcu_uart.config.data_width = DATA_WIDTH_8BIT;

     if (0 != hal_uart_init(&mcu_uart))
     {
          printf("uart(%d) init fail\r\n", GENIE_MCU_UART_PORT);
          return -1;
     }
     else
     {
          p_mcu_uart = &mcu_uart;
     }

     return aos_timer_new(&uart_frame_end_timer, uart_frame_end_timer_cb, NULL, FRAME_TIEMOUT, 0);
}
```
##### 5.2.2.2 数据串口IO口配置

- TG7120B上数据串口配置

boards/TG7120B_evb/board_init.c
```c
#if defined(CONIFG_GENIE_MESH_BINARY_CMD) || defined(CONFIG_GENIE_MESH_AT_CMD)
    drv_pinmux_config(MCU_TXD, MCU_TXD_FUNC);
    drv_pinmux_config(MCU_RXD, MCU_RXD_FUNC);
    uart_csky_register(MCU_UART_IDX);
#endif
```
boards/TG7120B_evb/include/board_config.h
```c
#if defined(CONIFG_GENIE_MESH_BINARY_CMD) || defined(CONFIG_GENIE_MESH_AT_CMD)
#define MCU_UART_IDX 1
#define MCU_TXD P18
#define MCU_RXD P20
#define MCU_TXD_FUNC FMUX_UART1_TX
#define MCU_RXD_FUNC FMUX_UART1_RX
#endif
```

- TG7121B上数据串口配置

boards/TG7121B_evb/src/board_init.c
```c
#if defined(CONIFG_GENIE_MESH_BINARY_CMD) || defined(CONFIG_GENIE_MESH_AT_CMD)
    uart2_io_init(PB08, PB09);
#endif
```

#### 5.3.宏定义说明
##### 5.3.1 透传固件相关功能宏定义
下面三个宏定义请只开启其中一个

| 宏定义的名称 | 功能说明 |
| --- | --- |
| CONFIG_GENIE_MESH_AT_CMD | 使用AT指令协议，参考5.4 |
| CONIFG_GENIE_MESH_BINARY_CMD | 使用二进制串口协议，参考5.5 |
| CONIFG_GENIE_MESH_USER_CMD | 使用用户自定义串口协议 |



##### 5.3.2 其他通用功能相关宏定义
| 宏定义的名称 | 功能说明 |
| --- | --- |
| CONFIG_BT_MESH_GATT_PROXY | 支持Proxy功能 |
| CONFIG_BT_MESH_PB_GATT | 支持手机配网功能 |
| CONFIG_BT_MESH_RELAY | 支持中继功能 |
| CONFIG_GENIE_OTA | 支持手机OTA功能 |
| CONFIG_GENIE_RESET_BY_REPEAT | 支持连续上电五次进入配网状态功能 |
| CONFIG_GENIE_MESH_NO_AUTO_REPLY | 控制在透传模式下，是否自动回复Attribute Status与属性变化后的Attribute Indication。这个可以由应用自行决定是否打开，默认会自动回复 |
| PROJECT_SW_VERSION | 配置版本号，OTA功能使用，int32数据类型 |
| CONFIG_PM_SLEEP | 支持低功耗功能
（芯片尚未完全适配，低功耗暂不推荐打开） |
| CONFIG_GENIE_MESH_GLP | 支持GLP模式的低功耗功能
（芯片尚未完全适配，低功耗暂不推荐打开） |
| CONFIG_DEBUG | 支持BT_xxx日志输出 |
| CONFIG_BT_DEBUG_LOG | 支持BT_DBG日志输出 |
| MESH_DEBUG_PROV | 支持配网日志输出 |
| MESH_DEBUG_TX | 支持Access层发送Mesh数据日志输出 |
| MESH_DEBUG_RX | 支持Access层接收Mesh数据日志输出 |


### 5.4 基于透传固件开发产品
#### 5.4.1 产品规范
接入天猫精灵的蓝牙mesh智能设备的属性、事件和场景模式等定义请参考：

- [设备属性表](https://help.aliyun.com/document_detail/173320.html?spm=a2c4g.11186623.2.18.7bb243677NmzaS#task-2542342)
- [设备事件表](https://help.aliyun.com/document_detail/173319.html?spm=a2c4g.11186623.6.684.50e22409bveDA7#title-8ux-4kp-0ph)
- [设备场景模式表](https://help.aliyun.com/document_detail/173319.html?spm=a2c4g.11186623.6.684.50e22409bveDA7#title-uwd-gjd-kz2)

如果属性，事件，场景模式无法满足产品开发需求，请及时与我们联系添加新的属性，事件，场景模式类型。(aligenie.iot@list.alibaba-inc.com)
#### 5.4.2 模型配置
通用开关服务模型（Generic OnOff Server）、场景控制服务模型（Scene CTL Server）与阿里的厂商自定义模型（Vendor Model）结合基本能满足大多数品类设备的需求。

| 

                       Primary Element | Configuration Server |
| --- | --- |
|  | Health Server |
|  | Generic OnOff Server |
|  | Scene CTL  Server（可选） |
|  | Vendor Model |



#### 5.4.3 开发不同品类产品
##### 5.4.3.1 创建与定义产品
请登录生活物联网平台创建项目，参考生活物联网平台[用户指南-创建项目](https://help.aliyun.com/document_detail/126400.html)。注意选择“天猫精灵生态项目”。
创建完项目后接着创建产品，参考[用户指南-创建产品](https://help.aliyun.com/document_detail/126542.html?spm=a2c4g.11186623.2.18.657923a8o9WcZH#task-1322575)。注意以下事项：

- **是否接入网关**配置为**是，**天猫精灵生态项目下创建的产品，接入网关协议可以选择**BLE Mesh**或者**BLE GATT**。
- **接入网关协议**配置为**BLE Mesh**时，可以根据产品类型选择**低功耗**与**非低功耗**。如选择**低功耗，**按照[精灵低功耗(GLP)](https://help.aliyun.com/document_detail/173310.html?spm=a2c4g.11186623.6.675.3e5b4367xlgBXe#title-oxu-qw4-1ll)方案，天猫精灵音箱在给此产品设备发送数据的时候，会在1.2s的时间内持续不断地发送数据。
- 数据格式如果选择**透传/自定义**，需要在云端实现自定义格式数据转换的脚本，并且设备与云端通信时使用[Vendor message透传消息](https://help.aliyun.com/document_detail/173311.html?spm=a2c4g.11186623.6.676.70cc7ceeyNuxam#title-ek1-xrb-znl)。


完成产品功能定义-人机交互-设备调试流程，申请该品类产品的测试三元组，烧录到模组中。

##### 5.4.3.2 选择适用的通用固件
修改文件：solutions/genie_mesh_node_ctl/package.yaml里面的宏定义配置使用相应的协议，参考5.3
​

##### 5.4.3.3 配置组播地址
注意不同品类的设备有不同的产品组播地址，genie_mesh_node_ctl应用支持通过AT指令或者二进制串口指令保存组播地址。请在烧录固件、MAC地址和三元组之后配置对应品类的组播地址。
配置组播地址的指令请参考5.5.2.8 与5.6.4.6。
[组播地址的定义参考](https://help.aliyun.com/document_detail/173319.html?spm=a2c4g.11186623.6.684.50e22409bveDA7#title-7k3-0xy-xyl)



### 5.5 模组AT协议
#### 5.5.1 AT协议约定
**串口设置**
默认波特率9600bps，8位数据位，无奇偶校验，1位停止位。


**AT指令格式**

| **符号** | **描述** |
| --- | --- |
| **AT** | 每个AT指令，都以AT开头，ASCII码，不区分大小写 |
| **+X1 ** | +X1为命令 |
| **=** | 说明当前为设置操作，例如：AT+MESHADV=0 |
| **?** | 说明当前为读取操作，例如：AT+MESHADV? |
| **,** | 参数分隔符，可能带多个参数，例如：AT+MESHGRP=0xC000,0xCFFF |
| **<CR><CF>** | 回车符,命令结束 |

注意：AT指令以回车为结束符，即(’\r’)或者（0x0D）。
**AT指令返回值**

| **返回结果** | **描述** |
| --- | --- |
| **OK** | 串口命令执行成功 |
| **+ERROR:Y1** | 执行错误，Y1为错误码，如下：
-1 - 表示命令错误
-2 - 表示参数错误
-3 - 表示执行错误 |
| **+X1:Y1..** | 命令X1对应的响应结果 |



#### 5.5.2 AT指令集 
##### 5.5.2.1 重启设备
| **重启设备** |  |  |
| --- | --- | --- |
| **描述** | mesh设备重启 |  |
| **命令** | AT+REBOOT
 |  |
| **响应1**  | OK  | 重启成功  |
| **响应2** | +ERROR:Y1 | 返回错误  |
| **示例 ** | AT+REBOOT
OK | 重启设备成功 |



##### 5.5.2.2 清除配网信息
| **清除配网信息** |  |  |
| --- | --- | --- |
| **描述** | 清除mesh设备配网信息 |  |
| **命令** | AT+MESHRST
 |  |
| **响应1**  | OK  | 清除成功  |
| **响应2** | +ERROR:Y1 | 返回错误  |
| **示例 ** | AT+MESHRST
OK | 复位清除配网信息成功，并重启设备 |



##### 5.5.2.3 获取固件版本号
| **获取固件版本号** |  |  |
| --- | --- | --- |
| **描述** | 获取mesh设备固件版本号 |  |
| **命令** | AT+MESHVER?
 |  |
| **响应1**  | +MESHVER:Y1
OK | 获取成功，返回结果Y1
Y1为当前固件版本号（4个字节） |
| **响应2** | +ERROR:Y1 | 返回错误  |
| **示例 ** | AT+MESHVER?
+MESHVER:00010104
OK | 成功获取设备固件版本号：00010104 |



##### 5.5.2.4 获取设备配网状态信息
| **获取设备配网状态信息** |  |  |
| --- | --- | --- |
| **描述** | 获取mesh设备当前配网状态 |  |
| **命令** | AT+MESHINF? 
 |  |
| **响应1**  | +MESHINF:Y1
OK | 获取成功，返回结果
Y1，表示当前配网状态，返回值范围：
0 - 表示未配网
1 - 表示已配网 |
| **响应2** | +ERROR:Y1 | 返回错误  |
| **示例 ** | AT+MESHINF? 
+MESHINF:1
OK | 返回结果1，表示设备已配网 |



##### 5.5.2.5 设置蓝牙mesh广播类型
| **设置蓝牙mesh广播类型** |  |  |
| --- | --- | --- |
| **描述** | 控制mesh设备mesh广播类型 |  |
| **命令** | AT+MESHADV=<mode>
 | <mode>为mesh广播类型，取值范围：
0 - 关闭mesh广播
1 - 开启mesh广播
2 - 开启静默广播 |
| **响应1**  | OK | 设置成功 |
| **响应2** | +ERROR:Y1 | 返回错误  |
| **示例 ** | AT+MESHADV=1
OK | 设置开启mesh广播成功 |



##### 5.5.2.6 获取蓝牙mesh广播类型
| **获取蓝牙mesh广播类型** |  |  |
| --- | --- | --- |
| **描述** | 获取mesh设备当前mesh广播类型 |  |
| **命令** | AT+MESHADV?
​
 | ​
 |
| **响应1**  | +MESHADV:Y1
OK | 获取成功，返回结果
Y1，表示设备当前mesh广播类型，返回值范围：
0 - 表示已关闭mesh广播
1 - 表示已开启mesh广播
2 - 表示已开启静默广播 |
| **响应2** | +ERROR:Y1  | 返回错误  |
| **示例 ** | AT+MESHADV?
+MESHADV:1
OK | 获取设备已开启mesh广播 |



##### 5.5.2.7 设置测试模式
| **设置测试模式** |  |  |
| --- | --- | --- |
| **描述** | 设置mesh设备进入测试模式，进行设备扫描，并获取指定MAC地址设备的信号强度 |  |
| **命令** | AT+MESHTEST=<testmode>,[param]
​
 | <testmode>为测试类型，目前可支持的测试类型值：
0 - 表示进入测试模式并获取指定MAC地址设备的信号强度
[param]为其对应的测试类型所需的参数，当testmode为0时，此值为设备MAC地址，格式为：xx:xx:xx:xx:xx:xx |
| **响应1**  | 
+MESHTEST:Y1,Y2
OK | 获取成功，返回结果
Y1，表示设备当前测试类型
Y2，表示所指定MAC地址设备的信号强度 |
| **响应2** | +ERROR:Y1 | 返回错误  |
| **示例 ** | AT+MESHTEST=0,88:8c:f3:70:5f:55
+MESHTEST:0,-93
OK | 设备进入测试模式，进行设备扫描，并成功获取到88:8c:f3:70:5f:55设备的信号强度为-93 |

##### 5.5.2.8 设置组播地址
| **设置组播地址** |  |  |
| --- | --- | --- |
| **描述** | 设置mesh设备组播地址 |  |
| **命令** | AT+MESHGRP=<addr1>,[addr2]...
 | <addr1>为组播地址，格式为：0xXXXX，例如：0xC000，取值范围: [0xC000~0xCFFF] |
| **响应1**  | 
OK | 设置成功 |
| **响应2** | +ERROR:Y1 | 返回错误  |
| **示例 ** | AT+MESHGRP=0xC000,0xCFFF
OK | 成功设置设备两个组播地址0xC000、0xCFFF |


##### 5.5.2.9 获取组播地址
| **获取组播地址** |  |  |
| --- | --- | --- |
| **描述** | 获取mesh设备组播地址 |  |
| **命令** | AT+MESHGRP?
​
 | ​
 |
| **响应1**  | +MESHGRP:Y1,Y2
OK | 获取成功，返回结果
Y1，表示组播地址1
Y2，表示组播地址2 |
| **响应2** | +ERROR:Y1  | 返回错误  |
| **示例 ** | AT+MESHGRP?
+MESHGRP:0xC000,0xCFFF
OK | 成功返回设备两个组播地址0xC000、0xCFFF |



##### 5.5.2.10 透传数据包发送
| **透传数据包发送** |  |  |
| --- | --- | --- |
| **描述** | 模组接收外部MCU串口透传数据包，通过蓝牙mesh转发到空口 |  |
| **命令** | AT+MESHMSGTX=<data>
​
 | <data>为二进制格式的数据包，例如：D4A801A500010021010000 |
| **响应1**  | OK | 发送成功 |
| **响应2** | +ERROR:Y1 | 返回错误  |
| **示例 ** | AT+MESHMSGTX=D4A801A500010021010000
OK | 接收到串口透传数据包：D4A801A500010021010000，通过蓝牙mesh成功转发到空口 |



##### 5.5.2.11 透传数据包接收
| **透传数据包接收** |  |  |
| --- | --- | --- |
| **描述** | 模组接收应用层透传数据包，通过串口转发给外部MCU |  |
| **命令** | +MESHMSGRX:<len>,<data> | <len>为接收到数据包的长度（字节数），<data>为二进制格式的数据包 |
| **示例 ** | +MESHMSGRX:6,820201624100 | 成功接收到应用层透传数据包：820201624100，通过串口转发 |



##### 5.5.2.12 设备事件上报
| **设备事件上报** |  |  |
| --- | --- | --- |
| **描述** | 设备事件上报 |  |
| **命令** | +MESHEVT:<state> | <state>为上报的设备事件，取值范围：
0x04:设备启动
0x00:配网成功
0x05:配网失败 |
| **示例 ** | +MESHEVT:0x04 | 设备启动事件上报 |



##### 5.5.2.13 获取设备MAC地址
| **获取设备MAC地址** |  |  |
| --- | --- | --- |
| **描述** | 获取设备MAC地址 |  |
| **命令** | AT+MESHMAC? | ​
 |
| **响应1**  | +MESHMAC:Y1
OK | 获取成功，返回结果Y1
Y1为设备MAC地址 |
| **响应2** | +ERROR:Y1 | 返回错误  |
| **示例 ** | AT+MESHMAC?
+MESHMAC: 28:fa:7a:33:d9:cf
OK | 成功获取设备MAC地址：28:fa:7a:33:d9:cf |


### 5.6 模组串口协议
#### 5.6.1 通信协议
**串口设置**
默认波特率9600bps，8位数据位，无奇偶校验，1位停止位。


**指令帧结构**

| 帧构成 | 占用字节数 | 备注 |
| --- | --- | --- |
| 数据类型 | 2 |  参照表2 |
| 数据长度 | 2 |   |
| 数据内容/参数 | 1-N |  |
| 校验码 | 1 | 累加求和，计算范围包括数据长度和数据内容 |

注：每条指令之间间隔至少60ms。
注：当设备主动发送指令时，必须等待接收到模块的回复指令后才可以发送下一条指令。


**指令类型**

|  | 数据类型 | 参数 |
| --- | --- | --- |
| 0xFF00 | mesh透传数据 |  |
| 0xFE00 | 控制指令 | 指令详情见下节 |

#### 5.6.2 控制指令
|  | 控制指令 | 参数 |
| --- | --- | --- |
| 0x01 | 控制蓝牙mesh广播 | 0x00:关闭
0x01:开启mesh广播
0x02:开启静默广播 |
| 0x02 | 清除配网信息 | 无 |
| 0x03（这个是模组的返回数据） | 设备事件/设备信息 | 0x00:配网完成
0x01:清除配网信息（0x01成功，0x00失败）
0x02+MAC地址+RSSI:对应MAC地址的信号强度
0x03+配网状态:设备配网状态（已配网为0x01，未配网为0x00）
0x04:设备启动
0x05:配网失败
0x06+版本号:模组固件版本号
0xFF:设备错误 |
| 0x04 | 设置获取蓝牙信号强度 | 0x00+MAC地址:获取设备扫描对应MAC地址的信号强度 |
| 0x05 | 查询设备信息 | 0x00:设备配网状态
0x01:模组版本号 |
| 0x06 | 重启设备 | 无 |
| 0x07 | 更新组播地址 | 8字节，每2个字节组成一个组播地址，小端格式，有效的组播地址要求最高2位为1 |
| 0x08 | 更新组播地址返回 | 更新组播地址的返回指令。
0x00:更新组播地址成功
0x01:更新组播地址失败 |
| 0x09 | 读取组播地址 | 无 |
| 0x0A | 读取组播地址返回 | byte0:
      0x00:读取组播地址成功。
      0x01:读取组播地址失败。
byte1-byte8:
      8字节，每2个字节组成一个组播地址，小端          格式。 |
| 0x0B | 设置进入产测模式 | 无参数。
回复：
成功：0x0B 0x00
失败：0x0B 0x01 |
| 0x0C | 开关串口日志输出功能 | 0x01：允许输出串口日志
0x00：禁止输出串口日志
回复
成功：0x0C 0x00
失败：0x0C 0x01 |



#### 5.6.3 透传数据示例
**上报开关属性（开）**

| 0xFF | 0x00 | 0x00 | 0x07 | 0xd4 | 0xa8 | 0x01 | 0xff | 0x00 | 0x01 | 0x01 | 0x85 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| mesh透传数据 |  | 数据长度 |  | opcode：d401a8 |  |  | tid | 0x0100 开关属性 |  | 开关开 | 求和校验 |

**上报开关属性（关）**

| 0xFF | 0x00 | 0x00 | 0x07 | 0xd4 | 0xa8 | 0x01 | 0x09 | 0x00 | 0x01 | 0x00 | 0x8E |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| mesh透传数据 |  | 数据长度 |  | opcode：d401a8 |  |  | tid | 0x0100 开关属性 |  | 开关关 | 求和校验 |



#### 5.6.4 控制指令示例
##### 5.6.4.1 打开蓝牙广播
| 0xFE | 0x00 | 0x00 | 0x02 | 0x01 | 0x01 | 0x04 |
| --- | --- | --- | --- | --- | --- | --- |
| 控制指令 |  | 数据长度 |  | 控制蓝牙mesh广播 | 开启mesh广播 | 求和校验 |

##### 5.6.4.2 清除配网信息
| 0xFE | 0x00 | 0x00 | 0x01 | 0x02 | 0x03 |
| --- | --- | --- | --- | --- | --- |
| 控制指令 |  | 数据长度 |  | 清除配网信息 | 求和校验 |

##### 5.6.4.3 测试模式获取信号强度
| 0xFE | 0x00 | 0x00 | 0x08 | 0x04 | 0x00 | 0x11 | 0x22 | 0x33 | 0x44 | 0x55 | 0x66 | 0x70 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 控制指令 |  | 数据长度 |  | 测试模式 | 获取信号强度 | MAC地址 |  |  |  |  |  | 求和校验 |

##### 5.6.4.4 测试模式信号强度
| 0xFE | 0x00 | 0x00 | 0x09 | 0x03 | 0x02 | 0x11 | 0x22 | 0x33 | 0x44 | 0x55 | 0x66 | 0x10 | 0x84 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 控制指令 |  | 数据长度 |  | 设备信息 | 信号强度 | MAC地址 |  |  |  |  |  | RSSI | 求和校验 |

##### 5.6.4.5 查询设备配网状态
| 0xFE | 0x00 | 0x00 | 0x02 | 0x05 | 0x00 | 0x07 |
| --- | --- | --- | --- | --- | --- | --- |
| 控制指令 |  | 数据长度 |  | 查询设备信息 | 配网状态 | 求和校验 |

##### 5.6.4.6 更新组播地址
| 0xFE | 0x00 | 0x00 | 0x09 | 0x07 | 0x12 | 0xc1 | 0x00 | 0xc0 | 0x11 | 0xc1 | 0x75 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 控制指令 |  | 数据长度 |  | 更新组播地址 | 组播地址 |  |  |  |  |  | 求和校验 |

| 0xFE | 0x00 | 0x00 | 0x02 | 0x08 | 0x00 | 0x0a |
| --- | --- | --- | --- | --- | --- | --- |
| 控制指令 |  | 数据长度 |  | 更新组播地址状态 | 更新状态 | 求和校验 |

##### 5.6.4.7 读取组播地址
| 0xFE | 0x00 | 0x00 | 0x01 | 0x09 | 0x0a |
| --- | --- | --- | --- | --- | --- |
| 控制指令 |  | 数据长度 |  | 读取组播地址 | 求和校验 |

| 0xFE | 0x00 | 0x00 | 0x0a | 0x0a | 0x00 | 0x12 | 0xc1 | 0x00 | 0xc0 | 0x11 | 0xc1 | 0x00 | 0x00 | 0x79 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 控制指令 |  | 数据长度 |  | 读取组播地址返回 | 返回状态 | 组播地址 |  |  |  |  |  |  |  | 求和校验 |

#### 5.6.5 模组回复错误码格式和定义
| 0x55 | 0x55 | 0x00 | 0x01 | XX | XX |
| --- | --- | --- | --- | --- | --- |
| 控制指令 |  | 数据长度 |  | 错误码 | 校验和 |

如上所示第五个字节为错误码，具体错误码内容定义如下：

| 错误码 | 含义 | 备注 |
| --- | --- | --- |
| 0x01 | 数据总长度小于五个字节 | 数据总长度错误 |
| 0x02 | 数据长度错误 | 解析len的值与数据总的长度不一致 |
| 0x03 | 校验和错误 |  |
| 0x04 | 获取RSSI的输入有误 |  |
| 0x05 | 获取RSSI失败 | 通常是BLE错误 |
| 0x06 | 获取RSSI MAC不匹配 |  |
| 0x07 | 更新组播地址长度错误 |  |
| 0x08 | 包含无效的组播地址 | 组播地址范围在C000-CFFF之间，字节顺序是低字节在前面 |
| 0x09 | 更新组播地址Flash错误 |  |
| 0x0A | 启动产测模式失败 |  |
| 0x80 | 透传发送失败 |  |
| 0xFF | 未知错误 |  |



### 5.7 用户自定义串口
#### 5.7.1 模组端接收MCU的数据
模组接收MCU的数据需要用户自己开发解析代码，可以参考CONIFG_GENIE_MESH_BINARY_CMD的实现方式，模组中发送Mesh数据调用函数int genie_sal_ble_send_msg(uint8_t element_id, uint8_t* p_data, uint8_t len)发送上行数据（如果没有特殊需求，请将**request_msg.tid**的值设置为0,即request_msg.tid = p_data[3]）

- 关于参数element_id支持开发多element的产品使用，如果是单个element那就是0



#### 5.7.2 模组收到的下行数据发给MCU
接收数据的地方如下：
```c
#ifdef CONIFG_GENIE_MESH_USER_CMD
    case GENIE_EVT_DOWN_MSG:
    {
        genie_down_msg_t *p_msg = (genie_down_msg_t *)p_arg;
        //User handle this msg,such as send to MCU
        if (p_msg)
        {
        }
    }
    break;
#endif
```

## 6.其他参考文档
- [蓝牙mesh智能家居产品规范](https://help.aliyun.com/document_detail/173319.html?spm=a2c4g.11186623.6.694.77dc41f0KNwhT0)

- [蓝牙设备属性表](https://help.aliyun.com/document_detail/173320.html?spm=a2c4g.11186623.6.694.51385ed8bpGvnn)

- [蓝牙Mesh模组软件规范](https://help.aliyun.com/document_detail/173310.html?spm=a2c4g.11174283.6.689.2d197132x9Gqoi)

- [蓝牙Mesh设备扩展协议](https://help.aliyun.com/document_detail/173311.html?spm=a2c4g.11186623.6.685.6e5c59eev3fdLS)

- [蓝牙mesh设备开发FAQ](https://help.aliyun.com/document_detail/173313.html?spm=a2c4g.11186623.6.686.6fdb14262iF9B3)

