# 概述

核间串口打印，主要功能为
1. 将在CPU1和CPU2中进行业务时需要的打印通过cpu0的串口实时显示出来；
2. 将cpu1或cpu2的异常信息显示出来。

void log_ipc_uart_register(int idx, int (*read)(const uint8_t *buf, uint32_t size),
                            int (*write)(const uint8_t *data, uint32_t size));
	CPU1和CPU2的核间串口注册函数。
	idx：核间串口idx
	int (*read)(const uint8_t *buf, uint32_t size)：核间字符读取回调函数，即调用log_ipc_rx_read
	int (*write)(const uint8_t *data, uint32_t size)：核间字符写入回调函数，即调用log_ipc_tx_write


int log_ipc_rx_read(const uint8_t * buffer, uint32_t size);
    核间字符读取。
	buffer：待读取的字符buffer
	size：待读取buffer的大小
	
int log_ipc_tx_write(const uint8_t * buffer, uint32_t size);
    核间字符写入。
	buffer：待写入的字符buffer
	size：待写入buffer的大小

int log_ipc_cp_init(void);
    CPU1和CPU2核间串口初始化函数

int log_ipc_ap_init(void);
	CPU0核间串口初始化函数

# 示例

```
本示例分别以CPU1&CPU2和CPU0为例
```

cpu1 & cpu2：
```c
void board_yoc_init()
{
#if defined(CONFIG_LOG_IPC_CP) && CONFIG_LOG_IPC_CP
    log_ipc_uart_register(CONSOLE_IDX, log_ipc_rx_read, log_ipc_tx_write);

    log_ipc_cp_init();
#else

#endif

	console_init(CONSOLE_IDX, 115200, 128);

	board_cli_init();
}
```

cpu0：
```c
void board_yoc_init()
{
int main()
{
	...

    ulog_init();

    log_ipc_ap_init();

    /* 日志等级配置 */
#if !defined(CONFIG_DEBUG) || (CONFIG_DEBUG == 0)
    aos_set_log_level(AOS_LL_WARN);
#else
    aos_set_log_level(AOS_LL_DEBUG);
#endif

    board_base_init();

	...
}
```