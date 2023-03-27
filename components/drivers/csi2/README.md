# 概述

驱动组件，以csi接口为基础实现对应HAL层的设备结构体功能，目前支持uart、spiflash、eflash、iic等，并提供设备注册接口。
**非csi接口直接实现，建议单独在componets下创建对应组件**

# 示例

```c
#include <devices/devicelist.h>

void board_yoc_init()
{
    rvm_uart_drv_register(0);
    rvm_iic_drv_register(0);
}
```
