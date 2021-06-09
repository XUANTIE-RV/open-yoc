## DMA

### 概要

该文档主要描述DMA模块的CSI2.0自动化测试AT命令。 文档中所涉及的这些命令是用来测试DMA模块的接口工作是否正常、数据传输是否正确、接口返回值是否符合预期。


### 测试原理

测试时，应给被测试板发送DMA测试命令进行相应的测试；测试程序会按照按照测试命令的配置进行相应的测试， 若结果与预期一致，则相应的测试成功完成，否则测试失败。


### 测试流程

- 测试前应当准备好测试板（以下简称A板）
- 发送DMA测试命令进行测试 （测试命令功能请参照下文）


### AT命令列表 

以下是DMA模块对应的测试命令列表及其功能简要介绍。

| AT命令列表                | 功能                                    |
| ------------------------- | --------------------------------------- |
| AT+DMA_MEMORY_TO_MEMORY   | 测试内存到内存的数据传输通过DMA搬运     |
| AT+DMA_CHANNEL_ALLOC_FREE | 测试DMA模块的通道申请和释放             |
| AT+DMA_HALF_DONE_EVENT    | 测试DMA模块的数据传输完成一半的事件触发 |


### 参数列表
以下是DMA模块对应的AT测试命令共用参数的简要介绍。

| 参数名字      		| 释义                                              | 取值范围                                                  |
| --------------------- | ------------------------------------------------- | --------------------------------------------------------- |
| dma_idx               | 表示使用那个DMA的设备号，0表示DMA0                | 0x0 - 0xFFFFFFFF                                          |
| channel_idx           | 表示DMA的通道号 									| 0x0 - 0xFFFFFFFF                                          |
| src_addr_inc          | 表示源地址变化方式 								| 0：地址递增方式<br>1：地址递减方式<br>2：地址不变方式<br> |
| dest_addr_inc         | 表示目的地址变化方式 								| 0：地址递增方式<br>1：地址递减方式<br>2：地址不变方式<br> |
| data_width            | 表示传输的数据的宽度								| 0x0 - 0xFFFFFFFF                                       	|
| group_length          | 表示使用那个UART的产生波形						| 0x0 - 0xFFFFFFFF                                          |
| transfer_length       | 表示数据传输长度 									| 0x0 - 0xFFFFFFFF                                          |


### AT+DMA_MEMORY_TO_MEMORY

| 命令格式 | AT+DMA_MEMORY_TO_MEMORY=dma_idx, channel_idx, data_width, src_addr_inc, dest_addr_inc, group_length, transfer_length |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | 测试内存到内存的数据传输通过DMA搬运 |
| 命令参数 | dma_idx:表示使用哪一个DMA。如DMA0值为0;DMA1值为1<br/>channel_idx:表示哪一个通道。如通道0值为0，通道3值为3<br/>data_width:表示传输的数据的宽度。8位宽度为0；32位宽度为2。具体见dma.h 中的csi_dma_data_width_t定义<br/>src_addr_inc：表示数据传输时源地址的变化类型。如自增方式为0；自减方式为1。具体见dma.h 中的csi_dma_addr_inc_t定义<br/>dest_addr_inc：表示数据传输时目的地址的变化类型。如自增方式为0；自减方式为1。具体见dma.h 中的csi_dma_addr_inc_t定义<br/>group_length：表示数据传输时硬件的一组时多少个字节。16位数据类型<br/>transfer_length: 表示传输的数据长度（bytes）。32位数据类型 |
| 示例 | 发送命令AT+DMA_MEMORY_TO_MEMORY=0,0,2,0,1,100,4096到A板，测试完成后， A板对应串口会打印OK；否则测试失败 |

### AT+DMA_CHANNEL_ALLOC_FREE

| 命令格式 | AT+DMA_CHANNEL_ALLOC_FREE=dma_idx,channel_idx                |
| -------- | ------------------------------------------------------------ |
| 命令功能 | 测试DMA模块的通道申请和释放                                  |
| 命令参数 | dma_idx:表示使用哪一个DMA。如DMA0值为0;DMA1值为1<br/>channel_idx:表示哪一个通道。如通道0值为0，通道3值为3 |
| 示例     | 发送命令AT+DMA_CHANNEL_ALLOC_FREE=0,0到A板，测试完成后， A板对应串口会打印OK；否则测试失败 |



### AT+DMA_HALF_DONE_EVENT

| 命令格式 | AT+DMA_HALF_DONE_EVENT=dma_idx,channel_idx, transfer_length  |
| -------- | ------------------------------------------------------------ |
| 命令功能 | 测试DMA模块的数据传输完成一半的事件触发                      |
| 命令参数 | dma_idx:表示使用哪一个DMA。如DMA0值为0;DMA1值为1<br/>channel_idx:表示哪一个通道。如通道0值为0，通道3值为3<br/>transfer_length: 表示传输的数据长度（bytes）。32位数据类型 |
| 示例     | 发送命令AT+DMA_HALF_DONE_EVENT=0,0,2048到A板，测试完成后， A板对应串口会打印OK；否则测试失败 |
