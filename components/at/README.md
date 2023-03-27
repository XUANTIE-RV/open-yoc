## 概述

### AT 命令

AT 命令（AT Commands）最早是由发明拨号调制解调器（MODEM）的贺氏公司（Hayes）为了控制 MODEM 而发明的控制协议。后来随着网络带宽的升级，速度很低的拨号 MODEM 基本退出一般使用市场，但是 AT 命令保留下来。当时主要的移动电话生产厂家共同为 GSM 研制了一整套 AT 命令，用于控制手机的 GSM 模块。AT 命令在此基础上演化并加入 GSM 07.05 标准以及后来的 GSM 07.07 标准，实现比较健全的标准化。

在随后的 GPRS 控制、3G 模块等方面，均采用的 AT 命令来控制，AT 命令逐渐在产品开发中成为实际的标准。如今，AT 命令也广泛的应用于嵌入式开发领域，AT 命令作为主芯片和通讯模块的协议接口，硬件接口一般为串口，这样主控设备可以通过简单的命令和硬件设计完成多种操作。

### AT 组件简介

AT 组件是由 `AT Server` 和 `AT Parser` 的组成，组件完成 AT 命令的发送、命令格式及参数判断、命令的响应、响应数据的接收、响应数据的解析、OOB 数据处理等整个 AT 命令数据交互流程。

通过 AT 组件，设备可以作为 `AT Parser` 使用串口连接其他设备发送并接收解析数据，可以作为` AT Server` 让其他设备甚至电脑端连接完成发送数据的响应，也可以在本地 shell 启动 CLI 模式使设备同时支持 AT Server 和 AT Client 功能，该模式多用于设备开发调试。

### AT指令解析器

主要用于发送命令、等待响应，用户可通过atparser_response_recv()获取并解析该应答数据，对于对端主动发送的数据（数据即为带外数据），则传入对应的带外处理函数中

### AT Server

提供AT指令的注册,将解析完成后数据提供至用户注册的命令回掉函数中,由用户做进一步处理。

## 配置

无

## 接口列表

### AT 指令解析器

| 函数                                    | 说明                               |
| :-------------------------------------- | :--------------------------------- |
| atparser_init                           | 初始化一个AT指令解析器             |
| atparser_channel_init                   | 初始化一个用户自定义的AT指令解析器 |
| atparser_oob_create/atparser_oob_delete | 创建（删除）一个带外数据服务       |
| atparser_set_terminator                 | 设置结束字符串                     |
| atparser_set_timeout                    | 设置超时时间                       |
| atparser_send                           | 格式化输出字符串                   |
| atparser_response_recv                  | 接收并解析一行数据                 |
| atparser_clr_buf                        | 清除缓存行数据                     |
| atparser_cmd_exit                       | 发送退出                           |
| atparser_write                          | 发送数据                           |
| atparser_debug_control                  | log控制开关                        |
| atparser_recv_str                       | 接收一行数据                       |

### AT Server 服务

| 函数        | 说明     |
| :---------- | :------- |
| aos_kv_init | KV初始化 |



## 接口详细说明

### AT 指令解析器

#### 初始化一个AT指令解析器

```c
atparser_uservice_t *atparser_init(utask_t *task, const char *name, void *config)
```

根据指定的微服务任务,串口名以及串口配置信息创建一个AT 指令解析器，成功则返回atparser_uservice_t指针，失败返回NULL

- 参数:
  - task：微服务任务
  - name: 串口名
  - config: 串口配置信息
- 返回值:
  - 成功则返回atparser_uservice_t指针，失败返回NULL



#### 初始化一个用户自定义的AT指令解析器

```c
atparser_uservice_t *atparser_channel_init(utask_t *task, const char *name, void *config, at_channel_t *channel);
```

根据指定的微服务任务,串口名以及串口配置信息创建一个AT 指令解析器，成功则返回atparser_uservice_t指针，失败返回NULL

- 参数:
  - task：微服务任务
  - name: 设备名
  - config: 设备配置信息
  - channel：at指令解析器的channel
- 返回值:
  - 成功则返回atparser_uservice_t指针，失败返回NULL



#### 带外数据处理函数的格式

```c
typedef int (*oob_callback)(atparser_uservice_t *at, void *pive, oob_data_t *oob_data);
```

 用户根据传入的数据，做相关的处理，若处理完成则返回0；若处理未完成则返回-1

- 参数:
  - at: AT 指令解析器
  - pive：用户自定义数据
  - oob_data: 带外数据结构体
- 返回值:
  - 0表示处理完成并退出本次OOB,-1表示处理未完成,需要继续执行OOB

#### 创建一个带外数据服务

```c
int atparser_oob_create(atparser_uservice_t *at, const char *prefix, oob_callback cb, void *context);
```

根据指定的AT 指令解析器，带外数据匹配字符串，带外数据处理函数，以及用户自定义的上下文创建一个带外数据处理服务，成功则返回0，失败则返回负值

- 参数:
  - at: AT 指令解析器
  - prefix：带外数据匹配字符串
  - cb: 带外数据处理函数
  - context: 服务自定义上下文，用户在带外数据处理函数时，可使用该上下文
- 返回值:
  - 成功则返回0，失败返回负值,错误号如下：
    - -ENOMEM  内存不足
    - -EINVAL 无效的参数

#### 删除一个带外数据服务

```c
int atparser_oob_delete(atparser_uservice_t *at, const char *prefix);
```

根据指定的AT 指令解析器，带外数据匹配字符串，删除对应带外数据处理函数并释放相关资源，成功则返回0，失败负值

- 参数:
  - at: AT 指令解析器
  - prefix：带外数据匹配字符串
- 返回值:
  - 成功则返回0，失败返回负值,错误号如下：
    - -EINVAL 无效的参数

#### 设置结束字符串

```c
void atparser_set_terminator(atparser_uservice_t *at, const char *terminator);
```

根据指定的AT 指令解析器，结束字符串，设置对应AT解析器的结束字符串

- 参数:
  - at: AT 指令解析器
  - terminator：待设置的结束字符串
- 返回值:
  - 无返回值

#### 设置超时时间

```c
void atparser_set_timeout(atparser_uservice_t *at, int timeout);
```

根据指定的AT 指令解析器，超时时间（单位：ms），设置对应AT解析器的超时时间

- 参数:
  - at: AT 指令解析器
  - timeout：待设置的超时时间
- 返回值:
  - 无返回值

#### 格式化输出字符串

```c
int atparser_send(atparser_uservice_t *at, const char *format, ...);
```

根据指定的AT 指令解析器，格式化字符串以及输出列表，发送对应的字符串，成功则返回0，失败则返回负值，用法参照printf()；

- 参数:
  - at: AT 指令解析器
  - format：格式化字符串
  - ...：输出列表
- 返回值:
  - 成功则返回0，失败返回负值，错误码如下：
    - -EINVAL 无效的参数

#### 接收并解析一行数据

```c
int atparser_response_recv(atparser_uservice_t *at, const char *response, ...);
```

根据指定的AT 指令解析器，格式化的字符串以及输出列表，接收并解析一行数据，成功则返回0，失败负值,用法参照sscanf();

- 参数:
  - at: AT 指令解析器
  - response：格式化字符串
  - ...：输出列表
- 返回值:
  - 成功则返回0，失败返回负值，错误码如下：
    - -EINVAL 无效的参数

#### 清除缓存行数据

```c
void atparser_clr_buf(atparser_uservice_t *at);
```

根据指定的AT 指令解析器，清除已接收的行数据;

- 参数:
  - at: AT 指令解析器
- 返回值:
  - 无返回值

#### 发送退出

```c
void atparser_cmd_exit(atparser_uservice_t *at);
```

根据指定的AT 指令解析器，在发送格式化字符串后退出，与atparser_send（）配合使用;

- 参数:
  - at: AT 指令解析器
- 返回值:
  - 无返回值

#### 发送数据

```c
int atparser_write(atparser_uservice_t *at, const void *data, int size);
```

根据指定的AT 指令解析器，数据长度，发送对应的数据;

- 参数:
  - at: AT 指令解析器
  - data：数据头指针
  - size：数据大小
- 返回值:
  - 成功则返回0，失败返回-1

#### log控制开关

```c
void atparser_debug_control(atparser_uservice_t *at,int flag);
```

控制指定的AT 指令解析器log开关;

- 参数:
  - at: AT 指令解析器
  - flag：0：关闭log；1：打开log
- 返回值:
  - 无返回值



#### 接收一行数据

```c
int atparser_recv_str(atparser_uservice_t *at, const char *str);
```

控制指定的AT 指令解析器log开关;

- 参数:
  - at: AT 指令解析器
  - str：一行数据
- 返回值:
  - 成功则返回0，失败返回-1

### AT Server 服务

#### 初始化AT Server 服务

```c
int atserver_init(utask_t *task, const char*name, void *config);
```

根据传入的微服务任务，串口名以及串口配置参数，初始化AT server 服务，成功返回0，失败返回负值

- 参数:
  - task：微服务任务
  - name：串口名
  - config：串口配置参数
- 返回值：
  - 成功则返回0，失败返回负值,错误号如下：
    - -ENOMEM  内存不足
    - -EIO     错误的硬件操作

#### 格式化输出字符串

```c
int atserver_send(const char *format, ...);
```

根据格式化字符串以及输出列表，发送对应的字符串，成功则返回0，失败返回负值

- 参数:
  - format：格式化字符串
  - ...：输出列表
- 返回值：
  - 成功则返回0，失败返回负值,错误号如下：  
    - -EINVAL 无效的参数
    - -EIO    错误的硬件操作

#### 使用参数列表格式化输出

```c
int atserver_sendv(const char *command, va_list args);
```

使用参数列表，发送对应的字符串，成功则返回0，失败返回负值

- 参数:
  - command：格式化字符串
  - args：可变参数列表的对象
- 返回值：
  - 成功则返回0，失败返回负值,错误号如下:
    - -EINVAL 无效的参数
    - -EIO    错误的硬件操作

#### 发送数据

```c
int atserver_write(const void *data, int size);
```

根据数据长度，发送对应的数据;

- 参数:
  - data：用户数据
  - size：数据长度
- 返回值：
  - 成功则返回0，失败返回负值,错误号如下:
    - -EIO    错误的硬件操作

#### 命令回调函数的格式

```c
void (*cmd_callback)(char *cmd, int type, char *data);
```

- type :

```c
typedef enum {
    TEST_CMD,
    READ_CMD,
    WRITE_CMD,
    EXECUTE_CMD,

    INVAILD_CMD
} AT_CMD_TYPE;
```

- 参数:
  - cmd:AT指令
  - type:AT指令类型
  - data:如果是设置命令,则将待设置的数据传入,否则为NULL
- 返回值：
  - 无返回值

#### 链接 AT Server 服务的命令

```c
int atserver_cmd_link(atserver_cmd_t *at_cmd);
```

将新命令加入AT Server 服务的命令链表中，成功则返回0，失败则返回负值，

- 参数:
  - at_cmd：待链接的命令
- 返回值：
  - 成功则返回0，失败返回负值,错误号如下：
    - -EINVAL 无效的参数
    - -ENOMEM  内存不足

#### 链接 AT Server 服务的命令表

```c
int atserver_add_command(const atserver_cmd_t at_cmd[]);
```

将新命令表（表格最后以NULL结尾）加入AT Server 服务的命令链表中，成功则返回0，失败则返回负值，

- 参数:
  - at_cmd：待链接的命令表
- 返回值：
  - 成功则返回0，失败返回负值,错误号如下：
    - -EINVAL 无效的参数
    - -ENOMEM  内存不足

#### 设置结束字符串

```c
void atserver_set_output_terminator(const char *terminator)
```

根据指定的结束字符串，设置对应AT Server的结束字符串

- 参数:
  - terminator：结束字符串
- 返回值：
  *无返回值

#### 设置结束参数分隔符

```c
void atserver_set_para_delimiter(char delimiter);
```

根据指定的参数分隔符，设置对应AT Server的参数分隔符

- 参数:
  - delimiter：参数分隔符
- 返回值：
  *无返回值

#### 设置超时时间

```c
void atserver_set_timeout(int timeout);
```

根据指定超时时间（单位：ms），设置对应AT Server的超时时间

- 参数:
  - timeout：待设置的超时时间
- 返回值:
  - 无返回值

#### 读取格式化的字符串中的数据

```c
int atserver_scanf(const char *format, ...);
```

读取格式化的字符串中的数据，用法参照sscanf()，成功则返回0，失败返回负值

- 参数:
  - format：格式化字符串
  - ...：输出列表
- 返回值:
  - 成功则返回0，失败返回负值,错误号如下：
    - -EBADFD 错误的设备指针  
    - -EINVAL 无效的参数

```c
void at_net_event_sub_init(void);
void at_net_event_unsub_init(void);
```

网络事件的订阅以及取消订阅

- 参数:
  - 无
- 返回值:
  - 无



## 示例代码

### AT 指令解析器

``` C
static atparser_uservice_t *g_atparser_uservice_t;

int esp8266_close(int id)
{
    /*
		AT协议为 --->"AT+CIPCLOSE=0\r\n"
		        <---"OK\r\n"
    */
    int ret = -1;

    atparser_clr_buf(g_atparser_uservice_t);  //清除之前的缓存数据

    if (atparser_send(g_atparser_uservice_t, "AT+CIPCLOSE=%d", id) == 0) { //发送数据“AT+CIPCLOSE=0”
        if (atparser_recv(g_atparser_uservice_t, "OK") == 0) { // 接收数据“OK”
            ret = 0;
        }
    }

    atparser_cmd_exit(g_atparser_uservice_t); // 退出本次接收

    return ret;
}

static int _gotip_handler(atparser_uservice_t *at, void *priv, oob_data_t *oob_data)
{
    //接收到“WIFI GOT IP” oob的处理
    LOGD(TAG, "got ip\n");

		......
      
    return 0;
}

int esp8266_module_init(void)
{
    rvm_hal_uart_config_t config;
    utask_t *task = utask_new("esp8266", 1 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI + 4);  //创建atparser 所需的utask

    if (task == NULL) {
        return -1;
    }

    rvm_hal_uart_config_default(&config);  // 初始化uart参数
    config.baud_rate = 115200;     // uart baud为115200

    g_atparser_uservice_t = atparser_init(task, “uart0”, &config); //初始化atparser 微服务

    atparser_oob_create(g_atparser_uservice_t, "WIFI GOT IP", _gotip_handler, NULL);  //注册“WIFI GOT IP”的oob，回掉函数为“_gotip_handler”

    return 0;
}
```

### AT Server 服务

```
void at_cmd_at(char *cmd, int type, char *data)
{
    if (type == EXECUTE_CMD) {
        atserver_send("\r\nOK\r\n");
    }
}

#define AT              {"AT", at_cmd_at}
#define AT_NULL         {NULL, NULL}
const atserver_cmd_t at_cmd[] = {
    AT,
    AT_NULL,
};

void at_server_init(utask_t *task)
{
    rvm_hal_uart_config_t config;
    rvm_hal_uart_config_default(&config);

    utask_t *task = utask_new("at_srv", 2 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI);

    if (task) {
        atserver_init(task, "uart1", &config);
        atserver_set_output_terminator("");
        atserver_add_command(at_cmd);
    }
}
```

## 诊断错误码

无。

## 运行资源

无。

## 依赖资源

minilibc: v7.2.0及以上。
aos: v7.2.0及以上。

## 组件参考

无。