AT串口指令
=============

AT指令是指在命令模式下用户通过 UART 与模块进行命令传递，以达到查询或者设置模块的某些配置。


一、串口配置
----------------

默认使用PB08、PB09作为AT指令的通讯串口，其中PB08为模块的TX，PB09为模块的RX，默认的uart口的配置参数为： 波特率 115200、无校验、8位数据位、1位停止位。


二、AT指令格式
----------------

AT+指令可以直接通过常用的串口的调试助手程序进行输入，AT+指令采用基于 ASCII 码的命令。

2.1 格式说明
+++++++++++++
 | < >: 表示必须包含的部分
 | [ ]: 表示可选部分

2.2 命令消息
+++++++++++++
 | AT+<CMD>[op][para-1,para-2,para-3,para-4…]<CR><LF>
 | AT+:      命令消息前缀
 | [op]:     指令操作符，指定是参数设置或查询； ``=`` 表示参数设置， ``?`` 表示查询
 | [para-n]: 参数设置时的输入，如果查询则不需要
 | <CR>:     结束符，回车，ASCII码0x0D
 | <LF>:     结束符，回车，ASCII码0x0A 

2.3 响应消息
+++++++++++++
 | <CR><LF>+<RSP>[op] [para-1,para-2,para-3,para-4…]<CR><LF>
 | +:        响应消息前缀
 | RSP:      响应字符串，包括：``OK`` 表示成功, ``ERR`` 表示失败
 | [para-n]: 查询时返回参数或出错时错误码
 | <CR>:     结束符，回车，ASCII码0x0D
 | <LF>:     结束符，换行，ASCII码0x0A

三、指令描述
--------------

3.1 AT+NAME
++++++++++++++
 | 功能： 查询/设置模块的名称
 | 格式：
 | 查询当前参数值：AT+NAME?{CR}{LF}
 | 回应：{CR}{LF}+NAME:name{CR}{LF}OK{CR}{LF}
 | 设置：AT+NAME=name{CR}{LF}
 | 回应：{CR}{LF}+NAME:name{CR}{LF}OK{CR}{LF}
 | 参数：
 | name： 模块的名称
 | 设置举例：设置模块名称为 AT_TEST， 则需设置如下 ``AT+NAME=AT_TEST{CR}{LF}``

3.2 AT+MAC
++++++++++++++
 | 功能： 查询/设置模块的MAC地址
 | 格式：
 | 查询当前参数值：AT+MAC?{CR}{LF}
 | 回应：{CR}{LF}+MAC:mac{CR}{LF}OK{CR}{LF}
 | 设置：AT+MAC=mac{CR}{LF}
 | 回应：{CR}{LF}+MAC:mac{CR}{LF}OK{CR}{LF}
 | 参数：
 | mac： 模块的MAC地址，
 | 设置举例：设置模块地址在手机上显示效果为C00000000001,则需设置如下 ``AT+MAC=0100000000C0{CR}{LF}``

3.3 AT+ADVINT
++++++++++++++
 | 功能： 查询/设置广播间隔
 | 格式：
 | 查询当前参数值：AT+ADVINT?{CR}{LF}
 | 回应：{CR}{LF}+ADVINT:set{CR}{LF}OK{CR}{LF}
 | 设置：AT+ADVINT=set{CR}{LF}
 | 回应：{CR}{LF}+ADVINT:set{CR}{LF}OK{CR}{LF}
 | 参数：
 | set： 模块的广播间隔

    - 0：50ms
    - 1：100ms 
    - 2：200ms
    - 3：500ms
    - 4：1000ms
    - 5：2000ms

3.4 AT+ADV
++++++++++++++
 | 功能： 查询/设置广播工作状态
 | 格式：
 | 查询当前参数值：AT+ADV?{CR}{LF}
 | 回应：{CR}{LF}+ADV:set{CR}{LF}OK{CR}{LF}
 | 设置：AT+ADVINT=set{CR}{LF}
 | 回应：{CR}{LF}+ADVINT:set{CR}{LF}OK{CR}{LF}
 | 参数：
 | set： 模块的广播状态

    - B: 广播开启
    - I: 广播空闲

3.5 AT+Z
++++++++++++++
 | 功能： 控制模块重启
 | 格式：
 | 查询当前参数值：AT+Z?{CR}{LF}
 | 回应：{CR}{LF}+Z:set{CR}{LF}OK{CR}{LF}

3.6 AT+LINK
++++++++++++++
 | 功能：查询模块的已连接的链路
 | 格式：
 | 查询当前参数值：AT+LINK?{CR}{LF}
 | 回应：{CR}{LF}+LINK{CR}{LF}OK{CR}{LF}
 | Link_ID：{SPACE}ID{SPACE}LinkMode:MODE{SPACE}PeerAddr:MMAC{CR}{LF}
 | 参数：

    - ID：连接号 
    - LinkMode：在链接中的角色，M表示做为Master，S表示做为Slaver
    - MAC：已连接设备的地址


3.7 AT+CONN
++++++++++++++
 | 功能：设置连接指定MAC值的设备
 | 格式：
 | 设置当前参数值：AT+CONN=mac{CR}{LF}
 | 回应：{CR}{LF}+CONN:mac{CR}{LF}OK{CR}{LF}
 | 参数：
 
    - mac：要连接的设备MAC值

3.8 AT+DISCONN
+++++++++++++++
 | 功能：设置断开当前连接
 | 格式：
 | 设置：AT+DISCONN=con_idx{CR}{LF}
 | 回应：{CR}{LF}+DISCONN: con_idx {CR}{LF}OK{CR}{LF}
 | 参数：

    - con_idx：断开连接的连接号或字符'A', ``A`` 表示断开当前所有连接

3.9 AT+FLASH
++++++++++++++
 | 功能：保存通过控制指令设置的参数到FLASH中
 | 格式：
 | 设置：AT+FLASH{CR}{LF}
 | 回应：{CR}{LF}+FLASH{CR}{LF}OK{CR}{LF}

3.10 AT+SEND
++++++++++++++
 | 功能： 通过某个连接发送数据到对端
 | 格式：
 | 设置：AT+SEND=con_idx,len{CR}{LF}
 | 回应：{CR}{LF}>{CR}{LF}
 | 参数：

    - con_idx: 要发送数据的链接号，从AT+LINK?的回复中得知
    - len：本次要发送数据的长度

*本条命令发送完毕，设备回复 > ，表示设备进入单次透传模式，在设备发送完 len 指定的数据长度之前，不解析命令。发送的数据达到 len 指定长度时，退出单次透传模式*

3.11 AT++++
++++++++++++++
 | 功能： 控制模块进入透传模式，仅在单连接时有用，此时不会解析AT指令
 | 格式：
 | 设置：AT++++{CR}{LF}
 | 回应：{CR}{LF}+++{CR}{LF}ret{CR}{LF}
 | 参数：

    - ret:模块进入透传的结果, ``OK`` 成功， ``ERR`` 失败

3.12 +++
++++++++++++++
 - 在透传模式下，发送三个字符+++，+++前面没有字符，在500ms之内后面也没有其他字符，即可退出透传模式进入命令模式
 - 在单连接时，如果有第二个连接建立。设备会自动退出透传模式，进入命令模式

3.13 AT+AUTO+++
+++++++++++++++++
 | 功能： 查询/设置模块在连接上后是否自动进入透传模式
 | 格式：
 | 查询当前参数值：AT+AUTO+++?{CR}{LF}
 | 回应：{CR}{LF}+AUTO+++:set{CR}{LF}OK{CR}{LF}
 | 设置：AT+AUTO+++=set{CR}{LF}
 | 回应：{CR}{LF}+AUTO+++:set {CR}{LF}OK{CR}{LF}
 | 参数：

    - set：``Y`` 模块连接后自动进入透传， ``N`` 不会自动进入透传


3.14 AT+POWER
+++++++++++++++++++++++++
 | 功能： 查询/设置模块的射频功率
 | 格式：
 | 查询当前参数值：AT+POWER?{CR}{LF}
 | 回应：{CR}{LF}+POWER:set{CR}{LF}OK{CR}{LF}
 | 设置：AT+POWER=set{CR}{LF}
 | 回应：{CR}{LF}+POWER:set{CR}{LF}OK{CR}{LF}
 | 参数：
 | set： 设置模块的发射功率

    - 0：0dBm
    - 1：-20dBm
    - 2：-8dBm
    - 3：4dBm
    - 4：10dBm
    - 5：12dBm

3.15 AT+SLEEP [**TODO**]
++++++++++++++++++++++++
