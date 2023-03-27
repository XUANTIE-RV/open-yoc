# 概述
aoshal_demo是aoshal组件下驱动的使用示例。

# 使用
## CDK
在CDK的首页，通过搜索aoshal_demo，可以找到aoshal_demo，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html) 章节

## 通过命令行
需要先安装[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install aoshal_demo
```

### 编译&烧录

注意：
    烧录时请注意当前目录下的`gdbinitflash`文件中的`target remote localhost:1025`内容需要改成用户实际连接时的T-HeadDebugServer中显示的对应的内容。

#### D1平台

1. 编译

```bash
make clean
make SDK=sdk_chip_d1
```

2. 烧写

```bash
make flashall SDK=sdk_chip_d1
```

#### bl606P平台

1. 编译

```bash
make clean
make SDK=sdk_chip_bl606p_e907
```

2. 烧写

```bash
make flashall SDK=sdk_chip_bl606p_e907
```

#### ch2601平台

1. 编译

```bash
make clean
make SDK=sdk_chip_ch2601
```

2. 烧写

```bash
make flashall SDK=sdk_chip_ch2601
```

#### f133平台

1. 编译

```bash
make clean
make SDK=sdk_chip_f133
```

2. 烧写

```bash
make flashall SDK=sdk_chip_f133
```

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

# 运行
烧录完成之后按下复位按键，之后输入cli命令运行dmeo。

## D1平台

```cli
1. pwm: appdemohal pwm 37 0 1000 0.1 10000 0.5

   37：PWM使用的IO口，更详细的信息请查看soc.h，D1使用的IO口均参见soc.h

   0：PWM idx=0

   1000：周期1000us

   0.1：占空比为0.1
     
   10000：freq_chg = 10000，变化后的频率
   
   0.5：duty_cycle_chg = 0.5，变化后的占空比 

2. wdt: appdemohal wdt

3. gpio

   appdemohal gpio_out 34（34：使用IO34测试GPIO的输出）

   appdemohal gpio_in 34 1(使用IO34测试GPIO的输入，1表示上升沿中断)

   appdemohal gpio_in 34 2(使用IO34测试GPIO的输入，2表示下降沿中断)

4. uart

   appdemohal uart 138 139(138 139: uart使用的IO口)

   appdemohal uart_multiple_task 138 139 138 139

   注意：uart demo使用的是板子的uart与PC机进行通信，uart需要连接串口线

5. flash: appdemohal flash
```

## bl606p平台

```cli
1. adc

   appdemohal adc_signal 5

   appdemohal adc_multiple 5

   appdemohal adc_multiple_task_multiple_data 5 5

   appdemohal adc_multiple_task_signal_data 5 5

   注意：adc demo需要关注adc的转换结果

2. pwm: appdemohal pwm 0 0 1000 0.1 10000 0.5

   0：PWM使用的IO口

   0：PWM idx=0

   1000：freq = 1000

   0.1：duty_cycle = 0.1
   
   10000：freq_chg = 10000，变化后的频率
   
   0.5：duty_cycle_chg = 0.5，变化后的占空比

3. wdt: appdemohal wdt

4. gpio

   appdemohal gpio_out 0(0：使用IO0测试GPIO的输出)

   appdemohal gpio_in 0 1(使用IO0测试GPIO的输入，1表示上升沿中断)

   appdemohal gpio_in 0 2(使用IO0测试GPIO的输入，2表示下降沿中断)

5. flash: appdemohal flash

6. spi: appdemohal spi <master|slave> <send|recv|send_recv> 25 26 27 28

   <master|slave>：master让设备作主, slave让设备作从

   <send|recv|send_recv>：send发送数据, recv接收数据,send_recv发送并接收数据

   25 26 27 28：spi0对应的IO口

   如设备作 spi 通信中的主发送并接收数据的命令为：appdemohal spi master send_recv 25 26 27 28

   注意：由于 BL606p 只有一个spi, 所以可使用两块 BL606p 开发板对接连线测试此 spi 功能

7. iic: appdemohal <iic|iic_task> <master|slave|mem>  0 1

   <iic|iic_task>: iic 单任务, iic_task 多任务

   <master|slave|mem>：master让设备作主去发送与接收数据, slave让设备作从去接收并发出数据, mem让设备与EEPROM同步读写数据

   0 1：iic对应的IO口

   如单任务时，设备作从去传输数据的命令为：appdemohal iic master 0 1

   注意：由于 BL606p 的iic只能作主，不能作从，所以需要其它开发板作从配合验证（如 RVB2601 开发板）

8. rtc: appdemohal rtc

9. uart

   appdemohal uart 11 12（11 12：uart使用的IO口）

   appdemohal uart_multiple_task 11 12 11 12
```

## ch2601平台

```cli
1. pwm: appdemohal pwm 0 0 1000 0.1 1000 0.9

   0：PWM使用的IO口

   0：PWM idx=0

   1000：周期1000us

   0.1：占空比为0.1
         
   10000：freq_chg = 10000，变化后的频率
   
   0.5：duty_cycle_chg = 0.5，变化后的占空比

2. wdt: appdemohal wdt

3. gpio

   appdemohal gpio_out 0(0：使用IO0测试GPIO的输出)

   appdemohal gpio_in 0 1(使用IO0测试GPIO的输入，1表示上升沿中断)

   appdemohal gpio_in 0 2(使用IO0测试GPIO的输入，2表示下降沿中断)

4. flash：appdemohal flash

5. spi: appdemohal spi <master|slave> <send|recv|send_recv> 0 1 4 5

   <master|slave>：master让设备作主, slave让设备作从

   <send|recv|send_recv>：send发送数据, recv接收数据,send_recv发送并接收数据

   0 1 4 5：spi对应的IO口

   如设备作 spi 通信中的主发送并接收数据的命令为：appdemohal spi master send_recv 0 1 4 5

   由于 ch2601 有两个spi端口, 故还支持使用以下命令测试spi的异步通信链路：appdemohal spi 0 1 4 5 2 3 6 7

   0 1 4 5：spi0对应的IO口

   2 3 6 7：spi1对应的IO口

   注意：spi dmeo使用的是spi0与spi1互发测试，需要把spi0与spi1进行对接连线

6. iic: appdemohal <iic|iic_task> <master|slave|mem>  8 9

   <iic|iic_task>: iic 单任务, iic_task 多任务

   <master|slave|mem>：master让设备作主去发送与接收数据; slave让设备作从去接收并发出数据; mem让设备与EEPROM同步读写数据

   8 9：iic对应的IO口

   如单任务时设备作从去传输数据的命令为：appdemohal iic slave 8 9

   注意：iic demo使用的是两个板子的iic0互发测试，需要把两个板子的IO口进行连线

7. uart

   appdemohal uart 27 28(27 28: uart使用的IO口)

   appdemohal uart_multiple_task 27 28 27 28

   注意：uart demo使用的是板子的uart与PC机进行通信，uart需要连接串口线
```

## f133平台

```cli
1. wdt: appdemohal wdt

2. flash：appdemohal flash

其余IP，f133由于没有引脚外拉均不支持
```

