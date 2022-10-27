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

### 调试

```bash
riscv64-unknown-elf-gdb yoc.elf -x gdbinit
```

# 运行
烧录完成之后按下复位按键，之后输入cli命令运行dmeo。

## D1平台

```cli
1. pwm: appdemohal pwm 37 0 1000 0.1

   37：PWM使用的IO口，更详细的信息请查看soc.h，D1使用的IO口均参见soc.h

   0：PWM idx=0

   1000：周期1000us

   0.1：占空比为0.1

2. wdt: appdemohal wdt

3. gpio

   appdemohal gpio_out 34（34：使用IO34测试GPIO的输出）

   appdemohal gpio_in 34 1(使用IO34测试GPIO的输入，1表示上升沿中断)

   appdemohal gpio_in 34 2(使用IO34测试GPIO的输入，2表示下降沿中断)

4. uart：appdemohal uart 138 139(138 139: uart使用的IO口)

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

2. pwm: appdemohal pwm 0 0 1000 0.1

   0：PWM使用的IO口

   0：PWM idx=0

   1000：周期1000us

   0.1：占空比为0.1

3. wdt: appdemohal wdt

4. gpio

      appdemohal gpio_out 0(0：使用IO0测试GPIO的输出)

      appdemohal gpio_in 0 1(使用IO0测试GPIO的输入，1表示上升沿中断)

      appdemohal gpio_in 0 2(使用IO0测试GPIO的输入，2表示下降沿中断)

5. flash: appdemohal flash
```

## ch2601平台

```cli
1. pwm: appdemohal pwm 0 0 1000 0.1

      0：PWM使用的IO口

      0：PWM idx=0

      1000：周期1000us

      0.1：占空比为0.1

2. wdt: appdemohal wdt

3. gpio

   appdemohal gpio_out 0(0：使用IO0测试GPIO的输出)

   appdemohal gpio_in 0 1(使用IO0测试GPIO的输入，1表示上升沿中断)

   appdemohal gpio_in 0 2(使用IO0测试GPIO的输入，2表示下降沿中断)

4. flash：appdemohal flash

5. spi: appdemohal spi 0 1 4 5 2 3 6 7

   0 1 4 5：spi0对应的IO口

   2 3 6 7：spi1对应的IO口

   注意：spi dmeo使用的是spi0与spi1互发测试，需要把spi0与spi1进行对接连线

6. iic: appdemohal iic_slave 8 9  appdemohal iic_master 8 9

   8 9：iic0对应的IO口

   注意：iic demo使用的是两个板子的iic0互发测试，需要把两个板子的IO口进行连线

7. uart: appdemohal uart 27 28(27 28: uart使用的IO口)

   注意：uart demo使用的是板子的uart与PC机进行通信，uart需要连接串口线
```
