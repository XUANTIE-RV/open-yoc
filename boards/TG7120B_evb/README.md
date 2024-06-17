## Change log

无

## 1. 概述

天猫精灵TG7120B板级配置组件。包括TG7120B开发板Pin脚定义，固件生成脚本, 分区定义和Flash烧录脚本。

## 2. 主要接口说明

### 2.1 板级初始化接口

函数名称：void board_init(void)
函数功能：完成开发板串口Pin脚复用、HCI初始化和看门狗功能初始化。

## 3. 配置

### 3.1. 配置文件

配置文件：package.yaml

| 宏定义名称                       | 功能说明                    | 备注                                   |
| -------------------------------| ---------------------------| --------------------------------------|
| BOARD_TG7120B_EVB              | 定义TG7120B开发板            |                                       |
| CONFIG_MAX_PARTITION_NUM       | 定义最大分区数                | 需要与configs/config.yaml中的定义一致    |
| CLI_CONFIG_TASK_PRIORITY       | 定义CLI任务优先级             |                                       |
| SYSINFO_PRODUCT_MODEL          | 定义产品模型                  | 默认为"TG7120B"                       |
| CONFIG_ARCH_INTERRUPTSTACK     | 定义中断栈大小                |                                       |
| CONFIG_INIT_TASK_STACK_SIZE    | 定义INIT TASK栈大小          |                                       |
| CONFIG_BT_RX_STACK_SIZE        | 定义RX TASK栈大小            |                                       |
| CONFIG_BT_ADV_STACK_SIZE       | 定义ADV TASK栈大小           |                                       |
| CONFIG_KERNEL_TIMER_STACK_SIZE | 定义TIMER TASK栈大小         |                                       |
| CLI_CONFIG_STACK_SIZE          | 定义CLI TASK栈大小           |                                       |
| CONFIG_LARGE_HEAP_SIZE         | 定义LARGE堆大小              |                                       |


### 3.2. 开发板管脚定义

头文件: board_config.h

```c
RGB三色灯管脚定义
#define PIN_LED_R  P23
#define PIN_LED_G  P31
#define PIN_LED_B  P32
```

```
串口管脚定义
#define CONSOLE_UART_IDX  0
#define CONSOLE_TXD       P9
#define CONSOLE_RXD       P10
```

## 4. 依赖资源

- chip_TG7120B
- drv_bt_TG7120B

## 5. 开发板烧录

### 5.1. 烧录工具

TG7120B烧录工具TG71XXProgrammer[下载](https://www.xrvm.cn/vendor/detail/download?spm=a2cl5.14300867.0.0.6bce180foUrUcQ&id=3898058978009550848&vendorId=3895463451199475712&module=1#sticky)，支持通过串口烧录镜像。

解压后，遵照TG71XX Programmer User's Guide.docx烧录。

**注意：**

> 1. `Erase Size`**下拉选项选择**`512K`**时**，**需要先点击**`Erase`**擦除Flash后，才能进行烧写。**
> 2. **Erase Size下拉选项选择HEXF时，只需点击Write按钮，此时烧录工具会根据镜像文件内容完成部分Flash的擦除和烧录动作。** 

- 

