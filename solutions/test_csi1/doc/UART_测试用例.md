## UART

### 概要

该文档主要描述UART模块的CSI1.0自动化测试AT命令。 文档中所涉及的这些命令是用来测试：（1）UART模块的接口工作是否正常以及UART实例支持的能力的正确性（2）csi_usart_send在不同的模式、不同的波特率、不同的数据位和停止位以及不同的校验模式下传输不同长度的数据的正确性（3）csi_usart_receive在不同的模式、不同的波特率、不同的数据位和停止位个数以及不同的校验模式下接收不同长度的数据的正确性（4）csi_usart_putchar在不同的模式、不同的波特率、不同的数据位和停止位个数以及不同的校验模式下发送数据的正确性（5）csi_usart_getchar在不同的模式、不同的波特率、不同的数据位和停止位个数以及不同的校验模式下接收数据的正确性（6）测试csi_usart_get_tx_count获取设备已发送的数据个数的正确性（7）获取csi_usart_get_rx_count获取设备已接收的数据个数的正确性(7)测试csi_usart_flush清除usart 数据缓存的正确性


### 测试原理

测试时，应使被测试板A开发板与另外一块拥有UART功能的B开发板连接，并分别发送AT+PINMUX_CONFIG命令到A和B两块测试板把需要测试的引脚功能复用成UART的引脚功能后，然后发送UART测试命令进行相应的测试；测试程序会按照按照测试命令的配置进行相应的数据收发测试， 若接收方接收到的数据与预期一致，则相应的测试成功完成，否则测试失败。



### 测试流程

- 测试前应当准备好测试板（以下简称A板）、测试辅助板（以下简称B板）
- 选定需要测试的UART针脚，并把A、B两块板子的UART针脚相连(A板TX接B板RX,A板RX接B板TX,A板GND接B板GND)
- 发送PINMUX_CONFIG命令到A板、B板， 把以上选定的引脚复用成UART功能
- 发送UART测试命令进行测试 （测试命令功能请参照下文）



### AT命令列表

以下是UART模块对应的测试命令列表及其功能简要介绍。

| AT命令列表                   | 功能                           |
| --------------------------  | ------------------------------ |
| AT+UART_ASYNC_SEND          | 测试UART模块的send函数多字节发送功能        |
| AT+UART_ASYNC_RECEIVE       | 测试UART模块的receive函数多字节接收功能     |
| AT+UART_ASYNC_SENDCHAR      | 测试UART模块的send函数单字节发送功能        |
| AT+UART_ASYNC_RECVCHAR      | 测试UART模块的receive函数单字节接收功能     |
| AT+UART_GETCHAR             | 测试UART模块的getchar函数接收功能          |
| AT+UART_PUTCHAR             | 测试UART模块的putchar函数发送功能          |
| AT+UART_INTERFACE           | 对UART模块进行接口测试                     |
| AT+UART_TX_COUNT            | 测试UART模块的csi_usart_get_tx_count功能   |
| AT+UART_RX_COUNT            | 测试UART模块的csi_usart_get_rx_count功能   |


### AT+UART_SYNC_SEND

| 命令格式             | AT+UART_ASYNC_SEND=idx,baudrate,data_bits,parity,stop_bits,uart_mode,uart_transfer_len|
| -------------------- | ------------------------------------------------------------   |
| 命令功能             | 测试UART模块的send函数多字节发送功能                               |
| 命令参数             | 
baudrate:配置的波特率。如9600波特率值为9600，115200波特率值为115200。<br>
data_bits:配置的数据位。如5个数据位值为0，6个数据位值为1，7个数据位值为2，8个数据位值为3，9个数据位值为4。<br>
parity:配置的奇偶校验位。如无校验位值为0；奇校验模式为1,偶校验模式为2。<br>
stop_bits:配置的停止位。如1个停止位值为0，2个停止位值为1。<br>|
uart_mode：配置的传输模式：如异步传输方式为0。<br>|
uart_transfer_len：配置的数据传输长度：如1位数据长度为1，512位数据长度为512，1024位数据长度为1024。<br>|
| 可搭配的辅助测试命令 | AT+UART_ASYNC_RECEIVE |
| 示例                 | 发送命令AT+UART_SYNC_RECEIVE=9600,3,0,0,0,0,100到B板,  B板对应串口打印READY后，发送命令 AT+UART_SYNC_SEND=9600,3,0,0,0,0,100到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_ASYNC_RECEIVE

| 命令格式             | AT+UART_ASYNC_SEND=idx,baudrate,data_bits,parity,stop_bits,uart_mode,uart_transfer_len |
| -------------------- | ------------------------------------------------------------   |
| 命令功能             | 测试UART模块的send函数多字节发送功能                               |
| 命令参数             | 
baudrate:配置的波特率。如9600波特率值为9600，115200波特率值为115200。<br>
data_bits:配置的数据位。如5个数据位值为0，6个数据位值为1，7个数据位值为2，8个数据位值为3，9个数据位值为4。<br>
parity:配置的奇偶校验位。如无校验位值为0；奇校验模式为1,偶校验模式为2。<br>
stop_bits:配置的停止位。如1个停止位值为0，2个停止位值为1。<br>|
uart_mode：配置的传输模式：如异步传输方式为0。<br>|
uart_transfer_len：配置的数据传输长度：如1位数据长度为1，512位数据长度为512，1024位数据长度为1024。<br>|
| 可搭配的辅助测试命令 | AT+UART_ASYNC_SEND |
| 示例                 | 发送命令AT+UART_SYNC_RECEIVE=9600,3,0,0,0,0,100到B板,  B板对应串口打印READY后，发送命令 AT+UART_SYNC_SEND=9600,3,0,0,0,0,100到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |

### AT+UART_ASYNC_SENDCHAR

| 命令格式             | UART_ASYNC_RECEIVE=idx,baudrate,data_bits,parity,stop_bits,uart_mode,uart_transfer_len |
| -------------------- | ------------------------------------------------------------   |
| 命令功能             | 测试UART模块的send函数多字节发送功能                               |
| 命令参数             | 
baudrate:配置的波特率。如9600波特率值为9600，115200波特率值为115200。<br>
data_bits:配置的数据位。如5个数据位值为0，6个数据位值为1，7个数据位值为2，8个数据位值为3，9个数据位值为4。<br>
parity:配置的奇偶校验位。如无校验位值为0；奇校验模式为1,偶校验模式为2。<br>
stop_bits:配置的停止位。如1个停止位值为0，2个停止位值为1。<br>|
uart_mode：配置的传输模式：如异步传输方式为0。<br>|
uart_transfer_len：配置的数据传输长度：如1位数据长度为1，512位数据长度为512，1024位数据长度为1024。<br>|
| 可搭配的辅助测试命令 | AT+UART_ASYNC_RECVCHAR |
| 示例                 | 发送命令AT+UART_SYNC_RECEIVE=9600,3,0,0,0,0,1到B板,  B板对应串口打印READY后，发送命令 AT+UART_SYNC_SEND=9600,3,0,0,0,0,1到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_ASYNC_RECVCHAR

| 命令格式             | UART_ASYNC_RECEIVE=idx,baudrate,data_bits,parity,stop_bits,uart_mode,uart_transfer_len |
| -------------------- | ------------------------------------------------------------   |
| 命令功能             | 测试UART模块的send函数多字节发送功能                               |
| 命令参数             | 
baudrate:配置的波特率。如9600波特率值为9600，115200波特率值为115200。<br>
data_bits:配置的数据位。如5个数据位值为0，6个数据位值为1，7个数据位值为2，8个数据位值为3，9个数据位值为4。<br>
parity:配置的奇偶校验位。如无校验位值为0；奇校验模式为1,偶校验模式为2。<br>
stop_bits:配置的停止位。如1个停止位值为0，2个停止位值为1。<br>|
uart_mode：配置的传输模式：如异步传输方式为0。<br>|
uart_transfer_len：配置的数据传输长度：如1位数据长度为1，512位数据长度为512，1024位数据长度为1024。<br>|
| 可搭配的辅助测试命令 | AT+UART_ASYNC_SENDCHAR |
| 示例                 | 发送命令AT+UART_ASYNC_RECVCHAR=9600,3,0,0,0,0,1到B板,  B板对应串口打印READY后，发送命令 AT+UART_ASYNC_SENDCHAR=9600,3,0,0,0,0,1到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |



### AT+UART_PUTCHAR

| 命令格式             | AT+UART_PUTCHAR=idx,baudrate,data_bits,parity,stop_bits,uart_mode |
| -------------------- | ------------------------------------------------------------   |
| 命令功能             | 测试UART模块的send函数多字节发送功能                               |
| 命令参数             | 
baudrate:配置的波特率。如9600波特率值为9600，115200波特率值为115200。<br>
data_bits:配置的数据位。如5个数据位值为0，6个数据位值为1，7个数据位值为2，8个数据位值为3，9个数据位值为4。<br>
parity:配置的奇偶校验位。如无校验位值为0；奇校验模式为1,偶校验模式为2。<br>
stop_bits:配置的停止位。如1个停止位值为0，2个停止位值为1。<br>|
uart_mode：配置的传输模式：如异步传输方式为0。<br>|
uart_transfer_len：配置的数据传输长度：如1位数据长度为1，512位数据长度为512，1024位数据长度为1024。<br>|
| 可搭配的辅助测试命令 | AT+UART_GETCHAR |
| 示例                 | 发送命令AT+UART_GETCHAR=9600,3,0,0,0,0,1到B板,  B板对应串口打印READY后，发送命令 AT+UART_PUTCHAR=9600,3,0,0,0,0,1到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_GETCHAR

| 命令格式             | AT+UART_GETCHAR=idx,baudrate,data_bits,parity,stop_bits,uart_mode |
| -------------------- | ------------------------------------------------------------   |
| 命令功能             | 测试UART模块的send函数多字节发送功能                               |
| 命令参数             | 
baudrate:配置的波特率。如9600波特率值为9600，115200波特率值为115200。<br>
data_bits:配置的数据位。如5个数据位值为0，6个数据位值为1，7个数据位值为2，8个数据位值为3，9个数据位值为4。<br>
parity:配置的奇偶校验位。如无校验位值为0；奇校验模式为1,偶校验模式为2。<br>
stop_bits:配置的停止位。如1个停止位值为0，2个停止位值为1。<br>|
uart_mode：配置的传输模式：如异步传输方式为0。<br>|
uart_transfer_len：配置的数据传输长度：如1位数据长度为1，512位数据长度为512，1024位数据长度为1024。<br>|
| 可搭配的辅助测试命令 | AT+UART_PUTCHAR |
| 示例                 | 发送命令AT+UART_GETCHAR=9600,3,0,0,0,0到B板,  B板对应串口打印READY后，发送命令 AT+UART_PUTCHAR=9600,3,0,0,0,0到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_TX_COUNT

| 命令格式             | AT+UART_TX_COUNT=idx,baudrate,data_bits,parity,stop_bits,uart_mode,uart_transfer_len |
| -------------------- | ------------------------------------------------------------   |
| 命令功能             | 测试UART模块的send函数多字节发送功能                               |
| 命令参数             | 
baudrate:配置的波特率。如9600波特率值为9600，115200波特率值为115200。<br>
data_bits:配置的数据位。如5个数据位值为0，6个数据位值为1，7个数据位值为2，8个数据位值为3，9个数据位值为4。<br>
parity:配置的奇偶校验位。如无校验位值为0；奇校验模式为1,偶校验模式为2。<br>
stop_bits:配置的停止位。如1个停止位值为0，2个停止位值为1。<br>|
uart_mode：配置的传输模式：如异步传输方式为0。<br>|
uart_transfer_len：配置的数据传输长度：如1位数据长度为1，512位数据长度为512，1024位数据长度为1024。<br>|
| 可搭配的辅助测试命令 | AT+UART_PUTCHAR |
| 示例                 | 发送命令AT+UART_RX_COUNT=9600,3,0,0,0,0,10到B板,  B板对应串口打印READY后，发送命令 AT+UART_TX_COUNT=9600,3,0,0,0,0,10到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+UART_RX_COUNT

| 命令格式             | AT+UART_RX_COUNT=idx,baudrate,data_bits,parity,stop_bits,uart_mode,uart_transfer_len |
| -------------------- | ------------------------------------------------------------   |
| 命令功能             | 测试UART模块的send函数多字节发送功能                               |
| 命令参数             | 
baudrate:配置的波特率。如9600波特率值为9600，115200波特率值为115200。<br>
data_bits:配置的数据位。如5个数据位值为0，6个数据位值为1，7个数据位值为2，8个数据位值为3，9个数据位值为4。<br>
parity:配置的奇偶校验位。如无校验位值为0；奇校验模式为1,偶校验模式为2。<br>
stop_bits:配置的停止位。如1个停止位值为0，2个停止位值为1。<br>|
uart_mode：配置的传输模式：如异步传输方式为0。<br>|
uart_transfer_len：配置的数据传输长度：如1位数据长度为1，512位数据长度为512，1024位数据长度为1024。<br>|
| 可搭配的辅助测试命令 | AT+UART_PUTCHAR |
| 示例                 | 发送命令AT+UART_RX_COUNT=9600,3,0,0,0,0,10到B板,  B板对应串口打印READY后，发送命令 AT+UART_TX_COUNT=9600,3,0,0,0,0,10到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+RTC_INTERFACE

| 命令格式 | AT+UART_INTERFACE=0                                             |
| -------- | ------------------------------------------------------------ |
| 命令功能 | UART模块接口测试。该命令将会测试UART模块下所有的CSI 接口， 测试CSI接口时将会传入异常值、非法值，验证CSI接口返回值是否符合期望 |
| 命令参数 | 无                                                           |
| 示例     | 发送命令AT+UART_INTERFACE=0到A板，测试完成后， 测试板对应串口会打印OK；否则测试失败 |
