## BAUD_CALC

### 概要
该文档主要描述CSI2.0自动化测试框架下波特率检测（BAUD_CALC）模块的AT测试命令。 文档中所涉及的命令是用来测试BAUD_CALC模块的接口工作是否正常、接口返回值是否符合预期和该模块的特性功能是否正常。

### 测试原理
PC主机用USB转串口线连接测试板（以下简称A板）和辅助测试板（以下简称B板），作为PC同A板或B板数据通信，命令收发的通道；同时PC会使用USB线连接A板和B板的HAD调试器，作为程序下载的通道；A板发送出一个串口波形，B板使用一个带有捕获功能的PWM通道来检测波形的波特率。通过PC下发AT指令到A板测试程序，发送AT指令到B板运行辅助测试程序，当A板和B板都有测试成功的信息返回PC后，PC判定本次测试执行通过。
<div align="center"><img src="./picture/baud_calc.png" width="500" height="500" alt="BAUD_CALC 测试拓扑图" /></div>

### 测试流程
- 测试前应当连接好A板和B板之间的引脚
- 分别发送引脚复用的配置命令（PINMUX_CONFIG）到A板和B板，配置好GPIO的复用功能
- 发送BAUD_CALC的AT测试命令进行功能测试

### 命令列表
以下是BAUD_CALC模块对应的AT测试命令列表及其功能简要介绍。

| AT命令列表                       | 功能简介                                  |
| -------------------------------- | ------------------------------------------|
| AT+BAUDCALC_OUT_BAUDRATE 		   | 配置一个UART，使其输出波形  			   |
| AT+BAUDCALC_CALC_BAUDRATE        | PWM通道检测波形的波特率                   |
| AT+BAUDCALC_ADJUST_BAUDRATE      | PWM通道检测波形的波特率并调整到一个常用值 |


### 参数列表
以下是BAUD_CALC模块对应的AT测试命令共用参数的简要介绍。

| 参数名字      | 释义                                              | 取值范围                                                     |
| ------------- | ------------------------------------------------- | ------------------------------------------------------------ |
| uart_idx           | 表示使用那个UART的产生波形 | 根据SOC的UART编号定义                                         |
| baudrate | 配置UART的波特率 | 0x0 - 0xFFFFFFFF |
| stop_bits | 配置UART的停止位 | 0：1位停止位<br>1:  2位停止位<br>2:  1.5 位停止位 |
| data_bits | 配置UART的数据位 | 0：5位数据位<br>1：6位数据位<br>2：7位数据位<br>3：8位数据位<br>4：9位数据位 |
| parity | 配置UART的校验位 | 0：无校验<br>1：偶校验 <br>2：奇校验 |
| flowctrl | 配置UART的流控位 | 0：无<br>1：RTS<br>2：CTS<br>3：RTS & CTS |
| times | 配置UART输出特定波形的次数 | 0x0 - 0xFFFFFFFF |
| pwm_idx      | 表示那个PWM | 根据SOC的PWM编号定义                                  |
| pwm_ch     | 表示使用pwm_id的那个通道检测波形           | 根据SOC的PWM通道编号定义 |
| baudrate_max     | 设置检测出的波特率不应该大于该值        | 0x0 - 0xFFFFFFFF                         |
| baudrate_min      | 设置检测出的波特率不应该小于该值 | 0x0 - 0xFFFFFFFF                                             |
| baudrate_expe | 设置检测出的波特率的期望值                   | 0x0 - 0xFFFFFFFF |


### 命令详解
#### AT+BAUDCALC_OUT_BAUDRATE
| 命令格式 | AT+BAUDCALC_OUT_BAUDRATE=uart_idx,baudrate,stop_bits,<br>data_bits,parity,flowctrl,times |
|----------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 命令功能 | 配置UART,让其发送特定序列的波形times次 |
| 搭配的辅助测试命令 | AT+BAUDCALC_CALC_BAUDRATE，AT+BAUDCALC_ADJUST_BAUDRATE |
| 示例 | 把A板的PA28和B板的PA25这两个引脚使用杜邦线连接, 同时把A板和B板共地连接，然后按照下列步骤进行测试<br>步骤1:配置A板的PA28引脚作为UART的TX功能,向A板发送下列命令<br>    AT+PINMUX_CONFIG=0,28,1<br>步骤2:配置B板的PA25引脚作为PWM的通道功能,向B板发送下列命令<br>    AT+PINMUX_CONFIG=0,25,2<br>步骤3:向A板发送让UART产生波形的命令<br>    AT+BAUDCALC_OUT_BAUDRATE=1,115200,0,3,0,256,500<br>步骤4:向B板检测波特率的命令<br>    AT+BAUDCALC_CALC_BAUDRATE=0,1,113200,117200 |
| 备注 | 由于步骤3是持续输出500次特定序列的波形,所以步骤4中命令必须在步骤3描述的命令持续产生波形的时候才能检测到波特率 |


#### AT+BAUDCALC_CALC_BAUDRATE
| 命令格式 | AT+BAUDCLAC_CLAC_BAUDRATE=pwm_idx,pwm_ch,baudrate_min,baudrate_max |
|----------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 命令功能 | 使用pwm_idx这个PWM控制器的pwm_ch通道来检测波特率，同时指定检测到的波特率的范围应该在baudrate_min和baudrate_max之间 |
| 可搭配的辅助测试命令 | AT+BAUDCALC_OUT_BAUDRATE |
| 示例 | 参考AT+BAUDCALC_OUT_BAUDRATE示例 |


#### AT+BAUDCALC_ADJUST_BAUDRATE
| 命令格式 | AT+BAUDCALC_ADJUST_BAUDRATE=pwm_idx,pwm_ch,baudrate_expe |
|----------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 命令功能 | 使用pwm_idx这个PWM控制器的pwm_ch通道，并使用调整功能来检测波特率, 最后同baudrate_expe值作比较，相等则该测试通过 |
| 可搭配的辅助测试命令 | AT+BAUDCALC_OUT_BAUDRATE |
| 示例 | 把A板的PA28和B板的PA25这两个引脚使用杜邦线连接, 同时把A板和B板共地连接，然后按照下列步骤进行测试<br/>步骤1:配置A板的PA28引脚作为UART的TX功能,向A板发送下列命令<br/>    AT+PINMUX_CONFIG=0,28,1<br/>步骤2:配置B板的PA25引脚作为PWM的通道功能,向B板发送下列命令<br/>    AT+PINMUX_CONFIG=0,25,2<br/>步骤3:向A板发送让UART产生波形的命令<br/>    AT+BAUDCALC_OUT_BAUDRATE=1,115200,0,3,0,256,500<br/>步骤4:向B板检测波特率的命令<br/>    AT+BAUDCALC_ADJUST_BAUDRATE=0,1,115200 |
| 备注 | 由于步骤3是持续输出500次特定序列的波形,所以步骤4中命令必须在步骤3描述的命令持续产生波形的时候才能检测到波特率 |

