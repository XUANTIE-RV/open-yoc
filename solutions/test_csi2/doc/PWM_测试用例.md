## PWM

### 概要

该文档主要描述PWM模块的CSI1.0自动化测试AT命令。 文档中所涉及的这些命令是用来测试PWM的输出以及PWM输出的检测。



### 测试原理

测试该项目，只需要一块测试板通过串口工具和PC相连。测试时PC发送AT指令启动测试，打印OK说明测试成功



### 测试流程

- 测试前应当准备好测试板
- 发送AT+PINMUX_CONFIG指令将对应引脚复用成对应功能
- 发送PWM测试命令进行测试 （测试命令功能请参照下文）



### AT命令列表

以下是PWM模块对应的测试命令列表及其功能简要介绍。



| AT命令列表                                            | 功能                                                         |
| ----------------------------------------------------- | ------------------------------------------------------------ |
| AT+PWM_INTERFACE                                      | PWM模块接口测试。该命令将会测试PWM模块下所有的CSI 接口， 测试CSI接口时将会传入异常值、非法值，验证CSI接口返回值是否符合期望 |
| AT+PWM_OUTPUT=idx,channel,period,pulse_width,delay_ms | 测试PWM输出。用PWM模块输出一段时间PWM                        |
| AT+PWM_OUTPUT_CHECK=pin,period,pulse_width            | 测试PWM的周期和占空比。                                      |
| AT+PWM_CAPTURE=channel,capture_polarity,capture_count,delay_ms | 测试PWM捕获功能                        |
| AT+PWM_CAPTURE_INPUT=channel,capture_polarity,capture_count,delay_ms | 测试PWM的捕获输入功能                                      |


### 参数列表

以下是PWM模块对应的AT测试命令共用参数的简要介绍。

| 参数名字    | 释义                                  | 取值范围         |
| ----------- | ------------------------------------- | ---------------- |
| idx         | 表示使用哪一组外设，0表示pwm0         | 0x0 - 0xFFFFFFFF |
| channel     | 表示通道的序号,0表示ch0<br>           | 0x0 - 0xFFFFFFFF |
| period      | 表示PWM的周期，10表示周期为10ms       | 0x0 - 0xFFFFFFFF |
| pulse_width | 表示PWM的低电平宽度，5表示低电平为5ms | 0x0 - 0xFFFFFFFF |
| pin         | 表示引脚号，5表示PA5                  | 0~31             |
| delay_ms    | 表示输出PWM的时间，5000表示输出5000ms | 0x0 - 0xFFFFFFFF |



### 命令详解

#### AT+PWM_INTERFACE

| 命令格式             | AT+PWM_INTERFACE                                             |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | PWM模块接口测试。该命令将会测试PWM模块下所有的CSI 接口， 测试CSI接口时将会传入异常值、非法值，验证CSI接口返回值是否符合期望 |
| 可搭配的辅助测试命令 | 无                                                           |
| 示例                 | 步骤1:发送测试接口指令<br>AT+PWM_INTERFACE<br>测试完成后， 板子对应串口会打印OK；否则测试失败 |
| 备注                 |                                                              |



#### AT+PWM_OUTPUT

| 命令格式             | AT+PWM_OUTPUT=idx,channel,period,pulse_width,delay_ms        |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试PWM输出。用PWM模块输出一段时间PWM                        |
| 可搭配的辅助测试命令 | AT+PWM_OUTPUT_CHECK                                          |
| 示例                 | 步骤1:将A板的PA4与B板的PA5通过杜邦线连接<br>步骤2:B板发送AT+PWM_OUTPUT_CHECK=5,10,5<br/>步骤3:A板发送AT+PINMUX_CONFIG=4,1<br/>步骤4:A板发送AT+PWM_OUTPUT=0,0,10,5,5000测试完成后B板对应的串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |



#### AT+PWM_OUTPUT_CHECK

| 命令格式             | AT+PWM_OUTPUT_CHECK=pin,period,pulse_width                   |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试PWM的周期和占空比。                                      |
| 可搭配的辅助测试命令 | AT+PWM_OUTPUT                                                |
| 示例                 | 步骤1:将A板的PA4与B板的PA5通过杜邦线连接<br/>步骤2:B板发送AT+PWM_OUTPUT_CHECK=5,10,5<br/>步骤3:A板发送AT+PINMUX_CONFIG=4,1<br/>步骤4:A板发送AT+PWM_OUTPUT=0,0,10,5,5000测试完成后B板对应的串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |


#### AT+PWM_CAPTURE

| 命令格式             | AT+PWM_CAPTURE=channel,capture_polarity,capture_count,delay_ms        |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试PWM的捕获功能                        |
| 可搭配的辅助测试命令 | AT+PWM_CAPTURE_INPUT                                         |
| 示例                 | 步骤1:将A板的PA29与B板的PB20通过杜邦线连接<br/>向A板发送命令AT+PINMUX_CONFIG=0,29,2,向B板发送命令AT+PINMUX_CONFIG=0,52,0<br/>步骤2:步骤3:B板发送AT+PWM_CAPTURE_INPUT=1,0,1,1000,A板发送AT+PWM_CAPTURE=0,0,1,1000<br/>步骤4:测试完成后B板对应的串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |



#### AT+PWM_CAPTURE_INPUT

| 命令格式             | AT+PWM_CAPTURE_INPUT=channel,capture_polarity,capture_count,delay_ms     |
| -------------------- | ------------------------------------------------------------ |
| 命令功能             | 测试PWM的捕获输入功能。                                      |
| 可搭配的辅助测试命令 | AT+PWM_CAPTURE                                                |
| 示例                 | 步骤1:将A板的PA29与B板的PB20通过杜邦线连接<br/>向A板发送命令AT+PINMUX_CONFIG=0,29,2,向B板发送命令AT+PINMUX_CONFIG=0,52,0<br/>步骤2:步骤3:B板发送AT+PWM_CAPTURE_INPUT=1,0,1,1000,A板发送AT+PWM_CAPTURE=0,0,1,1000<br/>步骤4:测试完成后B板对应的串口会打印OK；否则测试失败<br/> |
| 备注                 |                                                              |
