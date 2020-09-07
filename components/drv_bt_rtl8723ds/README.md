# 概述

蓝牙驱动组件，实现HAL层的设备结构体功能。并提供设备注册接口。

# 示例

```c
#include <devices/rtl8723ds.h>
#include <pin.h>

void board_yoc_init()
{
    rtl8723ds_bt_config config = {
        .uart_id = BT_UART_IDX,
        .bt_dis_pin = BT_DIS_PIN,
    };

    bt_rtl8723ds_register(&config);
}
```
