# 概述
devices_demo是devices组件下驱动的使用示例。

# 使用
## CDK
在CDK的首页，通过搜索devices_demo，可以找到devices_demo，然后创建工程。

CDK的使用可以参考YoCBook [《CDK开发快速上手》](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/%E4%BD%BF%E7%94%A8CDK%E5%BC%80%E5%8F%91%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.html) 章节

## 通过命令行
需要先安装[yoctools](https://yoc.docs.t-head.cn/yocbook/Chapter2-%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B%E6%8C%87%E5%BC%95/YocTools.html)。

### DEMO获取

```bash
mkdir workspace
cd workspace
yoc init
yoc install devices_demo
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
1. clk: appdemohal clk

2. gpio

   appdemohal gpio_out 34（34：使用IO34测试GPIO的输出）

   appdemohal gpio_in 34 1(使用IO34测试GPIO的输入，1表示上升沿中断)

   appdemohal gpio_in 34 2(使用IO34测试GPIO的输入，2表示下降沿中断)

3. flash: appdemohal flash

4. wdt: appdemohal wdt

5. uart

   appdemohal uart 138 139（138 139：uart使用的IO口）

   appdemohal uart_multiple_task 138 139 138 139
```

## bl606p平台

```cli
1. adc

   appdemohal adc 5

   appdemohal adc_multiple_task 5

   注意：adc demo需要关注adc的转换结果

2. wdt: appdemohal wdt

3. rtc: appdemohal rtc

4. uart

   appdemohal uart 11 12（11 12：uart使用的IO口）

   appdemohal uart_multiple_task 11 12 11 12
```

## ch2601平台

```cli
1. wdt: appdemohal wdt

2. uart

   appdemohal uart 27 28（27 28：uart使用的IO口）

   appdemohal uart_multiple_task 27 28 27 28

3. flash: appdemohal flash

4. iic: appdemohal <iic|iic_task> <master|slave|mem>  8 9

   <iic|iic_task>: iic 单任务, iic_task 多任务

   <master|slave|mem>：master让设备作主去发送与接收数据; slave让设备作从去接收并发出数据; mem让设备与EEPROM同步读写数据

   8 9：iic对应的IO口

   如单任务时设备作从去传输数据的命令为：appdemohal iic slave 8 9

   注意：iic demo使用的是两个板子的iic0互发测试，需要把两个板子的IO口进行连线
```

## f133平台

```cli
1. clk: appdemohal clk

2. flash: appdemohal flash

3. wdt: appdemohal wdt
```
