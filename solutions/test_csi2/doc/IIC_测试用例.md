## IIC

### 概要
该文档主要描述IIC模块的CSI2.0自动化测试AT命令。 文档中所涉及的这些命令是用来测试IIC模块的接口工作是否正常、数据传输是否正确、接口返回值是否符合预期。


### 测试原理
测试时，应使被测试板与另外一块具备IIC功能的板子连接，并发送AT+PINMUX_CONFIG到被测试板把需要测试的引脚功能复用成IIC的引脚功能后，然后发送IIC测试命令进行相应的测试；测试程序会按照按照测试命令的配置进行相应的数据收发测试， 若接收方接收到的数据与预期一致，则相应的测试成功完成，否则测试失败。


### 测试流程
- 测试前应当准备好测试板（以下简称A板）、测试辅助板（以下简称B板）
- 选定需要测试的IIC针脚，并把A、B两块板子的IIC针脚相连
- 发送PINMUX_CONFIG命令到A板、B板， 把以上选定的引脚复用成IIC功能
- 发送IIC测试命令进行测试 （测试命令功能请参照下文）


### AT命令列表
以下是IIC模块对应的测试命令列表及其功能简要介绍。
| AT命令列表                 | 参数                    							 |	功能         		   |
| -------------------------- | --------------------------------------------------| ----------------------- |
| AT+IIC_INTERFACE           |None   				   							 | IIC 模块接口测试        |
| AT+IIC_MASTER_ASYNC_SEND   |addr_mode,speed,own_addr,trans_size		         | MASTER的异步模式发送测试  |
| AT+IIC_MASTER_SYNC_SEND    |addr_mode,speed,own_addr,trans_size		         | MASTER的同步模式发送测试  |
| AT+IIC_MASTER_DMA_SEND     |addr_mode,speed,own_addr,trans_size		         | MASTER的DMA模式发送测试 |
| AT+IIC_SLAVE_ASYNC_SEND    |addr_mode,speed,own_addr,trans_size,timeout        | SLAVE的异步模式发送测试  |
| AT+IIC_SLAVE_SYNC_SEND     |addr_mode,speed,own_addr,trans_size,timeout        |SLAVE的同步模式发送测试   |
| AT+IIC_SLAVE_DMA_SEND      |addr_mode,speed,own_addr,trans_size,timeout        |SLAVE的DMA模式发送测试     |
| AT+IIC_MASTER_ASYNC_RECEIVE|addr_mode,speed,trans_size,slave_addr		         |MASTER的同步模式接收测试   |
| AT+IIC_MASTER_SYNC_RECEIVE |addr_mode,speed,trans_size,slave_addr,timeout      |MASTER的同步模式接收测试   |
| AT+IIC_MASTER_DMA_RECEIVE  |addr_mode,speed,trans_size,slave_addr		         |MASTER的DMA模式接收测试  |
| AT+IIC_SLAVE_ASYNC_RECEIVE |addr_mode,speed,trans_size,slave_addr		         |SLAVE的异步模式接收测试   |
| AT+IIC_SLAVE_SYNC_RECEIVE  |addr_mode,speed,own_addr,trans_size,timeout        |SLAVE的同步模式接收测试  |
| AT+IIC_SLAVE_DMA_RECEIVE   |addr_mode,speed,own_addr,trans_size		         | SLAVE的DMA模式接收测试   |
| AT+IIC_MEMORY_TRANSFER     |addr_mode,speed,trans_size,slave_addr,timeout,mem_addr,mem_addr_size        | IIC内存传输  		   |
| AT+IIC_GET_STATE_MASTER_SEND |addr_mode,speed,trans_size,slave_addr		     | IIC获取MASTER状态测试     |
| AT+IIC_GET_STATE_SLAVE_RECEIVE |addr_mode,speed,own_addr,trans_size		     | IIC获取SLAVE状态测试     |

### 参数列表
以下是GPIO模块对应的AT测试命令共用参数的简要介绍。

| 参数名字       | 释义                                              | 取值范围                                                     |
| ---------------| ------------------------------------------------- | ------------------------------------------------------------ |
| addr_mode      | 数据位宽度          								 | 0：IIC_ADDRESS_7BIT 1：IIC_ADDRESS_10BIT                     |
| speed          | 传输速率											 | 0:IIC_BUS_SPEED_STANDARD;1: IIC_BUS_SPEED_FAST;2:IIC_BUS_SPEED_FAST_ FAST_BUS;3:IIC_BUS_SPEED_HIGH                                                 |
| own_addr       | 表示master设备本身地址                            | 0x0-0xFFFF													|
| trans_size     | 表示传输的数据长度                                | 0~1024Byte                                         |
| timeout        | 表示超时时间                                      | 0x0 - 0xFFFF     |
| slave_addr     | 表示从机本身的iic通信地址                         | 0x0 - 0xFFFF                                         |
| mem_addr       | eeprom读取的起始地址                              | 0x0 - 0xFFFF                                                 |
| mem_addr_size  | 表示eeprom读取的地址长度                          | 0x0 - 0xFFFF     |


### 命令详解

### AT+IIC_INTERFACE

| 命令格式 | AT+IIC_INTERFACE |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | IIC模块接口测试, 该命令将会测试IIC模块下所有的CSI 接口， 测试CSI接口时将会传入异常值、非法值，验证CSI接口返回值是否符合期望 |
| 命令参数 | 无 |
|配对命令|无|
| 示例 | AT+IIC_INTERFACE<br>发送给下位机后， 若测试通过，则串口最终会打印OK |


| 命令格式 | AT+IIC_MASTER_ASYNC_SEND |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | 测试IIC模块ASYNC模式下MASTER的发送功能 |
| 示例     | 分别对A、B两块开发板发送AT+PINMUX_CONFIG=0,21,0、AT+PINMUX_CONFIG=0,22,0命令，将两块板的引脚复用成IIC功能引脚，
然后再分别对A板和B板发送AT测试命令，先对A板发送AT+IIC_SLAVE_ASYNC_RECEIVE=0,0,22,200；<br/>然后再对B板发送AT+IIC_MASTER_ASYNC_SEND=0,0,200,22，发送命令之后，A板和B板会分别打印OK，即为测试成功，否则测试失败<br/> |
| 可搭配的辅助测试命令 | AT+IIC_SLAVE_ASYNC_RECEIVE |


| 命令功能 | 测试IIC模块ASYNC模式下MASTER的发送功能 |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | 测试I2S模块ASYNC模式下Slave的接收功能 |
| 示例     | 分别对A、B两块开发板发送AT+PINMUX_CONFIG=0,21,0、AT+PINMUX_CONFIG=0,22,0命令，将两块板的引脚复用成IIC功能引脚，
然后再分别对A板和B板发送AT测试命令，先对A板发送AT+IIC_SLAVE_ASYNC_RECEIVE=0,0,22,200；<br/>然后再对B板发送AT+IIC_MASTER_ASYNC_SEND=0,0,200,22，发送命令之后，A板和B板会分别打印OK，即为测试成功，否则测试失败<br/> |
| 可搭配的辅助测试命令 | AT+IIC_MASTER_ASYNC_SEND |


| 命令格式 | AT+IIC_MASTER_SYNC_SEND |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | 测试IIC模块ASYNC模式下MASTER的发送功能 |
| 示例     | 分别对A、B两块开发板发送AT+PINMUX_CONFIG=0,21,0、AT+PINMUX_CONFIG=0,22,0命令，将两块板的引脚复用成IIC功能引脚，
然后再分别对A板和B板发送AT测试命令，先对A板发送AT+IIC_SLAVE_SYNC_RECEIVE=0,0,22,200；<br/>然后再对B板发送AT+IIC_MASTER_SYNC_SEND=0,0,200,22，发送命令之后，A板和B板会分别打印OK，即为测试成功，否则测试失败<br/> |
| 可搭配的辅助测试命令 | AT+IIC_SLAVE_SYNC_RECEIVE |


| 命令格式 | AT+IIC_SLAVE_SYNC_RECEIVE |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | 测试IIC模块ASYNC模式下MASTER的发送功能 |
| 示例     | 分别对A、B两块开发板发送AT+PINMUX_CONFIG=0,21,0、AT+PINMUX_CONFIG=0,22,0命令，将两块板的引脚复用成IIC功能引脚，
然后再分别对A板和B板发送AT测试命令，先对A板发送AT+IIC_SLAVE_SYNC_RECEIVE=0,0,22,200；<br/>然后再对B板发送AT+IIC_MASTER_SYNC_SEND=0,0,200,22，发送命令之后，A板和B板会分别打印OK，即为测试成功，否则测试失败<br/> |
| 可搭配的辅助测试命令 | AT+IIC_MASTER_SYNC_SEND |


| 命令格式 | AT+IIC_MASTER_DMA_SEND |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | 测试IIC模块ASYNC模式下MASTER的发送功能 |
| 示例     | 分别对A、B两块开发板发送AT+PINMUX_CONFIG=0,21,0、AT+PINMUX_CONFIG=0,22,0命令，将两块板的引脚复用成IIC功能引脚，
然后再分别对A板和B板发送AT测试命令，先对A板发送AT+IIC_SLAVE_DMA_RECEIVE=0,0,22,200；<br/>然后再对B板发送AT+IIC_MASTER_DMA_SEND=0,0,200,22，发送命令之后，A板和B板会分别打印OK，即为测试成功，否则测试失败<br/> |
| 可搭配的辅助测试命令 | AT+IIC_SLAVE_DMA_RECEIVE |


| 命令格式 | AT+IIC_SLAVE_DMA_RECEIVE |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | 测试IIC模块ASYNC模式下MASTER的发送功能 |
| 示例     | 分别对A、B两块开发板发送AT+PINMUX_CONFIG=0,21,0、AT+PINMUX_CONFIG=0,22,0命令，将两块板的引脚复用成IIC功能引脚，
然后再分别对A板和B板发送AT测试命令，先对A板发送AT+IIC_SLAVE_DMA_RECEIVE=0,0,22,200；<br/>然后再对B板发送AT+IIC_MASTER_DMA_SEND=0,0,200,22，发送命令之后，A板和B板会分别打印OK，即为测试成功，否则测试失败<br/> |
| 可搭配的辅助测试命令 | AT+IIC_MASTER_DMA_SEND |


| 命令格式 | AT+IIC_MEMORY_TRANSFER |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | 测试IIC模块ASYNC模式下MASTER的发送功能 |
| 示例     | 分别对A、B两块开发板发送AT+PINMUX_CONFIG=0,21,0、AT+PINMUX_CONFIG=0,22,0命令，将两块板的引脚复用成IIC功能引脚，
然后再分别对A板和B板发送AT测试命令，先对A板发送AT+IIC_SLAVE_DMA_RECEIVE=0,0,22,200；<br/>然后再对B板发送AT+IIC_MASTER_DMA_SEND=0,0,200,22，发送命令之后，A板和B板会分别打印OK，即为测试成功，否则测试失败<br/> |
| 可搭配的辅助测试命令 | 无 |
