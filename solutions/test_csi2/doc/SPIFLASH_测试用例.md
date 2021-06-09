## SPIFLASH

### 概要

该文档主要描述SPIFLASH模块的CSI2.0自动化测试AT命令。 文档中所涉及的这些命令是用来测试SPIFLASH模块的接口工作是否正常、数据传输是否正确、接口返回值是否符合预期。


### 测试原理

测试时，应给被测试板发送SPIFLASH测试命令进行相应的测试；测试程序会按照按照测试命令的配置进行相应的SPIFLASH测试， 若与预期一致，则相应的测试成功完成，否则测试失败。


### 测试流程

- 测试前应当准备好测试板（以下简称A板）
- 发送SPIFLASH测试命令进行测试 （测试命令功能请参照下文）


### AT命令列表 

以下是RTC模块对应的测试命令列表及其功能简要介绍。

| AT命令列表               | 功能                           |
| ------------------------ | ------------------------------ |
| AT+SPIFLASH_PROGRAM_READ | 测试SPIFLASH模块的读写功能     |
| AT+SPIFLASH_LOCK         | 测试SPIFLASH模块的写保护功能   |
| AT+SPIFLASH_PERFORMANCE  | 测试SPIFLASH模块的高速读写功能 |


### AT+SPIFLASH_PROGRAM_READ

| 命令格式 | AT+SPIFLASH_PROGRAM_READ=gpio_idx,gpio_pin_mask,idx,offset,size |
|------------------------------------------|--------------------------------------------------------------------------------------------------|
| 命令功能 | 测试SPIFLASH模块的读取功能<br>擦除spiflash内容，spiflash写入数据，spiflash读取数据，比对写入和读取的内容是否相同 |
| 命令参数 | gpio_idx：片选控制器号<br/>gpio_pin_mask：片选引脚<br/>idx：控制器号<br/>offset：偏移地址<br/>size：长度 |
| 示例 | 发送命令AT+SPIFLASH_PROGRAM_READ=0,1,0,0,256到A板，测试完成后， A板对应串口会打印OK；否则测试失败 |


### AT+SPIFLASH_LOCK

| 命令格式 | AT+SPIFLASH_LOCK=gpio_idx,gpio_pin_mask,idx,offset,size      |
| -------- | ------------------------------------------------------------ |
| 命令功能 | 测试SPIFLASH模块的写保护功能<br>擦除spiflash内容，打开spi写保护功能，spiflash写入数据，spiflash读取数据，比对写入和读取的内容，判断写入spiflash是否成功，关闭spi写保护功能，spiflash写入数据，spiflash读取数据，比对写入和读取的内容，判断写入spiflash是否成功 |
| 命令参数 | gpio_idx：片选控制器号<br/>gpio_pin_mask：片选引脚<br/>idx：控制器号<br>offset：偏移地址<br>size：长度 |
| 示例     | 发送命令AT+SPIFLASH_LOCK=0,1,0,0,256到A板，测试完成后， A板对应串口会打印OK；否则测试失败 |


### AT+SPIFLASH_PERFORMANCE

| 命令格式 | AT+SPIFLASH_PERFORMANCE=gpio_idx,gpio_pin_mask,idx,offset,size |
| -------- | ------------------------------------------------------------ |
| 命令功能 | 测试SPIFLASH模块的高速读取功能<br>擦除spiflash内容，定时器复位，spiflash开始写入数据，获取spiflash写入数据所用的时间，定时器复位，spiflash开始读取数据，获取spiflash读取数据所用的时间 |
| 命令参数 | gpio_idx：片选控制器号<br/>gpio_pin_mask：片选引脚<br/>idx：控制器号 |
| 示例     | 发送命令AT+SPIFLASH_PERFORMANCE=0,1,0到A板，测试完成后， A板对应串口会打印OK；否则测试失败 |
