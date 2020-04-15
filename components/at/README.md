# 概述

## AT 命令

AT 命令（AT Commands）最早是由发明拨号调制解调器（MODEM）的贺氏公司（Hayes）为了控制 MODEM 而发明的控制协议。后来随着网络带宽的升级，速度很低的拨号 MODEM 基本退出一般使用市场，但是 AT 命令保留下来。当时主要的移动电话生产厂家共同为 GSM 研制了一整套 AT 命令，用于控制手机的 GSM 模块。AT 命令在此基础上演化并加入 GSM 07.05 标准以及后来的 GSM 07.07 标准，实现比较健全的标准化。

在随后的 GPRS 控制、3G 模块等方面，均采用的 AT 命令来控制，AT 命令逐渐在产品开发中成为实际的标准。如今，AT 命令也广泛的应用于嵌入式开发领域，AT 命令作为主芯片和通讯模块的协议接口，硬件接口一般为串口，这样主控设备可以通过简单的命令和硬件设计完成多种操作。

## AT 组件简介

AT 组件是由 `AT Server` 和 `AT Parser` 的组成，组件完成 AT 命令的发送、命令格式及参数判断、命令的响应、响应数据的接收、响应数据的解析、OOB 数据处理等整个 AT 命令数据交互流程。

通过 AT 组件，设备可以作为 `AT Parser` 使用串口连接其他设备发送并接收解析数据，可以作为` AT Server` 让其他设备甚至电脑端连接完成发送数据的响应，也可以在本地 shell 启动 CLI 模式使设备同时支持 AT Server 和 AT Client 功能，该模式多用于设备开发调试。

## AT指令解析器

主要用于发送命令、等待响应，用户可通过atparser_response_recv()获取并解析该应答数据，对于对端主动发送的数据（数据即为带外数据），则传入对应的带外处理函数中

## AT Server

提供AT指令的注册,将解析完成后数据提供至用户注册的命令回掉函数中,由用户做进一步处理。


# 示例代码

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
    uart_config_t config;
    utask_t *task = utask_new("esp8266", 1 * 1024, QUEUE_MSG_COUNT, AOS_DEFAULT_APP_PRI + 4);  //创建atparser 所需的utask

    if (task == NULL) {
        return -1;
    }

    uart_config_default(&config);  // 初始化uart参数
    config.baud_rate = 115200;     // uart baud为115200

    g_atparser_uservice_t = atparser_init(task, “uart0”, &config); //初始化atparser 微服务

    atparser_oob_create(g_atparser_uservice_t, "WIFI GOT IP", _gotip_handler, NULL);  //注册“WIFI GOT IP”的oob，回掉函数为“_gotip_handler”

    return 0;
}
```
