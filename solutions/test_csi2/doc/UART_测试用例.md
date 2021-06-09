## UART

### 概要

该文档主要描述UART模块的CSI2.0自动化测试AT命令。 文档中所涉及的这些命令是用来测试UART模块的接口工作是否正常、数据传输是否正确、接口返回值是否符合预期。


### 测试原理

测试时，应使被测试板与另外一块具备UART功能的板子连接，并发送AT+PINMUX_CONFIG到被测试板把需要测试的引脚功能复用成UART的引脚功能后，然后发送UART测试命令进行相应的测试；测试程序会按照按照测试命令的配置进行相应的数据收发测试， 若接收方接收到的数据与预期一致，则相应的测试成功完成，否则测试失败。


### 测试流程

- 测试前应当准备好测试板（以下简称A板）、测试辅助板（以下简称B板）
- 选定需要测试的UART针脚，并把A、B两块板子的UART针脚相连(A板TX接B板RX,A板RX接B板TX)
- 发送PINMUX_CONFIG命令到A板、B板， 把以上选定的引脚复用成UART功能
- 发送UART测试命令进行测试 （测试命令功能请参照下文）


### AT命令列表

以下是UART模块对应的测试命令列表及其功能简要介绍。

| AT命令列表                 | 功能                           |
| -------------------------- | ------------------------------ |
| AT+UART_SYNC_SEND          | 测试UART模块的同步发送功能     |
| AT+UART_SYNC_RECEIVE       | 测试UART模块的同步接收功能     |
| AT+UART_ASYNC_SEND         | 测试UART模块的异步发送功能     |
| AT+UART_ASYNC_RECEIVE      | 测试UART模块的异步接收功能     |
| AT+UART_DMA_SEND           | 测试UART模块的DMA发送功能      |
| AT+UART_DMA_RECEIVE        | 测试UART模块的DMA接收功能      |
| AT+UART_PUT_CHAR           | 测试UART模块的字符发送功能     |
| AT+UART_GET_CHAR           | 测试UART模块的字符接收功能     |
| AT+UART_ASYNC_SEND_CHAR    | 测试UART模块的字符异步发送功能 |
| AT+UART_ASYNC_RECEIVE_CHAR | 测试UART模块的字符异步接收功能 |
| AT+UART_SYNC_SEND_CHAR     | 测试UART模块的字符同步发送功能 |
| AT+UART_SYNC_RECEIVE_CHAR  | 测试UART模块的字符同步接收功能 |


### AT+UART_SYNC_SEND

| 命令格式             | AT+UART_SYNC_SEND=baudrate,data_bits,parity,stop_bits,flowctrl |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试UART模块的同步发送功能                                   |
| 命令参数             | baudrate:配置的波特率。如9600波特率值为9600；115200波特率值为115200。数据类型为32位数据<br>data_bits:配置的数据位。如8个数据位值为3；5个数据位值为0。具体见uart.h 中的csi_uart_data_bits_t定义<br>parity:配置的奇偶校验位。如无校验位值为0；奇数校验位值为2。具体见uart.h 中的csi_uart_parity_t定义<br>stop_bits:配置的停止位。如无停止位值为0；1.5个停止位值为2。具体见uart.h 中的csi_uart_stop_bits_t定义<br>flowctrl:配置的流控。如无流控为0；RTS流控为1。具体见uart.h 中的csi_uart_flowctrl_t定义 |
| 可搭配的辅助测试命令 | AT+UART_SYNC_RECEIVE<br>AT+UART_ASYNC_RECEIVE<br>AT+UART_DMA_RECEIVE |
| 示例                 | 分别发送AT+PINMUX_CONFIG=2,56,1和AT+PINMUX_CONFIG=2,57,1给A板和B板，将其引脚复用成UART功能引脚，然后发送命令AT+UART_SYNC_RECEIVE=9600,3,0,0,0到B板,  B板对应串口打印READY后，发送命令 AT+UART_SYNC_SEND=9600,3,0,0,0到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_SYNC_RECEIVE
| 命令格式             | AT+UART_SYNC_RECEIVE=baudrate,data_bits,parity,stop_bits,flowctrl |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试UART模块的同步接收功能                                   |
| 命令参数             | baudrate:配置的波特率。如9600波特率值为9600；115200波特率值为115200。数据类型为32位数据<br>data_bits:配置的数据位。如8个数据位值为3；5个数据位值为0。具体见uart.h 中的csi_uart_data_bits_t定义<br>parity:配置的奇偶校验位。如无校验位值为0；奇数校验位值为2。具体见uart.h 中的csi_uart_parity_t定义<br>stop_bits:配置的停止位。如无停止位值为0；1.5个停止位值为2。具体见uart.h 中的csi_uart_stop_bits_t定义<br>flowctrl:配置的流控。如无流控为0；RTS流控为1。具体见uart.h 中的csi_uart_flowctrl_t定义 |
| 可搭配的辅助测试命令 | AT+UART_SYNC_SEND<br>AT+UART_ASYNC_SEND<br>AT+UART_DMA_SEND  |
| 示例                 | 分别发送AT+PINMUX_CONFIG=2,56,1和AT+PINMUX_CONFIG=2,57,1给A板和B板，将其引脚复用成UART功能引脚，然后发送命令AT+UART_SYNC_RECEIVE=9600,3,0,0,0到B板,  B板对应串口打印READY后，发送命令 AT+UART_SYNC_SEND=9600,3,0,0,0到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_ASYNC_SEND
| 命令格式             | AT+UART_ASYNC_SEND=baudrate,data_bits,parity,stop_bits,flowctrl |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试UART模块的异步发送功能                                   |
| 命令参数             | baudrate:配置的波特率。如9600波特率值为9600；115200波特率值为115200。数据类型为32位数据<br>data_bits:配置的数据位。如8个数据位值为3；5个数据位值为0。具体见uart.h 中的csi_uart_data_bits_t定义<br>parity:配置的奇偶校验位。如无校验位值为0；奇数校验位值为2。具体见uart.h 中的csi_uart_parity_t定义<br>stop_bits:配置的停止位。如无停止位值为0；1.5个停止位值为2。具体见uart.h 中的csi_uart_stop_bits_t定义<br>flowctrl:配置的流控。如无流控为0；RTS流控为1。具体见uart.h 中的csi_uart_flowctrl_t定义 |
| 可搭配的辅助测试命令 | AT+UART_SYNC_RECEIVE<br>AT+UART_ASYNC_RECEIVE<br>AT+UART_DMA_RECEIVE |
| 示例                 | 分别发送AT+PINMUX_CONFIG=2,56,1和AT+PINMUX_CONFIG=2,57,1给A板和B板，将其引脚复用成UART功能引脚，然后发送命令AT+UART_ASYNC_RECEIVE=9600,3,0,0,0到B板,  B板对应串口打印READY后，发送命令 AT+UART_ASYNC_SEND=9600,3,0,0,0到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_ASYNC_RECEIVE
| 命令格式             | AT+UART_ASYNC_RECEIVE=baudrate,data_bits,parity,stop_bits,flowctrl |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试UART模块的异步接收功能                                   |
| 命令参数             | baudrate:配置的波特率。如9600波特率值为9600；115200波特率值为115200。数据类型为32位数据<br/>data_bits:配置的数据位。如8个数据位值为3；5个数据位值为0。具体见uart.h 中的csi_uart_data_bits_t定义<br/>parity:配置的奇偶校验位。如无校验位值为0；奇数校验位值为2。具体见uart.h 中的csi_uart_parity_t定义<br/>stop_bits:配置的停止位。如无停止位值为0；1.5个停止位值为2。具体见uart.h 中的csi_uart_stop_bits_t定义<br/>flowctrl:配置的流控。如无流控为0；RTS流控为1。具体见uart.h 中的csi_uart_flowctrl_t定义 |
| 可搭配的辅助测试命令 | AT+UART_SYNC_SEND<br>AT+UART_ASYNC_SEND<br>AT+UART_DMA_SEND  |
| 示例                 | 分别发送AT+PINMUX_CONFIG=2,56,1和AT+PINMUX_CONFIG=2,57,1给A板和B板，将其引脚复用成UART功能引脚，然后发送命令AT+UART_ASYNC_RECEIVE=9600,3,0,0,0到B板,  B板对应串口打印READY后，发送命令 AT+UART_ASYNC_SEND=9600,3,0,0,0到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_DMA_SEND
| 命令格式             | AT+UART_DMA_SEND=baudrate,data_bits,parity,stop_bits,flowctrl |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试UART模块的DMA发送功能                                    |
| 命令参数             | baudrate:配置的波特率。如9600波特率值为9600；115200波特率值为115200。数据类型为32位数据<br/>data_bits:配置的数据位。如8个数据位值为3；5个数据位值为0。具体见uart.h 中的csi_uart_data_bits_t定义<br/>parity:配置的奇偶校验位。如无校验位值为0；奇数校验位值为2。具体见uart.h 中的csi_uart_parity_t定义<br/>stop_bits:配置的停止位。如无停止位值为0；1.5个停止位值为2。具体见uart.h 中的csi_uart_stop_bits_t定义<br/>flowctrl:配置的流控。如无流控为0；RTS流控为1。具体见uart.h 中的csi_uart_flowctrl_t定义 |
| 可搭配的辅助测试命令 | AT+UART_SYNC_RECEIVE<br>AT+UART_ASYNC_RECEIVE<br>AT+UART_DMA_RECEIVE |
| 示例                 | 分别发送AT+PINMUX_CONFIG=2,56,1和AT+PINMUX_CONFIG=2,57,1给A板和B板，将其引脚复用成UART功能引脚，然后发送命令AT+UART_DMA_RECEIVE=9600,3,0,0,0到B板,  B板对应串口打印READY后，发送命令 AT+UART_DMA_SEND=9600,3,0,0,0到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_DMA_RECEIVE
| 命令格式             | AT+UART_DMA_RECEIVE=baudrate,data_bits,parity,stop_bits,flowctrl |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试UART模块的DMA接收功能                                    |
| 命令参数             | baudrate:配置的波特率。如9600波特率值为9600；115200波特率值为115200。数据类型为32位数据<br/>data_bits:配置的数据位。如8个数据位值为3；5个数据位值为0。具体见uart.h 中的csi_uart_data_bits_t定义<br/>parity:配置的奇偶校验位。如无校验位值为0；奇数校验位值为2。具体见uart.h 中的csi_uart_parity_t定义<br/>stop_bits:配置的停止位。如无停止位值为0；1.5个停止位值为2。具体见uart.h 中的csi_uart_stop_bits_t定义<br/>flowctrl:配置的流控。如无流控为0；RTS流控为1。具体见uart.h 中的csi_uart_flowctrl_t定义 |
| 可搭配的辅助测试命令 | AT+UART_SYNC_SEND<br>AT+UART_ASYNC_SEND<br>AT+UART_DMA_SEND  |
| 示例                 | 分别发送AT+PINMUX_CONFIG=2,56,1和AT+PINMUX_CONFIG=2,57,1给A板和B板，将其引脚复用成UART功能引脚，然后发送命令AT+UART_DMA_RECEIVE=9600,3,0,0,0到A板,  A板对应串口打印READY后，发送命令 AT+UART_DMA_SEND=9600,3,0,0,0到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |

### AT+UART_PUT_CHAR
| 命令格式             | AT+UART_PUT_CHAR=baudrate,data_bits,parity,stop_bits,flowctrl |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试UART模块的字符发送功能                                   |
| 命令参数             | baudrate:配置的波特率。如9600波特率值为9600；115200波特率值为115200。数据类型为32位数据<br/>data_bits:配置的数据位。如8个数据位值为3；5个数据位值为0。具体见uart.h 中的csi_uart_data_bits_t定义<br/>parity:配置的奇偶校验位。如无校验位值为0；奇数校验位值为2。具体见uart.h 中的csi_uart_parity_t定义<br/>stop_bits:配置的停止位。如无停止位值为0；1.5个停止位值为2。具体见uart.h 中的csi_uart_stop_bits_t定义<br/>flowctrl:配置的流控。如无流控为0；RTS流控为1。具体见uart.h 中的csi_uart_flowctrl_t定义 |
| 可搭配的辅助测试命令 | AT+UART_GET_CHAR<br>AT+UART_ASYNC_RECEIVE_CHAR<br>AT+UART_SYNC_RECEIVE_CHAR |
| 示例                 | 分别发送AT+PINMUX_CONFIG=2,56,1和AT+PINMUX_CONFIG=2,57,1给A板和B板，将其引脚复用成UART功能引脚，然后发送命令AT+UART_GET_CHAR=9600,3,0,0,0到B板,  B板对应串口打印READY后，发送命令 AT+UART_PUT_CHAR=9600,3,0,0,0到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_GET_CHAR
| 命令格式             | AT+UART_GET_CHAR=baudrate,data_bits,parity,stop_bits,flowctrl |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试UART模块的字符接收功能                                   |
| 命令参数             | baudrate:配置的波特率。如9600波特率值为9600；115200波特率值为115200。数据类型为32位数据<br/>data_bits:配置的数据位。如8个数据位值为3；5个数据位值为0。具体见uart.h 中的csi_uart_data_bits_t定义<br/>parity:配置的奇偶校验位。如无校验位值为0；奇数校验位值为2。具体见uart.h 中的csi_uart_parity_t定义<br/>stop_bits:配置的停止位。如无停止位值为0；1.5个停止位值为2。具体见uart.h 中的csi_uart_stop_bits_t定义<br/>flowctrl:配置的流控。如无流控为0；RTS流控为1。具体见uart.h 中的csi_uart_flowctrl_t定义 |
| 可搭配的辅助测试命令 | AT+UART_PUT_CHAR<br>AT+UART_ASYNC_SEND_CHAR<br>AT+UART_SYNC_SEND_CHAR |
| 示例                 | 分别发送AT+PINMUX_CONFIG=2,56,1和AT+PINMUX_CONFIG=2,57,1给A板和B板，将其引脚复用成UART功能引脚，然后发送命令AT+UART_GET_CHAR=9600,3,0,0,0到A板,  A板对应串口打印READY后，发送命令 AT+UART_PUT_CHAR=9600,3,0,0,0到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_ASYNC_SEND_CHAR
| 命令格式             | AT+UART_ASYNC_SEND_CHAR=baudrate,data_bits,parity,stop_bits,flowctrl |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试UART模块的字符异步发送功能                               |
| 命令参数             | baudrate:配置的波特率。如9600波特率值为9600；115200波特率值为115200。数据类型为32位数据<br/>data_bits:配置的数据位。如8个数据位值为3；5个数据位值为0。具体见uart.h 中的csi_uart_data_bits_t定义<br/>parity:配置的奇偶校验位。如无校验位值为0；奇数校验位值为2。具体见uart.h 中的csi_uart_parity_t定义<br/>stop_bits:配置的停止位。如无停止位值为0；1.5个停止位值为2。具体见uart.h 中的csi_uart_stop_bits_t定义<br/>flowctrl:配置的流控。如无流控为0；RTS流控为1。具体见uart.h 中的csi_uart_flowctrl_t定义 |
| 可搭配的辅助测试命令 | AT+UART_GET_CHAR<br>AT+UART_ASYNC_RECEIVE_CHAR<br>AT+UART_SYNC_RECEIVE_CHAR |
| 示例                 | 分别发送AT+PINMUX_CONFIG=2,56,1和AT+PINMUX_CONFIG=2,57,1给A板和B板，将其引脚复用成UART功能引脚，然后发送命令AT+UART_ASYNC_RECEIVE_CHAR=9600,3,0,0,0到B板,  B板对应串口打印READY后，发送命令 AT+UART_ASYNC_SEND_CHAR=9600,3,0,0,0到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_SYNC_SEND_CHAR
| 命令格式             | AT+UART_SYNC_SEND_CHAR=baudrate,data_bits,parity,stop_bits,flowctrl |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试UART模块的字符同步发送功能                               |
| 命令参数             | baudrate:配置的波特率。如9600波特率值为9600；115200波特率值为115200。数据类型为32位数据<br/>data_bits:配置的数据位。如8个数据位值为3；5个数据位值为0。具体见uart.h 中的csi_uart_data_bits_t定义<br/>parity:配置的奇偶校验位。如无校验位值为0；奇数校验位值为2。具体见uart.h 中的csi_uart_parity_t定义<br/>stop_bits:配置的停止位。如无停止位值为0；1.5个停止位值为2。具体见uart.h 中的csi_uart_stop_bits_t定义<br/>flowctrl:配置的流控。如无流控为0；RTS流控为1。具体见uart.h 中的csi_uart_flowctrl_t定义 |
| 可搭配的辅助测试命令 | AT+UART_GET_CHAR<br>AT+UART_ASYNC_RECEIVE_CHAR<br>AT+UART_SYNC_RECEIVE_CHAR |
| 示例                 | 分别发送AT+PINMUX_CONFIG=2,56,1和AT+PINMUX_CONFIG=2,57,1给A板和B板，将其引脚复用成UART功能引脚，然后发送命令AT+UART_SYNC_RECEIVE_CHAR=9600,3,0,0,0到B板,  B板对应串口打印READY后，发送命令 AT+UART_SYNC_SEND_CHAR=9600,3,0,0,0到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_SYNC_RECEIVE_CHAR
| 命令格式 | AT+UART_SYNC_RECEIVE_CHAR=idx,baudrate,data_bits,parity,stop_bits,flowctrl |
|----------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 命令功能 | 测试UART模块的字符同步接收功能 |
| 命令参数 | baudrate:配置的波特率。如9600波特率值为9600；115200波特率值为115200。数据类型为32位数据<br/>data_bits:配置的数据位。如8个数据位值为3；5个数据位值为0。具体见uart.h 中的csi_uart_data_bits_t定义<br/>parity:配置的奇偶校验位。如无校验位值为0；奇数校验位值为2。具体见uart.h 中的csi_uart_parity_t定义<br/>stop_bits:配置的停止位。如无停止位值为0；1.5个停止位值为2。具体见uart.h 中的csi_uart_stop_bits_t定义<br/>flowctrl:配置的流控。如无流控为0；RTS流控为1。具体见uart.h 中的csi_uart_flowctrl_t定义 |
| 可搭配的辅助测试命令 | AT+UART_PUT_CHAR<br>AT+UART_ASYNC_SEND_CHAR<br>AT+UART_SYNC_SEND_CHAR |
| 示例 | 分别发送AT+PINMUX_CONFIG=2,56,1和AT+PINMUX_CONFIG=2,57,1给A板和B板，将其引脚复用成UART功能引脚，然后发送命令AT+UART_SYNC_RECEIVE_CHAR=9600,3,0,0,0到A板,  A板对应串口打印READY后，发送命令 AT+UART_SYNC_SEND_CHAR=9600,3,0,0,0到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_ASYNC_RECEIVE_CHAR
| 命令格式 | AT+UART_ASYNC_RECEIVE_CHAR=idx,baudrate,data_bits,parity,stop_bits,flowctrl |
|----------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 命令功能 | 测试UART模块的字符异步接收功能 |
| 命令参数 | baudrate:配置的波特率。如9600波特率值为9600；115200波特率值为115200。数据类型为32位数据<br/>data_bits:配置的数据位。如8个数据位值为3；5个数据位值为0。具体见uart.h 中的csi_uart_data_bits_t定义<br/>parity:配置的奇偶校验位。如无校验位值为0；奇数校验位值为2。具体见uart.h 中的csi_uart_parity_t定义<br/>stop_bits:配置的停止位。如无停止位值为0；1.5个停止位值为2。具体见uart.h 中的csi_uart_stop_bits_t定义<br/>flowctrl:配置的流控。如无流控为0；RTS流控为1。具体见uart.h 中的csi_uart_flowctrl_t定义 |
| 可搭配的辅助测试命令 | AT+UART_PUT_CHAR<br>AT+UART_ASYNC_SEND_CHAR<br>AT+UART_SYNC_SEND_CHAR |
| 示例 | 分别发送AT+PINMUX_CONFIG=2,56,1和AT+PINMUX_CONFIG=2,57,1给A板和B板，将其引脚复用成UART功能引脚，然后发送命令AT+UART_SYNC_RECEIVE_CHAR=9600,3,0,0,0到A板,  A板对应串口打印READY后，发送命令 AT+UART_SYNC_SEND_CHAR=9600,3,0,0,0到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


