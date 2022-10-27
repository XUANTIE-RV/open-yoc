## Change log

无

## 1. 概述

奉加微PHY6220板级配置组件。包括PHY6220开发板Pin脚定义，固件生成脚本, 分区定义和Flash烧录脚本。

## 2. 主要接口说明

### 2.1 板级初始化接口

函数名称：void board_init(void)
函数功能：完成开发板串口Pin脚复用、HCI初始化和看门狗功能初始化。

## 3. 配置

### 3.1. 配置文件

package.yaml

| 宏定义名称                | 功能说明                   | 备注                                  |
| ------------------------- | -------------------------- | ------------------------------------- |
| BOARD_PHY6220_EVB         | 定义PHY6220开发板          |                                       |
| CONFIG_UART_RECV_BUF_SIZE | 定义串口驱动ringbuffer大小 |                                       |
| CONFIG_MAX_PARTITION_NUM  | 定义分区个数               | 需要与configs/config.yaml中的定义一致 |
| CLI_CONFIG_TASK_PRIORITY  | 定义CLI任务优先级          |                                       |

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

- chip_phy6220
- drv_bt_phy6220

## 5. 开发板烧录

## 5.1. 烧录工具

PHY6220烧录工具PhyPlusKit[下载](https://occ.t-head.cn/vendor/detail/download?spm=a2cl5.14290816.0.0.d3ef82e980iPFE&id=3844141476569686016&vendorId=3712906428915875840&module=1#sticky)，支持通过串口烧录镜像。

- 打开烧写工具`PhyPlusKit.exe`

- 勾选`UART Setting`，选择开发板串口，串口配置为波特率：115200，停止位：1，校验：NO

- 点击`Connect`,连接串口

- 选择 `Flash_writer` 标签页

- 选择 `HEX` 烧入方式标签页

- 双击选择solutions\应用示例\generated\total_image.hexf

- 下方选择Single标签，TYPE选择MAC，VALUE填写MAC地址

- 将开发板拨码开关拨到 VDD 

- 按开发板上的 RESET 按键，重启开发板，串口打印`UART RX : cmd>>:`

- 点击 `Erase` 擦除

- 点击 `Write` 烧写

- LOG区域显示烧录过程

- 镜像烧录完成后，将开发板拨码开关拨到 GND，重启开发板即可看到打印的调试信息

  ![烧写工具配置](.\docs\phypluskit.png)

**注意：**

> 1. `Erase Size`**下拉选项选择**`512K`**时**，**需要先点击**`Erase`**擦除Flash后，才能进行烧写。**
> 2. **Erase Size下拉选项选择HEXF时，只需点击Write按钮，此时烧录工具会根据镜像文件内容完成部分Flash的擦除和烧录动作。** 

- 

