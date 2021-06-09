## iic

### 概要

该文档主要描述iic模块的CSI1.0自动化测试AT命令。 文档中所涉及的这些命令是用来测试(1)iic模块的接口工作是否正常以及iic实例支持的能力的正确性（2）csi_iic_slave_receive在不同的数据位宽度、不同iic传输速度和不同模式从机接收不同长度的数据的正确性（3）csi_iic_slave_send在不同的数据位宽度、不同iic传输速度和不同模式从机发送不同长度的数据的正确性（4）csi_iic_master_receive在不同的数据位宽度、不同iic传输速度和不同模式主机接收不同长度的数据的正确性（5）csi_iic_master_send在不同的数据位宽度、不同iic传输速度和不同模式主机发送不同长度的数据的正确性
（6）测试读写外部eeprom的数据的正确性


### 测试原理

测试时，应使被测试板A开发板与另外一块拥有iic功能的B开发板连接，并分别发送AT+PINMUX_CONFIG命令到A和B两块测试板把需要测试的引脚功能复用成iic的引脚功能后，然后发送iic测试命令进行相应的测试；测试程序会按照按照测试命令的配置进行相应的数据收发测试， 若接收方接收到的数据与预期一致，则相应的测试成功完成，否则测试失败。



### 测试流程

- 测试前应当准备好测试板（以下简称A板）、测试辅助板（以下简称B板）
- 选定需要测试的iic针脚，并把A、B两块板子的iic针脚相连
- 将选定的iic针脚与一块外部的eeprom相连
- 发送PINMUX_CONFIG命令到A板、B板， 把以上选定的引脚复用成iic功能
- 发送iic测试命令进行测试 （测试命令功能请参照下文）



### AT命令列表

以下是iic模块对应的测试命令列表及其功能简要介绍。

| AT命令列表                                                   | 功能                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| AT+iic_INTERFACE                                             | iic模块接口测试。该命令将会测试iic模块下所有的CSI 接口， 测试CSI接口时将会传入异常值、非法值，验证CSI接口返回值是否符合期望 |
| AT+iic_MEMORY_TRANSFER=idx,addr_mode,speed,slave_addr,trans_size,mem_addr | 读写外部的eeprom，将写入的eeprom的数据与读取的数据比较          |
| AT+iic_MASTER_SEND=idx,addr_mode,speed,slave_addr,trans_size,iic_mode              | iic主模式发送数据                                            |
| AT+iic_SLAVE_RECEIVE=idx,addr_mode,speed,own_addr,trans_size,iic_mode              | iic从模式接收数据                                            |
| AT+iic_MASTER_RECEIVE=idx,addr_mode,speed,slave_addr,trans_size,iic_mode           | iic主模式接收数据                                            |
| AT+iic_SLAVE_SEND=idx,addr_mode,speed,own_addr,trans_size,iic_mode                 | iic从模式发送数据                                            |



### 参数列表

以下是iic模块对应的AT测试命令共用参数的简要介绍。

| 参数名字   | 释义                          | 取值范围                                                     |
| ---------- | ----------------------------- | -----------------------------------------------------------|
| idx        | 表示使用那一组外设，0表示iic0 | 0x0 - 0xFFFFFFFF                                             |
| addr_mode  | 数据位宽度                    | 0：iic_ADDRESS_7BIT<br/>1：iic_ADDRESS_10BIT                |
| speed      | 时钟相位极性                  | 0：iic_BUS_SPEED_STANDARD<br/>1：iic_BUS_SPEED_FAST<br/>2：iic_BUS_SPEED_FAST_PLUS<br/>3：iic_BUS_SPEED_HIGH |
| slave_addr | 从机的iic通信地址             | 0x0 - 0xFFFF                                                 |
| trans_size | 传输的数据长度                | 0~1024B                                                      |
| own_addr   | 设备本身的iic通信地址         | 0x0 - 0xFFFF                                                 |
| mem_addr   | eeprom读取的起始地址          | 0x0 - 0xFFFF                                                 |

  

### 命令详解

#### AT+iic_INTERFACE

| 命令格式             | AT+iic_INTERFACE                                             |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | iic模块接口测试。该命令将会测试iic模块下所有的CSI 接口， 测试CSI接口时将会传入异常值、非法值，验证CSI接口返回值是否符合期望 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试接口指令<br>AT+iic_INTERFACE<br>测试完成后， 板子对应串口会打印OK；否则测试失败 |
| 备注                 |                                                              |



#### AT+iic_MEMORY_TRANSFER

| 命令格式             | AT+iic_MEMORY_TRANSFER=idx,addr_mode,speed,slave_addr,trans_size,mem_addr |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 读写外部的eeprom，将写入的eeprom的数据与读取的数据比较       |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 把A板的PB0、PB1和外部eeprom的SCL、SDA使用杜邦线连接, 然后按照下列步骤进行测试<br/>步骤1:配置A板的PB0、PB1引脚作为iic的功能,向A板发送下列命令<br/>    AT+PINMUX_CONFIG=6,0<br/>    AT+PINMUX_CONFIG=7,0<br/>步骤2:向A板发送SPI从接收命令<br/>    AT+iic_MEMORY_TRANSFER=0,0,0,80,1024,0<br/>测试完成后A板对应串口会打印OK；否则测试失败 |
| 备注                 |                                                              |



#### AT+iic_MASTER_SEND

| 命令格式             | AT+iic_MASTER_SEND=idx,addr_mode,speed,slave_addr,trans_size,iic_mode |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | iic主模式发送数据                                            |
| 可搭配的辅助测试命令 | AT+iic_SLAVE_RECEIVE                                         |
| 示例                 | 把A板的PB0、PB1和B板的PB0、PB1使用杜邦线连接, 然后按照下列步骤进行测试<br/>步骤1:配置A板的PB0、PB1引脚作为iic的功能,向A板发送下列命令<br/>    AT+PINMUX_CONFIG=6,0<br/>    AT+PINMUX_CONFIG=7,0<br/>步骤2:配置B板的PB0、PB1引脚作为iic的功能,向B板发送下列命令<br/>    AT+PINMUX_CONFIG=6,0<br/>    AT+PINMUX_CONFIG=7,0<br/> 步骤3:向B板发送iic从接收命令<br/>    AT+iic_SLAVE_RECEIVE=0,0,0,20,200,0<br/>步骤4:向A板发送iic主发送命令<br/>    AT+iic_MASTER_SEND=0,0,0,20,200,0<br/>测试完成后A板和B板对应串口会打印OK；否则测试失败 |
| 备注                 |                                                              |



#### AT+iic_SLAVE_RECEIVE

| 命令格式             | AT+iic_SLAVE_RECEIVE=idx,addr_mode,speed,own_addr,trans_size,iic_mode |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | iic从模式接收数据                                            |
| 可搭配的辅助测试命令 | AT+iic_MASTER_SEND                                           |
| 示例                 | 把A板的PB0、PB1和B板的PB0、PB1使用杜邦线连接, 然后按照下列步骤进行测试<br/>步骤1:配置A板的PB0、PB1引脚作为iic的功能,向A板发送下列命令<br/>    AT+PINMUX_CONFIG=6,0<br/>    AT+PINMUX_CONFIG=7,0<br/>步骤2:配置B板的PB0、PB1引脚作为iic的功能,向B板发送下列命令<br/>    AT+PINMUX_CONFIG=6,0<br/>    AT+PINMUX_CONFIG=7,0<br/> 步骤3:向B板发送iic从接收命令<br/>    AT+iic_SLAVE_RECEIVE=0,0,0,20,200,0<br/>步骤4:向A板发送iic主发送命令<br/>    AT+iic_MASTER_SEND=0,0,0,20,200,0<br/>测试完成后A板和B板对应串口会打印OK；否则测试失败 |
| 备注                 |                                                              |



#### AT+iic_MASTER_RECEIVE

| 命令格式             | AT+iic_MASTER_RECEIVE=idx,addr_mode,speed,slave_addr,trans_size,iic_mode |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | iic主模式接收数据                                            |
| 可搭配的辅助测试命令 | AT+iic_SLAVE_SEND                                            |
| 示例                 | 把A板的PB0、PB1和B板的PB0、PB1使用杜邦线连接, 然后按照下列步骤进行测试<br/>步骤1:配置A板的PB0、PB1引脚作为iic的功能,向A板发送下列命令<br/>    AT+PINMUX_CONFIG=6,0<br/>    AT+PINMUX_CONFIG=7,0<br/>步骤2:配置B板的PB0、PB1引脚作为iic的功能,向B板发送下列命令<br/>    AT+PINMUX_CONFIG=6,0<br/>    AT+PINMUX_CONFIG=7,0<br/> 步骤3:向B板发送iic从发送命令<br/>    AT+iic_SLAVE_SEND=0,0,0,20,200,0<br/>步骤4:向A板发送iic主接收命令<br/>    AT+iic_MASTER_RECEIVE=0,0,0,20,200,0<br/>测试完成后A板和B板对应串口会打印OK；否则测试失败 |
| 备注                 |                                                              |



#### AT+iic_SLAVE_SEND

| 命令格式             | AT+iic_SLAVE_SEND=idx,addr_mode,speed,own_addr,trans_size,iic_mode    |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | iic从模式发送数据                                            |
| 可搭配的辅助测试命令 | AT+iic_MASTER_RECEIVE                                        |
| 示例                 | 把A板的PB0、PB1和B板的PB0、PB1使用杜邦线连接, 然后按照下列步骤进行测试<br/>步骤1:配置A板的PB0、PB1引脚作为iic的功能,向A板发送下列命令<br/>    AT+PINMUX_CONFIG=6,0<br/>    AT+PINMUX_CONFIG=7,0<br/>步骤2:配置B板的PB0、PB1引脚作为iic的功能,向B板发送下列命令<br/>    AT+PINMUX_CONFIG=6,0<br/>    AT+PINMUX_CONFIG=7,0<br/> 步骤3:向B板发送iic从发送命令<br/>    AT+iic_SLAVE_SEND=0,0,0,20,200,0<br/>步骤4:向A板发送iic主接收命令<br/>    AT+iic_MASTER_RECEIVE=0,0,0,20,200,0<br/>测试完成后A板和B板对应串口会打印OK；否则测试失败 |
| 备注                 |                                                              |

