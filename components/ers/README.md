# 概述

at ERS 是基于at命令服务的一种远程服务。

# 接口定义

## 指令接口配置

指令的接口全都放在init目录的at_cmd.c中，通过配置`atserver_cmd_t at_cmd[]`数组来配置可执行的AT命令。

## 指令接口

```c
typedef struct atserver_cmd {
    const char      *cmd;
    cmd_callback    cb;
    void            *priv;
    char            use_heap;
    slist_t         next;
} atserver_cmd_t;
```

AT的所有命令格式都制定在 atserver_cmd_t 这个结构体里。

* 参数:
  * cmd: 命令字符串
  * cb: 命令执行函数
  * priv: 其他参数指针
  * use_heap: 堆标记
  * next: 命令链表指针

## 指令示意

```c
const atserver_cmd_t at_cmd[] = {
    {"AT", at_cmd_at, NULL},
    {"AT+HELP", at_cmd_help, NULL},
    {"AT+CGMR", at_cmd_cgmr, NULL},
    {"AT+FWVER", at_cmd_fwver, NULL},
    {"AT+SYSTIME", at_cmd_systime, NULL},
    {"AT+SAVE", at_cmd_save, NULL},
    {"AT+FACTORY", at_cmd_factory, NULL},
    {"AT+REBOOT", at_cmd_reboot, NULL},
    {"AT+EVENT", at_cmd_event, NULL},
    {"AT+ECHO", at_cmd_echo, NULL},
    {NULL, NULL, NULL},
};
```

其中`{"AT", at_cmd_at, NULL}`,`"AT"`为指令名称，`at_cmd_at`为指令回调函数，`priv`参数为`NULL`。

# 指令定义

## 指令类型

AT指令包括四种类型的指令，测试指令，查询指令，设置指令，执行指令。

| 类型     | 指令格式       | 描述                                                       |
| -------- | -------------- | ---------------------------------------------------------- |
| 测试指令 | AT+[x]=?      | 该命令用于查询设置指令的参数以及取值范围。     |
| 查询指令 | AT+[x]?       | 该命令用于返回参数的当前值。                              |
| 设置指令 | AT+[x]=[...] | 该命令用于设置用户⾃自定义的参数值。                      |
| 执行指令 | AT+[x]        | 该命令用于执⾏行行受模块内部程序控制的变参数不不可变的功能。 |

所有指令以[CR]结尾

## 指令回应

| 格式                        | 格式描述             |
| --------------------------- | -------------------- |
| \r\nOK\r\n                  | 执行成功             |
| \r\n+CMD:[x]\r\nOK\r\n     | 执行成功并返回结果   |
| \r\n+CMD:-[x]\r\nERROR\r\n | 执行失败并返回错误码 |

所有响应以[CR][LF]结尾

## 异步事件

| 格式             | 格式描述                 |
| ---------------- | ------------------------ |
| +EVENT=NAME,[x] | 返回某项事件，并携带参数 |

所有事件以[CR][LF]结尾

# AT指令集

## 基础AT指令

### 测试AT启动

命令字符串：AT

| 执行指令 | 响应   | 参数说明 |
|---------|--------|----------|
| AT      | \r\nOK | AT       |

### 显示AT帮助指令

命令字符串：AT+HELP

| 执行指令 | 响应                                  | 参数说明              |
|---------|---------------------------------------|-------------------|
| AT+HELP | \r\nAT+HELP:[command list]\r\nOK\r\n | command list：指令列表 |

### 查询模块版本

命令字符串：AT+CGMR

| 执行指令 | 响应                                    | 参数说明                  |
|---------|-----------------------------------------|-----------------------|
| AT+CGMR | \r\nAT+CGMR:[module version]\r\nOK\r\n | module version：AT模块版本 |

### 查询系统运行时间

命令字符串：AT+SYSTIME

| 执行指令    | 响应                           | 参数说明                  |
|------------|--------------------------------|-----------------------|
| AT+SYSTIME | \r\n+SYSTIME:[time]\r\nOK\r\n | time：系统运行时间，s为单位 |

### 查询三要素

命令字符串：AT+FACTORYR

| 执行指令                    | 响应                                   | 参数说明                                                                    |
|----------------------------|----------------------------------------|-------------------------------------------------------------------------|
| AT+FACTORYR?                | \r\n+FACTORYR:[factory info]\r\nOK\r\n | factory info: 出厂信息，包括设备名称和产品密钥                               |

### 设置三要素

命令字符串：AT+FACTORYW

| 执行指令                    | 响应                                   | 参数说明                                                                    |
|----------------------------|----------------------------------------|-------------------------------------------------------------------------|
| AT+FACTORYW=[key],[value] | \r\nOK\r\n                             | key: 暂时只支持PRODUCTKEY, DEVICENAME, DEVICESECRET 之一<br> value: key的值 |

### 重启模块

命令字符串：AT+REBOOT

| 执行指令   | 响应       | 参数说明 |
|-----------|------------|----------|
| AT+REBOOT | \r\nOK\r\n |          |

### 查询/设置是否开启事件通知功能

命令字符串：AT+EVENT

| 执行指令           | 响应                          | 参数说明                                 |
|-------------------|-------------------------------|--------------------------------------|
| AT+EVENT?         | \r\n+EVENT:[state]\r\nOK\r\n | OFF：关闭事件通知功能 ON：开启事件通知功能 |
| AT+EVENT=[state] | \r\nOK\r\n                    | OFF：关闭事件通知功能 ON：开启事件通知功能 |

### 查询/设置是否回显发送指令

命令字符串：AT+ECHO

| 执行指令          | 响应                         | 参数说明                 |
|------------------|------------------------------|----------------------|
| AT+ECHO?         | \r\n+ECHO:[state]\r\nOK\r\n | OFF：关闭回显 ON：开启回显 |
| AT+ECHO=[state] | \r\nOK\r\n                   | OFF：关闭回显 ON：开启回显 |

存储服务AT指令

### 获取键值(字符串)

命令字符串：AT+KVGET

| 执行指令         | 响应                          | 参数说明 |
|-----------------|-------------------------------|---------|
| AT+KVGET=[key] | \r\n+KVGET:[value]\r\nOK\r\n | key：键   |

### 设置键值(字符串)

命令字符串：AT+KVSET

| 执行指令               | 响应       | 参数说明                                                    |
|-----------------------|------------|---------------------------------------------------------|
| AT+KVSET=[key,value] | \r\nOK\r\n | key：键[长度不超过 32 字节] <br>value：值[长度不超过 32 字节] |

### 删除键值(字符串)

命令字符串：AT+KVDEL

| 执行指令         | 响应       | 参数说明                      |
|-----------------|------------|---------------------------|
| AT+KVDEL=[key] | \r\nOK\r\n | key：键[删除 key 对应的键值对] |

### 获取键值(整数型)

命令字符串：AT+KVGETINT

| 执行指令         | 响应                          | 参数说明 |
|-----------------|-------------------------------|---------|
| AT+KVGETINT=[key] | \r\n+KVGET:[value]\r\nOK\r\n | key：键 value: INT型值   |

### 设置键值(整数型)

命令字符串：AT+KVSETINT

| 执行指令               | 响应       | 参数说明                                                    |
|-----------------------|------------|---------------------------------------------------------|
| AT+KVSETINT=[key,value] | \r\nOK\r\n | key：键[长度不超过 32 字节] <br>value：值范围(-2147483648~2147483647) |

### 删除键值(整数型)

命令字符串：AT+KVDELINT

| 执行指令         | 响应       | 参数说明                      |
|-----------------|------------|---------------------------|
| AT+KVDELINT=[key] | \r\nOK\r\n | key：键[删除 key 对应的键值对] |

## socket管理AT指令

### 启动一个连接

命令字符串：AT+CIPSTART

| 执行指令 | 响应 | 参数说明 |
| - | - | - |
| AT+CIPSTART=[id],[type][,ip][,remote_port][,local_port] | \r\nOK\r\n | [id]：指定连接的 id 号,范围：0 - 2<br>[type]：连接类型，包括：<br>tcp_server,tcp_client, udp_unicast。分别如下：<br>- 当type为tcp_server时，参数为：<br>id,tcp_server,local_port<br>- 当type为tcp_client时，参数为：<br>id,tcp_client,ip,remote_port[,local_port]<br>- 当type为udp_unicast时，参数为：<br>id,udp_unicast,ip,remote_port,local_port<br>[ip]：目标服务器的 IP。当 type 为 tcp_server 时，不需要该参数。<br>[remote_port]：远程端口。<br>[local_port]：本地端口。 |



注意：

* 当前 id 的连接，如果不是关闭状态，再次设置并连接时，将无法成功执行并返回错误。
* 更改任何一个 id 的参数前，必须要手动关闭，即执行指令：AT+CIPSTOP=id\r，才能正确设置并连接。
* 连接结果通过CONNECT事件返回
* 当前tcp_server仅支持一个 client 连接
* 无线环境下有如下限制：
  * local_port 不起作用
  * 不能设置连接类型为 tcp_server

### 断开一个指定 id 的连接

命令字符串：AT+CIPSTOP

| 执行指令          | 响应       | 参数说明                                                           |
|------------------|------------|----------------------------------------------------------------|
| AT+CIPSTOP=[id] | \r\nOK\r\n | [id]：指定连接的 id 号 |

1. 网络指定3时，断开所有连接的 id
2. 断开结果通过 DISCONNECT 事件返回

### 查询/设置WiFi 数据发送至用户串口的配置参数

命令字符串：AT+CIPRECVCFG

| 执行指令 | 响应 | 参数说明 |
| - | - | - |
| AT+CIPRECVCFG? | \r\n+CIPRECVCFG:[recv mode]\r\n | [recv mode]:<br>0.表示raw透传模式，表示自动接收<br>1.表示cmd指令模式，需通过指令 AT+CIPRECV 接收 |
| AT+CIPRECVCFG=[recv mode] | \r\nOK\r\n | [recv mode]:<br>0.表示raw透传模式，表示自动接收<br>1.表示cmd指令模式，需通过指令 AT+CIPRECV 接收 |

默认是 cmd 指令模式
两种模式相比：
1.透传模式：简单，数据可及时接收
2.指令模式：符合一般编程习惯

### 查询&设置ID

命令字符串：AT+CIPID

| 执行指令        | 响应                       | 参数说明                          |
|----------------|----------------------------|-------------------------------|
| AT+CIPID=?     | \r\n+CIPID:0-2\r\nOK\r\n   | [id]：指定连接的 id 号,范围：0 - 2 |
| AT+CIPID?      | \r\n+CIPID:[id]\r\nOK\r\n | [id]：指定连接的 id 号,范围：0 - 2 |
| AT+CIPID=[id] | \r\nOK\r\n                 | [id]：指定连接的 id 号,范围：0 - 2 |

1. 默认值是0
2. 除了 CIPSTART / CIPSTOP / CIPRECVCFG指令，其他 AT 指令执行前，如果连接 id 号不为0的话，需要执行该指令

### 查询连接状态

命令字符串：AT+CIPSTATUS

| 执行指令 | 响应 | 参数说明 |
| - | - | - |
| AT+CIPSTATUS | \r\n+CIPSTATUS:[type],[status]\r\nOK\r\n | type：<br>- 指定 id 的连接类型，包括：<br>tcp_server,tcp_client,udp_unicast<br>status:<br>- start: 启动连接<br>- connect: 已连接 |

### 发送数据

命令字符串：AT+CIPSEND

| 执行指令 | 响应 | 参数说明 |
| - | - | - |
| AT+CIPSEND=[length],[data] | \r\n+CIPSEND:[len]\r\n | Length:发送的数据长度 最大数据长度为128<br>Data:发送数据<br>Len:返回当前发送的数据长度 |

### 接收数据

命令字符串：AT+CIPRECV

| 执行指令    | 响应                                        | 参数说明                                                       |
|------------|---------------------------------------------|------------------------------------------------------------|
| AT+CIPRECV | \r\n+CIPRECV:[id],[len],[data]\r\nOK\r\n | Id:当前的socket通道号<br>Len:接收的数据长度<br>Data:当前数据域 |

1. 同步接收数据
2. 返回实际接收的数据长度和数据(长度最大为 128)

## 数据上云服务AT指令

### 发布数据

命令字符串：AT+PUB

| 执行指令               | 响应       | 参数说明                                       |
|-----------------------|------------|--------------------------------------------|
| AT+PUB=[length,data] | \r\nOK\r\n | [length]：发布数据的长度<br>[data]：发布的数据 |

数据发送到云端
数据长度不能超过 128字节

### 查询 &&连接云通道

命令字符串：AT+ALIYUNCONN

| 执行指令 | 响应 | 参数说明 |
| - | - | - |
| AT+ALIYUNCONN | \r\nOK\r\n | 无 |
| AT+ALIYUNCONN? | \r\n+ALIYUNCONN=[sta]\r\n | [sta]:<br>connected = 0 (未连接)<br>connecting = 1 (已连接) |

建立连接

### 断开云通道

命令字符串：AT+ALIYUNDISCONN

| 执行指令          | 响应       | 参数说明 |
|------------------|------------|---------|
| AT+ALIYUNDISCONN | \r\nOK\r\n | 无       |

断开连接

## 事件通知

### 接收⽹络数据

命令字符串：+EVENT=RECV

| 执行指令                              | 响应 | 参数说明                                                          |
|--------------------------------------|------|---------------------------------------------------------------|
| +EVENT=RECV,[id],[len],[data]\r\n |      | [id]：连接的 id 号<br>[len]：接收的数据长度<br>[data]：接收的数据 |

工作在透传模式下

### 连接状态

命令字符串：+EVENT=CONNECT

| 执行指令                  | 响应 | 参数说明             |
|--------------------------|------|------------------|
| +EVENT=CONNECT,[id]\r\n |      | [id]：连接上的 id 号 |

### 断开连接状态

命令字符串：+EVENT=DISCONNECT

| 执行指令                     | 响应 | 参数说明               |
|-----------------------------|------|--------------------|
| +EVENT=DISCONNECT,[id]\r\n |      | [id]：断开连接的 id 号 |

### 上云推送结果

命令字符串：+EVENT=PUB

| 执行指令               | 响应 | 参数说明                                      |
|-----------------------|------|-------------------------------------------|
| +EVENT=PUB,[res]\r\n |      | [res]:返回上云推送结果，SUCC-成功 FAILED-失败 |

# 错误代码表

```c
#define AT_OK              	0     /*!< No error */
#define AT_ERR            	-1    /*!< General fail code */
#define AT_ERR_NO_KEY     	-2    /*!< Key not set */
#define AT_ERR_INVAL      	-3    /*!< Invalid argument */
#define AT_ERR_STATUS     	-4    /*!< Invalid status */
#define AT_ERR_RECV       	-5    /*!< socket API: Fail to recv data */
#define AT_ERR_SEND       	-6    /*!< socket API: Fail to send data */
#define AT_ERR_START      	-7    /*!< socket API: Fail to start */
#define AT_ERR_CHECK      	-8    /*!< data check error*/
#define AT_ERR_FOTA_NOVER 	-9    /* !< no version */
#define AT_ERR_FOTA_MDVER 	-10   /* !< version for module */
```
