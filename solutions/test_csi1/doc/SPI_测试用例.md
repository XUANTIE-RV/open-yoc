## SPI

### 概要

该文档主要描述SPI模块的CSI1.0自动化测试AT命令。 文档中所涉及的这些命令是用来测试(1)SPI模块的接口工作是否正常以及SPI实例支持的能力的正确性（2）测试csi_spi_send在不同的数据位宽度、不同的时钟相位极性、不同的传输速率、不同的数据端大小模式、不同的SPI模式、不同的从机选择模式下发送不同长度的数据的正确性（3）测试csi_spi_receive在不同的数据位宽度、不同的时钟相位极性、不同的传输速率、不同的数据端大小模式、不同的SPI模式、不同的从机选择模式下接收不同长度的数据的正确性（4）
测试csi_spi_transfer在不同的数据位宽度、不同的时钟相位极性、不同的传输速率、不同的数据端大小模式、不同的SPI模式、不同的从机选择模式下传输不同长度的数据的正确性（5）测试csi_spi_get_data_count获取设备上一次传输的数据个数的正确性



### 测试原理

测试时，应使被测试板与另外一块具备SPI功能的板子连接，并发送AT+PINMUX_CONFIG到被测试板把需要测试的引脚功能复用成SPI的引脚功能后，然后发送SPI测试命令进行相应的测试；测试程序会按照按照测试命令的配置进行相应的数据收发测试，若接收方接收到的数据与预期一致，则相应的测试成功完成，否则测试失败。



### 测试流程

- 测试前应当准备好测试板（以下简称A板）、测试辅助板（以下简称B板）
- 选定需要测试的SPI针脚（需要SCK/CS/TX/RX四个功能引脚），并把A、B两块板子的SPI针脚相连：A板SCK功能引脚接B板SCK功能引脚,A板CS功能引脚接B板CS功能引脚,A板TX功能引脚接B板RX功能引脚,A板RX功能引脚接B板TX功能引脚
- 发送PINMUX_CONFIG命令到A板、B板， 把以上选定的引脚复用成对应的功能
- 发送SPI测试命令进行测试 （测试命令功能请参照下文）



### AT命令列表

以下是SPI模块对应的测试命令列表及其功能简要介绍。

| AT命令列表                                                   | 功能                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| AT+SPI_INTERFACE                                             | SPI模块接口测试。该命令将会测试SPI模块下所有的CSI 接口， 测试CSI接口时将会传入异常值、非法值，验证CSI接口返回值是否符合期望 |
| AT+SPI_MASTER_SEND=idx,frame_len,cp_format,baud,bit_order,spi_mode,spi_ss_mode,spi_transfer_len    | SPI主模式发送数据                                            |
| AT+SPI_SLAVE_RECEIVE=idx,frame_len,cp_format,baud,bit_order,spi_mode,spi_ss_mode,spi_transfer_len  | SPI从模式接收数据                                            |
| AT+SPI_MASTER_RECEIVE=idx,frame_len,cp_format,baud,bit_order,spi_mode,spi_ss_mode,spi_transfer_len | SPI主模式接收数据                                            |
| AT+SPI_SLAVE_SEND=idx,frame_len,cp_format,baud,bit_order,spi_mode,spi_ss_mode,spi_transfer_len     | SPI从模式发送数据                                            |
| AT+SPI_TRANSFER=idx,frame_len,cp_format,baud,bit_order,spi_mode,spi_ss_mode,spi_transfer_len       | SPI传输数据                                                  |

### 参数列表

以下是SPI模块对应的AT测试命令共用参数的简要介绍。

| 参数名字  | 释义                          | 取值范围                                                      |
| --------- | ----------------------------- | ----------------------------------------------------------- |
| idx       | 表示使用那一组外设，0表示spi0 | 0x0 - 0xFFFFFFFF                                              |
| frame_len | 数据位宽度                    | 1~32                                                         |
| cp_format | 时钟相位极性                  | 0：SPI_FORMAT_CPOL0_CPHA0<br/>1：SPI_FORMAT_CPOL0_CPHA1<br/>2：SPI_FORMAT_CPOL1_CPHA0<br/>3：SPI_FORMAT_CPOL1_CPHA1 |
| baud      | 通信的波特率                  | 0x0 - 0xFFFFFFFF                                             |
| bit_order | 数据的大小端模式              | 0：SPI_ORDER_MSB2LSB<br/>1：SPI_ORDER_LSB2MSB                 |
| spi_mode      | spi模式                  | 0：SPI_MODE_INACTIVE；1：SPI_MODE_MASTER；2：SPI_MODE_SLAVE；3：SPI_MODE_MASTER_SIMPLEX；4：SPI_MODE_SLAVE_SIMPLEX                                             |
| spi_ss_mode | 数据的大小端模式              | 0：SPI_SS_MASTER_UNUSED；1：SPI_SS_MASTER_SW；2：SPI_SS_MASTER_HW_OUTPUT；3：SPI_SS_MASTER_HW_INPUT；4：SPI_SS_SLAVE_HW；5：SPI_SS_SLAVE_SW；                 |
| spi_transfer_len | 传输的数据长度              | 0~1kB                 |


### 命令详解

#### AT+SPI_INTERFACE

| 命令格式             | AT+SPI_INTERFACE                                             |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | SPI模块接口测试。该命令将会测试SPI模块下所有的CSI 接口， 测试CSI接口时将会传入异常值、非法值，验证CSI接口返回值是否符合期望 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试接口指令<br>AT+SPI_INTERFACE<br>测试完成后， 板子对应串口会打印OK；否则测试失败 |
| 备注                 |                                                              |



#### AT+SPI_MASTER_SEND

| 命令格式             | AT+SPI_MASTER_SEND=idx,frame_len,cp_format,baud,bit_order,spi_mode,spi_ss_mode,spi_transfer_len    |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | SPI主模式发送数据                                            |
| 可搭配的辅助测试命令 | AT+SPI_SLAVE_RECEIVE                                         |
| 示例                 | 把A板的P31、P32、P33、P34和B板的P31、P32、P34、P33使用杜邦线连接, 然后按照下列步骤进行测试<br/>步骤1:配置A板的PB2、PB3、PA6、PA7引脚作为SPI的功能,向A板发送下列命令<br/>    AT+PINMUX_CONFIG=19,16<br/>    AT+PINMUX_CONFIG=20,17<br/>    AT+PINMUX_CONFIG=21,18<br/>    AT+PINMUX_CONFIG=22,19<br/>步骤2:配置B板的P31、P32、P33、P34引脚作为SPI的功能,向B板发送下列命令<br/>    AT+PINMUX_CONFIG=19,16<br/>    AT+PINMUX_CONFIG=20,17<br/>    AT+PINMUX_CONFIG=21,18<br/>    AT+PINMUX_CONFIG=22,19<br/>步骤3:向B板发送SPI从接收命令<br/>    AT+SPI_SLAVE_RECEIVE=0,8,0,5000000,0,2,2,1024<br/>步骤4:向A板发送SPI主发送命令<br/>    AT+SPI_MASTER_SEND=0,8,0,5000000,0,1,2,1024<br/>测试完成后A板和B板对应串口会打印OK；否则测试失败 |
| 备注                 |                                                              |



#### AT+SPI_SLAVE_RECEIVE

| 命令格式             | AT+SPI_SLAVE_RECEIVE=idx,frame_len,cp_format,baud,bit_order,spi_mode,spi_ss_mode,spi_transfer_len  |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | SPI从模式接收数据                                            |
| 可搭配的辅助测试命令 | AT+SPI_MASTER_SEND                                           |
| 示例                 | 把A板的P31、P32、P33、P34和B板的P31、P32、P34、P33使用杜邦线连接, 然后按照下列步骤进行测试<br/>步骤1:配置A板的PB2、PB3、PA6、PA7引脚作为SPI的功能,向A板发送下列命令<br/>    AT+PINMUX_CONFIG=19,16<br/>    AT+PINMUX_CONFIG=20,17<br/>    AT+PINMUX_CONFIG=21,18<br/>    AT+PINMUX_CONFIG=22,19<br/>步骤2:配置B板的P31、P32、P33、P34引脚作为SPI的功能,向B板发送下列命令<br/>    AT+PINMUX_CONFIG=19,16<br/>    AT+PINMUX_CONFIG=20,17<br/>    AT+PINMUX_CONFIG=21,18<br/>    AT+PINMUX_CONFIG=22,19<br/>步骤3:向B板发送SPI从接收命令<br/>    AT+SPI_SLAVE_RECEIVE=0,8,0,5000000,0,2,2,1024<br/>步骤4:向A板发送SPI主发送命令<br/>    AT+SPI_MASTER_SEND=0,8,0,5000000,0,1,2,1024<br/>测试完成后A板和B板对应串口会打印OK；否则测试失败 |
| 备注                 |                                                              |



#### AT+SPI_MASTER_RECEIVE

| 命令格式             | AT+SPI_MASTER_RECEIVE=idx,frame_len,cp_format,baud,bit_order,spi_mode,spi_ss_mode,spi_transfer_len |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | SPI主模式接收数据                                            |
| 可搭配的辅助测试命令 | AT+SPI_SLAVE_SEND                                            |
| 示例                 | 把A板的P31、P32、P33、P34和B板的P31、P32、P34、P33使用杜邦线连接, 然后按照下列步骤进行测试<br/>步骤1:配置A板的PB2、PB3、PA6、PA7引脚作为SPI的功能,向A板发送下列命令<br/>    AT+PINMUX_CONFIG=19,16<br/>    AT+PINMUX_CONFIG=20,17<br/>    AT+PINMUX_CONFIG=21,18<br/>    AT+PINMUX_CONFIG=22,19<br/>步骤2:配置B板的P31、P32、P33、P34引脚作为SPI的功能,向B板发送下列命令<br/>    AT+PINMUX_CONFIG=19,16<br/>    AT+PINMUX_CONFIG=20,17<br/>    AT+PINMUX_CONFIG=21,18<br/>    AT+PINMUX_CONFIG=22,19<br/>步骤3:向B板发送SPI从发送命令<br/>    AT+SPI_SLAVE_SEND=0,8,0,240000,0,2,2,1024<br/>步骤4:向A板发送SPI主接收命令<br/>    AT+SPI_MASTER_RECEIVE=0,8,0,240000,0,1,2,1024<br/>测试完成后A板和B板对应串口会打印OK；否则测试失败 |
| 备注                 |                                                              |



#### AT+SPI_SLAVE_SEND

| 命令格式             | AT+SPI_SLAVE_SEND=idx,frame_len,cp_format,baud,bit_order,spi_mode,spi_ss_mode,spi_transfer_len     |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | SPI从模式发送数据                                            |
| 可搭配的辅助测试命令 | AT+SPI_MASTER_RECEIVE                                        |
| 示例                 | 把A板的P31、P32、P33、P34和B板的P31、P32、P34、P33使用杜邦线连接, 然后按照下列步骤进行测试<br/>步骤1:配置A板的PB2、PB3、PA6、PA7引脚作为SPI的功能,向A板发送下列命令<br/>    AT+PINMUX_CONFIG=19,16<br/>    AT+PINMUX_CONFIG=20,17<br/>    AT+PINMUX_CONFIG=21,18<br/>    AT+PINMUX_CONFIG=22,19<br/>步骤2:配置B板的P31、P32、P33、P34引脚作为SPI的功能,向B板发送下列命令<br/>    AT+PINMUX_CONFIG=19,16<br/>    AT+PINMUX_CONFIG=20,17<br/>    AT+PINMUX_CONFIG=21,18<br/>    AT+PINMUX_CONFIG=22,19<br/>步骤3:向B板发送SPI从发送命令<br/>    AT+SPI_SLAVE_SEND=0,8,0,240000,0,2,2,1024<br/>步骤4:向A板发送SPI主接收命令<br/>    AT+SPI_MASTER_RECEIVE=0,8,0,240000,0,1,2,1024<br/>测试完成后A板和B板对应串口会打印OK；否则测试失败 |
| 备注                 |                                                              |



#### AT+SPI_TRANSFER

| 命令格式             | AT+SPI_TRANSFER=idx,frame_len,cp_format,baud,bit_order,spi_mode,spi_ss_mode,spi_transfer_len     |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | SPI数据传输                                            |
| 可搭配的辅助测试命令 | AT+SPI_MASTER_RECEIVE                                        |
| 示例                 | 把A板的P31、P32、P33、P34和B板的P31、P32、P34、P33使用杜邦线连接, 然后按照下列步骤进行测试<br/>步骤1:配置A板的PB2、PB3、PA6、PA7引脚作为SPI的功能,向A板发送下列命令<br/>    AT+PINMUX_CONFIG=19,16<br/>    AT+PINMUX_CONFIG=20,17<br/>    AT+PINMUX_CONFIG=21,18<br/>    AT+PINMUX_CONFIG=22,19<br/>步骤2:配置B板的P31、P32、P33、P34引脚作为SPI的功能,向B板发送下列命令<br/>    AT+PINMUX_CONFIG=19,16<br/>    AT+PINMUX_CONFIG=20,17<br/>    AT+PINMUX_CONFIG=21,18<br/>    AT+PINMUX_CONFIG=22,19<br/>步骤3:向B板发送SPI从发送命令<br/>    AT+SPI_TRANSFER=0,8,0,240000,0,2,2,1024<br/>步骤4:向A板发送SPI主接收命令<br/>    AT+SPI_MASTER_RECEIVE=0,8,0,240000,0,1,2,1024<br/>测试完成后A板和B板对应串口会打印OK；否则测试失败 |
| 备注                 |                                                           |