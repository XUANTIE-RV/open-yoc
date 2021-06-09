## SPI

### 概要
该文档主要描述SPI模块的CSI2.0自动化测试AT命令。 文档中所涉及的这些命令是用来测试SPI模块的接口工作是否正常、数据传输是否正确、接口返回值是否符合预期。


### 测试原理
测试时，应使被测试板与另外一块具备SPI功能的板子连接，并发送AT+PINMUX_CONFIG到被测试板把需要测试的引脚功能复用成SPI的引脚功能后，然后发送SPI测试命令进行相应的测试；测试程序会按照按照测试命令的配置进行相应的数据收发测试， 若接收方接收到的数据与预期一致，则相应的测试成功完成，否则测试失败。


### 测试流程
- 测试前应当准备好测试板（以下简称A板）、测试辅助板（以下简称B板）
- 选定需要测试的SPI针脚，并把A、B两块板子的SPI针脚相连
- 发送PINMUX_CONFIG命令到A板、B板， 把以上选定的引脚复用成SPI功能
- 发送SPI测试命令进行测试 （测试命令功能请参照下文）


### AT命令列表
以下是SPI模块对应的测试命令列表及其功能简要介绍。
| AT命令列表                 | 功能                    |
| -------------------------- | ----------------------- |
| AT+SPI_INTERFACE           | SPI 模块接口测试        |
| AT+SPI_SYNC_SEND_M         | master同步模式发送测试  |
| AT+SPI_SYNC_SEND_S         | slave同步模式发送 测试  |
| AT+SPI_ASYNC_SEND_M        | master异步模式发送 测试 |
| AT+SPI_ASYNC_SEND_S        | slave异步模式发送 测试  |
| AT+SPI_SYNC_RECEIVE_M      | master同步模式接收      |
| AT+SPI_SYNC_RECEIVE_S      | slave同步模式接收       |
| AT+SPI_ASYNC_RECEIVE_M     | master异步模式接收      |
| AT+SPI_ASYNC_RECEIVE_S     | slave异步模式接收       |
| AT+SPI_SYNC_SENDRECEIVE_M  | master同步模式发送接收  |
| AT+SPI_SYNC_SENDRECEIVE_S  | slave同步模式发送接收   |
| AT+SPI_ASYNC_SENDRECEIVE_M | master异步模式发送接收  |
| AT+SPI_ASYNC_SENDRECEIVE_S | slave异步模式发送接收   |
| AT+SPI_DMA_SEND_M          | master DMA模式下发送    |
| AT+SPI_DMA_SEND_S          | slave DMA模式下发送     |
| AT+SPI_DMA_RECEIVE_M       | master DMA模式接收      |
| AT+SPI_DMA_RECEIVE_S       | slave DMA模式接收       |
| AT+SPI_DMA_SENDRECEIVE_M   | master DMA模式发送接收  |
| AT+SPI_DMA_SENDRECEIVE_S   | slave DMA模式发送接收   |

注：1）以上命令若带_M, 则表示主从模式为master, 若带_S则表示主从模式为slave。 依照SPI通信协议原理，应当首先发送slave模式的命令到开发板， 然后发送master命令到另外一块开发板


### AT+SPI_INTERFACE

| 命令格式 | AT+SPI_INTERFACE |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | SPI模块接口测试, 该命令将会测试SPI模块下所有的CSI 接口， 测试CSI接口时将会传入异常值、非法值，验证CSI接口返回值是否符合期望 |
| 命令参数 | 无 |
|配对命令|无|
| 示例 | AT+SPI_INTERFACE<br>发  送给下位机后， 若测试通过，则串口最终会打印OK |


### AT+SPI_SYNC_SEND_M

| 命令格式 | AT+SPI_SYNC_SEND_M=frame_len,cp_format,baud |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | master同步模式下发送数据； 发送该命令到被测试板时，应预先发送slave接收数据的命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
|配对命令|主从模式为slave且接收数据的命令|
| 示例 | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br/>发送命令AT+SPI_SYNC_RECEIVE_S=8,0,2500000到B板,  B板对应串口打印READY后，发送命令 AT+SPI_SYNC_SEND_M=8,0,2500000 到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败<br> |


### AT+SPI_SYNC_SEND_S

| 命令格式 | AT+SPI_SYNC_SEND_S=frame_len,cp_format,baud                  |
| -------- | ------------------------------------------------------------ |
| 命令功能 | slave同步模式下发送数据； 测试时，应首先发送该命令到测试板，然后发送master模式的接收数据命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
|配对命令|主从模式为master且接收数据的命令|
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br/>发送命令AT+SPI_SYNC_SEND_S=8,0,2500000到A板,  A板对应串口打印READY后，<br>发送命令 AT+SPI_SYNC_RECEIVE_M=8,0,2500000 到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+SPI_ASYNC_SEND_S

| 命令格式 | AT+SPI_ASYNC_SEND_S=frame_len,cp_format,baud                 |
| -------- | ------------------------------------------------------------ |
| 命令功能 | slave异步模式下发送数据； 测试时，应首先发送该命令到测试板，然后发送master模式的接收数据命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
|配对命令|主从模式为master且接收数据的SPI命令|
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br/>发送命令AT+SPI_ASYNC_SEND_S=8,0,2500000到A板,  A板对应串口打印READY后，<br>发送命令 AT+SPI_ASYNC_RECEIVE_M=8,0,2500000 到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+SPI_ASYNC_SEND_M

| 命令格式 | AT+SPI_ASYNC_SEND_M=frame_len,cp_format,baud                 |
| -------- | ------------------------------------------------------------ |
| 命令功能 | master异步模式下发送数据； 测试时，发送该命令到被测试板时，应预先发送slave接收数据的命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
| 配对命令 | 主从模式为slave且接收数据的SPI命令                           |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br/>发送命令AT+SPI_ASYNC_RECEIVE_S=8,0,2500000到A板,  A板对应串口打印READY后，<br>发送命令 AT+SPI_ASYNC_SEND_M=8,0,2500000 到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+SPI_SYNC_RECEIVE_M

| 命令格式 | AT+SPI_SYNC_RECEIVE_M=frame_len,cp_format,baud               |
| -------- | ------------------------------------------------------------ |
| 命令功能 | master同步模式下接收数据； 测试时，发送该命令到被测试板时，应预先发送slave发送数据的命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义，使用时应替换成真实枚举值<br>baud：通信的波特率。32位数据类型 |
| 配对命令 | 主从模式为slave且发送数据的SPI命令                           |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br/>发送命令AT+SPI_ASYNC_RECEIVE_S=8,0,2500000到A板,  A板对应串口打印READY后，<br>发送命令 AT+SPI_ASYNC_SEND_M=8,0,2500000 到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+SPI_SYNC_RECEIVE_S

| 命令格式 | AT+SPI_SYNC_RECEIVE_S=frame_len,cp_format,baud               |
| -------- | ------------------------------------------------------------ |
| 命令功能 | slave同步模式下接收数据； 测试时，应首先发送该命令到测试板，然后发送master模式的发送数据命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
| 配对命令 | 主从模式为master且发送数据的SPI命令                          |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br/>发送命令AT+SPI_SYNC_SEND_S=8,0,2500000到A板,  A板对应串口打印READY后，<br>发送命令 AT+SPI_SYNC_RECEIVE_M=8,0,2500000 到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+SPI_ASYNC_RECEIVE_M

| 命令格式 | AT+SPI_AYNC_RECEIVE_M=frame_len,cp_format,baud               |
| -------- | ------------------------------------------------------------ |
| 命令功能 | master异步模式下接收数据； 测试时，发送该命令到被测试板时，应预先发送slave发送数据的命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义，使用时应替换成真实枚举值<br>baud：通信的波特率(32位数据类型) |
| 配对命令 | 主从模式为slave且发送数据的SPI命令                           |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br/>发送命令AT+SPI_ASYNC_RECEIVE_S=8,0,2500000到A板,  A板对应串口打印READY后，<br>发送命令 AT+SPI_ASYNC_SEND_M=8,0,2500000 到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+SPI_ASYNC_RECEIVE_S

| 命令格式 | AT+SPI_ASYNC_RECEIVE_S=frame_len,cp_format,baud              |
| -------- | ------------------------------------------------------------ |
| 命令功能 | slave异步模式下接收数据； 测试时，应首先发送该命令到测试板，然后发送master模式的发送数据命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
| 配对命令 | 主从模式为master且发送数据的SPI命令                          |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br/>发送命令AT+SPI_SYNC_SEND_S=8,0,2500000到A板,  A板对应串口打印READY后，<br>发送命令 AT+SPI_SYNC_RECEIVE_M=8,0,2500000 到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+SPI_SYNC_SENDRECEIVE_M

| 命令格式 | AT+SPI_SYNC_SENDRECEIVE_M=frame_len,cp_format,baud           |
| -------- | ------------------------------------------------------------ |
| 命令功能 | master同步模式下发送接收数据； 发送该命令到被测试板时，应预先发送slave 发送接收数据的命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
| 配对命令 | 主从模式为slave且为发送接收数据的命令                        |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br/>发送命令AT+SPI_SYNC_SENDRECEIVE_S=8,0,2500000到B板,  B板对应串口打印READY后，发送命令 AT+SPI_SYNC_SENDRECEIVE_M=8,0,2500000 到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败<br> |


### AT+SPI_SYNC_SENDRECEIVE_S

| 命令格式 | AT+SPI_SYNC_SENDRECEIVE_S=frame_len,cp_format,baud           |
| -------- | ------------------------------------------------------------ |
| 命令功能 | slave同步模式下发送接收数据； 测试时，应首先发送该命令到测试板，然后发送master模式的发送接收数据命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
| 配对命令 | 主从模式为master且接发送收数据的命令                         |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br/>发送命令AT+SPI_SYNC_SENDRECEIVE_S=8,0,2500000到A板,  A板对应串口打印READY后，<br>发送命令 AT+SPI_SYNC_SENDRECEIVE_M=8,0,2500000 到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+SPI_ASYNC_SENDRECEIVE_M

| 命令格式 | AT+SPI_ASYNC_SENDRECEIVE_M=frame_len,cp_format,baud          |
| -------- | ------------------------------------------------------------ |
| 命令功能 | master异步模式下发送接收数据； 发送该命令到被测试板时，应预先发送slave 发送接收数据的命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
| 配对命令 | 主从模式为slave且为发送接收数据的命令                        |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br/>发送命令AT+SPI_ASYNC_SENDRECEIVE_S=8,0,2500000到B板,  B板对应串口打印READY后，发送命令 AT+SPI_ASYNC_SENDRECEIVE_M=8,0,2500000 到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败<br> |


### AT+SPI_ASYNC_SENDRECEIVE_S

| 命令格式 | AT+SPI_ASYNC_SENDRECEIVE_S=frame_len,cp_format,baud          |
| -------- | ------------------------------------------------------------ |
| 命令功能 | slave异步模式下发送接收数据； 测试时，应首先发送该命令到测试板，然后发送master模式的发送接收数据命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
| 配对命令 | 主从模式为master且接发送收数据的命令                         |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br/>发送命令AT+SPI_ASYNC_SENDRECEIVE_S=8,0,2500000到A板,  A板对应串口打印READY后，<br>发送命令 AT+SPI_ASYNC_SENDRECEIVE_M=8,0,2500000 到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+SPI_DMA_SEND_M

| 命令格式 | AT+SPI_DMA_SEND_M=frame_len,cp_format,baud                   |
| -------- | ------------------------------------------------------------ |
| 命令功能 | master DMA模式下发送数据； 发送该命令到被测试板时，应预先发送slave接收数据的命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
| 配对命令 | 主从模式为slave且接收数据的命令                              |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br/>发送命令AT+SPI_DMA_RECEIVE_S=8,0,2500000到B板,  B板对应串口打印READY后，发送命令 AT+SPI_DMA_SEND_M=8,0,2500000 到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败<br> |


### AT+SPI_DMA_SEND_S

| 命令格式 | AT+SPI_DMA_SEND_S=frame_len,cp_format,baud                   |
| -------- | ------------------------------------------------------------ |
| 命令功能 | slave DMA模式下发送数据； 测试时，应首先发送该命令到测试板，然后发送master模式的接收数据命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
| 配对命令 | 主从模式为master且接收数据的命令                             |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）：<br>发送命令AT+SPI_DMA_SEND_S=8,0,2500000到A板,  A板对应串口打印READY后，<br>发送命令 AT+SPI_DMA_RECEIVE_M=8,0,2500000 到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+SPI_DMA_RECEIVE_M

| 命令格式 | AT+SPI_DMA_RECEIVE_M=frame_len,cp_format,baud                |
| -------- | ------------------------------------------------------------ |
| 命令功能 | master DMA模式下接收数据； 测试时，发送该命令到被测试板时，应预先发送slave发送数据的命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义，使用时应替换成真实枚举值<br>baud：通信的波特率(32位数据类型) |
| 配对命令 | 主从模式为slave且发送数据的SPI命令                           |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）： <br/>发送命令AT+SPI_DMA_RECEIVE_S=8,0,2500000到A板,  A板对应串口打印READY后，<br>发送命令 AT+SPI_DMA_SEND_M=8,0,2500000 到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+SPI_DMA_RECEIVE_S

| 命令格式 | AT+SPI_DMA_RECEIVE_S=frame_len,cp_format,baud                |
| -------- | ------------------------------------------------------------ |
| 命令功能 | slave DMA模式下接收数据； 测试时，应首先发送该命令到测试板，然后发送master模式的发送数据命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
| 配对命令 | 主从模式为master且发送数据的SPI命令                          |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）： <br/>发送命令AT+SPI_DMA_SEND_S=8,0,2500000到A板,  A板对应串口打印READY后，<br>发送命令 AT+SPI_DMA_RECEIVE_M=8,0,2500000 到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |


### AT+SPI_DMA_SENDRECEIVE_M

| 命令格式 | AT+SPI_DMA_SENDRECEIVE_M=frame_len,cp_format,baud            |
| -------- | ------------------------------------------------------------ |
| 命令功能 | master同步模式下发送接收数据； 发送该命令到被测试板时，应预先发送slave 发送接收数据的命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
| 配对命令 | 主从模式为slave且为发送接收数据的命令                        |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）： <br/>发送命令AT+SPI_SYNC_SENDRECEIVE_S=8,0,2500000到B板,  B板对应串口打印READY后，发送命令 AT+SPI_SYNC_SENDRECEIVE_M=8,0,2500000 到A板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败<br> |


### AT+SPI_DMA_SENDRECEIVE_S

| 命令格式 | AT+SPI_DMA_SENDRECEIVE_S=frame_len,cp_format,baud            |
| -------- | ------------------------------------------------------------ |
| 命令功能 | slave同步模式下发送接收数据； 测试时，应首先发送该命令到测试板，然后发送master模式的发送接收数据命令到测试辅助板 |
| 命令参数 | frame_len：数据宽度。如8位数据宽度为8。具体见spi.h 中的csi_spi_frame_len_t定义<br>cp_format：时钟相位极性。如配置CPL0 CPHA0 值为0。具体见spi.h 中的csi_spi_cp_format_t定义<br>baud：通信的波特率。32位数据类型 |
| 配对命令 | 主从模式为master且接发送收数据的命令                         |
| 示例     | 如下测试使用数据帧宽带为8，时钟极性为SPI_FORMAT_CPOL0_CPHA0（即Clock Polarity 0, Clock Phase 0）: <br>发送命令AT+SPI_DMA_SENDRECEIVE_S=8,0,2500000到A板,  A板对应串口打印READY后，<br>发送命令 AT+SPI_DMA_SENDRECEIVE_M=8,0,2500000 到B板。测试完成后， A板、B板对应串口都会打印OK；否则测试失败 |