## SPINAND

### 概要
该文档主要描述CSI2.0自动化测试框架下SPINAND模块的AT测试命令。 文档中所涉及的命令是用来测试SPINAND模块的接口工作是否正常、接口返回值是否符合预期和该模块的特性功能是否正常。

### 测试原理
PC主机用USB转串口线连接测试板（以下简称A板），作为PC同A板数据通信，命令收发的通道；同时PC会使用USB线连接A板的HAD调试器，作为程序下载的通道；A板上需外接使用SPI接口的NAND FLASH。通过PC下发AT指令到A板触发测试程序执行，当A板有测试成功的信息返回PC后，PC判定本次测试执行通过。

### 测试流程
- 测试前应在A板上外接一个SPI接口的NAND FLASH
- 发送引脚复用的配置命令（PINMUX_CONFIG）到A板，配置好需要使用到的引脚
- 发送SPINAND的AT测试命令进行测试

### 命令列表
以下是SPINAND模块对应的AT测试命令列表及其功能简要介绍。
| AT命令列表                       | 功能简介                   |
| -------------------------------- | -------------------------- |
| AT+SPINAND_INTERFACE | 测试API接口的异常参数判断 |
| AT+SPINAND_DEVICE_RESET | 测试nand flash host控制器向device发出复位信号 |
| AT+SPINAND_DEVICE_INFO | 获取device的infomation |
| AT+SPINAND_REPEAT_INIT | 循环初始化和反初始化nand flash host控制器和device |
| AT+SPINAND_BAD_BLOCK_CHECK | 测试坏块检测功能 |
| AT+SPINAND_BAD_BLOCK_MARK | 测试坏块标记功能 |
| AT+SPINAND_DEVICE_ERASE | 测试device擦除 |
| AT+SPINAND_DEVICE_WRITE_READ | 测试device的 data存储区域的读写 |
| AT+SPINAND_DEVICE_CROSS_CHECK| 测试nand flash在指定的范围内读写是否存在越界 |
| AT+SPINAND_DEVICE_SPARE_WRITE_READ | 测试device的 spare存储区域的读写 |
| AT+SPINAND_DEVICE_SPEED | 测试spi nand flash的读、写、擦除速度 |
| AT+STRESS_SPINAND | spi nand flash的稳定性测试 |


### 参数列表
以下是SPINAND模块对应的AT测试命令共用参数的简要介绍
| 参数名字 | 释义 | 取值范围 |
| -------- | ---- | ------- |
| qspi_idx | 选择QSPI控制器的idx号 | 0x0 ~ 0xFF,参考对应SOC支持的QSPI控制器个数 |
| gpio_idx | 选择作为CS脚的GPIO所属的GPIO控制器的idx号 | 0x0 ~ 0xFF,参考对应SOC支持的GPIO控制器个数 |
| pin | 作为CS脚的pin脚号 | 0x0 ~ 0xFF,考对应SOC支持的pin脚号定义 |
| frequence | 配置的QSPI发出的时钟频率 | 0x0 ~ 0xFFFF |
| repeat_times | 重复操作的次数 | 0x00 ~ 0xFFFF |
| addr | 操作的device的memory的起始地址 | 0x0 ~ 0xFFFFFFFF |
| size | 操作的device的memory的大小 | 0x0 ~ 0xFFFFFFFF |
| test_mode | 选择读写操作的测试模式 | 1: 只测试读操作<br> 2: 只测试写操作<br> other: 读写操作一起测试 |
| timer_idx | 选用哪一个timer用来计时 |
| cell_time | 设置计时的最小单元是多少毫秒 |


### 功能测试命令详解
#### AT+SPINAND_INTERFACE
|     |     |
| --- | --- |
| 命令格式 | AT+SPINAND_INTERFACE= |
| 命令功能 | 测试API接口的异常参数判断 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接NAND FLASH<br> 步骤2：PC向A板发送AT+SPINAND_INTERFACE= |
| 备注 | 该测试命令不需要跟参数 |


#### AT+SPINAND_DEVICE_RESET
|     |     |
| --- | --- |
| 命令格式 | AT+SPINAND_DEVICE_RESET=qspi_idx,frequence,gpio_idx,pin |
| 命令功能 | QSPI控制器向nand flash发送一个设备复位信号 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接NAND FLASH<br> 步骤2：PC向A板发送AT+SPINAND_DEVICE_RESET=0,2000000,0,1 |
| 备注 | 该复位信号可使用示波器捕获出来进行确认 |


#### AT+SPINAND_DEVICE_INFO
|     |     |
| --- | --- |
| 命令格式 | AT+SPINAND_DEVICE_INFO=qspi_idx,frequence,gpio_idx,pin |
| 命令功能 | 获取nand flash的设备信息 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接NAND FLASH<br> 步骤2：PC向A板发送AT+SPINAND_DEVICE_INFO=0,2000000,0,1 |
| 备注 | 获取到的信息需要验证同nand flash的硬件描述一致 |


#### AT+SPINAND_REPEAT_INIT
|     |     |
| --- | --- |
| 命令格式 | AT+SPINAND_REPEAT_INIT=qspi_idx,repeat_times |
| 命令功能 | 测试QSPI控制器和nand flash反复初始化和反初始化 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接NAND FLASH<br> 步骤2：PC向A板发送AT+SPINAND_REPEAT_INIT=0,2000 |
| 备注 | |


#### AT+SPINAND_BAD_BLOCK_CHECK
|     |     |
| --- | --- |
| 命令格式 | AT+SPINAND_BAD_BLOCK_CHECK=qspi_idx,frequence,gpio_idx,pin |
| 命令功能 | 测试能否检测出外接nand flash存在坏块 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接NAND FLASH<br> 步骤2：PC向A板发送AT+SPINAND_BAD_BLOCK_CHECK=0,2000000,0,1 |
| 备注 | 任何一块nand flash一定存在坏块 |


#### AT+SPINAND_BAD_BLOCK_MARK
|     |     |
| --- | --- |
| 命令格式 | AT+SPINAND_BAD_BLOCK_MARK=qspi_idx,frequence,addr,gpio_idx,pin |
| 命令功能 | 标记外接的nand flash指定地址所处的块为坏块 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接NAND FLASH<br> 步骤2：PC向A板发送AT+SPINAND_BAD_BLOCK_MARK=0,2000000,0x0,0,1 |
| 备注 | |


#### AT+SPINAND_DEVICE_ERASE
|     |     |
| --- | --- |
| 命令格式 | AT+SPINAND_DEVICE_ERASE=qspi_idx,frequence,td.addr,size,gpio_idx,pin |
| 命令功能 | 擦除外接的nand flash指定区间的数据，当size小于块大小时，任然按照块大小擦除 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接NAND FLASH<br> 步骤2：PC向A板发送AT+SPINAND_DEVICE_ERASE=0,2000000,0x0,0x100,0,1 |
| 备注 | |


#### AT+SPINAND_DEVICE_WRITE_READ
|     |     |
| --- | --- |
| 命令格式 | AT+SPINAND_DEVICE_WRITE_READ=qspi_idx,addr,size,frequence,gpio_idx,pin,test_mode |
| 命令功能 | 读写外接的nand flash指定data区间的数据 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接NAND FLASH<br> 步骤2：PC向A板发送AT+SPINAND_DEVICE_WRITE_READ=0,0x0,0x100,2000000,0,1,0 |
| 备注 | test_mode为1时只测试读功能，为2时只测试写功能，为其它值时读写功能按先写入后读取的序列测试 |


#### AT+SPINAND_DEVICE_SPARE_WRITE_READ
|     |     |
| --- | --- |
| 命令格式 | AT+SPINAND_DEVICE_SPARE_WRITE_READ=qspi_idx,addr,size,frequence,gpio_idx,pin,test_mode |
| 命令功能 | 读写外接的nand flash指定spare区间的数据 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接NAND FLASH<br> 步骤2：PC向A板发送AT+SPINAND_DEVICE_SPARE_WRITE_READ=0,0x0,0x100,2000000,0,1,0 |
| 备注 | test_mode为1时只测试读功能，为2时只测试写功能，为其它值时读写功能按先写入后读取的序列测试 |


#### AT+SPINAND_DEVICE_CROSS_CHECK
|     |     |
| --- | --- |
| 命令格式 | AT+SPINAND_DEVICE_CROSS_CHECK=qspi_idx,addr,frequence,gpio_idx,pin |
| 命令功能 | 测试nand flash在指定的范围内读写是否存在越界 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接NAND FLASH<br> 步骤2：PC向A板发送AT+SPINAND_DEVICE_CROSS_CHECK=0,0x0,2000000,0,1 |
| 备注 | addr只能设定block size对齐的地址 |


### 性能测试命令详解
#### AT+SPINAND_DEVICE_SPEED
|     |     |
| --- | --- |
| 命令格式 | AT+SPINAND_DEVICE_SPEED=qspi_idx,frequence,addr,repeat_times,gpio_idx,pin,timer_idx,cell_time |
| 命令功能 | 测试nand flash读、写、擦除速率 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接NAND FLASH<br> 步骤2：PC向A板发送AT+SPINAND_DEVICE_SPEED=0,2000000,0x0,1000,0,1,0,10 |
| 备注 | |


**参数解释**
| 参数名 | 释义 |
| --- | --- |
| qspi_idx | 连接NAND FLASH的QSPI控制器 |
| frequence | 设置QSPI的工作频率 |
| addr | 设置操作的连续两个块的块大小对齐的地址 |
| repeat_times | 测试重复的次数 |
| gpio_idx | 使用那个GPIO控制器去控制cs功能脚 |
| pin | 使用哪一个GPIO引脚作为CS功能脚 |
| timer_idx | 使用哪一个timer来计算时间 |
| cell_time | 设置timer统计时间的最小单元, 比如10表示设置timery以10ms为最小单元统计时间 |


### 稳定性测试命令详解
#### AT+STRESS_SPINAND
|     |     |
| --- | --- |
| 命令格式 | AT+STRESS_SPINAND=max_block_cnt,block_size,gpio_idx,pin |
| 命令功能 | 测试nand flash读、写、擦除的稳定性 |
| 可搭配的辅助测试命令 | None |
| 示例 | 步骤1：A板外接NAND FLASH<br> 步骤2：PC向A板发送AT+STRESS_SPINAND=128,128000,0,1 |
| 备注 | |


**参数解释**
| 参数名 | 释义 |
| --- | --- |
| max_block_cnt | 外接的NAND FLASH总共有多少个块 |
| block_size | 外接的NAND FLASH一个块的大小是多少字节 |
| gpio_idx | 使用那个GPIO控制器去控制cs功能脚 |
| pin | 使用哪一个GPIO引脚作为CS功能脚 |
